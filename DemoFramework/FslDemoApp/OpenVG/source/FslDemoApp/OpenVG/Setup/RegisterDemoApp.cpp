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

#include <FslDemoApp/OpenVG/Setup/RegisterDemoApp.hpp>
#include <FslDemoApp/Base/Setup/HostDemoAppSetup.hpp>
#include <FslDemoApp/Base/Setup/IDemoAppRegistry.hpp>
#include <FslDemoApp/Base/Host/DemoAppSetup.hpp>
#include <FslDemoApp/Base/Host/DemoHostFeatureUtil.hpp>
#include <FslDemoHost/Base/Service/ServicePriorityList.hpp>
#include <FslDemoHost/EGL/EGLDemoHostSetup.hpp>
#include <FslDemoHost/EGL/Service/EGLHost/EGLHostServiceFactory.hpp>
#include <FslDemoHost/Base/Service/WindowHost/WindowHostServiceFactory.hpp>
#include <FslDemoPlatform/Setup/IDemoHostRegistry.hpp>
#include <FslDemoService/NativeGraphics/OpenVG/NativeGraphicsService.hpp>
#include <FslService/Impl/Registry/ServiceRegistry.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalSingletonServiceFactoryTemplate.hpp>
#include <FslUtil/EGL/Exceptions.hpp>
#include <FslUtil/EGL/DebugStrings.hpp>
#include <sstream>

namespace Fsl
{
  namespace
  {
    const DemoHostFeature CommenSetup(HostDemoAppSetup& rSetup)
    {
      // Use the EGLDemoHost for OpenVG
      std::deque<DemoHostFeatureName::Enum> eglHostFeatures;
      eglHostFeatures.push_back(DemoHostFeatureName::OpenVG);
      rSetup.TheHostRegistry.Register(eglHostFeatures, EGLDemoHostSetup::Get());
      rSetup.TheServiceRegistry.Register<ThreadLocalSingletonServiceFactoryTemplate<OpenVG::NativeGraphicsService, INativeGraphicsService> >(ServicePriorityList::NativeGraphicsService());
      rSetup.TheServiceRegistry.Register<EGLHostServiceFactory>(ServicePriorityList::EGLHostService());
      rSetup.TheServiceRegistry.Register<WindowHostServiceFactory>(ServicePriorityList::WindowHostService());

      return DemoHostFeature(DemoHostFeatureName::OpenVG, DemoHostFeatureUtil::EncodeOpenVGVersion(1));
    }


    inline bool TryFormatEGLGraphicsException(const std::exception& ex, std::string& rMessage)
    {
      auto pException = dynamic_cast<const EGLGraphicsException*>(&ex);
      if (pException == nullptr)
      {
        rMessage = std::string();
        return false;
      }

      const auto errorCode = pException->GetError();
      std::stringstream stream;
      stream << pException->what() << " failed with error code " << EGL::Debug::ErrorCodeToString(static_cast<EGLenum>(errorCode)) << " (" << errorCode << ") at " << pException->GetFilename() << "(" << pException->GetLineNumber() << ")";
      rMessage = stream.str();
      return true;
    }

    bool TryFormatException(const std::exception& ex, std::string& rMessage)
    {
      return TryFormatEGLGraphicsException(ex, rMessage);
    }
  }

  namespace DemoAppRegister
  {
    namespace OpenVG
    {
      void Register(HostDemoAppSetup& rSetup, const DemoAppSetup& demoAppSetup, const DemoAppHostConfigEGL& demoHostEGLConfig)
      {
        // Register a formatter for common OpenGLES3 exceptions (from the libs we utilize)
        rSetup.CustomExceptionFormatter.Add(TryFormatException);
        const DemoHostFeature feature = CommenSetup(rSetup);
        rSetup.TheDemoAppRegistry.Register(demoAppSetup, feature, demoHostEGLConfig);
      }
    }
  }
}
