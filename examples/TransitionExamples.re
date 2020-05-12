open Theia;

let ex0 =
  seq(
    ~nodes=[str(~uid="0", ~flow=["0"], "x", ()), str("y", ())],
    ~linkRender=None,
    ~gap=0.,
    ~direction=LeftRight,
    (),
  );

let ex1 =
  seq(
    ~nodes=[str("y", ()), str(~uid="0", ~flow=["0"], "x", ())],
    ~linkRender=None,
    ~gap=0.,
    ~direction=LeftRight,
    (),
  );