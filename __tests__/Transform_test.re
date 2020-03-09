open Jest;
open TestUtil;

describe("hide", () => {
  Expect.(
    test("hides by tag in a sequence", () => {
      expect(
        Theia.seq(
          ~nodes=[
            make(~tags=["foo"], ~nodes=[], ~links=[]),
            make(~tags=["bar"], ~nodes=[], ~links=[]),
          ],
          ~linkRender=None,
          ~gap=0.,
          ~direction=LeftRight,
          (),
        )
        |> Transform.hide("foo")
        |> Belt.Option.getExn
        |> kernelToStructure,
      )
      |> toEqual(
           Theia.seq(
             ~nodes=[make(~tags=["bar"], ~nodes=[], ~links=[])],
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
        make(~tags=["2"], ~nodes=[], ~links=[]),
        make(
          ~tags=["cons"],
          ~nodes=[
            make(~tags=["4"], ~nodes=[], ~links=[]),
            make(~tags=["/"], ~nodes=[], ~links=[]),
          ],
          ~links=[],
        ),
      ],
      ~links=[],
    );

  let ptr =
    Theia.atom(
      ~tags=["denest ptr"],
      ~links=[
        Link.{
          source: "0",
          target: "1",
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
      <circle r="2" cx="2" cy="2" />,
      Rectangle.fromPointSize(~x=0., ~y=0., ~width=4., ~height=4.),
      (),
    );

  let denestL =
    Theia.graph(
      ~tags=["denest"],
      ~nodes=[
        make(
          ~tags=["cons"],
          ~nodes=[make(~tags=["2"], ~nodes=[], ~links=[]), ptr],
          ~links=[],
        ),
        make(
          ~tags=["cons"],
          ~nodes=[
            make(~tags=["4"], ~nodes=[], ~links=[]),
            make(~tags=["/"], ~nodes=[], ~links=[]),
          ],
          ~links=[],
        ),
      ],
      ~links=[],
      ~gap=25.,
      ~linkDistance=40.,
      ~constraints=Theia.directionConstraints(LeftRight),
      (),
    );

  Expect.(
    test("denest linear cons structure", () => {
      expect(l |> Transform.denest("cons", "cons") |> kernelToStructure)
      |> toEqual(denestL |> kernelToStructure)
    })
  );
});