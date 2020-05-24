/* TODO: need to counter parent transformations */

type node = {
  uid: Node.uid,
  // flow: Flow.t,
  flowTag: FlowTag.t,
  /* initialized by propagating flowTags, then finalized by lowering */
  // newFlow: Flow.t,
  nodes: list(node),
  links: list(React.element),
  transform: Node.transform,
  /* transform relative to global frame. used for animation */
  globalTransform: Node.transform,
  bbox: Node.bbox,
  render: (list(React.element), Node.bbox, list(React.element)) => React.element,
};

let rec computeGlobalTransformAux =
        (
          globalTransform,
          RenderLinks.{uid, /* flow, */ flowTag, nodes, links, transform, bbox, render},
        ) => {
  let globalTransform = globalTransform->Transform.compose(transform);
  let nodes = List.map(computeGlobalTransformAux(globalTransform), nodes);
  {
    uid /* flow, */,
    /* newFlow: None, */ flowTag,
    nodes,
    links,
    transform,
    globalTransform,
    bbox,
    render,
  };
};

let computeGlobalTransform = computeGlobalTransformAux(Transform.init);

/* TODO: factor out this computation so it can be shared by Render and TransitionNode */

let computeSVGTransform =
    ({translate: {x: tx, y: ty}, scale: {x: sx, y: sy}}: Node.transform, bbox) => {
  /* https://css-tricks.com/transforms-on-svg-elements/ */
  let scale =
    "translate("
    ++ Js.Float.toString(bbox->Rectangle.x1 +. bbox->Rectangle.width /. 2.)
    ++ ", "
    ++ Js.Float.toString(bbox->Rectangle.y1 +. bbox->Rectangle.height /. 2.)
    ++ ")";
  let scale = scale ++ " " ++ {j|scale($sx, $sy)|j};
  let scale =
    scale
    ++ " "
    ++ "translate("
    ++ Js.Float.toString(-. (bbox->Rectangle.x1 +. bbox->Rectangle.width /. 2.))
    ++ ", "
    ++ Js.Float.toString(-. (bbox->Rectangle.y1 +. bbox->Rectangle.height /. 2.))
    ++ ")";

  let translate = {j|translate($tx, $ty)|j};

  scale ++ " " ++ translate;
};

let svgTransform = (transform, bbox, r) => {
  let transform = computeSVGTransform(transform, bbox);
  <g transform> r </g>;
};

let rec render = (RenderLinks.{nodes, links, transform, bbox, render: nodeRender}) => {
  let nodes = List.map(render, nodes);
  nodeRender(nodes, bbox, links) |> svgTransform(transform, bbox);
};

/* TODO: use react-spring */
let svgTransformTransition = (transform, bbox, nextTransform, nextBBox, r) => {
  let transform = computeSVGTransform(transform, bbox);
  <g transform> r </g>;
};

let rec findNodeByUID = (uid, {uid: candidate, nodes} as n) =>
  if (uid == candidate) {
    Some(n);
  } else {
    List.fold_left(
      (on, n) =>
        switch (on) {
        | None => findNodeByUID(uid, n)
        | Some(n) => Some(n)
        },
      None,
      nodes,
    );
  };

let rec findNodeByTag = (ft, {flowTag, nodes} as n) =>
  switch (flowTag) {
  | Some({uid}) when ft == uid => Some(n)
  | _ =>
    List.fold_left(
      (on, n) =>
        switch (on) {
        | None => findNodeByTag(ft, n)
        | Some(n) => Some(n)
        },
      None,
      nodes,
    )
  };

let findNodeByUIDExn = (uid, n) =>
  switch (findNodeByUID(uid, n)) {
  | None => failwith("couldn't find flow uid: " ++ uid)
  | Some(n) => n
  };

let findNodeByTagExn = (ft, n) =>
  switch (findNodeByTag(ft, n)) {
  | None => failwith("couldn't find flowTag: " ++ ft)
  | Some(n) => n
  };

let rec findNodeByPathExn = (path, {nodes} as n) =>
  switch (path) {
  | [] => n
  | [i, ...path] => findNodeByPathExn(path, List.nth(nodes, i))
  };

