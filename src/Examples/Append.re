/* Slide 22: https://courses.cs.washington.edu/courses/cse341/19sp/lec3slides.pdf */
open Theia;

let x = str("x");
let y = str("y");
let z = str("z");
let cons = (~dxH, ~dyH, ~h, ~dxT, ~dyT, ~t) => {
  seq(
    ~nodes=[box(~dx=dxH, ~dy=dyH, [h], []), box(~dx=dxT, ~dy=dyT, [t], [])],
    ~linkRender=(~source, ~target) => <> </>,
    ~gap=0.,
    ~direction=LeftRight,
  );
};

/* TODO: this seems to show the limitations of relative path definitions. They don't compose very
   well. But maybe it's fine the way it is! */
let xPtr =
  atom(
    ~links=[
      Link.{
        /* self */
        source: None,
        target: {
          ancestorRoot: 3, /* 0 = xPtr, 1 = cons-box, 2 = cons-seq, 3 = graph */
          absPath: [1],
        },
        linkRender: (~source, ~target) => {
          <>
            {if (debug_) {
               <>
                 {drawBBox(Node.sizeOffsetToBBox(source))}
                 {drawBBox(Node.sizeOffsetToBBox(target))}
               </>;
             } else {
               <> </>;
             }}
            <line
              x1={Js.Float.toString(source->Rectangle.x2)}
              y1={Js.Float.toString(source->Rectangle.cy)}
              x2={Js.Float.toString(target->Rectangle.x1)}
              y2={Js.Float.toString(target->Rectangle.cy)}
              stroke="purple"
            />
          </>;
        },
      },
    ],
    <circle r="2" cx="2" cy="2" />,
    Rectangle.fromPointSize(~x=0., ~y=0., ~width=4., ~height=4.),
  );
let xList0 =
  cons(
    ~dxH=(12.5 -. 9.) /. 2. +. 2.5 /. 2.,
    ~dyH=2.5 /. 2.,
    ~h=str("2"),
    ~dxT=11. /. 2.,
    ~dyT=11. /. 2.,
    ~t=xPtr,
  );

let xList1 =
  cons(
    ~dxH=(12.5 -. 9.) /. 2. +. 2.5 /. 2.,
    ~dyH=2.5 /. 2.,
    ~h=str("4"),
    ~dxT=(12.5 -. 9.) /. 2. +. 2.5 /. 2.,
    ~dyT=2.5 /. 2.,
    ~t=str("/") /* TODO: maybe do a box with a slash through it instead */,
  );

let xList =
  graph(
    ~nodes=[xList0, xList1],
    ~links=[],
    ~gap=25.,
    ~linkDistance=40.,
    ~constraints=directionConstraints(LeftRight),
  );

let xBinding =
  seq(
    ~nodes=[x, xList],
    ~linkRender=
      (~source, ~target) =>
        <line
          x1={Js.Float.toString(source->Rectangle.x2)}
          y1={Js.Float.toString(source->Rectangle.cy)}
          x2={Js.Float.toString(target->Rectangle.x1)}
          y2={Js.Float.toString(target->Rectangle.cy)}
          stroke="black"
        />,
    ~gap=30.,
    ~direction=LeftRight,
  );