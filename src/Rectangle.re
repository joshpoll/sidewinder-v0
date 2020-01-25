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

let fromTwoPoints = (~x1, ~x2, ~y1, ~y2) => {x1, x2, y1, y2};
let fromPointSize = (~x, ~y, ~width, ~height) => {x1: x, x2: x +. width, y1: y, y2: y +. height};
let fromCenterPointSize = (~cx, ~cy, ~width, ~height) => {
  x1: cx -. width /. 2.,
  x2: cx +. width /. 2.,
  y1: cy -. height /. 2.,
  y2: cy +. height /. 2.,
};

let empty = {x1: infinity, x2: neg_infinity, y1: infinity, y2: neg_infinity};

let x1 = ({x1}) => x1;
let x2 = ({x2}) => x2;
let y1 = ({y1}) => y1;
let y2 = ({y2}) => y2;

let cx = ({x1, x2}) => (x1 +. x2) /. 2.;
let cy = ({y1, y2}) => (y1 +. y2) /. 2.;

let width = ({x1, x2}) => x2 -. x1;
let height = ({y1, y2}) => y2 -. y1;

let union = ({x1, x2, y1, y2}, {x1: x1', x2: x2', y1: y1', y2: y2'}) => {
  x1: min(x1, x1'),
  x2: max(x2, x2'),
  y1: min(y1, y1'),
  y2: max(y2, y2'),
};

let union_list = List.fold_left(union, empty);

let inflateWidth = ({x1, x2, y1, y2}, dx) => {x1: x1 -. dx, x2: x2 +. dx, y1, y2};
let inflateHeight = ({x1, x2, y1, y2}, dy) => {x1, x2, y1: y1 -. dy, y2: y2 +. dy};
let inflate = (r, dx, dy) => r->inflateWidth(dx)->inflateHeight(dy);