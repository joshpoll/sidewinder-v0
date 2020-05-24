open Theia;

let ex0 =
  seq(
    ~nodes=[str(~flowTag={flowNodeType: Leaf, uid: "0", rootPath: []}, "x", ()), str("y", ())],
    ~linkRender=None,
    ~gap=0.,
    ~direction=LeftRight,
    (),
  );

let ex1 =
  seq(
    ~nodes=[
      str("y", ()),
      str(~uid="0", ~flowTag={flowNodeType: Leaf, uid: "0", rootPath: []}, "x", ()),
    ],
    ~linkRender=None,
    ~gap=0.,
    ~direction=LeftRight,
    (),
  );

/* multiple copies */
let ex2 =
  seq(
    ~nodes=[
      str(
        ~flowTag={flowNodeType: Leaf, uid: "0", rootPath: []},
        /* ~flow={pat: ["0", "1", "2"], ext: []}, */
        "x",
        (),
      ),
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
      str(~flowTag={flowNodeType: Leaf, uid: "0", rootPath: []}, "x", ()),
      str("y", ()),
      str(~flowTag={flowNodeType: Leaf, uid: "1", rootPath: []}, "x", ()),
      str(~flowTag={flowNodeType: Leaf, uid: "2", rootPath: []}, "x", ()),
    ],
    ~linkRender=None,
    ~gap=0.,
    ~direction=LeftRight,
    (),
  );