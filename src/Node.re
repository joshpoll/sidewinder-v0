type uid = string; /* globally unique node id used for links */
type localUID = int; /* locally unique node id used for layout */

type point = {
  x: float,
  y: float,
};

type transform = Transform.t;

type bbox = Rectangle.t;