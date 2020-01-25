type id = int;
type absPath = list(id);
/**
 * relPath's default root is its Graph scope
 * ancestorRoot acts like prepending .. to path. goes up nesting hierarchy
 * when ancestorRoot == 0, the path is local
 */
type relPath = {
  ancestorRoot: int,
  absPath,
};