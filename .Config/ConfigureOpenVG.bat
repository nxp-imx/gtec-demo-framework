@echo off
if [%1]==[] goto BACKEND_AUTO_SELECT
if /i %1==ref goto BACKEND_REF

:BACKEND_AUTO_SELECT
  goto BACKEND_REF

:BACKEND_REF
  call "%FSL_GRAPHICS_SDK%\.Config\Configure\ConfigureOpenVG_Reference.bat"
  goto EXIT

:EXIT
  echo OpenVG emulation: %FSL_GLES_NAME%
