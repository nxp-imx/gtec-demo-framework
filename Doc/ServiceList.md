# Service list

A list of all known service interfaces and their current providers.

Service interface             | Providers                                                                    | Description
------------------------------|------------------------------------------------------------------------------|--------------------------
IAppInfoService               | AppInfoService                                                               | AppName, IsDebugBuild
IAppInfoControlService        | AppInfoService                                                               | SetAppName
IAsyncImageService            | AsyncImageServiceProxy                                                       | Async image IO
IBitmapConverter              | BitmapConverterService                                                       | Highlevel bitmap and texture format converter.
IContentManager               | ContentManagerService                                                        | Read the readonly app content: bitmaps, fonts, textures, text and files.
IContentMonitor               | ContentMonitorService                                                        | Manage content monitoring
ICpuStatsService              | CpuStatsService                                                              | CPU count, CPU usage, RAM usage.
IDemoAppControl               | DemoAppControlService                                                        | Screenshot, App restart, Exit, time-step mode, Mouse capture, render loop control
IDemoAppControlEx             | DemoAppControlService                                                        | Clear various flags
IDemoPlatformControl          | DemoPlatformControl                                                          | flag clearing, misc
IEventService                 | EventsService                                                                | Register/unregister event listeners
IEventPoster                  | EventsService                                                                | Post events
IGamepads                     | GamepadsService                                                              | Gamepad state
IGraphicsService              | GraphicsService                                                              | Capture screen to bitmap, Basic2D, NativeBatch2D, NativeGraphics, BasicRenderSystem
IGraphicsServiceControl       | GraphicsService                                                              | SetActiveAPI, SetWindowMetrics
IGraphicsServiceHost          | GraphicsService                                                              | Device management.
IHostInfo                     | HostInfoService                                                              | IsConsoleBased, GetActiveApi, GetConfig
IHostInfoControl              | HostInfoService                                                              | Set host info
IImageBasicService            | ImageBasicService                                                            | High level image load/save via the available ImageLibraryServices.
IImageConverterLibraryService | ImageConverterLibraryBasicService, ImageConverterLibraryFP16Service          | Low level image format conversion.
IImageService                 | ImageService                                                                 | Route the image IO request to the current active image loader (basic or async).
IImageServiceControl          | ImageService                                                                 | SetPreferredBitmapOrigin
IImageLibraryService          | ImageLibrarySTBService, ImageLibraryGLIService, ImageLibraryServiceDevIL, ImageLibraryServiceAndroid | Bitmap and texture load/save library
IKeyboard                     | KeyboardService                                                              | Keyboard state
IMouse                        | MouseService|                                                                | Mouse state
INativeWindowEvents           | NativeWindowEventsService                                                    | Register/unregister native window events listeners.
INativeWindowEventSender      | NativeWindowEventsService                                                    | Post native window events.
IOptions                      | OptionsService                                                               | Option parser access.
IOptionsServiceControl        | OptionsService                                                               | AddOptionParser
IPersistentDataManager        | PersistentDataManagerService                                                 | Persistent data IO: bitmap, texture, text, binary.
IProfilerService              | ProfilerService                                                              | Frame time, Custom counters.
IProfilerServiceControl       | ProfilerService                                                              | Add frame times.
ITestService                  | TestService                                                                  | Screenshot frequency, name scheme.
ITextureService               | TextureService                                                               | Mipmap generation
IEGLHostInfo                  | EGLHostService                                                               | GetDisplay, GetSurface, GetContext
IVulkanHostInfo               | VulkanHostService                                                            | GetInstance, GetPhysicalDevice, GetSurfaceKHR, GetLaunchOptions
IWindowHostInfo               | WindowHostService                                                            | GetWindowSystem, GetWindows
IWindowHostInfoControl        | WindowHostService                                                            | Add/remove window
IDemoPlatformControl          | DemoPlatformControl                                                          | RequestExit, HasExitRequest
INativeGraphicsService        | G2D.NativeGraphicsService, OpenGLES2.NativeGraphicsService, OpenGLES3.NativeGraphicsService, OpenVG.NativeGraphicsService, Stub.NativeGraphicsService, Vulkan.NativeGraphicsService | Basic2D, NativeBatch2D, BasicRenderSystem
INativeGraphicsServiceControl | All native graphics services                                                 | Device and resource control
