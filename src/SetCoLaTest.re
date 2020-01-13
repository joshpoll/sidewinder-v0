open SetCoLa;

let setcolaSpec = [|
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
|];

let graph = {
  nodes: [|{name: "a"}, {name: "b"}, {name: "c"}, {name: "d"}, {name: "e"}, {name: "f"}|],
  links: [|
    {source: NN(0), target: NN(1), length: None},
    {source: NN(0), target: NN(2), length: None},
    {source: NN(1), target: NN(3), length: None},
    {source: NN(2), target: NN(4), length: None},
    {source: NN(2), target: NN(5), length: None},
  |],
};

let result = setCola->nodes(graph.nodes)->links(graph.links)->constraints(setcolaSpec)->layout;

open WebCoLa;

let cola =
  colaLayout()
  ->nodes([|{x: 5., y: 6.}|])
  ->links([|{length: Some(5.), source: NN(0.), target: NN(0.)}|])
  ->avoidOverlaps(true)
  ->start(Some(10.), Some(15.), Some(20.), None);

Js.log(getNodes(cola));