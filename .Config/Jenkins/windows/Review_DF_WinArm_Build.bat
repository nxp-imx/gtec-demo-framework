call %WORKSPACE%\.Config\Jenkins\windows\PrepareJenkinsEnvironment.bat
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)

FslBuild.py -t sdk -vv --BuildTime --UseFeatures %FSL_FEATURES%
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)
