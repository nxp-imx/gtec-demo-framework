@echo off
%1 --Window [0,0,1920,1080] --ScreenshotFrequency 5 --ExitAfterFrame 5 --ForceUpdateTime 16666
if %errorlevel% neq 0 exit /b %errorlevel%
FslImageConvert test-frame-0000000005.bmp Example
del test-frame-0000000005.bmp > nul
