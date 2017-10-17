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

#include "ContentMonitorThread.hpp"
#include <FslBase/Collections/Concurrent/ConcurrentQueue.hpp>
#include <FslBase/System/IThreadContext.hpp>
#include <FslBase/IO/Directory.hpp>
#include <FslBase/IO/PathDeque.hpp>
#include <FslBase/IO/PathWatcher.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/System/Threading/Thread.hpp>
#include <cassert>

namespace Fsl
{
  namespace
  {
    void AddCurrentContentFiles(IO::PathWatcher& rPathWatcher, const IO::Path& contentPath)
    {
      IO::PathDeque files;
      IO::Directory::GetFiles(files, contentPath, IO::SearchOptions::AllDirectories);

      for (IO::PathDeque::const_iterator itr = files.begin(); itr != files.end(); ++itr)
      {
        rPathWatcher.Add(**itr);
      }
    }


    class ContentMonitorThreadTask
    {
      std::shared_ptr<ConcurrentQueue<bool> > m_toQueue;
      std::weak_ptr<ConcurrentQueue<ContentMonitorResultCommand> > m_ownerQueue;
      IO::Path m_contentPath;
    public:
      ContentMonitorThreadTask(const std::shared_ptr<ConcurrentQueue<bool> >& toQueue, const std::weak_ptr<ConcurrentQueue<ContentMonitorResultCommand> >& ownerQueue, const IO::Path& contentPath)
        : m_toQueue(toQueue)
        , m_ownerQueue(ownerQueue)
        , m_contentPath(contentPath)
      {
      }

      void Run()
      {
        IO::PathWatcher pathWatcher;
        AddCurrentContentFiles(pathWatcher, m_contentPath);

        // As we use the queue as a cancellation token this means that if a message is in it we should shutdown
        bool queueEntry;
        bool stopNow = false;
        while (!stopNow && ! m_toQueue->TryDequeWait(queueEntry, std::chrono::milliseconds(1000 / 5)))
        {
          // A even better solution will use the 'platform dependent' filesystem change notification interfaces.
          // But that will also require more development time.

          if (pathWatcher.Check())
          {
            std::shared_ptr<ConcurrentQueue<ContentMonitorResultCommand> > ownerQueue = m_ownerQueue.lock();
            if (ownerQueue)
              ownerQueue->Enqueue(ContentMonitorResultCommand::Restart);
            else
              stopNow = true;
          }
        }
      }
    };


    struct LocalThreadContext : public IThreadContext
    {
      std::shared_ptr<ConcurrentQueue<bool> > Queue;
      std::weak_ptr<ConcurrentQueue<ContentMonitorResultCommand> > FromQueue;
      IO::Path ContentPath;

      LocalThreadContext(const std::shared_ptr<ConcurrentQueue<bool> >& queue, const std::weak_ptr<ConcurrentQueue<ContentMonitorResultCommand> >& fromQueue, const IO::Path& contentPath)
        : Queue(queue)
        , FromQueue(fromQueue)
        , ContentPath(contentPath)
      {
      }
    };

    void OnThreadStartMethod(const std::shared_ptr<IThreadContext>& threadContext)
    {
      try
      {
        auto localContext = std::dynamic_pointer_cast<LocalThreadContext>(threadContext);
        if (localContext)
        {
          auto obj = std::make_shared<ContentMonitorThreadTask>(localContext->Queue, localContext->FromQueue, localContext->ContentPath);
          obj->Run();
        }
      }
      catch (const std::exception& ex)
      {
        FSLLOG_ERROR("ContentMonitorThread threw exception: " << ex.what());
      }
    }
  }


  ContentMonitorThread::ContentMonitorThread(const std::weak_ptr<ConcurrentQueue<ContentMonitorResultCommand> >& fromQueue, const IO::Path& contentPath)
    : m_queue(std::make_shared<ConcurrentQueue<bool> >())
    , m_threadTask(std::make_shared<LocalThreadContext>(m_queue, fromQueue, contentPath))
    , m_thread(OnThreadStartMethod, m_threadTask)
  {
  }


  ContentMonitorThread::~ContentMonitorThread()
  {
    // Wait for the thread to shutdown
    m_queue->Enqueue(true);
    m_thread.Join();
  }

}
