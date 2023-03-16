
# Known issues

## Table Of Contents

<!-- #AG_TOC_BEGIN# -->
* [Platform specific](#platform-specific)
  * [Android](#android)
  * [Yocto](#yocto)
  * [Windows](#windows)
* [API specific](#api-specific)
  * [G2D](#g2d)
  * [OpenCL](#opencl)
  * [OpenVX](#openvx)
  * [Vulkan](#vulkan)
* [Apps](#apps)
  * [UI.Benchmark](#uibenchmark)
* [Libraries](#libraries)
  * [Graphics3D.BasicRender](#graphics3dbasicrender)
  * [Helios](#helios)
  * [SimpleUI](#simpleui)
<!-- #AG_TOC_END# -->


# Platform specific

## Android

* Android does not handle unicode file names inside the 'assets' folder,
  so do not utilize unicode for filenames stored in Content.

## Yocto

* If you are using a old version of the SDK that doesn't come with all required third party software you will need to add  ```--Recipes [*]``` to your build command as that will re-enable the download.
* If you are using a old version of the SDK build tools to build you might get a error like:
  "ERROR: \<urlopen error [SSL: CERTIFICATE_VERIFY_FAILED] certificate verify failed (_ssl.c:645)>"
  This is because the build tools are missing some required certificates.
  To solve this either upgrade to a new release of the build tools or delete the the python3.6 files from the build tools installation
  For example: Delete all python3.6 files from /opt/fsl-imx-internal-xwayland/4.9.51-mx8-beta/sysroots/x86_64-pokysdk-linux/usr/bin.
  Alternatively you can manually download the files and store them in the download cache,
  making the tools skip the download process (see "Doc\BuildingExternals.md" for more info).
* Assimp was updated from 3 to 4. So if you use a old version of the build tools and a new sd-card release
  The executable will be build to use version 3 and the sd-card contains version 4 causing the sample to fail.
  The solution is to make sure your build tools and sd-card version of the library match.
* If you get the error "RapidVulkan/System/Log.hpp: fatal error: FslBase/Log/BasicLog.hpp: No such file or directory" the Yocto sdk you are using contains a old incompatible version of RapidVulkan.
  Adding ```--Recipes [Recipe.RapidVulkan_1_2_148]``` to the build command might enable it to compile and work, but it potentially has issues as the RapidVulkan headers will be located at two different locations in the build.

## Windows

* Visual Studio 2019 16.5.x might not pickup the environment variables and paths it was launched with. This is a visual studio bug.
  Setting ```set ClearDevCommandPromptEnvVars=false``` before calling ```vcvarsall.bat``` can be used as a workaround until they fix it ([issue-link](https://developercommunity.visualstudio.com/content/problem/951981/environment-paths-not-respected.html)).
* CMake versions below 3.22 might report MSVC_TOOLSET_VERSION as 142 when it should be 143. See this [bug](https://gitlab.kitware.com/cmake/cmake/-/merge_requests/6497). Some versions of cmake shipping with VS2022 has this bug, to workaround it make sure that your cmake 3.22 install is in the path before the VS2022 version.
* Python default app issue: "Python was not found; run without arguments to install from the Microsoft Store, or disable this shortcut from Settings"
  See [this](https://stackoverflow.com/questions/65348890/python-was-not-found-run-without-arguments-to-install-from-the-microsoft-store)

* The generated project files do not detect changes to the build environment automatically.
  So its your job to run FslBuildGen when you change it!
* If a new shader is added to Content.bld and no files has been modified the content builder
  does not get run by visual studio.
* FslBuildCheck.py --tidy can fail to compile certain projects if the clang compiler is installed and it appears before the visual studio compiler in the project path.
  Since the ninja build will default to use clang instead of MSVC.
  To fix this ensure that the MSVC directories appear before the llvm tools in the path.
* If you get a "access denied" error when trying to run the cmake project under visual studio then you need to right click the executable project in the solution explorer and select "Set as Startup Project".

# API specific

## G2D

* Early access. Everything is subject to changes.
* Experimental support and it only works under Yocto.
* DFGraphicsBasic2D shows a black screen because its using stub implementations to render.
* '--Stats' shows nothing because its using stub implementations to render.
* Screenshot functionality does not work.
* NativeGraphicsBasic2D is mostly a stub.
* NativeTexture2D is a stub.
* NativeBatch2D is a stub.
* The screen resolution used by the demoframework is invalid (it thinks the size is 0,0).
* No input support.

## OpenCL

* Early access. Everything is subject to changes.
* Limited platform support

## OpenVX

* Early access. Everything is subject to changes.
* Windows and Ubuntu uses the Khronos OpenVX 1.1 sample implementation.
* Limited platform support

## Vulkan

* Please see the Known Issues section of the Vulkan [README](Doc/Vulkan/README.md).
* Early access. Everything is subject to changes.
* Qnx support is disabled.
* Requires 1.0.42.2 or newer or windows

# Apps

## UI.Benchmark

* All 'dialogs' consume all key events, preventing various special keys from working!
* Pressing 'page-down' to single step and then launching a dialog will prevent you from 'single-stepping' as the dialog consumes all special keys, causing the app to become stuck.

# Libraries

## Graphics3D.BasicRender

The basic rendering API.
The 'custom shaders' are still a work in progress and will be changed in upcoming releases.

## Helios

The new cross platform camera API.

* Very early access. Everything is subject to changes.
* Limited platform support
* Performance optimizations are missing.
* Camera support is limited.
* i.MX8 MIPI Camera always provides the data in a B8G8R8X8 format (even if you request a R8G8B8), fixed the camera adapter to consider the 32bpp format.
* Some of the camera adapters do not obey our origin request so the image is upside down

## SimpleUI

The simple UI library.

* **Charts:**  Is still a early access release with missing functionality.
* **Data-binding:** Not all UI controls have not been extended with full data binding support. The current data binding support has been based on the requirements from what our samples needed so far.
