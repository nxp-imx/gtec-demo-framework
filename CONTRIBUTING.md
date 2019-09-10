# Contributing to the DemoFramework

## Table Of Contents

[How Can I Contribute?](#how-can-i-contribute)

* [Pull Requests](#pull-requests)

[Styleguides](#Styleguides)

* [C++ Styleguide](#c++-styleguide)

[Tools](#Tools)

## How Can I Contribute

### Pull Requests

* Follow the [C++ styleguide](#c++-styleguide) with help from [FslBuildCheck](#preparing-the-code-for-a-pull-request).
* [Check](#check-resource-licenses) that all resources like images, textures and meshes have a attached license.
* Create the appropriate [meta data](#create-the-appropriate-meta-data).

#### Preparing the code for a pull request

Beware "FslBuildCheck --tidy" will most likely not run for yocto builds, it might work for those SDK's that use clang.

1. Ensure you checked in your change locally so you can revert the format changes if need be.
2. Run FslBuildCheck to do a final format of the code.
    ```bash
    FslBuildCheck.py --format --scan --repair
    ```
3. Check in the changes locally when you are ok with them.
4. Time to tidy the code, this is the more invasive part that updates the code.
    ```bash
    FslBuildCheck.py --format --scan --tidy --repair
    ```
5. Keep repeating step 4 until the 'tidy' doesn't fix any more issues.
6. If there were issued reported that couldn't be fixed automatically then fix them manually and then goto step 4.
7. Now we enable some stricter checks which unfortunately are a bit slow to run:
    ```bash
    FslBuildCheck.py --format --scan --tidy --repair --tidyStrictChecks
    ```
   For this its good to be aware that you can run the check on a specific file like this:

    ```bash
    FslBuildCheck.py --format --scan --tidy --repair --tidyStrictChecks --file include/FslBase/IO/Path.hpp
    ```

    Or you can use Unix shell-style wildcards

   ```bash
    FslBuildCheck.py --format --scan --tidy --repair --tidyStrictChecks --file *.hpp
    ```

8. Keep running the checks at step 7 until all issues have been fixed.
9. The code itself is now ready to do a pull request.

#### Check resource licenses

All graphical resources like images, textures, fonts and meshes need to have a license attached.
We utilize a custom ```License.json``` file that covers all resources in the same directory, so if you need to have multiple licenses then you need to put them in separate directories.

The ```FslResourceScan.py``` tool can be run in your 'app' directory to scan for common graphical resource files:

```bash
FslResourceScan.py --list -v .
```

Fix all resource license issues.

#### Create the appropriate meta data

If you are submitting a new demo app you also need to:

1. Create a Example.jpg screenshot
    ```bash
    FslBuildRun.py -- --Window [0,0,1920,1080] --ScreenshotNamePrefix Example --ScreenshotNameScheme exact --ScreenshotFormat jpg --ScreenshotFrequency 5 --ExitAfterFrame 5 --ForceUpdateTime 16666
    ```
   If your app is a console app or the API doesn't support taking screenshots please prepare a relevant screenshot (Example.jpg 1920x1080).
2. Write a description in the README.md
3. Attach the the argument list to the README.md using this command (WARNING: you might want to check-in your README.md before doing this)
    ```bash
    FslBuildDoc.py --ExtractArguments .
    ```
4. Verify that the app README.md file looks fine using a github markdown viewer (for example visual studio code).
5. Verify that the root README.md file contains a nice entry for the app.

## Styleguides

### C++ Styleguide

All C++ code must adhere to format defined by the included ```.clang-format```. For now we utilize the layout provided by clang-format 7.0. It's recommended to utilize a editor that is clang format compatible. Like Visual Studio, Visual Studio Code, Emacs, Vim and lots of others. For more information about clang format check the official [documentation](https://clang.llvm.org/docs/ClangFormat.html).

To further ensure a common style is used in the code we also employ clang-tidy using our ```.clang-tidy``` file.
For more information about clang tidy check the official [documentation](http://clang.llvm.org/extra/clang-tidy/).

#### Manual style recommendations

Before applying all these recommendations manually make sure you take advantage of the automation available using ClangTidy as described [here](preparing-the-pull-request).

* If possible make sure you sample works on Windows and Yocto, testing on all platforms gives bonus points :)
* Avoid platform dependent code when possible.
* All code must have a license attached.
* Use C++14
  * Use [RAII](https://en.cppreference.com/w/cpp/language/raii)
  * Use "enum class" for enums
  * Use 'nullptr' instead of NULL.
  * Make sure you use the C++ header files and not the C ones (the C headers end in .h the C++ ones do not)
  * Use the C++ or 'API' data types that are available on all platforms, like uint32_t and [similar](http://en.cppreference.com/w/cpp/types/integer).
  * Use std::size_t not size_t.
  * Use exceptions for error handling.
    * On modern compilers exceptions are cheaper than error code checking (until the exception occurs).
* Don't use these C++14 features:
  * [chrono](http://en.cppreference.com/w/cpp/chrono)
* Don't use 'using namespace' in a header file!
* Don't use global variables use members instead!
* Don't go overboard with template code.
* Never call exit.
* use the FSLLOG macros for logging, never printf or std::cout.
* As a rule of thumb dont use 'fopen' use the ContentManager or PersistentDataManager instead.
* Keep your functions small. As a rule of thumb keep them under 50 lines.

##### Memory Handling

* Don't use malloc or C style memory handling. Use new and utilize the STL classes as appropriate.
* Use the [RAII](https://en.cppreference.com/w/cpp/language/raii) [smarts pointers](http://en.cppreference.com/w/cpp/memory)
* For dynamically allocated arrays use std::vector instead of managing the memory yourself.

##### API specific

* If you just want to exit on a API error code then use the API checking macros instead of doing your own error checking as they will log more detailed information and throw an appropriate exception.
  * EGL_CHECK, GL_CHECK, RAPIDOPENCL_CHECK, RAPIDOPENVX_CHECK, RAPIDVULKAN_CHECK, etc
* Use the Helper classes as much as possible as they will do the necessary parameter validation and error checking.
  * If your sample is showcasing a specific API call sequence then you might consider not using the helper classes for that sequence.
  
#### FslBuildCheck tool-chain examples

**WARNING:** Before following the below example please ensure you checked in your change locally so you can revert the format changes if need be.

##### FslBuildCheck --format

The FslBuld tool-chain supports invoking clang-format like this:

```bash
FslBuildCheck.py --format --repair
```

To just preview the proposed changes run it like this:

```bash
FslBuildCheck.py --format
```

Additional arguments can be supplied to clang-format like this

```bash
FslBuildCheck.py --format --formatArgs="-h"
```


##### FslBuildCheck --tidy

The FslBuld tool-chain supports invoking clang-tidy like this:

```bash
FslBuildCheck.py --tidy --repair
```

To just preview the proposed changes run it like this:

```bash
FslBuildCheck.py --tidy
```

Additional arguments can be supplied directly to clang-tidy like this

```bash
FslBuildCheck.py --tidy --tidyArgs="-h"
```


If you just want to run a specific set of checks you can do it like this:
```bash
FslBuildCheck.py --tidy -tidyOverrideChecks clang-analyzer-core.*,bugprone-use-after-move
```


It's also possible to run both format and tidy at once

```bash
FslBuildCheck.py --tidy --format --repair
```

When both are invoked the tidy will execute first then the format.

## Tools

These are just recommended third party tools and plugins, but use them at your own risk!

* [Visual Studio Code](https://code.visualstudio.com/) a flexible cross platform editor
  * [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
  * [Clang-Format](https://marketplace.visualstudio.com/items?itemName=xaver.clang-format)
  * [GitLens](https://marketplace.visualstudio.com/items?itemName=eamodio.gitlens)
  * [Docker](https://marketplace.visualstudio.com/items?itemName=PeterJausovec.vscode-docker)
  * [Markdown Preview Github Styling](https://marketplace.visualstudio.com/items?itemName=bierner.markdown-preview-github-styles)
  * [markdownlint](https://marketplace.visualstudio.com/items?itemName=DavidAnson.vscode-markdownlint)
  * [PlantUML](https://marketplace.visualstudio.com/items?itemName=jebbs.plantuml)
  * [Python](https://marketplace.visualstudio.com/items?itemName=ms-python.python)
  * [Visual Studio Keymap](https://marketplace.visualstudio.com/items?itemName=ms-vscode.vs-keybindings)
* [Visual Studio 2017](https://visualstudio.microsoft.com/)
  * [Markdown Editor](https://marketplace.visualstudio.com/items?itemName=MadsKristensen.MarkdownEditor)
  * [Trailing Whitespace Visualizer](https://marketplace.visualstudio.com/items?itemName=MadsKristensen.TrailingWhitespaceVisualizer)
