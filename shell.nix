{pkgs ? import <nixpkgs> {}}:

pkgs.mkShell {
    buildInputs = with pkgs; [
        arduino-cli
        arduino-language-server
        avrdude
        pkgsCross.avr.buildPackages.gcc
        clang
        pkg-config
        libudev-zero
        msgpack-tools
    ];
}
