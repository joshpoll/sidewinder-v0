/* TODO: need to improve id representation */
/* TODO: need to add rendering and constraint information */
/* TODO: how to add transition information? */

type id = string;
type absPath = list(id);
/* relPath's default root is its Graph scope */
/* ancestorRoot acts like prepending .. to path. goes up nesting hierarchy */
/* when ancestorRoot == 0, the path is local */
type relPath = {
  ancestorRoot: int,
  absPath,
};
type edge = {
  source: id,
  target: relPath,
};

type element =
  | Graph(list(node), list(edge))
  | Nest(list(node), list(React.element) => React.element)

and node = {
  id,
  element,
};