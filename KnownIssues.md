# Known issues

## Android

- Android does not handle unicode file names inside the 'assets' folder,
  so do not utilize unicode for filenames stored in Content.

## Yocto

- If you are using a old version of the SDK that doesn't come with all required third party software you will need to add  ```--Recipes [*]``` to your build command as that will re-enable the download.
- If you are using a old version of the SDK build tools to build you might get a error like:
  "ERROR: <urlopen error [SSL: CERTIFICATE_VERIFY_FAILED] certificate verify failed (_ssl.c:645)>"
  This is because the build tools are missing some required certificates.
  To solve this either upgrade to a new release of the build tools or delete the the python3.5 files from the build tools installation
  For example: Delete all python3.5 files from /opt/fsl-imx-internal-xwayland/4.9.51-mx8-beta/sysroots/x86_64-pokysdk-linux/usr/bin.
  Alternatively you can manually download the files and store them in the download cache,
  making the tools skip the download process (see "Doc\BuildingExternals.md" for more info).
- Assimp was updated from 3 to 4. So if you use a old version of the build tools and a new sd-card release
  The executable will be build to use version 3 and the sd-card contains version 4 causing the sample to fail.
  The solution is to make sure your build tools and sd-card version of the library match.

## Windows

- Visual Studio 2019 16.5.x might not pickup the environment variables and paths it was launched with. This is a visual studio bug.
  Setting ```set ClearDevCommandPromptEnvVars=false``` before calling ```vcvarsall.bat``` can be used as a workaround until they fix it ([issue-link](https://developercommunity.visualstudio.com/content/problem/951981/environment-paths-not-respected.html)).
- The generated project files do not detect changes to the build environment automatically.
  So its your job to run FslBuildGen when you change it!
- If a new shader is added to Content.bld and no files has been modified the content builder
  does not get run by visual studio.

## G2D

- Early access. Everything is subject to changes.
- Experimental support and it only works under Yocto.
- DFGraphicsBasic2D shows a black screen because its using stub implementations to render.
- '--Stats' shows nothing because its using stub implementations to render.
- Screenshot functionality does not work.
- NativeGraphicsBasic2D is mostly a stub.
- NativeTexture2D is a stub.
- NativeBatch2D is a stub.
- The screen resolution used by the demoframework is invalid (it thinks the size is 0,0).
- No input support.

## OpenCL

- Early access. Everything is subject to changes.
- Limited platform support
- The recommended AMD OpenCL implementation appears to be unsupported now and hard to find. For more information check this [discussion](https://community.amd.com/thread/228114).

## OpenVX

- Early access. Everything is subject to changes.
- Windows and Ubuntu uses the Khronos OpenVX 1.1 sample implementation.
- Limited platform support

## Vulkan

- Please see the Known Issues section of the Vulkan [README](Doc/Vulkan/README.md).
- Early access. Everything is subject to changes.
- Qnx support is disabled.
- Yocto support is available for WindowSystem=FB, WindowSystem=Wayland only.
- Requires 1.0.42.2 or newer or windows

## Helios

The new cross platform camera API.

- Very early access. Everything is subject to changes.
- Limited platform support
- Performance optimizations are missing.
- Camera support is limited.
- i.MX8 MIPI Camera always provides the data in a B8G8R8X8 format (even if you request a R8G8B8), fixed the camera adapter to consider the 32bpp format.
- Some of the camera adapters do not obey our origin request so the image is upside down
