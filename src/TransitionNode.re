module SpringHook =
  Spring.MakeSpring({
    type t = (float, float, float, float);
    type interpolate = (float, float, float, float) => string;
  });

module G = {
  type reactElement;
  [@bs.module "react-spring"] [@bs.scope "animated"] external externalG_: reactElement = "g";
  [@bs.module "react"]
  external externalCreateElement: (reactElement, Js.t({..}), React.element) => React.element =
    "createElement";
  type spreadProps;
  [@bs.deriving abstract]
  type jsProps = {
    [@bs.optional]
    className: string,
    [@bs.optional]
    onMouseMove: ReactEvent.Mouse.t => unit,
    [@bs.optional]
    onClick: ReactEvent.Mouse.t => unit,
    [@bs.optional]
    onMouseDown: ReactEvent.Mouse.t => unit,
    [@bs.optional]
    onMouseLeave: unit => unit,
    [@bs.optional]
    transform: string,
    [@bs.optional]
    style: ReactDOMRe.Style.t,
  };

  [@react.component]
  let make =
      (
        ~className=?,
        ~spreadProps=?,
        ~onMouseMove=?,
        ~onMouseLeave=?,
        ~onClick=?,
        ~onMouseDown=?,
        ~transform=?,
        ~style,
        ~children=React.null,
        (),
      ) => {
    let props =
      Js.Obj.assign(
        switch (spreadProps) {
        | None => Js.Obj.empty()
        | Some(s) => (Obj.magic(s))(.)
        },
        Obj.magic(
          jsProps(
            ~className?,
            ~onMouseMove?,
            ~onMouseLeave?,
            ~onMouseDown?,
            ~onClick?,
            ~transform?,
            ~style,
            (),
          ),
        ),
      );

    externalCreateElement(externalG_, props, children);
  };
};

let computeSVGTransform =
    ({translate: {x: tx, y: ty}, scale: {x: sx, y: sy}}: Node.transform, bbox) => {
  /* https://css-tricks.com/transforms-on-svg-elements/ */
  let scale =
    "translate("
    ++ Js.Float.toString(bbox->Rectangle.x1 +. bbox->Rectangle.width /. 2.)
    ++ ", "
    ++ Js.Float.toString(bbox->Rectangle.y1 +. bbox->Rectangle.height /. 2.)
    ++ ")";
  let scale = scale ++ " " ++ {j|scale($sx, $sy)|j};
  let scale =
    scale
    ++ " "
    ++ "translate("
    ++ Js.Float.toString(-. (bbox->Rectangle.x1 +. bbox->Rectangle.width /. 2.))
    ++ ", "
    ++ Js.Float.toString(-. (bbox->Rectangle.y1 +. bbox->Rectangle.height /. 2.))
    ++ ")";

  let translate = {j|translate($tx, $ty)|j};

  scale ++ " " ++ translate;
};

let computeSVGTransformFlattened = (bbox, tx, ty, sx, sy) =>
  computeSVGTransform({
                        translate: {
                          x: tx,
                          y: ty,
                        },
                        scale: {
                          x: sx,
                          y: sy,
                        },
                      }, bbox);

[@react.component]
let make =
    (
      ~bbox: Sidewinder.Node.bbox,
      ~renderedElem: React.element,
      ~transform: Sidewinder.Node.transform,
      ~nextTransform: Sidewinder.Node.transform,
    ) => {
  let (values, setValues) =
    SpringHook.use(
      ~config=Spring.config(~mass=1., ~tension=80., ~friction=20.),
      (transform.translate.x, transform.translate.y, transform.scale.x, transform.scale.y),
    );
  <G
    onMouseMove={e => {
      setValues((
        nextTransform.translate.x,
        nextTransform.translate.y,
        nextTransform.scale.x,
        nextTransform.scale.y,
      ))
    }}
    transform={values->SpringHook.interpolate(computeSVGTransformFlattened(bbox))}
    style={ReactDOMRe.Style.make(
      ~transform=values->SpringHook.interpolate(computeSVGTransformFlattened(bbox)),
      (),
    )}>
    renderedElem
  </G>;
};