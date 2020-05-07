open Theia;

let value = (name, node) => box(~tags=[name], ~dx=5., ~dy=5., node, [], ());

let ex0 =
  table(
    ~nodes=[[str("Id", ()), str("Val", ())], [str("x", ()), value("5", str("5", ()))]],
    ~xLinkRender=
      Some(
        (~source, ~target) =>
          <line
            x1={Js.Float.toString(
              (source->Sidewinder.Rectangle.x2 +. target->Sidewinder.Rectangle.x1) /. 2.,
            )}
            x2={Js.Float.toString(
              (source->Sidewinder.Rectangle.x2 +. target->Sidewinder.Rectangle.x1) /. 2.,
            )}
            y1={Js.Float.toString(
              (source->Sidewinder.Rectangle.y1 +. target->Sidewinder.Rectangle.y1) /. 2.,
            )}
            y2={Js.Float.toString(
              (source->Sidewinder.Rectangle.y2 +. target->Sidewinder.Rectangle.y2) /. 2.,
            )}
            stroke="black"
          />,
      ),
    ~yLinkRender=
      Some(
        (~source, ~target) =>
          <line
            x1={Js.Float.toString(
              (source->Sidewinder.Rectangle.x1 +. target->Sidewinder.Rectangle.x1) /. 2.,
            )}
            x2={Js.Float.toString(
              (source->Sidewinder.Rectangle.x2 +. target->Sidewinder.Rectangle.x2) /. 2.,
            )}
            y1={Js.Float.toString(
              (source->Sidewinder.Rectangle.y2 +. target->Sidewinder.Rectangle.y1) /. 2.,
            )}
            y2={Js.Float.toString(
              (source->Sidewinder.Rectangle.y2 +. target->Sidewinder.Rectangle.y1) /. 2.,
            )}
            stroke="black"
          />,
      ),
    ~xGap=0.,
    ~yGap=0.,
    ~xDirection=LeftRight,
    ~yDirection=UpDown,
    (),
  );