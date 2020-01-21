open SidewinderKernel;

let atom = e => {nodes: [], edges: [], constraints: [||], nodeRender: _ => e};

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

let defaultRender = ({renderedNodes, renderedEdges}) => {
  let renderEdge = ({source, target, edgeRender}) => {
    let sourceNode = List.nth(renderedNodes, source);
    let targetNode = List.nth(renderedNodes, target);
    edgeRender({x1: sourceNode.x, y1: sourceNode.y, x2: targetNode.x, y2: targetNode.y});
  };
  <div style={ReactDOMRe.Style.make(~display="inline-block", ~position="absolute", ())}>
    <div style={ReactDOMRe.Style.make(~display="inline-block", ~position="absolute", ())}>
      {List.map(
         (n: WebCoLa.node({. rendered: React.element})) =>
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
         renderedNodes,
       )
       |> Array.of_list
       |> React.array}
    </div>
    <div style={ReactDOMRe.Style.make(~display="inline-block", ())}>
      {List.map(renderEdge, renderedEdges) |> Array.of_list |> React.array}
    </div>
  </div>;
};

let sequence = (nodes, edgeRender, direction) => {
  nodes,
  edges: makeEdges(List.length(nodes) |> range(0), edgeRender),
  constraints: directionConstraints(direction),
  nodeRender: defaultRender,
};

/* NOTE! Doesn't provide any edge connections. */
/* TODO: Could allow edges, just don't render them. */
let nest = (nodes, nestRender) => {
  nodes,
  edges: [],
  constraints: [||],
  nodeRender: ({renderedNodes, renderedEdges: _}) => nestRender(renderedNodes),
};