let dummyLayout = (_, _) => [];
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