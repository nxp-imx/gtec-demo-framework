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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslSimpleUI/Base/Event/EventTypeId.hpp>
#include <FslSimpleUI/Base/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowEventPool.hpp>
#include <FslSimpleUI/Base/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowMouseOverEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <cassert>

namespace Fsl::UI
{
  namespace
  {
    constexpr std::size_t MaxCapacity = 1024;
    constexpr std::size_t NumEntriesToGrow = 8;

    template <typename T>
    void GrowPool(std::deque<std::shared_ptr<T>>& rDeque, const std::size_t entries)
    {
      for (std::size_t i = 0; i < entries; ++i)
      {
        rDeque.push_back(std::make_shared<T>());
      }
    }

  }


  WindowEventPool::WindowEventPool()
  {
    GrowPool(m_poolWindowMouseOverEvent, NumEntriesToGrow);
    GrowPool(m_poolWindowInputClickEvent, NumEntriesToGrow);
    GrowPool(m_poolWindowSelectEvent, NumEntriesToGrow);
    GrowPool(m_poolWindowContentChangedEvent, NumEntriesToGrow);
  }


  WindowEventPool::~WindowEventPool() noexcept = default;


  std::shared_ptr<WindowMouseOverEvent> WindowEventPool::AcquireWindowMouseOverEvent(const MillisecondTickCount32 timestamp, const int32_t sourceId,
                                                                                     const int32_t sourceSubId, const EventTransactionState& state,
                                                                                     const bool isRepeat, const PxPoint2& screenPositionPx)
  {
    if (m_poolWindowMouseOverEvent.empty())
    {
      GrowPool(m_poolWindowMouseOverEvent, NumEntriesToGrow);
    }

    auto obj = m_poolWindowMouseOverEvent.front();
    m_poolWindowMouseOverEvent.pop_front();
    obj->SYS_Construct(timestamp, sourceId, sourceSubId, state, isRepeat, screenPositionPx);
    return obj;
  }


  std::shared_ptr<WindowInputClickEvent> WindowEventPool::AcquireWindowInputClickEvent(const MillisecondTickCount32 timestamp, const int32_t sourceId,
                                                                                       const int32_t sourceSubId, const EventTransactionState state,
                                                                                       const bool isRepeat, const PxPoint2& screenPositionPx)
  {
    if (m_poolWindowInputClickEvent.empty())
    {
      GrowPool(m_poolWindowInputClickEvent, NumEntriesToGrow);
    }

    auto obj = m_poolWindowInputClickEvent.front();
    m_poolWindowInputClickEvent.pop_front();
    obj->SYS_Construct(timestamp, sourceId, sourceSubId, state, isRepeat, screenPositionPx);
    return obj;
  }


  std::shared_ptr<WindowSelectEvent> WindowEventPool::AcquireWindowSelectEvent(const uint32_t contentId)
  {
    return AcquireWindowSelectEvent(contentId, std::shared_ptr<ITag>());
  }


  std::shared_ptr<WindowSelectEvent> WindowEventPool::AcquireWindowSelectEvent(const uint32_t contentId, const std::shared_ptr<ITag>& payload)
  {
    if (m_poolWindowSelectEvent.empty())
    {
      GrowPool(m_poolWindowSelectEvent, NumEntriesToGrow);
    }

    auto obj = m_poolWindowSelectEvent.front();
    m_poolWindowSelectEvent.pop_front();
    obj->SYS_Construct(contentId, payload);
    return obj;
  }

  std::shared_ptr<WindowContentChangedEvent> WindowEventPool::AcquireWindowContentChangedEvent(const uint32_t contentId)
  {
    return AcquireWindowContentChangedEvent(contentId, 0, 0);
  }

  std::shared_ptr<WindowContentChangedEvent> WindowEventPool::AcquireWindowContentChangedEvent(const uint32_t contentId, const int32_t param1,
                                                                                               const int32_t param2)
  {
    if (m_poolWindowContentChangedEvent.empty())
    {
      GrowPool(m_poolWindowContentChangedEvent, NumEntriesToGrow);
    }

    auto obj = m_poolWindowContentChangedEvent.front();
    m_poolWindowContentChangedEvent.pop_front();
    obj->SYS_Construct(contentId, param1, param2);
    return obj;
  }


  void WindowEventPool::Release(const std::shared_ptr<WindowEvent>& event) noexcept
  {
    if (!event)
    {
      return;
    }

    switch (event->GetEventTypeId())
    {
    case EventTypeId::MouseOver:
      Release(std::dynamic_pointer_cast<WindowMouseOverEvent>(event));
      break;
    case EventTypeId::InputClick:
      Release(std::dynamic_pointer_cast<WindowInputClickEvent>(event));
      break;
    case EventTypeId::Select:
      Release(std::dynamic_pointer_cast<WindowSelectEvent>(event));
      break;
    case EventTypeId::ContentChanged:
      Release(std::dynamic_pointer_cast<WindowContentChangedEvent>(event));
      break;
    default:
      FSLLOG3_ERROR("Unknown event type");
      break;
    }
  }


  void WindowEventPool::Release(const std::shared_ptr<WindowMouseOverEvent>& event) noexcept
  {
    ReleaseToPool(m_poolWindowMouseOverEvent, event, MaxCapacity);
  }

  void WindowEventPool::Release(const std::shared_ptr<WindowInputClickEvent>& event) noexcept
  {
    ReleaseToPool(m_poolWindowInputClickEvent, event, MaxCapacity);
  }

  void WindowEventPool::Release(const std::shared_ptr<WindowSelectEvent>& event) noexcept
  {
    ReleaseToPool(m_poolWindowSelectEvent, event, MaxCapacity);
  }

  void WindowEventPool::Release(const std::shared_ptr<WindowContentChangedEvent>& event) noexcept
  {
    ReleaseToPool(m_poolWindowContentChangedEvent, event, MaxCapacity);
  }
}
