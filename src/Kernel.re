/* TODO: need to improve id representation */
/* TODO: how to add transition information? */
/* TODO: move link types to a separate file */
/* type element =
     /* nodes, constraint edges, visual edges */
     | Graph(list(node), list(edge), list(edge))
     | Nest(list(node), list(React.element) => React.element)

   and node = {
     id,
     element,
   }; */

/* type renderedLocalEdge = {
     source: id,
     target: id,
     rendered: React.element,
   }; */

/* type nest = {
     children: list(node),
     render: list(React.element) => React.element,
   }

   and element =
     | Graph(list(node), list(localEdge), setCoLaSpec)
     | Nest(nest)
     | Ptr(React.element, relPath, linkRender)

   and node = {
     id,
     element,
   }; */

/* json literal syntax */
/* {json|

   {"collect": ["node", "node.neighbors"]}

   |json} */

/* type renderedWebCoLaNode = WebCoLa.node({. rendered: React.element}); */

/* type renderedGraphElements = {
     renderedNodes: list(Node.rendered),
     renderedEdges: list(Link.local) /* TODO: change? */,
     /* width and height of the graph. computed by unioning the node bounding boxes like webcola does */
     width: float,
     height: float,
   }; */

/* simplest version. shows local nodes and nonlocal edges */
/* maybe it's good enough for everything? */
/* type node2 = {
     // id,
     nodes: list(node2),
     edges: list(localEdge), /* TODO: should just be `edge` */
     constraints: array(SetCoLa.setColaConstraint),
     nodeRender: renderedGraphElements => renderedNode /* converts this node to a rendered node
     assuming its children have been rendered */
   }; */

/* question: how to do edge layout? */
/* possibility: do local edges first. then fix nodes with local edges. finally do layout for
   remaining edges */

/* basically from the combined one, you see a separation of concerns */
/* bottom-up we're alternating graph layout and rendering at each node. these steps should be
   separated especially since many things might not use graphs or might not use special rendering */
/* separation makes steps clearer */

/* TODO: transitions. essentially dynamic DPO I think. not sure how to frame or justify that. */
/* dynamic b/c allows people to use whatever language they want to specify semantics. */
/* double pushout naturally captures add, stay, delete, but needs to be extended with copy and
   mutate (maybe it does those, too? Haven't looked closely enough yet) */
/* DPO should be possible on this representation. Look at "Algebraic hierarchical graph
   transformation" and "Abstract hierarchical graph transformation". */

/* ------------------------------------------------- */

type tag = string;

type node = {
  uid: Node.uid,
  tags: list(tag) /* TODO: this is an experiment for writing some transformations. they are   currently erased during LCA. might want to propagate them so rendering can use tags. not sure */,
  nodes: list(node),
  links: list(Link.uid),
  /* TODO: links here shouldn't have rendering */
  layout:
    (Belt.Map.String.t(int), list(Node.sizeOffset), list(Link.layout)) => list(Node.bbox),
  /* TODO: this should actually compute a *bbox*. That way we can track offsets from the origin that
     the nodes produce. Right now we just assume that the nodes are rendered with bboxes with upper
     left corner at the origin. However things like inflation require recentering. It's much easier
     to track the bbox rather than the size here. */
  /* TODO: maybe not */
  computeSizeOffset: list(Node.bbox) => Node.sizeOffset,
  /* TODO: should this be lca? */
  render: (list(Node.rendered), Node.bbox, list(React.element)) => React.element,
};

let rec resolveAbsPath = (node, absPath) =>
  switch (absPath) {
  | [] => node
  | [h, ...t] => resolveAbsPath(List.nth(node.nodes, h), t)
  };