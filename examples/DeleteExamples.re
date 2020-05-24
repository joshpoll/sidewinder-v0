open Theia;

let ex0 =
  seq(
    ~nodes=[str(~flowTag={flowNodeType: Leaf, uid: "0", rootPath: []}, "x", ()), str("y", ())],
    ~linkRender=None,
    ~gap=0.,
    ~direction=LeftRight,
    (),
  );

let ex1 = seq(~nodes=[str("y", ())], ~linkRender=None, ~gap=0., ~direction=LeftRight, ());