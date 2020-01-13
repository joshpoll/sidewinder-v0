open WebCoLa;

[@react.component]
let make = (~nodes, ~links) => {
  <svg>

      <g transform="translate(200, 50) scale(2) rotate(90)">
        {Array.map(
           ({x, y}) =>
             <g
               transform={
                 "translate(" ++ Js.Float.toString(x) ++ ", " ++ Js.Float.toString(y) ++ ")"
               }>
               <rect
                 width="14"
                 height="14"
                 rx="14"
                 ry="14"
                 style={ReactDOMRe.Style.make(~stroke="white", ~opacity="1", ())}
               />
             </g>,
           nodes,
         )
         |> React.array}
      </g>
    </svg>;
    // <div style> children </div>;
    /* let (show, setShow) = React.useState(() => true);

       // Notice that instead of `useEffect`, we have `useEffect0`. See
       // reasonml.github.io/reason-react/docs/en/components#hooks for more info
       React.useEffect0(() => {
         let id = Js.Global.setInterval(() => setShow(previousShow => !previousShow), 1000);

         Some(() => Js.Global.clearInterval(id));
       });

       let style =
         if (show) {
           ReactDOMRe.Style.make(~opacity="1", ~transition="opacity 1s", ());
         } else {
           ReactDOMRe.Style.make(~opacity="0", ~transition="opacity 1s", ());
         }; */
};