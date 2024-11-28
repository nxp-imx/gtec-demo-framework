
# Setup guide Emscripten (experimental)

Please beware that emscripten support is highly experimental and incomplete. Many samples might not work at all.

## Table of contents

<!-- #AG_TOC_BEGIN# -->
* [Prerequisites](#prerequisites)
* [Simple setup](#simple-setup)
* [To Compile and run an existing sample application](#to-compile-and-run-an-existing-sample-application)
  * [To Compile and run an existing GLES2 sample application](#to-compile-and-run-an-existing-gles2-sample-application)
* [To create a new demo project named 'CoolNewDemo'](#to-create-a-new-demo-project-named-'coolnewdemo')
<!-- #AG_TOC_END# -->

# Prerequisites

* [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html)
  (community or better)
* [CMake 3.16 or newer](https://cmake.org/download/)
* [Python 3.10+](https://www.python.org/ftp/python/3.10.11/python-3.10.11-amd64.exe)
  To be able run python scripts.
* Download the source from git.

It's also a good idea to read the introduction to the [FslBuild toolchain](./FslBuild_toolchain_readme.md)

---------------------------------------------------------------------------------------------------

# Simple setup

1. Ensure that the emscripten environment has been configured as per the install guide. On windows you need to run ```emsdk_env.bat```
2. Start a console in the DemoFramework folder
3. Run the `prepare.bat` (windows) or `. prepare.sh` (unix) located in the root of the framework folder.
   configure the necessary environment variables and paths.
   Please beware that the `prepare` files requires the current working
   directory to be the root of your demo-framework folder to function
   (which is also the folder it resides in).

    Windows:

    ```bash
    prepare.bat
    ```

    unix:

    ```bash
    . prepare.sh
    ```

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

3. Build and run the project

    ```bash
    FslBuildRun.py -p emscripten --dev
    ```

## To Compile and run an existing GLES2 sample application

In this example we will utilize the GLES2.S06_Texturing app.

1. Make sure that you performed the [simple setup](#simple-setup), including installing and configuring a OpenGLES emulator.
2. Change directory to the sample directory:

    ```bash
    cd DemoApps\GLES2\S06_Texturing
    ```

3. Build and run the project

    ```bash
    FslBuildRun.py -p emscripten --dev
    ```

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

