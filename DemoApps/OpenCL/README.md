<!-- #AG_PROJECT_NAMESPACE_ROOT# -->
<!-- #AG_PROJECT_CAPTION_BEGIN# -->
# DemoFramework 6.6.0 OpenCL

To [main document](../../README.md)
<!-- #AG_PROJECT_CAPTION_END# -->
## Table of contents
<!-- #AG_TOC_BEGIN# -->
* [Demo applications](#demo-applications)
  * [OpenCL](#opencl)
    * [Colorseg](#colorseg)
    * [FastFourierTransform](#fastfouriertransform)
    * [GaussianFilter](#gaussianfilter)
    * [Gray2Rgb](#gray2rgb)
    * [Info](#info)
    * [MedianFilter](#medianfilter)
    * [MorphoDilate](#morphodilate)
    * [MorphoErode](#morphoerode)
    * [Rgb2Gray](#rgb2gray)
    * [Rgb2Hsv](#rgb2hsv)
    * [Rgb888toRgb565](#rgb888torgb565)
    * [Rgb888toUYVY](#rgb888touyvy)
    * [SobelHFilter](#sobelhfilter)
    * [SobelVFilter](#sobelvfilter)
    * [SobelVHFilter](#sobelvhfilter)
    * [SoftISP](#softisp)
<!-- #AG_TOC_END# -->

# Demo applications

<!-- #AG_DEMOAPPS_BEGIN# -->

## OpenCL

### [Colorseg](Colorseg)

OpenCL Kernel and code to execute a Colorseg
--Options

"InputBmpFile" Input BMP file.
"OutputHsvBmpFile" Output Hsv BMP file.
"OutputRgbBmpFile" Output Rgb BMP file.

### [FastFourierTransform](FastFourierTransform)

OpenCL Kernel and code to execute a Fast Fourier Transform
--Options

"Length" FFT length.

### [GaussianFilter](GaussianFilter)

OpenCL Kernel and code to execute a GaussianFilter
--Options

"InputBmpFile" Input BMP file.
"OutputBmpFile" Output BMP file.
"type" Select type: Gray, Rgb.

### [Gray2Rgb](Gray2Rgb)

OpenCL Kernel and code to execute a Gray2Rgb
--Options

"InputBmpFile" Input BMP file.
"OutputBmpFile" Output BMP file.

### [Info](Info)

Simple OpenCL Application that allows you to obtain your system's complete OpenCL information:
Information related to CL kernel compilers, number of buffers supported, extensions available and more.

### [MedianFilter](MedianFilter)

OpenCL Kernel and code to execute a MedianFilter
--Options

"InputBmpFile" Input BMP file.
"OutputBmpFile" Output BMP file.

### [MorphoDilate](MorphoDilate)

OpenCL Kernel and code to execute a MorphoDilate
--Options

"InputBmpFile" Input BMP file.
"OutputBmpFile" Output BMP file.

### [MorphoErode](MorphoErode)

### [Rgb2Gray](Rgb2Gray)

OpenCL Kernel and code to execute a Rgb2Gray
--Options

"InputBmpFile" Input BMP file.
"OutputBmpFile" Output BMP file.

### [Rgb2Hsv](Rgb2Hsv)

OpenCL Kernel and code to execute a Rgb2Hsv
--Options

"InputBmpFile" Input BMP file.

### [Rgb888toRgb565](Rgb888toRgb565)

OpenCL Kernel and code to execute a Rgb888toRgb565
--Options

"InputBmpFile" Input BMP file.
"OutputBmpFile" Output BMP file.

### [Rgb888toUYVY](Rgb888toUYVY)

OpenCL Kernel and code to execute a Rgb888toUYVY
--Options

"InputBmpFile" Input BMP file.
"OutputRawFile" Output RAW file.

### [SobelHFilter](SobelHFilter)

OpenCL Kernel and code to execute a SobelHFilter
--Options

"InputBmpFile" Input BMP file.
"OutputBmpFile" Output BMP file.

### [SobelVFilter](SobelVFilter)

### [SobelVHFilter](SobelVHFilter)

OpenCL Kernel and code to execute a SobelVHFilter
--Options

"InputBmpFile" Input BMP file.
"OutputBmpFile" Output BMP file.

### [SoftISP](SoftISP)

<a href="SoftISP/Thumbnail.jpg"><img src="SoftISP/Thumbnail.jpg" height="108px" title="OpenCL.SoftISP"></a>

It is a software-based image signal processing(SoftISP) application optimized by GPU. SoftISP --Options
"Enable" Enable high quality noise reduction node

<!-- #AG_DEMOAPPS_END# -->
