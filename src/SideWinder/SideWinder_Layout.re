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
    /* Js.log3("bbox node (x1, y1)", node.bbox->Rectangle.x1, node.bbox->Rectangle.y1);
       Js.log3("bbox subNode (x1, y1)", subNode.bbox->Rectangle.x1, subNode.bbox->Rectangle.y1);
       Js.log2("new x1", node.bbox->Rectangle.x1 +. subBBox->Rectangle.x1);
       Js.log2("new y1", node.bbox->Rectangle.y1 +. subBBox->Rectangle.y1); */
    let translation =
      Node.{
        x: node.bbox.translation.x +. subNode.bbox.translation.x,
        y: node.bbox.translation.y +. subNode.bbox.translation.y,
      };
    resolveAbsPath({
                     ...subNode,
                     bbox: {
                       sizeOffset: subNode.bbox.sizeOffset,
                       translation,
                     },
                   }, t);
  };

/**
 *  computes the bboxes of the child nodes of the input node
 */
let rec computeBBoxes = ({nodes, links, layout, computeSizeOffset, render}: LCA.node): node => {
  let bboxSizeOffsets = List.map(computeBBoxes, nodes);
  let nodeBBoxes =
    layout(
      bboxSizeOffsets |> List.map(n => n.bbox.sizeOffset),
      links |> List.map(Link.lcaToLocal),
    );
  let sizeOffset = computeSizeOffset(nodeBBoxes);
  if (List.length(bboxSizeOffsets) == List.length(nodeBBoxes)) {
    Js.log2("nodeBBoxes", nodeBBoxes |> Array.of_list);
    let nodes =
      List.combine(bboxSizeOffsets, nodeBBoxes) |> List.map(((n, bbox)) => {...n, bbox});
    {
      nodes,
      links,
      bbox: {
        translation: {
          x: 0.,
          y: 0.,
        },
        sizeOffset,
      },
      render,
    };
  } else {
    Js.log("layout function doesn't preserve nodes!");
    Js.log2("bboxSizeOffsets", bboxSizeOffsets |> Array.of_list);
    Js.log2("nodeBBoxes", nodeBBoxes |> Array.of_list);
    assert(false);
  };
};