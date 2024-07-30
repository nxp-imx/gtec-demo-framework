#ifndef FSLSIMPLEUI_BASE_EVENT_WINDOWEVENTPOOL_HPP
#define FSLSIMPLEUI_BASE_EVENT_WINDOWEVENTPOOL_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Math/Pixel/PxVector2.hpp>
#include <FslBase/Time/MillisecondTickCount32.hpp>
#include <FslSimpleUI/Base/Event/EventTransactionState.hpp>
#include <deque>
#include <memory>

namespace Fsl
{
  struct PxPoint2;
  class ITag;

  namespace UI
  {
    class WindowEvent;
    class WindowInputClickEvent;
    class WindowMouseOverEvent;
    class WindowSelectEvent;
    class WindowContentChangedEvent;

    //! @brief A simple event object pool
    //! @note All events have to acquired and released to the pool.
    class WindowEventPool
    {
      std::deque<std::shared_ptr<WindowMouseOverEvent>> m_poolWindowMouseOverEvent;
      std::deque<std::shared_ptr<WindowInputClickEvent>> m_poolWindowInputClickEvent;
      std::deque<std::shared_ptr<WindowSelectEvent>> m_poolWindowSelectEvent;
      std::deque<std::shared_ptr<WindowContentChangedEvent>> m_poolWindowContentChangedEvent;

    public:
      WindowEventPool(const WindowEventPool&) = delete;
      WindowEventPool& operator=(const WindowEventPool&) = delete;

      WindowEventPool();
      ~WindowEventPool() noexcept;

      std::shared_ptr<WindowMouseOverEvent> AcquireWindowMouseOverEvent(const MillisecondTickCount32 timestamp, const int32_t sourceId,
                                                                        const int32_t sourceSubId, const EventTransactionState& state,
                                                                        const bool isRepeat, const PxPoint2& screenPositionPx);
      std::shared_ptr<WindowInputClickEvent> AcquireWindowInputClickEvent(const MillisecondTickCount32 timestamp, const int32_t sourceId,
                                                                          const int32_t sourceSubId, const EventTransactionState state,
                                                                          const bool isRepeat, const PxPoint2& screenPositionPx);
      std::shared_ptr<WindowSelectEvent> AcquireWindowSelectEvent(const uint32_t contentId);
      std::shared_ptr<WindowSelectEvent> AcquireWindowSelectEvent(const uint32_t contentId, const std::shared_ptr<ITag>& payload);
      std::shared_ptr<WindowContentChangedEvent> AcquireWindowContentChangedEvent(const uint32_t contentId);
      std::shared_ptr<WindowContentChangedEvent> AcquireWindowContentChangedEvent(const uint32_t contentId, const int32_t param1,
                                                                                  const int32_t param2);

      void Release(const std::shared_ptr<WindowEvent>& event) noexcept;

      void Release(const std::shared_ptr<WindowMouseOverEvent>& event) noexcept;
      void Release(const std::shared_ptr<WindowInputClickEvent>& event) noexcept;
      void Release(const std::shared_ptr<WindowSelectEvent>& event) noexcept;
      void Release(const std::shared_ptr<WindowContentChangedEvent>& event) noexcept;

    private:
      template <typename TEventType>
      void ReleaseToPool(std::deque<std::shared_ptr<TEventType>>& rPool, const std::shared_ptr<TEventType>& event,
                         const std::size_t maxCapacity) noexcept
      {
        if (!event)
        {
          return;
        }
        event->SYS_Destruct();

        if (rPool.size() < maxCapacity)
        {
          try
          {
            rPool.push_back(event);
          }
          catch (const std::exception&)
          {
            FSLLOG3_WARNING("Failed to push to pool, dropping object");
          }
        }
        else
        {
          FSLLOG3_WARNING("Pool capacity reached, dropping object");
        }
      }
    };
  }
}

#endif
