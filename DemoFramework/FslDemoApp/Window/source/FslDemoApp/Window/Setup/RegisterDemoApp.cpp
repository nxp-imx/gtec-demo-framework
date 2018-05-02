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

#include <FslDemoApp/Window/Setup/RegisterDemoApp.hpp>
#include <FslDemoApp/Base/Setup/HostDemoAppSetup.hpp>
#include <FslDemoApp/Base/Setup/IDemoAppRegistry.hpp>
#include <FslDemoApp/Base/Host/DemoAppSetup.hpp>
#include <FslDemoApp/Shared/Host/DemoHostFeatureUtil.hpp>
#include <FslDemoHost/Base/Service/ServicePriorityList.hpp>
#include <FslDemoHost/Base/Service/WindowHost/WindowHostServiceFactory.hpp>
#include <FslDemoHost/Base/Setup/IDemoHostRegistry.hpp>
#include <FslDemoHost/Window/WindowDemoHostSetup.hpp>
#include <FslService/Impl/Registry/ServiceRegistry.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalSingletonServiceFactoryTemplate.hpp>

namespace Fsl
{
  namespace
  {
    const DemoHostFeature CommenSetup(HostDemoAppSetup& rSetup)
    {
      std::deque<DemoHostFeatureName::Enum> hostFeatures;
      hostFeatures.push_back(DemoHostFeatureName::Window);
      rSetup.TheHostRegistry.Register(hostFeatures, WindowDemoHostSetup::Get());
      rSetup.TheServiceRegistry.Register<WindowHostServiceFactory>(ServicePriorityList::WindowHostService());

      return DemoHostFeature(DemoHostFeatureName::Window, DemoHostFeatureUtil::EncodeVersion(1));
    }
  }

  namespace DemoAppRegister
  {
    namespace Window
    {
      void Register(HostDemoAppSetup& rSetup, const DemoAppSetup& demoAppSetup, const DemoAppHostConfigWindow& demoHostConfig)
      {
        const DemoHostFeature feature = CommenSetup(rSetup);
        const auto appHostConfig = std::make_shared<DemoAppHostConfigWindow>(demoHostConfig);
        rSetup.TheDemoAppRegistry.Register(demoAppSetup, feature, appHostConfig);
      }
    }
  }
}
