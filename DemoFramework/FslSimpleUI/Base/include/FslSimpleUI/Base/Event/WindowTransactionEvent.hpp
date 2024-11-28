#ifndef FSLSIMPLEUI_BASE_EVENT_WINDOWTRANSACTIONEVENT_HPP
#define FSLSIMPLEUI_BASE_EVENT_WINDOWTRANSACTIONEVENT_HPP
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

#include <FslBase/Time/MillisecondTickCount32.hpp>
#include <FslSimpleUI/Base/Event/EventTransactionState.hpp>
#include <FslSimpleUI/Base/Event/WindowEvent.hpp>

namespace Fsl::UI
{
  namespace Internal
  {
    class ScopedWindowTransactionEventPatch;
  }

  class WindowTransactionEvent : public WindowEvent
  {
    MillisecondTickCount32 m_timestamp;
    int32_t m_sourceId{0};
    int32_t m_sourceSubId{0};
    EventTransactionState m_state{EventTransactionState::End};
    bool m_isRepeat{false};
    uint32_t m_interceptionCount{0};
    bool m_allowIntercept{false};

  public:
    MillisecondTickCount32 GetTimestamp() const noexcept
    {
      return m_timestamp;
    }

    int32_t GetSourceId() const noexcept
    {
      return m_sourceId;
    }

    int32_t GetSourceSubId() const noexcept
    {
      return m_sourceSubId;
    }

    EventTransactionState GetState() const noexcept
    {
      return m_state;
    }

    bool IsRepeat() const noexcept
    {
      return m_isRepeat;
    }

    bool GetAllowIntercept() const noexcept
    {
      return m_allowIntercept;
    }

    bool Intercepted() const noexcept
    {
      return m_interceptionCount > 0;
    }

    //! @brief Intercept this transaction event, successfully intercepting clears the status (so it goes back to unhandled)
    //!        This causes all children to receive a 'Cancel event' and the interceptor and its parent will continue the transaction.
    //!        A good candidate for using this is a scroll view when it detects the view being dragged instead of clicked.
    bool Intercept();


    //! @brief Internal and test use only
    uint32_t GetInterceptionCount() const noexcept
    {
      return m_interceptionCount;
    }

    //! @brief Internal and test use only
    // NOLINTNEXTLINE(readability-identifier-naming)
    bool SYS_SetAllowIntercept(bool allowIntercept)
    {
      bool changed = m_allowIntercept != allowIntercept;
      m_allowIntercept = allowIntercept;
      return changed;
    }

  protected:
    WindowTransactionEvent(const EventTypeId typeId, const EventDescription& eventDescription) noexcept;

    // NOLINTNEXTLINE(readability-identifier-naming)
    void SYS_DoConstruct(const MillisecondTickCount32 timestamp, const int32_t sourceId, const int32_t sourceSubId, const EventTransactionState state,
                         const bool isRepeat) noexcept;

    // NOLINTNEXTLINE(readability-identifier-naming)
    void SYS_Destruct() noexcept override;

  private:
    // NOLINTNEXTLINE(readability-identifier-naming)
    void SYS_PatchInternals(EventTransactionState transactionState, bool isRepeat, EventHandlingStatus handlingStatus, bool allowIntercept);

    friend class Internal::ScopedWindowTransactionEventPatch;
  };
}

#endif
