#ifndef FSLBASE_COLLECTIONS_CONCURRENT_CONCURRENTQUEUE_FWD_HPP
#define FSLBASE_COLLECTIONS_CONCURRENT_CONCURRENTQUEUE_FWD_HPP
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

#include <FslBase/Span/Span.hpp>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>

namespace Fsl
{
  template <typename T>
  class ConcurrentQueue
  {
    std::mutex m_mutex;
    std::condition_variable m_waitForMsgCondition;
    std::queue<T> m_queue;

  public:
    ConcurrentQueue(const ConcurrentQueue&) = delete;
    ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;
    ConcurrentQueue() = default;

    using value_type = T;

    //! @brief Clear all elements from the queue
    void Clear();

    //! @brief Add element to queue
    void Enqueue(const T& value);

    //! @brief Remove element form queue
    T Dequeue();

    //! @brief Tries to remove and return the element at the beginning of the concurrent queue.
    //! @return true on success, false if unsuccessful. When false rValue will be set to T().
    bool TryDequeue(T& rValue);

    bool TryDequeueWait(T& rValue, const std::chrono::milliseconds& duration);

    //! @brief Tries to return a element from the beginning of the queue without removing it.
    //! @return true on success, false if unsuccessful. When false rValue will be set to T().
    bool TryPeek(T& rValue);

    //! @brief Swap the internal queue with the supplied queue
    //! @note Use this if you send a lot of big messages
    void SwapQueue(std::queue<T>& rSwapQueue);

    //! @brief Extract all pending messages into the supplied queue
    //! @note  This will probably be slower than just using swap queue
    void Extract(std::queue<T>& rDstQueue);

    //! @brief Extract all pending messages that can fit into the supplied span
    //! @return the number of entries written to the span.
    std::size_t Extract(Span<T> dstSpan);

  private:
    void UnsafeWake();
  };
}

#endif
