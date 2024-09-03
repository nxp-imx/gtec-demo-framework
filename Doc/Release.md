# Releases

## Release 6.4.1 (September 3, 2024)

* The Vulkan swapchain creation now use more sensible defaults when the app has no preferences.
* Upgraded to FslBuild 3.7.0.1
* Updated the way we build zlib on ubuntu.

## Release 6.4.0 (July 30, 2024)

* Now require a compiler with [C++20](https://en.cppreference.com/w/cpp/compiler_support/20) support.
* New OpenCL samples
  * ColorSeg
  * GaussianFilter
  * Gray2Rgb
  * MedianFilter
  * MorphoDilate
  * MorphoErode
  * Rgb2Gray
  * Rgb2Hsv
  * Rgb888ToRgb565
  * Rgb888ToUYVY
  * SobelHFilter
  * SobelVFilter
* New OpenGL ES2 samples
  * System.Gestures
  * UI.RenderTest
* New OpenGL ES3 samples
  * System.Gestures
  * UI.RenderTest
  * UI.Gestures
* New Vulkan samples
  * System.EasingFunctions
  * System.Gestures
  * UI.RenderTest
  * UI.Gestures
* Added new demo app templates
  * OpenGLES3.HDR  
  * Vulkan.HDR  
* New OpenGL ES 2+3 command line arguments
  * ```--EGLSampleBuffers <arg>``` Force EGL_SAMPLE_BUFFERS to the given value
  * ```--EGLSamples <arg>``` Force EGL_SAMPLES to the given value
* FslSimpleUI
  * Reworked the way colors are handled so we can better support SRGB and HDR rendering.
    * All control color properties are now represented by UIColor instead of Color (16bit per channel and color space info).
    * Rendering uses UIRenderColor which stores the colors using 16bit per channel mathing the frame buffer format (linear/non-linear).
  * Basic gesture support.
  * Basic scroll view with gesture support.
  * FslSimpleUI.Delcarative updated to support most controls, it can also now dump a xsd from code describing the possibilities.
  * The default theme texture atlas is now generated in two versions
    * NonLinear (premultiply alpha is done directly on the non-linear SRGB colors, like we used to do)
    * Linear (premultiply alpha is done by transforming the RGB colors to linear values and applying the alpha, then converting back to non-linear).
  * Support for line rendering for charts.
* Misc
  * Clang tidy now enforces naming conventions.
  * HDR screenshot support.
  * ImageConverter service updated.
  * Added FslDemoService.ImageConverter.HDR service.
  * Added FslGraphics2D.PixelFormatConverter package.
  * BasicRender line list+strip support.
  * Stats sample now show CPU chars (GLES2, GLES3, Vulkan).
  * TightBitmap added for the cases where you need to work on bitmap data where the data is tighly packed in memory.
  * ```StringViewLite``` now utilize a ```std::string_view``` internally.
  * ```ReadOnlySpan``` now utilize a ```std::span<const T>``` internally.
  * ```Span``` now utilize a ```std::span<T>``` internally.
  * Updated easing function in FslBase
  * Many old classes now utilize span's instead of pointer, length.
  * UIDemoAppLegacyExtension was removed and all samples using it updated to use UIDemoAppExtension.

## Release 6.3.1 (May 1, 2024)

* Fixed Vulkan.Screenshot, so it captures it at the correct time.
* OpenCL.SoftISP example updated.
* When UI.Benchmark is using DrawCaching any configuration change will force invalidate the cache so the effect of the options is immediately visible.
* Windows did not get marked as dirty by layout changes.
* Updated zlib version
* OpenCL SoftISP Fix denoise kernel issue

## Release 6.2.4 (Feb 27, 2024)

* Updated zlib version
* OpenCL SoftISP Fix denoise kernel issue

## Release 6.3.0 (Jan 15, 2024)

* FslDataBinding now supports two way binding.
* UI.Benchmark now supports a draw cache option.
* FslSimpleUI
  * Draw caching support.
  * Handles SRGB and HDR framebuffers better.
* Added examples for Custom app services.
* Android HDR check improved.
* FslNativeWindowSystem modernized.
* Yocto: Improved Wayland XDG implementations.
  * Now supports XDG decorations where applicable.
* FslBuild
  * Visual Studio projects no longer references unused flavors.
  * Clang format/tidy are now flavor aware.
  * -open2 can now supply arguments to visual studio code (can be used as a workaround for wayland issues).
* Upgraded fmt to 10.1.1
* Upgraded clang tools to 16

## Release 6.2.3 (Jan 15, 2024)

* Marked samples that use the GL_VIV_direct_texture extension but was not marked as such.
* SmoothScroll should now scroll on displays that are too small to show the entire text being shown
* Fix out-of-bound R/W in gaussian_filter.cl
* FslUtil.OpenCL.ContextEx added new constructor and reset method.

## Release 6.2.2 (Nov 6, 2023)

* Fixed a stride error in VUSegmentedBufferMemory.
* Upgraded to zlib 1.3

## Release 6.2.1 (Aug 9, 2023)

* GLES3.ModelInstancing and Vulkan.ModelInstancing: Added arguments to control the instance count.
* Improved EGLConfig selection and improved logging.
* Ensured that OpenGL ES samples clear the alpha channel correctly.

## Release 6.2.0 (Jul 13, 2023)

* GLES2, GLES3, Vulkan UI.DeclarativeUI sample added.
* Android changes
  * Upgraded to match latest Android Studio.
  * Fixed a issue with Android screen rotation support in some apps.
  * Switched from NativeActivity to the new Android Game Development kit.
* FslSimpleUI
  * Improved DataBinding support.
  * Controls now support min and max width.
  * Hover events are now handled properly for touch input.
* Third party dependencies
  * Upgraded RapidVulkan to 1.3.250
  * Upgraded to fmt 10.
* Build system
  * Now utilize flavors instead of variants.
  * FslBuildDoc now keeps the SCR file up to date.
* Improved Wayland XDG shell support.

## Release 6.1.1 (Jun 7, 2023)

* GCC13 support.
* Small wayland bug fix.
* Vulkan.NativeWindowTest update.
* New 'feature' to make it easier to filter apps during testing.

## Release 6.1.0 (Mar 16, 2023)

* Added Vulkan.Stats and GLES3.Stats.
* Yocto wayland XDG shell support.
* Upgraded to fmt9
* Bumped Android ndk to r25b
* Updated android studio support to latest version.

## Release 6.0.3 (Dec 9, 2022)

* Samples that depend on "GL_VIV_direct_texture" now list it in their meta data.
* Fixed a unit test.
* Updated Yocto documentation.
* Updated Visual Studio detection.
* Updated zlib

## Release 6.0.2 (Oct 18, 2022)

* Forward compatibility with fmt 9.x
* Upgraded assimp to 5.2.5 for gcc compatibility fix.
* Fixed openvx recipe as its repository was changed.
* Fixed openexr recipe as its repository was changed.

## Release 6.0.1 (Jul 28, 2022)

* Minor bugfix to OpenVG/NativeGraphicsBasic2D

## Release 6.0.0 (Jun 29, 2022)

* Samples
  * Added [ModelInstancing](../DemoApps/Vulkan/ModelInstancing/README.md) for GLES3 and Vulkan.
  * Added [System.DevBasicCustomRender](../DemoApps/Vulkan/System/DevBasicRenderCustomShader/README.md) for GLES2, GLES3 and Vulkan.
  * Added [UI.BasicDataBinding](../DemoApps/Vulkan/UI/BasicDataBinding/README.md) for GLES2, GLES3 and Vulkan.
  * Added [UI.Charts](../DemoApps/Vulkan/UI/Charts/README.md) for GLES2, GLES3 and Vulkan.
  * Converted [SpatialHashGrid2D](../DemoApps/GLES3/SpatialHashGrid2D/README.md) from Vulkan to GLES3.
  * [UI.Benchmark](../DemoApps/Vulkan/UI/Benchmark/README.md) added 'alternative' draw re-order algorithm.
* Libraries
  * [FslBase](../DemoFramework/FslBase/README.md) replaced Optional with std::optional
  * New library [FslDataBinding](../DemoFramework/FslDataBinding/Base/README.md) which provides basic data binding support.
  * [FslGraphics3D.BasicRender](../DemoFramework/FslGraphics3D/BasicRender/README.md): Added initial custom shader support.
  * [FslSimpleUI](../DemoFramework/FslSimpleUI/README.md): Added initial data binding support
  * [FslSimpleUI.Charts](../DemoFramework/FslSimpleUI/Controls/Charts/README.md):
    * Added BoxPlot control
    * Added initial data-binding support.
  * FslSimpleUI.Render cached font information for faster text rendering.
  * C++17 allowed a lot more things to be constexpr.
* Experimental [Emscripten build support](Setup_guide_emscripten.md).
* Now require a compiler with [C++17](https://en.cppreference.com/w/cpp/compiler_support/17) support.
* Tool changes
  * **VS2022** is now the default on windows.
  * FslBuild tools added feature ```HW_GPU_VIVANTE``` for samples that require a vivante GPU.
  * The [gtec-texture-packer](https://github.com/NXPmicro/gtec-texture-packer) used to create texture atlases for the samples is now freely available for everyone to use.
  * Now require at least CMake 3.16.
  * Now require clang format and clang tidy 13+
  * Android NDK r23c
* Misc:
  * Clang format was allowed to auto sort all includes.
  * Google [Angle](https://github.com/google/angle) was [re-tested](Vulkan/AngleCompatibility.md) to see if it could serve as the default OpenGL-ES emulator on windows.
  * Upgraded to [Assimp 5.2.4](https://github.com/assimp/assimp/releases/tag/v5.2.4).
  * Upgraded to [FMT 8.1.1](https://github.com/fmtlib/fmt/releases/tag/8.1.1).
  * Upgraded to [GoogleTest 1.11.0](https://github.com/google/googletest/releases/tag/release-1.11.0).
  * Upgraded to [nlohmann/json 3.10.5](https://github.com/nlohmann/json/releases/tag/v3.10.5).
  * Upgraded to [RapidVulkan 1.3.211](https://github.com/Unarmed1000/RapidVulkan/releases/tag/1.3.211.0).
  * OpenVX was upgraded to 1.2

## Release 5.9.2 (Jan 24, 2022)

* Fixed OpenVX.Stereo compilation issue with new versions of OpenCV.

## Release 5.9.1 (Jan 19, 2022)

* Samples
  * Added System.OnDemandRendering which showcase the new on demand rendering support. It can also be used to detect frame * timing issues. (GLES2+3, Vulkan)
  * Added System.BasicRenderModelLoader which showcase how to load a mesh and render it using the BasicRender (GLES2+3, Vulkan).
  * Updated UI.Benchmark app with on demand rendering (GLES2+3, Vulkan).
  * Much more precise frame-timing.
  * Added new command line parameter '--Version' which lists the release version and git commit (if available).
  * Added a 'license.json' for the example screenshots 'Example.jpg'
  * Initial Visual Studio 2022 support.
* Breaking changes:
  * FslResourceScan.py was integrated into 'FslBuildCheck --license' so the standalone tool was removed.
  * Replace deprecated Draw methods with the new Draw method.
  * Removed HighResolutionTimer.GetTime() use GetTimestamp() instead.
  * DemoTime now provides the higher resolution TimeSpan's as time measurements.
* Upgraded dependencies
  * FMT 8.1.1
  * RapidVulkan 1.2.189

## Release 5.8.1 (Nov 10, 2021)

* Various minor fixes.
* Angle compatibility fix.
* Upgraded FslBuild.

## Release 5.8.0 (Sep 6, 2021)

* New samples
  * GLES2.System.DevBasicRender
  * GLES2.UI.Benchmark
  * GLES3.System.DevBasicRender
  * GLES3.UI.Benchmark
  * Vulkan.GpuTimestamp
  * Vulkan.System.DevBasicRender
  * Vulkan.UI.Benchmark
* UI rendering
  * Rewrote the UI rendering code to depend on the new FslGraphics3D.BasicRender.
  * Significant performance improvements.
  * Rendering code support various rendering optimizations that can be toggled at run-time.
  * Updated the [custom UI guidelines](./FslSimpleUI.md)
* Android
  * Android apps now support command line arguments. Add the arguments using ```-e "args" "<arguments>" for example ```-e "args" "--DensityDpi 160 -vvvv"```
* Dependencies:
  * Upgraded to OpenCV 4.5.2

## Release 5.7.1 (May 6, 2021)

* Compatible with GCC11
* Fixed G2D dependency issue.
* Fixed android compilation issues.
* Updated to OpenCV 4.5.2
* Added some notes to help people that hit a "access denied" error when trying to run the visual studio cmake project.

## Release 5.7.0 (Jan 27, 2021)

* Python 3.6 or better is now required
* Updated all setup guides.
* Windows and Ubuntu now uses cmake find_package to locate the system OpenCL implementation.
  This removes the the dependency on the deprecated AMD App SDK.
* Samples:
  * New Vulkan sample: Vulkan.GenerateMipMaps
  * New Vulkan sample: Vulkan.ShaderClock sample (VK_KHR_shader_clock)
  * Vulkan samples updated to utilize the new mipmap generation code.
* Build tools:
  * FslBuildCheck.py clang tidy now defaults to use cmake for dependencies. Legacy mode can still be used by adding ```--legacy```.
  * FslBuild on windows now defaults to the cmake visual studio generator.
  * FslBuild now support package flavors
  * **Breaking change** FslBuild removed support for dynamic variants.
  * **Breaking change** FslBuild custom Visual Studio project generator no longer use dynamic variants for the OpenGL ES and OpenVG emulation. This means we no longer generate a the ```.StartProject.bat``` file. The user is now responsible calling ```ConfigureOpenGLESEmu.bat```   to configure the OpenGL ES emulator before launching the project.. Its recommended to call this right after the prepare.bat file.
* Dependencies:
  * Upgraded FMT to 7.1.
  * Upgraded GLM to 0.9.9
  * Upgraded GLI to master branch snapshot (0c171ee87fcfe35a7e0e0445adef06f92e0b6a91)
  * Upgraded RapidVulkan to 1.2.162

## Release 5.6.2 (Nov 3, 2020)

* Fixed a issue in OpenVX.SoftISP
* Improved the FslSimpleUI document.

## Release 5.6.1 (Aug 21, 2020)

* Fixed a issue in the basic UI theme for 80dpi.
* Updated Doc/FslSimpleUI.md
* Updated KnownIssues.md
* Upgraded the fmt library to 7.x to improve compile time and reduce exe size.
* Latest build tools

## Release 5.6.0 (Jul 3, 2020)

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

## Release 5.5.3 (Jun 3, 2020)

* Fixed a issue with packages marked as not supported
* Now using libwayland-egl on some platforms.

## Release 5.5.2 (May 5, 2020)

* Fixed a matrix alignment bug in Vulkan.ObjectSelection.
* Added a Matrix3Std140 helper class that matches the Vulkan shader layout for mat3 in std140 layout.

## Release 5.5.1 (Feb 5, 2020)

* Added a known issue for Visual Studio 2019 16.5.x and provided a workaround until its fixed.
* Fixed a crash bug in Vulkan.Bloom and Vulkan.ModelViewer.
* Fixed vulkan DFGraphicsBasic2D so it works without a --Stats overlay.
* T3DStressTest. Fixed shader precision issue (that also exist in 5.4.0 and previous releases).
* Vulkan depth selection no longer picks unwanted depth+stencil formats.
* Workaround for OpenVG reference implementation EGL bug.

## Release 5.5.0 (Feb 5, 2020)

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

## Release 5.4 (Sep 10, 2019)

* Now requires C++14.
* New Samples
  * GLES2.TextureCompression
  * Vulkan.ObjectSelection
  * Vulkan. SRGBFramebuffer
* Vulkan screenshot support
* Replaced TCLAP with a custom argument parser to reduce executable size.
* FslBuild
  * now supports "-c install".
  * Experimental CMake support (see FslBuild_CMake.md)
  * Experimental VS2019 support
  * Android Studio 3.5.0
  * Android NDK r20

## Release 5.3.1 (May 21, 2019)

* Added new sample Vulkan.NativeWindowTest.
* Improved Vulkan Wayland support.
* Yocto wayland fix: Not create shell surface for ivi applications which only requires ivi surface.

## Release 5.3 (Feb 11, 2019)

* Disable third party downloads per default for Yocto builds. To build using an old Yocto release that doesn't come with all third party software you need to add "--Recipes [*]" to your command line which will re-enable the download.
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

## Release 5.2 (Oct 15, 2018)

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

## Release 5.1.1 (Jul 20, 2018)

* FslBuild 2.8.4
* Texture2D SetData
* Fixed pixel formats in OpenCV3 demos
* GLRawBitmapUtil::Result can now be constructed as 'empty'.
* TClap recipe updated to use internal mirror of tclap.
* Updated DemoFramework.docx yocto section as it was out of date.
* Updated the TPConvert script

## Release 5.1 (May 8, 2018)

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

## DemoFramework 5.0.2 (Feb 26, 2018)

* GLM 0.9.8.5 patch to fix a failure to identify new compilers like GCC 7.3 which caused compilation errors on GCC 7.3.
* SoftISP samples: bug fix and added OptionParser.
* Android: ContentSync now recognizes the new content format.
* Android: Bumps the minimum android API level to fix some android header file incompatibilities.
* FslBuild tools upgraded, this includes a fix to the automatic vulkan shader compilation.

## Release 5.0.1 (Jan 23, 2018)

* OpenVX.SoftISP demo.
* OpenCL.SoftISP demo.

## Release 5.0 (Dec 9, 2017)

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

## Release 4.0.2 (Sep 14, 2017)

* Minor corrections mainly targeting the yocto build.

## Release 4.0.1 (Jun 6, 2017)

* Contains a few minor bug fixes for Windows and Yocto.

## Release 4.0 (May 1, 2017)

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
