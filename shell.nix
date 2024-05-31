{ pkgs ? import <nixpkgs> { } }:
let mainPkg = pkgs.callPackage ./default.nix { };
in mainPkg.overrideAttrs (oa: {
  nativeBuildInputs = with pkgs;
    [
      ncurses
      valgrind
      gdb
    ] ++ (oa.nativeBuildInputs or [ ]);
})
