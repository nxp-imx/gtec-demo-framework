<!-- #AG_DEMOAPP_HEADER_BEGIN# -->
# SoftISP
<img src="./Example.jpg" height="135px" style="float:right">

<!-- #AG_DEMOAPP_HEADER_END# -->
<!-- #AG_BRIEF_BEGIN# -->
It is a software-based image signal processing(SoftISP)  application optimized by GPU.
 SoftISP Roles:
 - Bad Pixel Correction
 - White Balance
 - Histogram Equalization
 - High-quality Demosaicing
 - High-quality Noise Reduction
<!-- #AG_BRIEF_END# -->

Input Image:
 - Bayer format raw data with color filter array of BG/GR
 - Resolution: 1920x1080

Output Image:
 - "0-SourceImage.bmp": Input image -> Demosaicing
 - "1-RemoveBadPixel.bmp": Input image -> Bad Pixel Correction -> Demosaicing
 - "2-WhiteBalance.bmp": Input image -> Bad Pixel Correction -> White Balance -> Demosaicing
 - "3-Equalization.bmp": Input image -> Bad Pixel Correction -> Histogram Equalization -> Demosaicing
 - "4-RemoveNoise.bmp": Input image -> Bad Pixel Correction -> Histogram Equalization -> Demosaicing -> Noise Reduction

Note:
Kernels running on GPU are built online, but building the noise reduction kernels will take several minutes. So the high quality noise reduction is disabled in default SoftISP pipeline by setting "m_denoiseEn" in class UserNodes to "0". Set "m_denoiseEn" to "1" to enable the high quality noise reduction node.

Inspired by:
 - http://ieeexplore.ieee.org/abstract/document/6419046/
 - https://courses.cs.washington.edu/courses/cse467/08au/labs/l5/whiteBalance.pdf
 - https://en.wikipedia.org/wiki/Histogram_equalization
 - http://www.tandfonline.com/doi/abs/10.1080/2151237X.2008.10129267
 - https://en.wikipedia.org/wiki/Bilateral_filter
<!-- #AG_DEMOAPP_COMMANDLINE_ARGUMENTS_BEGIN# -->
<!-- #AG_DEMOAPP_COMMANDLINE_ARGUMENTS_END# -->
