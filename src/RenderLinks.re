type node = {
  uid: Node.uid,
  flow: list(Node.uid),
  nodes: list(node),
  links: list(React.element),
  transform: Node.transform,
  bbox: Node.bbox,
  render: (list(React.element), Node.bbox, list(React.element)) => React.element,
};
module MS = Belt.Map.String;

let mapUnion = (m1: MS.t('a), m2: MS.t('a)) => {
  MS.reduce(m2, m1, (m, k, v) => m->MS.set(k, v));
};

let rec computeLCAFrameBBoxAux =
        (oldTransform: Node.transform, Layout.{uid, nodes, links, transform, bbox, render}) => {
  let transform: Node.transform =
    Node.{
      translate: {
        x: oldTransform.translate.x +. transform.translate.x,
        y: oldTransform.translate.y +. transform.translate.y,
      },
      scale: {
        x: oldTransform.scale.x *. transform.scale.x,
        y: oldTransform.scale.y *. transform.scale.y,
      },
    };
  nodes
  |> List.map(computeLCAFrameBBoxAux(transform))
  |> List.fold_left(mapUnion, MS.empty)
  |> MS.set(_, uid, bbox->Rectangle.transform(transform));
};

let computeLCAFrameBBox = computeLCAFrameBBoxAux(Transform.init);

let rec computeTransform = (node: Layout.node, path) =>
  switch (path) {
  | [] => node.bbox
  | [h, ...path] =>
    let node = List.find((Layout.{uid}) => h == uid, node.nodes);
    computeTransform(node, path)->Rectangle.transform(node.transform);
  };

let renderLink = (node, Link.{source, target, linkRender}: Link.lcaPath): React.element =>
  switch (linkRender) {
  | None => <> </>
  | Some(lr) =>
    let source = computeTransform(node, source);
    let target = computeTransform(node, target);
    lr(~source, ~target);
  };

let rec renderLinks = (Layout.{uid, flow, nodes, links, transform, bbox, render} as n): node => {
  let nodes = List.map(renderLinks, nodes);
  let links = List.map(renderLink(n), links);
  {uid, flow, nodes, links, transform, bbox, render};
};