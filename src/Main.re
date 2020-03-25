/* construct a node. links are turned into lcaLinks automatically for constraint layout */

let counter = ref(0);

let readAndUpdateCounter = () => {
  counter := counter^ + 1;
  counter^ - 1;
};

/* TODO: may want to restrict inputs to local ids somehow? */
let make = (~tags, ~nodes, ~links, ~layout, ~computeSizeOffset, ~render): Kernel.node => {
  uid: string_of_int(readAndUpdateCounter()),
  tags,
  nodes,
  links,
  layout,
  computeSizeOffset,
  render,
};

let render = (~debug=false, n: Kernel.node) =>
  n
  |> (
    if (debug) {
      Debug.transform;
    } else {
      x => x;
    }
  )
  |> LCA.fromKernel
  |> Layout.computeBBoxes
  |> RenderLinks.renderLinks
  |> Render.render
  |> ((Node.{rendered}) => rendered);

let debugLCA = (n: Kernel.node) => n |> LCA.fromKernel |> Js.log2("debug lca");