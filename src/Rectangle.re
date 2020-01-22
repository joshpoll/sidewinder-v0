/**
 * Rectangle module based heavily on a subset of
 * https://github.com/tgdwyer/WebCola/blob/master/WebCola/src/rectangle.ts.
 *
 * Used to
 *   - compute bounding boxes for rendering
 *   - aid arrow rendering?
 */;

type t = {
  x1: float,
  x2: float,
  y1: float,
  y2: float,
};

let make = (~x1, ~x2, ~y1, ~y2, ()) => {x1, x2, y1, y2};

let cx = ({x1, x2}) => x1 +. x2 /. 2.;
let cy = ({y1, y2}) => y1 +. y2 /. 2.;

let width = ({x1, x2}) => x2 -. x1;
let height = ({y1, y2}) => y2 -. y1;

let union = ({x1, x2, y1, y2}, {x1: x1', x2: x2', y1: y1', y2: y2'}) => {
  x1: min(x1, x1'),
  x2: max(x2, x2'),
  y1: min(y1, y1'),
  y2: max(y2, y2'),
};