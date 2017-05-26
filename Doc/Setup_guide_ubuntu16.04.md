Prerequisites
=============
- Ubuntu16.04 64 bit
- Build tools and xrand
  ```bash
  sudo apt-get install build-essential libxrandr-dev
  ```
- Python 2.7 

  To be able run python scripts, they are not needed to build.
  It should be part of the default Ubuntu install.
 
- One of these (they are mutually exclusive):
  - Mesa OpenGL ES 2
    ```bash
    sudo apt-get install libgles2-mesa-dev
    ```
  - [Arm Mali OpenGL ES 3.0 Emulator V1.4.1 (64 bit)](http://malideveloper.arm.com/develop-for-mali/tools/opengl-es-3-0-emulator/)
    ```bash
    wget http://malideveloper.arm.com/downloads/tools/emulator/1.4.1/Mali_OpenGL_ES_Emulator-1.4.1-Linux-64bit.deb
    sudo dpkg -i Mali_OpenGL_ES_Emulator-1.4.1-Linux-64bit.deb
    ```

- DevIL (Developer's Image Library)
  ```bash
  sudo apt-get install libdevil-dev 
  ```
    
- Assimp (Open Asset Import Library)
  ```bash
  sudo apt-get install libassimp-dev
  ```
    
- Download the source from git.

It's also a good idea to read the introduction to the [FslBuild toolchain](./FslBuild_toolchain_readme.md)

Simple setup
============
  1. Start a terminal (ctrl+alt t) in the DemoFramework folder
  2. Run the `prepare.sh` file located in the root of the framework folder to
     configure the necessary environment variables and paths.
     Please beware that the `prepare.sh` file requires the current working 
     directory to be the root of your demoframework folder to function 
     (which is also the folder it resides in).
```bash
      source prepare.sh
```
 
 
To Compile all samples
----------------------
  1. Make sure that you performed the [simple setup].
  2. Compile everything (a good rule of thumb for '-j N' is number of cpu cores * 2)
```bash
      FslBuild.py -- -j 2
```

     
To Compile and run an existing sample application 
-------------------------------------------------
In this example we will utilize the GLES2.S06_Texturing app.
  1. Make sure that you performed the [simple setup].
  2. Change directory to the sample directory:
    ```bash
    cd DemoApps/GLES2/S06_Texturing
    ```
  3. Compile the project (a good rule of thumb for '-j N' is number of cpu cores * 2)
    ```bash
    FslBuild.py -- -j 2
    ```

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
  5. Compile the project (a good rule of thumb for '-j N' is number of cpu cores * 2)
    ```bash
    FslBuild.py -- -j 2
    ```

Note: 

Once a build has been done once you can just invoke the make file directly.
However this requires that you didn't change any dependencies or add files.
To do this run 
```bash
      make -j 2
```

If you add source files to a project or change the Fsl.gen file then run the 
`FslBuildGen.py` script in the project root folder to regenerate the various 
build files or just make sure you always use the `FslBuild.py` script as it 
automatically adds files and regenerate build files as needed.

Building Vulkan demo framework apps
-----------------------------------

See the [official SDK guide](https://vulkan.lunarg.com/doc/sdk/latest/linux/getting_started.html)

1. Download the vulkan sdk from https://vulkan.lunarg.com/sdk/home
2. Make the downloaded file executable
    ```bash
    chmod ugo+x vulkansdk-linux-x86_64-1.0.30.0.run
    ```
3. Move the downloaded file to a sdk dir
    ```bash
    mkdir ~/vulkan
    mv vulkansdk-linux-x86_64-1.0.30.0.run ~/vulkan
    ```
4. Run it
    ```bash
    cd ~/vulkan
    ./vulkansdk-linux-x86_64-1.0.30.0.run
    ```
5. Install the necessary packages
    ```bash
    sudo apt-get install libglm-dev graphviz libxcb-dri3-0 libxcb-present0 libpciaccess0 cmake libpng-dev libxcb-dri3-dev libx11-dev
    ```
6. Setup the vulkan environment
    ```bash
    pushd ~/vulkan/VulkanSDK/1.0.30.0
    source setup-env.sh
    popd
    ```
7. Ensure that the LIBRARY_PATH is set for GCC
    ```bash
    export LIBRARY_PATH=$VULKAN_SDK/lib:$LIBRARY_PATH     
    ```
8. Run the normal setup.


Building OpenCV 2.4.11 demo framework apps
------------------------------------------
1. Follow the normal setup procedure for the sdk
2. Install the required packages
    ```bash
    sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
    ```
3. Install the optional packages
    ```bash
    sudo apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev
    ```
4. Create a sdk directory
    ```bash
    mkdir ~/sdk
    cd ~/sdk
    ```
5. Download the 2.4.11 release for ubuntu, unzip it, remove the download, enter the directory
    ```bash
    wget https://github.com/Itseez/opencv/archive/2.4.11.zip
    unzip 2.4.11.zip
    rm 2.4.11.zip
    cd opencv-2.4.11
    ```
6. Build OpenCV
    ```bash
    mkdir release
    cd release
    cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local ..
    make
    sudo make install
    ```
7. Ensure that you can locate the OpenCV .so files
    ```bash
    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"
    ```
8. Add a dependency to OpenCV to your "fsl.gen" file like this
    ```xml
    <Dependency Name="OpenCV"/>
    ```
   See DemoApps/GLES2/OpenCV101/Fsl.gen for how its done.

9. Run FslBuildGen.py to regenerate the project files. 

Building OpenCL demo framework apps
-----------------------------------
- This is a guide that works for running inside a virtual machine.
  It will use the AMD OpenCL CPU implementation. 
  If you encounter problems or want to use proper GPU accelleration please refer to AMD's guidelines.

1. Download the [AMD sdk](http://developer.amd.com/tools-and-sdks/opencl-zone/amd-accelerated-parallel-processing-app-sdk/) 
   (AMD APP SDK v2.9.1 beware the 3.0 release appears to be broken) 
     
2. Go to the location of the downloaded file AMD-APP-SDKInstaller-v3.0.130.136-GA-linux64.tar.bz2
3. Extract the sdk (Follow the next three steps or look at the install guide)
    ```bash
    tar xvjf AMD-APP-SDK-linux-v2.9-1.599.381-GA-x64.tar.bz2
    ```
4. Remove the downloaded file
    ```bash
    rm AMD-APP-SDK-linux-v2.9-1.599.381-GA-x64.tar.bz2
    ```
5. Run the extracted script
    ```bash
    sudo ./AMD-APP-SDK-v2.9-1.599.381-GA-linux64.sh
    ```
6. Ensure that the environment variable AMDAPPSDKROOT is set (You might need to restart your terminal or logoff)
    ```bash
    printenv AMDAPPSDKROOT
    ```
7. You are now ready to build and run OpenCL apps.     
