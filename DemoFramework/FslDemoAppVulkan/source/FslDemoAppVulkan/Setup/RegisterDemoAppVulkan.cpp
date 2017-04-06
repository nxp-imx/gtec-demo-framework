/****************************************************************************************************************************************************
* Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslDemoAppVulkan/Setup/RegisterDemoApp.hpp>
#include <FslDemoApp/Setup/HostDemoAppSetup.hpp>
#include <FslDemoApp/Setup/IDemoAppRegistry.hpp>
#include <FslDemoApp/Host/DemoAppSetup.hpp>
#include <FslDemoApp/Host/DemoHostFeatureUtil.hpp>
#include <FslDemoHost/Service/ServicePriorityList.hpp>
#include <FslDemoHost/Service/ServiceRegistry.hpp>
#include <FslDemoHost/Service/ThreadLocal/ThreadLocalSingletonServiceFactoryTemplate.hpp>
#include <FslDemoHostVulkan/VulkanDemoHostSetup.hpp>
//#include <FslDemoHostVulkan/Service/VulkanHost/VulkanHostServiceFactory.hpp>
#include <FslDemoPlatform/Setup/IDemoHostRegistry.hpp>
#include <FslNativeGraphicsVulkan/NativeGraphicsService.hpp>

namespace Fsl
{
  namespace
  {
    const DemoHostFeature CommenSetup(const HostDemoAppSetup& setup)
    {
      // Use the VulkanDemoHost for Vulkan
      std::deque<DemoHostFeatureName::Enum> hostFeatures;
      hostFeatures.push_back(DemoHostFeatureName::Vulkan);
      setup.TheHostRegistry.Register(hostFeatures, VulkanDemoHostSetup::Get());
      setup.TheServiceRegistry.Register<ThreadLocalSingletonServiceFactoryTemplate<Vulkan::NativeGraphicsService, INativeGraphicsService> >(ServicePriorityList::NativeGraphicsService());
      // setup.TheServiceRegistry.Register<VulkanHostServiceFactory>(ServicePriorityList::VulkanHostService());

      return DemoHostFeature(DemoHostFeatureName::Vulkan, DemoHostFeatureUtil::EncodeVersion(1,0));
    }
  }


  void RegisterDemoAppVulkan::Register(const HostDemoAppSetup& setup, const DemoAppHostConfigVulkan& demoHostConfig, const std::shared_ptr<IDemoAppFactory>& appFactory)
  {
    const DemoHostFeature feature = CommenSetup(setup);
    const DemoAppSetup demoAppSetup(appFactory);
    setup.TheDemoAppRegistry.Register(demoAppSetup, feature, demoHostConfig);
  }


  void RegisterDemoAppVulkan::Register(const HostDemoAppSetup& setup, const DemoAppHostConfigVulkan& demoHostConfig, const std::shared_ptr<IDemoAppFactory>& appFactory, const std::shared_ptr<Fsl::ADemoOptionParser> appOptionParser)
  {
    const DemoHostFeature feature = CommenSetup(setup);
    const DemoAppSetup demoAppSetup(appFactory, appOptionParser);
    setup.TheDemoAppRegistry.Register(demoAppSetup, feature, demoHostConfig);
  }
}
