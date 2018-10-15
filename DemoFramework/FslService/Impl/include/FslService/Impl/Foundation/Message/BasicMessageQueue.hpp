#ifndef FSLSERVICE_IMPL_FOUNDATION_MESSAGE_BASICMESSAGEQUEUE_HPP
#define FSLSERVICE_IMPL_FOUNDATION_MESSAGE_BASICMESSAGEQUEUE_HPP
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
#include <FslService/Impl/Foundation/Message/IBasicMessageProvider.hpp>
#include <FslService/Impl/Foundation/Message/IBasicMessageQueue.hpp>
#include <FslService/Impl/Registry/ServiceGroupId.hpp>
#include <condition_variable>
#include <mutex>

namespace Fsl
{
  class BasicMessageQueue
    : public IBasicMessageQueue
    , public IBasicMessageProvider
  {
    std::mutex m_mutex;
    std::condition_variable m_waitForMsgCondition;
    std::queue<BasicMessage> m_queue;
    bool m_shutdownMarked;

  public:
    const ServiceGroupId TheServiceGroupId;

    BasicMessageQueue(const BasicMessageQueue&) = delete;
    BasicMessageQueue& operator=(const BasicMessageQueue&) = delete;

    BasicMessageQueue(const ServiceGroupId& serviceGroupId);

    // Inherited via IBasicMessageQueue
    void Push(const BasicMessage& message) override;
    bool TryPush(const BasicMessage& message) override;

    // Inherited via IBasicMessageProvider
    bool TryPop(std::queue<BasicMessage>& rQueue) override;
    bool TryPop(BasicMessage& rMessage) override;
    void PopWait(std::queue<BasicMessage>& rQueue) override;
    void PopWait(BasicMessage& rMessage) override;
    bool TryPopWait(std::queue<BasicMessage>& rQueue, const std::chrono::milliseconds& duration) override;
    bool TryPopWait(BasicMessage& rMessage, const std::chrono::milliseconds& duration) override;

  private:
    void UnsafeWake();
  };
}

#endif