let processSingleTransition =
    (
      prevState,
      currState,
      nextNode,
      nodes,
      {links, transform, globalTransform, bbox, render: nodeRender},
      uid,
    ) => {
  let next = findNodeByUIDExn(uid, nextNode);
  let transformDelta =
    Transform.compose(next.globalTransform, Transform.invert(globalTransform));
  /* Js.log2("next.transform", next.transform);
     Js.log2("transformDelta", transformDelta);
     Js.log2("transform", transform);
     Js.log2("nextTransform", Transform.compose(transform, transformDelta)); */
  <TransitionNode
    bbox
    renderedElem={nodeRender(nodes, bbox, links)}
    transform
    nextTransform={Transform.compose(transform, transformDelta)}
    prevState
    currState
  />;
};

let processSingleTransitionFlow =
    (
      prevState,
      currState,
      nextGlobalTransform,
      nodes,
      {links, transform, globalTransform, bbox, render: nodeRender},
    ) => {
  let transformDelta = Transform.compose(nextGlobalTransform, Transform.invert(globalTransform));
  /* Js.log2("next.transform", next.transform);
     Js.log2("transformDelta", transformDelta);
     Js.log2("transform", transform);
     Js.log2("nextTransform", Transform.compose(transform, transformDelta)); */
  <TransitionNode
    bbox
    renderedElem={nodeRender(nodes, bbox, links)}
    transform
    nextTransform={Transform.compose(transform, transformDelta)}
    prevState
    currState
  />;
};

/* let lowerSingleFlow =
       (node: node, uid: Node.uid, flowTag: option(Node.uid), nextNode: node, nodes: list(node))
       : list(node) => {
     switch (flowTag) {
     | None => nodes
     | Some(flowTag) =>
       /* find node corresponding to given uid */
       let next = findNodeByUIDExn(uid, nextNode);
       /* match up given nodes and next's nodes */
       Js.log2("attempting to match nodes against nextNode:", nextNode);
       Js.log2("attempting to match nodes against next:", next);
       Js.log2("attempting to match nodes against node:", node);
       Js.log3("attempting to match nodes:", nodes |> Array.of_list, next.nodes |> Array.of_list);
       let nodePairs = List.combine(nodes, next.nodes);
       /* add next's nodes as a targets for given nodes */
       List.map(
         ((n, next)) =>
           switch (n.flowTag) {
           | nFT /*  when nFT == flowTag */ => {
               ...n,
               flow: Flow.merge(Some({pat: [next.uid], ext: []}), n.flow),
             }
           | _ => n
           },
         nodePairs,
       );
     };
   }; */

/* let lowerFlowTags = (i, node, ftNodes) => {
     let ftNodes = List.filter(((ft, _)) => List.mem(ft, node.flowTags), ftNodes);
     List.fold_left(
       ((node, l), (ft, n)) => {
         let nChild = List.nth(n.nodes, i);
         (
           n /* {...n, flow: Flow.merge(Some({pat: [nChild.uid], ext: []}), n.flow)} */,
           [(ft, nChild), ...l],
         );
       },
       (node, []),
       ftNodes,
     );
   }; */

/* let rec reconstructConfigStructureAux =
           (ftNodes: list((Node.uid, node)), nextNode: node, node: node): node => {
     /* for each ft-pair in flowTags, look at children of node. if they share that tag, add ft-node as
        a destination pattern*/

     /* filter out the flowTags that don't appear on this node */
     let ftNodes = List.filter(((ft, _)) => List.mem(ft, node.flowTags), ftNodes);
     /* forall tags that are on this node, but not int filterTags, find the corresponding node in
        nextNode */
     let ftNodes =
       List.map(
         ft =>
           switch (List.assoc_opt(ft, ftNodes)) {
           | None => (ft, []) /* TODO: this should actually track the destinations of the flows!!!!! */
           | Some(n) => (ft, n)
           },
         node.flowTags,
       );
     let nodesAndFTs = List.mapi((i, n) => lowerFlowTags(i, n, ftNodes), node.nodes);
     {
       ...node,
       nodes:
         List.map(((n, fts)) => reconstructConfigStructureAux(fts, nextNode, n), nodesAndFTs),
     };
   };

   let reconstructConfigStructure = reconstructConfigStructureAux([]); */

