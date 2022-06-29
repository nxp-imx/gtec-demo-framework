@echo off
call "%WORKSPACE%\.Config\Jenkins\windows\PrepareJenkinsEnvironment.bat"
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)

echo Test start with reports at '%FSL_CI_TEST_REPORTS%/(PACKAGE_NAME).xml'
FslBuild.py --noGitHash -t sdk --UseFeatures %FSL_CI_FEATURES% --RequireFeature [GoogleUnitTest] --ForAllExe "(EXE) --gtest_output=xml:""%FSL_CI_TEST_REPORTS%/(PACKAGE_NAME).xml"""
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)
echo Test completed
