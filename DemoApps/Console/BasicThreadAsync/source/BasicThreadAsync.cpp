/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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

#include "BasicThreadAsync.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <fmt/std.h>    // to allow us to print a std::thread::id
#include <chrono>
#include <thread>
#include "CustomThread.hpp"

namespace Fsl
{
  namespace
  {
    bool OnWorkerThread(std::shared_ptr<ConcurrentQueue<int32_t>> threadSafeQueue, const std::shared_ptr<CustomCancellationToken>& cancellationToken)
    {
      const auto threadId = std::this_thread::get_id();
      FSLLOG3_INFO("OnWorkerThread: (ThreadId: {})", threadId);
      if (!threadSafeQueue)
      {
        throw std::invalid_argument("threadSafeQueue can not be nullptr");
      }
      if (!cancellationToken)
      {
        throw std::invalid_argument("cancellationToken can not be nullptr");
      }

      CustomThread work(std::move(threadSafeQueue));
      const bool result = work.Run(*cancellationToken);
      FSLLOG3_INFO("OnWorkerThread completed with: {} (ThreadId: {})", result, threadId);
      return result;
    }
  }


  BasicThreadAsync::BasicThreadAsync(const DemoAppConfig& config)
    : DemoAppConsoleMinimal(config)
    , m_incomingQueue(std::make_shared<ConcurrentQueue<int32_t>>())
    , m_cancellationToken(std::make_shared<CustomCancellationToken>())
  {
    m_runFuture = std::async(OnWorkerThread, m_incomingQueue, m_cancellationToken);
  }

  BasicThreadAsync::~BasicThreadAsync()
  {
    const auto threadId = std::this_thread::get_id();
    using namespace std::chrono_literals;

    if (m_cancellationToken)
    {
      FSLLOG3_INFO("App: Requesting shutdown (ThreadId: {})", threadId);
      m_cancellationToken->Cancel();
    }

    try
    {
      FSLLOG3_INFO("App: Waiting for shutdown (ThreadId: {})", threadId);
      if (m_runFuture.wait_for(10s) == std::future_status::ready)
      {
        const bool result = m_runFuture.get();
        FSLLOG3_INFO("App: Custom thread shutdown result: {} (ThreadId: {})", result, threadId);

        {    // Pump the last messages (will only be used in case a exception occurs)
          FSLLOG3_INFO("App: Checking for any remaining messsages (ThreadId: {})", threadId);
          int32_t queueResult = 0;
          if (m_incomingQueue->TryDequeue(queueResult))
          {
            FSLLOG3_INFO("App: Got message: {} (ThreadId: {})", queueResult, threadId);
          }
        }
      }
      else
      {
        FSLLOG3_ERROR("App: Wait for shutdown result failed (ThreadId: {})", threadId);
      }
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_INFO("App: Custom thread failed with: {} (ThreadId: {})", ex.what(), threadId);
    }
  }


  void BasicThreadAsync::Run()
  {
    using namespace std::chrono_literals;

    const auto threadId = std::this_thread::get_id();

    FSLLOG3_INFO("App: Running (ThreadId: {})", threadId);

    const std::chrono::duration totalTime = 2s;
    auto currentTime = std::chrono::system_clock::now();
    auto endTime = currentTime + totalTime;

    do
    {
      int32_t queueResult = 0;
      std::chrono::duration timeLeft = endTime - currentTime;
      FSLLOG3_INFO("App: Waiting for message (ThreadId: {})", threadId);
      if (m_incomingQueue->TryDequeueWait(queueResult, std::chrono::duration_cast<std::chrono::milliseconds>(timeLeft)))
      {
        FSLLOG3_INFO("App: Got message: {} (ThreadId: {})", queueResult, threadId);
      }

      currentTime = std::chrono::system_clock::now();
    } while (currentTime < endTime);

    // Request the thread to shutdown
    FSLLOG3_INFO("App: Time limit hit, requesting shutdown (ThreadId: {})", threadId);
    m_cancellationToken->Cancel();

    FSLLOG3_INFO("App: Waiting for run result to be set (ThreadId: {})", threadId);
    if (m_runFuture.wait_for(10s) == std::future_status::ready)
    {
      // Custom thread has been shutdown so pump the last messages
      FSLLOG3_INFO("App: Checking for any remaining messsages (ThreadId: {})", threadId);
      int32_t queueResult = 0;
      if (m_incomingQueue->TryDequeue(queueResult))
      {
        FSLLOG3_INFO("App: Got message: {} (ThreadId: {})", queueResult, threadId);
      }
    }
    else
    {
      FSLLOG3_ERROR("App: Wait for shutdown reault failed (ThreadId: {})", threadId);
    }

    FSLLOG3_INFO("App: Running (ThreadId: {})", threadId);
  }
}
