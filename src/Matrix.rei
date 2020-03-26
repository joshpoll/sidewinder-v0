type t('a);

let fromListList: list(list('a)) => t('a);

let fromList: (list('a), int) => t('a);

let toListList: t('a) => list(list('a));

let transpose: t('a) => t('a);

let map: ('a => 'b, t('a)) => t('b);