@echo off
%1 --Window [0,0,1920,1080] --ScreenshotNamePrefix Example --ScreenshotNameScheme exact --ScreenshotFormat jpg --ScreenshotFrequency 5 --ExitAfterFrame 5 --ForceUpdateTime 16666
if %errorlevel% neq 0 exit /b %errorlevel%
