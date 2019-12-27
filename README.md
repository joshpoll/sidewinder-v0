# Sidewinder

Sidewinder is the successor to Theia. Its main objective is to allow users to express a wide range
of state+transition abstractions and transformations. To accomplish this, Sidewinder represents
program state using *hierarchical graphs*. We build off existing literature in this space, but find
that no existing representation is entirely suitable for our needs (maybe). Using hierarchical
graphs makes Sidewinder much more expressive and much simpler than Theia. Where Theia had
seven/eight constructs and counting, Sidewinder has just two/three. Theia is a DSL on top of
Sidewinder that covers many abstract machines. This simplicity makes transformation specification
far simpler and more formal (since many papers already address hierarchical graph transformation).
For example, converting a sequence to a tree by adding extra state is difficult to impossible in
Theia, but very natural in Sidewinder. Representing a tool like Oddity might require adding new
nodes to Theia, but can be relatively easily represented in Sidewinder.

Sidewinder uses hierarchical graphs with information about layout and rendering. It lays out the
diagram using SetCoLa, a high-level constraint language for WebCoLa. Authors can write DSLs for
different language domains, which can serve as interfaces to debugger writers and that compile to
Sidewinder specifications. Authors can also write hierarchical graph transformations that are
language-agnostic.

Existing Enabling Ideas:
- Use graphs to represent program structure.
- Constraints allow for expressive graph layout.
- Hierarchical graph representation.
- "Essence of" papers strive to find the kernel of a program representation. Sidewinder aims to do
  that for Theia.

Key Ideas(?):
- Adding a little bit more HTML/CSS rendering to a graph can make it look like a more arbitrary
  diagram. Nearly all graph layout examples I can find are too constrained to using graph layout for
  simple nodes and edges rather than more complex layout.
- There are powerful, simple, language-agnostic transformations and these transformations rely on
  both state and transition information. We only need a few to replicate existing diagrams when
  starting with ground-truth semantics.

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
