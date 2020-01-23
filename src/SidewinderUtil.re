open SidewinderKernel;

/* TODO: nesting and continuations? */

let atom = (e, width, height) => {
  nodes: [],
  links: [],
  constraints: [||],
  gap: None,
  linkDistance: None,
  render: (_, _) => {
    width,
    height,
    custom: {
      "rendered": e,
    },
  },
};

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

let makeEdges = (l: list(int), edgeRender): list(globalLink) =>
  List.combine(List.rev(l) |> List.tl |> List.rev, List.tl(l))
  |> List.map(((source, target)) =>
       (
         {
           source: {
             ancestorRoot: 0,
             absPath: [source],
           },
           target: {
             ancestorRoot: 0,
             absPath: [target],
           },
           edgeRender,
         }: globalLink
       )
     );

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

let defaultRender = (nodes: list(renderedWebCoLaNode), edges): renderedNode => {
  let renderEdge = ({source, target, edgeRender}: localLink) => {
    let sourceNode = List.nth(nodes, source);
    let targetNode = List.nth(nodes, target);
    edgeRender({x1: sourceNode.x, y1: sourceNode.y, x2: targetNode.x, y2: targetNode.y});
  };
  let bbox =
    List.map(
      ({x, y, width, height}: renderedWebCoLaNode) =>
        Rectangle.fromPointSize(~x, ~y, ~width, ~height, ()),
      nodes,
    )
    |> Rectangle.union_list;
  {
    width: Rectangle.width(bbox),
    height: Rectangle.height(bbox),
    custom: {
      "rendered":
        <>
          <g>
            {List.mapi(
               (i, n: renderedWebCoLaNode) =>
                 <g
                   key={string_of_int(i)}
                   transform={
                     "translate("
                     ++ Js.Float.toString(n.custom##bbox |> Rectangle.cx)
                     ++ ", "
                     ++ Js.Float.toString(n.custom##bbox |> Rectangle.cy)
                     ++ ")"
                   }>
                   {n.custom##rendered}
                 </g>,
               nodes,
             )
             |> Array.of_list
             |> React.array}
          </g>
          <g> {List.map(renderEdge, edges) |> Array.of_list |> React.array} </g>
        </>,
    },
  };
};

let sequence = (~gap=?, ~linkDistance=?, nodes, edgeRender, direction) => {
  nodes,
  links: makeEdges(List.length(nodes) |> range(0), edgeRender),
  gap,
  linkDistance,
  constraints: directionConstraints(direction),
  render: defaultRender,
};

/* NOTE! Doesn't provide any edge connections. */
/* TODO: Could allow edges, just don't render them. */
let nest = (nodes, nestRender) => {
  nodes,
  links: [],
  gap: None,
  linkDistance: None,
  constraints: [||],
  render: (nodes, _) => nestRender(nodes),
};