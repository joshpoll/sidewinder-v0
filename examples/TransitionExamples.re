open Theia;

let ex0 =
  seq(
    ~nodes=[str(~uid="0", ~flow=Some([Pattern("0")]), "x", ()), str("y", ())],
    ~linkRender=None,
    ~gap=0.,
    ~direction=LeftRight,
    (),
  );

let ex1 =
  seq(
    ~nodes=[str("y", ()), str(~uid="0", ~flow=Some([Pattern("0")]), "x", ())],
    ~linkRender=None,
    ~gap=0.,
    ~direction=LeftRight,
    (),
  );

/* multiple copies */
let ex2 =
  seq(
    ~nodes=[
      str(~uid="0", ~flow=Some([Pattern("0"), Pattern("1"), Pattern("2")]), "x", ()),
      str("y", ()),
    ],
    ~linkRender=None,
    ~gap=0.,
    ~direction=LeftRight,
    (),
  );

let ex3 =
  seq(
    ~nodes=[
      str(~uid="0", "x", ()),
      str("y", ()),
      str(~uid="1", "x", ()),
      str(~uid="2", "x", ()),
    ],
    ~linkRender=None,
    ~gap=0.,
    ~direction=LeftRight,
    (),
  );