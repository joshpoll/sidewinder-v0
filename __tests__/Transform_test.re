/* open Jest;
   open TestUtil;

   describe("hide", () => {
     Expect.(
       test("hides by tag in a sequence", () => {
         expect(
           Theia.seq(
             ~nodes=[makeAtom(["foo"]), makeAtom(["bar"])],
             ~linkRender=None,
             ~gap=0.,
             ~direction=LeftRight,
             (),
           )
           |> Abstract.hide("foo")
           |> Belt.Option.getExn
           |> kernelToStructure,
         )
         |> toEqual(
              Theia.seq(
                ~nodes=[makeAtom(["bar"])],
                ~linkRender=None,
                ~gap=0.,
                ~direction=LeftRight,
                (),
              )
              |> kernelToStructure,
            )
       })
     )
   });

   describe("denest", () => {
     /* [2, 4] */
     let l =
       make(
         ~tags=["cons"],
         ~nodes=[
           makeAtom(["2"]),
           make(~tags=["cons"], ~nodes=[makeAtom(["4"]), makeAtom(["/"])], ~links=[]),
         ],
         ~links=[],
       );

     let ptr =
       Theia.atom(
         ~tags=["denest ptr"],
         ~links=[],
         <circle r="2" cx="2" cy="2" />,
         Rectangle.fromPointSize(~x=0., ~y=0., ~width=4., ~height=4.),
         (),
       );

     let denestL = {
       let tgtNode = make(~tags=["cons"], ~nodes=[makeAtom(["4"]), makeAtom(["/"])], ~links=[]);
       Theia.graph(
         ~tags=["denest"],
         ~nodes=[make(~tags=["cons"], ~nodes=[makeAtom(["2"]), ptr], ~links=[]), tgtNode],
         ~links=[
           Link.{
             source: ptr.uid,
             target: tgtNode.uid,
             linkRender:
               Some(
                 (~source, ~target) => {
                   <line
                     x1={Js.Float.toString(source->Rectangle.cx)}
                     y1={Js.Float.toString(source->Rectangle.cy)}
                     x2={Js.Float.toString(target->Rectangle.x1)}
                     y2={Js.Float.toString(target->Rectangle.cy)}
                     stroke="purple"
                   />
                 },
               ),
           },
         ],
         ~gap=25.,
         ~linkDistance=40.,
         ~constraints=Theia.directionConstraints(LeftRight),
         (),
       );
     };

     Expect.(
       test("denest linear cons structure", () =>
         expect(l |> Abstract.denest("cons", "cons") |> kernelToStructure)
         |> toEqual(denestL |> kernelToStructure)
       )
     );

     Expect.(
       test("original structure is LCA", () =>
         expect(l |> kernelToStructure)
         |> toEqual(l |> LCA.fromKernel |> LCA.toKernel |> kernelToStructure)
       )
     );

     Expect.(
       test("denested structure is LCA", () =>
         expect(denestL |> kernelToStructure)
         |> toEqual(denestL |> LCA.fromKernel |> LCA.toKernel |> kernelToStructure)
       )
     );
   }); */