# Sidewinder

Sidewinder is the successor to Theia. Its main objective is to allow users to express a wide range
of state+transition abstractions and transformations. To accomplish this, Sidewinder represents
program state using *compound graphs*. We build on and are inspired by the existing hierarchical and
compound graph literature, with Eclipse's ELK and T.W. Pratt's H-Graph probably being the closest
prior work. Using compound graphs makes Sidewinder much more expressive and much simpler than Theia.
Where Theia had seven/eight constructs and counting, Sidewinder has just one (albeit a compound
graph is more complex than any of Theia's constructs). Theia is now a DSL on top of Sidewinder that
covers many abstract machines, making it a suitable target for debugger writers.

Sidewinder's simplicity makes transformation specification far simpler and more formal. We can now
rely on the existing hierarchical/compound graph literature for this formalism. For example,
converting a sequence to a tree via an IR transformation would be possible in Theia, but doesn't
seem very natural. When should one construct turn into the other? Why are some constructs (like map)
easily expressible as others (like sequence)? Sidewinder makes these transformations more natural.
They always send compound graphs to compound graphs. Another example: Representing a tool like
Oddity might require adding new nodes to Theia, but can be relatively easily represented in
Sidewinder.

Unlike Theia, Sidewinder makes it easy for authors to customize its output. To customize Theia, an
author needed to change the actual source code of the renderer. Even then, one could only change the
look of _every_ value or _every_ sequence in an output, not the look of individual nodes, unless
they passed in some custom parameters. In contrast, Sidewinder exposes both layout and rendering to
authors, affording ubiquitous customization. Every node has custom rendering and layout. Every link
has custom rendering.

With this system, authors can easily write DSLs simply by writing functions that construct pieces of
compound graphs with their own custom layout and rendering. These DSLs can serve as interfaces to
debugger writers, who can be oblivious to Sidewinder's compound graph representation. Moreover, this
approach allows authors to write language-agnostic compound graph transformations.

Existing Enabling Ideas:
- Use graphs to represent program structure.
- Compound and hierarchical graph representations.
- "Essence of" papers strive to find the kernel of a program representation. Sidewinder aims to do
  that for Theia.

Key Ideas(?):
- We catalog the **transformations/abstractions** necessary to recreate SML diagrams used in 341. This
  process has two uses:
  - We produce powerful, simple, language-agnostic transformations, and these transformations rely
    on both state and transition information. We only need a few to replicate existing diagrams when
    starting with ground-truth semantics.
  - We catalog the types of abstractions instructors use. This is an interesting area for future
    work. We could study what abstractions people use in practice and how often, now that we can
    talk about them rigorously!
- **Algorithmic contribution**: To make layout predictable and reasonably stable under common transformations, we restrict layout
  to local nodes in a special way (see LCA pass). Links are specified in the compound graph using
  relative paths. The link definition is then "bubbled" up to the LCA of the source and target
  nodes, treated as local to that node for layout, and then rendered normally.
- Based on prior work (ELK), we can use a uniform compound graph representation to visualize structures that don't seem very graph-like at all! [not a contribution]



Related Work
- Comparison to T.W. Pratt's H-Graph from "Definition of Programming Language Semantics Using
  Grammars for Hierarchical Graphs" and also another paper of his (forgot which one).
  - "Within an H-graph, a particular directed graph may only exist as the value of some node, and
    the arcs and initial node designation are "local" to that graph. The nodes, however, are not
    local to the graph; a node is inherently "global" in that it may appear in many different graphs
    within an H-graph, and its value is independent of the particular graph within which it is
    found. Global nodes combined with local graphs in H-graphs are important for natural modeling of
    many programming language structures." Pratt 78
  - Pratt justifies this choice by noting that data (atoms) may be shared by multiple data
    structures (nested graphs) and thus atoms must be shared.
  - Sidewinder takes the opposite approach: nodes are local but edges may be nonlocal. We believe
    this more accurately models existing diagrams. Typically, people visualize shared data using
    global pointers to a single region (e.g. heap) rather than a global node in multiple region.
    Moreover, Pratt's Figure 1, which visualizes the Lisp expression `(A (B (C A)) D)`, can be
    replicated in Sidewinder, because the debugger and language semantics are responsible for
    remembering that the two `A`'s are the same. Sidewinder can be oblivious to this and work just
    fine without a global pointer to a shared data structure.
    - Addendum: Sidewinder's tracking of data flow can also capture that two A's are the same if
      they are labeled as such by the debugger. Also, there are cases where one *would* like the same
      node to appear in multiple graphs, such as when layering two visualizations. This is an area
      for future work.
  - H-Graphs are useful for defining semantics. Sidewinder is good for visualizing them.

## ReasonReact Template & Examples

This is:
- A template for your new ReasonReact project.
- A collection of thin examples illustrating ReasonReact usage.
- Extra helper documentation for ReasonReact (full ReasonReact docs [here](https://reasonml.github.io/reason-react/)).

`src` contains 4 sub-folders, each an independent, self-contained ReasonReact example. Feel free to delete any of them and shape this into your project! This template's more malleable than you might be used to =).

The point of this template and examples is to let you understand and personally tweak the entirely of it. We **don't** give you an opaque, elaborate mega build setup just to put some boxes on the screen. It strikes to stay transparent, learnable, and simple. You're encouraged to read every file; it's a great feeling, having the full picture of what you're using and being able to touch any part.

### Run

```sh
npm install
npm run server
# in a new tab
npm start
```

Open a new web page to `http://localhost:8000/`. Change any `.re` file in `src` to see the page auto-reload. **You don't need any bundler when you're developing**!

**How come we don't need any bundler during development**? We highly encourage you to open up `index.html` to check for yourself!

## Features Used

|                           | Blinking Greeting | Reducer from ReactJS Docs | Fetch Dog Pictures | Reason Using JS Using Reason |
|---------------------------|------------------|----------------------------|--------------------|------------------------------|
| No props                  |                  | ✓                          |                    |                             |
| Has props                 |                  |                            |                    | ✓                           |
| Children props            | ✓                |                            |                    |                             |
| No state                  |                  |                            |                    | ✓                           |
| Has state                 | ✓                |                            |  ✓                 |                             |
| Has state with useReducer |                  | ✓                          |                    |                             |
| ReasonReact using ReactJS |                  |                            |                    | ✓                           |
| ReactJS using ReasonReact |                  |                            |                    | ✓                           |
| useEffect                 | ✓                |                            |  ✓                |                             |
| Dom attribute             | ✓                | ✓                          |                   | ✓                           |
| Styling                   | ✓                | ✓                          |  ✓                | ✓                           |
| React.array               |                  |                            |  ✓                 |                             |

## Bundle for Production

We've included a convenience `UNUSED_webpack.config.js`, in case you want to ship your project to production. You can rename and/or remove that in favor of other bundlers, e.g. Rollup.

We've also provided a barebone `indexProduction.html`, to serve your bundle.

```sh
npm install webpack webpack-cli
# rename file
mv UNUSED_webpack.config.js webpack.config.js
# call webpack to bundle for production
./node_modules/.bin/webpack
open indexProduction.html
```
