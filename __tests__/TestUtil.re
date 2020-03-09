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
  tags: list(Kernel.tag),
  nodes: list(structure),
  links: list(structureLink),
};

module MI = Belt.Map.String;

let mapUnion = (m1: MI.t('a), m2: MI.t('a)) => {
  MI.reduce(m2, m1, (m, k, v) => m->MI.set(k, v));
};

let rec computeKernelUIDMapAux = (nextId, map, Kernel.{uid, nodes}) => {
  List.fold_left(
    ((nextId, map), n) => {
      let nextMap = computeKernelUIDMapAux(nextId, map, n);
      (nextId + nextMap->MI.size, mapUnion(map, nextMap));
    },
    (nextId + 1, map->MI.set(uid, nextId)),
    nodes,
  )
  |> snd;
};

let computeKernelUIDMap = computeKernelUIDMapAux(0, MI.empty);

let rec computeLCAUIDMapAux = (nextId, map, LCA.{uid, nodes}) => {
  List.fold_left(
    ((nextId, map), n) => {
      let nextMap = computeLCAUIDMapAux(nextId, map, n);
      (nextId + nextMap->MI.size, mapUnion(map, nextMap));
    },
    (nextId + 1, map->MI.set(uid, nextId)),
    nodes,
  )
  |> snd;
};

let computeLCAUIDMap = computeLCAUIDMapAux(0, MI.empty);

let rec kernelToStructureAux = (uidMap, Kernel.{uid, tags, nodes, links} as n) => {
  {
    uid: uidMap->MI.getExn(uid),
    tags,
    nodes: List.map(kernelToStructureAux(uidMap), nodes),
    links:
      List.map(
        ({source, target}: Link.uid) =>
          {source: uidMap->MI.getExn(source), target: uidMap->MI.getExn(target)},
        links,
      ),
  };
};

let kernelToStructure = n => kernelToStructureAux(computeKernelUIDMap(n), n);

let rec lcaToStructureAux = (uidMap, LCA.{uid, nodes, links} as n) => {
  {
    uid: uidMap->MI.getExn(uid),
    tags: [],
    nodes: List.map(lcaToStructureAux(uidMap), nodes),
    links:
      List.map(
        ({source, target}: Link.uid) =>
          {source: uidMap->MI.getExn(source), target: uidMap->MI.getExn(target)},
        links,
      ),
  };
};

let lcaToStructure = n => lcaToStructureAux(computeLCAUIDMap(n));