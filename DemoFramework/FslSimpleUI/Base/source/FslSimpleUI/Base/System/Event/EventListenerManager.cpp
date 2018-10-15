/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include "EventListenerManager.hpp"
#include <FslBase/Exceptions.hpp>
#include <algorithm>
#include <cassert>

namespace Fsl
{
  namespace UI
  {
    EventListenerManager::EventListenerManager() = default;

    void EventListenerManager::RegisterEventListener(const std::weak_ptr<IEventListener>& eventListener)
    {
      m_eventListeners.push_back(eventListener);
      m_cacheDirty = true;
    }

    void EventListenerManager::UnregisterEventListener(const std::weak_ptr<IEventListener>& eventListener)
    {
      auto listener = eventListener.lock();
      // If the pointer has become invalid we just ignore the request as the next broadcast will remove the entry
      if (!listener)
      {
        ForceGC();
        return;
      }

      auto itr = std::find_if(m_eventListeners.begin(), m_eventListeners.end(),
                              [listener](const std::weak_ptr<IEventListener>& record) { return record.lock() == listener; });
      if (itr != m_eventListeners.end())
      {
        m_eventListeners.erase(itr);
        m_cacheDirty = true;
      }
    }

    void EventListenerManager::UpdateCache()
    {
      if (!m_cacheDirty)
      {
        return;
      }

      m_eventListenersCache.clear();
      std::copy(m_eventListeners.begin(), m_eventListeners.end(), std::back_inserter(m_eventListenersCache));
      m_cacheDirty = false;
    }

    void EventListenerManager::ForceGC()
    {
      m_gcRequested = true;
      PerformGC();
    }

    void EventListenerManager::PerformGC()
    {
      if (!m_gcRequested)
      {
        return;
      }

      m_cacheDirty = true;

      auto itr = m_eventListeners.begin();
      while (itr != m_eventListeners.end())
      {
        if (itr->expired())
        {
          itr = m_eventListeners.erase(itr);
        }
        else
        {
          ++itr;
        }
      }
    }
  }
}
