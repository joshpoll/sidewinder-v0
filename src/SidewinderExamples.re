/* The problem with my current layout stuff is that I can't agree on whether the center is at the
   top or the middle of the space! */
/* NOTE: everything should be rendered so that its bounding box starts at the origin in the top left
   corner. */
let debug_ = false;

let translate = (x, y, r) =>
  <g transform={"translate(" ++ Js.Float.toString(x) ++ ", " ++ Js.Float.toString(y) ++ ")"}>
    r
  </g>;

type direction =
  | UpDown
  | DownUp
  | LeftRight
  | RightLeft;

let directionConstraints = direction =>
  SetCoLa.(
    switch (direction) {
    | DownUp => [|
        C({
          "name": "layer",
          "sets": {
            "partition": "depth",
          },
          "forEach": [|{"constraint": "align", "axis": "x"}|],
        }),
        C({
          "name": "sort",
          "sets": [|"layer"|],
          "forEach": [|{"constraint": "order", "axis": "y", "by": "depth"}|],
        }),
      |]
    | UpDown => [|
        C({
          "name": "layer",
          "sets": {
            "partition": "depth",
          },
          "forEach": [|{"constraint": "align", "axis": "x"}|],
        }),
        C({
          "name": "sort",
          "sets": [|"layer"|],
          "forEach": [|{"constraint": "order", "axis": "y", "by": "depth", "reverse": true}|],
        }),
      |]
    | RightLeft => [|
        C({
          "name": "layer",
          "sets": {
            "partition": "depth",
          },
          "forEach": [|{"constraint": "align", "axis": "y"}|],
        }),
        C({
          "name": "sort",
          "sets": [|"layer"|],
          "forEach": [|{"constraint": "order", "axis": "x", "by": "depth"}|],
        }),
      |]
    | LeftRight => [|
        C({
          "name": "layer",
          "sets": {
            "partition": "depth",
          },
          "forEach": [|{"constraint": "align", "axis": "y"}|],
        }),
        C({
          "name": "sort",
          "sets": [|"layer"|],
          "forEach": [|{"constraint": "order", "axis": "x", "by": "depth", "reverse": true}|],
        }),
      |]
    }
  );

let drawBBox = (~stroke="red", bbox) =>
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
    /* strokeDasharray="10 5" */
  />;

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

let defaultRender = (nodes, bbox, links) => {
  Js.log2("big bbox", bbox);
  <>
    /* {drawBBox(~stroke="blue", bbox)} */
    /* translate the coordinate system */
    {translate(
       bbox->Rectangle.x1,
       bbox->Rectangle.y1,
       <>
         <g className="nodes">
           {nodes
            |> List.map((Node.{bbox, rendered}) => {
                 Js.log2("bbox", bbox);
                 rendered;
               })
            |> Array.of_list
            |> React.array}
         </g>
         <g className="links"> {links |> Array.of_list |> React.array} </g>
       </>,
     )}
  </>;
};

/**
 * Inputs: the element to render and the bounding box surrounding the rendered element
 */
