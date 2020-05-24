/* construct a node. links are turned into lcaLinks automatically for constraint layout */

let counter = ref(0);

let readAndUpdateCounter = () => {
  counter := counter^ + 1;
  counter^ - 1;
};

/* TODO: may want to restrict inputs to local ids somehow? */
let make =
    (~tags, ~nodes, ~links, ~layout, ~computeBBox, ~render, ~uid=?, ~flowTag=?, ()): Kernel.node => {
  uid:
    switch (uid) {
    | None => "autogen__" ++ string_of_int(readAndUpdateCounter())
    | Some(uid) => uid
    },
  /* flow, */
  flowTag,
  tags,
  nodes,
  links,
  layout,
  computeBBox,
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
  |> Render.render;

let renderLinks = (~debug=false, n: Kernel.node) =>
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
  |> RenderLinks.renderLinks;

let renderTransition =
    (
      ~debug=false,
      ~lowerFlow=false,
      ~prevState: TransitionNode.state,
      ~currState: TransitionNode.state,
      n1: Kernel.node,
      flow,
      n2: Kernel.node,
    )
    : React.element =>
  Render.renderTransition(
    ~lowerFlow,
    ~prevState,
    ~currState,
    flow,
    renderLinks(~debug, n2) |> Render.computeGlobalTransform,
    renderLinks(~debug, n1) |> Render.computeGlobalTransform,
  );

let debugLCA = (n: Kernel.node) => n |> LCA.fromKernel |> Js.log2("debug lca");