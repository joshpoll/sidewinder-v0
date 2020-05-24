module MS = Belt.Map.String;

/* type flow = {
     pat: list(Node.uid),
     ext: list(Node.uid),
   };

   type t = option(flow);

   let merge = (f1: t, f2: t): t =>
     switch (f1, f2) {
     | (Some({pat: pat1, ext: ext1}), Some({pat: pat2, ext: ext2})) =>
       Some({pat: pat1 @ pat2, ext: ext1 @ ext2})
     | (Some(f1), None) => Some(f1)
     | (None, Some(f2)) => Some(f2)
     | (None, None) => None
     }; */

type t = MS.t(list(FlowTag.uid));

let fromArray = MS.fromArray;

let union = (m1: t, m2: t) => {
  MS.reduce(m2, m1, (m, k, v) =>
    m->MS.update(k, v' =>
      switch (v') {
      | None => Some(v)
      | Some(v') => Some(v @ v')
      }
    )
  );
};