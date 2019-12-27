/* TODO: need to improve id representation */
/* TODO: need to add rendering and constraint information */

type id = string;
type edge = {
  source: id,
  target: id,
};

type element =
  | Graph(list(node), list(edge))
  | Atom(React.element)
  | Ptr(id) /* TODO: still not sure whether this shoudl be its own variant or if it should be folded
  into the graph. This variant is intended for nonlocal pointers. */

and node = {
  id,
  element,
};