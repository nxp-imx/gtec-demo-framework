# Setup guide Android SDK+NDK

## Prerequisites

- **IMPORTANT!**
  The way gradle currently handles CMake builds on windows place some serious limits on
  the path length, so its recommended to either place the DemoFramework folder close to
  the root of the drive or to set the environment variable
  FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR to a directory close to the root of the drive.

- [JDK (64 bit)](http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html)
  - Make sure to configure JAVA_HOME to point to the JDK directory.
  - Using the 32bit sdk might give you build memory issues.

- [Android SDK](https://developer.android.com/studio/index.html)

  Once its installed its a good idea to run "SDK Manager.exe" and make sure everything is up to date.
  
  **IMPORTANT!**
  - Android studio must be at least 3.5.3
  - Get the android studio full package and enable the default packages.
  - Configure the SDK manager
    - "SDK Platforms" add if necessary "Android 9.0 (Pie)"
    - "SDK Tools" add if necessary "CMake, LLDB, NDK, Android Support Repository"
  - Make sure to configure ANDROID_HOME to point to the android sdk directory
  - Make sure to configure ANDROID_NDK to point to the android ndk directory
  - Make sure you have at least android-ndk-r19
  
- [Python 3.4+](https://www.python.org/ftp/python/3.8.1/python-3.8.1-amd64.exe)

  To be able run python scripts, they are needed to build.
  If you use 3.4 you need to install the 'typing' library manually so we highly recommended using 3.5 or newer.
  
- Download the source from git.

It's also a good idea to read the introduction to the [FslBuild toolchain](./FslBuild_toolchain_readme.md)

## Simple setup

Android projects are generated to the path specified in the environment variable
FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR. If it's not defined the 'prepare' script
sets it to a default location.

1. Start a windows console (cmd.exe) in the DemoFramework folder
2. Run the `prepare.bat` file located in the root of the framework folder to
    configure the necessary environment variables and paths.
    Please beware that the `prepare.bat` file requires the current working
    directory to be the root of your demoframework folder to function
    (which is also the folder it resides in).

## To Compile and run an existing sample application

In this example we will utilize the GLES2 S06_Texturing app.

1. Make sure that you performed the [simple setup](#simple-setup)
2. Change directory to the sample directory:

    ```bash
    cd DemoApps\GLES2\S06_Texturing
    ```

3. Build a app for Android using gradle + cmake

    ```bash
    FslBuild.py -p android
    ```

If you just want to regenerate the cmake build files then you can just run

```bash
FslBuildGen.py -p android
```

If you want to save a bit of compilation time you can build for the ANDROID ABI you need by adding

```bash
FslBuildGen.py --Variants [ANDROID_ABI=armeabi-v7a]
```

or

```bash
FslBuild.py --Variants [ANDROID_ABI=armeabi-v7a]
```

## To create a new GLES2 demo project named 'CoolNewDemo'
  
1. Make sure that you performed the [simple setup](#simple-setup)
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

5. Build a app for Android using gradle + cmake

    ```bash
    FslBuild.py -p android
    ```

If you just want to regenerate the cmake build files then you can just run

```bash
FslBuildGen.py -p android
```

If you want to save a bit of compilation time you can build for the ANDROID ABI you need by adding

```bash
FslBuildGen.py -p android --Variants [ANDROID_ABI=armeabi-v7a]
```

or

```bash
FslBuild.py -p android --Variants [ANDROID_ABI=armeabi-v7a]
```

Using android studio.

1. Follow the instructions for "creating a new project" or "building an existing project".  
2. As projects are generated to the path specified by the FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR
    environment variable you can locate the project there and open it with android studio.
    Be sure to open Android studio in a correctly configured environment.
    Here it could be a good idea to create a script for launching android studio with the
    right environment.

If you add source files to a project or change the Fsl.gen file then run the
FslBuildGen.py script in the project root folder to regenerate the various
build files.

## Linux notes

Install for private user

Unzip android studio like this:

```bash
sudo unzip android-studio-ide_FILENAME.zip -d ~/sdk
cd ~/sdk/android-studio/bin
./studio.sh  
```

In the UI make sure to install the sdk in a directory you have access to for example

```bash
~/sdk/android-sdk-linux
```

- https://developer.android.com/studio/install.html
- http://askubuntu.com/questions/634082/how-to-install-android-studio-on-ubuntu
