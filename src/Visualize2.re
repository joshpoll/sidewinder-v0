/* TODO: visualization of the entire structure
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
let rec renderSW =
        (SidewinderKernel.{nodes, links, constraints, render}): SidewinderKernel.renderedNode => {
  /* 1. render nodes (to get bboxes for layout) */
  let renderedNodes = List.map(renderSW, nodes);
  /* 2. layout graph */
  /* let layoutNodes =
     List.map(_ => SetCoLa.{width: 10., height: 10., custom: Js.Obj.empty()}, renderedNodes); */
  let layoutEdges =
    List.map(
      (SidewinderKernel.{source, target, edgeRender: _}) =>
        WebCoLa.{length: None, source: NN(source), target: NN(target)},
      links,
    );
  let graphLayout =
    layoutGraph(
      SetCoLa.{nodes: renderedNodes |> Array.of_list, links: layoutEdges |> Array.of_list},
      constraints,
    );
  /* 3. compute node's bbox (TODO!) */
  /* may want to move bbox rendering into the render function to give the caller the choice */
  /* can push implementation to rectangle module */
  /* 4. render self */
  let layoutNodesNoTemp =
    List.filter((n: WebCoLa.node('a)) => !n.temp, graphLayout.colaNodes |> Array.to_list);
  /* TODO: compute width and height! cf. https://github.com/tgdwyer/WebCola/blob/34433da152b590ba212fc373af608b68110aa6d1/WebCola/src/rectangle.ts */
  render(
    {bbox: Rectangle.make(~x1=0., ~x2=0., ~y1=0., ~y2=0., ()), nodes: layoutNodesNoTemp},
    links,
  );
  /* create temporary bbox-testing container */
};

[@react.component]
let make = (~node) => {
  <div style={ReactDOMRe.Style.make(~position="relative", ())}>
    {renderSW(node).custom##rendered}
  </div>;
};