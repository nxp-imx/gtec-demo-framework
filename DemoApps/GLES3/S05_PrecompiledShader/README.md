<!-- #AG_DEMOAPP_HEADER_BEGIN# -->
# S05_PrecompiledShader
<img src="./Example.jpg" height="135px" style="float:right">

<!-- #AG_DEMOAPP_HEADER_END# -->
<!-- #AG_BRIEF_BEGIN# -->
Demonstrates how to use a pre-compiled shader using the offline compiler tool 'vCompiler' from Verisilicon.

This currently only works on the Yocto platform.
<!-- #AG_BRIEF_END# -->

The tool can create either a shader program binary file, that only requires to call the function:

```C++
GL_CHECK(glProgramBinaryOES(hProgram, GL_PROGRAM_BINARY_VIV, buf, length));
```

Or a set of pre-compiled shaders that will save you the compile stage, but need to be linked into a final program.
Both options are supported in this sample.

--Options
-a, --SeparateShader : if -a or --Separate shader is passed as argument when launching the application, it will use the pre-compiled shaders, if not, it will use the pre-compiled program binary.

<!-- #AG_DEMOAPP_COMMANDLINE_ARGUMENTS_BEGIN# -->
<!-- #AG_DEMOAPP_COMMANDLINE_ARGUMENTS_END# -->
