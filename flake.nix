{
  description = "Flake for DeskX app";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-parts = {
      url = "github:hercules-ci/flake-parts";
      inputs.nixpkgs-lib.follows = "nixpkgs";
    };
  };

  outputs =
    inputs@{ flake-parts, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } (
      { self, ... }:
      {
        systems = [
          "x86_64-linux"
          "aarch64-linux"
        ];
        perSystem =
          {
            config,
            pkgs,
            ...
          }:
          {
            packages.deskx = pkgs.callPackage ./default.nix { packageSrc = self; };
            packages.default = config.packages.deskx;
          };
      }
    );
}
