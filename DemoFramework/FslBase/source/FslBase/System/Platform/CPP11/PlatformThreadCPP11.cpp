#if !defined(FSLBASE_THREAD_BACKEND_PTHREAD) && !defined(FSLBASE_THREAD_BACKEND_NOT_SUPPORTED)
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslBase/System/Platform/PlatformThread.hpp>
#include <FslBase/Log/Log.hpp>
#include <thread>
#include <utility>

namespace Fsl
{
  class PlatformThreadImpl : public std::thread
  {
  public:
    PlatformThreadImpl(const std::function<void(const std::shared_ptr<IThreadContext>)>& fnRun, const std::shared_ptr<IThreadContext>& threadContext)
      : std::thread(OnThreadRunning, fnRun, threadContext)
    {
    }


    static void OnThreadRunning(const std::function<void(const std::shared_ptr<IThreadContext>)>& fnRun,
                                const std::shared_ptr<IThreadContext>& threadContext)
    {
      fnRun(threadContext);
    }
  };


  PlatformThread::PlatformThread(const std::function<void(const std::shared_ptr<IThreadContext>)>& fnRun,
                                 const std::shared_ptr<IThreadContext>& threadContext)
    : m_impl(std::make_shared<PlatformThreadImpl>(fnRun, threadContext))
  {
  }


  PlatformThread::~PlatformThread()
  {
    Join();
  }


  void PlatformThread::Join()
  {
    if (m_impl)
    {
      m_impl->join();
      m_impl.reset();
    }
  }


  void PlatformThread::SleepMilliseconds(const uint32_t milliseconds)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
  }
}

#endif
