let rec hide = (tag, Kernel.{tags, nodes, links, layout, computeSizeOffset, render}) =>
  if (List.mem(tag, tags)) {
    None;
  } else {
    Some(
      Kernel.{
        tags,
        nodes:
          List.map(hide(tag), nodes) |> List.filter(x => x != None) |> List.map((Some(x)) => x),
        links, /* TODO: remove associated links, too */
        layout,
        computeSizeOffset,
        render,
      },
    );
  };

let rec denestAux = (tag, Kernel.{tags, nodes} as node) => {
  let (nodes, denestedNodes) = List.map(denestAux(tag), nodes) |> List.split;
  if (List.mem(tag, tags)) {
    (
      /* create pointer */
      Theia.atom(
        ~tags=["denest ptr"],
        ~links=[
          Link.{
            source: None,
            target: {
              ancestorRoot: 2, /* TODO: this shouldn't be hard coded. it depends on some other properties */
              absPath: [0] /* TODO: would be a lot easier to use global/local unique ids */,
            },
            linkRender:
              Some(
                (~source, ~target) => {
                  <line
                    x1={Js.Float.toString(source->Rectangle.cx)}
                    y1={Js.Float.toString(source->Rectangle.cy)}
                    x2={Js.Float.toString(target->Rectangle.x1)}
                    y2={Js.Float.toString(target->Rectangle.cy)}
                    stroke="purple"
                  />
                },
              ),
          },
        ],
        <circle r="2" cx="2" cy="2" />,
        Rectangle.fromPointSize(~x=0., ~y=0., ~width=4., ~height=4.),
        (),
      ),
      [{...node, nodes}, ...List.flatten(denestedNodes)],
    );
  } else {
    (node, List.flatten(denestedNodes));
  };
};

let rec denest = (tagBegin, tagDenest, Kernel.{tags, nodes} as node) =>
  if (List.mem(tagDenest, tags)) {
    Theia.graph(
      ~tags=["denest"],
      ~nodes=denestAux(tagDenest, node) |> snd,
      ~links=[],
      ~gap=25.,
      ~linkDistance=40.,
      ~constraints=Theia.directionConstraints(LeftRight),
      (),
    );
  } else {
    {...node, nodes: List.map(denest(tagBegin, tagDenest), nodes)};
  };