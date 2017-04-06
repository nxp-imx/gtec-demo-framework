@echo off
if defined FSL_GLES_NAME goto EXIT
if [%1]==[] goto BACKEND_AUTO_SELECT
if /i %1==arm goto BACKEND_ARM
if /i %1==powervr goto BACKEND_POWERVR
if /i %1==powervr_frame goto BACKEND_POWERVR_FRAME
if /i %1==powervr_trace goto BACKEND_POWERVR_TRACE
if /i %1==qualcomm goto BACKEND_QUALCOMM
if /i %1==vivante goto BACKEND_VIVANTE

:BACKEND_AUTO_SELECT
  goto BACKEND_ARM

:BACKEND_ARM
  call %FSL_GRAPHICS_SDK%\.Config\Configure\ConfigureOpenGLES_Arm.bat
  goto EXIT

:BACKEND_POWERVR
  call %FSL_GRAPHICS_SDK%\.Config\Configure\ConfigureOpenGLES_PowerVR.bat
  goto EXIT

:BACKEND_POWERVR_FRAME
  call %FSL_GRAPHICS_SDK%\.Config\Configure\ConfigureOpenGLES_PowerVR_Frame.bat
  goto EXIT
  
:BACKEND_POWERVR_TRACE
  call %FSL_GRAPHICS_SDK%\.Config\Configure\ConfigureOpenGLES_PowerVR_Trace.bat
  goto EXIT

:BACKEND_QUALCOMM
  call %FSL_GRAPHICS_SDK%\.Config\Configure\ConfigureOpenGLES_Qualcomm.bat
  goto EXIT

:BACKEND_VIVANTE
  call %FSL_GRAPHICS_SDK%\.Config\Configure\ConfigureOpenGLES_Vivante.bat
  goto EXIT

:EXIT
