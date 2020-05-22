/* type t =
   | Flow(list(Node.uid))
   | Inherit
   | Untracked; */

type flow =
  /* flow created by a pattern */
  | Pattern(Node.uid)
  /* flow created by an external function */
  | External(Node.uid)
  /* flow created by a relation */
  | Relation;

type t = option(list(flow));

let merge = (f1: t, f2: t): t =>
  switch (f1, f2) {
  | (Some(f1), Some(f2)) => Some(f1 @ f2)
  | (Some(f1), None) => Some(f1)
  | (None, Some(f2)) => Some(f2)
  | (None, None) => None
  };