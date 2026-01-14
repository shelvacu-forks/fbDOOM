{
  inputs.nixpkgs.url = "nixpkgs/nixos-unstable-small";

  outputs = { nixpkgs, self }:
  let
    system = "x86_64-linux";
    pkgs = import nixpkgs { inherit system; };
  in
  {
    legacyPackages.${system} = { inherit pkgs; };
    packages.${system} = rec {
      fbdoom = pkgs.callPackage ./package.nix { };
      default = fbdoom;
    };
  };
}
