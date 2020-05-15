type node = {
  uid: Node.uid,
  flow: option(list(Node.uid)),
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
  let globalTransform =
    globalTransform
    ->Transform.compose(transform)
    /* TODO: is this necessary? */
    ->Transform.translate(-. bbox->Rectangle.x1, -. bbox->Rectangle.y1);
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

let rec findUIDAndPath = (uid, {uid: candidate, nodes} as n) =>
  if (uid == candidate) {
    Some(n);
  } else {
    List.fold_left(
      (on, n) =>
        switch (on) {
        | None => findUIDAndPath(uid, n)
        | Some(n) => Some(n)
        },
      None,
      nodes,
    );
  };

let rec renderTransition =
        (
          ~prevState: TransitionNode.state,
          ~currState: TransitionNode.state,
          nextNode: node,
          {nodes, flow, links, transform, globalTransform, bbox, render: nodeRender},
        ) => {
  let nodes = List.map(renderTransition(~prevState, ~currState, nextNode), nodes);
  /* 1. look for a node in nextNode matching flow. (just first flow value for now) */
  switch (flow) {
  /* render normally */
  | None => nodeRender(nodes, bbox, links) |> svgTransform(transform, bbox)
  | Some(flow) =>
    let first_flow = flow->List.nth_opt(0);
    switch (first_flow) {
    /* node gets deleted. just render normally for now. */
    | None => nodeRender(nodes, bbox, links) |> svgTransform(transform, bbox)
    | Some(first_flow) =>
      switch (findUIDAndPath(first_flow, nextNode)) {
      | None => failwith("couldn't find flow uid: " ++ first_flow)
      | Some(next) =>
        /* 2. apply svgTransformTransition from this node to new node */
        /* nodeRender(nodes, bbox, links) |> svgTransformTransition(transform, bbox, (), ()); */
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
      }
    };
  };
};