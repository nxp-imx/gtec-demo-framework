# Releases

## Release 5.6

* This release contains breaking changes.
* Recommendations and examples for designing custom UI systems.
  * FslSimpleUI system now follows these best recommendations and comes with a default theme.
  * This includes DPI aware rendering.
* New samples
  * GLES3.RenderToTexture
  * GLES2, GLES3, Vulkan, SdfFonts
  * GLES2, GLES3, Vulkan, UI.DevNativeTexture2D
  * GLES2, GLES3, Vulkan, UI.DpiScale
  * GLES2, GLES3, Vulkan, UI.PixelPerfect
  * GLES2, GLES3, Vulkan, UI.SmoothScroll
  * GLES2, GLES3, Vulkan, UI.ThemeBasicUI
* Better CMake integration.
  * Release and debug builds now use separate output dirs by default.
  * Better support for CMake generators.
  * CMake + ninja is now the default backend for Ubuntu and Yocto for improved build speed.
  * Ubuntu builds now utilize CMake's find_package support to locate third party dependencies.
* Code modernization with PathView, StringViewLite, NumericCast and Spans.
* Clang tidy + format now requires clang 10.
* Downgraded ninja build system requirement to 1.8.2 from 1.9 for better ubuntu 18.04 compatibility.
* Better display metrics available to all samples.
* Misc: New binary texture atlas format, new bitmap font format with simple kerning.
* NativeBatch upgrades
  * SDF rendering.
  * Pixel perfect bitmap font rendering.
  * New rendering primitives.
* Now require a compiler with proper C++14 constexpr support.
  * This means GCC 5.x and below is not supported.

## Release 5.5

* New samples
  * GLES2.Stats
  * Vulkan.Bloom
  * Vulkan.EffectOffscreen
  * Vulkan.EffectSubpass
  * Vulkan.FractalShader
  * Vulkan.FurShellRendering
  * Vulkan.HDR04_HDRFramebuffer
  * Vulkan.ModelLoaderBasics
  * Vulkan.ModelViewer
  * Vulkan.T3DStressTest
* GLES2, GLES3, Vulkan ModelViewer now supports wireframe rendering.
* Dependency changes
  * Assimp 5.0.1 from 4.1.0
  * Fmt 6.1.2 from 6.0.0
  * Google test 1.10.0 from 1.8.1
  * Ninja from 1.9.0 from 1.8.2
  * OpenCV 4.2 from OpenCV 4
* Tool changes
  * Clang tidy+format now requires clang 8
  * VS2019 is now the default on windows.
  * Ubuntu and yocto builds now default to the cmake generator.
* Deprecated FSLLOG and FSLLOG2. 
* Introduced FSLLOG3 which use the fmt library instead of streams. The fmt library is a implementation of the C++20 std::format.
* Experimental Angle support and compatibility test.

## Release 5.4

* Now requires C++14.
* New Samples
  * GLES2.TextureCompression
  * Vulkan.ObjectSelection
  * Vulkan. SRGBFramebuffer
* Vulkan screenshot support
* Replaced TCLAP with a custom argument parser to reduce executable size.
* FslBuild
  * now supports “-c install”.
  * Experimental CMake support (see FslBuild_CMake.md)
  * Experimental VS2019 support
  * Android Studio 3.5.0
  * Android NDK r20

## Release 5.3

* Disable third party downloads per default for Yocto builds. To build using an old Yocto release that doesn't come with all third party software you need to add “--Recipes [*]” to your command line which will re-enable the download.
* Updated all OpenCV 3 apps to OpenCV 4.
* New samples
  * GLES2.LineBuilder
  * GLES3.AssimpDoubleTexture
  * GLES3.LineBuilder
  * Vulkan.DFGraphicsBasic2D
  * Vulkan.DFNativeBatch2D
  * Vulkan.DFSimpleUI100
  * Vulkan.DFSimpleUI101
  * Vulkan.GammaCorrection
  * Vulkan.HDR01_BasicToneMapping
  * Vulkan.HDR02_FBBasicToneMapping
  * Vulkan.HDR03_SkyboxToneMapping
  * Vulkan.InputEvents
  * Vulkan.LineBuilder
  * Vulkan.OpenCLGaussianFilter
  * Vulkan.OpenCV101
  * Vulkan.OpenCVMatToNativeBatch
  * Vulkan.OpenCVMatToUI
  * Vulkan.OpenVX101
  * Vulkan.Scissor101
  * Vulkan.Skybox
  * Vulkan.TextureCompression
  * Vulkan.Vulkan101
* Vulkan demo host was refined and is much closer to a finished implementation.
  * All Vulkan samples now utilize it. Due to low level nature of the Vulkan API they use it to varying degree, all new Vulkan samples use it extensively and therefore support the usual stats overlay etc. Older samples will be converted in upcoming releases.
  *	Screenshots are still not supported out of the box, this is coming soon.
  *	This includes a basic NativeGraphics service for Vulkan.
* Lots of GLES 2 + 3 samples were updated to make it easier to compare the Vulkan and OpenGL ES implementations.
* New logging functionality
  * FslBase
  * FslGraphics
* Additional unit tests made available
  * FslBase
  * FslGraphics
  * FslUtil.Vulkan
* Unit tests enabled for Yocto builds.
* Improved procedural line builder, useful for procedurally generating line meshes for debugging.
  * Available in FslGraphics3D.Build 
  * Used in GLES3.ObjectSelection and the LineBuilder samples.
* Large content files moved to the shared Resource directory, taking advantage of the build pipeline.
* Upgraded build tools to 3.0.0.
  * FslBuildCheck now use clang 7 instead of 6.
  * Fixed vulkan shader ‘on changed’ compilation under VS2017.
  * Added Vulkan project template.
