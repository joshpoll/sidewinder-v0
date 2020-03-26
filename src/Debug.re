Random.self_init();

let drawBBox = (~stroke="red", bbox) => {
  Node.(
    <rect
      transform={
        "translate("
        ++ Js.Float.toString(bbox.sizeOffset->Rectangle.x1 +. bbox.translation.x)
        ++ ", "
        ++ Js.Float.toString(bbox.sizeOffset->Rectangle.y1 +. bbox.translation.y)
        ++ ")"
      }
      width={Js.Float.toString(bbox.sizeOffset->Rectangle.width)}
      height={Js.Float.toString(bbox.sizeOffset->Rectangle.height)}
      stroke
      fillOpacity="0"
      strokeWidth="2px"
      /* strokeDasharray="10 5" */
    />
  );
};

let translate = (Node.{x, y}, r) =>
  <g transform={"translate(" ++ Js.Float.toString(x) ++ ", " ++ Js.Float.toString(y) ++ ")"}>
    r
  </g>;

let defaultRender = (nodes, bbox: Node.bbox, links) => {
  <>
    /* translate the coordinate system */
    {translate(
       bbox.translation,
       <>
         <g className="nodes">
           {nodes
            |> List.map((Node.{bbox, rendered}) => {
                 //  Js.log2("bbox", bbox);
                 rendered
               })
            |> Array.of_list
            |> React.array}
         </g>
       </>,
     )}
    /* TODO: links already take their parent translation into account unlike nodes. is that good? */
    <g className="links"> {links |> Array.of_list |> React.array} </g>
  </>;
};

let debugBBox = (~dx, ~dy, node) => {
  open Rectangle;
  let render = (nodes, bbox: Node.bbox, links) => {
    let bbox = {...bbox, sizeOffset: bbox.sizeOffset->inflate(dx, dy)};
    <>
      {drawBBox(bbox)}
      {defaultRender(nodes, {...bbox, sizeOffset: bbox.sizeOffset->inflate(dx, dy)}, links)}
    </>;
  };
  Kernel.{
    uid: Random.int(1000000) + 1000 |> string_of_int,
    tags: ["debug"],
    nodes: [node],
    links: [],
    layout: (_, sizeOffsets, _) => List.map(Node.sizeOffsetToBBox, sizeOffsets),
    computeSizeOffset: bs => List.map(Node.bboxToSizeOffset, bs)->union_list,
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
              {drawBBox(~stroke="orange", Sidewinder.Node.sizeOffsetToBBox(source))}
              {drawBBox(~stroke="blue", Sidewinder.Node.sizeOffsetToBBox(target))}
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