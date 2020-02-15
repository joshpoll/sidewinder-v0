type uid = string; /* globally unique node id used for links */
type localUID = int; /* locally unique node id used for layout */
type sizeOffset = Rectangle.t;

type point = {
  x: float,
  y: float,
};

/**
 * sizeOffset stores the size and offset from the origin of the node when it's rendered in its
default position.
 * translation moves this box around.
 */
type bbox = {
  translation: point,
  sizeOffset,
};

type rendered = {
  bbox,
  rendered: React.element,
};

let sizeOffsetToBBox = sizeOffset => {
  translation: {
    x: 0.,
    y: 0.,
  },
  sizeOffset,
};

let bboxToSizeOffset = ({translation: {x, y}, sizeOffset}) =>
  Rectangle.fromPointSize(
    ~x=x +. sizeOffset->Rectangle.x1,
    ~y=y +. sizeOffset->Rectangle.y1,
    ~width=sizeOffset->Rectangle.width,
    ~height=sizeOffset->Rectangle.height,
  );