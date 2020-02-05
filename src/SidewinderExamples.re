/* The problem with my current layout stuff is that I can't agree on whether the center is at the
   top or the middle of the space! */
/* NOTE: everything should be rendered so that its bounding box starts at the origin in the top left
   corner. */
open Sidewinder;
open Theia;

let a = str("2");
let a' = box(~dx=(12.5 -. 9.) /. 2. +. 2.5 /. 2., ~dy=2.5 /. 2., [a], []);

let b =
  atom(
    ~links=[
      Link.{
        /* b */
        source: None,
        /* f */
        target: {
          ancestorRoot: 3, /* 0 = b, 1 = b', 2 = e, 3 = g */
          absPath: [1 /* 1, 0 */],
        },
        linkRender: (~source, ~target) => {
          <>
            {if (debug_) {
               <>
                 {drawBBox(Sidewinder.Node.sizeOffsetToBBox(source))}
                 {drawBBox(Sidewinder.Node.sizeOffsetToBBox(target))}
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
let b' = box(~dx=11. /. 2., ~dy=11. /. 2., [b], []);

let c = str("4");
let c' = box(~dx=(12.5 -. 9.) /. 2. +. 2.5 /. 2., ~dy=2.5 /. 2., [c], []);

let d = str("/");
let d' = box(~dx=(12.5 -. 9.) /. 2. +. 2.5 /. 2., ~dy=2.5 /. 2., [d], []);

let e =
  seq(
    ~nodes=[a', b'],
    ~linkRender=(~source: _, ~target: _) => <> </>,
    ~gap=0.,
    ~direction=LeftRight,
  );

let f =
  seq(~nodes=[c', d'], ~linkRender=(~source, ~target) => <> </>, ~gap=0., ~direction=LeftRight);

let g =
  graph(
    ~nodes=[e, f],
    ~links=[],
    ~gap=25.,
    ~linkDistance=40.,
    ~constraints=directionConstraints(LeftRight),
  );

/**
       * AST Example: https://courses.cs.washington.edu/courses/cse341/19sp/lec5slides.pdf
       */
let linkRender = (~source, ~target) => {
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
      x1={Js.Float.toString(source->Rectangle.cx)}
      y1={Js.Float.toString(source->Rectangle.y2)}
      x2={Js.Float.toString(target->Rectangle.cx)}
      y2={Js.Float.toString(target->Rectangle.y1)}
      stroke="black"
    />
  </>;
};

let ast0 = str("Add");
let ast1 = str("Constant");
let ast2 = str("Negate");
let ast3 = str("19");
let ast4 = str("Constant");
let ast5 = str("4");

let astExample =
  graph(
    ~nodes=[ast0, ast1, ast2, ast3, ast4, ast5],
    ~links=[
      Link.{
        /* Add -> Constant */
        source: Some(0),
        target: {
          ancestorRoot: 0,
          absPath: [1],
        },
        linkRender,
      },
      Link.{
        /* Add -> Negate */
        source: Some(0),
        target: {
          ancestorRoot: 0,
          absPath: [2],
        },
        linkRender,
      },
      Link.{
        /* Constant -> 19 */
        source: Some(1),
        target: {
          ancestorRoot: 0,
          absPath: [3],
        },
        linkRender,
      },
      Link.{
        /* Negate -> Constant */
        source: Some(2),
        target: {
          ancestorRoot: 0,
          absPath: [4],
        },
        linkRender,
      },
      Link.{
        /* Constant -> 4 */
        source: Some(4),
        target: {
          ancestorRoot: 0,
          absPath: [5],
        },
        linkRender,
      },
    ],
    ~gap=20.,
    ~linkDistance=80.,
    ~constraints=directionConstraints(UpDown),
  );