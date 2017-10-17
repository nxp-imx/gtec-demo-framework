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

#include <FslDemoHost/Base/Service/NativeWindowEvents/NativeWindowEventsService.hpp>
#include <FslBase/Exceptions.hpp>

namespace Fsl
{
  NativeWindowEventsService::NativeWindowEventsService(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)
    , m_listeners()
    , m_isLocked(false)
  {
  }


  void NativeWindowEventsService::Register(const std::weak_ptr<INativeWindowEventListener>& subscriber)
  {
    if (m_isLocked)
      throw UsageErrorException("Can not register during callback");

    // Force unregister to ensure a listener is only registered once!
    Unregister(subscriber);
    m_listeners.push_back(subscriber);
  }


  void NativeWindowEventsService::Unregister(const std::weak_ptr<INativeWindowEventListener>& subscriber)
  {
    if (m_isLocked)
      throw UsageErrorException("Can not register during callback");

    std::shared_ptr<INativeWindowEventListener> listenerToRemove = subscriber.lock();
    if (!listenerToRemove)
      return;

    EventListenerList::iterator itr = m_listeners.begin();
    while (itr != m_listeners.end())
    {
      const std::shared_ptr<INativeWindowEventListener> listener = itr->lock();
      if (!listener || listener == listenerToRemove)
        itr = m_listeners.erase(itr);
      else
        ++itr;
    }
  }


  void NativeWindowEventsService::SendEvent(const NativeWindowEvent& event)
  {
    try
    {
      m_isLocked = true;

      EventListenerList::iterator itr = m_listeners.begin();
      while (itr != m_listeners.end())
      {
        std::shared_ptr<INativeWindowEventListener> listener = itr->lock();
        if (listener)
        {
          listener->OnNativeWindowEvent(event);
          ++itr;
        }
        else
          itr = m_listeners.erase(itr);
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
