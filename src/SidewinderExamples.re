type direction =
  | UpDown
  | DownUp
  | LeftRight
  | RightLeft;

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
       |> List.map((Node.{bbox, rendered}) => <g> rendered </g>)  /* transform={
                "translate("
                ++ Js.Float.toString(-. bbox->Rectangle.width /. 2.)
                ++ ", "
                ++ Js.Float.toString(-. bbox->Rectangle.height /. 2.)
                ++ ")"
              } */
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
        /* move the center of the coordinate system to the center of the box */
        "translate("
        ++ Js.Float.toString(bbox->cx -. bbox->width /. 2.)
        ++ ", "
        ++ Js.Float.toString(bbox->cy -. bbox->height /. 2.)
        ++ ")"
      }>
      <rect
        width={Js.Float.toString(bbox->width)}
        height={Js.Float.toString(bbox->height)}
        fillOpacity="0"
        stroke="#000"
      />
      <g
        transform={
          /* move the center of the coordinate system to the center of the box */
          "translate("
          ++ Js.Float.toString(bbox->width /. 2.)
          ++ ", "
          ++ Js.Float.toString(bbox->height /. 2.)
          ++ ")"
        }>
        {defaultRender(nodes, Rectangle.empty /* unused */, links)}
      </g>
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

/* https://2ality.com/2018/01/lists-arrays-reasonml.html */
/**
* Compute a list of integers starting with `start`,
* up to and excluding `end_`.
*/
let rec range = (start: int, end_: int) =>
  if (start >= end_) {
    [];
  } else {
    [start, ...range(start + 1, end_)];
  };

let makeLinks = (linkRender, i) => {
  let nodeIndices = range(0, i);
  let stPairs =
    List.combine(List.rev(nodeIndices) |> List.tl |> List.rev, List.tl(nodeIndices));
  List.map(
    ((source, t)): Link.sourceLocal =>
      Link.{
        source,
        target: {
          ancestorRoot: 0,
          absPath: [t],
        },
        linkRender,
      },
    stPairs,
  );
};

/* TODO: may want e.g. UpDown and DownUp options to occupy the same space rather than moving in
   opposite direction from a shared starting point. */
let seqPos = (i, gap, direction) =>
  switch (direction) {
  | UpDown => (0., float_of_int(i) *. gap)
  | DownUp => (0., -. float_of_int(i) *. gap)
  | LeftRight => (float_of_int(i) *. gap, 0.)
  | RightLeft => (-. float_of_int(i) *. gap, 0.)
  };

/* TODO: should gap be gap between adjacent node sides or centers? For now assuming centers i.e.
   treating nodes as point-like. should probably be adjacent b/c it makes more sense */
let seq = (~nodes, ~linkRender, ~gap, ~direction) =>
  SideWinder.make(
    ~nodes,
    ~links=makeLinks(linkRender, List.length(nodes)),
    ~layout=
      (ns, _) =>
        List.mapi(
          (i, Node.{width, height}) => {
            let (cx, cy) = seqPos(i, gap, direction);
            Rectangle.fromCenterPointSize(~cx, ~cy, ~width, ~height);
          },
          ns,
        ),
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

let str = s =>
  atom(
    <text textAnchor="middle" dominantBaseline="middle"> {React.string(s)} </text>,
    Node.{width: 10., height: 20.},
  );

let a = str("2");
let a' = box(~dx=5., [a], []);

let b = str({js|•|js});
let b' = box(~dx=5., [b], []);

let c = str("4");
let c' = box(~dx=5., [c], []);

let d = str("/");
let d' = box(~dx=5., [d], []);

let e =
  seq(
    ~nodes=[a', b'],
    ~linkRender=(~source: _, ~target: _) => <> </>,
    ~gap=20.,
    ~direction=LeftRight,
  );

let f =
  seq(
    ~nodes=[c', d'],
    ~linkRender=(~source, ~target) => <> </>,
    ~gap=20.,
    ~direction=LeftRight,
  );

let g =
  graph(
    ~nodes=[e, f],
    ~links=[
      Link.{
        /* b */
        source: 0,
        /* f */
        target: {
          ancestorRoot: 0,
          absPath: [0, 1, 0],
        },
        linkRender: (~source, ~target) =>
          <line
            x1={Js.Float.toString(source->Rectangle.cx)}
            y1={Js.Float.toString(source->Rectangle.cy)}
            x2={Js.Float.toString(target->Rectangle.cx)}
            y2={Js.Float.toString(target->Rectangle.cy)}
            stroke="blue"
          />,
      },
    ],
    ~gap=20.,
    ~linkDistance=20.,
    ~constraints=[||],
  );