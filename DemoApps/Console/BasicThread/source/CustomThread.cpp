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

#include "CustomThread.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <fmt/std.h>    // to allow us to print a std::thread::id
#include <chrono>
#include <thread>
#include <utility>

namespace Fsl
{
  CustomThread::CustomThread(std::shared_ptr<ConcurrentQueue<int32_t>> threadSafeQueue)
    : m_threadSafeQueue(std::move(threadSafeQueue))
  {
    const auto threadId = std::this_thread::get_id();
    FSLLOG3_INFO("CustomThread: Created on thread: (ThreadId: {})", threadId);
  }


  CustomThread::~CustomThread()
  {
    FSLLOG3_INFO("CustomThread: Destroyed on thread: (ThreadId: {})", std::this_thread::get_id());
  }


  bool CustomThread::Run(const CustomCancellationToken& cancellationToken)
  {
    using namespace std::chrono_literals;

    const auto threadId = std::this_thread::get_id();

    int32_t counter = 42;
    while (!cancellationToken.IsCanceled())
    {
      FSLLOG3_INFO("CustomThread: Doing work (ThreadId: {})", threadId);
      m_threadSafeQueue->Enqueue(counter);
      ++counter;
      std::this_thread::sleep_for(250ms);
    }
    // Post a quit message
    m_threadSafeQueue->Enqueue(1337);

    // Simulate a bit of additional delay reacting to the cancel
    std::this_thread::sleep_for(100ms);
    FSLLOG3_INFO("CustomThread: Completed (ThreadId: {})", threadId);
    return true;
  }
}
