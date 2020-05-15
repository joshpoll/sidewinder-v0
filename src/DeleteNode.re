/* rendering for nodes that vanish */
open SpringHelper;

module SpringHook =
  Spring.MakeSpring({
    type t = float;
    type interpolate = float => float;
  });

[@react.component]
let make =
    (
      ~renderedElem: React.element,
      ~prevState: TransitionNode.state,
      ~currState: TransitionNode.state,
    ) => {
  let initPos =
    switch (prevState) {
    | Before => 1.
    | After => 0.
    };

  let (values, setValues) =
    SpringHook.use(~config=Spring.config(~mass=1., ~tension=80., ~friction=20.), initPos);
  switch (currState) {
  | Before => setValues(1.)
  | After => setValues(0.)
  };

  <G style={ReactDOMRe.Style.make(~opacity=values->SpringHook.interpolate(x => x), ())}>
    renderedElem
  </G>;
};