/* Lowering Theia into Sidewinder */
/* TODO: should probably generate new ids for children, but ids aren't fully baked anyway */
/* TODO: some of this conversion is probably wrong or incomplete */

open SidewinderKernel;

let atom = (id, e) => {id, element: Atom(e)};

let makeEdges = l =>
  List.combine(List.rev(l) |> List.tl |> List.rev, List.tl(l))
  |> List.map(((source, target)) => {source, target});

let sequence = (id, nodes) => {
  id,
  element: Graph(nodes, makeEdges(List.map(e => [e.id], nodes))),
};

let rec interleave = (xs, ys) =>
  switch (xs, ys) {
  | ([], _) => ys
  | ([x, ...xs], _) => [x, ...interleave(ys, xs)]
  };

let apply = (id, ops, args) => sequence(id, interleave(ops, args));

let value = apply;

let kv = (id, k, v) => sequence(id, [k, v]);

let map = (id, kvs) => sequence(id, List.map(((k, v)) => kv(id, k, v), kvs));

type evalCtxt = {
  ops: list(node),
  args: list(node),
  holePos: int,
};

let split = (list, n) => {
  let rec aux = (i, acc) =>
    fun
    | [] => (List.rev(acc), [])
    | [h, ...t] as l =>
      if (i == 0) {
        (List.rev(acc), l);
      } else {
        aux(i - 1, [h, ...acc], t);
      };
  aux(n, [], list);
};

let insert = (x, xs, i) => {
  let (xs, ys) = split(xs, i);
  xs @ [x, ...ys];
};

let rec kont = (id, value, ecs) =>
  switch (ecs) {
  | [] => value
  | [{ops, args, holePos}, ...ecs] =>
    apply(id, ops, insert(kont(id, value, ecs), args, holePos))
  };

let cell = (id, name, nodes) => sequence(id, [name, sequence(id, nodes)]);