let debug_ = false;
open Node;

/* Because Relude only implements scanl'.
   This implementation is copied from Haskell: https://hackage.haskell.org/package/base-4.12.0.0/docs/src/GHC.List.html#scanl */
let rec scanl: (('b, 'a) => 'b, 'b, list('a)) => list('b) =
  (f, q, ls) => {
    List.cons(
      q,
      switch (ls) {
      | [] => []
      | [x, ...xs] => scanl(f, f(q, x), xs)
      },
    );
  };

let translate = ({x, y}, r) =>
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
      /* strokeDasharray="10 5" */
    />
  );
};
module MS = Belt.Map.String;
let graphLayout =
    (
      ~constraints,
      ~gap,
      ~linkDistance,
      uidMap,
      nodeSizeOffsets,
      links: list(Sidewinder.Link.layout),
    ) => {
  let nodes =
    List.map(
      sizeOffset =>
        SetCoLa.{
          width: sizeOffset->Rectangle.width,
          height: sizeOffset->Rectangle.height,
          custom: Js.Obj.empty(),
        },
      nodeSizeOffsets,
    )
    |> Array.of_list;
  let links =
    List.map(
      (Link.{source, target}: Link.layout) =>
        WebCoLa.{
          source: NN(uidMap->MS.getExn(source)),
          target: NN(uidMap->MS.getExn(target)),
          length: None,
        },
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
  nodes
  |> Array.to_list
  |> List.filter((WebCoLa.{temp}) => !temp)
  |> List.combine(nodeSizeOffsets)
  |> List.map(((sizeOffset, WebCoLa.{x, y})) =>
       {
         translation: {
           x: x -. sizeOffset->Rectangle.x1,
           y: y -. sizeOffset->Rectangle.y1,
         },
         sizeOffset,
       }
     );
  /* TODO: this translation works, but not sure why. need to fix things. renders it at the origin */
  /* let union = Rectangle.union_list(bboxes); */
  /* List.map(
       bbox => bbox->Rectangle.translate(-. union->Rectangle.x1, -. union->Rectangle.y1),
       bboxes,
     ); */
};

let defaultRender = (nodes, bbox, links) => {
  <>
    {if (debug_) {
       //  Js.log2("default render bbox", bbox);
       drawBBox(
         ~stroke="blue",
         {...bbox, sizeOffset: bbox.sizeOffset->Rectangle.inflate(0.5, 0.5)},
       );
     } else {
       <> </>;
     }}
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

/**
 * Inputs: the element to render and the bounding box surrounding the rendered element
 */
let atom = (~tags=[], ~links=[], r, sizeOffset, ()) =>
  Main.make(
    ~tags=["atom", ...tags],
    ~nodes=[],
    ~links,
    ~layout=(_, _, _) => [],
    ~computeSizeOffset=_ => sizeOffset,
    ~render=
      (_, bbox, _) =>
        <>
          {if (debug_) {
             <>
               {/* {drawBBox(~stroke="purple", sizeOffset)} */ drawBBox(~stroke="magenta", bbox)}
             </>;
           } else {
             <> </>;
           }}
          {translate(bbox.translation, r)}
        </>,
  );

/* TODO: this needs to accept a layout parameter probably. Ideally box should be able to call this.
   But if I add that as a parameter this function is the same as Sidewinder.make */
let nest = (~tags=[], ~nodes, ~links, ~computeSizeOffset, ~render, ()) =>
  Main.make(
    ~tags=["nest", ...tags],
    ~nodes,
    ~links,
    ~layout=(_, sizeOffsets, _) => List.map(sizeOffsetToBBox, sizeOffsets),
    ~computeSizeOffset,
    ~render,
  );

let box = (~tags=[], ~dx=0., ~dy=0., node, links, ()) => {
  open Rectangle;
  let render = (nodes, bbox, links) => {
    <>
      /*     Js.log3("p1", bbox->x1, bbox->y1);
             Js.log3("p2", bbox->x2, bbox->y2);
             Js.log3("center", bbox->cx, bbox->cy);
             Js.log3("width/2 height/2", bbox->width /. 2., bbox->height /. 2.);
             Js.log3("center - wh/2", bbox->cx -. bbox->width /. 2., bbox->cy -. bbox->height /. 2.);
             Js.log2("nodes", nodes |> Array.of_list); */
      {if (debug_) {
         drawBBox(
           ~stroke="green",
           {...bbox, sizeOffset: bbox.sizeOffset->Rectangle.inflate(0.5, 0.5)},
         );
       } else {
         <> </>;
       }}
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
        stroke="#000"
      />
      {defaultRender(
         nodes,
         {...bbox, sizeOffset: bbox.sizeOffset->inflate(-. dx, -. dy)},
         links,
       )}
    </>;
  };
  Main.make(
    ~tags=["box", ...tags],
    ~nodes=[node],
    ~links,
    ~layout=(_, sizeOffsets, _) => List.map(sizeOffsetToBBox, sizeOffsets),
    ~computeSizeOffset=bs => List.map(bboxToSizeOffset, bs)->union_list->inflate(dx, dy),
    ~render,
  );
};

let graph = (~tags=[], ~nodes, ~links, ~gap=?, ~linkDistance=?, ~constraints, ()) =>
  Main.make(
    ~tags=["graph", ...tags],
    ~nodes,
    ~links,
    ~layout=graphLayout(~constraints, ~gap, ~linkDistance),
    ~computeSizeOffset=bs => List.map(bboxToSizeOffset, bs)->Rectangle.union_list,
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

let makeLinks = (linkRender, uids) => {
  // Js.log2("seq uids", uids |> Array.of_list);
  let stPairs = List.combine(List.rev(uids) |> List.tl |> List.rev, List.tl(uids));
  // Js.log2("pairs", stPairs |> Array.of_list);
  List.map(((source, target)): Link.uid => Link.{source, target, linkRender}, stPairs);
};

/* NOTE: gap is between neighboring sides of bounding boxes */
/* TODO: need to recenter DownUp and RightLeft so they are contained in the positive quadrant.
   Maybe more reason to have layout take care of that type of stuff. */
/* TODO: add an alignment flag for beginning/middle/end or something */
let seq = (~tags=[], ~nodes, ~linkRender, ~gap, ~direction, ()) =>
  Main.make(
    ~tags=["seq", ...tags],
    ~nodes,
    ~links=makeLinks(linkRender, List.map((Kernel.{uid}) => uid, nodes)),
    ~layout=
      (_, [n, ...rest] as ns, _) => {
        /* LR
            {w0, h0}
            {w1, h1}
            {w2, h2}
            -->
            {0, 0, w0, h0}
            {w0 + gap, 0, w1, h1}
            {(w0 + gap) + w1 + gap, 0, w2, h2}
           */
        let newBBox = (bbox, sizeOffset) => {
          let width = sizeOffset->Rectangle.width;
          let height = sizeOffset->Rectangle.height;
          switch (direction) {
          | UpDown => {
              translation: {
                x: -. sizeOffset->Rectangle.x1 /* -. sizeOffset->Rectangle.width   /. 2. /* center horizontally */ */, /* move to origin */
                y:
                  -. sizeOffset->Rectangle.y1  /* move to origin */
                  +. bbox.translation.y  /* translate past prev. bbox */
                  +. bbox.sizeOffset->Rectangle.y2  /* translate past prev. bbox */
                  +. gap /* add gap */,
              },
              sizeOffset,
            }
          /* Rectangle.fromPointSize(~x=0., ~y=bbox->Rectangle.y2 +. gap, ~width, ~height); */
          | DownUp => raise(failwith("TODO"))
          /* Rectangle.fromCenterPointSize(
               ~cx=bbox->Rectangle.cx,
               ~cy=bbox->Rectangle.y1 -. gap -. height /. 2.,
               ~width=bbox->Rectangle.width,
               ~height=bbox->Rectangle.height,
             ); */
          | LeftRight => {
              translation: {
                x:
                  -. sizeOffset->Rectangle.x1  /* move to origin */
                  +. bbox.translation.x  /* translate past prev. bbox */
                  +. bbox.sizeOffset->Rectangle.x2  /* translate past prev. bbox */
                  +. gap /* add gap */,
                y:
                  -. sizeOffset->Rectangle.y1  /* move to origin */
                  -. sizeOffset->Rectangle.height
                  /. 2. /* center vertically */,
              },
              sizeOffset,
            }
          /* Rectangle.fromPointSize(
               ~x=bbox->Rectangle.x2 +. gap,
               ~y=sizeOffset->Rectangle.y1,
               ~width,
               ~height,
             ) */
          | RightLeft => raise(failwith("TODO"))
          /* Rectangle.fromCenterPointSize(
               ~cx=bbox->Rectangle.x1 -. gap -. width /. 2.,
               ~cy=bbox->Rectangle.cy,
               ~width=bbox->Rectangle.width,
               ~height=bbox->Rectangle.height,
             ); */
          };
        };
        let initialBBox =
          switch (direction) {
          | UpDown => {
              translation: {
                x: -. n->Rectangle.x1 /* -. n->Rectangle.width /. 2. */,
                y: 0.,
              },
              sizeOffset: n,
            }
          | DownUp => raise(failwith("TODO"))
          | LeftRight => {
              translation: {
                x: 0.,
                y: -. n->Rectangle.y1 -. n->Rectangle.height /. 2.,
              },
              sizeOffset: n,
            }
          | RightLeft => raise(failwith("TODO"))
          };
        let bboxes: list(bbox) = scanl(newBBox, initialBBox, rest);
        // Js.log2("seq boxes", bboxes |> Array.of_list);
        // Js.log2("seq union", Rectangle.union_list(bboxes));
        /* Js.log2(
             "seq scanl boxes",
             Sidewinder.Util.scanl((bbox, size) => newBBox(bbox, size), Node.sizeToBBox(n), rest)
             |> Array.of_list,
           );
           Js.log2("seq boxes", bboxes |> Array.of_list);
           Js.log2("seq boxes size", bboxes |> Array.of_list |> Array.map(Node.bboxToSize)); */
        /* Js.log2(
             "test",
             Sidewinder.Util.scanl((a, b) => a / b, 64, [4, 2, 4]) |> Array.of_list,
           ); */
        Js.log2("seq bboxes", bboxes |> Array.of_list);
        bboxes;
      },
    // Js.log2("seq ns sizes", ns |> Array.of_list);
    ~computeSizeOffset=bs => List.map(bboxToSizeOffset, bs)->Rectangle.union_list,
    ~render=defaultRender,
  );

let str = (~tags=[], s, ()) => {
  let width = float_of_int(String.length(s) * 10);
  let height = 12.5;
  atom(
    ~tags,
    <text
      textAnchor="middle"
      dominantBaseline="middle"
      fontFamily="courier"
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
    (),
  );
};

let rec product = (l1, l2) =>
  switch (l1, l2) {
  | ([], _)
  | (_, []) => []
  | ([h1, ...t1], [h2, ...t2]) => [(h1, h2), ...product([h1], t2)] @ product(t1, l2)
  };

let rec repeat = (n, a) =>
  if (n == 0) {
    [];
  } else {
    [a, ...repeat(n - 1, a)];
  };

/* http://blog.shaynefletcher.org/2017/08/transpose.html */
let rec transposeAux = (acc, ls) =>
  switch (ls) {
  | []
  | [[], ..._] => List.rev(acc)
  | ls => transposeAux([List.map(List.hd, ls), ...acc], List.map(List.tl, ls))
  };

let transpose = transposeAux([]);

let makeTableLinks = (linkRender, uids) => {
  // Js.log2("table uids", uids |> List.map(Array.of_list) |> Array.of_list);
  let horizontalPairs = List.map(makeLinks(linkRender), uids);
  // Js.log2("hPairs", horizontalPairs |> List.map(Array.of_list) |> Array.of_list);
  let verticalPairs = List.map(makeLinks(linkRender), transpose(uids));
  // Js.log2("vPairs", verticalPairs |> List.map(Array.of_list) |> Array.of_list);
  List.flatten(horizontalPairs @ verticalPairs);
};

let rec partitionAux = (acc, n, l) =>
  if (n == 0) {
    (List.rev(acc), l);
  } else {
    switch (l) {
    | [] => failwith("ran out of elements to partition")
    | [h, ...t] => partitionAux([h, ...acc], n - 1, t)
    };
  };

let partition = partitionAux([]);

let rec constructMatrix = (l, rowLen) =>
  switch (l) {
  | [] => []
  | l =>
    let (h, t) = partition(rowLen, l);
    [h, ...constructMatrix(t, rowLen)];
  };

/* table. Like a nested hSeq and vSeq except allows layout and rendering to be influenced by all
   elements of the table */
/* kind of weird, because rows seem like a natural linkage more than columns, but need to consider
   them equally. hopefully this doesn't mess with transformations too much */
/* nodes is a list of rows */
/* TODO: test! */
let table = (~tags=[], ~nodes, ~linkRender, ~xGap, ~yGap, ~xDirection, ~yDirection, ()) => {
  let colLen = List.length(nodes);
  let rowLen = List.length(List.nth(nodes, 0));
  Main.make(
    ~tags=["table", ...tags],
    ~nodes=List.flatten(nodes),
    ~links=makeTableLinks(linkRender, List.map(List.map((Kernel.{uid}) => uid), nodes)),
    ~layout=
      (_, ns, _) => {
        /* reconstruct matrix */
        let mat = constructMatrix(ns, rowLen);

        /* max height per row */
        let maxHeightPerRow: list(float) =
          mat |> List.map(row => row |> List.map(Rectangle.height) |> List.fold_left(max, 0.));
        let cumulativeHeight = scanl((+.), 0., maxHeightPerRow);
        /* transpose */
        let matT = range(0, rowLen) |> List.map(i => List.map(l => List.nth(l, i), mat));
        /* max width per col */
        let maxWidthPerCol: list(float) =
          matT |> List.map(col => col |> List.map(Rectangle.width) |> List.fold_left(max, 0.));
        let cumulativeWidth = scanl((+.), 0., maxWidthPerCol);

        /* TODO: incorporate gap and direction info. currently assumes gaps are 0 and directions are
           topleft to bottomright */
        let computeBBox = (i, j, sizeOffset) => {
          let widthSoFar = List.nth(cumulativeWidth, j);
          let heightSoFar = List.nth(cumulativeHeight, i);
          {
            translation: {
              x: -. sizeOffset->Rectangle.x1 +. widthSoFar,
              y: -. sizeOffset->Rectangle.y1 -. sizeOffset->Rectangle.height /. 2. +. heightSoFar,
            },
            sizeOffset,
          };
        };

        let bboxes =
          mat
          |> List.mapi((i, row) => row |> List.mapi((j, so) => computeBBox(i, j, so)))
          |> List.flatten;
        Js.log2("table bboxes", bboxes |> Array.of_list);
        bboxes;
      },
    ~computeSizeOffset=bs => List.map(bboxToSizeOffset, bs)->Rectangle.union_list,
    ~render=defaultRender,
  );
};