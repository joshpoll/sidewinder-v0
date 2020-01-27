open Jest;

let dummyLayout = (_, _) => [];
let dummyComputeSize = _ => Node.{width: 0., height: 0.};

let dummyRender = (_, _, _) => <> </>;

let linkRender = (~source, ~target) => <> </>;

let make = (~nodes, ~links) =>
  SideWinder.make(
    ~nodes,
    ~links,
    ~layout=dummyLayout,
    ~computeSize=dummyComputeSize,
    ~render=dummyRender,
  );

let makeLCA = (~nodes, ~links) =>
  SideWinder.LCA.{
    nodes,
    links,
    layout: dummyLayout,
    computeSize: dummyComputeSize,
    render: dummyRender,
  };

describe("LCA", () => {
  /*
   nesting:
   b <- a
   d <- b
   d <- c

   links:
   a -> c
   */
  let a = make(~nodes=[], ~links=[]);
  let b =
    make(
      ~nodes=[a],
      ~links=[Link.{
                source: 0,
                target: {
                  ancestorRoot: 1,
                  absPath: [1],
                },
                linkRender,
              }],
    );
  let c = make(~nodes=[], ~links=[]);
  let d = make(~nodes=[b, c], ~links=[]);

  let a' = makeLCA(~nodes=[], ~links=[]);
  let b' = makeLCA(~nodes=[a'], ~links=[]);
  let c' = makeLCA(~nodes=[], ~links=[]);
  let d' = makeLCA(~nodes=[b', c'], ~links=[Link.{source: [0, 0], target: [1], linkRender}]);

  Expect.(
    test("bubbles one level in small example", () =>
      expect(d |> SideWinder.LCA.propagateLCA) |> toEqual(d')
    )
  );
});

describe("Expect.Operators", () => {
  open Expect;
  open! Expect.Operators;

  test("==", () =>
    expect(1 + 2) === 3
  );
});