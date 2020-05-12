Random.self_init();

let drawBBox = (~stroke="red", bbox) => {
  Node.(
    <rect
      transform={
        "translate("
        ++ Js.Float.toString(bbox->Rectangle.x1)
        ++ ", "
        ++ Js.Float.toString(bbox->Rectangle.y1)
        ++ ")"
      }
      width={Js.Float.toString(bbox->Rectangle.width)}
      height={Js.Float.toString(bbox->Rectangle.height)}
      stroke
      fillOpacity="0"
      strokeWidth="2px"
      /* strokeDasharray="10 5" */
    />
  );
};

let computeSVGTransform =
    ({translate: {x: tx, y: ty}, scale: {x: sx, y: sy}}: Node.transform, bbox) => {
  /* https://css-tricks.com/transforms-on-svg-elements/ */
  let scale =
    "translate("
    ++ Js.Float.toString(bbox->Rectangle.x1 +. bbox->Rectangle.width /. 2.)
    ++ ", "
    ++ Js.Float.toString(bbox->Rectangle.y1 +. bbox->Rectangle.height /. 2.)
    ++ ")";
  let scale =
    scale ++ " " ++ "scale(" ++ Js.Float.toString(sx) ++ ", " ++ Js.Float.toString(sy) ++ ")";
  let scale =
    scale
    ++ " "
    ++ "translate("
    ++ Js.Float.toString(-. (bbox->Rectangle.x1 +. bbox->Rectangle.width /. 2.))
    ++ ", "
    ++ Js.Float.toString(-. (bbox->Rectangle.y1 +. bbox->Rectangle.height /. 2.))
    ++ ")";

  let translate = "translate(" ++ Js.Float.toString(tx) ++ ", " ++ Js.Float.toString(ty) ++ ")";

  scale ++ " " ++ translate;
};

let svgTransform = (transform, bbox, r) => {
  let transform = computeSVGTransform(transform, bbox);
  <g transform> r </g>;
};

let defaultRender = (nodes, links) => {
  <>
    <g className="nodes">
      {nodes
       /* |> List.map((Node.{transform, bbox, rendered}) =>
            svgTransform(transform, bbox, rendered)
          ) */
       |> Array.of_list
       |> React.array}
    </g>
    <g className="links"> {links |> Array.of_list |> React.array} </g>
  </>;
};

let debugBBox = (~dx, ~dy, node) => {
  open Rectangle;
  let render = (nodes, bbox: Node.bbox, links) => {
    <> {drawBBox(bbox)} {defaultRender(nodes, links)} </>;
  };
  Kernel.{
    uid: Random.int(1000000) + 1000 |> string_of_int,
    flow: None /* TODO */,
    tags: ["debug"],
    nodes: [node],
    links: [],
    layout: (_, sizeOffsets, _) => MS.map(sizeOffsets, _ => Transform.ident),
    computeBBox: bs => bs->MS.valuesToArray->Array.to_list->union_list,
    render,
  };
};

let debugLink = (Link.{source, target, linkRender} as l: Link.uid): Link.uid =>
  Link.{
    source,
    target,
    linkRender:
      switch (linkRender) {
      | None => None
      | Some(lr) =>
        Some(
          (~source, ~target) => {
            Js.log3("source sizeOffset", source->Rectangle.x1, source->Rectangle.y1);
            Js.log3("target sizeOffset", target->Rectangle.x1, target->Rectangle.y1);
            <>
              /* {drawBBox(~stroke="orange", source)}
                 {drawBBox(~stroke="blue", target)} */
              {lr(~source, ~target)}
            </>;
          },
        )
      },
  };

let rec transform = (Kernel.{nodes, links} as n) => {
  Kernel.{
    ...n,
    nodes: nodes |> List.map(transform) |> List.map(debugBBox(~dx=0., ~dy=0.)),
    links: List.map(debugLink, links),
  };
};