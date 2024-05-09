{ pkgs ? import <nixpkgs> { } }:
let mainPkg = pkgs.callPackage ./default.nix { };
in mainPkg.overrideAttrs (oa: {
  nativeBuildInputs = with pkgs;
    [
      ncurses
    ] ++ (oa.nativeBuildInputs or [ ]);
})
