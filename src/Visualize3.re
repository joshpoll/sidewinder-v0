[@react.component]
let make = (~node, ~width, ~height) => {
  SideWinder.debugLCA(node);
  <svg
    xmlns="http://www.w3.org/2000/svg"
    width={Js.Float.toString(width)}
    height={Js.Float.toString(height)}>
    <g
      transform={
        "translate("
        ++ Js.Float.toString(width /. 2.)
        ++ ", "
        ++ Js.Float.toString(height /. 2.)
        ++ ")"
      }>
      {SideWinder.render(node)}
    </g>
  </svg>;
};