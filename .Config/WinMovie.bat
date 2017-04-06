@echo off
del *.bmp > nul
%1 --Window [0,0,1920,1080] --ScreenshotFrequency 2 --ScreenshotNameScheme 1 --ExitAfterFrame 1800 --ForceUpdateTime 16666
if %errorlevel% neq 0 exit /b %errorlevel%
x264 --demuxer lavf --preset veryslow --tune film --fps 30 -o "%1.mkv" "test-screenshot-000000%%04d.bmp"
if %errorlevel% neq 0 exit /b %errorlevel%
del *.bmp > nul
