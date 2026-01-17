{
  lib,
  stdenv,
  SDL,
  SDL_mixer,
  withSDL ? true,
  libserialport,
  pkg-config,
}:
stdenv.mkDerivation {
  name = "fbdoom";
  version = "0-unstable-2025-02-06";

  src = ./.;

  strictDeps = true;

  nativeBuildInputs = [ pkg-config ];

  buildInputs = [ libserialport ]
  ++ lib.optionals withSDL [ SDL SDL_mixer ];

  buildFlags = lib.optional (!withSDL) "NOSDL=1";

  installFlags = [ "PREFIX=${placeholder "out"}" ];
}
