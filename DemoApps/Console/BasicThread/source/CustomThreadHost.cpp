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

#include "CustomThreadHost.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <fmt/std.h>    // to allow us to print a std::thread::id
#include <chrono>
#include <utility>
#include "CustomThread.hpp"

namespace Fsl
{
  namespace
  {
    void OnThreadRun(const std::shared_ptr<CustomCancellationToken>& cancellationTokenSource, std::promise<bool> promise,
                     std::shared_ptr<ConcurrentQueue<int32_t>> threadSafeQueue)
    {
      if (!threadSafeQueue)
      {
        throw std::invalid_argument("Can not be null");
      }

      const auto threadId = std::this_thread::get_id();
      FSLLOG3_INFO("CustomThreadHost: OnThreadRun: (ThreadId: {})", threadId);

      // Create the custom thread object
      try
      {
        CustomThread custom(std::move(threadSafeQueue));

        bool result = custom.Run(*cancellationTokenSource);

        FSLLOG3_INFO("CustomThreadHost: OnThreadRun completed with: {} (ThreadId: {})", result, threadId);
        promise.set_value(result);
      }
      catch (const std::exception& ex)
      {
        FSLLOG3_INFO("CustomThreadHost: OnThreadRun failed with exception: {} (ThreadId: {})", ex.what(), threadId);
        promise.set_exception(std::current_exception());
      }
    }
  }

  CustomThreadHost::CustomThreadHost(std::shared_ptr<ConcurrentQueue<int32_t>> threadSafeQueue)
    : m_cancellationTokenSource(std::make_shared<CustomCancellationToken>())
  {
    if (!threadSafeQueue)
    {
      throw std::invalid_argument("Can not be null");
    }

    const auto threadId = std::this_thread::get_id();
    FSLLOG3_INFO("CustomThreadHost: created on thread: (ThreadId: {})", threadId);
    auto promise = std::promise<bool>();
    m_runFuture = promise.get_future();
    m_thread = std::thread(OnThreadRun, m_cancellationTokenSource, std::move(promise), std::move(threadSafeQueue));
  }


  CustomThreadHost::~CustomThreadHost()
  {
    FSLLOG3_INFO("CustomThreadHost: destroyed on thread: (ThreadId: {})", std::this_thread::get_id());
  }


  const std::future<bool>& CustomThreadHost::GetRunResult() const
  {
    return m_runFuture;
  }

  void CustomThreadHost::RequestShutdown()
  {
    if (!m_shutdownRequested)
    {
      FSLLOG3_INFO("CustomThreadHost: Setting cancellation token (ThreadId: {})", std::this_thread::get_id());
      m_cancellationTokenSource->Cancel();
      m_shutdownRequested = true;
    }
  }

  bool CustomThreadHost::TryWaitForShutdownResult()
  {
    using namespace std::chrono_literals;
    return m_runFuture.wait_for(10s) == std::future_status::ready;
  }

  bool CustomThreadHost::Shutdown()
  {
    const auto threadId = std::this_thread::get_id();

    RequestShutdown();

    FSLLOG3_INFO("CustomThreadHost: Waiting for thread shutdown result (ThreadId: {})", threadId);
    if (!TryWaitForShutdownResult())
    {
      FSLLOG3_ERROR("CustomThreadHost: Wait for thread result timed out (ThreadId: {})", threadId);
      throw InternalErrorException("Thread timed out");
    }

    FSLLOG3_INFO("CustomThreadHost: Waiting for thread to shutdown (ThreadId: {})", threadId);
    m_thread.join();
    FSLLOG3_INFO("CustomThreadHost: Thread shutdown (ThreadId: {})", threadId);
    return m_runFuture.get();
  }
}
