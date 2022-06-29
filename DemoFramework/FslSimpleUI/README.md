# FslSimpleUI

A simple UI framework that makes it easy to get a basic UI up and running. The main code is API independent. It is not a show case of how to render a UI fast but only intended to allow you to quickly get a UI ready that is good enough for a demo.

For new samples its recommended to utilize the new basic theme which will give you access to preconfigured UI controls that fits a simple theme. This also allows us to update the themes for all apps in one location.

Take a look at one of these apps for examples:

* DpiScale
* PixelPerfect
* SmoothScroll
* ThemeBasicUI

If you want to see a example of how to use the basic UI controls from scratch you can look at one of these samples:

* DFSimpleUI100
* DFSimpleUI101

## Recommendations

When working with the UI system its recommended to store all or at least the most used bitmaps in the same texture atlas.
The texture atlases used in our samples are defined by ```TexturePacker.xml``` files and the freely available [gtec-texture-packer](https://github.com/NXPmicro/gtec-texture-packer) can be used to generate the actual atlas files.

## More details

For detailed information about designing a custom UI read [this document](../../Doc/FslSimpleUI.md).
