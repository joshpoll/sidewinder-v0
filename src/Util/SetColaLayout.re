type colaGraph('a) = {
  colaNodes: array(WebCoLa.node('a)),
  colaLinks: array(WebCoLa.link(WebCoLa.node('a))),
};

let layout = (~nodes, ~links, ~constraints, ~gap=?, ~linkDistance=?, ()) => {
  let result =
    SetCoLa.setCola
    ->SetCoLa.nodes(nodes)
    ->SetCoLa.links(links)
    ->SetCoLa.constraints(constraints);

  let result =
    switch (gap) {
    | None => result
    | Some(gap) => result->SetCoLa.gap(gap)
    };

  let result = result->SetCoLa.layout;

  let cola =
    WebCoLa.(
      colaLayout()
      ->nodes(result.nodes)
      ->links(result.links)
      ->constraints(result.constraints)
      ->avoidOverlaps(true)
    );

  let cola =
    switch (linkDistance) {
    | None => cola
    | Some(linkDistance) => cola->WebCoLa.linkDistance(linkDistance)
    };

  let cola = cola->WebCoLa.start(Some(50.), Some(100.), Some(200.), None);

  {colaNodes: WebCoLa.getNodes(cola), colaLinks: WebCoLa.getLinks(cola)};
};