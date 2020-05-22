type t =
  | Flow(list(Node.uid))
  | Inherit
  | Untracked;

let merge = (f1: t, f2: t): t =>
  switch (f1, f2) {
  // | (Untracked, Untracked) => Untracked
  | (Flow(f1), Flow(f2)) => Flow(f1 @ f2)
  | (Flow(f1), Inherit) => Flow(f1)
  | (Inherit, Flow(f2)) => Flow(f2)
  | _ => Untracked /* TODO: this might be wrong */
  };