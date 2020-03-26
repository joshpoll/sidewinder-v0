type translate = {
  x: float,
  y: float,
};

/* should be interpreted as scaling from the origin of the object */
type scale = {
  x: float,
  y: float,
};

type t = {
  translate,
  scale,
};

let ident: t;

let init: t;

let translateX: (t, float) => t;

let translateY: (t, float) => t;

let translate: (t, float, float) => t;

let scaleX: (t, float) => t;

let scaleY: (t, float) => t;

let scale: (t, float, float) => t;