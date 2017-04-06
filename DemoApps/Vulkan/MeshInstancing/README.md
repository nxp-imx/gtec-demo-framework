## Mesh instancing
<img src="./Example.jpg" height="96px" align="right">

Shows the use of instancing for rendering many copies of the same mesh using different attributes and textures. A secondary vertex buffer containing instanced data, stored in device local memory, is used to pass instance data to the shader via vertex attributes with a per-instance step rate. The instance data also contains a texture layer index for having different textures for the instanced meshes.
