# FslBuild Toolchain

## FslBuildGen.py

Is a cross-platform build-file generator. Which main purpose is to keep all 
build files consistent, in sync and up to date. See [FslBuildGen.docx](FslBuildGen.docx) for
details.

## FslBuild.py

Extends the technology behind `FslBuildGen.py` with additional knowledge about how
to execute the build system for a given platform.

So basically FslBuild works like this

1. Invoke the build-file generator that updates all build files if necessary.
2. Filter the builds request based on the provided feature and extension lists.
3. Build all necessary build files in the correct order.

### FslBuild - Useful arguments

FslBuild comes with a few useful arguments

Argument           | Description
-------------------|------------------------
--ListFeatures     | List all features required by the build and exit.
--ListExtensions   | List all extensions required by the build and exit.
--ListRequirements | List the requirement tree and exit.
--ListVariants     | List all build variants and their options used by the build and exit.
--RequireFeatures  | The list of features that are required for a executable package to be build. For example [OpenGLES2] to build all executables that use OpenGLES2.
--UseFeatures      | Allows you to limit whats build based on a provided feature list. For example [EGL,OpenGLES2]. This parameter defaults to all features.
--UseExtensions    | The list of available extensions to build for. For example [OpenGLES3.1:EXT_geometry_shader,OpenGLES3.1:EXT_tessellation_shader] to allow the OpenGLES3.1 extensions EXT_geometry_shader and EXT_tessellation_shader. You can also specify * for all extensions (default).
--Variants         | specify a list of variants and their configuration for example "--Variants [config=Debug,FSL_GLES_NAME=vivante]" or "--Variants [config=Debug,WindowSystem=X11]" This means that we have build system independent way of doing this so it works on all platforms.  
--BuildThreads     | Configures how many threads the builder should use, beware this is a hint that may be ignored by the builder, can be a number or "auto". Defaults to auto.
--BuildTime        | Time the build and print the result and the end.
-c \<command\>     | Specify the build command type that should be executed: build, clean. Defaults to build.
-t 'sdk'           | Build all demo framework projects
-v                 | Set verbosity level (can be specified multiple times).
--                 | All arguments written after this is send directly to the native build system.

### Important notes

- Don't modify the auto-generated files.
  The FslBuild scripts are responsible for creating all the build files for a
  platform and verifying dependencies. Since all build files are auto generated
  you can never modify them directly as the next build will overwrite your
  changes. Instead add your changes to the `Fsl.gen` files as they control the
  build file generation!
- The 'Fsl.gen' file is the real build file.
- All include and source files in the respective folders are automatically
  added to the build files.

### Build system per platform

Platform | Build system
---------|--------------------------------
Android  | gradle + cmake
Qnx      | make
Ubuntu   | cmake -> ninja
Windows  | Visual studio (IDE or nmake)
Yocto    | cmake -> ninja

## FslBuildInfo.py

`FslBuildInfo.py` can be used to extract useful information about packages and export it to a easy to parse json file.

So for example

```bash
   FslBuildInfo.py --PackageType [Executable] -t sdk --SaveJson "AllMetaData.json"
```

Would export all meta data for all executables in the sdk.

To get a overview of how the features and extensions dependent on eachother you can run

```bash
   FslBuildInfo.py -t sdk --ListRequirements
```

Would currently produce this list

```
Requirements:
- 'EarlyAccess' (introduced by package: OpenVX, OpenCL, OpenCV, G2D, FslGraphics3D.Procedural, Vulkan, Vulkan.DevBatch)
- 'EGL' (introduced by package: EGL)
- 'G2D' (introduced by package: G2D)
- 'OpenCL' V1 (introduced by package: OpenCL)
  - 'OpenCL1.1' V1.1 (introduced by package: OpenCL1_1)
  - 'OpenCL1.2' V1.2 (introduced by package: OpenCL1_2)
- 'OpenCV' (introduced by package: OpenCV)
  - 'OpenCV3' V3 (introduced by package: OpenCV3)
- 'OpenGLES' (introduced by package: GLES)
  - 'OpenGLES2' V2 (introduced by package: GLES2)
  - 'OpenGLES3' V3 (introduced by package: GLES3)
    - 'OpenGLES3.1' V3.1 (introduced by package: GLES3_1)
      - extension: 'EXT_geometry_shader' (introduced by package: GLES3.TessellationSample, GLES3.Tessellation101)
      - extension: 'EXT_tessellation_shader' (introduced by package: GLES3.TessellationSample, GLES3.Tessellation101)
      - 'OpenGLES3.2' V3.2 (introduced by package: GLES3_2)
- 'OpenVG' (introduced by package: OpenVG)
- 'OpenVX' (introduced by package: OpenVX)
  - 'OpenVX1.1' V1.1 (introduced by package: OpenVX1_1)
- 'Vulkan' V1 (introduced by package: Vulkan)
```

