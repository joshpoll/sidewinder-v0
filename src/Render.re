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

let rec findUID = (uid, RenderLinks.{uid: candidate, nodes} as n) =>
  if (uid == candidate) {
    Some(n);
  } else {
    List.fold_left(
      (on, n) =>
        switch (on) {
        | None => findUID(uid, n)
        | Some(n) => Some(n)
        },
      None,
      nodes,
    );
  };

let rec renderTransition =
        (nextNode, RenderLinks.{nodes, flow, links, transform, bbox, render: nodeRender}) => {
  let nodes = List.map(renderTransition(nextNode), nodes);
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
      let next = findUID(first_flow, nextNode) |> Belt.Option.getExn;
      /* 2. apply svgTransformTransition from this node to new node */
      /* nodeRender(nodes, bbox, links) |> svgTransformTransition(transform, bbox, (), ()); */

      <TransitionNode
        bbox
        renderedElem={nodeRender(nodes, bbox, links)}
        transform
        nextTransform={next.transform}
      />;
    };
  };
};