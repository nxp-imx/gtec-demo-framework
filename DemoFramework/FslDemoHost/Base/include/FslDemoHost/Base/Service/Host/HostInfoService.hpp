#ifndef FSLDEMOHOST_BASE_SERVICE_HOST_HOSTINFOSERVICE_HPP
#define FSLDEMOHOST_BASE_SERVICE_HOST_HOSTINFOSERVICE_HPP
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

#include <FslService/Impl/ServiceType/Local/ThreadLocalService.hpp>
#include <FslDemoApp/Base/Service/Host/IHostInfo.hpp>
#include <FslDemoHost/Base/Service/Host/IHostInfoControl.hpp>

namespace Fsl
{
  class HostInfoService
    : public ThreadLocalService
    , public IHostInfo
    , public IHostInfoControl
  {
    bool m_isConsoleBasedHost = false;
    DemoHostFeature m_activeAPI;
    std::shared_ptr<DemoAppHostConfig> m_appHostConfig;

  public:
    HostInfoService(const ServiceProvider& serviceProvider);
    ~HostInfoService() override;

    // From IHostInfo
    bool IsConsoleBasedHost() const override;
    DemoHostFeature GetActiveAPI() const override;
    std::shared_ptr<DemoAppHostConfig> TryGetAppHostConfig() const override;
    // From IHostInfoControl
    void SetIsConsoleBasedHost(const bool isConsoleBasedHost) override;
    void SetActiveAPI(const DemoHostFeature feature) override;
    void SetAppHostConfig(std::shared_ptr<DemoAppHostConfig> appHostConfig) override;

  private:
  };
}

#endif
