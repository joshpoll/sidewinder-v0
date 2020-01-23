/**
 * Compute layout of the graph.
 */
open SidewinderKernel;

/* let layout = ({nodes, links, constraints, gap, linkDistance, render}) => {
     let result = SetCoLa.(setCola->nodes(nodes)->links(links)->constraints(spec));

     let result =
       switch (g) {
       | None => result
       | Some(g) => result->SetCoLa.gap(g)
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
       switch (ld) {
       | None => cola
       | Some(ld) => cola->WebCoLa.linkDistance(ld)
       };

     let cola = cola->WebCoLa.start(Some(50.), Some(100.), Some(200.), None);

     {colaNodes: WebCoLa.getNodes(cola), colaLinks: WebCoLa.getLinks(cola)};
   }; */