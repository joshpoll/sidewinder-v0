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
/*
 open SetCoLa;
 /*   "constraintDefinitions": [
        {
          "name": "layer",
          "sets": {"partition": "depth"},
          "forEach": [{ "constraint": "align", "axis": "x" }]
        },
        {
          "sets": ["layer"],
          "forEach": [{ "constraint": "order", "axis": "y", "by": "depth", "reverse": true }]
        }
      ] */
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
     "forEach": [|{"constraint": "order", "axis": "y", "by": "depth", "reverse": true}|],
   }),
 |];

 let graph = {
   nodes: [|
     {
       width: 10.,
       height: 10.,
       custom: {
         "name": "a",
       },
     },
     {
       width: 10.,
       height: 10.,
       custom: {
         "name": "b",
       },
     },
     {
       width: 10.,
       height: 10.,
       custom: {
         "name": "c",
       },
     },
     {
       width: 10.,
       height: 10.,
       custom: {
         "name": "d",
       },
     },
     {
       width: 10.,
       height: 10.,
       custom: {
         "name": "e",
       },
     },
     {
       width: 10.,
       height: 10.,
       custom: {
         "name": "f",
       },
     },
   |],
   links: [|
     {source: NN(0), target: NN(1), length: None},
     {source: NN(0), target: NN(2), length: None},
     {source: NN(1), target: NN(3), length: None},
     {source: NN(2), target: NN(4), length: None},
     {source: NN(2), target: NN(5), length: None},
   |],
 };

 let result =
   setCola->nodes(graph.nodes)->links(graph.links)->constraints(setcolaSpec)->gap(40.)->layout;

 Js.log(result.constraints);

 open WebCoLa;

 let cola =
   colaLayout()
   ->nodes(result.nodes)
   ->links(result.links)
   ->constraints(result.constraints)
   ->avoidOverlaps(true)
   ->linkDistance(50.)
   ->start(Some(50.), Some(100.), Some(200.), None);
 // ->start(Some(50.), Some(100.), Some(200.), None);

 Js.log(getNodes(cola));
 Js.log(getLinks(cola));
  */
/* TODO: use renderSW in Visualize on a small example */

/* ReactDOMRe.render(
     <Visualize nodes={getNodes(cola)} links={getLinks(cola)} />,
     makeContainer("Sidewinder"),
   ); */

/* Js.log(result.nodes);
   Js.log(result.links); */

/* ReactDOMRe.render(
     <Visualize2 node=SidewinderExamples.g width=500. height=500. />,
     makeContainer("Sidewinder2"),
   ); */

ReactDOMRe.render(
  <Visualize3 node=SidewinderExamples.g width=500. height=500. />,
  makeContainer("Sidewinder"),
);