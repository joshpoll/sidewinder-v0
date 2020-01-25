module LCA = SideWinder_LCA;

type node = {
  nodes: list(node),
  links: list(Link.lca),
  bbox: Node.bbox,
  render: (list(Node.rendered), Node.bbox, list(React.element)) => React.element,
};

let rec resolveAbsPath = (node, absPath) =>
  switch (absPath) {
  | [] => node
  | [h, ...t] => resolveAbsPath(List.nth(node.nodes, h), t)
  };

/* TODO: need to run graph layout here!!! */
/* TODO: this is weird because the child controls its size but the parent controls its position.
   They are currently combined in bboxes. */

type position = {
  x: float,
  y: float,
};

let rec computeBBoxes = ({nodes, links, layout, computeSize, render}: LCA.node): node => {
  let bboxNodes = List.map(computeBBoxes, nodes);
  let nodeBBoxes =
    layout(
      bboxNodes |> List.map(n => n.bbox |> Node.bboxToSize),
      links |> List.map(Link.lcaToLocal),
    );
  let bbox = computeSize(nodeBBoxes) |> Node.sizeToBBox;
  if (List.length(bboxNodes) == List.length(nodeBBoxes)) {
    let nodes = List.combine(bboxNodes, nodeBBoxes) |> List.map(((n, bbox)) => {...n, bbox});
    {nodes, links, bbox, render};
  } else {
    Js.log("layout function doesn't preserve nodes!");
    Js.log2("bboxNodes", bboxNodes |> Array.of_list);
    Js.log2("nodeBBoxes", nodeBBoxes |> Array.of_list);
    assert(false);
  };
};