module MS = Belt.Map.String;

/* TODO: I'm not sure lowering is actually necessary!!!! */
/* /* step 1: lower flowTags, building up paths as we go */
   /* TODO: currently assumes existing Leaf nodes have precomputed paths, but could be fancier than
      that eventually by computing on the fly */
   let rec lowerFlowTag = (ft: FlowTag.t, {flowTag, nodes} as node) => {
     let (flowTag, nodes) =
       switch (flowTag) {
       /* if we are a None node, change flowTag to the input flowTag */
       | None => (ft, List.map(lowerFlowTag(flowTag), nodes))
       /* if we are a Leaf or Dummy node, propagate flow to children  */
       | Some({flowNodeType, uid, rootPath}) => (
           flowTag,
           List.mapi(
             (i, n) => lowerFlowTag(Some({flowNodeType, uid, rootPath: [i, ...rootPath]}), n),
             nodes,
           ),
         )
       };
     {...node, flowTag, nodes};
   }; */

/* step 2: add flow */

type flowEntries =
  | FTUIDs(list(FlowTag.uid))
  | Nodes(list(node));

let rec renderFlowAux =
        (
          prevState,
          currState,
          flow: MS.t(flowEntries),
          nextNode,
          {flowTag, nodes, links, transform, bbox, render: nodeRender} as node,
        )
        : (MS.t(flowEntries), React.element) => {
  /* update flow map */
  let flow =
    switch (flowTag) {
    /* if this is the first time we're seeing this flowTag */
    | Some({uid, rootPath: []}) =>
      MS.update(flow, uid, optFlowEntries =>
        switch (optFlowEntries) {
        /* There's no entry for this uid and thus no flow */
        | None => None
        | Some(FTUIDs(fts)) => Some(Nodes(List.map(findNodeByTagExn(_, nextNode), fts)))
        | Some(Nodes(_)) => failwith("we didn't expect nodes in the map yet! Looked up " ++ uid)
        }
      )
    | _ => flow
    };

  let (flow, nodes) =
    List.fold_right(
      (n, (flow, nodes)) => {
        let (flow, n) = renderFlowAux(prevState, currState, flow, nextNode, n);
        (flow, [n, ...nodes]);
      },
      nodes,
      (flow, []),
    );

  let rendered =
    switch (flowTag) {
    | None
    | Some({flowNodeType: Dummy}) =>
      nodeRender(nodes, bbox, links) |> svgTransform(transform, bbox)
    | Some({flowNodeType: Leaf, uid, rootPath}) =>
      /* lookup flowTag to get Nodes. use path to find corresponding child in each node to use for
         flow */
      switch (MS.get(flow, uid)) {
      | None => nodeRender(nodes, bbox, links) |> svgTransform(transform, bbox)
      | Some(FTUIDs(_)) => failwith("we didn't expect flowTag uids now!")
      /* deletion */
      | Some(Nodes([])) =>
        <DeleteNode renderedElem={nodeRender(nodes, bbox, links)} prevState currState />
      /* movement */
      | Some(Nodes(ns)) =>
        /* Js.log2("processing flowTag:", flowTag);
           Js.log2("processing flowTag ns:", ns |> Array.of_list);
           Js.log2("processing flowTag rootPath:", rootPath |> Array.of_list); */
        let nextNodeTransforms =
          List.map(n => findNodeByPathExn(rootPath, n).globalTransform, ns);
        /* Js.log2("processing flowTag node.globalTransform:", node.globalTransform);
           Js.log2("processing flowTag nextNodeTransforms:", nextNodeTransforms |> Array.of_list);
           Js.log2(
             "processing flowTag nextNodes:",
             List.map(n => findNodeByPathExn(rootPath, n), ns) |> Array.of_list,
           ); */
        <g>
          {List.map(
             processSingleTransitionFlow(prevState, currState, _, nodes, node),
             nextNodeTransforms,
           )
           |> Array.of_list
           |> React.array}
        </g>;
      }
    };
  (flow, rendered);
};

