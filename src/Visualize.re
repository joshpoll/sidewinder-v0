/* /* TODO: visualization of the entire structure
      pre-pass: For every nonlocal link add a hidden link whose source and target are in the LCA node of
      the original link's source and target.

       node rendering (recursive. base case is a node with no children):
         - render children nodes
         - temporarily add to DOM to compute BBox (see https://talk.observablehq.com/t/getbbox/1222/2)
         for each child node
         - compute graph layout using bounding box information, nodes, local links, and constraints
         - render graph using node's render function

       render nonlocal links (what level of the SVG hierarchy should they sit at? check with ELK)
       */

   let f_to_s = Js.Float.toString;

   let tempOpacity = temp => if (temp) {"0.25"} else {"1"};
   /* type node2 = {
        id,
        nodes: list(node2),
        edges: list(edge),
        constraints: setCoLaSpec,
        nodeRender: renderedGraphElements => React.element,
      }; */

   type colaGraph('a) = {
     colaNodes: array(WebCoLa.node('a)),
     colaLinks: array(WebCoLa.link(WebCoLa.node('a))),
   };

   let layoutGraph = (graph, spec) => {
     let result =
       SetCoLa.(
         setCola->nodes(graph.nodes)->links(graph.links)->constraints(spec)->gap(40.)->layout
       );

     let cola =
       WebCoLa.(
         colaLayout()
         ->nodes(result.nodes)
         ->links(result.links)
         ->constraints(result.constraints)
         ->avoidOverlaps(true)
         ->linkDistance(50.)
         ->start(Some(50.), Some(100.), Some(200.), None)
       );
     {colaNodes: WebCoLa.getNodes(cola), colaLinks: WebCoLa.getLinks(cola)};
   };

   /* TODO: maybe should add to the dom at a specified location and return its bounding box? */
   let rec renderSW = (SidewinderKernel.{nodes, edges, constraints, nodeRender}) => {
     let renderedNodes = List.map(renderSW, nodes);
     let layoutNodes =
       List.map(_ => SetCoLa.{width: 10., height: 10., custom: Js.Obj.empty()}, renderedNodes);
     let layoutEdges =
       List.map(
         (SidewinderKernel.{source, target, edgeRender: _}) =>
           WebCoLa.{length: None, source: NN(source), target: NN(target)},
         edges,
       );
     let graphLayout =
       layoutGraph(
         SetCoLa.{nodes: layoutNodes |> Array.of_list, links: layoutEdges |> Array.of_list},
         constraints,
       );
     let combinedNodes = List.combine(renderedNodes, graphLayout.colaNodes |> Array.to_list);
     let renderedNodes =
       List.map(
         ((rn, cn)) =>
           WebCoLa.{
             x: cn.x,
             y: cn.y,
             temp: cn.temp,
             width: cn.width,
             height: cn.height,
             custom: {
               "rendered": rn,
             },
           },
         combinedNodes,
       );
     nodeRender({renderedNodes, renderedEdges: edges});
     /* TODO: this seem really really hard to do in React
         https://stackoverflow.com/questions/49058890/how-to-get-a-react-components-size-height-width-before-render/49058984
         can't measure dom unless rendered, so need to add to dom, measure, then remove
        */
     /* create temporary bbox-testing container */
     /* let bbox = Index.document##createElement("svg");
        bbox##className #= "bbox";
        ReactDOMRe.render(rendered, bbox);
        ReactDOMRe.findDOMNode();
        ReactDOMRe.unmountComponentAtNode(bbox); */
   };

   open WebCoLa;

   [@react.component]
   let make = (~nodes, ~links) => {
     <svg width="1000" height="1000">
       <g transform="translate(200, 200) scale(2)">
         /* TODO: use _temp variable to determine whether or not to render node */
         /* cf. https://github.com/uwdata/setcola/blob/gh-pages/app/renderer.js */

           {Array.mapi(
              (i, {x, y, temp, custom}) =>
                <g
                  key={string_of_int(i)}
                  transform={"translate(" ++ f_to_s(x) ++ ", " ++ f_to_s(y) ++ ")"}>
                  /* <rect
                       width="14"
                       height="14"
                       rx="14"
                       ry="14"
                       style={ReactDOMRe.Style.make(~stroke="white", ~opacity=tempOpacity(temp), ())}
                     /> */

                    <circle
                      r="5"
                      style={ReactDOMRe.Style.make(~stroke="white", ~opacity=tempOpacity(temp), ())}
                    />
                    {if (!temp) {
                       <text>
                          {custom##name} </text>; /* TODO: this is a hack. custom field doesn't exist on temp nodes */
                     } else {
                       <> </>;
                     }}
                  </g>,
              nodes,
            )
            |> React.array}
           {Array.mapi(
              (i, {length: _, source, target}) =>
                <line
                  key={string_of_int(i)}
                  x1={f_to_s(source.x)}
                  y1={f_to_s(source.y)}
                  x2={f_to_s(target.x)}
                  y2={f_to_s(target.y)}
                  style={ReactDOMRe.Style.make(~stroke="black", ())}
                />,
              links,
            )
            |> React.array}
         </g>
     </svg>;
   }; */