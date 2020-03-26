type t('a) = list(list('a));

let fromListList = x => x;

let rec partitionAux = (acc, n, l) =>
  if (n == 0) {
    (List.rev(acc), l);
  } else {
    switch (l) {
    | [] => raise(failwith("ran out of elements to partition"))
    | [h, ...t] => partitionAux([h, ...acc], n - 1, t)
    };
  };

let partition = (n, l) => partitionAux([], n, l);

let rec fromList = (l, rowLen) =>
  switch (l) {
  | [] => []
  | l =>
    let (h, t) = partition(rowLen, l);
    [h, ...fromList(t, rowLen)];
  };

let toListList = x => x;

/* http://blog.shaynefletcher.org/2017/08/transpose.html */
let rec transposeAux = (acc, ls) =>
  switch (ls) {
  | []
  | [[], ..._] => List.rev(acc)
  | ls => transposeAux([List.map(List.hd, ls), ...acc], List.map(List.tl, ls))
  };

let transpose = ls => transposeAux([], ls);

let map = (f, m) => List.map(List.map(f), m);