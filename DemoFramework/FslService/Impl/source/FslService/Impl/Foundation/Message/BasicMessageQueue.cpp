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

#include <FslService/Impl/Foundation/Message/BasicMessageQueue.hpp>
#include <FslBase/Exceptions.hpp>

namespace Fsl
{
  BasicMessageQueue::BasicMessageQueue(const ServiceGroupId& serviceGroupId)
    : m_shutdownMarked(false)
    , TheServiceGroupId(serviceGroupId)
  {
  }


  void BasicMessageQueue::Push(const BasicMessage& message)
  {
    if (!TryPush(message))
    {
      throw std::runtime_error("Queue has been shutdown as the thread has been killed");
    }
  }


  bool BasicMessageQueue::TryPush(const BasicMessage& message)
  {
    // FIX: this check was incorrect as it marked the wrong queue, a mark as dead function is probably better
    const bool isShutdownMessage = false;    // (message.Type == BasicMessageType::ThreadShutdown);
    bool wasEmpty = false;
    {
      std::lock_guard<std::mutex> lock(m_mutex);

      if (m_shutdownMarked)
      {
        return false;
      }

      wasEmpty = m_queue.empty();
      m_queue.push(message);

      if (isShutdownMessage)
      {
        m_shutdownMarked = true;
      }
    }
    if (wasEmpty)
    {
      UnsafeWake();
    }

    return true;
  }


  bool BasicMessageQueue::TryPop(std::queue<BasicMessage>& rQueue)
  {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_queue.empty())
    {
      return false;
    }

    while (!m_queue.empty())
    {
      rQueue.push(m_queue.front());
      m_queue.pop();
    }
    return true;
  }


  bool BasicMessageQueue::TryPop(BasicMessage& rMessage)
  {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_queue.empty())
    {
      rMessage = BasicMessage();
      return false;
    }
    rMessage = m_queue.front();
    m_queue.pop();
    return true;
  }


  void BasicMessageQueue::PopWait(std::queue<BasicMessage>& rQueue)
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    // Wait for a message to arrive
    while (m_queue.empty())
    {
      m_waitForMsgCondition.wait(lock);
    }

    while (!m_queue.empty())
    {
      rQueue.push(m_queue.front());
      m_queue.pop();
    }
  }


  void BasicMessageQueue::PopWait(BasicMessage& rMessage)
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    // Wait for a message to arrive
    while (m_queue.empty())
    {
      m_waitForMsgCondition.wait(lock);
    }

    rMessage = m_queue.front();
    m_queue.pop();
  }


  bool BasicMessageQueue::TryPopWait(std::queue<BasicMessage>& rQueue, const std::chrono::milliseconds& duration)
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    // Wait for a message to arrive
    if (duration.count() > 0)
    {
      if (m_queue.empty())
      {
        m_waitForMsgCondition.wait_for(lock, duration);
      }
    }

    // We dont just check rQueue.empty() since it might have been non-empty to begin with
    const bool hasMessage = !m_queue.empty();
    while (!m_queue.empty())
    {
      rQueue.push(m_queue.front());
      m_queue.pop();
    }
    return hasMessage;
  }


  bool BasicMessageQueue::TryPopWait(BasicMessage& rMessage, const std::chrono::milliseconds& duration)
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    // Wait for a message to arrive
    if (duration.count() > 0)
    {
      if (m_queue.empty())
      {
        m_waitForMsgCondition.wait_for(lock, duration);
      }
    }

    if (m_queue.empty())
    {
      rMessage = BasicMessage();
      return false;
    }

    rMessage = m_queue.front();
    m_queue.pop();
    return true;
  }


  void BasicMessageQueue::UnsafeWake()
  {
    m_waitForMsgCondition.notify_one();
  }
}
