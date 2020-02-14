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

let makeLCA = (~nodes, ~links) =>
  LCA.{
    nodes,
    links,
    layout: dummyLayout,
    computeSizeOffset: dummyComputeSizeOffset,
    render: dummyRender,
  };