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

#include <FslSimpleUI/Base/Event/WindowTransactionEvent.hpp>

namespace Fsl
{
  namespace UI
  {
    int32_t WindowTransactionEvent::GetSourceId() const
    {
      return m_sourceId;
    }


    int32_t WindowTransactionEvent::GetSourceSubId() const
    {
      return m_sourceSubId;
    }


    EventTransactionState WindowTransactionEvent::GetState() const
    {
      return m_state;
    }


    bool WindowTransactionEvent::IsRepeat() const
    {
      return m_isRepeat;
    }


    WindowTransactionEvent::WindowTransactionEvent(const EventTypeId typeId, const EventDescription& eventDescription)
      : WindowEvent(typeId, eventDescription)
      , m_sourceId(0)
      , m_sourceSubId(0)
      , m_state(EventTransactionState::End)
      , m_isRepeat(false)
    {
    }


    void WindowTransactionEvent::SYS_DoConstruct(const int32_t sourceId, const int32_t sourceSubId, const EventTransactionState state,
                                                 const bool isRepeat)
    {
      WindowEvent::SYS_DoConstruct();
      m_sourceId = sourceId;
      m_sourceSubId = sourceSubId;
      m_state = state;
      m_isRepeat = isRepeat;
    }


    void WindowTransactionEvent::SYS_Destruct()
    {
      m_sourceId = 0;
      m_sourceSubId = 0;
      m_state = EventTransactionState::End;
      m_isRepeat = false;
      WindowEvent::SYS_Destruct();
    }
  }
}
