# MTSDF font generation

## Generation

The font was generated using [msdf-atlas-gen](https://github.com/Chlumsky/msdf-atlas-gen).

```bash
msdf-atlas-gen -yorigin top -size 37 -type sdf -emrange 0.2 -potr -font segoeui.ttf -imageout PsdfFont.png -json PsdfFont.json -charset charset.txt
```

Content of charset.txt

```bash
[32,126],956
```

## JSON fields

Some quick notes taken from the documentation at [msdf-atlas-gen](https://github.com/Chlumsky/msdf-atlas-gen)

* Atlas section includes the settings used to generate the atlas, including its type and dimensions.
  * The size field represents the font size in pixels per em.
* Metrics section contains useful font metric values retrieved from the font.
  * All values are in em's.
* Glyphs is an array of individual glyphs identified by Unicode character index (unicode) or glyph index (index), depending on whether character set or glyph set mode is used.
  * advance is the horizontal advance in em's.
  * planeBounds represents the glyph quad's bounds in em's relative to the baseline and horizontal cursor position.
  * atlasBounds represents the glyph's bounds in the atlas in pixels.
* If available, kerning lists all kerning pairs and their advance adjustment.
  * Which needs to be added to the base advance of the first glyph in the pair
