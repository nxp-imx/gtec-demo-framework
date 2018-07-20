# FslSimpleUI

A new experimental UI framework that makes it easy to get a basic UI up and running.
The main code is API independent. It is not a show case of how to render a UI fast but only intended
to allow you to quickly get a UI ready that is good enough for a demo.

You can look at:

- GLES3.DFSimpleUI100
- GLES3.DFSimpleUI101
- GLES3.TessellationSample

To see how it’s used.

When working with the UI system its recommended to store all or at least the most used bitmaps in the same texture atlas.
One commercially available texture packer is [Texture Packer](https://www.codeandweb.com/texturepacker) which can output a
json file that we can convert to a binary format that can be loaded by the demo framework.  

If you look at the DFSimpleUI100 sample, there is `OriginalContent/TextureAtlas` directory which contain a `MainAtlas.tps` file
that can be loaded into texture packer. Pressing publish in texture packer produces a `MainAtlas.png` and `MainAtlas.json` file
based on the files under `Main`. The `MainAtlas.png` can be copied directly to the samples `Content` directory but the json file needs to
be converted to a binary file. For this we included the TPConvert python script that can be run like this:

```bash
TPConvert MainAtlas.json -f bta1
```

This will then produce a `MainAtlas.bta` file that can be copied to the `Content` directory which contains all the needed atlas meta data.
Please beware that the default atlas is required to contain the default font as well.

The documentation for creating the `MainAtlas.fbk` file has not been completed yet. The fbk file contains some basic font kerning information.
