/* namespace goodness */
module Kernel = Sidewinder_Kernel;
module LCA = Sidewinder_LCA;
module Layout = Sidewinder_Layout;
module RenderLinks = Sidewinder_RenderLinks;
module Render = Sidewinder_Render;

/* construct a node. links are turned into lcaLinks automatically for constraint layout */
let make = (~nodes, ~links, ~layout, ~computeSizeOffset, ~render): Kernel.node => {
  nodes,
  links: List.map(Link.sourceLocalToGlobal, links),
  layout,
  computeSizeOffset,
  render,
};

let render = (n: Kernel.node) =>
  n
  |> LCA.propagateLCA
  |> Layout.computeBBoxes
  |> RenderLinks.renderLinks
  |> Render.render
  |> ((Node.{rendered}) => rendered);

let debugLCA = (n: Kernel.node) => n |> LCA.propagateLCA |> Js.log2("debug lca");