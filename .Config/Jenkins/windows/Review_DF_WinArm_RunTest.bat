call %WORKSPACE%\.Config\Jenkins\windows\PrepareJenkinsEnvironment.bat
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)

FslBuild.py -t sdk --UseFeatures %FSL_FEATURES% --RequireFeature [GoogleUnitTest] --ForAllExe "(EXE) --gtest_output=xml:""%FSL_TEST_REPORTS%/(PACKAGE_NAME).xml"""
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)
