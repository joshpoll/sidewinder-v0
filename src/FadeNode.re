/* rendering for nodes that vanish */
open SpringHelper;

module SpringHook =
  Spring.MakeSpring({
    type t = float;
    type interpolate = float => float;
  });

type fadeDir =
  | Out
  | In;

[@react.component]
let make =
    (
      ~fadeDir: fadeDir,
      ~renderedElem: React.element,
      ~prevState: TransitionNode.state,
      ~currState: TransitionNode.state,
    ) => {
  let initPos =
    switch (prevState, fadeDir) {
    | (Before, Out) => 1.
    | (After, Out) => 0.
    | (Before, In) => 0.
    | (After, In) => 1.
    };

  let (values, setValues) =
    SpringHook.use(~config=Spring.config(~mass=1., ~tension=300., ~friction=26.), initPos);
  switch (currState, fadeDir) {
  | (Before, Out) => setValues(1.)
  | (After, Out) => setValues(0.)
  | (Before, In) => setValues(0.)
  | (After, In) => setValues(1.)
  };

  <G style={ReactDOMRe.Style.make(~opacity=values->SpringHook.interpolate(x => x), ())}>
    renderedElem
  </G>;
};