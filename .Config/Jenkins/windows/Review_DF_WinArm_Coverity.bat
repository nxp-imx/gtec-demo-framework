@echo off

echo ************************************
echo *** Preparing for Coverity build ***
echo ************************************

call "%WORKSPACE%\.Config\Jenkins\windows\PrepareJenkinsEnvironment.bat"
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)


if not defined FSL_VS_TOOLSET_VERSION (
set FSL_VS_TOOLSET_VERSION=142
echo FSL_VS_TOOLSET_VERSION was not set so using a default value of '%FSL_VS_TOOLSET_VERSION%'
)

echo ******************************
echo *** Coverity build cleanup ***
echo ******************************

if not defined FSL_CI_CMAKEOUT (
  FslBuild.py -t sdk  --debug -vv --BuildTime --set VS_TOOLSET_VERSION=%FSL_VS_TOOLSET_VERSION% --UseFeatures %FSL_CI_FEATURES% -c clean
) else (
  ECHO CMakeBuildDir='%FSL_CI_CMAKEOUT%'
  FslBuild.py -t sdk  --debug -vv --BuildTime --set VS_TOOLSET_VERSION=%FSL_VS_TOOLSET_VERSION% --UseFeatures %FSL_CI_FEATURES% -c clean --CMakeGeneratorName Ninja --CMakeBuildDir %FSL_CI_CMAKEOUT%
)

if %errorlevel% neq 0 (
  exit /b %errorlevel%
)

echo ******************************
echo *** Running Coverity build ***
echo ******************************
set FSL_COVERITY_DIR=c:\data\coverity

if exist "%FSL_COVERITY_DIR%\" (
  echo Removing coverity data directory at '%FSL_COVERITY_DIR%'
  rd /s/q %FSL_COVERITY_DIR%
)

echo FSL_COVERITY_DIR=%FSL_COVERITY_DIR%

cov-configure --msvc
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)
if not defined FSL_CI_CMAKEOUT (
  cov-build --dir %FSL_COVERITY_DIR% cmd.exe /c "FslBuild.py -t sdk --debug -vv --BuildTime --set VS_TOOLSET_VERSION=%FSL_VS_TOOLSET_VERSION% --UseFeatures %FSL_CI_FEATURES% %FSL_CI_BUILD_PARAM%"
) else (
  cov-build --dir %FSL_COVERITY_DIR% cmd.exe /c "FslBuild.py -t sdk --debug -vv --BuildTime --set VS_TOOLSET_VERSION=%FSL_VS_TOOLSET_VERSION% --UseFeatures %FSL_CI_FEATURES% %FSL_CI_BUILD_PARAM% --CMakeGeneratorName Ninja --CMakeBuildDir %FSL_CI_CMAKEOUT%"
)

if %errorlevel% neq 0 (
  exit /b %errorlevel%
)
cov-analyze --dir %FSL_COVERITY_DIR% --all
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)

rem cov-format-errors --dir %FSL_COVERITY_DIR% --html-output coverity.html
rem if %errorlevel% neq 0 (
rem   exit /b %errorlevel%
rem )
