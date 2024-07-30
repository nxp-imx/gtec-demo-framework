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
  class WindowTransactionEvent : public WindowEvent
  {
    MillisecondTickCount32 m_timestamp;
    int32_t m_sourceId;
    int32_t m_sourceSubId;
    EventTransactionState m_state;
    bool m_isRepeat;

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

  protected:
    WindowTransactionEvent(const EventTypeId typeId, const EventDescription& eventDescription) noexcept;
    void SYS_DoConstruct(const MillisecondTickCount32 timestamp, const int32_t sourceId, const int32_t sourceSubId, const EventTransactionState state,
                         const bool isRepeat) noexcept;
    void SYS_Destruct() noexcept override;
  };
}

#endif
