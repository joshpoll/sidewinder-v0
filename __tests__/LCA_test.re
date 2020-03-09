open Jest;
open TestUtil;

/* TODO: to appropriately test this, need to compare layout links as well */

describe("LCA", () => {
  /*
   nesting:
   b <- a
   d <- b
   d <- c

   links:
   a -> c
   */
  let a = make(~tags=["a"], ~nodes=[], ~links=[]);
  let c = make(~tags=["c"], ~nodes=[], ~links=[]);
  let b =
    make(
      ~tags=["b"],
      ~nodes=[a],
      ~links=[Sidewinder.Link.{source: a.uid, target: c.uid, linkRender}],
    );
  let d = make(~tags=["d"], ~nodes=[b, c], ~links=[]);

  let a' = make(~tags=["a"], ~nodes=[], ~links=[]);
  let b' = make(~tags=["b"], ~nodes=[a'], ~links=[]);
  let c' = make(~tags=["c"], ~nodes=[], ~links=[]);
  let d' =
    make(
      ~tags=["d"],
      ~nodes=[b', c'],
      ~links=[Sidewinder.Link.{source: a'.uid, target: c'.uid, linkRender}],
    );

  Expect.(
    test("bubbles one level in small example", () => {
      expect(d |> Sidewinder.LCA.fromKernel |> Sidewinder.LCA.toKernel |> kernelToStructure)
      |> toEqual(d' |> Sidewinder.LCA.fromKernel |> Sidewinder.LCA.toKernel |> kernelToStructure)
    })
  );

  let seq =
    make(
      ~tags=["seq"],
      ~nodes=[a, c],
      ~links=[Sidewinder.Link.{source: a.uid, target: c.uid, linkRender}],
    );

  Expect.(
    test("sequence stays put", () => {
      expect(seq |> Sidewinder.LCA.fromKernel |> Sidewinder.LCA.toKernel |> kernelToStructure)
      |> toEqual(seq |> kernelToStructure)
    })
  );
});