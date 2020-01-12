/* wrapper for WebCoLa */

type colaLayout;
type node = {
  x: float,
  y: float,
  width: option(float),
  height: option(float),
};

type link('nt) = {
  length: float,
  source: 'nt,
  target: 'nt,
};

/* TODO: ideally this should be a union type using unboxed and GADTs somehow */
[@unboxed]
type node_or_num =
  | NN('a): node_or_num;

[@bs.new] [@bs.module "webcola"] external colaLayout: colaLayout = "Layout";
[@bs.send] external nodes: (colaLayout, array(node)) => colaLayout = "nodes";
[@bs.send] external links: (colaLayout, array(link(node_or_num))) => colaLayout = "links";
[@bs.send] external constraints: (colaLayout, array('a)) => colaLayout = "constraints";
[@bs.send] external avoidOverlaps: (colaLayout, bool) => colaLayout = "avoidOverlaps";
[@bs.send]
external start:
  (colaLayout, option(float), option(float), option(float), option(float)) => colaLayout =
  "start";
[@bs.send] external getNodes: colaLayout => array(node) = "nodes";