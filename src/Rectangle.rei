type t;

/**
 * Use lower left(?) and upper right(?) corner of the rectangle.
 */
let fromTwoPoints: (~x1: float, ~x2: float, ~y1: float, ~y2: float) => t;
/**
 * Use upper left corner and size of the rectangle.
 * TODO: Convention should match that of WebCoLa?
 */
let fromPointSize: (~x: float, ~y: float, ~width: float, ~height: float) => t;
/**
 * Ues center of and size of the rectangle.
 */
let fromCenterPointSize: (~cx: float, ~cy: float, ~width: float, ~height: float) => t;

/**
 * Returns an "empty" rectangle. Essentially an inversion of the plane.
 * Useful as identity for `union`.
 */
let empty: t;

let x1: t => float;
let x2: t => float;
let y1: t => float;
let y2: t => float;

/**
 * Horizontal center.
 */
let cx: t => float;
/**
 * Vertical center.
 */
let cy: t => float;

let width: t => float;
let height: t => float;

/**
 * Computes the smallest rectangle containing the two input rectangles.
 */
let union: (t, t) => t;
/**
 * Unions all rectangles in the input list.
 */
let union_list: list(t) => t;

/**
 * Symmetrically extend width.
 */
let inflateWidth: (t, float) => t;
/**
 * Symmetrically extend height.
 */
let inflateHeight: (t, float) => t;
/**
 * Symmetrically extend width and height.
 */
let inflate: (t, float, float) => t;