open WebCoLa;

let cl = colaLayout();

cl
->nodes([|{x: 5., y: 6.}|])
->links([|{length: Some(5.), source: NN(0.), target: NN(0.)}|])
->avoidOverlaps(true)
->start(Some(10.), Some(15.), Some(20.), None);

Js.log(getNodes(cl));