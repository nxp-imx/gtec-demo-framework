## Blur
<img src="./Example.jpg" height="96px" align="right">

Showcases multiple ways to implement a gaussian blur.
- One pass blur
- Two pass blur
  The 2D Gaussian filter kernel is separable. This allows us two produce the same output as a one pass algorithm by first applying a X-blur and then a Y-blur.
- Two pass linear blur
  Uses the two pass technique and further reduces the bandwidth requirement by taking advantage of the GPU's linear texture filtering
  which allows us to reduce the needed kernel length to roughly half its length while producing the same output as the full kernel length.
- Two pass linear scaled blur
  Uses the two pass linear technique and further reduces the bandwidth requirement by downscaling the 'source image' to 1/4 its size (1/2w x 1/2h) before applying the blur and
  and then upscaling the blurred image to provide the final image. This works well for large kernel sizes and relatively high sigma's but the downscaling produces visible artifacts with low sigma's

Inspired by:
http://prideout.net/archive/bloom/
http://www.sunsetlakesoftware.com/2013/10/21/optimizing-gaussian-blurs-mobile-gpu
https://software.intel.com/en-us/blogs/2014/07/15/an-investigation-of-fast-real-time-gpu-based-image-blur-algorithms