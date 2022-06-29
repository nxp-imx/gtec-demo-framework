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

#include "ServiceThreadRecord.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslService/Impl/Foundation/Message/BasicMessageQueue.hpp>
#include <FslService/Impl/Foundation/Message/ThreadInitBasicMessage.hpp>
#include <FslService/Impl/Foundation/Message/ThreadShutdownBasicMessage.hpp>
#include <FslService/Impl/Foundation/Message/ThreadShutdownBeginBasicMessage.hpp>
#include <FslService/Impl/Threading/IServiceHost.hpp>
#include <FslService/Impl/Threading/IServiceHostFactory.hpp>
#include <fmt/ostream.h>
#include <cassert>
#include "ServiceHostCreateInfo.hpp"

namespace Fsl
{
  namespace
  {
    void ManagedThreadMain(const std::shared_ptr<IBasicMessageQueue>& ownerQueue, const std::shared_ptr<IBasicMessageProvider>& incomingProvider,
                           const ThreadLocalServiceConfig& serviceConfig, const std::shared_ptr<IServiceHostFactory>& serviceHostFactory)
    {
      assert(ownerQueue);
      assert(incomingProvider);

      const auto currentThreadId = std::this_thread::get_id();

      FSLLOG3_VERBOSE("Thread started for serviceGroupId {} on {}", serviceConfig.Id.GetValue(), currentThreadId);
      try
      {
        ServiceHostContext hostContext(incomingProvider);
        ServiceHostCreateInfo createInfo(hostContext, serviceConfig);
        // Allocate the host 'inside' the right thread so it lives it life fully in this thread
        const auto serviceHost = serviceHostFactory->Allocate(createInfo);
        serviceHost->Run();

        // Notify the owner queue that we shutdown
        ownerQueue->Push(ThreadShutdownBasicMessage(serviceConfig.Id));
      }
      catch (const std::exception& ex)
      {
        FSLLOG3_ERROR("Thread serviceGroupId {} on {} threw exception: {}", serviceConfig.Id.GetValue(), currentThreadId, ex.what());
        // Notify the owner queue that we shutdown due to a exception
        ownerQueue->Push(ThreadShutdownBasicMessage(serviceConfig.Id, std::current_exception()));
      }
      FSLLOG3_VERBOSE("Thread stopped for serviceGroupId {} on {}", serviceConfig.Id.GetValue(), currentThreadId);
    }
  }


  ServiceThreadRecord::ServiceThreadRecord(const std::shared_ptr<IBasicMessageQueue>& ownerQueue,
                                           const std::shared_ptr<BasicMessageQueue>& hostReceiveQueue, const ThreadLocalServiceConfig& serviceConfig,
                                           const std::shared_ptr<IServiceHostFactory>& serviceHostFactory)
    : m_hostMessageQueue(hostReceiveQueue)
    , m_thread(ManagedThreadMain, ownerQueue, hostReceiveQueue, serviceConfig, serviceHostFactory)
  {
    m_hostMessageQueue->Push(ThreadInitBasicMessage());
  }


  ServiceThreadRecord::~ServiceThreadRecord()
  {
    if (m_hostMessageQueue)
    {
      m_hostMessageQueue->Push(ThreadShutdownBeginBasicMessage());
    }
    m_thread.join();
  }
}
