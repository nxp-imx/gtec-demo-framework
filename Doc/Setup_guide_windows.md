
# Setup guide Windows

## Table of contents

<!-- #AG_TOC_BEGIN# -->
* [Prerequisites](#prerequisites)
* [Simple setup](#simple-setup)
  * [Add OpenGL ES support](#add-opengl-es-support)
  * [Add Vulkan support](#add-vulkan-support)
  * [Add OpenCL support](#add-opencl-support)
  * [Add OpenCV 4.5 support](#add-opencv-45-support)
  * [Add OpenVX support](#add-openvx-support)
* [To Compile and run an existing sample application](#to-compile-and-run-an-existing-sample-application)
  * [To Compile and run an existing GLES2 sample application](#to-compile-and-run-an-existing-gles2-sample-application)
* [To create a new demo project named 'CoolNewDemo'](#to-create-a-new-demo-project-named-'coolnewdemo')
  * [So to create a new GLES2 demo project named 'CoolNewDemo'](#so-to-create-a-new-gles2-demo-project-named-'coolnewdemo')
* [Visual Studio Code support](#visual-studio-code-support)
  * [VS Code Prerequisites](#vs-code-prerequisites)
  * [To Compile and run an existing sample application using VSCode](#to-compile-and-run-an-existing-sample-application-using-vscode)
* [Appendix](#appendix)
  * [Appendix: Windows configure OpenGLS emulation bat file changes](#appendix-windows-configure-opengls-emulation-bat-file-changes)
  * [Appendix: Notes](#appendix-notes)
<!-- #AG_TOC_END# -->

# Prerequisites

* [Visual Studio 2019](https://visualstudio.microsoft.com/vs/community/)
  (community or better)
* [CMake 3.10.2 or newer](https://cmake.org/download/)
* [Python 3.6+](https://www.python.org/ftp/python/3.8.3/python-3.8.3-amd64.exe)
  To be able run python scripts.
* Download the source from git.

For OpenGL ES, OpenCL, OpenCV, OpenVX and Vulkan support additional packages are required, see below.

It's also a good idea to read the introduction to the [FslBuild toolchain](./FslBuild_toolchain_readme.md)

---------------------------------------------------------------------------------------------------

# Simple setup

1. Decide what API's you want to compile and run apps for, then install them using one of the guides below.
2. Start a windows console (cmd.exe) in the DemoFramework folder
3. Run the visual studio ```vcvarsall.bat x64``` to prepare your command line compiler environment for x64 compilation.
   * For VS2019 its often located here: ```"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64```
4. Run the `prepare.bat` file located in the root of the framework folder to
   configure the necessary environment variables and paths.
   Please beware that the `prepare.bat` file requires the current working
   directory to be the root of your demo-framework folder to function
   (which is also the folder it resides in).

    ```bash
    prepare.bat
    ```

## Add OpenGL ES support

1. One of these (they are mutually exclusive):
    * [Arm Mali OpenGL ES Emulator v3.0.4-2c-g8d905 (64 bit)](https://developer.arm.com/tools-and-software/graphics-and-gaming/opengl-es-emulator/downloads)

       *Please use the exact version (64bit) and use the installer to install it to the default location!*
    * Vivante OpenGL ES Emulator

    To get started its recommended to utilize the Arm Mali OpenGL ES 3.0.4 emulator (64 bit)
    which this guide will assume you are using.

2. Configure the OpenGL ES emulator for arm

    ```bash
    ConfigureOpenGLESEmu.bat arm
    ```

3. Continue the normal setup.

To utilize a different emulator the `ConfigureOpenGLESEmu.bat` file can be launched with the following arguments

argument|emulator
--------|-----------------------------------------------------------------------------------
arm     |the arm mali emulator
powervr |the powervr emulator
qualcomm|the qualcomm andreno adreno emulator (expects its installed in "c:\AdrenoSDK")
vivante |the vivante emulator

## Add Vulkan support

Install the Vulkan SDK, See the [official SDK guide](https://vulkan.lunarg.com/doc/sdk/latest/windows/getting_started.html)

1. [Download the Vulkan sdk](https://vulkan.lunarg.com/sdk/home)
2. Follow the [documentation](https://vulkan.lunarg.com/doc/sdk/latest/linux/getting_started.html)

3. Continue the normal setup.

## Add OpenCL support

1. Download and install a OpenCL implementation. Beware we depend on cmake's find_package support to locate the installed OpenCL implementation.
2. Continue the normal setup.

## Add OpenCV 4.5 support

1. Install the 4.5.2 sdk files.
2. Configure the environment variable OPENCV_DIR to point to the sdk location like this

    ```bash
    set OPENCV_DIR=c:\_sdk\opencv-4.5.2\build
    ```

3. Continue the normal setup.

## Add OpenVX support

1. Follow the guide for setting up OpenCL and OpenCV support. (need OpenCV4)
2. Continue the normal setup.

The build chain will download and compile a OpenVX implementation.

---------------------------------------------------------------------------------------------------

# To Compile and run an existing sample application

The general approach will be:

1. Make sure that you performed the [simple setup](#simple-setup), including the API's the sample need.
2. Change directory to the sample directory:

    ```bash
    cd DemoApps\<MAIN-API-NAME>\<SAMPLE-NAME>
    ```

    Example MAIN-API-NAME's: GLES2, GLES3, OpenCL, Vulkan.

    Type ```dir``` to see the dir choice.

    Example SAMPLE-NAME's: Bloom, S06_Texturing.

    Type ```dir``` after entering a API folder to see the list of samples

3. Launch the visual studio project

    ```bash
    FslBuild.py -c open
    ```

4. Verify that the executable is set as the startup project in the solution explorer, if its not then right click the executable and select "Set as Startup Project".
   If this is not done you are likely to get a "access denied" error when you try to run the project as it might try to run the "ALL_BUILD" target instead.
5. Compile and run the project (The default is to press F5)

The following commands are also pretty useful to know.

Command                                       | Description
----------------------------------------------|---------------------------------------
`FslBuild.py`                                 | To build the example.
`FslBuildRun.py`                              | To build and run the example.
`FslBuild.py --ListVariants`                  | List all the build variants that can be specified.
`FslBuildInfo.py --ListRequirements`          | To list the samples requirements.
`FslBuild.py -c open2`                        | *Experimental* [Open the project in Visual Studio Code](#Visual-Studio-Code-Support)

It is also recommended to check out the `README.md` and `Example.jpg` that is included with all samples.

Note:

If you add source files to a project or change the Fsl.gen file then run the
`FslBuildGen.py` script in the project root folder to regenerate the various
build files or just make sure you always use the `FslBuild.py` script as it
automatically adds files and regenerate build files as needed.

## To Compile and run an existing GLES2 sample application

In this example we will utilize the GLES2.S06_Texturing app.

1. Make sure that you performed the [simple setup](#simple-setup), including installing and configuring a OpenGLES emulator.
2. Change directory to the sample directory:

    ```bash
    cd DemoApps\GLES2\S06_Texturing
    ```

3. Launch the visual studio project

    ```bash
    FslBuild.py -c open
    ```

4. Compile and run the project (The default is to press F5)

---------------------------------------------------------------------------------------------------

# To create a new demo project named 'CoolNewDemo'

1. Make sure that you performed the [simple setup](#simple-setup) including the additional OpenGL ES setup.
2. Change directory to the appropriate sample directory:

    ```bash
    cd DemoApps\<MAIN-API-NAME>
    ```

    Example MAIN-API-NAME's: GLES2, GLES3, OpenCL, Vulkan.

    Type ```dir``` to see the dir choice.

3. Create the project template using the FslBuildNew.py script

    ```bash
    FslBuildNew.py <TEMPLATE-NAME> CoolNewDemo
    ```

    Example TEMPLATE-NAME's: GLES2, GLES3, OpenCL1_2, OpenCV4, Vulkan.

    To get a full list run ```FslBuildNew.py . . --List```

4. Change directory to the newly created project folder 'CoolNewDemo'

    ```bash
    cd CoolNewDemo
    ```

5. Launch the visual studio project

    ```bash
    FslBuild.py -c open
    ```

Note:

If you add source files to a project or change the Fsl.gen file then run the
`FslBuildGen.py` script in the project root folder to regenerate the various
build files or just make sure you always use the `FslBuild.py` script as it
automatically adds files and regenerate build files as needed.

## So to create a new GLES2 demo project named 'CoolNewDemo'

In this example we will create a GLES2 app called CoolNewDemo.

1. Make sure that you performed the [simple setup](#simple-setup) including the additional OpenGL ES setup.
2. Change directory to the GLES2 sample directory:

    ```bash
    cd DemoApps/GLES2
    ```

3. Create the project template using the FslBuildNew.py script

    ```bash
    FslBuildNew.py GLES2 CoolNewDemo
    ```

4. Change directory to the newly created project folder 'CoolNewDemo'

    ```bash
    cd CoolNewDemo
    ```

5. Launch the visual studio project

    ```bash
    FslBuild.py -c open
    ```

---------------------------------------------------------------------------------------------------

# Visual Studio Code support

There is now ***experimental*** visual studio code support.
To support visual studio code we generate/patch the configuration files stored under ```.vscode```.

* ```launch.json``` contains the configuration used to launch the executable if one exist
* ```settings.json``` contains the cmake configuration required to build the project.

## VS Code Prerequisites

* [Visual Studio Code](https://code.visualstudio.com/docs/setup/windows)
* Visual Studio Code Extensions:
  * [CMake Tools](https://code.visualstudio.com/docs/cpp/CMake-linux)
  * [C/C++](https://code.visualstudio.com/docs/languages/cpp)
* Recommended visual studio code extensions:
  * [Code Spell Checker](https://github.com/streetsidesoftware/vscode-spell-checker)
  * [Gitlens - Git supercharged](https://github.com/eamodio/vscode-gitlens)
  * [Visual Studio Keymap](https://github.com/rebornix/vscode-vs-keybindings)

## To Compile and run an existing sample application using VSCode

In this example we will utilize the GLES2.S06_Texturing app.

1. Make sure that you performed the [simple setup](#simple-setup).
2. Change directory to the sample directory:

    ```bash
    cd DemoApps/GLES2/S06_Texturing
    ```

3. Open the project in visual studio code

    ```bash
    FslBuild.py -c open2
    ```

    This should launch visual studio code configured for the app.

***BEWARE***: Visual Studio Code has its own setting for debug/release build which is configured in the bottom blue line.
***BEWARE***: Make sure you use the correct launch script. "FslBuild-Windows" for windows and "FslBuild-Ubuntu" for ubuntu.

---------------------------------------------------------------------------------------------------

# Appendix

## Appendix: Windows configure OpenGLS emulation bat file changes

* Introduced FSL_GLES_EMULATOR_PATH as the main path to the emulation installation
* Renamed FSL_GLES_NAME to FSL_GLES_EMULATOR_NAME
* Renamed FSL_GLES_LIB_PATH to FSL_GLES_EMULATOR_LIB_PATH
* Removed FSL_GLES_INCLUDE_PATH
* Removed FSL_GLES_LIB_EGL
* Removed FSL_GLES_LIB_GLES

---------------------------------------------------------------------------------------------------

## Appendix: Notes

### Legacy Git note

When you add the generated build.sh to git on windows then please remember to set the
executable bit using:

```bash
git update-index --chmod=+x build.sh
```

### Building AMD OpenVX files yourself

WARNING: only do this if you have issues with the pre-compiled version we include