Argument            | Description
--------------------|------------------------
--ListBuildVariants | List all build variants.
--ListExtensions    | List all extensions required by the package.
--ListFeatures      | List all features required by the package.
--ListRequirements  | List the requirement tree.
--ListVariants      | List all build variants and their options.
--UseFeatures       | The list of available features used to save packages. For example [OpenGLES2,OpenGLES3.1] or * for everything.
--RequireFeatures   | The list of features that are required for a executable package to be saved. For example [OpenGLES2] to save packages for all executables that use OpenGLES2.
--UseExtensions     | The list of available extensions to save for. For example [OpenGLES3.1:EXT_geometry_shader,OpenGLES3.1:EXT_tessellation_shader] to allow the OpenGLES3.1 extensions EXT_geometry_shader and EXT_tessellation_shader. You can also specify * for all extensions (default).
--PackageType       | The list of package types that will be saved [Executable, Libary, HeaderLibrary, etc]. For example [Executable] to save all packages of the 'Executable' type.
--SaveJson          | Save package information to the given json output file
-t 'sdk'            | Build all demo framework projects
-v                  | Set verbosity level (can be specified multiple times).


## FslBuildNew.py

`FslBuildNew.py` is a new project wizard script. To create a new project you run

```bash
   FslBuildNew.py <PackageTemplate> <ProjectName>
```

So for example

```bash
   FslBuildNew.py GLES2 SuperGLES2Demo
```

Creates a new SuperGLES2Demo applicaiton based on the GLES2 template.

### Template sanity checking

From 1.8.0 FslBuildNew can now test build its templates to see if they compile.

Run sanity check on the 'Console' template:

```bash
FslBuildNew.py Console * --SanityCheck on
```

Run sanity check on all template:

```bash
FslBuildNew.py * * --SanityCheck on
```

Run sanity check in debug mode on all template:

```bash
FslBuildNew.py * * --SanityCheck debug
```

Debug mode leaves all 'genrated' projects behind so they can be examined manually.
Beware that repeated runs in debug mode skips existing sanity check dirs.

### Templates

The templates are currently located at `.Config/Templates.gen/NewProject` the first folder name specify the template language 'C++' inside
that directory all the supported templates are stored. Each of these folders that contain a 'Template.xml' file is considered to be a valid template.

Filename     | Description
-------------|--------------------------------
Template.xml | Basic template configuration
Modify       | A optional directory that will be recursively scanned for files. Any files found under it it processed and copied to the destination directory.
Copy         | A optional directory that will be recursively scanned for files. Any files found are copied to the destination directory.

All processed files (those under modify) will have their magic `##VARIABLES##` patched with proper values before they are written to the destination directory.

### Note

The processing engine for the template 'Modify' and 'Copy' is the same engine used to generate the visual studio projects and android-gradle-cmake projects.

## FslBuildRun.py

A simple way to build and run a sample, its basically a simplified version of ```FslBuild.py --ForAll (EXE)``` command.
All parameters after '--' gets send directly to the exectuable.
It also has a experimental s

### FslBuildRun - Useful arguments

FslBuild comes with a few useful arguments

Argument           | Description
-------------------|------------------------
--standalone       | Allow the tool to run without the SDK present. This is a experimental option.
--RequireFeatures  | The list of features that are required for a executable package to be build. For example [OpenGLES2] to build all executables that use OpenGLES2.
--UseFeatures      | Allows you to limit whats build based on a provided feature list. For example [EGL,OpenGLES2]. This parameter defaults to all features.
--UseExtensions    | The list of available extensions to build for. For example [OpenGLES3.1:EXT_geometry_shader,OpenGLES3.1:EXT_tessellation_shader] to allow the OpenGLES3.1 extensions EXT_geometry_shader and EXT_tessellation_shader. You can also specify * for all extensions (default).
--Variants         | specify a list of variants and their configuration for example "--Variants [config=Debug,FSL_GLES_NAME=vivante]" or "--Variants [config=Debug,WindowSystem=X11]" This means that we have build system independent way of doing this so it works on all platforms.  
--BuildThreads     | Configures how many threads the builder should use, beware this is a hint that may be ignored by the builder, can be a number or "auto"
--BuildTime        | Time the build and print the result and the end.
--AppInfo          | Run via .AppInfo.json files instead of the packages this parameter can be: file or recursive.
-v                 | Set verbosity level (can be specified multiple times).
--                 | All arguments written after this is send directly to the native build system.

### Experimental standalone test

This tool can be run without the SDK present by running it with the ```--standalone``` parameter.
This will require you to specify the platform using ```-p```.

Scan the current directory and all subdirectories for .AppInfo.json files and run any executable found.

```bash
FslBuildRun.py --standalone -p Yocto --AppInfo recursive -v --Variants [WindowSystem=X11]
```

Scan the current directory and all subdirectories for .AppInfo.json files and run any executable found that requires OpenCL.

```bash
FslBuildRun.py --standalone -p Yocto --AppInfo recursive -v --RequireFeatures [OpenCL] --Variants [WindowSystem=X11]
```

Scan the current directory and all subdirectories for .AppInfo.json files and run any executable that can run with the given feature list.

```bash
FslBuildRun.py --standalone -p Yocto --AppInfo recursive -v --UseFeatures [EGL,OpenGLES2] --Variants [WindowSystem=X11]
```
