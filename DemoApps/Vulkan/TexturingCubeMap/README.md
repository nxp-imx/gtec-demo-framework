## Cube maps (Texture mapping)
<img src="./Example.jpg" height="96px" align="right">

Building on the basic texture loading example, a cubemap texture is loaded into a staging buffer and is copied over to a device local optimal image using buffer to image copies for all of it's faces and mip maps.

The demo then uses two different pipelines (and shader sets) to display the cubemap as a skybox (background) and as a source for reflections.