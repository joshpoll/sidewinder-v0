type node = {
  uid: Node.uid,
  flow: Flow.t,
  nodes: list(node),
  links: list(React.element),
  transform: Node.transform,
  /* transform relative to global frame. used for animation */
  globalTransform: Node.transform,
  bbox: Node.bbox,
  render: (list(React.element), Node.bbox, list(React.element)) => React.element,
};

let rec computeGlobalTransformAux =
        (globalTransform, RenderLinks.{uid, flow, nodes, links, transform, bbox, render}) => {
  let globalTransform = globalTransform->Transform.compose(transform);
  let nodes = List.map(computeGlobalTransformAux(globalTransform), nodes);
  {uid, flow, nodes, links, transform, globalTransform, bbox, render};
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

let findNodeByUIDExn = (uid, n) =>
  switch (findNodeByUID(uid, n)) {
  | None => failwith("couldn't find flow uid: " ++ uid)
  | Some(n) => n
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

let lowerSingleFlow = (node: node, uid: Node.uid, nextNode: node, nodes: list(node)): list(node) => {
  /* find node corresponding to given uid */
  let next = findNodeByUIDExn(uid, nextNode);
  /* match up given nodes and next's nodes */
  Js.log2("attempting to match nodes against nextNode:", nextNode);
  Js.log2("attempting to match nodes against next:", next);
  Js.log2("attempting to match nodes against node:", node);
  Js.log3("attempting to match nodes:", nodes |> Array.of_list, next.nodes |> Array.of_list);
  let nodePairs = List.combine(nodes, next.nodes);
  /* add next's nodes as a targets for given nodes */
  let computeFlow = (n, next) =>
    switch (n.flow) {
    | Inherit => Flow.Flow([next.uid])
    | flow => flow
    };
  List.map(
    ((n, next)) =>
      {...n, flow: /* Flow.merge(Flow([next.uid]), n.flow) */ computeFlow(n, next)},
    nodePairs,
  );
};

/* lowerFlow lowers the IDs in the given flow down to the next level of nodes */
let rec lowerFlow = (nextNode: node, node: node): node => {
  Js.log2("lowerFlow on node:", node);
  /* propagate our current flow down to children */
  let nodes =
    switch (node.flow) {
    | Inherit =>
      failwith(
        "lowerFlow encountered an Inherit flow. Inherit flows should be normal flows by now.",
      )
    | Untracked => node.nodes
    | Flow(flow) =>
      List.fold_left((ns, uid) => lowerSingleFlow(node, uid, nextNode, ns), node.nodes, flow)
    };
  /* visit children */
  {...node, nodes: List.map(lowerFlow(nextNode), nodes)};
};

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

let rec renderTransitionAux =
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
  | Untracked
  | Inherit => nodeRender(nodes, bbox, links) |> svgTransform(transform, bbox)
  /* node gets deleted */
  | Flow([]) => <DeleteNode renderedElem={nodeRender(nodes, bbox, links)} prevState currState />
  | Flow(flow) =>
    <g>
      {List.map(processSingleTransition(prevState, currState, nextNode, nodes, n), flow)
       |> Array.of_list
       |> React.array}
    </g>
  };
};

let renderTransition =
    (
      ~lowerFlow as lf: bool,
      ~prevState: TransitionNode.state,
      ~currState: TransitionNode.state,
      nextNode: node,
      n: node,
    ) => {
  let n =
    if (lf) {
      lowerFlow(nextNode, n);
    } else {
      n;
    };
  renderTransitionAux(~prevState, ~currState, nextNode, n);
};