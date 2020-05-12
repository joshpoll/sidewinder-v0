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

let computeSVGTransformFlattened = (bbox, tx, ty, sx, sy) =>
  computeSVGTransform({
                        translate: {
                          x: tx,
                          y: ty,
                        },
                        scale: {
                          x: sx,
                          y: sy,
                        },
                      }, bbox);

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

module SpringHook =
  Spring.MakeSpring({
    type t = (float, float, float, float);
    type interpolate = (float, float, float, float) => string;
  });

module G = {
  type reactElement;
  [@bs.module "react-spring"] [@bs.scope "animated"] external externalG_: reactElement = "g";
  [@bs.module "react"]
  external externalCreateElement: (reactElement, Js.t({..}), React.element) => React.element =
    "createElement";
  type spreadProps;
  [@bs.deriving abstract]
  type jsProps = {
    [@bs.optional]
    className: string,
    [@bs.optional]
    onMouseMove: ReactEvent.Mouse.t => unit,
    [@bs.optional]
    onClick: ReactEvent.Mouse.t => unit,
    [@bs.optional]
    onMouseDown: ReactEvent.Mouse.t => unit,
    [@bs.optional]
    onMouseLeave: unit => unit,
    [@bs.optional]
    transform: string,
    [@bs.optional]
    style: ReactDOMRe.Style.t,
  };

  [@react.component]
  let make =
      (
        ~className=?,
        ~spreadProps=?,
        ~onMouseMove=?,
        ~onMouseLeave=?,
        ~onClick=?,
        ~onMouseDown=?,
        ~transform=?,
        ~style,
        ~children=React.null,
        (),
      ) => {
    let props =
      Js.Obj.assign(
        switch (spreadProps) {
        | None => Js.Obj.empty()
        | Some(s) => (Obj.magic(s))(.)
        },
        Obj.magic(
          jsProps(
            ~className?,
            ~onMouseMove?,
            ~onMouseLeave?,
            ~onMouseDown?,
            ~onClick?,
            ~transform?,
            ~style,
            (),
          ),
        ),
      );

    externalCreateElement(externalG_, props, children);
  };
};

let rec renderTransition =
        (nextNode, RenderLinks.{nodes, flow, links, transform, bbox, render: nodeRender}) => {
  let nodes = List.map(renderTransition(nextNode), nodes);
  /* 1. look for a node in nextNode matching flow. (just first flow value for now) */
  let first_flow = flow->List.nth_opt(0);
  switch (first_flow) {
  /* no flow. render normally */
  | None => nodeRender(nodes, bbox, links) |> svgTransform(transform, bbox)
  | Some(first_flow) =>
    let next = findUID(first_flow, nextNode) |> Belt.Option.getExn;
    /* 2. apply svgTransformTransition from this node to new node */
    /* nodeRender(nodes, bbox, links) |> svgTransformTransition(transform, bbox, (), ()); */

    let (values, setValues) =
      SpringHook.use(
        ~config=Spring.config(~mass=1., ~tension=80., ~friction=20.),
        (transform.translate.x, transform.translate.y, transform.scale.x, transform.scale.y),
      );

    /* TODO: need to trigger differently somehow */

    <G
      onMouseMove={e => {
        setValues((
          next.transform.translate.x,
          next.transform.translate.y,
          next.transform.scale.x,
          next.transform.scale.y,
        ))
      }}
      transform={values->SpringHook.interpolate(computeSVGTransformFlattened(bbox))}
      style={ReactDOMRe.Style.make(
        ~transform=values->SpringHook.interpolate(computeSVGTransformFlattened(bbox)),
        (),
      )}>
      {nodeRender(nodes, bbox, links)}
    </G>;
  };
};