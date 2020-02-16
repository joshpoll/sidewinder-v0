/* /* Slide 22: https://courses.cs.washington.edu/courses/cse341/19sp/lec3slides.pdf */
   open Sidewinder;
   open Theia;

   let x = str("x", ());
   let y = str("y", ());
   let z = str("z", ());
   let cons = (~dxH, ~dyH, ~h, ~dxT, ~dyT, ~t) => {
     seq(
       ~nodes=[box(~dx=dxH, ~dy=dyH, h, [], ()), box(~dx=dxT, ~dy=dyT, t, [], ())],
       ~linkRender=None,
       ~gap=0.,
       ~direction=LeftRight,
     );
   };

   /* TODO: this seems to show the limitations of relative path definitions. They don't compose very
      well. But maybe it's fine the way it is! After all, it doesn't seem to break anything (see misc
      thoughts). */
   let xPtr0 =
     atom(
       ~links=[
         Link.{
           /* self */
           source: None,
           target: {
             ancestorRoot: 3, /* 0 = xPtr0, 1 = cons-box, 2 = cons-seq, 3 = graph */
             absPath: [1],
           },
           linkRender:
             Some(
               (~source, ~target) => {
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
                     x1={Js.Float.toString(source->Rectangle.cx)}
                     y1={Js.Float.toString(source->Rectangle.cy)}
                     x2={Js.Float.toString(target->Rectangle.x1)}
                     y2={Js.Float.toString(target->Rectangle.cy)}
                     stroke="purple"
                   />
                 </>
               },
             ),
         },
       ],
       <circle r="2" cx="2" cy="2" />,
       Rectangle.fromPointSize(~x=0., ~y=0., ~width=4., ~height=4.),
       (),
     );
   let xList0 =
     cons(
       ~dxH=(12.5 -. 9.) /. 2. +. 2.5 /. 2.,
       ~dyH=2.5 /. 2.,
       ~h=str("2", ()),
       ~dxT=11. /. 2.,
       ~dyT=11. /. 2.,
       ~t=xPtr0,
       (),
     );

   let xList1 =
     cons(
       ~dxH=(12.5 -. 9.) /. 2. +. 2.5 /. 2.,
       ~dyH=2.5 /. 2.,
       ~h=str("4", ()),
       ~dxT=(12.5 -. 9.) /. 2. +. 2.5 /. 2.,
       ~dyT=2.5 /. 2.,
       ~t=str("/", ()) /* TODO: maybe do a box with a slash through it instead */,
       (),
     );

   let xList =
     graph(
       ~nodes=[xList0, xList1],
       ~links=[],
       ~gap=25.,
       ~linkDistance=40.,
       ~constraints=directionConstraints(LeftRight),
       (),
     );

   let xBinding = [x, xList];

   let yPtr0 =
     atom(
       ~links=[
         Link.{
           /* self */
           source: None,
           target: {
             ancestorRoot: 3, /* 0 = yPtr0, 1 = cons-box, 2 = cons-seq, 3 = graph */
             absPath: [1],
           },
           linkRender:
             Some(
               (~source, ~target) => {
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
                     x1={Js.Float.toString(source->Rectangle.cx)}
                     y1={Js.Float.toString(source->Rectangle.cy)}
                     x2={Js.Float.toString(target->Rectangle.x1)}
                     y2={Js.Float.toString(target->Rectangle.cy)}
                     stroke="purple"
                   />
                 </>
               },
             ),
         },
       ],
       <circle r="2" cx="2" cy="2" />,
       Rectangle.fromPointSize(~x=0., ~y=0., ~width=4., ~height=4.),
       (),
     );
   let yList0 =
     cons(
       ~dxH=(12.5 -. 9.) /. 2. +. 2.5 /. 2.,
       ~dyH=2.5 /. 2.,
       ~h=str("5", ()),
       ~dxT=11. /. 2.,
       ~dyT=11. /. 2.,
       ~t=yPtr0,
       (),
     );

   let yPtr1 =
     atom(
       ~links=[
         Link.{
           /* self */
           source: None,
           target: {
             ancestorRoot: 3, /* 0 = yPtr1, 1 = cons-box, 2 = cons-seq, 3 = graph */
             absPath: [2],
           },
           linkRender:
             Some(
               (~source, ~target) => {
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
                     x1={Js.Float.toString(source->Rectangle.cx)}
                     y1={Js.Float.toString(source->Rectangle.cy)}
                     x2={Js.Float.toString(target->Rectangle.x1)}
                     y2={Js.Float.toString(target->Rectangle.cy)}
                     stroke="purple"
                   />
                 </>
               },
             ),
         },
       ],
       <circle r="2" cx="2" cy="2" />,
       Rectangle.fromPointSize(~x=0., ~y=0., ~width=4., ~height=4.),
       (),
     );

   let yList1 =
     cons(
       ~dxH=(12.5 -. 9.) /. 2. +. 2.5 /. 2.,
       ~dyH=2.5 /. 2.,
       ~h=str("3", ()),
       ~dxT=11. /. 2.,
       ~dyT=11. /. 2.,
       ~t=yPtr1,
       (),
     );

   let yList2 =
     cons(
       ~dxH=(12.5 -. 9.) /. 2. +. 2.5 /. 2.,
       ~dyH=2.5 /. 2.,
       ~h=str("0", ()),
       ~dxT=(12.5 -. 9.) /. 2. +. 2.5 /. 2.,
       ~dyT=2.5 /. 2.,
       ~t=str("/", ()) /* TODO: maybe do a box with a slash through it instead */,
       (),
     );

   let yList =
     graph(
       ~nodes=[yList0, yList1, yList2],
       ~links=[],
       ~gap=25.,
       ~linkDistance=40.,
       ~constraints=directionConstraints(LeftRight),
       (),
     );

   let yBinding = [y, yList];

   let zPtr0 =
     atom(
       ~links=[
         Link.{
           /* self */
           source: None,
           target: {
             ancestorRoot: 3, /* 0 = zPtr0, 1 = cons-box, 2 = cons-seq, 3 = graph */
             absPath: [1],
           },
           linkRender:
             Some(
               (~source, ~target) => {
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
                     x1={Js.Float.toString(source->Rectangle.cx)}
                     y1={Js.Float.toString(source->Rectangle.cy)}
                     x2={Js.Float.toString(target->Rectangle.x1)}
                     y2={Js.Float.toString(target->Rectangle.cy)}
                     stroke="purple"
                   />
                 </>
               },
             ),
         },
       ],
       <circle r="2" cx="2" cy="2" />,
       Rectangle.fromPointSize(~x=0., ~y=0., ~width=4., ~height=4.),
       (),
     );
   let zList0 =
     cons(
       ~dxH=(12.5 -. 9.) /. 2. +. 2.5 /. 2.,
       ~dyH=2.5 /. 2.,
       ~h=str("2", ()),
       ~dxT=11. /. 2.,
       ~dyT=11. /. 2.,
       ~t=zPtr0,
       (),
     );

   let zPtr1 =
     atom(
       ~links=[
         Link.{
           /* self */
           source: None,
           target: {
             ancestorRoot: 5, /* 0 = zPtr1, 1 = cons-box, 2 = cons-seq, 3 = graph-list, 4 = binding, 5
             = env */

             absPath: [1 /* yBinding */, 1 /* graph-list */, 0 /* hd */],
           },
           linkRender:
             Some(
               (~source, ~target) => {
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
                     x1={Js.Float.toString(source->Rectangle.cx)}
                     y1={Js.Float.toString(source->Rectangle.cy)}
                     x2={Js.Float.toString(target->Rectangle.x1)}
                     y2={Js.Float.toString(target->Rectangle.cy)}
                     stroke="purple"
                   />
                 </>
               },
             ),
         },
       ],
       <circle r="2" cx="2" cy="2" />,
       Rectangle.fromPointSize(~x=0., ~y=0., ~width=4., ~height=4.),
       (),
     );

   let zList1 =
     cons(
       ~dxH=(12.5 -. 9.) /. 2. +. 2.5 /. 2.,
       ~dyH=2.5 /. 2.,
       ~h=str("4", ()),
       ~dxT=11. /. 2.,
       ~dyT=11. /. 2.,
       ~t=zPtr1,
       (),
     );

   let zList =
     graph(
       ~nodes=[zList0, zList1],
       ~links=[],
       ~gap=25.,
       ~linkDistance=40.,
       ~constraints=directionConstraints(LeftRight),
       (),
     );

   /* (~source, ~target) =>
      <line
        x1={Js.Float.toString(source->Rectangle.x2)}
        y1={Js.Float.toString(source->Rectangle.cy)}
        x2={Js.Float.toString(target->Rectangle.x1)}
        y2={Js.Float.toString(target->Rectangle.cy)}
        stroke="black"
      /> */

   let zBinding = [z, zList];

   let env =
     table(
       ~nodes=[xBinding, yBinding, zBinding],
       ~linkRender=None,
       ~xGap=0.,
       ~yGap=0.,
       ~xDirection=LeftRight,
       ~yDirection=UpDown,
       (),
     ); */