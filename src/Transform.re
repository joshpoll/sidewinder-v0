type translate = {
  x: float,
  y: float,
};

type scale = {
  x: float,
  y: float,
};

type t = {
  translate,
  scale,
};

let ident = {
  translate: {
    x: 0.,
    y: 0.,
  },
  scale: {
    x: 1.,
    y: 1.,
  },
};

let init = ident;

let translateX = ({translate: {x, y}} as t, dx) => {
  ...t,
  translate: {
    x: x +. dx,
    y,
  },
};

let translateY = ({translate: {x, y}} as t, dy) => {
  ...t,
  translate: {
    x,
    y: y +. dy,
  },
};

let translate = (t, dx, dy) => t->translateX(dx)->translateY(dy);

let scaleX = ({scale: {x, y}} as t, dx) => {
  ...t,
  translate: {
    x: x *. dx,
    y,
  },
};

let scaleY = ({scale: {x, y}} as t, dy) => {
  ...t,
  translate: {
    x,
    y: y *. dy,
  },
};

let scale = (t, dx, dy) => t->scaleX(dx)->scaleY(dy);