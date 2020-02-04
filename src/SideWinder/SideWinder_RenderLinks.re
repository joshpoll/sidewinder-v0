module Kernel = Sidewinder_Kernel;
module Layout = Sidewinder_Layout;

type node = {
  nodes: list(node),
  links: list(React.element),
  bbox: Node.bbox,
  render: (list(Node.rendered), Node.bbox, list(React.element)) => React.element,
};

let renderLink = (node: Layout.node, Link.{source, target, linkRender}: Link.lca): React.element => {
  let sourceNode = Layout.resolveAbsPath(node, source);
  let targetNode = Layout.resolveAbsPath(node, target);
  linkRender(
    ~source=Node.bboxToSizeOffset(sourceNode.bbox),
    ~target=Node.bboxToSizeOffset(targetNode.bbox),
  );
};

let rec renderLinks = (Layout.{nodes, links, bbox, render} as n): node => {
  let nodes = List.map(renderLinks, nodes);
  let renderedLinks = List.map(renderLink(n), links);
  {nodes, links: renderedLinks, bbox, render};
};