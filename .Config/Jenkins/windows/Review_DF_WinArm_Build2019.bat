@echo off
call "%WORKSPACE%\.Config\Jenkins\windows\PrepareJenkinsEnvironment2019.bat"
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)

echo Build start
FslBuild.py --noGitHash --VSVersion 2019 -t sdk -vv --BuildTime --UseFeatures %FSL_CI_FEATURES%
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)
echo Build completed
