open Jest;
open TestUtil;

describe("hide", () => {
  Expect.(
    test("hides by tag in a sequence", () => {
      expect(
        Theia.seq(
          ~nodes=[Theia.str(~tags=["foo"], "", ()), Theia.str(~tags=["bar"], "", ())],
          ~linkRender=None,
          ~gap=0.,
          ~direction=LeftRight,
          (),
        )
        |> Transform.hide("foo"),
      )
      |> toEqual(
           Some(
             Theia.seq(
               ~nodes=[Theia.str(~tags=["bar"], "", ())],
               ~linkRender=None,
               ~gap=0.,
               ~direction=LeftRight,
               (),
             ),
           ),
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
          source: None,
          target: {
            ancestorRoot: 2, /* TODO: this shouldn't be hard coded. it depends on some other properties */
            absPath: [0] /* TODO: would be a lot easier to use global/local unique ids */,
          },
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
      expect(l |> Transform.denest("cons", "cons")) |> toEqual(denestL)
    })
  );
});