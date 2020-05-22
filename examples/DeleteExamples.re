open Theia;

let ex0 =
  seq(
    ~nodes=[str(~uid="0", ~flow=Flow([]), "x", ()), str("y", ())],
    ~linkRender=None,
    ~gap=0.,
    ~direction=LeftRight,
    (),
  );

let ex1 = seq(~nodes=[str("y", ())], ~linkRender=None, ~gap=0., ~direction=LeftRight, ());