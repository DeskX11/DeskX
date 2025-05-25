{
  pkgs ? import <nixpkgs> { },
  packageSrc ? ./.,
}:

pkgs.stdenv.mkDerivation {
  pname = "deskx";
  version = "2.0.2";

  src = packageSrc;

  nativeBuildInputs = [ pkgs.pkg-config ];

  buildInputs = with pkgs; [
    # x11
    xorg.libX11
    xorg.libXtst
    xorg.libXext
    xorg.libXi

    # wayland
    libportal

    pipewire
    SDL2
  ];

  installPhase = ''
    cp deskx $out
  '';
}
