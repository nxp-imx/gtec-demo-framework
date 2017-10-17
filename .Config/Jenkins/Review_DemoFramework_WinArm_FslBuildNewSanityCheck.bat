call %WORKSPACE%\.Config\Jenkins\PrepareJenkinsEnvironment.bat

pushd %WORKSPACE%\DemoApps
FslBuildNew.py --SanityCheck debug * * -vv --BuildTime --UseFeatures [EGL,EarlyAccess,G2D,OpenCL,OpenCL1.1,OpenCL1.2,OpenCV,OpenCV2_4,OpenCV3,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVG,OpenVX,OpenVX1.0.1,OpenVX1.1,Vulkan] --BuildThreads auto
popd
