type size = {
  width: float,
  height: float,
};

type bbox = Rectangle.t;

// type rendered = SetCoLa.node({. rendered: React.element});
type rendered = {
  bbox,
  rendered: React.element,
};

let sizeToBBox = ({width, height}) =>
  Rectangle.fromCenterPointSize(~cx=0., ~cy=0., ~width, ~height);
/* Rectangle.fromPointSize(~x=0., ~y=0., ~width, ~height); */
let bboxToSize = bbox => {width: bbox->Rectangle.width, height: bbox->Rectangle.height};