<!-- #AG_DEMOAPP_HEADER_BEGIN# -->
# Blur
<img src="./Example.jpg" height="135px" style="float:right">

<!-- #AG_DEMOAPP_HEADER_END# -->
<!-- #AG_BRIEF_BEGIN# -->
Showcases multiple ways to implement a gaussian blur.
- One pass blur
- Two pass blur
  The 2D Gaussian filter kernel is separable. This allows us two produce the same output as a one pass algorithm by first applying a X-blur and then a Y-blur.
- Two pass linear blur
  Uses the two pass technique and further reduces the bandwidth requirement by taking advantage of the GPU's linear texture filtering
  which allows us to reduce the needed kernel length to roughly half its length while producing the same output as the full kernel length.
- Two pass linear scaled blur
  Uses the two pass linear technique and further reduces the bandwidth requirement by downscaling the 'source image' to 1/4 its size (1/2w x 1/2h) before applying the blur and
  and then upscaling the blurred image to provide the final image. This works well for large kernel sizes and relatively high sigma's but the downscaling produces visible artifacts with low sigma's
<!-- #AG_BRIEF_END# -->

Inspired by:
- http://prideout.net/archive/bloom/
- http://www.sunsetlakesoftware.com/2013/10/21/optimizing-gaussian-blurs-mobile-gpu
- https://software.intel.com/en-us/blogs/2014/07/15/an-investigation-of-fast-real-time-gpu-based-image-blur-algorithms

<!-- #AG_DEMOAPP_COMMANDLINE_ARGUMENTS_BEGIN# -->

Command line arguments':

Argument                       |Description                                                                                                                                                          |Source
-------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------
--AutoSigma                    |Try to use a optimal sigma for the kernel size                                                                                                                       |Demo
--Caption \<arg>               |Set the caption type (0=off,1=Algorithm,2=Description)                                                                                                               |Demo
--Scene \<arg>                 |Select the scene to run (0 to 1)                                                                                                                                     |Demo
-c, --Compare                  |Enable RenderMode compare.                                                                                                                                           |Demo
-d, --Demo                     |Lock into demo mode (nothing can be overridden).                                                                                                                     |Demo
-f, --CRMFlags                 |Compare render mode flags. Render mode 0 is bit 0, render mode1 is bit 1. Defaults to all bits on except for the one pass reference                                  |Demo
-l, --KernelLength \<arg>      |Set the gaussian blur kernel length                                                                                                                                  |Demo
-r, --RenderMode \<arg>        |Select the render mode (0=ReferenceTwoPass, 1=ReferenceTwoPassLinear, 2=TwoPassLinarScaled, 3=ReferenceOnePass).                                                     |Demo
-s, --Sigma \<arg>             |Set the gaussian blur sigma.                                                                                                                                         |Demo
-t, --ShaderType \<arg>        |Set the shader type (0=HardCoded, 1=NonDependent).                                                                                                                   |Demo
--DisplayId \<arg>             |DisplayId \<number>                                                                                                                                                  |DemoHost
--EGLAlphaSize \<arg>          |Force EGL_ALPHA_SIZE to the given value                                                                                                                              |DemoHost
--EGLBlueSize \<arg>           |Force EGL_BLUE_SIZE to the given value                                                                                                                               |DemoHost
--EGLDepthSize \<arg>          |Force EGL_DEPTH_SIZE to the given value                                                                                                                              |DemoHost
--EGLGreenSize \<arg>          |Force EGL_GREEN_SIZE to the given value                                                                                                                              |DemoHost
--EGLLogConfig                 |Output the EGL config to the log                                                                                                                                     |DemoHost
--EGLRedSize \<arg>            |Force EGL_RED_SIZE to the given value                                                                                                                                |DemoHost
--Window \<arg>                |Window mode [left,top,width,height]                                                                                                                                  |DemoHost
--AppFirewall                  |Enable the app firewall, reporting crashes on-screen instead of exiting                                                                                              |DemoHostManager
--ContentMonitor               |Monitor the Content directory for changes and restart the app on changes. WARNING: Might not work on all platforms and it might impact app performance (experimental)|DemoHostManager
--ExitAfterFrame \<arg>        |Exit after the given number of frames has been rendered                                                                                                              |DemoHostManager
--ForceUpdateTime \<arg>       |Force the update time to be the given value in microseconds (can be useful when taking a lot of screen-shots). If 0 this option is disabled                          |DemoHostManager
--LogStats                     |Log basic rendering stats (this is equal to setting LogStatsMode to latest)                                                                                          |DemoHostManager
--LogStatsMode \<arg>          |Set the log stats mode, more advanced version of LogStats. Can be disabled, latest, average                                                                          |DemoHostManager
--ScreenshotFrequency \<arg>   |Create a screenshot at the given frame frequency                                                                                                                     |DemoHostManager
--Stats                        |Display basic frame profiling stats                                                                                                                                  |DemoHostManager
--Profiler.AverageEntries \<arg>|The number of frames used to calculate the average frame-time. Defaults to: 60                                                                                       |ProfilerService
--ghelp \<arg>                 |Display option groups: all, demo or host                                                                                                                             |base
-h, --help                     |Display options                                                                                                                                                      |base
-v, --verbose                  |Enable verbose output                                                                                                                                                |base
<!-- #AG_DEMOAPP_COMMANDLINE_ARGUMENTS_END# -->
