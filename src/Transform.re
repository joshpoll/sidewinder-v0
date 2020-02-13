let rec hide = (tag, Kernel.{tags, nodes, links, layout, computeSizeOffset, render}) => {
  let nodes =
    if (List.mem(tag, tags)) {
      [];
    } else {
      List.map(hide(tag), nodes);
    };
  Kernel.{tags, nodes, links, layout, computeSizeOffset, render};
};