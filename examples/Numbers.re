type prop = {
  opacity: string,
  transform: string,
};
module TransitionHooks =
  Spring.MakeTransition({
    type t = prop;
    type item = int;
  });

let pages = [|
  (style, key) =>
    <Spring.Div
      key
      style={ReactDOMRe.Style.make(
        ~opacity=style.opacity,
        ~transform=style.transform,
        ~backgroundColor="lightpink",
        (),
      )}>
      "1"->React.string
    </Spring.Div>,
  (style, key) =>
    <Spring.Div
      key
      style={ReactDOMRe.Style.make(
        ~opacity=style.opacity,
        ~transform=style.transform,
        ~backgroundColor="lightblue",
        (),
      )}>
      "2"->React.string
    </Spring.Div>,
  (style, key) =>
    <Spring.Div
      key
      style={ReactDOMRe.Style.make(
        ~opacity=style.opacity,
        ~transform=style.transform,
        ~backgroundColor="lightgreen",
        (),
      )}>
      "3"->React.string
    </Spring.Div>,
|];
[@react.component]
let make = () => {
  let (indexes, setIndexes) = React.useState(_ => [|0|]);
  /* encapsulated */
  let transitions =
    TransitionHooks.use(
      indexes,
      item => item->string_of_int,
      TransitionHooks.config(
        ~from={opacity: "0", transform: "translate3d(100%,0,0)"},
        ~enter={opacity: "1", transform: "translate3d(0%,0,0)"},
        ~leave={opacity: "0", transform: "translate3d(-50%,0,0)"},
        ~config=Spring.config(~tension=200., ()),
        (),
      ),
    );
  /* doesn't have to do with transitions */
  let onClick =
    React.useCallback0(_ =>
      setIndexes(state => {
        let index = state->Belt.Array.get(0)->Belt.Option.getWithDefault(0);
        [|(index + 1) mod 3|];
      })
    );
  <div className="numbers">
    <div className="simple-trans-main" onClick>
      {Array.map(
         ({TransitionHooks.item, props, key}) => {
           let page = Array.unsafe_get(pages, item);
           // <Page key={key} style={props} />
           page(props, key);
         },
         transitions,
       )
       |> React.array}
    </div>
  </div>;
};