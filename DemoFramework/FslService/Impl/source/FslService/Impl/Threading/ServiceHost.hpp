#ifndef FSLSERVICE_IMPL_THREADING_SERVICEHOST_HPP
#define FSLSERVICE_IMPL_THREADING_SERVICEHOST_HPP
/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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
 *    * Neither the name of the NXP. nor the names of
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

#include <FslService/Impl/Foundation/Message/BasicMessage.hpp>
#include <FslService/Impl/Threading/IServiceHost.hpp>
#include <FslService/Impl/Threading/ServiceHostContext.hpp>
#include <queue>
#include <memory>

namespace Fsl
{
  class AsyncServiceImplHost;
  struct ServiceHostCreateInfo;
  class ServiceProviderImpl;

  class ServiceHost : public IServiceHost
  {
    ServiceHostContext m_hostContext;
    std::shared_ptr<ServiceProviderImpl> m_serviceProvider;
    std::unique_ptr<AsyncServiceImplHost> m_asyncServiceImplHost;
    std::queue<BasicMessage> m_messageScratchpad;
    bool m_quitRequested;

  public:
    ServiceHost(const ServiceHostCreateInfo& createInfo, const bool clearOwnedUniqueServices = true);
    ~ServiceHost() override;

    // Inherited via IServiceHost
    void ProcessMessages(const bool useTimeout = true, const std::chrono::milliseconds& duration = std::chrono::milliseconds(0)) override;
    std::shared_ptr<IServiceProvider> GetServiceProvider() const override;
    void Run() override;

  private:
    void ProcessMessage(const BasicMessage& message);
  };
}

#endif
