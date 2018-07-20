call %WORKSPACE%\.Config\Jenkins\windows\PrepareJenkinsEnvironment.bat
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)

FslBuild.py -t sdk -vv --BuildTime --UseFeatures %FSL_FEATURES% --ForAllExe "FslContentSync.py (CONTENT_PATH) %FSL_GRAPHICS_SDK%/bin/(PACKAGE_NAME) --ifExist --create -v -i --version" --BuildThreads auto
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)
