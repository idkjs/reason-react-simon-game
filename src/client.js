import React from "react";
import ReactDOM from "react-dom";

import { make as App } from "../lib/es6_global/src/App.bs";

ReactDOM.render(<App />, document.querySelector("#root"));
