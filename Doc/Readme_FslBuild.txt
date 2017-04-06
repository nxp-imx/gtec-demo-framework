FslBuildGen.py
Is a cross-platform build-file generator. Which main purpose is to keep all 
build files consistent, in sync and up to date. See FslBuildGen.docx for 
details.


FslBuild.py
Extends the technology behind FslBuildGen with additional knowledge about how
to execute the build system for a given platform.

So basically FslBuild works like this
1. Invoke the build-file generator that updates all build files if necessary.
2. Filter the builds request based on the provided feature list.
3. Build all necessary build files in the correct order.

Useful arguments:
FslBuild comes with a few useful arguments
  --ListFeatures = List all features required by the build
  --UseFeatures  = Allows you to limit whats build based on a provided feature 
                   list. For example [EGL,OpenGLES2]. This parameter defaults 
                   to all features.
  --ListVariants = List all build variants and their options used by the build.
  --Variants     = specify a list of variants and their configuration 
                   for example 
                   "--Variants [config=Debug,FSL_GLES_NAME=vivante]" or 
                   "--Variants [config=Debug,WindowSystem=X11]"
                   This means that we have build system independent way of 
                   doing this so it works on all platforms.  
  --BuildTime    = Time the build and print the result and the end.
  -t 'sdk'       = Build all demo framework projects
  -v             = Set verbosity level (can be specified multiple times).
  --             = All arguments written after this is send directly to the 
                   native build system.

Important notes:
-	Don’t modify the auto-generated files.
  The FslBuild scripts are responsible for creating all the build files for a 
  platform and verifying dependencies. Since all build files are auto generated
  you can never modify them directly as the next build will overwrite your 
  changes. Instead add your changes to the ‘Fsl.gen’ files as they control the
  build file generation!
-	The ‘Fsl.gen’ file is the real build file.
-	All include and source files in the respective folders are automatically 
  added to the build files.


Build system per platform:
  Android = ant
  Qnx     = make
  Ubuntu  = make
  Windows = Visual studio (IDE or nmake)
  Yocto   = make


Known issues:
- Android has experimental support.

