@echo off

if not defined FSL_CI_FEATURES (
set FSL_CI_FEATURES=[EarlyAccess,EGL,G2D,OpenCL1.2,GoogleUnitTest,OpenCV,OpenCV4,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVG,OpenVX,OpenVX1.2]
)

call "%WORKSPACE%\.Config\Jenkins\windows\PrepareJenkinsEnvironment2019.bat"
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)

echo Test start with reports at '%FSL_CI_TEST_REPORTS%/(PACKAGE_NAME).xml'
FslBuild.py --noGitHash --VSVersion 2019 -t sdk --UseFeatures %FSL_CI_FEATURES% --RequireFeature [GoogleUnitTest] --ForAllExe "(EXE) --gtest_output=xml:""%FSL_CI_TEST_REPORTS%/(PACKAGE_NAME).xml"""
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)
echo Test completed
