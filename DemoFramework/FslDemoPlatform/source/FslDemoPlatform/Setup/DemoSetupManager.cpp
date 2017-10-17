/****************************************************************************************************************************************************
* Copyright (c) 2014 Freescale Semiconductor, Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*    * Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*
*    * Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*
*    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
*      its contributors may be used to endorse or promote products derived from
*      this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
****************************************************************************************************************************************************/

#include <FslDemoPlatform/Setup/DemoSetupManager.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslDemoApp/Base/Setup/HostDemoAppSetup.hpp>
#include <FslDemoPlatform/Setup/DemoHostAppSetupBuilder.hpp>
#include <FslDemoPlatform/Setup/DemoHostRegistry.hpp>
#include <FslDemoHost/Base/Service/ServicePriorityList.hpp>
#include <FslDemoHost/Base/Service/ServiceGroupName.hpp>
#include <FslDemoHost/Base/Service/AsyncImage/AsyncImageServiceImpl.hpp>
#include <FslDemoHost/Base/Service/AsyncImage/AsyncImageServiceProxy.hpp>
#include <FslDemoHost/Base/Service/BitmapConverter/BitmapConverterService.hpp>
#include <FslDemoHost/Base/Service/Content/ContentManagerServiceFactory.hpp>
#include <FslDemoHost/Base/Service/ContentMonitor/ContentMonitorService.hpp>
#include <FslDemoHost/Base/Service/DemoAppControl/DemoAppControlServiceFactory.hpp>
#include <FslDemoHost/Base/Service/Events/EventsService.hpp>
#include <FslDemoHost/Base/Service/Gamepad/GamepadsService.hpp>
#include <FslDemoHost/Base/Service/Graphics/GraphicsService.hpp>
#include <FslDemoHost/Base/Service/Host/HostInfoService.hpp>
#include <FslDemoHost/Base/Service/Image/ImageService.hpp>
#include <FslDemoHost/Base/Service/ImageBasic/ImageBasicService.hpp>
#include <FslDemoHost/Base/Service/Keyboard/KeyboardService.hpp>
#include <FslDemoHost/Base/Service/Mouse/MouseService.hpp>
#include <FslDemoHost/Base/Service/NativeWindowEvents/NativeWindowEventsService.hpp>
#include <FslDemoHost/Base/Service/Options/OptionsService.hpp>
#include <FslDemoHost/Base/Service/Profiler/ProfilerService.hpp>
#include <FslDemoHost/Base/Service/Profiler/ProfilerServiceFactory.hpp>
#include <FslDemoHost/Base/Service/Persistent/PersistentDataManagerServiceFactory.hpp>
#include <FslDemoHost/Base/Service/Test/TestService.hpp>
#include <FslDemoPlatform/Service/DemoPlatformControl/DemoPlatformControl.hpp>
#include <FslService/Impl/ServiceOptionParserDeque.hpp>
#include <FslService/Impl/Registry/ServiceRegistry.hpp>
#include <FslService/Impl/ServiceType/Async/AsynchronousServiceFactory.hpp>
#include <FslService/Impl/ServiceType/Async/AsynchronousServiceImplFactoryTemplate.hpp>
#include <FslService/Impl/ServiceType/Async/AsynchronousServiceProxyFactoryTemplate.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalSingletonServiceFactoryTemplate.hpp>
#include "../Configuration/PlatformConfig.hpp"
#include <FslDemoApp/Base/Setup/RegisterDemoApp_fwd.hpp>
#include <cassert>
//#include <FslDemoPlatform/Service/MMDCStats/MMDCStatsService.hpp>

#ifdef FSL_FEATURE_GLI
#include <FslDemoPlatform/Service/ImageLibrary/ImageLibraryGLIService.hpp>
#endif
#ifdef FSL_FEATURE_STB
#include <FslDemoPlatform/Service/ImageLibrary/ImageLibrarySTBService.hpp>
#endif

namespace Fsl
{
  typedef AsynchronousServiceProxyFactoryTemplate<AsyncImageServiceProxy, IAsyncImageService> AsyncImageServiceProxyFactory;
  typedef AsynchronousServiceImplFactoryTemplate<AsyncImageServiceImpl> AsyncImageServiceImplFactory;

  typedef ThreadLocalSingletonServiceFactoryTemplate<BitmapConverterService, IBitmapConverter> BitmapConverterServiceFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate<GamepadsService, IGamepads> GamepadsFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate2<ImageService, IImageService, IImageServiceControl> ImageServiceFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate<ImageBasicService, IImageBasicService> ImageBasicServiceFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate<DemoPlatformControl, IDemoPlatformControl> PlatformControlFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate2<EventsService, IEventService, IEventPoster> EventsFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate2<GraphicsService, IGraphicsService, IGraphicsServiceControl> GraphicsServiceFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate<KeyboardService, IKeyboard> KeyboardFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate<MouseService, IMouse> MouseFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate2<NativeWindowEventsService, INativeWindowEvents, INativeWindowEventSender> NativeWindowEventsFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate2<OptionsService, IOptions, IOptionsServiceControl> OptionsServiceFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate<TestService, ITestService> TestServiceFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate<ContentMonitorService, IContentMonitor> ContentMonitorServiceFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate2<HostInfoService, IHostInfo, IHostInfoControl> HostInfoServiceFactory;


#ifdef FSL_FEATURE_GLI
  typedef ThreadLocalSingletonServiceFactoryTemplate<ImageLibraryGLIService, IImageLibraryService> ImageLibraryServiceGLIFactory;
#endif
#ifdef FSL_FEATURE_STB
  typedef ThreadLocalSingletonServiceFactoryTemplate<ImageLibrarySTBService, IImageLibraryService> ImageLibraryServiceSTBFactory;
#endif


