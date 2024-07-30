<!-- #AG_DEMOAPP_HEADER_BEGIN# -->
# SoftISP
<img src="Example.jpg" height="135px">

<!-- #AG_DEMOAPP_HEADER_END# -->
<!-- #AG_BRIEF_BEGIN# -->
It is a software-based image signal processing(SoftISP) application optimized by GPU. SoftISP --Options
"Enable" Enable high quality noise reduction node
<!-- #AG_BRIEF_END# -->

SoftISP Roles:
 - Bad Pixel Correction
 - White Balance
 - Histogram Equalization
 - High-quality Demosaicing
 - High-quality Noise Reduction

Input Image:
 - Bayer format raw data with color filter array of BG/GR
 - Resolution: 1920x1080

Output Image:
 - "0-SourceImage.bmp": Input image -> Demosaicing
 - "1-RemoveBadPixel.bmp": Input image -> Bad Pixel Correction -> Demosaicing
 - "2-WhiteBalance.bmp": Input image -> Bad Pixel Correction -> White Balance -> Demosaicing
 - "3-Equalization.bmp": Input image -> Bad Pixel Correction -> Histogram Equalization -> Demosaicing
 - "4-RemoveNoise.bmp": Input image -> Bad Pixel Correction -> Histogram Equalization -> Demosaicing -> Noise Reduction

Inspired by:
 - http://ieeexplore.ieee.org/abstract/document/6419046/
 - https://courses.cs.washington.edu/courses/cse467/08au/labs/l5/whiteBalance.pdf
 - https://en.wikipedia.org/wiki/Histogram_equalization
 - http://www.tandfonline.com/doi/abs/10.1080/2151237X.2008.10129267
 - https://en.wikipedia.org/wiki/Bilateral_filter

<!-- #AG_DEMOAPP_COMMANDLINE_ARGUMENTS_BEGIN# -->

Command line arguments':

Argument                        |Description                                                                                                                                                         |Source
--------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------
--Enable                        |Enable denoise function.                                                                                                                                            |Demo
-c, --m_cycleNum \<arg>         |Control the number of cycle                                                                                                                                         |Demo
--AppFirewall                   |Enable the app firewall, reporting crashes on-screen instead of exiting                                                                                             |DemoHostManager
--ContentMonitor                |Monitor the Content directory for changes and restart the app on changes.WARNING: Might not work on all platforms and it might impact app performance (experimental)|DemoHostManager
--ExitAfterDuration \<arg>      |Exit after the given duration has passed. The value can be specified in seconds or milliseconds. For example 10s or 10ms.                                           |DemoHostManager
--ExitAfterFrame \<arg>         |Exit after the given number of frames has been rendered                                                                                                             |DemoHostManager
--ForceUpdateTime \<arg>        |Force the update time to be the given value in microseconds (can be useful when taking a lot of screen-shots). If 0 this option is disabled                         |DemoHostManager
--LogStats                      |Log basic rendering stats (this is equal to setting LogStatsMode to latest)                                                                                         |DemoHostManager
--LogStatsMode \<arg>           |Set the log stats mode, more advanced version of LogStats. Can be disabled, latest, average                                                                         |DemoHostManager
--ScreenshotFormat \<arg>       |Chose the format for the screenshot: bmp, jpg, png (default), tga                                                                                                   |DemoHostManager
--ScreenshotFrequency \<arg>    |Create a screenshot at the given frame frequency                                                                                                                    |DemoHostManager
--ScreenshotNamePrefix \<arg>   |Chose the screenshot name prefix (defaults to 'Screenshot')                                                                                                         |DemoHostManager
--ScreenshotNameScheme \<arg>   |Chose the screenshot name scheme: frame (default), sequence, exact.                                                                                                 |DemoHostManager
--Stats                         |Display basic frame profiling stats                                                                                                                                 |DemoHostManager
--StatsFlags \<arg>             |Select the stats to be displayed/logged. Defaults to frame\|cpu. Can be 'frame', 'cpu' or any combination                                                           |DemoHostManager
--Version                       |Print version information                                                                                                                                           |DemoHostManager
--Profiler.AverageEntries \<arg>|The number of frames used to calculate the average frame-time. Defaults to: 60                                                                                      |ProfilerService
--ghelp \<arg>                  |Display option groups: all, demo or host                                                                                                                            |base
-h, --help                      |Display options                                                                                                                                                     |base
-v, --verbose                   |Enable verbose output                                                                                                                                               |base
<!-- #AG_DEMOAPP_COMMANDLINE_ARGUMENTS_END# -->
