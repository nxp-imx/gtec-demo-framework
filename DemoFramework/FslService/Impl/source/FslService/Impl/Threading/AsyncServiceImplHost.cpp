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

#include "AsyncServiceImplHost.hpp"
#include <FslBase/Log/BasicLog.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslService/Impl/Foundation/Message/FireAndForgetBasicMessage.hpp>
#include <FslService/Impl/ServiceType/Async/AsynchronousServiceImplCreateInfo.hpp>
#include "AsynchronousServiceMessageHandlerRegistryImpl.hpp"
#include "Launcher/AsynchronousServiceImplLaunchFactoryRecord.hpp"
#include <algorithm>

namespace Fsl
{
  AsyncServiceImplHost::AsyncServiceImplHost(const std::deque<AsynchronousServiceImplLaunchFactoryRecord>& asyncServiceImplLaunchFactories,
                                             const std::shared_ptr<IServiceProvider>& serviceProvider)
  {
    std::deque<AsynchronousServiceImplLaunchFactoryRecord> sortedList(asyncServiceImplLaunchFactories);
    std::sort(sortedList.begin(), sortedList.end(),
              [](const AsynchronousServiceImplLaunchFactoryRecord& lhs, const AsynchronousServiceImplLaunchFactoryRecord& rhs) {
                return lhs.StartupPriority > rhs.StartupPriority;
              });

    ServiceProvider theServiceProvider(serviceProvider);

    for (const auto& entry : sortedList)
    {
      if (m_providerToMessageHandlerRegistry.find(entry.Id.Get()) != m_providerToMessageHandlerRegistry.end())
      {
        throw std::runtime_error("Tried to register the same providerId twice");
      }

      auto messageHandlerRegistry = std::make_shared<AsynchronousServiceMessageHandlerRegistryImpl>();
      m_providerToMessageHandlerRegistry[entry.Id.Get()] = messageHandlerRegistry;

      AsynchronousServiceImplCreateInfo createInfo(entry.Id, messageHandlerRegistry);

      auto serviceInstance = entry.Factory->Allocate(createInfo, theServiceProvider);
      m_services.push_back(serviceInstance);
      // Allow for some 'after object creation' fiddeling, if need be
      serviceInstance->Link(theServiceProvider);
    }
  };


  AsyncServiceImplHost::~AsyncServiceImplHost() = default;


  void AsyncServiceImplHost::ProcessMessage(const FireAndForgetBasicMessage& message)
  {
    if (!message.Content)
    {
      FSLBASICLOG_WARNING("Missing content message, request ignored");
      return;
    }

    auto itrFind = m_providerToMessageHandlerRegistry.find(message.TargetId.Get());
    if (itrFind == m_providerToMessageHandlerRegistry.end())
    {
      FSLLOG_WARNING("No message handler registry found for providerId: " << message.TargetId.Get()
                                                                          << " was the messsage send to the correct queue?");
      return;
    }

    if (!itrFind->second->TryProcessMessage(message.Content))
    {
      FSLLOG_WARNING("No message handler found for providerId: " << message.TargetId.Get());
      return;
    }
  }
}
