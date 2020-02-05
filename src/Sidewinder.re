/* namespace goodness */
module Node = Sidewinder_Node;
module Path = Sidewinder_Path;
module Link = Sidewinder_Link;

module Kernel = Sidewinder_Kernel;
module LCA = Sidewinder_LCA;
module Layout = Sidewinder_Layout;
module RenderLinks = Sidewinder_RenderLinks;
module Render = Sidewinder_Render;

/* construct a node. links are turned into lcaLinks automatically for constraint layout */
let make = (~nodes, ~links, ~layout, ~computeSizeOffset, ~render): Sidewinder_Kernel.node => {
  nodes,
  links: List.map(Link.sourceLocalToGlobal, links),
  layout,
  computeSizeOffset,
  render,
};

let render = (n: Sidewinder_Kernel.node) =>
  n
  |> Sidewinder_LCA.propagateLCA
  |> Sidewinder_Layout.computeBBoxes
  |> Sidewinder_RenderLinks.renderLinks
  |> Sidewinder_Render.render
  |> ((Node.{rendered}) => rendered);

let debugLCA = (n: Sidewinder_Kernel.node) =>
  n |> Sidewinder_LCA.propagateLCA |> Js.log2("debug lca");