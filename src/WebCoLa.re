/* wrapper for WebCoLa */

type colaLayout;
type colaConstraint;

/* TODO: this is incorrect */
type node = {
  x: float,
  y: float,
};

/* TODO: this type is incorrect. random things add random stuff. */
type link('nt) = {
  length: option(float),
  source: 'nt,
  target: 'nt,
};

/* TODO: ideally this should be a union type using unboxed and GADTs somehow */
[@unboxed]
type node_or_num =
  | NN('a): node_or_num;

type webColaLink = link(node_or_num);

[@bs.new] [@bs.module "webcola"] external colaLayout: unit => colaLayout = "Layout";
[@bs.send] external nodes: (colaLayout, array(node)) => colaLayout = "nodes";
[@bs.send] external links: (colaLayout, array(webColaLink)) => colaLayout = "links";
[@bs.send]
external constraints: (colaLayout, array(colaConstraint)) => colaLayout = "constraints";
[@bs.send] external avoidOverlaps: (colaLayout, bool) => colaLayout = "avoidOverlaps";
[@bs.send]
external start:
  (colaLayout, option(float), option(float), option(float), option(float)) => colaLayout =
  "start";
[@bs.send] external getNodes: colaLayout => array(node) = "nodes";