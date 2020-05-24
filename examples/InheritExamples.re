open Theia;
open Flow;

let hSeq = (~uid=?, ~flowTag=?, ~gap=0., nodes) =>
  seq(~uid?, ~flowTag?, ~nodes, ~linkRender=None, ~gap, ~direction=LeftRight, ());

let parenList = (~flowUID as uid, x) => [
  str(~flowTag={flowNodeType: Leaf, uid, rootPath: [0]}, "(", ()),
  x,
  str(~flowTag={flowNodeType: Leaf, uid, rootPath: [2]}, ")", ()),
];

let paren = (~flowUID as uid, x) =>
  seq(
    ~flowTag={flowNodeType: Dummy, uid, rootPath: []},
    ~nodes=[
      str(~flowTag={flowNodeType: Leaf, uid, rootPath: [0]}, "(", ()),
      x,
      str(~flowTag={flowNodeType: Leaf, uid, rootPath: [2]}, ")", ()),
    ],
    ~linkRender=None,
    ~gap=0.,
    ~direction=LeftRight,
    (),
  );

let ex0 =
  seq(
    ~flowTag={flowNodeType: Dummy, uid: "foo", rootPath: []},
    ~nodes=
      parenList(
        ~flowUID="foo",
        str(
          ~flowTag={flowNodeType: Leaf, uid: "0", rootPath: []} /* , ~flow={pat: ["0", "1", "2"], ext: []} */,
          "x",
          (),
        ),
      )
      @ [str("y", ())],
    ~linkRender=None,
    ~gap=0.,
    ~direction=LeftRight,
    (),
  );

let ex1 =
  seq(
    ~flowTag={flowNodeType: Dummy, uid: "foo", rootPath: []},
    ~nodes=
      parenList(
        ~flowUID="foo",
        str(
          ~flowTag={flowNodeType: Leaf, uid: "0", rootPath: []} /* , ~flow={pat: ["0", "1", "2"], ext: []} */,
          "xz",
          (),
        ),
      )
      @ [str("y", ())],
    ~linkRender=None,
    ~gap=0.,
    ~direction=LeftRight,
    (),
  );