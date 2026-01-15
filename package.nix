{
  stdenv,
  SDL,
  SDL_mixer,
  libserialport,
  pkg-config,
}:
stdenv.mkDerivation {
  name = "fbdoom";
  version = "0-unstable-2025-02-06";

  src = ./.;

  nativeBuildInputs = [ pkg-config ];

  buildInputs = [ SDL SDL_mixer libserialport ];

  installFlags = [ "PREFIX=${placeholder "out"}" ];
}
