/* A bunch of link types */
open Path;

/* TODO: this is the same as rectangle although it's used for a different purpose. combine? */
type linkRender = (~source: Node.sizeOffset, ~target: Node.sizeOffset) => React.element;

/* TODO: this might be a nice way to compress, but not sure if everything needs linkRender yet! */
type link('s, 't) = {
  source: 's,
  target: 't,
  linkRender,
};

type local = {
  source: id,
  target: id,
  linkRender,
};

type sourceLocal = {
  source: id,
  target: relPath,
  linkRender,
};

type lca = {
  source: absPath,
  target: absPath,
  linkRender,
};

type global = {
  source: relPath,
  target: relPath,
  linkRender,
};

let lcaToLocal = ({source: [s, ..._], target: [t, ..._], linkRender}: lca): local => {
  source: s,
  target: t,
  linkRender,
};

let sourceLocalToGlobal = ({source, target, linkRender}: sourceLocal): global => {
  source: {
    ancestorRoot: 0,
    absPath: [source],
  },
  target,
  linkRender,
};