  DemoBasicSetup DemoSetupManager::GetSetup(const DemoSetupManagerConfig& config, ExceptionMessageFormatter& rExceptionMessageFormatter, const std::weak_ptr<IServiceRegistry> weakServiceRegistry, const bool verbose, bool& rEnableFirewallRequest)
  {
    ServiceRegistry serviceRegistry(weakServiceRegistry);

    DemoHostRegistry hostRegistry;
    // Prepare the app and its desired host configuration.
    DemoHostAppSetupBuilder hostAppSetupBuilder;
    HostCustomization hostCustomization;
    {
      HostDemoAppSetup hostDemoAppSetup(hostRegistry, serviceRegistry, hostAppSetupBuilder, rExceptionMessageFormatter);
      ConfigureDemoAppEnvironment(hostDemoAppSetup);
      hostCustomization = hostDemoAppSetup.CustomizeHost;
    }

    const DemoHostAppSetup appSetup = hostAppSetupBuilder.GetSetup();

    const bool useAsyncImage = hostCustomization.Service.PreferAsyncImageService;
    auto imageServiceGroup = serviceRegistry.GetMainServiceGroup();
    if (useAsyncImage)
    {
      FSLLOG2(LogType::Verbose, "AsyncImage service enabled");
      // Setup all image loading and conversion to run in a separate thread
      imageServiceGroup = serviceRegistry.CreateServiceGroup(ServiceGroupName::Image());
      serviceRegistry.Register(AsynchronousServiceFactory(std::make_shared<AsyncImageServiceProxyFactory>(), std::make_shared<AsyncImageServiceImplFactory>()), ServicePriorityList::AsyncImageService(), imageServiceGroup);
    }
    else
    {
      // Not using async, so make sure that all image services run in the main thread
      serviceRegistry.SetServiceGroupName(imageServiceGroup, ServiceGroupName::Image());
    }

    const auto contentManagerServiceFactory = std::make_shared<ContentManagerServiceFactory>(config.ContentPath);
    const auto persistentDataManagerServiceFactory = std::make_shared<PersistentDataManagerServiceFactory>(config.PersistentDataPath);
    const auto demoAppControlServiceFactory = std::make_shared<DemoAppControlServiceFactory>(EXIT_SUCCESS);

    serviceRegistry.Register(demoAppControlServiceFactory);
    serviceRegistry.Register(contentManagerServiceFactory);
    serviceRegistry.Register(persistentDataManagerServiceFactory);

    // The image service always run on the main thread
    serviceRegistry.Register<ImageServiceFactory>(ServicePriorityList::ImageService());
    serviceRegistry.Register<BitmapConverterServiceFactory>(ServicePriorityList::BitmapConverterService(), imageServiceGroup);
    serviceRegistry.Register<ImageBasicServiceFactory>(ServicePriorityList::ImageBasicService(), imageServiceGroup);
#ifdef FSL_FEATURE_GLI
    serviceRegistry.Register<ImageLibraryServiceGLIFactory>(ServicePriorityList::ImageLibraryService(), imageServiceGroup);
#endif
#ifdef FSL_FEATURE_STB
    serviceRegistry.Register<ImageLibraryServiceSTBFactory>(ServicePriorityList::ImageLibraryService(), imageServiceGroup);
#endif
    serviceRegistry.Register<HostInfoServiceFactory>(ServicePriorityList::HostInfo());
    serviceRegistry.Register<GraphicsServiceFactory>();
    serviceRegistry.Register<GamepadsFactory>();
    serviceRegistry.Register<KeyboardFactory>();
    serviceRegistry.Register<MouseFactory>();
    serviceRegistry.Register<EventsFactory>(ServicePriorityList::EventsService());
    serviceRegistry.Register<NativeWindowEventsFactory>(ServicePriorityList::NativeWindowEventsService());
    // Launch the test service with minimum priority ensuring it has access to all other services (except other min priority services)
    serviceRegistry.Register<TestServiceFactory>(ServicePriorityList::TestService());
    serviceRegistry.Register<PlatformControlFactory>(ServicePriorityList::PlatformControlService());
    serviceRegistry.Register<ProfilerServiceFactory>(ServicePriorityList::ProfilerService());
    serviceRegistry.Register<ContentMonitorServiceFactory>(ServicePriorityList::ContentMonitor());
    serviceRegistry.Register<OptionsServiceFactory>(ServicePriorityList::Options());

    //serviceRegistry.Register<MMDCStatsServiceFactory>(ServicePriorityList::MMDCStatsService());

    // Prepare the hosts
    PlatformConfig::Configure(hostRegistry, serviceRegistry, rEnableFirewallRequest);


    DemoHostSetup hostSetup = hostRegistry.GetSetup(*(appSetup.DemoHostFeatures));

    return DemoBasicSetup(hostSetup, appSetup, verbose);
  }
}
