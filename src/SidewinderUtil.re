open SidewinderKernel;

/* TODO: nesting and continuations? */

let atom = (e, width, height) => {
  nodes: [],
  links: [],
  constraints: [||],
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

let makeEdges = (l, edgeRender) =>
  List.combine(List.rev(l) |> List.tl |> List.rev, List.tl(l))
  |> List.map(((source, target)) => {source, target, edgeRender});

type direction =
  | UpDown
  | DownUp
  | LeftRight
  | RightLeft;

let directionConstraints = direction =>
  SetCoLa.(
    switch (direction) {
    | RightLeft => [|
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
    | LeftRight => [|
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
    | DownUp => [|
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
    | UpDown => [|
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

let defaultRender = ({bbox, nodes}, edges): renderedNode => {
  let renderEdge = ({source, target, edgeRender}) => {
    let sourceNode = List.nth(nodes, source);
    let targetNode = List.nth(nodes, target);
    edgeRender({x1: sourceNode.x, y1: sourceNode.y, x2: targetNode.x, y2: targetNode.y});
  };
  {
    width: Rectangle.width(bbox),
    height: Rectangle.height(bbox),
    custom: {
      "rendered":
        <div style={ReactDOMRe.Style.make(~display="inline-block", ~position="absolute", ())}>
          <div style={ReactDOMRe.Style.make(~display="inline-block", ~position="absolute", ())}>
            {List.map(
               (n: renderedWebCoLaNode) =>
                 <div
                   style={ReactDOMRe.Style.make(
                     ~display="inline-block",
                     ~position="absolute",
                     ~top=Js.Float.toString(n.x) ++ "px",
                     ~left=Js.Float.toString(n.y) ++ "px",
                     (),
                   )}>
                   {n.custom##rendered}
                 </div>,
               nodes,
             )
             |> Array.of_list
             |> React.array}
          </div>
          <div style={ReactDOMRe.Style.make(~display="inline-block", ())}>
            {List.map(renderEdge, edges) |> Array.of_list |> React.array}
          </div>
        </div>,
    },
  };
};

let sequence = (nodes, edgeRender, direction) => {
  nodes,
  links: makeEdges(List.length(nodes) |> range(0), edgeRender),
  constraints: directionConstraints(direction),
  render: defaultRender,
};

/* NOTE! Doesn't provide any edge connections. */
/* TODO: Could allow edges, just don't render them. */
/* NOTE! Doesn't rely on bounding box? Because computes its own? */
let nest = (nodes, nestRender) => {
  nodes,
  links: [],
  constraints: [||],
  render: ({bbox: _, nodes}, _) => nestRender(nodes),
};