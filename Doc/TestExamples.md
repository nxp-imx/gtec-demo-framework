# Test example commmands

Perform the necessary environment setup steps needed for building the demoframework apps (See the platform 'Setup_guide').

Contains notes

-----------------------------------------------------------------------------------------------------------------------------------------------------

Build and run a single GLES2 app to produce a screenshot on windows:

```bash
pushd DemoApps\GLES2\S06_Texturing
FslBuild.py --ForAllExe "(EXE) --Window [0,0,1920,1080] --ScreenshotFrequency 5 --ExitAfterFrame 5 --ForceUpdateTime 16666"
popd
```

This produces the file "test-frame-0000000005.bmp" in the root of the DemoApps\GLES2\S06_Texturing folder.

-----------------------------------------------------------------------------------------------------------------------------------------------------

Build and run a single GLES2 app to produce a screenshot on windows using the vivante GLES2 emulator

```bash
set FSLBUILD_VARIANT_FSL_GLES_NAME=vivante
pushd DemoApps\GLES2\S06_Texturing
FslBuild.py --ForAllExe "(EXE) --Window [0,0,1920,1080] --ScreenshotFrequency 5 --ExitAfterFrame 5 --ForceUpdateTime 16666"
popd
```

-----------------------------------------------------------------------------------------------------------------------------------------------------

Build and run all GLES2 apps and produce a screenshot for each using the Arm OpenGLES emulator (beware this can build cross API demos)

```bash
set FSLBUILD_VARIANT_FSL_GLES_NAME=arm
FslBuild.py --ForAllExe "(EXE) --Window [0,0,1920,1080] --ScreenshotFrequency 5 --ExitAfterFrame 5 --ForceUpdateTime 16666" -t sdk -r [OpenGLES2] --BuildTime
```

-----------------------------------------------------------------------------------------------------------------------------------------------------

Build and run all GLES2 apps but limit it EGL+OpenGLES2 use which disallows mixed API demos and
produce a screenshot for each using the Arm OpenGLES emulator

```bash
set FSLBUILD_VARIANT_FSL_GLES_NAME=arm
FslBuild.py --ForAllExe "(EXE) --Window [0,0,1920,1080] --ScreenshotFrequency 5 --ExitAfterFrame 5 --ForceUpdateTime 16666" -t sdk -r [OpenGLES2] --UseFeatures [EGL,OpenGLES2] --BuildTime
```

-----------------------------------------------------------------------------------------------------------------------------------------------------

Build and run all GLES3 apps and produce a screenshot for each using the Arm OpenGLES emulator (beware this also builds 3.1 apps)

```bash
set FSLBUILD_VARIANT_FSL_GLES_NAME=arm
FslBuild.py --ForAllExe "(EXE) --Window [0,0,1920,1080] --ScreenshotFrequency 5 --ExitAfterFrame 5 --ForceUpdateTime 16666" -t sdk -r [OpenGLES3] --BuildTime
```

-----------------------------------------------------------------------------------------------------------------------------------------------------

Build and run all GLES3.1 apps and produce a screenshot for each using the Arm OpenGLES emulator

```bash
set FSLBUILD_VARIANT_FSL_GLES_NAME=arm
FslBuild.py --ForAllExe "(EXE) --Window [0,0,1920,1080] --ScreenshotFrequency 5 --ExitAfterFrame 5 --ForceUpdateTime 16666" -t sdk -r [OpenGLES3.1] --BuildTime
```

-----------------------------------------------------------------------------------------------------------------------------------------------------

Build and run all Vulkan apps, screenshots are not supported at this time.

```bash
FslBuild.py --ForAllExe "(EXE) --Window [0,0,1920,1080] --ExitAfterFrame 5 --ForceUpdateTime 16666" -t sdk -r [Vulkan] --BuildTime
```

-----------------------------------------------------------------------------------------------------------------------------------------------------

Build and run all OpenVX apps, screenshots are not supported at this time.

```bash
FslBuild.py --ForAllExe "(EXE) --Window [0,0,1920,1080] --ExitAfterFrame 5 --ForceUpdateTime 16666" -t sdk -r [OpenVX] --BuildTime
```

-----------------------------------------------------------------------------------------------------------------------------------------------------

```bash
FslBuild.py --ForAllExe "(EXE) --Window [0,0,1920,1080] --ScreenshotFrequency 5 --ExitAfterFrame 5 --ForceUpdateTime 16666" -t sdk -r [OpenGLES2] --BuildTime
FslBuild.py --ForAllExe "(EXE) --Window [0,0,1920,1080] --ScreenshotFrequency 5 --ExitAfterFrame 5 --ForceUpdateTime 16666" -t sdk -r [OpenGLES3] --BuildTime
FslBuild.py --ForAllExe "(EXE) --Window [0,0,1920,1080] --ExitAfterFrame 5 --ForceUpdateTime 16666" -t sdk -r [Vulkan] --BuildTime
FslBuild.py --ForAllExe "(EXE) --Window [0,0,1920,1080] --ScreenshotFrequency 5 --ExitAfterFrame 5 --ForceUpdateTime 16666" -t sdk -r [OpenVG] --BuildTime
FslBuild.py --ForAllExe "(EXE) --Window [0,0,1920,1080] --ExitAfterFrame 5 --ForceUpdateTime 16666" -t sdk -r [OpenCL] --BuildTime
FslBuild.py --ForAllExe "(EXE) --Window [0,0,1920,1080] --ExitAfterFrame 5 --ForceUpdateTime 16666" -t sdk -r [OpenCV] --BuildTime
FslBuild.py --ForAllExe "(EXE) --Window [0,0,1920,1080] --ExitAfterFrame 5 --ForceUpdateTime 16666" -t sdk -r [OpenVX] --BuildTime
```

Beware G2D is not supported on windows:

```bash
FslBuild.py --ForAllExe "(EXE) --Window [0,0,1920,1080] --ExitAfterFrame 5 --ForceUpdateTime 16666" -t sdk -r [G2D] --BuildTime
```

Run the WinScreenshot bat script (only works on windows)

```bash
FslBuild.py --ForAllExe "WinScreenshot (EXE)"
```