let atom = (~links=[], r, size) =>
  SideWinder.make(
    ~nodes=[],
    ~links,
    ~layout=(_, _) => [],
    ~computeSize=_ => size,
    ~render=
      (_, _, _) =>
        <>
          {if (debug_) {
             drawBBox(size->Node.sizeToBBox);
           } else {
             <> </>;
           }}
          r
        </>,
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
    Js.log3("width/2 height/2", bbox->width /. 2., bbox->height /. 2.);
    Js.log3("center - wh/2", bbox->cx -. bbox->width /. 2., bbox->cy -. bbox->height /. 2.);
    Js.log2("nodes", nodes |> Array.of_list);
    <>
      <rect
        transform={
          "translate("
          ++ Js.Float.toString(bbox->x1)
          ++ ", "
          ++ Js.Float.toString(bbox->y1)
          ++ ")"
        }
        width={Js.Float.toString(bbox->width)}
        height={Js.Float.toString(bbox->height)}
        fillOpacity="0"
        stroke="#000"
      />
      {defaultRender(nodes, bbox->inflate(-. dx, -. dy), links)}
    </>;
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

/* let seqPos = (i, gap, size, direction) =>
   switch (direction) {
   | UpDown => (0., float_of_int(i) *. gap)
   | DownUp => (0., -. float_of_int(i) *. gap)
   | LeftRight => (float_of_int(i) *. gap, 0.)
   | RightLeft => (-. float_of_int(i) *. gap, 0.)
   }; */

/* NOTE: gap is between neighboring sides of bounding boxes */
/* TODO: need to recent DownUp and RightLeft so they are contained in the positive quadrant.
   Maybe more reason to have layout take care of that type of stuff. */
let seq = (~nodes, ~linkRender, ~gap, ~direction) =>
  SideWinder.make(
    ~nodes,
    ~links=makeLinks(linkRender, List.length(nodes)),
    ~layout=
      ([n, ..._] as ns, _) => {
        let stPairs = List.combine(List.rev(ns) |> List.tl |> List.rev, List.tl(ns));
        let newBBox = (bbox, Node.{width, height}) =>
          switch (direction) {
          | UpDown =>
            Rectangle.fromCenterPointSize(
              ~cx=bbox->Rectangle.cx,
              ~cy=bbox->Rectangle.y2 +. gap +. height /. 2.,
              ~width,
              ~height,
            )
          | DownUp =>
            Rectangle.fromCenterPointSize(
              ~cx=bbox->Rectangle.cx,
              ~cy=bbox->Rectangle.y1 -. gap -. height /. 2.,
              ~width,
              ~height,
            )
          | LeftRight =>
            Rectangle.fromCenterPointSize(
              ~cx=bbox->Rectangle.x2 +. gap +. width /. 2.,
              ~cy=bbox->Rectangle.cy,
              ~width,
              ~height,
            )
          | RightLeft =>
            Rectangle.fromCenterPointSize(
              ~cx=bbox->Rectangle.x1 -. gap -. width /. 2.,
              ~cy=bbox->Rectangle.cy,
              ~width,
              ~height,
            )
          };
        Relude.List.scanLeft((bbox, size) => newBBox(bbox, size), Node.sizeToBBox(n), ns);
      },
    /* TODO: compute gap cumulatively. */
    /* List.mapi(
         (i, Node.{width, height}) => {
           let (x, y) = seqPos(i, gap, Node.{width, height}, direction);
           Rectangle.fromPointSize(~x, ~y, ~width, ~height);
         },
         ns,
       ); */
    ~computeSize=computeSizeUnion,
    ~render=defaultRender,
  );

let str = (~width=9., ~height=12.5, s) =>
  atom(
    <text
      textAnchor="middle"
      dominantBaseline="middle"
      transform={
        "translate("
        ++ Js.Float.toString(width /. 2.)
        ++ ", "
        ++ Js.Float.toString(height /. 2. +. 1.5)
        ++ ")"
      }>
      {React.string(s)}
    </text>,
    {width, height},
  );

let a = str("2");
let a' = box(~dx=(12.5 -. 9.) /. 2. +. 2.5 /. 2., ~dy=2.5 /. 2., [a], []);

let b = atom(<circle r="2" cx="2" cy="2" />, {width: 4., height: 4.});
let b' = box(~dx=11. /. 2., ~dy=11. /. 2., [b], []);

let c = str("4");
let c' = box(~dx=(12.5 -. 9.) /. 2. +. 2.5 /. 2., ~dy=2.5 /. 2., [c], []);

let d = str("/");
let d' = box(~dx=(12.5 -. 9.) /. 2. +. 2.5 /. 2., ~dy=2.5 /. 2., [d], []);

let e =
  seq(
    ~nodes=[a', b'],
    ~linkRender=(~source: _, ~target: _) => <> </>,
    ~gap=0.,
    ~direction=LeftRight,
  );

let f =
  seq(~nodes=[c', d'], ~linkRender=(~source, ~target) => <> </>, ~gap=0., ~direction=LeftRight);

let g =
  graph(
    ~nodes=[e, f],
    ~links=[
      Link.{
        /* TODO: should go on b so it's b->f. instead we're reversing it for now */
        /* f */
        source: 1,
        /* b' */
        target: {
          ancestorRoot: 0,
          absPath: [1, 0],
        }, /* TODO: appending 0 here should be correct and give b, but it screws up */
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
    ~constraints=directionConstraints(RightLeft),
  );