## Particle system (Compute shader)
<img src="./Example.jpg" height="96px" align="right">

Attraction based particle system. A shader storage buffer is used to store particle on which the compute shader does some physics calculations. The buffer is then used by the graphics pipeline for rendering with a gradient texture for. Demonstrates the use of memory barriers for synchronizing vertex buffer access between a compute and graphics pipeline
