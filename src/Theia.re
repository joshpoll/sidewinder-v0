let debug_ = false;

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

let drawBBox = (~stroke="red", transform, bbox) => {
  Node.(
    <rect
      transform={computeSVGTransform(transform, bbox)}
      width={Js.Float.toString(bbox->Rectangle.width)}
      height={Js.Float.toString(bbox->Rectangle.height)}
      stroke
      fillOpacity="0"
      /* strokeDasharray="10 5" */
    />
  );
};
module MS = Belt.Map.String;
/* https://stackoverflow.com/questions/31279920/finding-an-item-in-a-list-and-returning-its-index-ocaml */
let rec find = (x, lst) =>
  switch (lst) {
  | [] => raise(failwith("Not Found"))
  | [h, ...t] =>
    if (x == h) {
      0;
    } else {
      1 + find(x, t);
    }
  };

let graphLayout =
    (
      ~constraints,
      ~gap,
      ~linkDistance,
      uids,
      bboxes: MS.t(Node.bbox),
      links: list(Sidewinder.Link.layout),
    ) => {
  let nodes =
    MS.map(bboxes, sizeOffset =>
      SetCoLa.{
        width: sizeOffset->Rectangle.width,
        height: sizeOffset->Rectangle.height,
        custom: Js.Obj.empty(),
      }
    );
  let links =
    List.map(
      (Link.{source, target}: Link.layout) =>
        WebCoLa.{
          source: NN(find(source, uids)),
          target: NN(find(target, uids)),
          length: None,
        },
      links,
    )
    |> Array.of_list;

  let setCoLaGraph =
    SetCoLa.setCola
    ->SetCoLa.nodes(nodes->MS.valuesToArray)
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

  let nodes =
    colaGraph->WebCoLa.getNodes |> Array.to_list |> List.filter((WebCoLa.{temp}) => !temp);
  MS.mapWithKey(
    bboxes,
    (uid, bbox) => {
      let colaNode = List.nth(nodes, find(uid, uids));
      Transform.{
        scale: {
          x: 1.,
          y: 1.,
        },
        translate: {
          x: colaNode.x -. bbox->Rectangle.x1,
          y: colaNode.y -. bbox->Rectangle.y1,
        },
      };
    },
  );
};

let defaultRender = (nodes, links) => {
  <>
    <g className="nodes"> {nodes |> Array.of_list |> React.array} </g>
    <g className="links"> {links |> Array.of_list |> React.array} </g>
  </>;
};

/**
 * Inputs: the element to render and the bounding box surrounding the rendered element
 */
let atom = (~uid=?, ~flowTag=?, ~tags=[], ~links=[], r, sizeOffset, ()) =>
  Main.make(
    ~uid?,
    ~flowTag?,
    ~tags=["atom", ...tags],
    ~nodes=[],
    ~links,
    ~layout=(_, _, _) => MS.empty,
    ~computeBBox=_ => sizeOffset,
    ~render=(_, _, _) => r,
    (),
  );

/* TODO: this needs to accept a layout parameter probably. Ideally box should be able to call this.
   But if I add that as a parameter this function is the same as Sidewinder.make */
let nest = (~uid=?, ~flowTag=?, ~tags=[], ~nodes, ~links, ~computeBBox, ~render, ()) =>
  Main.make(
    ~uid?,
    ~flowTag?,
    ~tags=["nest", ...tags],
    ~nodes,
    ~links,
    ~layout=(_, bboxes, _) => bboxes->MS.map(_ => Transform.ident),
    ~computeBBox,
    ~render,
    (),
  );

let noop = (~uid=?, ~flowTag=?, ~tags=[], node, links, ()) => {
  let render = (nodes, bbox, links) => defaultRender(nodes, links);
  Main.make(
    ~uid?,
    ~flowTag?,
    ~tags=["noop", ...tags],
    ~nodes=[node],
    ~links,
    ~layout=(_, bboxes, _) => MS.map(bboxes, _ => Transform.ident),
    ~computeBBox=bs => bs->MS.valuesToArray->Array.to_list->Rectangle.union_list,
    ~render,
    (),
  );
};

