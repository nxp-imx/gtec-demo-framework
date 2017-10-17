#ifndef FSLSERVICE_IMPL_SERVICETYPE_ASYNC_ASYNCHRONOUSSERVICEPROXY_HPP
#define FSLSERVICE_IMPL_SERVICETYPE_ASYNC_ASYNCHRONOUSSERVICEPROXY_HPP
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

#include <FslService/Consumer/IBasicService.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslService/Impl/Foundation/Message/IBasicMessageQueue.hpp>
#include <FslService/Impl/Foundation/Message/FireAndForgetBasicMessage.hpp>
#include <memory>
#include <future>

namespace Fsl
{
  struct AsynchronousServiceProxyCreateInfo;
  class IBasicMessageQueue;

  class AsynchronousServiceProxy : public IBasicService
  {
    ProviderId m_id;
    std::weak_ptr<IBasicMessageQueue> m_serviceQueue;
    //std::shared_ptr<IMessagePool> m_messagePool;
  public:
    AsynchronousServiceProxy(const AsynchronousServiceProxyCreateInfo& createInfo);
    ~AsynchronousServiceProxy();

  protected:
    //! @brief Post a message to the 'service' backing this proxy
    template<typename TMessage>
    std::future<typename TMessage::promise_return_type> PostMessage(TMessage&& message) const
    {
      // FIX: get this from a message pool
      auto newMessage = std::make_shared<TMessage>();
      // move the content into the shared_ptr message instance
      *newMessage = std::move(message);

      auto serviceQueue = m_serviceQueue.lock();
      if (!serviceQueue)
        throw std::runtime_error("The service is no longer running");

      auto future = newMessage->Promise.get_future();
      serviceQueue->Push(FireAndForgetBasicMessage(m_id, newMessage));
      return future;
    }
  };
}

#endif
