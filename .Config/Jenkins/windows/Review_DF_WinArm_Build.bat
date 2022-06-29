@echo off
call "%WORKSPACE%\.Config\Jenkins\windows\PrepareJenkinsEnvironment.bat"
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)

echo Build start
FslBuild.py --noGitHash -t sdk -vv --BuildTime --UseFeatures %FSL_CI_FEATURES%
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)
echo Build completed
