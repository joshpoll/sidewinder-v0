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

let unzip3: list(('a, 'b, 'c)) => (list('a), list('b), list('c)) =
  l =>
    List.fold_right(
      ((a, b, c), (la, lb, lc)) => ([a, ...la], [b, ...lb], [c, ...lc]),
      l,
      ([], [], []),
    );

/* denest everything matching tag in nodes */
let rec denestAux = (tag, Kernel.{tags, nodes} as node) => {
  let (nodes, denestedNodes, denestingLinks) = List.map(denestAux(tag), nodes) |> unzip3;
  let node = {...node, nodes};
  if (List.mem(tag, tags)) {
    let ptr =
      Theia.atom(
        ~tags=["denest ptr"],
        ~links=[],
        <circle r="2" cx="2" cy="2" />,
        Rectangle.fromPointSize(~x=0., ~y=0., ~width=4., ~height=4.),
        (),
      );
    (
      ptr,
      [node, ...List.flatten(denestedNodes)],
      [
        (
          Link.{
            source: ptr.uid,
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
          }: Link.uid
        ),
        ...List.flatten(denestingLinks),
      ],
    );
  } else {
    (node, List.flatten(denestedNodes), List.flatten(denestingLinks));
  };
};

/* search for tagBegin to begin denesting. denest everything matching tagDenest in that subtree */
let rec denest = (tagBegin, tagDenest, Kernel.{tags, nodes} as node) =>
  if (List.mem(tagBegin, tags)) {
    let (nodes, rest, links) = List.map(denestAux(tagDenest), nodes) |> unzip3;
    Theia.graph(
      ~tags=["denest"],
      ~nodes=[{...node, nodes}, ...List.flatten(rest)],
      ~links=List.flatten(links),
      ~gap=25.,
      ~linkDistance=40.,
      ~constraints=Theia.directionConstraints(LeftRight),
      (),
    );
  } else {
    {...node, nodes: List.map(denest(tagBegin, tagDenest), nodes)};
  };