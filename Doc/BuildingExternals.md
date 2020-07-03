# Building external dependencies

**The recipes are still experimental!**

Since we no longer ship with precompiled versions of third party libraries we depend upon we introduced a new step in the build process that attempts to fetch and build these dependencies automatically on your machine.

The environment variable ```FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR``` is used to determine which directory is used to build and store these external dependencies. **Beware** that only one SDK installation can use a specific directory.
So if you have multiple installations each require its own third party installation directory.
This is necessary as multiple concurrent builds into the same directory will produce errors.

To help facilitate package reuse you can utilize the experimental [ReadonlyCache](#ReadonlyCache)

The external dependencies are build on a 'as required' basis meaning that we 'fetch & compile' them the first time you try to build something that depends on them.

## Common requirements

* Python 3.4 or better is required by all our scripts (beware using 3.4 requires you to install the 'typing' library for python it comes with 3.5+).
* [CMake](https://cmake.org/) is required by most external source packages.
* Git is required by a few external packages.

## Commands

All existing FslBuild python scripts will automatically invoke the external builder when required. Additionally we introduced a new FslBuildExternal.py script that work directory on the new external build recipes.

### FslBuildExternal.py

* Only builds external packages
* Has various command line options to help develop the external package recipe's.

To build all external packages you can use:

```bash
FslBuildExternal.py -t sdk -v
```

## The technical details

### Paths

The paths below are subject to change.

* All downloads and git clones are done to a cache area where each package must have a unique name. This cache exist at

  ```bash
  $(FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR)/.DownloadCache
  ```

* All pipeline steps are executed under 

  ```bash
  $(FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR)/.Temp/Pipeline/<Platform>/<GeneratorShortName>/<PipelineStep>
  ```

* The final package is installed to 

  ```bash
  $(FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR)/<Platform>/<GeneratorShortName>/<ExperimentalRecipeName>
  ```
  
This setup allows us to

* Re-use a package from the download cache for future rebuilding.
* Delete all pipeline 'steps' in case a error occurs or after the final package has been installed.
* Support multiple platforms in one directory (if need be)
* Support multiple generators in on directory, which for example allows packages for Visual Studio 2015 and 2017 to co-exist.


## Automatic download

The automatic download functionality can be disabled by setting the environment variable ```FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DISABLE_DOWNLOAD``` or 
by changing the the value in the root "Project.gen" file from

```xml 
<Experimental AllowDownloads="true">
```

`To 

```xml 
<Experimental AllowDownloads="false">
```

# Package build recipe

The recipe system is integrated into the Fsl.gen files used by the FslBuild tools.

A recipe defines a:

- optional build pipeline.
- installation section.

The build pipeline is responsible for fetching and building the recipe and 
the installation section is responsible for defining where headers, libs and DLL's can be located and
other optional installation validations steps.

ExperimentalRecipe has basic support for CMake find_package as well so we can get a nice error if something is missing.

| Attribute                | Description                                                                                    |
|--------------------------|------------------------------------------------------------------------------------------------|
| Name                     | The name of the package                                                                        |
| Version                  | The package version (optional)                                                                 |
| Find                     | CMake only: true, false. adds a find_package statement for the given recipe version (optional) |
| FindVersion              | CMake only: the minimum find version. It  must be <= Version. (optional)                       |
| FindTargetName           | CMake only: A optional target name, if not specified we use "Name::Name". (optional)           |
| ExternalInstallDirectory | A environment variable the specifies the external location of the library (optional)           |

```xml
      <ExperimentalRecipe Name="zlib" Version="1.2.11" FindVersion="1.2">
        <Pipeline>
          <Download URL="https://zlib.net/zlib-1.2.11.tar.gz"/>
          <Unpack File="zlib-1.2.11.tar.gz" OutputPath="zlib-1.2.11"/>
          <CMakeBuild Project="" Target="install" Configuration="debug;release" OutputPath="_Install"/>
        </Pipeline>
        <Installation>
          <AddHeaders Name="include"/>
          <AddLib Name="lib/zlib.lib" DebugName="lib/zlibd.lib"/>
          <AddDLL Name="bin/zlib.dll" DebugName="bin/zlibd.dll"/>
          <Path Name="include/zconf.h" Method="IsFile"/>
          <Path Name="include/zlib.h" Method="IsFile"/>
        </Installation>
      </ExperimentalRecipe>
```

## The build pipeline

The basic principle of the pipeline is that each step in it takes a input directory and produces a output directory. 

So if you have step StepA, StepB and StepC like this

```
StepA->StepB->StepC
```

* The output from StepA is used as input to StepB
* The output from StepB is used as input to StepC
* The output from StepC is installed into the 'installation' area

A build recipe consists of a optional Pipeline and a mandatory ValidationInstallation step.

## Pipeline composition

### Fetch

In a build pipeline the very first step will always be a *Fetch* command which can be either:

| Fetch command | Description                                   |
|---------------|-----------------------------------------------|
| Download      | Download a file using the given URL           |
| GitClone      | Clone a repository using the given URL        |
| Source        | Fetch from the local directory of the package |

The result of the fetch command will be stored in a 'cache' area so it can be reused in case of build errors.

### Building

After the fetch step comes 0-n build steps.

| Command       | Description                                             |
|---------------|---------------------------------------------------------|
| Copy          | Copies the content of the input folder                  |
| CMakeAndBuild | Runs CMake and its platform associated builder          |
| Combine       | Runs multiple CMake and its platform associated builder |
| Unpack        | Unpack a specific file (.zip, .tar, .tar.gz, .tgz)      |

Each of these commands also take a optional 'join command list' that is executed before we 
hand over the result to the next step in the pipeline.

| Join command  | Description                                        |
|---------------|----------------------------------------------------|
| Copy          | Copy a file or directory                           |
| Delete        | Delete a file or directory                         |
| GitApply      | Apply a git patch file                             |

## Install section

The installation section is responsible for defining
- where all the important files can be found in a build package.
- some basic installation validation checks.


| Command                  | Description                                                                                                                     |
|--------------------------|---------------------------------------------------------------------------------------------------------------------------------|
| AddHeaders               | Defines where the 'header' files can be found                                                                                   |
| AddLib                   | Define a library file that can be linked against                                                                                |
| AddDLL                   | Define a DLL file that should be copied to the executable output folder                                                         |
| AddTool                  | ToolRecipe only. Define a tool that can be used by the build pipeline                                                           |
| EnvironmentVariable      | Can be used to check if a environment variable is defined                                                                       |
| Path                     | Can be used to check if a path exist.                                                                                           |
| FindFileInPath           | Can be used to check if a file can be found in the current path                                                                 |
| FindExectuableFileInPath | Can be used to check if a executable file can be found in the current path (this is platform aware and will add .exe if needed) |


# Package recipe examples

Examples of existing recipe's can be located here: ```ThirdParty/Recipe```

The recipe's basically come in two variants:

1. Those that deal with a 'pre-installed and pre-compiled' library. These recipes has no Pipeline component.
2. Those that download the source and compile it. These recipes has both a Pipeline and Install component.

## A recipe for a pre-installed library.

The recipe below setups the AmdAppSDK-3.0 by pointing it at the right directory, 
then adds the necessary headers, static libs and DLL's.

```xml
      <ExperimentalRecipe Name="AmdAppSDK-3.0" ExternalInstallDirectory="$(AMDAPPSDKROOT)">
        <Installation>
          <AddHeaders Name="include"/>
          <AddLib Name="lib/x86_64/OpenCL.lib" Help="The main OpenCL static library."/>
          <AddDLL Name="bin/x86_64/OpenCL.dll" Help="This is the OpenCL implementation loaded by the OpenCL library."/>
          <Path Name="include/CL/cl.h" Method="IsFile" Help="The main OpenCL header file."/>
        </Installation>
      </ExperimentalRecipe>
```

## A recipe to download and compile.

The recipe below, downloads the zlib source, unpacks it and the builds it using cmake. It then finally adds its headers, static libs and dll's.

```xml
      <ExperimentalRecipe Name="zlib" Version="1.2.11">
        <Pipeline>
          <Download URL="https://zlib.net/zlib-1.2.11.tar.gz"/>
          <Unpack File="zlib-1.2.11.tar.gz" OutputPath="zlib-1.2.11"/>
          <CMakeBuild Project="" Target="install" Configuration="debug;release" OutputPath="_Install"/>
        </Pipeline>
        <Installation>
          <AddHeaders Name="include"/>
          <AddLib Name="lib/zlib.lib" DebugName="lib/zlibd.lib"/>
          <AddDLL Name="bin/zlib.dll" DebugName="bin/zlibd.dll"/>
          <Path Name="include/zconf.h" Method="IsFile"/>
          <Path Name="include/zlib.h" Method="IsFile"/>
        </Installation>
      </ExperimentalRecipe>
```

# ReadonlyCache

This is a experimental feature intended to help people that utilize multiple SDK installations or run build bots.
Using it will improve build time as it can skip downloading of source.
**Beware** this feature is still experimental!

The environment variable: ```FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR``` can be set to point to a shared caching directory. 
Once you completed a build of the externals to the ```FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR``` directory you can manually copy it to the 
shared cache directory. It is up to YOU to ensure that non concurrency issues occur when you do this copy so make sure that no builds 
are running in the background!

All scripts will only read from the directory so no concurrency issues should exist.

For now we only support reusing the .DownloadCache directory, but future updates will ensure that any already build package will
be re-used too.

## Populate the cache

The simplest way to get the files for the cache is to run 

```bash
FslBuildExternal.py -t sdk -v
```

This will download and build all externals to the FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR directory. 
Once that is done the files can be manually copied to the Readonly cache area. Just make sure nothing is using any of the 
two directories when you do the copy, this is especially important for build bots etc.

# Appendix: Recipe pipeline commands

## Pipeline fetch commands

### Download

Download the specified URL.

| Attribute  | Description                                                                                                            |
|------------|------------------------------------------------------------------------------------------------------------------------|
| URL        | The url to download                                                                                                    |
| To         | [Optional] Specifies where to store it under the output path                                                           |
| Hash       | [Optional] A SHA256 hash of the file, if this is specified the downloaded file will be validated against it.           |
| OutputPath | [Optional] Once command has been completely executed, the path specified here will be considered the final output path |


### GitClone

Git clone the specified URL.

| Attribute  | Description                                                                                                            |
|------------|------------------------------------------------------------------------------------------------------------------------|
| URL        | The url to download                                                                                                    |
| Tag        | [Optional] If specified this specific branch/tag will be cloned                                                        |
| Hash       | [Optional] The repo hash, if this is specified the cloned repo will be validated against it.                           |
| OutputPath | [Optional] Once command has been completely executed, the path specified here will be considered the final output path |

## Pipeline build commands

### Clone

Copy the content of the input path.

| Attribute  | Description                                                                                                            |
|------------|------------------------------------------------------------------------------------------------------------------------|
| OutputPath | [Optional] Once command has been completely executed, the path specified here will be considered the final output path |


### CMakeAndBuild

| Attribute     | Description                                                                                                                     |
|---------------|---------------------------------------------------------------------------------------------------------------------------------|
| Project       | The name of the project as specified in the CMakeLists                                                                          |
| Target        | 'project' or 'install'. If project is specified everything will be build, if install is specified we will try do a cmake install|
| Configuration | A list of configurations to build. We support 'debug' and 'release'. Both can be specified like this 'debug;release'.           |
| Options       | [Optional] Custom CMake options to utilize.                                                                                     |
| OutputPath    | [Optional] Once command has been completely executed, the path specified here will be considered the final output path          |


### Unpack

Unpack the specified file.
A unpack command will often utilize the OutputPath attribute or the Join-Command functionality.

| Attribute  | Description                                                                                                            |
|------------|------------------------------------------------------------------------------------------------------------------------|
| File       | The specified file from the input path is unpacked to the output path.                                                 |
| OutputPath | [Optional] Once command has been completely executed, the path specified here will be considered the final output path |


## Pipeline build join commands

### Copy

Copy a file from input to output.

| Attribute | Description                                                                    |
|-----------|--------------------------------------------------------------------------------|
| From      | The file to copy from the input path.                                          |
| To        | The output path to copy it to.                                                 |


### GitApply

Apply a git patch file.

| Attribute | Description                                                  |
|-----------|--------------------------------------------------------------|
| From      | The name of the file patch file to use from the input path.  |

# Appendix: Install commands

### AddHeaders

Defines where the 'header' files can be found relative to the package installation path. 
The header file location under the installation path will be automatically added as a ExternalDependency 
just as if the Fsl.gen file had defined it manually.

To validate the command it will be checked if the path represents a directory under the installation path.

| Attribute | Description                                                                        |
|-----------|------------------------------------------------------------------------------------|
| Name      | The path to header files relative to the package installation path.                |
| Help      | [Optional] A additional help string that is shown if the command fails validation. |


### AddLib
Define the path to a library file that can be linked against. The path is relative to the package installation path.
The lib under the installation path will be automatically added as a ExternalDependency 
just as if the Fsl.gen file had defined it manually.

To validate the command it will be checked if the path represents a file under the installation path.

| Attribute | Description                                                                                                                 |
|-----------|-----------------------------------------------------------------------------------------------------------------------------|
| Name      | The path to a lib file relative to the package installation path.                                                           |
| DebugName | [Optional] The path to a debug lib file relative to the package installation path, this file will be used for debug builds. |
| Help      | [Optional] A additional help string that is shown if the command fails validation.                                          |


### AddDLL
Define a DLL file that should be copied to the executable output folder. 
The lib under the installation path will be automatically added as a ExternalDependency 
just as if the Fsl.gen file had defined it manually.

To validate the command it will be checked if the path represents a file under the installation path.

| Attribute | Description                                                                                                                 |
|-----------|-----------------------------------------------------------------------------------------------------------------------------|
| Name      | The path to a DLL file relative to the package installation path.                                                           |
| DebugName | [Optional] The path to a debug DLL file relative to the package installation path, this file will be used for debug builds. |
| Help      | [Optional] A additional help string that is shown if the command fails validation.                                          |


### EnvironmentVariable

Can be used to check if a environment variable is defined.

| Attribute     | Description                                                                                                           |
|---------------|-----------------------------------------------------------------------------------------------------------------------|
| Name          | The name of an environment variable.                                                                                  |
| Method        | The method to use for validation of the content: IsDirectory, IsFile, Exists                                          |
| AllowEndSlash | [Optional] Bool to determine if the path contained in the environment variable can end in a slash (defaults to false) |
| Help          | [Optional] A additional help string that is shown if the command fails validation.                                    |


### Path

Can be used to check if a path exist.

| Attribute     | Description                                                                        |
|---------------|------------------------------------------------------------------------------------|
| Name          | The path relative to the installation path.                                        |
| Method        | The method to use for validation of the content: IsDirectory, IsFile, Exists       |
| Help          | [Optional] A additional help string that is shown if the command fails validation. |


### FindFileInPath

Can be used to check if a file can be found in the current path.

| Attribute    | Description                                                                                                         |
|--------------|---------------------------------------------------------------------------------------------------------------------|
| Name         | The name of an environment variable.                                                                                |
| ExpectedPath | [Optional] The expected path to find it under, relative to the installation path. We just check if its in the path. |
| Help         | [Optional] A additional help string that is shown if the command fails validation.                                  |



# Appendix: Path commands

Per default all path commands in the pipeline read from the input path and write to the output path.
However most paths can be overidden by using one of the predefined variables or even a environment variable. 
Beware that the use of environment variables is not recommended.

Variables are indicated with $\{NAME\} and environment variables $(NAME).

## Input path modifiers

All paths that defaults to use the input path, can use these:

| Variable    | Description                                                  |
|-------------|--------------------------------------------------------------|
| RECIPE_PATH | The path where the package build recipe is stored.           |
| SRC_PATH    | The original input path.                                     |
| DST_PATH    | The original output path.                                    |
| OUTPUT_PATH | The final output path.                                       |


## Output path modifiers

All paths that defaults to use the output path, can use these:

| Variable    | Description                                                  |
|-------------|--------------------------------------------------------------|
| DST_PATH    | The original output path.                                    |
| OUTPUT_PATH | The final output path.                                       |


# Manual download/compile of external package

1. Look in the recipe for the package and look at the name and version of the ExperimentalRecipe. In the example below its 'zlib' and '1.2.11' which gets resolved to 'zlib-1.2.11'. 

  ```xml
      <ExperimentalRecipe Name="zlib" Version="1.2.11">
  ```
2. Build the library with the required toolchain.
3. Put the final files into the 'named' directory, for zlib it was 'zlib-1.2.11'
4. Make sure that the various files mentioned in the recipe exist at the correct locations.
5. Add a empty 'BuildOverride' file inside the directory to indicate that the library should be considered valid.
