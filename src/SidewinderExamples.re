/* (
     list(ReasonReactExamples.Node.size),
     list(
       ReasonReactExamples.Link.local
     )
   ) => list(ReasonReactExamples.Node.bbox) */
let graphLayout = (~constraints, ~gap, ~linkDistance, nodeSizes, links) => {
  let nodes =
    List.map(
      (Node.{width, height}) => SetCoLa.{width, height, custom: Js.Obj.empty()},
      nodeSizes,
    )
    |> Array.of_list;
  let links =
    List.map(
      (Link.{source, target}: Link.local) =>
        WebCoLa.{source: NN(source), target: NN(target), length: None},
      links,
    )
    |> Array.of_list;

  let setCoLaGraph =
    SetCoLa.setCola
    ->SetCoLa.nodes(nodes)
    ->SetCoLa.links(links)
    ->SetCoLa.constraints(constraints);

  let setCoLaGraph =
    switch (gap) {
    | None => setCoLaGraph
    | Some(gap) => setCoLaGraph->SetCoLa.gap(gap)
    };

  let setCoLaGraph = setCoLaGraph->SetCoLa.layout;

  let colaGraph =
    WebCoLa.(
      colaLayout()
      ->nodes(setCoLaGraph.nodes)
      ->links(setCoLaGraph.links)
      ->constraints(setCoLaGraph.constraints)
      ->avoidOverlaps(true)
    );

  let colaGraph =
    switch (linkDistance) {
    | None => colaGraph
    | Some(linkDistance) => colaGraph->WebCoLa.linkDistance(linkDistance)
    };

  let colaGraph = colaGraph->WebCoLa.start(Some(50.), Some(100.), Some(200.), None);

  let nodes = colaGraph->WebCoLa.getNodes;
  nodes
  |> Array.to_list
  |> List.filter((WebCoLa.{temp}) => !temp)
  |> List.map((WebCoLa.{x, y, width, height}) =>
       Rectangle.fromPointSize(~x, ~y, ~width, ~height)
     );
};

let computeSizeUnion = bboxes => {
  let union = Rectangle.union_list(bboxes);
  Node.{width: union->Rectangle.width, height: union->Rectangle.height};
};

let defaultRender = (nodes, _bbox, links) =>
  <g>
    <g className="nodes">
      {nodes
       |> List.map((Node.{bbox, rendered}) =>
            <g
              transform={
                "translate("
                ++ Js.Float.toString(bbox->Rectangle.x1)
                ++ ", "
                ++ Js.Float.toString(bbox->Rectangle.x2)
                ++ ")"
              }>
              rendered
            </g>
          )
       |> Array.of_list
       |> React.array}
    </g>
    <g className="links"> {links |> Array.of_list |> React.array} </g>
  </g>;

let atom = (r, size) =>
  SideWinder.make(
    ~nodes=[],
    ~links=[],
    ~layout=(sizes, _) => List.map(Node.sizeToBBox, sizes),
    ~computeSize=_ => size,
    ~render=(_, _, _) => r,
  );

let nest = (~nodes, ~computeSize, ~render) =>
  SideWinder.make(
    ~nodes,
    ~links=[],
    ~layout=(sizes, _) => List.map(Node.sizeToBBox, sizes),
    ~computeSize,
    ~render=(n, b, _) => render(n, b),
  );

let box = (~dx=0., ~dy=0., nodes, links) => {
  open Rectangle;
  let render = (nodes, bbox) => {
    Js.log3("p1", bbox->x1, bbox->y1);
    Js.log3("p2", bbox->x2, bbox->y2);
    Js.log3("center", bbox->cx, bbox->cy);
    <g
      transform={
        "translate(" ++ Js.Float.toString(bbox->cx) ++ ", " ++ Js.Float.toString(bbox->cy) ++ ")"
      }>
      <rect
        width={Js.Float.toString(bbox->width)}
        height={Js.Float.toString(bbox->height)}
        fillOpacity="0"
        stroke="#000"
      />
      {defaultRender(nodes, Rectangle.empty /* unused */, links)}
    </g>;
  };
  nest(~nodes, ~computeSize=bs => union_list(bs)->inflate(dx, dy)->Node.bboxToSize, ~render);
};

