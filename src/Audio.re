// type t;
// [@bs.new] external make: string => t = "Audio";
// [@bs.send] external play: t => unit = "play";
class type audioInstance =
  [@bs]
  {
    pub play: unit => unit;
  };

type t = Js.t(audioInstance);

[@bs.new] external make: string => t = "Audio";
[@bs.send] external play: t => unit = "play";
