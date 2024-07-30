#ifndef FSLSIMPLEUI_BASE_EVENT_WINDOWEVENT_HPP
#define FSLSIMPLEUI_BASE_EVENT_WINDOWEVENT_HPP
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

#include <FslSimpleUI/Base/Event/EventDescription.hpp>
#include <FslSimpleUI/Base/Event/EventTypeId.hpp>
#include <memory>

namespace Fsl::UI
{
  class IWindowId;

  class WindowEvent
  {
    // NOLINTNEXTLINE(readability-identifier-naming)
    const EventTypeId m_eventTypeId;
    // NOLINTNEXTLINE(readability-identifier-naming)
    const EventDescription m_eventDescription;

    std::shared_ptr<IWindowId> m_originalSource;
    std::shared_ptr<IWindowId> m_source;
    bool m_isHandled;
    bool m_isInitialized;

  public:
    WindowEvent(const WindowEvent&) = delete;
    WindowEvent& operator=(const WindowEvent&) = delete;

    virtual ~WindowEvent() noexcept;

    bool IsOriginalSource(const IWindowId* const pWindowId) const noexcept;
    bool IsSource(const IWindowId* const pWindowId) const noexcept;

    //! @brief Get the original source of the event.
    const std::shared_ptr<IWindowId>& GetOriginalSource() const noexcept;

    //! @brief Get the source of the event.
    const std::shared_ptr<IWindowId>& GetSource() const noexcept;

    //! @brief Check if this event has been handled.
    bool IsHandled() const noexcept;

    //! @brief mark the event as handled.
    void Handled() noexcept;

    //! @brief Get the event type id
    EventTypeId GetEventTypeId() const noexcept
    {
      return m_eventTypeId;
    }

    //! @brief Get the event description.
    EventDescription GetDescription() const noexcept
    {
      return m_eventDescription;
    }


    // NOLINTNEXTLINE(readability-identifier-naming)
    void SYS_SetSource(const std::shared_ptr<IWindowId>& value) noexcept;
    // NOLINTNEXTLINE(readability-identifier-naming)
    void SYS_SetOriginalSource(const std::shared_ptr<IWindowId>& value) noexcept;

  protected:
    WindowEvent(const EventTypeId typeId, EventDescription eventDescription) noexcept;

    // NOLINTNEXTLINE(readability-identifier-naming)
    void SYS_DoConstruct() noexcept;

    // NOLINTNEXTLINE(readability-identifier-naming)
    virtual void SYS_Destruct() noexcept;

    bool IsDisposed() const noexcept
    {
      return !m_isInitialized;
    }
  };
}

#endif
