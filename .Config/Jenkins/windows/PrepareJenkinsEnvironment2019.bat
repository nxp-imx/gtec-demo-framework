set FSL_GRAPHICS_SDK=%WORKSPACE%
PATH=%path%;%FSL_GRAPHICS_SDK%\.Config;
echo FSL_MSVC_ENV_SETUP=%FSL_MSVC2019_ENV_SETUP%
call "%FSL_MSVC2019_ENV_SETUP%/vcvarsall.bat" amd64
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)

if not defined FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR (
  if defined FSL_JENKINS_THIRD_PARTY_INSTALL (
    set FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR=%FSL_JENKINS_THIRD_PARTY_INSTALL%\work\%EXECUTOR_NUMBER%
  ) else (
    echo FSL_JENKINS_THIRD_PARTY_INSTALL not configured, using defalt settings for FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR.
  )
)

if defined FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR_OVERRIDE (
  if defined FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR (
    echo FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR_OVERRIDE override FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR=%FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR%
  ) else (
    echo FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR_OVERRIDE used as FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR
  )
  set FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR=%FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR_OVERRIDE%
)

if not defined FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR (
  if defined FSL_JENKINS_THIRD_PARTY_INSTALL (
    set FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR=%FSL_JENKINS_THIRD_PARTY_INSTALL%\ro-cache
  ) else (
    echo FSL_JENKINS_THIRD_PARTY_INSTALL not configured, using defalt settings for FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR.
  )
) 

echo - FSL_JENKINS_THIRD_PARTY_INSTALL as %FSL_JENKINS_THIRD_PARTY_INSTALL%
echo - FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR as %FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR%
echo - FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR as %FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_READONLY_CACHE_DIR%

echo CWD: %cd%

call .Config\ConfigureOpenGLESEmu.bat
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)

call prepare.bat
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)

rem Since a specific executor can run build multiple version of the %JOB_NAME% and there is no way 
rem to get information about that to create a unique directory name based on 'JOB_NAME' 'EXECUTOR_NUMBER' and this 'magic workspace #'
rem we force claim the install area instead bypassing a security check
FslBuildExternal.py --noGitHash --ForceClaimInstallArea --VoidBuild
if %errorlevel% neq 0 (
  exit /b %errorlevel%
)

if not defined FSL_CI_TEST_REPORTS (
  set FSL_CI_TEST_REPORTS=%WORKSPACE%/.Reports/UnitTests
)

if not defined FSL_CI_FEATURES (
set FSL_CI_FEATURES=[ConsoleHost,WindowHost,Test_RequireUserInputToExit,EarlyAccess,EGL,G2D,OpenCL1.2,GoogleUnitTest,OpenCV,OpenCV4,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVG,OpenVX,OpenVX1.2,Vulkan]
)

