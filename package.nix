{
  stdenv,
  SDL,
}:
stdenv.mkDerivation {
  name = "fbdoom";
  version = "0-unstable-2025-02-06";

  src = ./.;

  buildInputs = [ SDL ];

  installFlags = [ "PREFIX=${placeholder "out"}" ];
}
