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

#include <FslDemoApp/OpenVX/Setup/RegisterDemoApp.hpp>
#include <FslDemoApp/Base/Setup/HostDemoAppSetup.hpp>
#include <FslDemoApp/Base/Setup/IDemoAppRegistry.hpp>
#include <FslDemoApp/Base/Host/DemoAppSetup.hpp>
#include <FslDemoApp/Shared/Host/DemoHostFeatureUtil.hpp>
#include <FslDemoHost/Base/Service/ServicePriorityList.hpp>
#include <FslDemoHost/Base/Setup/IDemoHostRegistry.hpp>
#include <FslDemoHost/Console/ConsoleDemoHostSetup.hpp>
#include <FslService/Impl/Registry/ServiceRegistry.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalSingletonServiceFactoryTemplate.hpp>
#include <FslBase/ExceptionMessageFormatter.hpp>
#include <RapidOpenVX/Exceptions.hpp>
#include <RapidOpenVX/Debug/Strings/vx_status_e.hpp>
#include <sstream>

namespace Fsl
{
  namespace
  {
    const DemoHostFeature CommenSetup(HostDemoAppSetup& rSetup)
    {
      std::deque<DemoHostFeatureName::Enum> hostFeatures;
      hostFeatures.push_back(DemoHostFeatureName::OpenVX);
      rSetup.TheHostRegistry.Register(hostFeatures, ConsoleDemoHostSetup::Get());

      return DemoHostFeature(DemoHostFeatureName::OpenVX, DemoHostFeatureUtil::EncodeVersion(1));
    }

    inline bool TryFormatOpenVXStatusErrorException(const std::exception& ex, std::string& rMessage)
    {
      auto pException = dynamic_cast<const RapidOpenVX::OpenVXStatusErrorException*>(&ex);
      if (pException == nullptr)
      {
        rMessage = std::string();
        return false;
      }

      const auto errorCode = pException->GetStatus();
      std::stringstream stream;
      stream << pException->what() << " failed with error code " << RapidOpenVX::Debug::ToString(static_cast<vx_status_e>(errorCode)) << " (" << errorCode << ") at " << pException->GetFileName() << "(" << pException->GetLineNumber() << ")";
      rMessage = stream.str();
      return true;
    }

    inline bool TryFormatExceptionOpenVXException(const std::exception& ex, std::string& rMessage)
    {
      auto pException = dynamic_cast<const RapidOpenVX::OpenVXException*>(&ex);
      if (pException == nullptr)
      {
        rMessage = std::string();
        return false;
      }

      std::stringstream stream;
      stream << pException->what() << " failed at " << pException->GetFileName() << "(" << pException->GetLineNumber() << ")";
      rMessage = stream.str();
      return true;
    }

    bool TryFormatException(const std::exception& ex, std::string& rMessage)
    {
      if (TryFormatOpenVXStatusErrorException(ex, rMessage))
        return true;
      return TryFormatExceptionOpenVXException(ex, rMessage);
    }
  }

  namespace DemoAppRegister
  {
    namespace OpenVX
    {
      void Register(HostDemoAppSetup& rSetup, const DemoAppSetup& demoAppSetup)
      {
        // Register a formatter for common OpenVX exceptions (from the libs we utilize)
        rSetup.CustomExceptionFormatter.Add(TryFormatException);

        const DemoHostFeature feature = CommenSetup(rSetup);
        rSetup.TheDemoAppRegistry.Register(demoAppSetup, feature);
      }
    }
  }
}
