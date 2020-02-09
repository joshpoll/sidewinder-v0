open Jest;

let dummyLayout = (_, _) => [];
let dummyComputeSizeOffset = _ => Rectangle.fromPointSize(~x=0., ~y=0., ~width=0., ~height=0.);

let dummyRender = (_, _, _) => <> </>;

let linkRender = (~source, ~target) => <> </>;

let make = (~nodes, ~links) =>
  Main.make(
    ~nodes,
    ~links,
    ~layout=dummyLayout,
    ~computeSizeOffset=dummyComputeSizeOffset,
    ~render=dummyRender,
  );

let makeLCA = (~nodes, ~links) =>
  LCA.{
    nodes,
    links,
    layout: dummyLayout,
    computeSizeOffset: dummyComputeSizeOffset,
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
  let a = make(~tags=[], ~nodes=[], ~links=[]);
  let b =
    make(
      ~tags=[],
      ~nodes=[a],
      ~links=[
        Sidewinder.Link.{
          source: Some(0),
          target: {
            ancestorRoot: 1,
            absPath: [1],
          },
          linkRender,
        },
      ],
    );
  let c = make(~tags=[], ~nodes=[], ~links=[]);
  let d = make(~tags=[], ~nodes=[b, c], ~links=[]);

  let a' = makeLCA(~nodes=[], ~links=[]);
  let b' = makeLCA(~nodes=[a'], ~links=[]);
  let c' = makeLCA(~nodes=[], ~links=[]);
  let d' =
    makeLCA(
      ~nodes=[b', c'],
      ~links=[Sidewinder.Link.{source: [0, 0], target: [1], linkRender}],
    );

  Expect.(
    test("bubbles one level in small example", () =>
      expect(d |> Sidewinder.LCA.propagateLCA) |> toEqual(d')
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