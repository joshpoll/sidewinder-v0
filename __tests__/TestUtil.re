/* hack to make jest happy about running this file. should really ignore this file */
Jest.(
  describe("", () => {
    open Expect;
    open! Expect.Operators;

    test("", () =>
      expect(true) === true
    );
  })
);

let dummyLayout = (_, _, _) => [];
let dummyComputeSizeOffset = _ => Rectangle.fromPointSize(~x=0., ~y=0., ~width=0., ~height=0.);

let dummyRender = (_, _, _) => <> </>;

let linkRender = None;

let make = (~tags, ~nodes, ~links) =>
  Main.make(
    ~tags,
    ~nodes,
    ~links,
    ~layout=dummyLayout,
    ~computeSizeOffset=dummyComputeSizeOffset,
    ~render=dummyRender,
  );

let makeAtom = tags => make(~tags, ~nodes=[], ~links=[]);

let counter = ref(0);

let readAndUpdateCounter = () => {
  counter := counter^ + 1;
  counter^ - 1;
};

let makeLCA = (~nodes, ~links) =>
  LCA.{
    uid: string_of_int(readAndUpdateCounter()),
    nodes,
    links,
    layout: dummyLayout,
    computeSizeOffset: dummyComputeSizeOffset,
    render: dummyRender,
  };

type structureLink = {
  source: int,
  target: int,
};

type structure = {
  uid: int,
  tags: array(Kernel.tag),
  nodes: array(structure),
  links: array(structureLink),
};

module MI = Belt.Map.String;

let mapUnion = (m1: MI.t('a), m2: MI.t('a)) => {
  MI.reduce(m2, m1, (m, k, v) => m->MI.set(k, v));
};

let rec computeKernelUIDMapAux = (map, Kernel.{uid, nodes}) => {
  List.fold_left(
    (map, n) => computeKernelUIDMapAux(map, n),
    map->MI.set(uid, map->MI.size),
    nodes,
  );
};

let computeKernelUIDMap = computeKernelUIDMapAux(MI.empty);

let rec computeLCAUIDMapAux = (map, LCA.{uid, nodes}) => {
  List.fold_left(
    (map, n) => computeLCAUIDMapAux(map, n),
    map->MI.set(uid, map->MI.size),
    nodes,
  );
};

let computeLCAUIDMap = computeLCAUIDMapAux(MI.empty);

let rec kernelToStructureAux = (uidMap, Kernel.{uid, tags, nodes, links} as n) => {
  {
    uid: uidMap->MI.getExn(uid),
    tags: Array.of_list(tags),
    nodes: List.map(kernelToStructureAux(uidMap), nodes) |> Array.of_list,
    links:
      List.map(
        ({source, target}: Link.uid) => {
          {
            // Js.log4("source:", source, "->", uidMap->MI.getExn(source));
            // Js.log4("target:", target, "->", uidMap->MI.getExn(target));
            source: uidMap->MI.getExn(source),
            target: uidMap->MI.getExn(target),
          }
        },
        links,
      )
      |> Array.of_list,
  };
};

let kernelToStructure = n => kernelToStructureAux(computeKernelUIDMap(n), n);

let rec lcaToStructureAux = (uidMap, LCA.{uid, nodes, links} as n) => {
  {
    uid: uidMap->MI.getExn(uid),
    tags: [||],
    nodes: List.map(lcaToStructureAux(uidMap), nodes) |> Array.of_list,
    links:
      List.map(
        ({source, target}: Link.uid) =>
          {source: uidMap->MI.getExn(source), target: uidMap->MI.getExn(target)},
        links,
      )
      |> Array.of_list,
  };
};

let lcaToStructure = n => lcaToStructureAux(computeLCAUIDMap(n), n);