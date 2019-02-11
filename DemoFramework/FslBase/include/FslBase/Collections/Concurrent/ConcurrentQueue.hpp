#ifndef FSLBASE_COLLECTIONS_CONCURRENT_CONCURRENTQUEUE_HPP
#define FSLBASE_COLLECTIONS_CONCURRENT_CONCURRENTQUEUE_HPP
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

#include <FslBase/Collections/Concurrent/ConcurrentQueue_fwd.hpp>

namespace Fsl
{
  template <typename T>
  void ConcurrentQueue<T>::Clear()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    // std::queue is missing a clear method
    while (!m_queue.empty())
    {
      m_queue.pop();
    }
  }


  template <typename T>
  void ConcurrentQueue<T>::Enqueue(const T& value)
  {
    bool wasEmpty;
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      wasEmpty = m_queue.empty();
      m_queue.push(value);
    }
    if (wasEmpty)
    {
      UnsafeWake();
    }
  }


  template <typename T>
  T ConcurrentQueue<T>::Dequeue()
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    // Wait for a message to arrive
    while (m_queue.empty())
    {
      m_waitForMsgCondition.wait(lock);
    }

    const T result = m_queue.front();
    m_queue.pop();
    return result;
  }


  template <typename T>
  bool ConcurrentQueue<T>::TryDequeue(T& rValue)
  {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_queue.empty())
    {
      rValue = T();
      return false;
    }
    rValue = m_queue.front();
    m_queue.pop();
    return true;
  }


  template <typename T>
  bool ConcurrentQueue<T>::TryDequeWait(T& rValue, const std::chrono::milliseconds& duration)
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    if (duration.count() > 0)
    {
      // Wait for a message to arrive
      if (m_queue.empty())
      {
        m_waitForMsgCondition.wait_for(lock, duration);
      }
    }

    if (m_queue.empty())
    {
      rValue = T();
      return false;
    }


    const T result = m_queue.front();
    m_queue.pop();
    rValue = result;
    return true;
  }


  template <typename T>
  bool ConcurrentQueue<T>::TryPeek(T& rValue)
  {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_queue.empty())
    {
      rValue = T();
      return false;
    }
    rValue = m_queue.front();
    return true;
  }


  template <typename T>
  void ConcurrentQueue<T>::UnsafeWake()
  {
    m_waitForMsgCondition.notify_one();
  }
}

#endif
