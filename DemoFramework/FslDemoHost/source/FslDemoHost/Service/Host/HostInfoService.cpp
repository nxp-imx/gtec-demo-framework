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

#include <FslDemoHost/Service/Host/HostInfoService.hpp>
#include <FslBase/ITag.hpp>

namespace Fsl
{

  HostInfoService::HostInfoService(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)
    , m_activeAPI(DemoHostFeatureName::Stub, 0)
    , m_appHostConfig()
  {
  }


  HostInfoService::~HostInfoService()
  {
  }


  DemoHostFeature HostInfoService::GetActiveAPI() const
  {
    return m_activeAPI;
  }


  std::shared_ptr<DemoAppHostConfig> HostInfoService::GetAppHostConfig() const
  {
    return m_appHostConfig;
  }


  void HostInfoService::SetActiveAPI(const DemoHostFeature feature)
  {
    m_activeAPI = feature;
  }


  void HostInfoService::SetAppHostConfig(std::shared_ptr<DemoAppHostConfig> appHostConfig)
  {
    m_appHostConfig = appHostConfig;
  }
}
