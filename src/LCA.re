/* Least Common Ancestor algorithm. */
/*

 - input: a kernel node with links in arbitrary locations in the graph
 - output: a kernel node with links at the LCA of their source and target

 - pass 1 (compute paths): build a map from uid to path from root
 - pass 2 (locate links): build a map from LCA path to links
 - pass 3 (place links): place links

 eventually passes 2 and 3 could be combined

  */

module MS = Belt.Map.String;

type node = {
  uid: Node.uid,
  tags: list(Kernel.tag),
  nodes: list(node),
  renderingLinks: list(Link.lcaPath),
  layoutLinks: list(Link.layout),
  layout: (list(Node.uid), MS.t(Node.bbox), list(Link.layout)) => MS.t(Node.transform),
  computeBBox: MS.t(Node.bbox) => Node.bbox,
  render: (list(React.element), Node.bbox, list(React.element)) => React.element,
};

let mapUnion = (m1: MS.t('a), m2: MS.t('a)) => {
  MS.reduce(m2, m1, (m, k, v) => m->MS.set(k, v));
};

/* pass 1 */
/* uid -> path */
let rec computePathMapAux =
        (path, Kernel.{uid, nodes, links, layout, computeBBox, render}): MS.t(Path.path) => {
  let uids = List.map((Kernel.{uid}) => uid, nodes);
  nodes
  |> List.combine(_, uids)
  |> List.map(((n, uid)) => computePathMapAux([uid, ...path], n))
  |> List.fold_left(mapUnion, MS.empty)
  |> MS.set(_, uid, path);
};

let computePathMap = (n: Kernel.node) => computePathMapAux([n.uid], n)->MS.map(List.rev);

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
    Js.log2("p1:", p1 |> Array.of_list);
    Js.log2("p2:", p2 |> Array.of_list);
    raise(failwith("No LCA found.")); /* TODO: improve this error */
  };
};

/* TODO: handle identical paths gracefully */
let computeLocalUID = (p1, p2) => {
  // Js.log3("LCA of", p1 |> Array.of_list, p2 |> Array.of_list);
  switch (p1, p2) {
  | ([h1, ...t1], [h2, ...t2]) when h1 == h2 => computeLocalUIDAux(h1, t1, t2)
  | _ => raise(failwith("Expected both paths to start at the same root node."))
  };
};

let rec computeLocalPathAux = (lca: Node.uid, p1, p2) => {
  switch (p1, p2) {
  | ([], [_])
  | ([_], [])
  | ([_], [_]) => (p1, p2, lca)
  | ([h1, ..._], [h2, ..._]) when h1 != h2 => (p1, p2, lca)
  | ([h1, ...t1], [h2, ...t2]) => computeLocalPathAux(h1, t1, t2)
  | _ =>
    Js.log2("candidate LCA:", lca);
    Js.log2("p1:", p1 |> Array.of_list);
    Js.log2("p2:", p2 |> Array.of_list);
    raise(failwith("No LCA found.")); /* TODO: improve this error */
  };
};

/* TODO: handle identical paths gracefully */
let computeLocalPath = (p1, p2) => {
  // Js.log3("LCA of", p1 |> Array.of_list, p2 |> Array.of_list);
  switch (p1, p2) {
  | ([h1, ...t1], [h2, ...t2]) when h1 == h2 => computeLocalPathAux(h1, t1, t2)
  | _ => raise(failwith("Expected both paths to start at the same root node."))
  };
};

/* pass 2 */
/* path -> local links */
/* TODO: construct a map from uid to a list of links that belong to that uid */
let rec computeLinksMapAux =
        (uidToPath, Kernel.{nodes, links}): MS.t(list((Link.lcaPath, Link.layout))) => {
  let localLinks =
    links
    |> List.map((Link.{source, target, linkRender} as renderingLink: Link.uid) => {
         let (source, target, lca) =
           computeLocalPath(uidToPath->MS.getExn(source), uidToPath->MS.getExn(target));
         (
           (
             Link.{lca, source, target, linkRender},
             Link.{source: List.nth(source, 0), target: List.nth(target, 0)}: Link.layout,
           ),
           lca,
         );
       });
  let linksMap =
    List.fold_left(
      (mp, (links, ancestor)) =>
        mp->MS.update(ancestor, l => {
          switch (l) {
          | None => Some([links])
          | Some(l) => Some([links, ...l])
          }
        }),
      MS.empty,
      localLinks,
    );
  nodes |> List.map(computeLinksMapAux(uidToPath)) |> List.fold_left(mapUnion, linksMap);
};

let computeLinksMap = (uidToPath, n) => computeLinksMapAux(uidToPath, n)->MS.map(List.rev);

/* TODO: use that map to place links in their proper spots */
let rec placeLinks = (localLinks, Kernel.{uid, tags, nodes, links, layout, computeBBox, render}) => {
  let (renderingLinks, layoutLinks) = List.split(localLinks->MS.getWithDefault(uid, []));
  {
    uid,
    tags,
    nodes: List.map(placeLinks(localLinks), nodes),
    renderingLinks,
    layoutLinks,
    layout,
    computeBBox,
    render,
  };
};

let fromKernel = n => {
  let uidToPath = computePathMap(n);
  let pathToLinks = computeLinksMap(uidToPath, n);
  placeLinks(pathToLinks, n);
};

let rec toKernel =
        ({uid, tags, nodes, renderingLinks, layoutLinks: _, layout, computeBBox, render})
        : Kernel.node => {
  uid,
  tags,
  nodes: List.map(toKernel, nodes),
  links: List.map(Link.lcaPathToUID, renderingLinks),
  layout,
  computeBBox,
  render,
};