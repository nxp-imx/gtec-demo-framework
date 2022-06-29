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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoApp/Base/Setup/HostDemoAppSetup.hpp>
#include <FslDemoApp/Base/Setup/RegisterDemoApp_fwd.hpp>
#include <FslDemoHost/Base/Service/AppInfo/AppInfoService.hpp>
#include <FslDemoHost/Base/Service/Content/ContentManagerServiceFactory.hpp>
#include <FslDemoHost/Base/Service/ContentMonitor/ContentMonitorService.hpp>
#include <FslDemoHost/Base/Service/DemoAppControl/DemoAppControlServiceFactory.hpp>
#include <FslDemoHost/Base/Service/Events/EventsService.hpp>
#include <FslDemoHost/Base/Service/Gamepad/GamepadsService.hpp>
#include <FslDemoHost/Base/Service/Host/HostInfoService.hpp>
#include <FslDemoHost/Base/Service/Keyboard/KeyboardService.hpp>
#include <FslDemoHost/Base/Service/Mouse/MouseService.hpp>
#include <FslDemoHost/Base/Service/NativeWindowEvents/NativeWindowEventsService.hpp>
#include <FslDemoHost/Base/Service/Options/OptionsService.hpp>
#include <FslDemoHost/Base/Service/Persistent/PersistentDataManagerServiceFactory.hpp>
#include <FslDemoHost/Base/Service/Profiler/ProfilerService.hpp>
#include <FslDemoHost/Base/Service/Profiler/ProfilerServiceFactory.hpp>
#include <FslDemoHost/Base/Service/ServicePriorityList.hpp>
#include <FslDemoHost/Base/Service/Test/TestService.hpp>
#include <FslDemoPlatform/Service/DemoPlatformControl/DemoPlatformControl.hpp>
#include <FslDemoPlatform/Setup/DemoHostAppSetupBuilder.hpp>
#include <FslDemoPlatform/Setup/DemoHostRegistry.hpp>
#include <FslDemoPlatform/Setup/DemoSetupManager.hpp>
#include <FslService/Impl/Registry/ServiceRegistry.hpp>
#include <FslService/Impl/ServiceOptionParserDeque.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalSingletonServiceFactoryTemplate.hpp>
#include <cassert>
#include "../Configuration/PlatformConfig.hpp"


namespace Fsl
{
  using AppInfoServiceFactory = ThreadLocalSingletonServiceFactoryTemplate2<AppInfoService, IAppInfoService, IAppInfoControlService>;
  using GamepadsFactory = ThreadLocalSingletonServiceFactoryTemplate<GamepadsService, IGamepads>;
  using PlatformControlFactory = ThreadLocalSingletonServiceFactoryTemplate<DemoPlatformControl, IDemoPlatformControl>;
  using EventsFactory = ThreadLocalSingletonServiceFactoryTemplate2<EventsService, IEventService, IEventPoster>;
  using KeyboardFactory = ThreadLocalSingletonServiceFactoryTemplate<KeyboardService, IKeyboard>;
  using MouseFactory = ThreadLocalSingletonServiceFactoryTemplate<MouseService, IMouse>;
  using NativeWindowEventsFactory =
    ThreadLocalSingletonServiceFactoryTemplate2<NativeWindowEventsService, INativeWindowEvents, INativeWindowEventSender>;
  using OptionsServiceFactory = ThreadLocalSingletonServiceFactoryTemplate2<OptionsService, IOptions, IOptionsServiceControl>;
  using TestServiceFactory = ThreadLocalSingletonServiceFactoryTemplate<TestService, ITestService>;
  using ContentMonitorServiceFactory = ThreadLocalSingletonServiceFactoryTemplate<ContentMonitorService, IContentMonitor>;
  using HostInfoServiceFactory = ThreadLocalSingletonServiceFactoryTemplate2<HostInfoService, IHostInfo, IHostInfoControl>;


  DemoBasicSetup DemoSetupManager::GetSetup(const DemoSetupManagerConfig& config, ExceptionMessageFormatter& rExceptionMessageFormatter,
                                            const std::weak_ptr<IServiceRegistry>& weakServiceRegistry, const uint32_t verbosityLevel,
                                            bool& rEnableFirewallRequest)
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


    const auto contentManagerServiceFactory = std::make_shared<ContentManagerServiceFactory>(config.ContentPath);
    const auto persistentDataManagerServiceFactory = std::make_shared<PersistentDataManagerServiceFactory>(config.PersistentDataPath);
    const auto demoAppControlServiceFactory = std::make_shared<DemoAppControlServiceFactory>(EXIT_SUCCESS);

    serviceRegistry.Register(demoAppControlServiceFactory);
    serviceRegistry.Register(contentManagerServiceFactory);
    serviceRegistry.Register(persistentDataManagerServiceFactory);

    serviceRegistry.Register<HostInfoServiceFactory>(ServicePriorityList::HostInfo());
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
    serviceRegistry.Register<AppInfoServiceFactory>(ServicePriorityList::AppInfoService());
    serviceRegistry.Register<OptionsServiceFactory>(ServicePriorityList::Options());

    // Prepare the hosts
    PlatformConfig::Configure(hostRegistry, serviceRegistry, rEnableFirewallRequest);


    DemoHostSetup hostSetup = hostRegistry.GetSetup(*(appSetup.DemoHostFeatures));

    return {hostSetup, appSetup, verbosityLevel};
  }
}
