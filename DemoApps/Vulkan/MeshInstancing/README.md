<!-- #AG_DEMOAPP_HEADER_BEGIN# -->
# MeshInstancing
<img src="./Example.jpg" height="135px" style="float:right">

<!-- #AG_DEMOAPP_HEADER_END# -->
<!-- #AG_BRIEF_BEGIN# -->
Shows the use of instancing for rendering many copies of the same mesh using different attributes and textures.
A secondary vertex buffer containing instanced data, stored in device local memory,
is used to pass instance data to the shader via vertex attributes with a per-instance step rate.
The instance data also contains a texture layer index for having different textures for the instanced meshes.

Based on a example called [Mesh instancing](https://github.com/SaschaWillems/Vulkan) by Sascha Willems.
Recreated as a DemoFramework freestyle window sample in 2016.
<!-- #AG_BRIEF_END# -->

<!-- #AG_DEMOAPP_COMMANDLINE_ARGUMENTS_BEGIN# -->

Command line arguments':

Argument                       |Description                                                                                                                                                          |Source
-------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------
--InstanceCount \<arg>         |Set the instance count directly.                                                                                                                                     |Demo
--Preset \<arg>                |Set the instance count preset (low,medium,high,ultra)                                                                                                                |Demo
--VkPhysicalDevice \<arg>      |Set the physical device index.                                                                                                                                       |Demo
--VkValidate \<arg>            |Enable/disable the VK_LAYER_LUNARG_standard_validation layer.                                                                                                        |Demo
--DisplayId \<arg>             |DisplayId \<number>                                                                                                                                                  |DemoHost
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
