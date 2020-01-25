/* Least Common Ancestor algorithm. */
module Kernel = SideWinder_Kernel;

type node = {
  nodes: list(node),
  links: list(Link.lca),
  layout: (list(Node.size), list(Link.local)) => list(Node.bbox),
  computeSize: list(Node.bbox) => Node.size,
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

/* processes a node and returns a layoutNode and the nonlocal links it bubbles up */
let rec propagateLCAAux =
        ({nodes, links, layout, computeSize, render}: Kernel.node): (node, list(Link.global)) => {
  /* visit nodes first */
  let (lcaNodes, bubblingLinksList) = List.map(propagateLCAAux, nodes) |> List.split;
  /* update link paths in bubblingLinksList */
  let bubblingLinksList =
    List.mapi(
      i =>
        List.map(
          (
            {
              source: {ancestorRoot: sourceRoot, absPath: sourcePath},
              target: {ancestorRoot: targetRoot, absPath: targetPath},
              linkRender,
            }: Link.global,
          ) =>
          (
            {
              source: {
                ancestorRoot: sourceRoot,
                absPath: [i, ...sourcePath],
              },
              target: {
                ancestorRoot: targetRoot - 1,
                absPath: targetPath,
              },
              linkRender,
            }: Link.global
          )
        ),
      bubblingLinksList,
    );
  /* separate LCA links from other global links */
  let (lcaLinks, globalLinks) =
    [links, ...bubblingLinksList] |> List.flatten |> List.partition(isLCALink);
  (
    {nodes: lcaNodes, links: lcaLinks |> List.map(globalToLCA), layout, computeSize, render},
    globalLinks,
  );
};

/* NOTE: Final global links list should be empty! */
let propagateLCA = n => propagateLCAAux(n) |> fst;