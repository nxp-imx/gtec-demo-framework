## Basic texture mapping (Texture mapping)
<img src="./Example.jpg" height="96px" align="right">

Shows how to upload a 2D texture into video memory for sampling in a shader. Loads a compressed texture into a host visible staging buffer and copies all mip levels to a device local optimal tiled image for best performance.

Also demonstrates the use of combined image samplers. Samplers are detached from the actual texture image and only contain information on how an image is sampled in the shader.
