/* A bunch of link types */
open Path;

/* TODO: this is the same as rectangle although it's used for a different purpose. combine? */
type linkRender = option((~source: Node.sizeOffset, ~target: Node.sizeOffset) => React.element);

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

/* if source is None, the node is the source, otherwise one of its sub nodes is the source */
type sourceLocal = {
  source: option(id),
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

type uid = {
  source: Node.uid,
  target: Node.uid,
  linkRender,
};

type localUID = {
  source: Node.localUID,
  target: Node.localUID,
  linkRender,
};

type pathID = {
  source: Path.path,
  target: Path.path,
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
    absPath:
      switch (source) {
      | None => []
      | Some(id) => [id]
      },
  },
  target,
  linkRender,
};

type layout = {
  source: Node.uid,
  target: Node.uid,
};