open WebCoLa;

let cl = colaLayout();

cl
->nodes([|{
            x: 5.,
            y: 6.,
            width: 10.,
            height: 10.,
            temp: false,
            custom: {
              "name": "foo",
            },
          }|])
->links([|{length: Some(5.), source: NN(0.), target: NN(0.)}|])
->avoidOverlaps(true)
->start(Some(10.), Some(15.), Some(20.), None);

Js.log(getNodes(cl));