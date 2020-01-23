/* wrapper for WebCoLa */

type colaLayout;
type colaConstraint;

/* TODO: this is incorrect */
type node('a) = {
  x: float,
  y: float,
  width: float,
  height: float,
  [@bs.as "_temp"]
  temp: bool,
  custom: Js.t({..} as 'a),
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
[@bs.send] external nodes: (colaLayout, array(node('a))) => colaLayout = "nodes";
[@bs.send] external links: (colaLayout, array(webColaLink)) => colaLayout = "links";
[@bs.send]
external constraints: (colaLayout, array(colaConstraint)) => colaLayout = "constraints";
[@bs.send] external avoidOverlaps: (colaLayout, bool) => colaLayout = "avoidOverlaps";
[@bs.send]
external start:
  (colaLayout, option(float), option(float), option(float), option(float)) => colaLayout =
  "start";
[@bs.send] external getNodes: colaLayout => array(node('a)) = "nodes";
[@bs.send] external getLinks: colaLayout => array(link(node('a))) = "links";
[@bs.send] external linkDistance: (colaLayout, float) => colaLayout = "linkDistance";
[@bs.send]
external linkDistanceFn: (colaLayout, link(node('a)) => float) => colaLayout = "linkDistance";