#ifndef FSLDEMOHOST_BASE_SERVICE_CONTENTMONITOR_CONTENTMONITORTHREAD_HPP
#define FSLDEMOHOST_BASE_SERVICE_CONTENTMONITOR_CONTENTMONITORTHREAD_HPP
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

#include <FslBase/Collections/Concurrent/ConcurrentQueue_fwd.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/System/Platform/PlatformThread.hpp>
#include <FslDemoHost/Base/Service/ContentMonitor/ContentMonitorResultCommand.hpp>

namespace Fsl
{
  class ContentMonitorThread
  {
    // We actually just need a cancellation token, but as we don't have a nice implementation we just use the queue for now
    std::shared_ptr<ConcurrentQueue<bool>> m_queue;
    std::shared_ptr<IThreadContext> m_threadTask;
    PlatformThread m_thread;

  public:
    ContentMonitorThread(const std::weak_ptr<ConcurrentQueue<ContentMonitorResultCommand>>& fromQueue, const IO::Path& contentPath);
    ~ContentMonitorThread() noexcept;
  };
}

#endif
