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

#include <FslDemoApp/Base/Host/DemoAppSetup.hpp>
#include <FslDemoApp/Base/Setup/HostDemoAppSetup.hpp>
#include <FslDemoApp/Base/Setup/IDemoAppRegistry.hpp>
#include <FslDemoApp/Shared/Host/DemoHostFeatureUtil.hpp>
#include <FslDemoApp/Util/Graphics/RegisterDemoAppUtilGraphics.hpp>
#include <FslDemoApp/Vulkan/Setup/RegisterDemoApp.hpp>
#include <FslDemoHost/Base/Service/ServicePriorityList.hpp>
#include <FslDemoHost/Base/Service/WindowHost/WindowHostServiceFactory.hpp>
#include <FslDemoHost/Base/Setup/IDemoHostRegistry.hpp>
#include <FslDemoHost/Vulkan/Service/VulkanHost/VulkanHostServiceFactory.hpp>
#include <FslDemoHost/Vulkan/VulkanDemoHostSetup.hpp>
// #include <FslDemoHost/Vulkan/Service/VulkanHost/VulkanHostServiceFactory.hpp>
#include <FslDemoService/Graphics/Impl/GraphicsServiceFactory.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsService.hpp>
#include <FslService/Impl/Registry/ServiceRegistry.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalSingletonServiceFactoryTemplate.hpp>
// #include <FslUtil/Vulkan1_0/Exceptions.hpp>
// #include <FslUtil/Vulkan1_0/DebugStrings.hpp>
#include <sstream>

namespace Fsl
{
  namespace
  {
    using NativeGraphicsServiceFactory =
      ThreadLocalSingletonServiceFactoryTemplate2<Vulkan::NativeGraphicsService, INativeGraphicsService, Vulkan::NativeGraphicsService>;

    DemoHostFeature CommenSetup(HostDemoAppSetup& rSetup)
    {
      // Use the VulkanDemoHost for Vulkan
      std::deque<DemoHostFeatureName::Enum> hostFeatures;
      hostFeatures.push_back(DemoHostFeatureName::Vulkan);
      rSetup.TheHostRegistry.Register(hostFeatures, VulkanDemoHostSetup::Get());
      rSetup.TheServiceRegistry.Register<VulkanHostServiceFactory>(ServicePriorityList::VulkanHostService());
      rSetup.TheServiceRegistry.Register<GraphicsServiceFactory>();
      rSetup.TheServiceRegistry.Register<NativeGraphicsServiceFactory>(ServicePriorityList::NativeGraphicsService());
      rSetup.TheServiceRegistry.Register<WindowHostServiceFactory>(ServicePriorityList::WindowHostService());


      // Do common graphics app setup
      RegisterDemoAppUtilGraphics::Setup(rSetup);

      return {DemoHostFeatureName::Vulkan, DemoHostFeatureUtil::EncodeVersion(1, 0)};
    }

    // inline bool TryFormatException(const std::exception& ex, std::string& rMessage)
    //{
    //  auto pException = dynamic_cast<const GLES2::GLESGraphicsException*>(&ex);
    //  if (pException == nullptr)
    //  {
    //    rMessage = std::string();
    //    return false;
    //  }

    //  const auto errorCode = pException->GetError();
    //  std::stringstream stream;
    //  stream << pException->what() << " failed with error code " << GLES2::Debug::ErrorCodeToString(static_cast<GLenum>(errorCode)) << " (" <<
    //  errorCode << ") at " << pException->GetFilename() << "(" << pException->GetLineNumber() << ")"; rMessage = stream.str(); return true;
    //}
  }

  namespace DemoAppRegister::Vulkan
  {
    void Register(HostDemoAppSetup& rSetup, const DemoAppSetup& demoAppSetup, const DemoAppHostConfigVulkan& demoHostConfig)
    {
      // Register a formatter for common Vulkan exceptions (from the libs we utilize)
      // rSetup.CustomExceptionFormatter.Add(TryFormatException);

      const DemoHostFeature feature = CommenSetup(rSetup);
      const auto appHostConfig = std::make_shared<DemoAppHostConfigVulkan>(demoHostConfig);
      rSetup.TheDemoAppRegistry.Register(demoAppSetup, feature, appHostConfig);
    }
  }
}
