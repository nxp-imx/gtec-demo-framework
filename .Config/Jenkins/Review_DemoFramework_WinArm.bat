set FSL_GRAPHICS_SDK=%WORKSPACE%
PATH=%path%;%FSL_GRAPHICS_SDK%\.Config;
call vcvarsall.bat amd64
call prepare.bat

FslBuild.py -t sdk -vv --BuildTime --UseFeatures [EGL,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVG,OpenCL,OpenCL1.1,OpenCL1.2,OpenVX,OpenVX1.0.1,OpenVX1.1,G2D,Vulkan,EarlyAccess] -- /m
