#ifndef FSLSIMPLEUI_BASE_SYSTEM_EVENT_EVENTLISTENERMANAGER_HPP
#define FSLSIMPLEUI_BASE_SYSTEM_EVENT_EVENTLISTENERMANAGER_HPP
/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Core.hpp>
#include <FslSimpleUI/Base/System/IEventListener.hpp>
#include <algorithm>
#include <deque>
#include <iterator>
#include <memory>

namespace Fsl::UI
{
  class EventListenerManager
  {
    std::deque<std::weak_ptr<IEventListener>> m_eventListeners;
    std::deque<std::weak_ptr<IEventListener>> m_eventListenersCache;
    bool m_inCall = false;
    bool m_gcRequested = false;
    bool m_cacheDirty = true;

  public:
    EventListenerManager();

    //! @brief Register a event listener
    void RegisterEventListener(const std::weak_ptr<IEventListener>& eventListener);
    //! @brief Unregister a event listener
    void UnregisterEventListener(const std::weak_ptr<IEventListener>& eventListener);


    template <typename TFunc>
    void Call(TFunc fnToCall)
    {
      if (m_inCall)
      {
        // If this exception is thrown one of our callbacks caused a 're-entry' into this object
        throw UsageErrorException("Can not perform two calls at the same time");
      }

      try
      {
        m_inCall = true;

        if (m_cacheDirty)
        {
          UpdateCache();
        }

        bool performGC = false;
        auto itr = m_eventListenersCache.begin();
        while (itr != m_eventListenersCache.end())
        {
          auto listener = itr->lock();
          if (listener)
          {
            fnToCall(listener);
          }
          else
          {
            performGC = true;
          }
          ++itr;
        }

        if (performGC)
        {
          ForceGC();
        }

        m_inCall = false;
      }
      catch (const std::exception&)
      {
        FSLLOG3_WARNING("eventListener threw exception");
        m_inCall = false;
        throw;
      }
    }

  private:
    //! @brief Ensure that the cache 'm_eventListenersCache' is a exact copy of 'm_eventListeners'
    void UpdateCache();

    void ForceGC();

    //! @brief Ensure all expired weak pointers in 'm_eventListeners' are removed.
    void PerformGC();
  };
}

#endif
