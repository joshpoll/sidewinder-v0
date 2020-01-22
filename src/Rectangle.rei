type t;

let make: (~x1: float, ~x2: float, ~y1: float, ~y2: float, unit) => t;

let cx: t => float;
let cy: t => float;

let width: t => float;
let height: t => float;

let union: (t, t) => t;