/* extra bindings for react-spring */
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