type node = {
  nodes: list(node),
  links: list(React.element),
  bbox: Node.bbox,
  render: (list(Node.rendered), Node.bbox, list(React.element)) => React.element,
};

let renderLink = (node: Layout.node, Link.{source, target, linkRender}: Link.lca): React.element => {
  switch (linkRender) {
  | None => <> </>
  | Some(lr) =>
    /* TODO: would be nice to keep this information around during computeBBoxes */
    let sourceNode = Layout.resolveAbsPath(node, source);
    let targetNode = Layout.resolveAbsPath(node, target);
    lr(
      ~source=Node.bboxToSizeOffset(sourceNode.bbox),
      ~target=Node.bboxToSizeOffset(targetNode.bbox),
    );
  };
};

let rec renderLinks = (Layout.{nodes, links, bbox, render} as n): node => {
  let nodes = List.map(renderLinks, nodes);
  let renderedLinks = List.map(renderLink(n), links);
  {nodes, links: renderedLinks, bbox, render};
};