
# Setup guide Android SDK+NDK

## Table of contents

<!-- #AG_TOC_BEGIN# -->
* [Prerequisites](#prerequisites)
* [Simple setup](#simple-setup)
* [To compile and run an existing sample application](#to-compile-and-run-an-existing-sample-application)
* [To compile and run an existing GLES2 sample application](#to-compile-and-run-an-existing-gles2-sample-application)
* [To create a new GLES2 demo project named 'CoolNewDemo'](#to-create-a-new-gles2-demo-project-named-'coolnewdemo')
* [Using android studio](#using-android-studio)
* [Command line arguments](#command-line-arguments)
  * [Android studio](#android-studio)
  * [Adb](#adb)
* [Linux notes](#linux-notes)
<!-- #AG_TOC_END# -->

# Prerequisites

* **IMPORTANT!**
  The way gradle currently handles CMake builds on windows place some serious limits on
  the path length, so its recommended to either place the DemoFramework folder close to
  the root of the drive or to set the environment variable
  FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR to a directory close to the root of the drive.

* [JDK (64 bit)](http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html)
  * Make sure to configure JAVA_HOME to point to the JDK directory.
  * Using the 32bit sdk might give you build memory issues.

* [Android SDK](https://developer.android.com/studio/index.html)

  Once its installed its a good idea to run "SDK Manager.exe" and make sure everything is up to date.

  **IMPORTANT!**
  * Android studio must be at least 4.1.2
  * Get the android studio full package and enable the default packages.
  * Configure the SDK manager
    * "SDK Platforms" add if necessary "Android 12.0" which is **SDK 31!**
    * "SDK Tools" add if necessary "CMake, LLDB, NDK, Android Support Repository"
  * Make sure to configure ANDROID_HOME to point to the android sdk directory
  * Make sure to configure ANDROID_NDK to point to the android ndk directory
  * Make sure you have at least android-ndk-r25b.
    * We recommend the latest LTS which currently is **NDK r25b**.

* [Python 3.6+](https://www.python.org/ftp/python/3.8.1/python-3.8.1-amd64.exe)

  To be able run python scripts, they are needed to build.

* Download the source from git.

It's also a good idea to read the introduction to the [FslBuild toolchain](./FslBuild_toolchain_readme.md)

---------------------------------------------------------------------------------------------------

# Simple setup

Android projects are generated to the path specified in the environment variable
FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR. If it's not defined the 'prepare' script
sets it to a default location.

1. Start a windows console (cmd.exe) in the DemoFramework folder
2. Run the `prepare.bat` file located in the root of the framework folder to
    configure the necessary environment variables and paths.
    Please beware that the `prepare.bat` file requires the current working
    directory to be the root of your demoframework folder to function
    (which is also the folder it resides in).

---------------------------------------------------------------------------------------------------

# To compile and run an existing sample application

The general approach will be:

1. Make sure that you performed the [simple setup](#simple-setup), including the API's the sample need.
2. Change directory to the sample directory:

    ```bash
    cd DemoApps/<MAIN-API-NAME>/<SAMPLE-NAME>
    ```

    Example MAIN-API-NAME's: GLES2, GLES3, OpenCL, Vulkan.

    Type ```dir``` to see the dir choice.

    Example SAMPLE-NAME's: Bloom, S06_Texturing.

    Type ```dir``` after entering a API folder to see the list of samples

3. Build the application

    ```bash
    FslBuild.py -p android
    ```

If you want to save a bit of compilation time you can build for the ANDROID ABI you need by adding

```bash
FslBuild.py -p android --Variants [ANDROID_ABI=armeabi-v7a]
```

The following commands are also pretty useful to know.

Command                                         | Description
------------------------------------------------|---------------------------------------
`FslBuildGen.py -p android`                     | Generate the project which can then be opened in gradle
`FslBuild.py -p android --ListVariants`         | List all the build variants that can be specified. This is useful to see the ANDROID_ABI's.
`FslBuildInfo.py -p android --ListRequirements` | To list the samples requirements.

It is also recommended to check out the `README.md` and `Example.jpg` that is included with all samples.

Note:

If you add source files to a project or change the Fsl.gen file then run the
`FslBuildGen.py` script in the project root folder to regenerate the various
build files or just make sure you always use the `FslBuild.py` script as it
automatically adds files and regenerate build files as needed.

# To compile and run an existing GLES2 sample application

In this example we will utilize the GLES2.S06_Texturing app.

1. Make sure that you performed the [simple setup](#simple-setup)
2. Change directory to the sample directory:

    ```bash
    cd DemoApps\GLES2\S06_Texturing
    ```

3. Build a app for Android using gradle + cmake

    ```bash
    FslBuild.py -p android
    ```

---------------------------------------------------------------------------------------------------

# To create a new GLES2 demo project named 'CoolNewDemo'

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

---------------------------------------------------------------------------------------------------

# Using android studio

1. Follow the instructions for "creating a new project" or "building an existing project".
2. As projects are generated to the path specified by the FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR
    environment variable you can locate the project there and open it with android studio.
    Be sure to open Android studio in a correctly configured environment.
    Here it could be a good idea to create a script for launching android studio with the
    right environment.

If you add source files to a project or change the Fsl.gen file then run the
FslBuildGen.py script in the project root folder to regenerate the various
build files.

---------------------------------------------------------------------------------------------------

# Command line arguments

## Android studio

Android Studio->Run->Edit Configurations->App->Launch Flags:

Add the arguments using ```-e "args" "<arguments>"```

```bash
-e "args" "--DensityDpi 160 -vvvv"
```

## Adb

Add the arguments using ```-e "args" "<arguments>"```

```bash
-e "args" "--DensityDpi 160 -vvvv"
```

---------------------------------------------------------------------------------------------------

# Linux notes

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

* https://developer.android.com/studio/install.html
* http://askubuntu.com/questions/634082/how-to-install-android-studio-on-ubuntu
