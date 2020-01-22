/* TODO: need to improve id representation */
/* TODO: need to add rendering and constraint information */
/* TODO: how to add transition information? */

type id = int;
type absPath = list(id);
type setCoLaSpec = string;
/* relPath's default root is its Graph scope */
/* ancestorRoot acts like prepending .. to path. goes up nesting hierarchy */
/* when ancestorRoot == 0, the path is local */
type relPath = {
  ancestorRoot: int,
  absPath,
};

type coordinates = {
  x1: float,
  y1: float,
  x2: float,
  y2: float,
};

type edgeRender = coordinates => React.element;

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

type renderedLocalEdge = {
  source: id,
  target: id,
  rendered: React.element,
};

/* type nest = {
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
   }; */

/* json literal syntax */
/* {json|

   {"collect": ["node", "node.neighbors"]}

   |json} */

type renderedNode = SetCoLa.node({. rendered: React.element});
type renderedWebCoLaNode = WebCoLa.node({. rendered: React.element});

type renderedGraphElements = {
  renderedNodes: list(renderedNode),
  renderedEdges: list(localEdge) /* TODO: change? */,
  /* width and height of the graph. computed by unioning the node bounding boxes like webcola does */
  width: float,
  height: float,
};

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
/* nodes with annotations for their width and height */
/* type sizedNode = {
     nodes: list(sizedNode),
     edges: list(localEdge),
     constraints: array(SetCoLa.setColaConstraint),
     nodeRender: renderedGraphElements => React.element,
     width: float,
     height: float,
   }; */

/* adapted from https://bl.ocks.org/tophtucker/62f93a4658387bb61e4510c37e2e97cf */
let measureCourier = (string, fontSize) => {
  let widths = [|
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.,
    0.45,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.7566665649414063,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.7666656494140625,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.7633331298828125,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.7633331298828125,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
    0.75,
  |];
  let avg = 0.7473684049907483;
  (
    string
    |> List.map(c =>
         c |> Js.String.charCodeAt(0) < float_of_int(Array.length(widths))
           ? widths[c |> Js.String.charCodeAt(0) |> int_of_float] : avg
       )
    |> List.fold_left((cur, acc) => acc +. cur, 0.)
  )
  *. float_of_int(fontSize);
};

type node = {
  nodes: list(node),
  links: list(localEdge) /* TODO: generalize this to nonlocal edges! */,
  constraints: array(SetCoLa.setColaConstraint),
  render: (list(renderedWebCoLaNode), list(localEdge)) => renderedNode,
};