let renderFlow = (prevState, currState, flow, nextNode, node) => {
  let (_, rendered) = renderFlowAux(prevState, currState, flow, nextNode, node);
  rendered;
};

/*
 let rec initFlow = (flow: MS.t(Flow.flow), {flowTags} as node) => {
   let newFlow =
     switch (flowTags) {
     | None => None
     | Some(fts) =>
       List.map(
         ft =>
           switch (ft) {
           | FlowTag.Dummy(uid)
           | Leaf(uid) => MS.get(flow, uid)
           },
         fts,
       )
       |> List.filter(Belt.Option.isSome)
       |> List.map(Belt.Option.getExn)
       |> (x => Some(x))
     };
   {...node, newFlow};
 };

 /* lowerFlow lowers the IDs in the given flow down to the next level of nodes */
 let rec lowerFlow = (nextNode: node, node: node): node => {
   Js.log2("lowerFlow on node:", node);
   /* propagate our current flow down to children */
   let nodes =
     switch (node.flow) {
     | None => node.nodes
     | Some({pat, ext: _}) =>
       List.fold_left(
         (ns, uid) => lowerSingleFlow(node, uid, node.flowTag, nextNode, ns),
         node.nodes,
         pat,
       )
     };
   /* visit children */
   {...node, nodes: List.map(lowerFlow(nextNode), nodes)};
 }; */

/* /* TODO: write a new function that propagates flow downwards through children. This ensures that
      movement in children relative to parent will be captured e.g. state 2 in `id id`. */
   let rec propagateFlow = (accFlow, nextNode: node, {nodes, flow, links} as n) => {
     let flow = mergeFlow(accFlow, flow);
     let loweredFlows = lowerFlow(nextNode, n);
     switch (flow) {
     /* no flow at this node, visit children */
     | None => {...n, nodes: List.map(propagateFlow(flow, nextNode), nodes)}
     /* flow is deleting, children will be deleted, too so don't need to propagate */
     | Some([]) => n
     /* accumulate flow */
     | Some(_) => {...n, flow, nodes: List.map(propagateFlow(flow, nextNode), nodes)}
     };
   }; */

/* let rec renderTransitionAux =
           (
             ~prevState: TransitionNode.state,
             ~currState: TransitionNode.state,
             nextNode: node,
             {nodes, flow, links, transform, bbox, render: nodeRender} as n: node,
           ) => {
     let nodes = List.map(renderTransitionAux(~prevState, ~currState, nextNode), nodes);
     /* 1. look for a node in nextNode matching flow. (just first flow value for now) */
     switch (flow) {
     /* render normally */
     | None => nodeRender(nodes, bbox, links) |> svgTransform(transform, bbox)
     /* node gets deleted */
     | Some({pat: [], ext: _}) =>
       <DeleteNode renderedElem={nodeRender(nodes, bbox, links)} prevState currState />
     | Some({pat, ext: _}) =>
       <g>
         {List.map(processSingleTransition(prevState, currState, nextNode, nodes, n), pat)
          |> Array.of_list
          |> React.array}
       </g>
     };
   }; */

let renderTransition =
    (
      ~lowerFlow as lf: bool,
      ~prevState: TransitionNode.state,
      ~currState: TransitionNode.state,
      flow: Flow.t,
      nextNode: node,
      n: node,
    ) => {
  /*  let n =
      if (lf) {
        lowerFlow(nextNode, n);
      } else {
        n;
      }; */
  /* renderTransitionAux(
       ~prevState,
       ~currState,
       nextNode,
       n,
     ); */
  // Js.log2("renderTransition - nextNode:", nextNode);
  // Js.log2("renderTransition - n:", n);
  renderFlow(
    prevState,
    currState,
    MS.map(flow, fts => FTUIDs(fts)),
    nextNode,
    n,
  );
};