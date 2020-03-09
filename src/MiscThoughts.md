TODO: this seems really really hard to do in React
https://stackoverflow.com/questions/49058890/how-to-get-a-react-components-size-height-width-before-render/49058984
can't measure dom unless rendered, so need to add to dom, measure, then remove

TODO: Consider using https://roughjs.com/posts/release-4.0/ and the associated library
https://wiredjs.com/ for making hand-drawn type diagrams.

TODO: For local links, I want to put it on the node containing both nodes. For nonlocal links, I
want to put it on the source node. This suggests a non-uniform representation. It already tripped me
up in an example, since I wanted to put the nonlocal link on the source node. One option could be to
have an ancestorRoot == -1 (or just a separate variant). During LCA this would move the source node
up a level.

TODO: check out https://stackoverflow.com/a/43892905 for resolving React key warning.

TODO: if a target node doesn't exist, because its path is above the root node, then canonicalization
seems to remove it?!? That's really cool.

TODO: global positioning of elements could be an optimization pass. merge multiple graphs together
then hide pieces in each state. that way there is always room for future pieces.

TODO: check out how Penrose computes label size