let graph = (~nodes, ~links, ~gap=?, ~linkDistance=?, ~constraints) =>
  SideWinder.make(
    ~nodes,
    ~links,
    ~layout=graphLayout(~constraints, ~gap, ~linkDistance),
    ~computeSize=computeSizeUnion,
    ~render=defaultRender,
  );

/* open SidewinderUtil;

   let box = (~dx=0., ~dy=0., child) =>
     nest(
       [child],
       ([c]) => {
         let bbox =
           Rectangle.(
             fromPointSize(~x=c.x, ~y=c.y, ~width=c.width, ~height=c.height, ())->inflate(dx, dy)
           );
         Rectangle.{
           width: bbox->width,
           height: bbox->height,
           custom: {
             "rendered":
               <g
                 transform={
                   "translate("
                   ++ Js.Float.toString(bbox->x1)
                   ++ ", "
                   ++ Js.Float.toString(bbox->y1)
                   ++ ")"
                 }>
                 <rect
                   width={Js.Float.toString(bbox->width)}
                   height={Js.Float.toString(bbox->height)}
                   fillOpacity="0"
                   stroke="#000"
                   transform={
                     "translate("
                     ++ Js.Float.toString(-. bbox->width /. 2.)
                     ++ ", "
                     ++ Js.Float.toString(-. bbox->height /. 2.)
                     ++ ")"
                   }
                 />
                 {c.custom##rendered}
               </g>,
           },
         };
       },
     );

   let a =
     atom(
       <text textAnchor="middle" dominantBaseline="middle"> {React.string("2")} </text>,
       10.,
       20.,
     );
   let a' = box(~dx=5., a);
   let b =
     atom(
       <text textAnchor="middle" dominantBaseline="middle"> {React.string({js|•|js})} </text>,
       10.,
       20.,
     );
   let b' = box(~dx=5., b);
   let c =
     atom(
       <text textAnchor="middle" dominantBaseline="middle"> {React.string("4")} </text>,
       10.,
       20.,
     );
   let c' = box(~dx=5., c);
   let d =
     atom(
       <text textAnchor="middle" dominantBaseline="middle"> {React.string("/")} </text>,
       10.,
       20.,
     );
   let d' = box(~dx=5., d);
   let e = sequence(~gap=0.1, ~linkDistance=0.1, [a', b'], _ => <> </>, LeftRight);
   let f = sequence(~gap=0.1, ~linkDistance=0.1, [c', d'], _ => <> </>, LeftRight);
   /* TODO: need a nonlocal edge from b to f */
   let g = sequence(~gap=30., [e, f], _ => <> </>, LeftRight); */

let a =
  atom(
    <text textAnchor="middle" dominantBaseline="middle"> {React.string("2")} </text>,
    Node.{width: 10., height: 20.},
  );
let a' = box(~dx=5., [a], []);
/* let b =
     atom(
       <text textAnchor="middle" dominantBaseline="middle"> {React.string({js|•|js})} </text>,
       10.,
       20.,
     );
   let b' = box(~dx=5., b);
   let c =
     atom(
       <text textAnchor="middle" dominantBaseline="middle"> {React.string("4")} </text>,
       10.,
       20.,
     );
   let c' = box(~dx=5., c);
   let d =
     atom(
       <text textAnchor="middle" dominantBaseline="middle"> {React.string("/")} </text>,
       10.,
       20.,
     );
   let d' = box(~dx=5., d);
   let e = sequence(~gap=0.1, ~linkDistance=0.1, [a', b'], _ => <> </>, LeftRight);
   let f = sequence(~gap=0.1, ~linkDistance=0.1, [c', d'], _ => <> </>, LeftRight);
   /* TODO: need a nonlocal edge from b to f */
   let g = sequence(~gap=30., [e, f], _ => <> </>, LeftRight); */