* Various packages were improved to allow for better compiler optimizations.
* Better support for various verbose output levels (-v, -vv, etc).

## Release 5.2

* CONTRIBUTING.md describes how to contribute.
* Early access to the Helios cross platform camera API.
  * GLES3.CameraDemo
  * GLES3. DFNativeBatchCamera
* Includes a small subset of the unit tests. More will be made available in future release.
* Code cleanup
  * FslBuildCheck now supports clang format and clang tidy.
  * All code was reformatted with clang format.
  * All code had a tidy pass using clang tidy.
* Added OpenVX.Stereo.
* Vulkan console demos no longer gets build with a window system
* Vulkan.DevBatch implementation improved.
* Fixed all Vulkan 1.1.85.0 validation layer warnings in:
  * Vulkan.DevBatch
  * Vulkan.DisplacementMapping
  * Vulkan.Gears
  * Vulkan.MeshInstancing
  * Vulkan.TessellationPNTriangles
  * Vulkan.Texturing
  * Vulkan.TexturingArrays
  * Vulkan.TexturingCubeMap

## Release 5.1

* All ThirdParty code is now downloaded as needed instead of being included in the repo.
* Windows builds now default to Visual Studio 2017 instead of 2015.
* Basic support for changing the color-space via EGL.
* Examples of how to setup SRGB and HDR framebuffers.
* HDR to LDR display rendering examples with various basic tone-mapping algorithms.
* Vulkan enabled for the Yocto Wayland backend.
* Assimp upgraded to 4.1 on most platforms.
* GLES3.ColorspaceInfo
* GLES3.EquirectangularToCubemap
* GLES3.GammaCorrection demo.
* GLES3.HDR01_BasicToneMapping
* GLES3.HDR02_FBBasicToneMapping
* GLES3.HDR03_SkyboxTonemapping
* GLES3.HDR04_HDRFramebuffer
* GLES3.MultipleViewportsFractalShader demo.
* GLES3.Scissor101
* GLES3.Skybox
* GLES3.SRGBFramebuffer
* GLES3.TextureCompression demo.
* Vulkan.VulkanInfo demo.
* Android build now requires Android Studio 3.1 and the Android NDK16b or newer.

## Release 5.0.1

* OpenVX.SoftISP demo.
* OpenCL.SoftISP demo.

## Release 5.0

* Tools now require Python 3.4+ instead of python 2.7
* FslBuildNew script that can help you create a new project fast.
* Vulkan support is much closer to its final state.
* The application registration method has been changed so it’s more future proof and allow for greater customization.
* Prebuild binaries have been removed.
  * FslImageConvert.exe was removed as we now support saving screenshots directly in jpg.
  * Prebuild windows libraries removed as we now download and build them on demand instead.
* The directory structure was updated to make it simpler.
* Some tags in Fsl.gen xml files were deprecated.
* Gamepad support.
* New libraries
  * Stb, xinput, perfcounters.

## Release 4.0

* First public release on github.
* Early access support for Vulkan, OpenCL, OpenCV and OpenVX.
  * Vulkan samples.
  * OpenVX samples.
  * OpenCL samples.
  * OpenCV samples.
* New libraries
  * GLI 0.8.10, GLM 0.9.7.6
* PixelFormats are now compatible with the vulkan pixel formats.
* FslBuild.py script introduced as a simple unified way to build on all platforms if so desired. It’s still possible to build using the native platform method.
* FslBuild scripts now support limited feature based filtering.
* Introduced a content pipeline to help build vulkan shaders.
* Windows builds
  * Visual Studio 2015 is now the default environment instead of 2013
  * We now use the OpenVG reference implementation to emulate OpenVG.

## Release 2.3

* OpenGLES 3.1 support.
* A new ContentMonitor can reload your sample when it detects changes to the content folder (this does not work on Android). This allows for rapid prototyping on most platforms.
* New samples: 
  * DFSimpleUI101, ModelLoaderBasics, ModelLoaderViewer, Tessellation101, TessellationSample.
* New libraries:
  * FslAssimp, FslGraphics3D, FslSceneFormat, FslSimpleUI, FslGraphicsGLES3v1
* New experimental UI framework intended to quickly create a UI for your sample app.
* Assimp support on most platforms. It is not supported on Android here we recommend using the FslSceneFormat instead. In general, it will be much more efficient to preprocess your model on a fast platform like a PC and save it in the FslSceneFormat instead of doing it on relatively slow target platform.
* Experimental support for generating Visual Studio 2015 projects (see the FslBuildgen documentation for details).
* Content loader for Binary texture and basic font kerning information.
* Windows PowerVR OpenGLES emulation support.

## Release 2.2

* Demo content can now be stored in bmp, png and jpeg format on all platforms. 
  * Some platforms support extra formats via the DevIL image library.
* Onscreen performance graph support that can be augmented with custom data.
* Pause and single stepping during demo playback.
* Added infrastructure that allows samples to share a library. See DemoApps/Shared for example libraries.
* Lots of new samples.
  * The Blur, FractalShader, FurShellRendering and DirectMultiSamplingVideoYUV are functional but experimental.
* Experimental G2D support.
* Experimental NativeBatch2D support under 3D api’s. See the DFNativeBatch2D samples for an example of how it works.
* Experimental –mmdc parameter for Yocto builds. If it shows the wrong info then run mmdc2 before running the sample as it will reset things correctly.
Version 2.1
* OpenVG support.
* OpenVG examples
* Examples: T3DstressTest for GLES2 + GLES3
* Most samples were upgraded to use the Content system to load their shaders and graphics.
* All samples now support the following arguments
  * –LogStats = Log basic rendering stats
  * –ScreenshotFrequency \<frequency> = Create a screenshot at the given frame frequency (Not supported for OpenVG).
