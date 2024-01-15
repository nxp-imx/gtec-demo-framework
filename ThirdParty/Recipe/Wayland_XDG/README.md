# XDG Shell

Recipe for building XDG shell header and glue code library using [wayland-scanner](https://wayland-book.com/libwayland/wayland-scanner.html).
As the library does not come with an official build solution we have our own cmake based one.
This work is based on https://github.com/KhronosGroup/VK-GL-CTS/commit/745d68215ebcfaf7fdb2a460447a0b6da3fda85a.

This requires "pkg-config libwayland-dev wayland-protocols" on Ubuntu
