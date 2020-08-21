call "%WORKSPACE%\.Config\Jenkins\windows\PrepareJenkinsEnvironment.bat"
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)

pushd "%WORKSPACE%\DemoApps"
FslBuildNew.py --SanityCheck debug * * -vv --BuildTime --UseFeatures %FSL_CI_FEATURES% --BuildThreads auto
popd
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)
