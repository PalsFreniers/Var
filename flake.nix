{
  description = "dev-env";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    flake-utils = {
      url = "github:numtide/flake-utils";
    };
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils
  }: flake-utils.lib.eachDefaultSystem
    (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        stdenv = pkgs.stdenvAdapters.useMoldLinker pkgs.llvmPackages_19.stdenv;
      in
      {
        devShell = (pkgs.mkShell.override { inherit stdenv; }) {
          nativeBuildInputs = with pkgs; [
            valgrind
            gdb
          ];
        };
      }
    );
}

