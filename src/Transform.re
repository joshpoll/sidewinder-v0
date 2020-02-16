let rec hideNodes = (tag, Kernel.{uid, tags, nodes, links, layout, computeSizeOffset, render}) =>
  if (List.mem(tag, tags)) {
    (None, [uid]);
  } else {
    let (nodes, uids) = List.map(hideNodes(tag), nodes) |> List.split;
    (
      Some(
        Kernel.{
          uid,
          tags,
          nodes: nodes |> List.filter(Belt.Option.isSome) |> List.map(Belt.Option.getExn),
          links,
          layout,
          computeSizeOffset,
          render,
        },
      ),
      List.flatten(uids),
    );
  };

let rec hideLinks =
        (hiddenUIDs, Kernel.{uid, tags, nodes, links, layout, computeSizeOffset, render}) =>
  Kernel.{
    uid,
    tags,
    nodes: List.map(hideLinks(hiddenUIDs), nodes),
    links:
      List.filter(
        ({source, target}: Link.uid) =>
          !List.mem(source, hiddenUIDs) && !List.mem(target, hiddenUIDs),
        links,
      ),
    layout,
    computeSizeOffset,
    render,
  };

let hide = (tag, n) => {
  let (node, uids) = hideNodes(tag, n);
  /* TODO: monad! */
  switch (node) {
  | None => None
  | Some(node) => Some(hideLinks(uids, node))
  };
};

/* denest everything matching tag in nodes */
let rec denestAux = (tag, Kernel.{tags, nodes} as node) => {
  let (nodes, denestedNodes) = List.map(denestAux(tag), nodes) |> List.split;
  let node = {...node, nodes};
  if (List.mem(tag, tags)) {
    (
      /* create pointer */
      Theia.atom(
        ~tags=["denest ptr"],
        ~links=[
          Link.{
            source: string_of_int(Main.counter^),
            target: node.uid,
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
      [node, ...List.flatten(denestedNodes)],
    );
  } else {
    (node, List.flatten(denestedNodes));
  };
};

/* search for tagBegin to begin denesting. denest everything matching tagDenest in that subtree */
let rec denest = (tagBegin, tagDenest, Kernel.{tags, nodes} as node) =>
  if (List.mem(tagBegin, tags)) {
    let (nodes, rest) = List.map(denestAux(tagDenest), nodes) |> List.split;
    Theia.graph(
      ~tags=["denest"],
      ~nodes=[{...node, nodes}, ...List.flatten(rest)],
      ~links=[],
      ~gap=25.,
      ~linkDistance=40.,
      ~constraints=Theia.directionConstraints(LeftRight),
      (),
    );
  } else {
    {...node, nodes: List.map(denest(tagBegin, tagDenest), nodes)};
  };