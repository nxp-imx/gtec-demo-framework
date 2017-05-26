Prerequisites
=============
- [Visual Studio 2015](https://www.visualstudio.com/vs/community/)
  (community or better)
- [Python 2.7.x](https://www.python.org/ftp/python/2.7.13/python-2.7.13.msi)
  To be able run python scripts.
  
- One of these:
  - [Arm Mali OpenGL ES Emulator 3.0.2.g694a9 (64 bit)](https://developer.arm.com/products/software-development-tools/graphics-development-tools/opengl-es-emulator/downloads)

    *Please use the exact version (64bit) and use the installer to install it to the default location!*
  - Vivante OpenGL ES Emulator

For OpenCL, OpenCV and OpenVX support additional packages are required, see below.
  
To get started its recommended to utilize the Arm Mali OpenGL ES 3.0.2 emulator (64 bit) 
which this guide will assume you are using.

- Download the source from git.

It's also a good idea to read the introduction to the [FslBuild toolchain](./FslBuild_toolchain_readme.md)

Simple setup
============
1. Start a windows console (cmd.exe) in the DemoFramework folder
2. Run the `prepare.bat` file located in the root of the framework folder to
   configure the necessary environment variables and paths.
   Please beware that the `prepare.bat` file requires the current working 
   directory to be the root of your demoframework folder to function 
   (which is also the folder it resides in).
 

To Compile and run an existing GLES2 sample application 
-------------------------------------------------------
In this example we will utilize the GLES2.S06_Texturing app.

1. Make sure that you performed the [simple setup].
2. Change directory to the sample directory:
    ```bash
    cd DemoApps\GLES2\S06_Texturing
    ```
3. Generate build files
    ```bash
    FslBuildGen.py
    ```
4. Launch visual studio using the Arm Mali Emulator:
    ```bash
    .StartProject.bat arm
    ```
5. Compile and run the project (The default is to press F5)
   
To utilize a different emulator the `.StartProject.bat` file can be launched with the following arguments

argument|emulator
--------|-----------------------------------------------------------------------------------
arm     |the arm mali emulator
powervr |the powervr emulator
qualcomm|the qualcomm andreno adreno emulator (expects its installed in "c:\AdrenoSDK")
vivante |the vivante emulator
  
To create a new GLES2 demo project named 'CoolNewDemo'
------------------------------------------------------  
1. Make sure that you performed the [simple setup].
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
5. Generate build files for Android, Ubuntu and Yocto (this step will be simplified soon)
    ```bash
    FslBuildGen.py
    ```
6. Launch visual studio using the Arm Mali Emulator:
    ```bash
    .StartProject.bat arm
    ```
7. Compile and run the project (The default is to press F5) or start creating your new demo.


If you add source files to a project or change the Fsl.gen file then run the 
FslBuildGen.py script in the project root folder to regenerate the various 
build files.


Building OpenCV 2.4.11 demo framework apps:
-------------------------------------------
1. See [Setup OpenCV 2.4.11]
2. Run `prepare.bat` as usual.
3. Add a dependency to OpenCV to your "fsl.gen" file like this
    ```xml
    <Dependency Name="OpenCV2_4"/>
    ```
   See [DemoApps/GLES3/OpenCV101/Fsl.gen](../DemoApps/GLES3/OpenCV101/Fsl.gen) for how its done.
4. Run `FslBuildGen.py` to regenerate the project files.   


Building OpenCV 3.1 demo framework apps:
----------------------------------------
1. See [Setup OpenCV 3.1]
2. Run `prepare.bat` as usual.
3. Add a dependency to OpenCV to your "fsl.gen" file like this
    ```xml
    <Dependency Name="OpenCV3_1"/>
    ```
   See DemoApps/GLES3/OpenCV101_V3_1/Fsl.gen for how its done.
4. Run `FslBuildGen.py` to regenerate the project files.   


Building OpenCL demo framework apps:
------------------------------------
1. Download and install the desired OpenCL sdk from Amd
   - [Amd (AMD APP SDK 3.0)](http://developer.amd.com/tools-and-sdks/opencl-zone/amd-accelerated-parallel-processing-app-sdk/)
2. Generate a new project add a dependency to OpenCL by adding 
    ```xml
    <Dependency Name="OpenCL"/>
    ```
   to the projects "Fsl.gen" file.
   See [DemoApps/GLES3/OpenCL101/Fsl.gen](../DemoApps/GLES3/OpenCL101/Fsl.gen) for a example.
3. Run `FslBuildGen.py` to regenerate the project files.   
4. Start the project


Building OpenVX demo framework apps:
------------------------------------
1. See [Setup OpenVX]
2. Generate a new project add a dependency to OpenVX by adding 
    ```xml
    <Dependency Name="OpenVX"/>
    ```
   to the projects "Fsl.gen" file.
   See [DemoApps/GLES3/OpenVX101/Fsl.gen](../DemoApps/GLES3/OpenVX101/Fsl.gen) for a example.
3. Run `FslBuildGen.py` to regenerate the project files.   
4. Start the project


Setup OpenCV 2.4.11
-------------------
1. Install the 2.4.11 sdk files.
2. Configure the environment variable OPENCV2_4_DIR to point to the sdk location like this
    ```bash
    set OPENCV2_4_DIR=c:\_sdk\OpenCV-2.4.11\build
    ```

Setup OpenCV 3.1
----------------
1. Install the 3.1 sdk files.
2. Configure the environment variable OPENCV_DIR to point to the sdk location like this
    ```bash
    set OPENCV_DIR=c:\_sdk\OpenCV-3.1.\build
    ```


Setup OpenVX
------------
1. Follow the guide for setting up OpenCL and OpenCV support. (need OpenCV3.1)

   
### Legacy Git note:
When you add the generated build.sh to git on windows then please remember to set the 
executable bit using: 
```bash
git update-index --chmod=+x build.sh
```

--------------------------------------------------------------------------------
Notes for building AMD OpenVX files yourself
============================================
WARNING: only do this if you have issues with the precompiled version we include

Building OpenVX demo framework apps
-----------------------------------
1. Download and build the OpenVX sdk using the AMD SDK Setup below
2. Generate a new project add a dependency to OpenVX by adding 
    ```xml
    <Dependency Name="OpenVX"/>
    ```
   to the projects "Fsl.gen" file
   See [DemoApps/GLES3/OpenVX101/Fsl.gen](../DemoApps/GLES3/OpenVX101/Fsl.gen) for a example.
3. Run `FslBuildGen.py` to regenerate the project files.   
4. Start the project


AMD SDK Setup
-------------
1. Make sure you have cmake 

   You can get it here: https://cmake.org/download/
2. [Amd (AMD APP SDK 3.0)](http://developer.amd.com/tools-and-sdks/opencl-zone/amd-accelerated-parallel-processing-app-sdk/)
3. Make sure your OpenCV_DIR environment variable points to the OpenCV 3.1 build directory
    ```bash
    set OpenCV_DIR=c:\sdk\OpenCV-3.1\build
    ```
4. Download the AMD OpenVX sdk code
    ```bash
    git clone https://github.com/GPUOpen-ProfessionalCompute-Libraries/amdovx-core.git
    ```
5. Make sure that OpenVX finds the AMD OpenCL implementation and the OpenCV directory
    ```bash
    cmake-gui.exe 
    ```
   Point it to the cmake directory you cloned the OpenVX files to then use the 'advanced' checkbox to see the make environment
6. Generate the build files for Visual Studio 12 (2013) (x86) using cmake
7. Open the `%FSL_AMD_OPENVX_PATH%\openvx.sln` project file and 
   build the release version of OpenVX.
   - If you dont have a supported AMD gpu set the build flag ENABLE_OPENCL=0 before you build!
8 Copy the build files into the correct locations under %FSL_GRAPHICS_SDK%\ThirdParty\Precompiled\amdovx-core-0.9-beta1
9. Do the normal setup
10. You can now build OpenVX apps
