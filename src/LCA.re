/* Least Common Ancestor algorithm. */
type node = {
  uid: Node.uid,
  nodes: list(node),
  links: list(Link.uid),
  layout: (Belt.Map.String.t(int), list(Node.sizeOffset), list(Link.uid)) => list(Node.bbox),
  computeSizeOffset: list(Node.bbox) => Node.sizeOffset,
  render: (list(Node.rendered), Node.bbox, list(React.element)) => React.element,
};

/* TODO: test this algorithm!!! */
/* NOTE: The algorithm assumes that links are specified canonically. That is, the path doesn't go up
   the hierarchy only to go back down the way it came. */
/* NOTE: This implementation floats nonlocal links to their LCAs. */

/* NOTE: Don't need to check source node because of link type restriction. */
let isLCALink = ({source, target}: Link.global) =>
  source.ancestorRoot == 0 && target.ancestorRoot == 0;

let lcaToGlobal = ({source, target, linkRender}: Link.lca): Link.global => {
  source: {
    ancestorRoot: 0,
    absPath: source,
  },
  target: {
    ancestorRoot: 0,
    absPath: target,
  },
  linkRender,
};

let globalToLCA =
    ({source: {absPath: apSource}, target: {absPath: apTarget}, linkRender}: Link.global)
    : Link.lca => {
  source: apSource,
  target: apTarget,
  linkRender,
};

module MS = Belt.Map.String;

let mapUnion = (m1: MS.t('a), m2: MS.t('a)) => {
  MS.reduce(m2, m1, (m, k, v) => m->MS.set(k, v));
};

/* let rec addPathsAux =
           (path, {uid, nodes, links, layout, computeSizeOffset, render}: Kernel.node): node => {
     {
       uid,
       path,
       nodes: List.map(addPathsAux([uid, ...path]), nodes),
       links,
       layout,
       computeSizeOffset,
       render,
     };
   };

   let addPaths = addPathsAux([]); */

let rec kernelToLCA = ({uid, nodes, links, layout, computeSizeOffset, render}: Kernel.node) => {
  uid,
  nodes: List.map(kernelToLCA, nodes),
  links,
  layout,
  computeSizeOffset,
  render,
};

let rec computePathMapAux =
        (path, {uid, nodes, links, layout, computeSizeOffset, render}): MS.t(Path.path) =>
  nodes
  |> List.map(computePathMapAux([uid, ...path]))
  |> List.fold_left(mapUnion, MS.empty)
  |> MS.set(_, uid, path);

let computePathMap = n => computePathMapAux([], n)->MS.map(List.rev);

/* TODO: compute LCA from two paths */

/* [b, a], [b, a] => ((a, a), b) */
/* [b], [b] => (None, b) */
/* [c, a], [c, b] => ((a, b), c) */
/* [c, b, a], [c, b] => ((b, b), c) */
/* [b, a], [b] => ((b, b)) */

/* [], [c, b, a], [c, b] => [c], [b, a], [b] */
/* [c], [b, a], [b] => [b, c], [a], [] */
/* [b, c], [a], [] => (a, b) */

let rec computeLocalUIDAux = (lca: Node.uid, p1, p2) => {
  switch (p1, p2) {
  | ([], [h2]) => (lca, h2, lca)
  | ([h1], []) => (h1, lca, lca)
  | ([h1], [h2]) => (h1, h2, lca)
  | ([h1, ...t1], [h2, ...t2]) =>
    if (h1 == h2) {
      computeLocalUIDAux(h1, t1, t2);
    } else {
      (h1, h2, lca);
    }
  | _ =>
    Js.log2("candidate LCA:", lca);
    Js.log2("p1:", p1);
    Js.log2("p2:", p2);
    raise(failwith("No LCA found.")); /* TODO: improve this error */
  };
};

let computeLocalUID = (p1, p2) =>
  switch (p1, p2) {
  | ([h1, ...t1], [h2, ...t2]) when h1 == h2 => computeLocalUIDAux(h1, t1, t2)
  | _ => raise(failwith("Expected both paths to start at the same root node."))
  };

/* TODO: construct a map from uid to a list of links that belong to that uid */
let rec computeLocalLinksAux = (uidToPath, {nodes, links}): MS.t(list(Link.uid)) => {
  let localLinks =
    links
    |> List.map((Link.{source, target}: Link.uid) => {
         let (localSrc, localTarget, ancestor) =
           computeLocalUID(uidToPath->MS.getExn(source), uidToPath->MS.getExn(target));
         (Link.{source: localSrc, target: localTarget, linkRender: None}: Link.uid, ancestor);
       });
  let linksMap =
    List.fold_left(
      (mp, (link, ancestor)) =>
        mp->MS.update(ancestor, l => {
          switch (l) {
          | None => Some([link])
          | Some(l) => Some([link, ...l])
          }
        }),
      MS.empty,
      localLinks,
    );
  nodes |> List.map(computeLocalLinksAux(uidToPath)) |> List.fold_left(mapUnion, linksMap);
};

let computeLocalLinks = n => computeLocalLinksAux(computePathMap(n), n)->MS.map(List.rev);

/* TODO: use that map to place links in their proper spots */
let rec placeLinks = (localLinks, {uid, nodes, links: _, layout, computeSizeOffset, render}) => {
  {
    uid,
    nodes: List.map(placeLinks(localLinks), nodes),
    links: localLinks->MS.getWithDefault(uid, []),
    layout,
    computeSizeOffset,
    render,
  };
};

let propagateLCA = n => placeLinks(computeLocalLinks(n), n);

let fromKernel = n => n |> kernelToLCA |> propagateLCA;

/*
 /* processes a node and returns a layoutNode and the nonlocal links it bubbles up */
 let rec propagateLCAAux =
         ({uid, nodes, links, layout, computeSizeOffset, render}: node): (node, list(Link.uid)) => {
   /* visit nodes first */
   let (lcaNodes, bubblingLinksList) = List.map(propagateLCAAux, nodes) |> List.split;
   /* separate LCA links from other global links */
   let (lcaLinks, globalLinks) =
     [links, ...bubblingLinksList] |> List.flatten |> List.partition(isLCALink);
   (
     {
       nodes: lcaNodes,
       links: lcaLinks |> List.map(globalToLCA),
       layout,
       computeSizeOffset,
       render,
     },
     globalLinks,
   );
 };

 /* NOTE: Final global links list should be empty! */
 let propagateLCA = n => propagateLCAAux(n) |> fst; */