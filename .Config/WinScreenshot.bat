@echo off
%1 --Window [0,0,1920,1080] --ScreenshotNamePrefix Example --ScreenshotNameScheme exact --ScreenshotFormat jpg --ScreenshotFrequency 100 --ExitAfterFrame 100 --ForceUpdateTime 16666
if %errorlevel% neq 0 exit /b %errorlevel%
if exist "Example.jpg" (
  move /Y Example.jpg %2
  if %errorlevel% neq 0 exit /b %errorlevel%
)

