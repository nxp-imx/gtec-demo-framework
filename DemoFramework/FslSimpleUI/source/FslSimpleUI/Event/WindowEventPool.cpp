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

#include <FslSimpleUI/Event/WindowEventPool.hpp>
#include <FslSimpleUI/Event/EventTypeId.hpp>
#include <FslSimpleUI/Event/WindowContentChangedEvent.hpp>
#include <FslSimpleUI/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Event/WindowSelectEvent.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <cassert>

namespace Fsl
{
  namespace UI
  {
    namespace
    {
      const std::size_t MAX_CAPACITY = 1024;
      const std::size_t NUM_ENTRIES_TO_GROW = 8;

      template<typename T>
      void GrowPool(std::deque<std::shared_ptr<T> >& rDeque, const std::size_t entries)
      {
        for (std::size_t i = 0; i < entries; ++i)
          rDeque.push_back(std::make_shared<T>());
      }
    }


    WindowEventPool::WindowEventPool()
    {
    }


    WindowEventPool::~WindowEventPool()
    {
    }


    std::shared_ptr<WindowInputClickEvent> WindowEventPool::AcquireWindowInputClickEvent(const int32_t sourceId, const int32_t sourceSubId, const EventTransactionState state, const bool isRepeat, const Vector2& screenPosition)
    {
      if (m_poolWindowInputClickEvent.size() <= 0)
        GrowPool(m_poolWindowInputClickEvent, NUM_ENTRIES_TO_GROW);

      auto obj = m_poolWindowInputClickEvent.front();
      m_poolWindowInputClickEvent.pop_front();
      obj->SYS_Construct(sourceId, sourceSubId, state, isRepeat, screenPosition);
      return obj;
    }


    std::shared_ptr<WindowSelectEvent> WindowEventPool::AcquireWindowSelectEvent(const uint32_t contentId)
    {
      return AcquireWindowSelectEvent(contentId, std::shared_ptr<ITag>());
    }


    std::shared_ptr<WindowSelectEvent> WindowEventPool::AcquireWindowSelectEvent(const uint32_t contentId, const std::shared_ptr<ITag>& payload)
    {
      if (m_poolWindowSelectEvent.size() <= 0)
        GrowPool(m_poolWindowSelectEvent, NUM_ENTRIES_TO_GROW);

      auto obj = m_poolWindowSelectEvent.front();
      m_poolWindowSelectEvent.pop_front();
      obj->SYS_Construct(contentId, payload);
      return obj;
    }

    std::shared_ptr<WindowContentChangedEvent> WindowEventPool::AcquireWindowContentChangedEvent(const uint32_t contentId)
    {
      return AcquireWindowContentChangedEvent(contentId, 0, 0);
    }

    std::shared_ptr<WindowContentChangedEvent> WindowEventPool::AcquireWindowContentChangedEvent(const uint32_t contentId, const int32_t param1, const int32_t param2)
    {
      if (m_poolWindowContentChangedEvent.size() <= 0)
        GrowPool(m_poolWindowContentChangedEvent, NUM_ENTRIES_TO_GROW);

      auto obj = m_poolWindowContentChangedEvent.front();
      m_poolWindowContentChangedEvent.pop_front();
      obj->SYS_Construct(contentId, param1, param2);
      return obj;
    }


    void WindowEventPool::Release(const std::shared_ptr<WindowEvent>& event)
    {
      if (!event)
        return;

      switch (event->GetEventTypeId())
      {
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
        FSLLOG_ERROR("Unknown event type");
        break;
      }
    }


    void WindowEventPool::Release(const std::shared_ptr<WindowInputClickEvent>& event)
    {
      if (!event)
        return;
      event->SYS_Destruct();
      if (m_poolWindowInputClickEvent.size() < MAX_CAPACITY)
        m_poolWindowInputClickEvent.push_back(event);
      else
        FSLLOG_WARNING("Pool capacity reached for WindowInputClickEvent");
    }


    void WindowEventPool::Release(const std::shared_ptr<WindowSelectEvent>& event)
    {
      if (!event)
        return;
      event->SYS_Destruct();
      if (m_poolWindowSelectEvent.size() < MAX_CAPACITY)
        m_poolWindowSelectEvent.push_back(event);
      else
        FSLLOG_WARNING("Pool capacity reached for WindowSelectEvent");
    }


    void WindowEventPool::Release(const std::shared_ptr<WindowContentChangedEvent>& event)
    {
      if (!event)
        return;
      event->SYS_Destruct();

      if (m_poolWindowContentChangedEvent.size() < MAX_CAPACITY)
        m_poolWindowContentChangedEvent.push_back(event);
      else
        FSLLOG_WARNING("Pool capacity reached for WindowContentChangedEvent");
    }
  }
}
