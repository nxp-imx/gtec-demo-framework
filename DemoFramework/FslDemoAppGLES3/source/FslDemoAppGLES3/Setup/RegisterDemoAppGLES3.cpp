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

#include <FslDemoAppGLES3/Setup/RegisterDemoAppGLES3.hpp>
#include <FslDemoApp/Setup/HostDemoAppSetup.hpp>
#include <FslDemoApp/Setup/IDemoAppRegistry.hpp>
#include <FslDemoApp/Host/DemoAppSetup.hpp>
#include <FslDemoApp/Host/DemoHostFeatureUtil.hpp>
#include <FslDemoHost/Service/ServicePriorityList.hpp>
#include <FslDemoHost/Service/ServiceRegistry.hpp>
#include <FslDemoHost/Service/ThreadLocal/ThreadLocalSingletonServiceFactoryTemplate.hpp>
#include <FslDemoHostEGL/EGLDemoHostSetup.hpp>
#include <FslDemoHostEGL/Service/EGLHost/EGLHostServiceFactory.hpp>
#include <FslDemoPlatform/Setup/IDemoHostRegistry.hpp>
#include <FslNativeGraphicsGLES3/NativeGraphicsService.hpp>

namespace Fsl
{
  namespace
  {
    DemoHostFeature CommenSetup(const HostDemoAppSetup& setup, const uint16_t majorVersion, const uint16_t minorVersion)
    {
      // Use the EGLDemoHost for OpenGLES
      std::deque<DemoHostFeatureName::Enum> eglHostFeatures;
      eglHostFeatures.push_back(DemoHostFeatureName::OpenGLES);
      setup.TheHostRegistry.Register(eglHostFeatures, EGLDemoHostSetup::Get());
      setup.TheServiceRegistry.Register<ThreadLocalSingletonServiceFactoryTemplate<GLES3::NativeGraphicsService, INativeGraphicsService> >(ServicePriorityList::NativeGraphicsService());
      setup.TheServiceRegistry.Register<EGLHostServiceFactory>(ServicePriorityList::EGLHostService());

      return DemoHostFeature(DemoHostFeatureName::OpenGLES, DemoHostFeatureUtil::EncodeOpenGLESVersion(majorVersion, minorVersion));
    }

    DemoAppHostConfigEGL CommenSetup(const DemoAppHostConfigEGL& cfg, const OpenGLESMinorVersion& minorVersion)
    {
      DemoAppHostConfigEGL config(cfg);
      config.SetMinimumMiniorVersion(minorVersion.MinimumMinorVersion);
      return config;
    }
  }


  void RegisterDemoAppGLES3::Register(const HostDemoAppSetup& setup, const OpenGLESMinorVersion minorVersion, const DemoAppHostConfigEGL& demoHostEGLConfig, const std::shared_ptr<IDemoAppFactory>& appFactory)
  {
    const DemoHostFeature feature = CommenSetup(setup, 3, minorVersion.MinorVersion);
    const DemoAppSetup demoAppSetup(appFactory);
    const DemoAppHostConfigEGL demoHostEGLConfigEx(CommenSetup(demoHostEGLConfig, minorVersion));
    setup.TheDemoAppRegistry.Register(demoAppSetup, feature, demoHostEGLConfigEx);
  }


  void RegisterDemoAppGLES3::Register(const HostDemoAppSetup& setup, const OpenGLESMinorVersion minorVersion, const DemoAppHostConfigEGL& demoHostEGLConfig, const std::shared_ptr<IDemoAppFactory>& appFactory, const std::shared_ptr<Fsl::ADemoOptionParser> appOptionParser)
  {
    const DemoHostFeature feature = CommenSetup(setup, 3, minorVersion.MinorVersion);
    const DemoAppSetup demoAppSetup(appFactory, appOptionParser);
    const DemoAppHostConfigEGL demoHostEGLConfigEx(CommenSetup(demoHostEGLConfig, minorVersion));
    setup.TheDemoAppRegistry.Register(demoAppSetup, feature, demoHostEGLConfigEx);
  }
}
