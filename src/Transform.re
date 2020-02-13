let rec hide = (tag, Kernel.{tags, nodes, links, layout, computeSizeOffset, render}) =>
  if (List.mem(tag, tags)) {
    None;
  } else {
    Some(
      Kernel.{
        tags,
        nodes:
          List.map(hide(tag), nodes) |> List.filter(x => x != None) |> List.map((Some(x)) => x),
        links, /* TODO: remove associated links, too */
        layout,
        computeSizeOffset,
        render,
      },
    );
  };