// Entry point

[@bs.val] external document: Js.t({..}) = "document";

// We're using raw DOM manipulations here, to avoid making you read
// ReasonReact when you might precisely be trying to learn it for the first
// time through the examples later.
let style = document##createElement("style");
document##head##appendChild(style);
style##innerHTML #= ExampleStyles.style;

let makeContainer = text => {
  let container = document##createElement("div");
  container##className #= "container";

  let title = document##createElement("div");
  title##className #= "containerTitle";
  title##innerText #= text;

  let content = document##createElement("div");
  content##className #= "containerContent";

  let () = container##appendChild(title);
  let () = container##appendChild(content);
  let () = document##body##appendChild(container);

  content;
};

// All 4 examples.
ReactDOMRe.render(
  <BlinkingGreeting> {React.string("Hello!")} </BlinkingGreeting>,
  makeContainer("Blinking Greeting"),
);

ReactDOMRe.render(<ReducerFromReactJSDocs />, makeContainer("Reducer From ReactJS Docs"));

ReactDOMRe.render(<FetchedDogPictures />, makeContainer("Fetched Dog Pictures"));

ReactDOMRe.render(<ReasonUsingJSUsingReason />, makeContainer("Reason Using JS Using Reason"));

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
  ->nodes(result.nodes)
  ->links(result.links)
  ->avoidOverlaps(true)
  ->start(Some(10.), Some(15.), Some(20.), None);

Js.log(getNodes(cola));

ReactDOMRe.render(<Visualize nodes={getNodes(cola)} links=None />, makeContainer("Sidewinder"));