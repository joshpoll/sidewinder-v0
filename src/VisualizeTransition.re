type state = {
  prevState: TransitionNode.state,
  currState: TransitionNode.state,
};

let initialState = {prevState: Before, currState: Before};

let toggle = (s: TransitionNode.state): TransitionNode.state =>
  switch (s) {
  | Before => After
  | After => Before
  };

[@react.component]
let make = (~node, ~nodeNext, ~width, ~height, ~debug=false, ~lowerFlow=false) => {
  // let (state, dispatch) = React.useReducer(reducer, initialState);
  let ({prevState, currState}, setState) = React.useState(() => initialState);

  Main.debugLCA(node);
  <>
    <button onClick={_ => setState(_ => {prevState: currState, currState: toggle(currState)})}>
      {switch (currState) {
       | Before => React.string("To After")
       | After => React.string("To Before")
       }}
    </button>
    <svg
      xmlns="http://www.w3.org/2000/svg"
      width={Js.Float.toString(width)}
      height={Js.Float.toString(height)}>
      <g
        transform={
          "translate("
          ++ Js.Float.toString(width /. 2.)
          ++ ", "
          ++ Js.Float.toString(height /. 2.)
          ++ ")"
        }>
        {Main.renderTransition(~debug, ~lowerFlow, ~prevState, ~currState, node, nodeNext)}
      </g>
    </svg>
  </>;
};