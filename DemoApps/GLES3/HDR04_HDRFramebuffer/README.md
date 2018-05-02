<!-- #AG_DEMOAPP_HEADER_BEGIN# -->
# HDR04_HDRFramebuffer
<img src="./Example.jpg" height="135px" style="float:right">

<!-- #AG_DEMOAPP_HEADER_END# -->
<!-- #AG_BRIEF_BEGIN# -->
Demonstrates how to enable HDRFramebuffer mode if available.
The render a test scene using a pattern that makes it easy to detect if the display actually enabled HDR mode.

This sample outputs to a HDR screen if supported.
<!-- #AG_BRIEF_END# -->

This is a work in progress sample as the currently used tonemapper isn't suited for HDR displays.

Inspired by:
- [Implementing HDR in ‘Rise of the Tomb Raider’](https://developer.nvidia.com/implementing-hdr-rise-tomb-raider)
- [High Dynamic Range Display Development](https://developer.nvidia.com/high-dynamic-range-display-development)
- [Advanced Techniques and Optimization of HDR Color Pipelines](https://gpuopen.com/gdc16-wrapup-presentations/)
- [VDR Follow Up – Tonemapping for HDR Signals](https://gpuopen.com/vdr-follow-up-tonemapping-for-hdr-signals/)
- [HDR Programming](http://on-demand.gputechconf.com/siggraph/2016/presentation/sig1611-thomas-true-high-dynamic-range-rendering-displays.pdf)

The app can be controlled using:

Key          | Result
-------------|----------------------------------
1            | Fullscreen linear rendering
2            | Fullscreen tonemapped rendering
3            | Split-screen rendering (default)
Q            | Decrease exposure
E            | Increase exposure
R            | Reset exposure
P            | Toggle HDR debug pattern on/off
A            | Strafe left
W            | Strafe right
S            | Move forward
D            | Move backward
Right Mouse  | Mouse look (hold it)
Middle mouse | Reset camera

<!-- #AG_DEMOAPP_COMMANDLINE_ARGUMENTS_BEGIN# -->

Command line arguments':

Argument                       |Description                                                                                                                                                          |Source
-------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------
--DisableDisplayHDRCheck       |Disables the check that verifies that the display is HDR compatible                                                                                                  |Demo
--DisablePattern               |Disable the debug pattern                                                                                                                                            |Demo
-s, --Scene \<arg>             |Select the scene to run (0 to 2)                                                                                                                                     |Demo
--DisplayId \<arg>             |DisplayId \<number>                                                                                                                                                  |DemoHost
--EGLAlphaSize \<arg>          |Force EGL_ALPHA_SIZE to the given value                                                                                                                              |DemoHost
--EGLBlueSize \<arg>           |Force EGL_BLUE_SIZE to the given value                                                                                                                               |DemoHost
--EGLDepthSize \<arg>          |Force EGL_DEPTH_SIZE to the given value                                                                                                                              |DemoHost
--EGLGreenSize \<arg>          |Force EGL_GREEN_SIZE to the given value                                                                                                                              |DemoHost
--EGLLogConfig                 |Output the EGL config to the log                                                                                                                                     |DemoHost
--EGLLogConfigs \<arg>         |Output the supported configurations to the log. 0=Off, 1=All, 2=HDR. Don't confuse this with LogConfig.                                                              |DemoHost
--EGLLogExtensions             |Output the EGL extensions to the log                                                                                                                                 |DemoHost
--EGLRedSize \<arg>            |Force EGL_RED_SIZE to the given value                                                                                                                                |DemoHost
--Window \<arg>                |Window mode [left,top,width,height]                                                                                                                                  |DemoHost
--AppFirewall                  |Enable the app firewall, reporting crashes on-screen instead of exiting                                                                                              |DemoHostManager
--ContentMonitor               |Monitor the Content directory for changes and restart the app on changes. WARNING: Might not work on all platforms and it might impact app performance (experimental)|DemoHostManager
--ExitAfterDuration \<arg>     |Exit after the given duration has passed. The value can be specified in seconds or milliseconds. For example 10s or 10ms.                                            |DemoHostManager
--ExitAfterFrame \<arg>        |Exit after the given number of frames has been rendered                                                                                                              |DemoHostManager
--ForceUpdateTime \<arg>       |Force the update time to be the given value in microseconds (can be useful when taking a lot of screen-shots). If 0 this option is disabled                          |DemoHostManager
--LogStats                     |Log basic rendering stats (this is equal to setting LogStatsMode to latest)                                                                                          |DemoHostManager
--LogStatsMode \<arg>          |Set the log stats mode, more advanced version of LogStats. Can be disabled, latest, average                                                                          |DemoHostManager
--ScreenshotFormat \<arg>      |Chose the format for the screenshot: bmp, jpg, png or tga (defaults to png)                                                                                          |DemoHostManager
--ScreenshotFrequency \<arg>   |Create a screenshot at the given frame frequency                                                                                                                     |DemoHostManager
--ScreenshotNamePrefix \<arg>  |Chose the screenshot name prefix (defaults to 'Screenshot')                                                                                                          |DemoHostManager
--ScreenshotNameScheme \<arg>  |Chose the screenshot name scheme: frame, sequence or exact (defaults to frame)                                                                                       |DemoHostManager
--Stats                        |Display basic frame profiling stats                                                                                                                                  |DemoHostManager
--Profiler.AverageEntries \<arg>|The number of frames used to calculate the average frame-time. Defaults to: 60                                                                                       |ProfilerService
--ghelp \<arg>                 |Display option groups: all, demo or host                                                                                                                             |base
-h, --help                     |Display options                                                                                                                                                      |base
-v, --verbose                  |Enable verbose output                                                                                                                                                |base
<!-- #AG_DEMOAPP_COMMANDLINE_ARGUMENTS_END# -->
