open Jest;
open TestUtil;

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
      ~links=[Sidewinder.Link.{source: "0", target: "2", linkRender}],
    );
  let c = make(~tags=[], ~nodes=[], ~links=[]);
  let d = make(~tags=[], ~nodes=[b, c], ~links=[]);

  let a' = makeLCA(~nodes=[], ~links=[]);
  let b' = makeLCA(~nodes=[a'], ~links=[]);
  let c' = makeLCA(~nodes=[], ~links=[]);
  let d' =
    makeLCA(~nodes=[b', c'], ~links=[Sidewinder.Link.{source: "1", target: "3", linkRender}]);

  Expect.(
    test("bubbles one level in small example", () =>
      expect(d |> Sidewinder.LCA.fromKernel |> lcaToStructure) |> toEqual(d' |> lcaToStructure)
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