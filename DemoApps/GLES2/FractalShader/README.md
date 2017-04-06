## FractalShader
<img src="./Example.jpg" height="96px" align="right">

Can render both the julia and mandelbrot set.
Was used to demonstrates GPU shader performance by using up to 515 instructions each fragment while generating the julia set. 
No texture and no overdraw, minimal bandwidth requirements.

Arguments
--Scene        Select the scene to run (0-1)
--Quality      Select the rendering quality (low, medium, high)
--RenderMode   0=Gray, 1=Col, 2=Smooth, 3=Tex
--Iterations   the number of iterations to perform >= 1
--ForceUnroll  Force unroll the shader loops.
--Show         Show coordinate in julia animation
--AnimMode     Chose the julia animation mode.
--Demo         Lock into demo mode (nothing can be overriden)
