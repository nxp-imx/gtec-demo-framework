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
One commercially available texture packer is [Texture Packer](https://www.codeandweb.com/texturepacker) which can output a
json file that we can convert to a binary format that can be loaded by the demo framework.  

If you look at the DFSimpleUI100 sample its based on the texture atlas at `Resources/Source/DFSimpleUI101/TextureAtlas` directory which contain a `MainAtlas.tps` file that can be loaded into texture packer. Pressing publish in texture packer produces a `MainAtlas.png` and `MainAtlas.json` file based on the files under `Main`. The `MainAtlas.png` can be copied directly to the samples `Content` directory but the json file needs to be converted to a binary file. For this we included the TPConvert python script that can be run like this:

```bash
TPConvert MainAtlas.json -f bta3
```

This will then produce a `MainAtlas.bta` file that can be copied to the `Content` directory which contains all the needed atlas meta data.
Please beware that the default atlas is required to contain the default font as well.

The documentation for creating the `MainAtlas.fbk` file has not been completed yet. The fbk file contains some basic font kerning information.

## More details

For detailed information about designing a custom UI read [this document](../../Doc/FslSimpleUI.md).
