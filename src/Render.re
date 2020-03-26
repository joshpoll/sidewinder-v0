let rec render = (RenderLinks.{nodes, links, transform, bbox, render: nodeRender}) => {
  let nodes = List.map(render, nodes);
  Node.{transform, bbox, rendered: nodeRender(nodes, bbox, links)};
};