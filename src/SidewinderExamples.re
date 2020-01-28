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

let graphLayout = (~constraints, ~gap, ~linkDistance, nodeBBoxes, links) => {
  let nodes =
    List.map(
      bbox =>
        SetCoLa.{
          width: bbox->Rectangle.width,
          height: bbox->Rectangle.height,
          custom: Js.Obj.empty(),
        },
      nodeBBoxes,
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
    WebCoLa.colaLayout()
    ->WebCoLa.nodes(setCoLaGraph.nodes)
    ->WebCoLa.links(setCoLaGraph.links)
    ->WebCoLa.constraints(setCoLaGraph.constraints)
    ->WebCoLa.avoidOverlaps(true);

  let colaGraph =
    switch (linkDistance) {
    | None => colaGraph
    | Some(linkDistance) => colaGraph->WebCoLa.linkDistance(linkDistance)
    };

  let colaGraph = colaGraph->WebCoLa.start(Some(50.), Some(100.), Some(200.), None);

  let nodes = colaGraph->WebCoLa.getNodes;
  let bboxes =
    nodes
    |> Array.to_list
    |> List.filter((WebCoLa.{temp}) => !temp)
    |> List.map((WebCoLa.{x, y, width, height}) =>
         Rectangle.fromPointSize(~x, ~y, ~width, ~height)
       );
  /* TODO: this translation works, but not sure why. need to fix things. renders it at the origin */
  let union = Rectangle.union_list(bboxes);
  List.map(
    bbox => bbox->Rectangle.translate(-. union->Rectangle.x1, -. union->Rectangle.y1),
    bboxes,
  );
};

let defaultRender = (nodes, bbox, links) => {
  <>
    {if (debug_) {
       Js.log2("default render bbox", bbox);
       drawBBox(~stroke="blue", bbox->Rectangle.inflate(0.5, 0.5));
     } else {
       <> </>;
     }}
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
let atom = (~links=[], r, bbox) =>
  SideWinder.make(
    ~nodes=[],
    ~links,
    ~layout=(_, _) => [],
    ~computeBBox=_ => bbox,
    ~render=
      (_, _, _) =>
        <>
          {if (debug_ && false) {
             drawBBox(bbox);
           } else {
             <> </>;
           }}
          r
        </>,
  );

/* TODO: this needs to accept a layout parameter probably. Ideally box should be able to call this.
   But if I add that as a parameter this function is the same as SideWinder.make */
let nest = (~nodes, ~links, ~computeBBox, ~render) =>
  SideWinder.make(~nodes, ~links, ~layout=(bboxes, _) => bboxes, ~computeBBox, ~render);

let box = (~dx=0., ~dy=0., nodes, links) => {
  open Rectangle;
  let render = (nodes, bbox, links) => {
    Js.log3("p1", bbox->x1, bbox->y1);
    Js.log3("p2", bbox->x2, bbox->y2);
    Js.log3("center", bbox->cx, bbox->cy);
    Js.log3("width/2 height/2", bbox->width /. 2., bbox->height /. 2.);
    Js.log3("center - wh/2", bbox->cx -. bbox->width /. 2., bbox->cy -. bbox->height /. 2.);
    Js.log2("nodes", nodes |> Array.of_list);
    <>
      {if (debug_) {
         drawBBox(~stroke="green", bbox->Rectangle.inflate(0.5, 0.5));
       } else {
         <> </>;
       }}
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
  SideWinder.make(
    ~nodes,
    ~links,
    ~layout=(bboxes, _) => List.map(n => n->Rectangle.translate(dx, dy), bboxes),
    ~computeBBox=bs => union_list(bs)->inflate(dx, dy),
    ~render,
  );
};

let graph = (~nodes, ~links, ~gap=?, ~linkDistance=?, ~constraints) =>
  SideWinder.make(
    ~nodes,
    ~links,
    ~layout=graphLayout(~constraints, ~gap, ~linkDistance),
    ~computeBBox=Rectangle.union_list,
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

/* NOTE: gap is between neighboring sides of bounding boxes */
/* TODO: need to recenter DownUp and RightLeft so they are contained in the positive quadrant.
   Maybe more reason to have layout take care of that type of stuff. */
let seq = (~nodes, ~linkRender, ~gap, ~direction) =>
  SideWinder.make(
    ~nodes,
    ~links=makeLinks(linkRender, List.length(nodes)),
    ~layout=
      ([n, ...rest] as ns, _) => {
        Js.log2("seq ns sizes", ns |> Array.of_list);
        /* LR
            {w0, h0}
            {w1, h1}
            {w2, h2}
            -->
            {0, 0, w0, h0}
            {w0 + gap, 0, w1, h1}
            {(w0 + gap) + w1 + gap, 0, w2, h2}
           */
        let newBBox = (bbox, Node.{width, height}) =>
          switch (direction) {
          | UpDown =>
            raise(failwith("TODO"));
            Rectangle.fromCenterPointSize(
              ~cx=bbox->Rectangle.cx,
              ~cy=bbox->Rectangle.y2 +. gap +. height /. 2.,
              ~width=bbox->Rectangle.width,
              ~height=bbox->Rectangle.height,
            );
          | DownUp =>
            raise(failwith("TODO"));
            Rectangle.fromCenterPointSize(
              ~cx=bbox->Rectangle.cx,
              ~cy=bbox->Rectangle.y1 -. gap -. height /. 2.,
              ~width=bbox->Rectangle.width,
              ~height=bbox->Rectangle.height,
            );
          | LeftRight =>
            /* TODO: center boxes vertically */
            Rectangle.fromPointSize(~x=bbox->Rectangle.x2 +. gap, ~y=0., ~width, ~height)
          | RightLeft =>
            raise(failwith("TODO"));
            Rectangle.fromCenterPointSize(
              ~cx=bbox->Rectangle.x1 -. gap -. width /. 2.,
              ~cy=bbox->Rectangle.cy,
              ~width=bbox->Rectangle.width,
              ~height=bbox->Rectangle.height,
            );
          };
        let bboxes =
          SideWinder.Util.scanl(
            (bbox, size) => newBBox(bbox, size),
            n,
            rest |> List.map(Node.bboxToSize) /* TODO: come up with a better solution here */,
          );
        /* Js.log2(
             "seq scanl boxes",
             SideWinder.Util.scanl((bbox, size) => newBBox(bbox, size), Node.sizeToBBox(n), rest)
             |> Array.of_list,
           );
           Js.log2("seq boxes", bboxes |> Array.of_list);
           Js.log2("seq boxes size", bboxes |> Array.of_list |> Array.map(Node.bboxToSize)); */
        /* Js.log2(
             "test",
             SideWinder.Util.scanl((a, b) => a / b, 64, [4, 2, 4]) |> Array.of_list,
           ); */
        bboxes;
      },
    ~computeBBox=Rectangle.union_list,
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
    Rectangle.fromPointSize(~x=0., ~y=0., ~width, ~height),
  );

let a = str("2");
let a' = box(~dx=(12.5 -. 9.) /. 2. +. 2.5 /. 2., ~dy=2.5 /. 2., [a], []);

let b =
  atom(
    ~links=[],
    <circle r="2" cx="2" cy="2" />,
    Rectangle.fromPointSize(~x=0., ~y=0., ~width=4., ~height=4.),
  );
let b' =
  box(
    ~dx=11. /. 2.,
    ~dy=11. /. 2.,
    [b],
    [
      Link.{
        /* b */
        source: 0,
        /* f */
        target: {
          ancestorRoot: 2, /* 0 = b', 1 = e, 2 = g */
          absPath: [1 /* 1, 0 */],
        },
        linkRender: (~source, ~target) => {
          <>
            {if (debug_) {
               <> {drawBBox(source)} {drawBBox(target)} </>;
             } else {
               <> </>;
             }}
            <line
              x1={Js.Float.toString(source->Rectangle.x2)}
              y1={Js.Float.toString(source->Rectangle.cy)}
              x2={Js.Float.toString(target->Rectangle.x1)}
              y2={Js.Float.toString(target->Rectangle.cy)}
              stroke="purple"
            />
          </>;
        },
      },
    ],
  );

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
    ~links=[],
    ~gap=25.,
    ~linkDistance=40.,
    ~constraints=directionConstraints(LeftRight),
  );