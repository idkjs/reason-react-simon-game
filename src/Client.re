[@bs.val]
external querySelector: string => option(Dom.element) =
  "window.document.querySelector";
Js.log(querySelector("#root"));
// switch (querySelector("#root")) {
// | Some(root) => ReactDOMRe.renderToElementWithId(<App />, "root")
// | None => Js.log("did not find root")
// };
ReactDOMRe.renderToElementWithId(<App />, "root")
