/* Wrapper for SetCoLa */
/* var result = setcola
   .nodes(graph.nodes)        // Set the graph nodes
   .links(graph.links)        // Set the graph links
   .groups(groups)            // (Optional) Set any predefined groups in the graph
   .guides(guides)            // (Optional) Define any guides that are used by the SetCoLa layout
   .constraints(setcolaSpec)  // Set the constraints
   .gap(gap)                  // The default gap size to use for generating the constraints (if not specified in the SetCoLa spec)
   .layout();      */

type setCola;
[@unboxed]
type setColaConstraint =
  | C('a): setColaConstraint;

type groups;
type guides;

type setColaLayout = {
  nodes: array(WebCoLa.node),
  links: array(WebCoLa.webColaLink),
  groups,
  guides,
  constraints: array(WebCoLa.colaConstraint),
  constraintDefs: array(setColaConstraint),
};

/* TODO: this is the type in the example, but I'm not sure what the "real" type is. */
type node = {name: string};
type graph = {
  nodes: array(node),
  links: array(WebCoLa.webColaLink),
};

[@bs.module] external setCola: setCola = "./setcola";
[@bs.send] external nodes: (setCola, array(node)) => setCola = "nodes";
[@bs.send] external links: (setCola, array(WebCoLa.webColaLink)) => setCola = "links";
/* TODO: */
/* [@bs.send] external groups: (setCola, groups) => setCola = "groups";
   [@bs.send] external guides: (setCola, guides) => setCola = "guides"; */
[@bs.send] external constraints: (setCola, array(setColaConstraint)) => setCola = "constraints";
[@bs.send] external gap: (setCola, float) => setCola = "gap";
[@bs.send] external layout: setCola => setColaLayout = "layout";