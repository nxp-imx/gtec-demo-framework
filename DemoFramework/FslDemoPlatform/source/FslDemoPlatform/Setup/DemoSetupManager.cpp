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
#include <FslDemoApp/Setup/HostDemoAppSetup.hpp>
#include <FslDemoPlatform/Setup/DemoHostAppSetupBuilder.hpp>
#include <FslDemoPlatform/Setup/DemoHostRegistry.hpp>
#include <FslDemoHost/Service/ServiceOptionParserDeque.hpp>
#include <FslDemoHost/Service/ServicePriorityList.hpp>
#include <FslDemoHost/Service/ServiceRegistryImpl.hpp>
#include <FslDemoHost/Service/ServiceRegistry.hpp>
#include <FslDemoHost/Service/ThreadLocal/ThreadLocalSingletonServiceFactoryTemplate.hpp>
#include <FslDemoHost/Service/BitmapConverter/BitmapConverterService.hpp>
#include <FslDemoHost/Service/Image/ImageService.hpp>
#include <FslDemoHost/Service/Content/ContentManagerServiceFactory.hpp>
#include <FslDemoHost/Service/ContentMonitor/ContentMonitorService.hpp>
#include <FslDemoHost/Service/DemoAppControl/DemoAppControlServiceFactory.hpp>
#include <FslDemoHost/Service/Events/EventsService.hpp>
#include <FslDemoHost/Service/Graphics/GraphicsService.hpp>
#include <FslDemoHost/Service/Host/HostInfoService.hpp>
#include <FslDemoHost/Service/Keyboard/KeyboardService.hpp>
#include <FslDemoHost/Service/Mouse/MouseService.hpp>
#include <FslDemoHost/Service/NativeWindowEvents/NativeWindowEventsService.hpp>
#include <FslDemoHost/Service/Options/OptionsService.hpp>
#include <FslDemoHost/Service/Profiler/ProfilerService.hpp>
#include <FslDemoHost/Service/Persistent/PersistentDataManagerServiceFactory.hpp>
#include <FslDemoHost/Service/Test/TestService.hpp>
#include <FslDemoPlatform/Service/DemoPlatformControl/DemoPlatformControl.hpp>
#include "../Configuration/PlatformConfig.hpp"
#include <FslDemoApp/Setup/RegisterDemoApp_fwd.hpp>
#include <cassert>
//#include <FslDemoPlatform/Service/MMDCStats/MMDCStatsService.hpp>

#ifdef FSL_FEATURE_GLI
#include <FslDemoPlatform/Service/ImageLibrary/ImageLibraryGLIService.hpp>
#endif

namespace Fsl
{

  //typedef ThreadLocalSingletonServiceFactoryTemplate<MMDCStatsService, IMMDCStatsService> MMDCStatsServiceFactory;

  typedef ThreadLocalSingletonServiceFactoryTemplate<BitmapConverterService, IBitmapConverter> BitmapConverterServiceFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate2<ImageService, IImageService, IImageServiceControl> ImageServiceFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate<DemoPlatformControl, IDemoPlatformControl> PlatformControlFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate2<EventsService, IEventService, IEventPoster> EventsFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate2<GraphicsService, IGraphicsService, IGraphicsServiceControl> GraphicsServiceFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate<KeyboardService, IKeyboard> KeyboardFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate<MouseService, IMouse> MouseFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate2<NativeWindowEventsService, INativeWindowEvents, INativeWindowEventSender> NativeWindowEventsFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate2<OptionsService, IOptions, IOptionsServiceControl> OptionsServiceFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate<TestService, ITestService> TestServiceFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate2<ProfilerService, IProfilerService, IProfilerServiceControl> ProfilerServiceFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate<ContentMonitorService, IContentMonitor> ContentMonitorServiceFactory;
  typedef ThreadLocalSingletonServiceFactoryTemplate2<HostInfoService, IHostInfo, IHostInfoControl> HostInfoServiceFactory;


#ifdef FSL_FEATURE_GLI
  typedef ThreadLocalSingletonServiceFactoryTemplate<ImageLibraryGLIService, IImageLibraryService> ImageLibraryServiceGLIFactory;
#endif


  DemoBasicSetup DemoSetupManager::GetSetup(const DemoSetupManagerConfig& config, const bool verbose, bool& rEnableFirewallRequest)
  {
    auto theServiceRegistry = std::make_shared<ServiceRegistryImpl>();
    ServiceRegistry serviceRegistry(theServiceRegistry);

    const auto contentManagerServiceFactory = std::make_shared<ContentManagerServiceFactory>(config.ContentPath);
    const auto persistentDataManagerServiceFactory = std::make_shared<PersistentDataManagerServiceFactory>(config.PersistentDataPath);
    const auto demoAppControlServiceFactory = std::make_shared<DemoAppControlServiceFactory>(EXIT_SUCCESS);

    serviceRegistry.Register(demoAppControlServiceFactory);
    serviceRegistry.Register(contentManagerServiceFactory);
    serviceRegistry.Register(persistentDataManagerServiceFactory);

    serviceRegistry.Register<BitmapConverterServiceFactory>(ServicePriorityList::BitmapConverterService());
    serviceRegistry.Register<ImageServiceFactory>(ServicePriorityList::ImageService());
#ifdef FSL_FEATURE_GLI
    serviceRegistry.Register<ImageLibraryServiceGLIFactory>(ServicePriorityList::ImageLibraryService());
#endif
    serviceRegistry.Register<HostInfoServiceFactory>(ServicePriorityList::HostInfo());
    serviceRegistry.Register<GraphicsServiceFactory>();
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
    DemoHostRegistry hostRegistry;
    PlatformConfig::Configure(hostRegistry, serviceRegistry, rEnableFirewallRequest);

    // Prepare the app and its desired host configuration.
    DemoHostAppSetupBuilder hostAppSetupBuilder;
    HostDemoAppSetup hostDemoAppSetup(hostRegistry, serviceRegistry, hostAppSetupBuilder);
    HOST_DemoAppSetup(&hostDemoAppSetup);

    const DemoHostAppSetup appSetup = hostAppSetupBuilder.GetSetup();
    DemoHostSetup hostSetup = hostRegistry.GetSetup(*(appSetup.DemoHostFeatures));


    // Extract the registered services and their option parsers
    assert(!hostSetup.ServiceOptionParsers);
    hostSetup.ServiceOptionParsers = std::make_shared<ServiceOptionParserDeque>();

    auto services = std::make_shared<ThreadLocalServiceDeque>();
    theServiceRegistry->LockAndExtractServices(*services, *hostSetup.ServiceOptionParsers);
    theServiceRegistry.reset();


    return DemoBasicSetup(services, hostSetup, appSetup, verbose);
  }
}
