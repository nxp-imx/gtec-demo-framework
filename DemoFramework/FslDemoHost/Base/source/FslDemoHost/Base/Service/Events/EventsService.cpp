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

#include <FslBase/Exceptions.hpp>
#include <FslDemoHost/Base/Service/Events/EventsService.hpp>
#include <cassert>

namespace Fsl
{
  EventsService::EventsService(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)
    , m_pEventQueue(&m_eventQueue1)
    , m_isLocked(false)
  {
  }


  void EventsService::Update()
  {
    ThreadLocalService::Update();

    assert(m_pEventQueue != nullptr);

    int loopCount = 0;
    const int maxLoopCount = 7;
    // We loop around a few times to allow events to post events that can be acted upon almost immediately
    // However we have a max limit to prevent it from looping indefinitely
    while (!m_pEventQueue->empty() && loopCount < maxLoopCount)
    {
      EventQueue& eventQueue = *m_pEventQueue;

      // Swap the event queue
      m_pEventQueue = (m_pEventQueue == &m_eventQueue1 ? &m_eventQueue2 : &m_eventQueue1);
      while (!eventQueue.empty())
      {
        const EventRecord eventRecord = eventQueue.front();
        eventQueue.pop();

        SendEvent(eventRecord);
      }
      ++loopCount;
    }
  }


  void EventsService::Register(const std::weak_ptr<IEventListener>& subscriber)
  {
    if (m_isLocked)
    {
      throw UsageErrorException("Can not register during callbacks");
    }

    // Force unregister to ensure a listener is only registered once!
    Unregister(subscriber);
    m_listeners.push_back(subscriber);
  }


  void EventsService::Unregister(const std::weak_ptr<IEventListener>& subscriber)
  {
    if (m_isLocked)
    {
      throw UsageErrorException("Can not unregister during callbacks");
    }

    std::shared_ptr<IEventListener> listenerToRemove = subscriber.lock();
    if (!listenerToRemove)
    {
      return;
    }

    auto itr = m_listeners.begin();
    while (itr != m_listeners.end())
    {
      const std::shared_ptr<IEventListener> listener = itr->lock();
      if (!listener || listener == listenerToRemove)
      {
        itr = m_listeners.erase(itr);
      }
      else
      {
        ++itr;
      }
    }
  }


  void EventsService::Post(const std::shared_ptr<IEvent>& event)
  {
    if (!event)
    {
      throw std::invalid_argument("Event can not be null");
    }

    assert(m_pEventQueue != nullptr);
    m_pEventQueue->push(EventRecord(event));
  }


  void EventsService::Post(const BasicEvent& event)
  {
    assert(m_pEventQueue != nullptr);
    m_pEventQueue->push(EventRecord(event));
  }


  void EventsService::SendEvent(const EventRecord& eventRecord)
  {
    try
    {
      m_isLocked = true;

      if (eventRecord.ComplexEvent)
      {
        // Send a complex event
        IEvent* const pEvent = eventRecord.ComplexEvent.get();
        assert(pEvent != nullptr);

        auto itr = m_listeners.begin();
        while (itr != m_listeners.end())
        {
          std::shared_ptr<IEventListener> listener = itr->lock();
          if (listener)
          {
            listener->OnEvent(pEvent);
            ++itr;
          }
          else
          {
            itr = m_listeners.erase(itr);
          }
        }
      }
      else
      {
        BasicEvent event = eventRecord.Event;

        // Send a basic event
        auto itr = m_listeners.begin();
        while (itr != m_listeners.end())
        {
          std::shared_ptr<IEventListener> listener = itr->lock();
          if (listener)
          {
            listener->OnEvent(&event);
            ++itr;
          }
          else
          {
            itr = m_listeners.erase(itr);
          }
        }
      }


      m_isLocked = false;
    }
    catch (...)
    {
      m_isLocked = false;
      throw;
    }
  }
}
