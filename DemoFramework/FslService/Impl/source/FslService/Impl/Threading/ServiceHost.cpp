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

#include "ServiceHost.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/BasicLog.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslService/Impl/Foundation/Message/FireAndForgetBasicMessage.hpp>
#include <FslService/Impl/Foundation/Message/ThreadShutdownBasicMessage.hpp>
#include <FslService/Impl/Threading/Launcher/ServiceLauncher.hpp>
#include "AsyncServiceImplHost.hpp"
#include "ServiceHostCreateInfo.hpp"
#include "Provider/ServiceProviderImpl.hpp"
#include <cassert>

namespace Fsl
{
  ServiceHost::ServiceHost(const ServiceHostCreateInfo& createInfo, const bool clearOwnedUniqueServices)
    : m_hostContext(createInfo.HostContext)
    , m_quitRequested(false)
  {
    m_serviceProvider =
      ServiceLauncher::Launch(createInfo.ServiceConfig.GlobalServiceTypeMaps, createInfo.ServiceConfig.ThreadLocalServices, clearOwnedUniqueServices);
    m_asyncServiceImplHost.reset(new AsyncServiceImplHost(createInfo.ServiceConfig.AsyncServiceImplLaunchFactories, m_serviceProvider));
  }


  ServiceHost::~ServiceHost() = default;


  void ServiceHost::ProcessMessages(const bool useTimeout, const std::chrono::milliseconds& duration)
  {
    if (m_hostContext.TheCancellationToken->IsCancellationRequested())
    {
      m_quitRequested = true;
      return;
    }

    assert(m_messageScratchpad.empty());

    if (useTimeout)
    {
      // use a timeout while waiting for a message
      m_hostContext.IncomingProvider->TryPopWait(m_messageScratchpad, duration);
    }
    else
    {
      // Sleep until a message arrives
      m_hostContext.IncomingProvider->PopWait(m_messageScratchpad);
    }

    while (!m_messageScratchpad.empty())
    {
      auto message = m_messageScratchpad.front();
      m_messageScratchpad.pop();

      ProcessMessage(message);

      // FIX: support message pooling
      // if (message.MessagePool)
      //  message.MessagePool->Release(message.Content);
    }

    // Give the various services types a chance to update
    m_serviceProvider->Update();
  }


  std::shared_ptr<IServiceProvider> ServiceHost::GetServiceProvider() const
  {
    assert(m_serviceProvider);
    return m_serviceProvider;
  }


  void ServiceHost::Run()
  {
    while (!m_quitRequested)
    {
      ProcessMessages(false);
    }
  }


  void ServiceHost::ProcessMessage(const BasicMessage& message)
  {
    switch (message.Type)
    {
    case BasicMessageType::ThreadInit:
      break;
    case BasicMessageType::ThreadShutdownBegin:
      m_quitRequested = true;
      break;
    case BasicMessageType::ThreadShutdown:
    {
      auto messageEx = ThreadShutdownBasicMessage::Decode(message);
      // If a exception occurred then rethrow it on this thread so we get a more controlled shutdown
      if (messageEx.Exception)
      {
        std::rethrow_exception(messageEx.Exception);
      }
      break;
    }
    case BasicMessageType::FireAndForgetMessage:
      if (m_asyncServiceImplHost)
      {
        m_asyncServiceImplHost->ProcessMessage(FireAndForgetBasicMessage::Decode(message));
      }
      FSLBASICLOG_WARNING_IF(!m_asyncServiceImplHost, "the expected handler has been destroyed");
      break;
    default:
      FSLLOG_DEBUG_WARNING("Unknown message type: " << static_cast<int32_t>(message.Type));
      break;
    }
  }
}
