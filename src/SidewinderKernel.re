/* TODO: need to improve id representation */
/* TODO: need to add rendering and constraint information */
/* TODO: how to add transition information? */

type id = string;
type path = list(id);
type edge = {
  /* paths are relative to the Graph scope. Makes local edges nice to define. */
  /* Local edges can connect children of top-level nodes in the Graph */
  /* This is currently expressive enough to subsume Ptr. Still not sure if Ptr is needed. */
  source: path,
  target: path,
};

type element =
  | Graph(list(node), list(edge))
  | Atom(React.element)
  | Ptr(path) /* TODO: still not sure whether this shoudl be its own variant or if it should be folded
  into the graph. This variant is intended for nonlocal pointers. */

and node = {
  id,
  element,
};