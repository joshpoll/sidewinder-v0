module RenderLinks = Sidewinder_RenderLinks;

let rec render = (RenderLinks.{nodes, links, bbox, render: nodeRender}) => {
  let nodes = List.map(render, nodes);
  Node.{bbox, rendered: nodeRender(nodes, bbox, links)};
};