open Jest;

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