Random.self_init();

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
        fillOpacity="0"
        stroke="red"
        strokeWidth="2px"
      />
      {defaultRender(nodes, {...bbox, sizeOffset: bbox.sizeOffset->inflate(dx, dy)}, links)}
    </>;
  };
  Kernel.{
    uid: Random.int(1000000) |> string_of_int,
    tags: ["debug"],
    nodes: [node],
    links: [],
    layout: (_, sizeOffsets, _) => List.map(Node.sizeOffsetToBBox, sizeOffsets),
    computeSizeOffset: bs => List.map(Node.bboxToSizeOffset, bs)->union_list,
    render,
  };
};

let rec transform = (Kernel.{nodes} as n) => {
  Kernel.{...n, nodes: nodes |> List.map(transform) |> List.map(debugBBox(~dx=0., ~dy=0.))};
};