/* TODO: transform must include scaling! */
let box = (~uid=?, ~flowTag=?, ~tags=[], ~dx=0., ~dy=0., node, links, ()) => {
  open Rectangle;
  let render = (nodes, bbox, links) => {
    <>
      <rect
        x={Js.Float.toString(bbox->Rectangle.x1)}
        y={Js.Float.toString(bbox->Rectangle.y1)}
        width={Js.Float.toString(bbox->Rectangle.width)}
        height={Js.Float.toString(bbox->Rectangle.height)}
        fillOpacity="0"
        stroke="#000"
      />
      {defaultRender(nodes, links)}
    </>;
  };
  Main.make(
    ~uid?,
    ~flowTag?,
    ~tags=["box", ...tags],
    ~nodes=[node],
    ~links,
    ~layout=(_, bboxes, _) => MS.map(bboxes, _ => Transform.ident),
    ~computeBBox=bs => bs->MS.valuesToArray->Array.to_list->union_list->inflate(dx, dy),
    ~render,
    (),
  );
};

let graph =
    (~uid=?, ~flowTag=?, ~tags=[], ~nodes, ~links, ~gap=?, ~linkDistance=?, ~constraints, ()) =>
  Main.make(
    ~uid?,
    ~flowTag?,
    ~tags=["graph", ...tags],
    ~nodes,
    ~links,
    ~layout=graphLayout(~constraints, ~gap, ~linkDistance),
    ~computeBBox=bs => bs->MS.valuesToArray->Array.to_list->Rectangle.union_list,
    ~render=(nodes, _, links) => defaultRender(nodes, links),
    (),
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
let seq = (~uid=?, ~flowTag=?, ~tags=[], ~nodes, ~linkRender, ~gap, ~direction, ()) =>
  Main.make(
    ~uid?,
    ~flowTag?,
    ~tags=["seq", ...tags],
    ~nodes,
    ~links=makeLinks(linkRender, List.map((Kernel.{uid}) => uid, nodes)),
    ~layout=
      (uids, ns, _) => {
        let [n, ...rest] as ns = List.map(MS.getExn(ns), uids);
        /* LR
            {w0, h0}
            {w1, h1}
            {w2, h2}
            -->
            {0, 0, w0, h0}
            {w0 + gap, 0, w1, h1}
            {(w0 + gap) + w1 + gap, 0, w2, h2}
           */
        let newBBox = ((transform, prevBBox), bbox) => {
          let width = bbox->Rectangle.width;
          let height = bbox->Rectangle.height;
          switch (direction) {
          | UpDown => (
              Transform.{
                scale: {
                  x: 1.,
                  y: 1.,
                },
                translate: {
                  x: -. bbox->Rectangle.x1 /* -. sizeOffset->Rectangle.width   /. 2. /* center horizontally */ */, /* move to origin */
                  y:
                    -. bbox->Rectangle.y1  /* move to origin */
                    +. transform.translate.y  /* translate past prev. bbox */
                    +. prevBBox->Rectangle.y2  /* translate past prev. bbox */
                    +. gap /* add gap */,
                },
              },
              bbox,
            )
          /* Rectangle.fromPointSize(~x=0., ~y=bbox->Rectangle.y2 +. gap, ~width, ~height); */
          | DownUp => raise(failwith("TODO"))
          /* Rectangle.fromCenterPointSize(
               ~cx=bbox->Rectangle.cx,
               ~cy=bbox->Rectangle.y1 -. gap -. height /. 2.,
               ~width=bbox->Rectangle.width,
               ~height=bbox->Rectangle.height,
             ); */
          | LeftRight => (
              Transform.{
                scale: {
                  x: 1.,
                  y: 1.,
                },
                translate: {
                  x:
                    -. bbox->Rectangle.x1  /* move to origin */
                    +. transform.translate.x  /* translate past prev. bbox */
                    +. prevBBox->Rectangle.x2  /* translate past prev. bbox */
                    +. gap /* add gap */,
                  y:
                    -. bbox->Rectangle.y1  /* move to origin */
                    -. bbox->Rectangle.height
                    /. 2. /* center vertically */,
                },
              },
              bbox,
            )
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
          | UpDown => (
              Transform.{
                scale: {
                  x: 1.,
                  y: 1.,
                },
                translate: {
                  x: -. n->Rectangle.x1 /* -. n->Rectangle.width /. 2. */,
                  y: 0.,
                },
              },
              n,
            )
          | DownUp => raise(failwith("TODO"))
          | LeftRight => (
              {
                scale: {
                  x: 1.,
                  y: 1.,
                },
                translate: {
                  x: 0.,
                  y: -. n->Rectangle.y1 -. n->Rectangle.height /. 2.,
                },
              },
              n,
            )
          | RightLeft => raise(failwith("TODO"))
          };
        let (transforms, _) = scanl(newBBox, initialBBox, rest) |> List.split;
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
        // Js.log2("seq bboxes", bboxes |> Array.of_list);
        // Js.log2("[seq] transforms", transforms |> Array.of_list);
        List.combine(uids, transforms)
        |> List.fold_left((mp, (uid, transform)) => mp->MS.set(uid, transform), MS.empty);
      },
    // Js.log2("seq ns sizes", ns |> Array.of_list);
    ~computeBBox=bs => bs->MS.valuesToArray->Array.to_list->Rectangle.union_list,
    ~render=(nodes, _, links) => defaultRender(nodes, links),
    (),
  );

let str = (~uid=?, ~flowTag=?, ~tags=[], s, ()) => {
  let width = float_of_int(String.length(s) * 10);
  let height = 12.5;
  atom(
    ~uid?,
    ~flowTag?,
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

let makeTableLinks = (xLinkRender, yLinkRender, uids) => {
  // Js.log2("table uids", uids |> List.map(Array.of_list) |> Array.of_list);
  let horizontalPairs = List.map(makeLinks(xLinkRender), uids |> Matrix.toListList);
  // Js.log2("hPairs", horizontalPairs |> List.map(Array.of_list) |> Array.of_list);
  let verticalPairs =
    List.map(makeLinks(yLinkRender), Matrix.transpose(uids) |> Matrix.toListList);
  // Js.log2("vPairs", verticalPairs |> List.map(Array.of_list) |> Array.of_list);
  List.flatten(horizontalPairs @ verticalPairs);
};

/* table. Like a nested hSeq and vSeq except allows layout and rendering to be influenced by all
   elements of the table */
/* kind of weird, because rows seem like a natural linkage more than columns, but need to consider
   them equally. hopefully this doesn't mess with transformations too much */
/* nodes is a list of rows */
/* TODO: add more customization for row and column arrangement */
let table =
    (
      ~uid=?,
      ~flowTag=?,
      ~tags=[],
      ~nodes,
      ~xLinkRender,
      ~yLinkRender,
      ~xGap,
      ~yGap,
      ~xDirection,
      ~yDirection,
      (),
    ) => {
  let colLen = List.length(nodes);
  let rowLen = List.length(List.nth(nodes, 0));
  Main.make(
    ~uid?,
    ~flowTag?,
    ~tags=["table", ...tags],
    ~nodes=List.flatten(nodes),
    ~links=
      makeTableLinks(
        xLinkRender,
        yLinkRender,
        nodes |> Matrix.fromListList |> Matrix.map((Kernel.{uid}) => uid),
      ),
    ~layout=
      (uids, ns, _) => {
        /* reconstruct matrix */
        let mat = Matrix.fromList(List.map(MS.getExn(ns), uids), rowLen);

        /* max height per row */
        let maxHeightPerRow: list(float) =
          mat
          |> Matrix.toListList
          |> List.map(row => row |> List.map(Rectangle.height) |> List.fold_left(max, 0.));
        let cumulativeHeight = scanl((+.), 0., maxHeightPerRow);
        /* transpose */
        let matT = Matrix.transpose(mat);
        /* max width per col */
        let maxWidthPerCol: list(float) =
          matT
          |> Matrix.toListList
          |> List.map(col => col |> List.map(Rectangle.width) |> List.fold_left(max, 0.));
        let cumulativeWidth = scanl((+.), 0., maxWidthPerCol);

        /* TODO: incorporate gap and direction info. currently assumes gaps are 0 and directions are
           topleft to bottomright */
        let computeTransform = (i, j, bbox) => {
          let widthSoFar = List.nth(cumulativeWidth, j);
          let heightSoFar = List.nth(cumulativeHeight, i);
          let cellWidth = List.nth(maxWidthPerCol, j);
          let cellHeight = List.nth(maxHeightPerRow, i);
          let widthPadding = cellWidth -. bbox->Rectangle.width;
          let heightPadding = cellHeight -. bbox->Rectangle.height;
          Transform.{
            scale: {
              x: 1.,
              y: 1.,
            },
            translate: {
              x: -. bbox->Rectangle.x1 +. widthPadding /. 2. +. widthSoFar,
              y: -. bbox->Rectangle.y1 +. heightPadding /. 2. +. heightSoFar,
            },
          };
        };

        let transforms =
          mat
          |> Matrix.toListList
          |> List.mapi((i, row) => row |> List.mapi((j, so) => computeTransform(i, j, so)))
          |> List.flatten;
        // Js.log2("table transforms", transforms |> Array.of_list);
        List.combine(uids, transforms)
        |> List.fold_left((mp, (uid, transform)) => mp->MS.set(uid, transform), MS.empty);
      },
    ~computeBBox=bs => bs->MS.valuesToArray->Array.to_list->Rectangle.union_list,
    ~render=(nodes, _, links) => defaultRender(nodes, links),
    (),
  );
};