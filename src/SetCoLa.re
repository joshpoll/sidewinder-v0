/* Wrapper for SetCoLa */
/* var result = setcola
   .nodes(graph.nodes)        // Set the graph nodes
   .links(graph.links)        // Set the graph links
   .groups(groups)            // (Optional) Set any predefined groups in the graph
   .guides(guides)            // (Optional) Define any guides that are used by the SetCoLa layout
   .constraints(setcolaSpec)  // Set the constraints
   .gap(gap)                  // The default gap size to use for generating the constraints (if not specified in the SetCoLa spec)
   .layout();      */

type setCola('a);
[@unboxed]
type setColaConstraint =
  | C('a): setColaConstraint;

type groups;
type guides;

type setColaLayout('a) = {
  nodes: array(WebCoLa.node('a)),
  links: array(WebCoLa.webColaLink),
  groups,
  guides,
  constraints: array(WebCoLa.colaConstraint),
  constraintDefs: array(setColaConstraint),
};

/* TODO: this is the type in the example, but I'm not sure what the "real" type is. */
type node('a) = {
  width: float,
  height: float,
  custom: Js.t({..} as 'a),
};
type graph('a) = {
  nodes: array(node('a)),
  links: array(WebCoLa.webColaLink),
};

[@bs.module] external setCola: setCola('a) = "./setcola";
[@bs.send] external nodes: (setCola('a), array(node('a))) => setCola('a) = "nodes";
[@bs.send] external links: (setCola('a), array(WebCoLa.webColaLink)) => setCola('a) = "links";
/* TODO: */
/* [@bs.send] external groups: (setCola, groups) => setCola = "groups";
   [@bs.send] external guides: (setCola, guides) => setCola = "guides"; */
[@bs.send]
external constraints: (setCola('a), array(setColaConstraint)) => setCola('a) = "constraints";
[@bs.send] external gap: (setCola('a), float) => setCola('a) = "gap";
[@bs.send] external layout: setCola('a) => setColaLayout('a) = "layout";

/* TODO: this is not correct */
/* type setColaOutNode('a) = {
     _id: int,
     depth: int,
     custom: 'a,
     getSources: unit => array(setColaOutNode('a)),
     getTargets: unit => array(setColaOutNode('a)),
     getNeighbors: unit => array(setColaOutNode('a)),
     getIncoming: unit => array(setColaOutLink('a)),
     getOutgoing: unit => array(setColaOutLink('a)),
     getEdges: unit => array(setColaOutLink('a)),
     getFirstChild: unit => Js.Nullable.t(setColaOutNode('a)),
   }

   and setColaOutLink('b) = 'b; */