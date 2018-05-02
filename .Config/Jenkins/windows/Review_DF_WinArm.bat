call %WORKSPACE%\.Config\Jenkins\windows\PrepareJenkinsEnvironment.bat
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)

FslBuild.py -t sdk -vv --BuildTime --UseFeatures [EGL,EarlyAccess,G2D,OpenCL,OpenCL1.1,OpenCL1.2,OpenCV,OpenCV3,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVG,OpenVX,OpenVX1.1,Vulkan]
rem FslBuild.py --VSVersion 2017 -t sdk -vv --BuildTime --UseFeatures [EGL,EarlyAccess,G2D,OpenCL,OpenCL1.1,OpenCL1.2,OpenCV,OpenCV3,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVG,OpenVX,OpenVX1.1,Vulkan]
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)
