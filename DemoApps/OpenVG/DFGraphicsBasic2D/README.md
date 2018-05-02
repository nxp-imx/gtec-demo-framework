<!-- #AG_DEMOAPP_HEADER_BEGIN# -->
# DFGraphicsBasic2D
<img src="./Example.jpg" height="135px" style="float:right">

<!-- #AG_DEMOAPP_HEADER_END# -->
<!-- #AG_BRIEF_BEGIN# -->
Shows how to use the Demo Frameworks 'basic' 2d rendering capabilities that work across all backends.
The basic2D interface allows you to render ASCII strings using a system provided font and draw colored points.

The functionality in Basic2D is used internally in the framework to render the profiling overlays like the frame rate counter and graph.
<!-- #AG_BRIEF_END# -->

<!-- #AG_DEMOAPP_COMMANDLINE_ARGUMENTS_BEGIN# -->

Command line arguments':

Argument                       |Description                                                                                                                                                          |Source
-------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------
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
