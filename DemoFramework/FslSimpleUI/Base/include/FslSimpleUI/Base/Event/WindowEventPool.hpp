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

#include <FslBase/Noncopyable.hpp>
#include <FslBase/BasicTypes.hpp>
#include <FslSimpleUI/Base/Event/EventTransactionState.hpp>
#include <deque>
#include <memory>

namespace Fsl
{
  struct Vector2;
  class ITag;

  namespace UI
  {
    class WindowEvent;
    class WindowInputClickEvent;
    class WindowSelectEvent;
    class WindowContentChangedEvent;

    //! @brief A simple event object pool
    //! @note All events have to acquired and released to the pool.
    class WindowEventPool : private Noncopyable
    {
      std::deque<std::shared_ptr<WindowInputClickEvent> > m_poolWindowInputClickEvent;
      std::deque<std::shared_ptr<WindowSelectEvent> > m_poolWindowSelectEvent;
      std::deque<std::shared_ptr<WindowContentChangedEvent> > m_poolWindowContentChangedEvent;
    public:
      WindowEventPool();
      ~WindowEventPool();

      std::shared_ptr<WindowInputClickEvent> AcquireWindowInputClickEvent(const int32_t sourceId, const int32_t sourceSubId, const EventTransactionState state, const bool isRepeat, const Vector2& screenPosition);
      std::shared_ptr<WindowSelectEvent> AcquireWindowSelectEvent(const uint32_t contentId);
      std::shared_ptr<WindowSelectEvent> AcquireWindowSelectEvent(const uint32_t contentId, const std::shared_ptr<ITag>& payload);
      std::shared_ptr<WindowContentChangedEvent> AcquireWindowContentChangedEvent(const uint32_t contentId);
      std::shared_ptr<WindowContentChangedEvent> AcquireWindowContentChangedEvent(const uint32_t contentId, const int32_t param1, const int32_t param2);

      void Release(const std::shared_ptr<WindowEvent>& event);
      void Release(const std::shared_ptr<WindowInputClickEvent>& event);
      void Release(const std::shared_ptr<WindowSelectEvent>& event);
      void Release(const std::shared_ptr<WindowContentChangedEvent>& event);
    };
  }
}

#endif
