/* // In this Interop example folder, we have:
// - A ReasonReact component, ReasonReactCard.re
// - Used by a ReactJS component, ReactJSCard.js (this file)
// - ReactJSCard.js can be used by ReasonReact, through bindings in ReasonUsingJSUsingReason.re
// - ReasonUsingJSUsingReason.re is used by Index.re

var ReactDOM = require('react-dom');
var React = require('react');
var WebCola = require('react-cola');
var ReactLineTo = require('react-lineto');
var Line = ReactLineTo.Line;

// var ReasonReactCard = require('./ReasonReactCard2.bs').make;

// var ReactJSComponent = function () {
//   let backgroundColor = "rgba(0, 0, 0, 0.05)";
//   let padding = "12px";

//   // We're not using JSX here, to avoid folks needing to install the related
//   // React toolchains just for this example.
//   // <div style={...}>
//   //   <div style={...}>This is a ReactJS card</div>
//   //   <ReasonReactCard style={...} />
//   // </div>
//   return React.createElement(
//     "div",
//     { style: { backgroundColor, padding, borderRadius: "8px" } },
//     React.createElement("div", { style: { marginBottom: "8px" } }, "This is a ReactJS card"),
//     React.createElement(ReasonReactCard, { style: { backgroundColor, padding, borderRadius: "4px" } }),
//   )
// };
// ReactJSComponent.displayName = "MyBannerFOO";

// module.exports = ReactJSComponent;

var myComponent = function () {
  return React.createElement(WebCola, {
    renderLayout: layout => (React.createElement("", [layout.groups().map(
      ({ bounds: { x, X, y, Y } }, i) => {
        const width = X - x;
        const height = Y - y;
        return (
          React.createElement("div", {
            key: i, style: {
              position: 'absolute',
              left: x,
              top: y,
              width,
              height,
              backgroundColor: 'orange',
              borderRadius: 5,
              zIndex: -2,
            }
          })
        );
      },
    ), layout.links().map(
      ({ source, target }, i) => {
        const { x, y } = source;
        const { x: x2, y: y2 } = target;
        return (
          React.createElement(Line, {
            key: i,
            x0: x,
            y0: y,
            x1: x2,
            y1: y2,
            borderColor: "blue",
            zIndex: -1
          })
        );
      }), layout.nodes().map(
        ({ x, y, width, height, name }, i) => (
          React.element("div",
            {
              key: i,
              style: {
                position: 'absolute',
                left: x - width * 0.5,
                top: y - height * 0.5,
                width,
                height,
                backgroundColor: 'red',
                borderRadius: 5,
              }
            }
            ,
            name
          ))
      )])),
    nodes:
      [
        {
          width: 60,
          height: 40,
          name: 'a',
        },
        {
          width: 60,
          height: 40,
          name: 'b',
        },
        {
          width: 60,
          height: 40,
          name: 'c',
        },
        {
          width: 60,
          height: 40,
          name: 'd',
        },
        {
          width: 60,
          height: 40,
          name: 'e',
        },
        {
          width: 60,
          height: 40,
          name: 'f',
        },
        {
          width: 60,
          height: 40,
          name: 'g',
        },
      ],
    links:
      [
        { source: 1, target: 2 },
        { source: 2, target: 3 },
        { source: 3, target: 4 },
        { source: 0, target: 1 },
        { source: 2, target: 0 },
        { source: 3, target: 5 },
        { source: 0, target: 5 },
      ],
    groups:
      [
        { leaves: [0], groups: [1] },
        { leaves: [1, 2] },
        { leaves: [3, 4] },
      ],
    width: 540,
    height: 760,
  })
  /* <WebCola
      renderLayout={layout => (
        <>
          {layout.groups().map(
            ({ bounds: { x, X, y, Y } }, i) => {
              const width = X - x;
              const height = Y - y;
              return (
                <div
                  key={i}
                  style={{
                    position: 'absolute',
                    left: x,
                    top: y,
                    width,
                    height,
                    backgroundColor: 'orange',
                    borderRadius: 5,
                    zIndex: -2,
                  }}
                />
              );
            },
          )}
          {layout.links().map(
            ({ source, target }, i) => {
              const { x, y } = source;
              const { x: x2, y: y2 } = target;
              return (
                <Line
                  key={i}
                  x0={x}
                  y0={y}
                  x1={x2}
                  y1={y2}
                  borderColor="blue"
                  zIndex={-1}
                />
              );
            },
          )}
          {layout.nodes().map(
            ({ x, y, width, height, name }, i) => (
              <div
                key={i}
                style={{
                  position: 'absolute',
                  left: x - width * 0.5,
                  top: y - height * 0.5,
                  width,
                  height,
                  backgroundColor: 'red',
                  borderRadius: 5,
                }}
              >
                {name}
              </div>
            ),
          )}
        </>
      )}
      nodes={[
        {
          width: 60,
          height: 40,
          name: 'a',
        },
        {
          width: 60,
          height: 40,
          name: 'b',
        },
        {
          width: 60,
          height: 40,
          name: 'c',
        },
        {
          width: 60,
          height: 40,
          name: 'd',
        },
        {
          width: 60,
          height: 40,
          name: 'e',
        },
        {
          width: 60,
          height: 40,
          name: 'f',
        },
        {
          width: 60,
          height: 40,
          name: 'g',
        },
      ]}
      links={[
        { source: 1, target: 2 },
        { source: 2, target: 3 },
        { source: 3, target: 4 },
        { source: 0, target: 1 },
        { source: 2, target: 0 },
        { source: 3, target: 5 },
        { source: 0, target: 5 },
      ]}
      groups={[
        { leaves: [0], groups: [1] },
        { leaves: [1, 2] },
        { leaves: [3, 4] },
      ]}
      width={540}
      height={760}
    /> */
};

module.exports = myComponent; * /