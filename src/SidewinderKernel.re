/* TODO: need to improve id representation */
/* TODO: need to add rendering and constraint information */
/* TODO: how to add transition information? */

type id = string;
type absPath = list(id);
type json = string;
type setCoLaSpec = json;
/* relPath's default root is its Graph scope */
/* ancestorRoot acts like prepending .. to path. goes up nesting hierarchy */
/* when ancestorRoot == 0, the path is local */
type relPath = {
  ancestorRoot: int,
  absPath,
};

type edgeRender = React.element;

type edge = {
  source: id,
  target: relPath,
  edgeRender,
};

/* type element =
     /* nodes, constraint edges, visual edges */
     | Graph(list(node), list(edge), list(edge))
     | Nest(list(node), list(React.element) => React.element)

   and node = {
     id,
     element,
   }; */

type localEdge = {
  source: id,
  target: id,
  edgeRender,
};

type nest = {
  children: list(node),
  render: list(React.element) => React.element,
}

and element =
  | Graph(list(node), list(localEdge), setCoLaSpec)
  | Nest(nest)
  | Ptr(React.element, relPath, edgeRender)

and node = {
  id,
  element,
};

/* json literal syntax */
/* {json|

   {"collect": ["node", "node.neighbors"]}

   |json} */

type renderedGraphElements = {
  renderedNodes: list(React.element),
  renderedEdges: list(React.element),
};

/* simplest version. shows local nodes and nonlocal edges */
/* maybe it's good enough for everything? */
type node2 = {
  id,
  nodes: list(node2),
  edges: list(edge),
  constraints: setCoLaSpec,
  render: renderedGraphElements => React.element,
};

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