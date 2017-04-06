set FSL_GRAPHICS_SDK=%WORKSPACE%
PATH=%path%;%FSL_GRAPHICS_SDK%\.Config;
call vcvarsall.bat amd64
call prepare.bat
FslBuild.py -t sdk -v --BuildTime --UseFeatures [EGL,OpenGLES2,OpenGLES3,OpenVG] -- /m
