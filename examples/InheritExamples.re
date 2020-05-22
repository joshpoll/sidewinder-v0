open Theia;
open Flow;

let hSeq = (~uid=?, ~flow=Untracked, ~gap=0., nodes) =>
  seq(~uid?, ~flow, ~nodes, ~linkRender=None, ~gap, ~direction=LeftRight, ());

let paren = (~uid=?, ~flow=Untracked, x) =>
  hSeq(~uid?, ~flow, [str(~flow=Inherit, "(", ()), x, str(~flow=Inherit, ")", ())]);

let ex0 =
  seq(
    ~nodes=[
      paren(
        ~uid="foo",
        ~flow=Flow(["foo"]),
        str(~uid="0", ~flow=Flow(["0", "1", "2"]), "x", ()),
      ),
      str("y", ()),
    ],
    ~linkRender=None,
    ~gap=0.,
    ~direction=LeftRight,
    (),
  );

let ex1 =
  seq(
    ~nodes=[
      str(~uid="0", "x", ()),
      paren(~uid="foo", str("yz", ())),
      str(~uid="1", "x", ()),
      str(~uid="2", "x", ()),
    ],
    ~linkRender=None,
    ~gap=0.,
    ~direction=LeftRight,
    (),
  );