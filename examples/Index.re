// Entry point

[@bs.val] external document: Js.t({..}) = "document";

// We're using raw DOM manipulations here, to avoid making you read
// ReasonReact when you might precisely be trying to learn it for the first
// time through the examples later.
/* let style = document##createElement("style");
   document##head##appendChild(style);
   style##innerHTML #= ExampleStyles.style; */

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

/* ReactDOMRe.render(
     <Visualize3 node=SidewinderExamples.g width=500. height=300. />,
     makeContainer("linked list"),
   );

   ReactDOMRe.render(
     <Visualize3 node=SidewinderExamples.astExample width=500. height=300. />,
     makeContainer("AST"),
   );

   ReactDOMRe.render(
     <Visualize3 node=Append.env width=500. height=300. />,
     makeContainer("extended linked list example"),
   );

   ReactDOMRe.render(
     <Visualize3 node=AppendTable.env width=500. height=300. />,
     makeContainer("append table"),
   ); */

/* ReactDOMRe.render(
     <Visualize3 node={Append.seqTest()} width=500. height=300. />,
     makeContainer("append"),
   );

   ReactDOMRe.render(
     <Visualize3 node=Append.env width=500. height=300. />,
     makeContainer("append2"),
   );

   ReactDOMRe.render(
     <Visualize3 node=Append.tableTest width=500. height=300. />,
     makeContainer("append2.5"),
   );

   ReactDOMRe.render(
     <Visualize3 node=Append.seqTableTest width=500. height=300. />,
     makeContainer("append3"),
   );

   ReactDOMRe.render(
     <Visualize3 node=Append.seqSeqTest width=500. height=300. />,
     makeContainer("append3"),
   ); */

ReactDOMRe.render(
  React.createElement(Card.make, Card.makeProps()),
  makeContainer("spring test"),
);

ReactDOMRe.render(
  <Visualize3 node=TableExamples.ex0 width=500. height=300. debug=true />,
  makeContainer("table0"),
);

ReactDOMRe.render(
  <VisualizeTransition
    node=TransitionExamples.ex0
    nodeNext=TransitionExamples.ex1
    width=500.
    height=300.
    lowerFlow=true
    /* debug=true */
  />,
  makeContainer("transition0"),
);

ReactDOMRe.render(
  <VisualizeTransition
    node=DeleteExamples.ex0
    nodeNext=DeleteExamples.ex1
    width=500.
    height=300.
    lowerFlow=true
    /* debug=true */
  />,
  makeContainer("delete0"),
);

ReactDOMRe.render(
  <VisualizeTransition
    node=TransitionExamples.ex2
    nodeNext=TransitionExamples.ex3
    width=500.
    height=300.
    lowerFlow=true
    /* debug=true */
  />,
  makeContainer("transition1"),
);