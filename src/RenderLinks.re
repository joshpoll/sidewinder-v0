type node = {
  uid: Node.uid,
  nodes: list(node),
  links: list(React.element),
  bbox: Node.bbox,
  render: (list(Node.rendered), Node.bbox, list(React.element)) => React.element,
};
module MS = Belt.Map.String;

let mapUnion = (m1: MS.t('a), m2: MS.t('a)) => {
  MS.reduce(m2, m1, (m, k, v) => m->MS.set(k, v));
};

let rec computeCumulativeOffsetAux = (pos, Layout.{uid, nodes, links, bbox, render}) => {
  let newPos = Node.{x: pos.x +. bbox.translation.x, y: pos.y +. bbox.translation.y};
  nodes
  |> List.map(computeCumulativeOffsetAux(newPos))
  |> List.fold_left(mapUnion, MS.empty)
  |> MS.set(_, uid, Node.{translation: newPos, sizeOffset: bbox.sizeOffset});
};

let computeCumulativeOffset = computeCumulativeOffsetAux({x: 0., y: 0.});

let renderLink =
    (mp, node: Layout.node, Link.{source, target, linkRender}: Link.uid): React.element => {
  switch (linkRender) {
  | None => <> </>
  | Some(lr) =>
    /* TODO: would be nice to keep this information ar ound during computeBBoxes */
    let sourceBBox = mp->MS.getExn(source);
    let targetBBox = mp->MS.getExn(target);
    lr(~source=Node.bboxToSizeOffset(sourceBBox), ~target=Node.bboxToSizeOffset(targetBBox));
  };
};

let rec renderLinksAux =
        (mp: MS.t(Node.bbox), Layout.{uid, nodes, links, bbox, render} as n): node => {
  let nodes = List.map(renderLinksAux(mp), nodes);
  let renderedLinks = List.map(renderLink(mp, n), links);
  {uid, nodes, links: renderedLinks, bbox, render};
};

let renderLinks = n => renderLinksAux(computeCumulativeOffset(n), n);