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

#include <FslDemoApp/OpenGLES3/Setup/RegisterDemoApp.hpp>
#include <FslDemoApp/Base/Setup/HostDemoAppSetup.hpp>
#include <FslDemoApp/Base/Setup/IDemoAppRegistry.hpp>
#include <FslDemoApp/Base/Host/DemoAppSetup.hpp>
#include <FslDemoApp/Shared/Host/DemoHostFeatureUtil.hpp>
#include <FslDemoHost/Base/Service/ServicePriorityList.hpp>
#include <FslDemoHost/Base/Service/WindowHost/WindowHostServiceFactory.hpp>
#include <FslDemoHost/Base/Setup/IDemoHostRegistry.hpp>
#include <FslDemoHost/EGL/Service/EGLHost/EGLHostServiceFactory.hpp>
#include <FslDemoHost/OpenGLES3/DemoHostSetupOpenGLES3.hpp>
#include <FslDemoService/Graphics/Impl/GraphicsService.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES3/NativeGraphicsService.hpp>
#include <FslService/Impl/Registry/ServiceRegistry.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalSingletonServiceFactoryTemplate.hpp>
#include <FslUtil/EGL/Exceptions.hpp>
#include <FslUtil/EGL/DebugStrings.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/DebugStrings.hpp>
#include <sstream>

namespace Fsl
{
  namespace
  {
    typedef ThreadLocalSingletonServiceFactoryTemplate2<GraphicsService, IGraphicsService, IGraphicsServiceControl> GraphicsServiceFactory;

    DemoHostFeature CommenSetup(HostDemoAppSetup& rSetup, const uint16_t majorVersion, const uint16_t minorVersion)
    {
      // Use the EGLDemoHost for OpenGLES
      std::deque<DemoHostFeatureName::Enum> eglHostFeatures;
      eglHostFeatures.push_back(DemoHostFeatureName::OpenGLES);
      rSetup.TheHostRegistry.Register(eglHostFeatures, DemoHostSetupOpenGLES3::Get());
      rSetup.TheServiceRegistry.Register<GraphicsServiceFactory>();
      rSetup.TheServiceRegistry.Register<ThreadLocalSingletonServiceFactoryTemplate<GLES3::NativeGraphicsService, INativeGraphicsService> >(ServicePriorityList::NativeGraphicsService());
      rSetup.TheServiceRegistry.Register<EGLHostServiceFactory>(ServicePriorityList::EGLHostService());
      rSetup.TheServiceRegistry.Register<WindowHostServiceFactory>(ServicePriorityList::WindowHostService());

      return DemoHostFeature(DemoHostFeatureName::OpenGLES, DemoHostFeatureUtil::EncodeOpenGLESVersion(majorVersion, minorVersion));
    }

    DemoAppHostConfigEGL CommenSetup(const DemoAppHostConfigEGL& cfg, const OpenGLESMinorVersion& minorVersion)
    {
      DemoAppHostConfigEGL config(cfg);
      config.SetMinimumMiniorVersion(minorVersion.MinimumMinorVersion);
      return config;
    }


    inline bool TryFormatGLESGraphicsException(const std::exception& ex, std::string& rMessage)
    {
      auto pException = dynamic_cast<const GLES3::GLESGraphicsException*>(&ex);
      if (pException == nullptr)
      {
        rMessage = std::string();
        return false;
      }

      const auto errorCode = pException->GetError();
      std::stringstream stream;
      stream << pException->what() << " failed with error code " << GLES3::Debug::ErrorCodeToString(static_cast<GLenum>(errorCode)) << " (" << errorCode << ") at " << pException->GetFilename() << "(" << pException->GetLineNumber() << ")";
      rMessage = stream.str();
      return true;
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
      if (TryFormatGLESGraphicsException(ex, rMessage))
        return true;
      return TryFormatEGLGraphicsException(ex, rMessage);
    }
  }

  namespace DemoAppRegister
  {
    namespace GLES3
    {
      void Register(HostDemoAppSetup& rSetup, const DemoAppSetup& demoAppSetup, const DemoAppHostConfigEGL& demoHostEGLConfig, const OpenGLESMinorVersion minorVersion)
      {
        // Register a formatter for common OpenGLES3 exceptions (from the libs we utilize)
        rSetup.CustomExceptionFormatter.Add(TryFormatException);

        const DemoHostFeature feature = CommenSetup(rSetup, 3, minorVersion.MinorVersion);

        const auto appHostConfig = std::make_shared<DemoAppHostConfigEGL>(CommenSetup(demoHostEGLConfig, minorVersion));

        rSetup.TheDemoAppRegistry.Register(demoAppSetup, feature, appHostConfig);
      }
    }
  }
}
