module LCA = SideWinder_LCA;

type node = {
  nodes: list(node),
  links: list(Link.lca),
  bbox: Node.bbox,
  render: (list(Node.rendered), Node.bbox, list(React.element)) => React.element,
};

/**
 * Returns the sub-node in node specified by absPath.
 * Translates its bbox to be relative to node's origin.
 */
let rec resolveAbsPath = (node, absPath) =>
  switch (absPath) {
  | [] =>
    Js.log("bbox end");
    node;
  | [h, ...t] =>
    let subNode = List.nth(node.nodes, h);
    let subBBox = subNode.bbox;
    Js.log3("bbox node (x1, y1)", node.bbox->Rectangle.x1, node.bbox->Rectangle.y1);
    Js.log3("bbox subNode (x1, y1)", subNode.bbox->Rectangle.x1, subNode.bbox->Rectangle.y1);
    Js.log2("new x1", node.bbox->Rectangle.x1 +. subBBox->Rectangle.x1);
    Js.log2("new y1", node.bbox->Rectangle.y1 +. subBBox->Rectangle.y1);
    let bbox =
      Rectangle.fromPointSize(
        ~x=node.bbox->Rectangle.x1 +. subBBox->Rectangle.x1,
        ~y=node.bbox->Rectangle.y1 +. subBBox->Rectangle.y1,
        ~width=subBBox->Rectangle.width,
        ~height=subBBox->Rectangle.height,
      );
    resolveAbsPath({...subNode, bbox}, t);
  };

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
    Js.log2("nodeBBoxes", nodeBBoxes |> Array.of_list);
    let nodes = List.combine(bboxNodes, nodeBBoxes) |> List.map(((n, bbox)) => {...n, bbox});
    {nodes, links, bbox, render};
  } else {
    Js.log("layout function doesn't preserve nodes!");
    Js.log2("bboxNodes", bboxNodes |> Array.of_list);
    Js.log2("nodeBBoxes", nodeBBoxes |> Array.of_list);
    assert(false);
  };
};