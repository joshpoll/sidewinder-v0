open SidewinderUtil;

let box = child =>
  nest([child], ([c]) =>
    {
      width: c.width,
      height: c.height,
      custom: {
        "rendered":
          <g
            transform={
              "translate(" ++ Js.Float.toString(c.x) ++ ", " ++ Js.Float.toString(c.y) ++ ")"
            }>
            <rect
              width={Js.Float.toString(c.width)}
              height={Js.Float.toString(c.height)}
              fillOpacity="0"
              stroke="#000"
              transform={
                "translate("
                ++ Js.Float.toString(-. c.width /. 2.)
                ++ ", "
                ++ Js.Float.toString(-. c.height /. 2.)
                ++ ")"
              }
            />
            {c.custom##rendered}
          </g>,
      },
    }
  );

let a =
  atom(
    <text textAnchor="middle" alignmentBaseline="middle"> {React.string("2")} </text>,
    10.,
    20.,
  );
let a' = box(a);
let b =
  atom(
    <text textAnchor="middle" alignmentBaseline="middle"> {React.string({js|•|js})} </text>,
    10.,
    20.,
  );
let b' = box(b);
let c =
  atom(
    <text textAnchor="middle" alignmentBaseline="middle"> {React.string("4")} </text>,
    10.,
    20.,
  );
let c' = box(c);
let d =
  atom(
    <text textAnchor="middle" alignmentBaseline="middle"> {React.string("/")} </text>,
    10.,
    20.,
  );
let d' = box(d);
let e = sequence([a', b'], _ => <> </>, LeftRight);
let f = sequence([c', d'], _ => <> </>, LeftRight);
/* TODO: need a nonlocal edge from b to f */
let g = sequence([e, f], _ => <> </>, LeftRight);