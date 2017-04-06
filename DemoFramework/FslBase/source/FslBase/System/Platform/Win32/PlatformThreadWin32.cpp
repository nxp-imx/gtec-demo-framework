#ifdef _WIN32
/****************************************************************************************************************************************************
* Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include <FslBase/System/Platform/PlatformThread.hpp>
#include <FslBase/Log/Log.hpp>
#include <windows.h>

namespace Fsl
{
  namespace
  {
    struct ObjectThreadContext
    {
      std::weak_ptr<PlatformThreadImpl> Object;

      ObjectThreadContext(const std::weak_ptr<PlatformThreadImpl>& object)
        : Object(object)
      {
      }
    };
  }

  class PlatformThreadImpl
  {
    std::function<void(const std::shared_ptr<IThreadContext>)> m_fnRun;
    std::shared_ptr<IThreadContext> m_threadContext;
    HANDLE m_hThread;
    DWORD m_threadId;
  public:
    PlatformThreadImpl(const std::function<void(const std::shared_ptr<IThreadContext>)>& fnRun, const std::shared_ptr<IThreadContext>& threadContext)
      : m_fnRun(fnRun)
      , m_threadContext(threadContext)
      , m_hThread(nullptr)
      , m_threadId(0)
    {
    }

    void Start(const std::weak_ptr<PlatformThreadImpl>& weakThisObject)
    {
      ObjectThreadContext* pThreadContext = new ObjectThreadContext(weakThisObject);

      m_hThread = CreateThread(
        nullptr,                          // default security attributes
        0,                                // use default stack size
        PlatformThreadImpl::RunOnThread,  // thread function name
        pThreadContext,                   // argument to thread function
        0,                                // use default creation flags
        &m_threadId);
    }


    void Join()
    {
      if (m_hThread != nullptr)
        WaitForSingleObject(m_hThread, INFINITE);
    }


    void OnThreadRunning()
    {
      m_fnRun(m_threadContext);
    }


    static DWORD __stdcall RunOnThread(LPVOID lpThreadParameter)
    {
      if (lpThreadParameter == nullptr)
        return 0;
      ObjectThreadContext* pThreadContext = static_cast<ObjectThreadContext*>(lpThreadParameter);
      ObjectThreadContext threadContext = *pThreadContext;
      delete pThreadContext;
      pThreadContext = nullptr;

      const std::shared_ptr<PlatformThreadImpl> context = threadContext.Object.lock();
      if (!context)
        return 0;
      context->OnThreadRunning();
      return 1;
    }
  };


  PlatformThread::PlatformThread(const std::function<void(const std::shared_ptr<IThreadContext>)>& fnRun, const std::shared_ptr<IThreadContext>& threadContext)
    : m_impl()
  {
    m_impl = std::make_shared<PlatformThreadImpl>(fnRun, threadContext);
    m_impl->Start(m_impl);
  }


  PlatformThread::~PlatformThread()
  {
  }


  void PlatformThread::Join()
  {
    m_impl->Join();
  }


  void PlatformThread::SleepMilliseconds(const uint32_t milliseconds)
  {
    Sleep(milliseconds);
  }

}

#endif
