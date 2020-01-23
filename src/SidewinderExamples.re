open SidewinderUtil;

let box = (~dx=0., ~dy=0., child) =>
  nest(
    [child],
    ([c]) => {
      let bbox =
        Rectangle.(
          fromPointSize(~x=c.x, ~y=c.y, ~width=c.width, ~height=c.height, ())->inflate(dx, dy)
        );
      Rectangle.{
        width: bbox->width,
        height: bbox->height,
        custom: {
          "rendered":
            <g
              transform={
                "translate("
                ++ Js.Float.toString(bbox->x1)
                ++ ", "
                ++ Js.Float.toString(bbox->y1)
                ++ ")"
              }>
              <rect
                width={Js.Float.toString(bbox->width)}
                height={Js.Float.toString(bbox->height)}
                fillOpacity="0"
                stroke="#000"
                transform={
                  "translate("
                  ++ Js.Float.toString(-. bbox->width /. 2.)
                  ++ ", "
                  ++ Js.Float.toString(-. bbox->height /. 2.)
                  ++ ")"
                }
              />
              {c.custom##rendered}
            </g>,
        },
      };
    },
  );

let a =
  atom(
    <text textAnchor="middle" dominantBaseline="middle"> {React.string("2")} </text>,
    10.,
    20.,
  );
let a' = box(~dx=5., a);
let b =
  atom(
    <text textAnchor="middle" dominantBaseline="middle"> {React.string({js|•|js})} </text>,
    10.,
    20.,
  );
let b' = box(~dx=5., b);
let c =
  atom(
    <text textAnchor="middle" dominantBaseline="middle"> {React.string("4")} </text>,
    10.,
    20.,
  );
let c' = box(~dx=5., c);
let d =
  atom(
    <text textAnchor="middle" dominantBaseline="middle"> {React.string("/")} </text>,
    10.,
    20.,
  );
let d' = box(~dx=5., d);
let e = sequence(~gap=0.1, ~linkDistance=0.1, [a', b'], _ => <> </>, LeftRight);
let f = sequence(~gap=0.1, ~linkDistance=0.1, [c', d'], _ => <> </>, LeftRight);
/* TODO: need a nonlocal edge from b to f */
let g = sequence(~gap=30., [e, f], _ => <> </>, LeftRight);