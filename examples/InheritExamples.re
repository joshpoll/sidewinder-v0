open Theia;

let hSeq = (~uid=?, ~flow=None, ~gap=0., nodes) =>
  seq(~uid?, ~flow, ~nodes, ~linkRender=None, ~gap, ~direction=LeftRight, ());

let paren = (~uid=?, ~flow=None, x) =>
  hSeq(~uid?, ~flow, [str(~flow=None, "(", ()), x, str(~flow=None, ")", ())]);

let ex0 =
  seq(
    ~nodes=[
      paren(
        ~uid="foo",
        ~flow=Some([Pattern("foo")]),
        str(~uid="0", ~flow=Some([Pattern("0"), Pattern("1"), Pattern("2")]), "x", ()),
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