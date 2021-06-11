open Webapi.Dom;

type sequence = list(Colorz.colors);

type state = {
  sequence,
  level: int,
  active: option(Colorz.colors),
  input: list(Colorz.colors),
  isStrict: bool,
  isPlaying: bool,
};

type action =
  | SetSequence(sequence)
  | PlaySequence
  | PlaySound(Colorz.colors)
  | ResetColor
  | Input(Colorz.colors)
  | CheckInput(state)
  | Reset
  | SetStrictness
  | SetPlaying;

module Styles = {
  open Css;

  global(
    "body",
    [
      fontFamily(
        `custom(
          "-apple-system,BlinkMacSystemFont,\"Segoe UI\",Roboto,Oxygen-Sans,Ubuntu,Cantarell,\"Helvetica Neue\",sans-serif",
        ),
      ),
    ],
  );

  let container =
    style([
      display(`flex),
      justifyContent(`center),
      alignItems(`center),
      minHeight(`vh(100.0)),
      flexDirection(`column),
    ]);

  let boxes =
    style([
      display(`flex),
      flexWrap(`wrap),
      maxWidth(`px(500)),
      maxHeight(`px(500)),
    ]);

  let box = (~bgColor: Colorz.colors, ~active: option(Colorz.colors)) => {
    let baseStyle = [
      minHeight(`px(250)),
      minWidth(`px(250)),
      border(`px(0), `none, `transparent),
    ];

    let opacity =
      switch (bgColor, active) {
      | (Green, Some(Green)) => opacity(0.5)
      | (Red, Some(Red)) => opacity(0.5)
      | (Blue, Some(Blue)) => opacity(0.5)
      | (Yellow, Some(Yellow)) => opacity(0.5)
      | (_, None) => opacity(1.0)
      | (_, Some(_)) => opacity(1.0)
      };

    let bgColor =
      switch (bgColor) {
      | Green => backgroundColor(`hex("07f767"))
      | Red => backgroundColor(`hex("f95e59"))
      | Blue => backgroundColor(`hex("00bcea"))
      | Yellow => backgroundColor(`hex("f4ed7c"))
      };

    style([bgColor, opacity, ...baseStyle]);
  };
  let controls = style([marginTop(`px(10)), textAlign(`center)]);
  let buttons = style([marginTop(`px(10))]);
};

let makeSequence = (~len=5, ()) =>
  Belt.List.makeBy(
    len,
    _i => {
      open Colorz;
      let num = Js.Math.floor(Js.Math.random() *. 4.0 +. 1.0);
      switch (num) {
      | 1 => Green
      | 2 => Red
      | 3 => Blue
      | 4 => Yellow
      | _ => Green
      };
    },
  );

let initialState = {
  sequence: [],
  level: 1,
  active: None,
  input: [],
  isStrict: false,
  isPlaying: false,
};
[@react.component]
let make = () => {
  let (state, dispatch) =
    React.useReducer(
      (state, action) =>
        switch (action) {
        | SetSequence(list) => {...state, sequence: list}
        | PlaySequence => {...state, isPlaying: true}
        | PlaySound(color) => {...state, active: Some(color)}
        | ResetColor => {...state, active: None}
        | Input(color) => {...state, input: [color, ...state.input]}
        | CheckInput(state) => state
        | Reset => {...state, input: [], level: 1}
        | SetStrictness => {...state, isStrict: !state.isStrict}
        | SetPlaying => {...state, isPlaying: !state.isPlaying}
        },
      initialState,
    );

  React.useEffect0(() => {
    let list = makeSequence();
    dispatch(SetSequence(list));
    None;
  });

  let playSound = color => {
    let sound =
      Belt.List.getAssoc(Sounds.map, color, (==))
      ->Belt.Option.getWithDefault(Sounds.green);
    sound##play();
    let _id = Js.Global.setTimeout(() => dispatch(ResetColor), 300);

    // Js.Global.clearTimeout(_id);
    dispatch(PlaySound(color));
  };
  let playSequence = () => {
    let l =
      Belt.List.take(state.sequence, state.level)
      ->Belt.Option.getWithDefault([]);

    Belt.List.forEachWithIndex(
      l,
      (index, color) => {
        let _id =
          Js.Global.setTimeout(() => playSound(color), (index + 1) * 1000);
        ();
      },
    );

    let _id =
      Js.Global.setTimeout(
        () => dispatch(SetPlaying),
        state.level * 1000 + 300,
      );
    // Js.Global.clearTimeout(_id);
  };
  let checkInput = () => {
    let {level, input, sequence, isStrict} = state;
    let currentUserColor = Belt.List.headExn(input);
    let inputLength = Belt.List.length(input);
    let currentSequenceColor = Belt.List.getExn(sequence, inputLength - 1);
    let isEnd = inputLength === Belt.List.length(sequence);
   let state = switch (
      currentUserColor === currentSequenceColor,
      inputLength === level,
      isStrict,
      isEnd,
    ) {
    | (false, _, false, _) =>
      Sounds.error##play();
      playSequence();
      {...state, input: []};
    | (false, _, true, _) =>
      Sounds.error##play();
      playSequence();
      {...state, input: [], level: 1};
    | (true, false, _, false) =>
      playSound(currentUserColor);
      state;
    | (true, true, _, false) =>
      playSound(currentUserColor);
      playSequence();
      {...state, input: [], level: state.level + 1};
    | (true, _, _, true) =>
      let list = makeSequence();

      playSound(currentUserColor);
      let _id =
        Js.Global.setTimeout(() => Window.alert("You won!", window), 400);
      // Js.Global.clearTimeout(_id);

      {...state, input: [], level: 1, sequence: list};
    };
  dispatch(CheckInput(state))
  };
  let input = color => {
    Js.log(checkInput());
    checkInput()->ignore;
    dispatch(Input(color));
  };
  let {level, active, isStrict, isPlaying} = state;
  <div className=Styles.container>
    <h1> "Simon Game in ReasonReact"->React.string </h1>
    <div className=Styles.boxes>
      <button
        type_="button"
        className={Styles.box(~bgColor=Green, ~active)}
        onClick={_e => input(Green)}
        disabled=isPlaying
      />
      <button
        type_="button"
        className={Styles.box(~bgColor=Red, ~active)}
        onClick={_e => input(Red)}
        disabled=isPlaying
      />
      <button
        type_="button"
        className={Styles.box(~bgColor=Blue, ~active)}
        onClick={_e => input(Blue)}
        disabled=isPlaying
      />
      <button
        type_="button"
        className={Styles.box(~bgColor=Yellow, ~active)}
        onClick={_e => input(Yellow)}
        disabled=isPlaying
      />
    </div>
    <div className=Styles.controls>
      <div>
        <span> "Strict"->React.string </span>
        <input
          type_="checkbox"
          checked=isStrict
          onChange={_e => dispatch(SetStrictness)}
        />
      </div>
      <div> {{j|Level: $level|j} |> React.string} </div>
      <div className=Styles.buttons>
        <button onClick={_e => playSequence()} disabled=isPlaying>
          {"Start" |> React.string}
        </button>
        <button onClick={_e => dispatch(Reset)} disabled=isPlaying>
          "Reset"->React.string
        </button>
      </div>
    </div>
  </div>;
};
