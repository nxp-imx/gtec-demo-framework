#ifndef FSLSIMPLEUI_BASE_SYSTEM_EVENT_STATEEVENTSENDERHISTORY_HPP
#define FSLSIMPLEUI_BASE_SYSTEM_EVENT_STATEEVENTSENDERHISTORY_HPP
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

namespace Fsl::UI
{
  class StateEventSenderHistory
  {
    int32_t m_lastSourceId{0};
    int32_t m_lastSourceSubId{0};
    bool m_useHistory{false};
    bool m_historyCompleted{false};
    bool m_locked{false};
    bool m_isReceiverDead{false};

  public:
    StateEventSenderHistory() = default;


    void Begin(const int32_t sourceId, const int32_t sourceSubId)
    {
      if (!m_locked)
      {
        m_lastSourceId = sourceId;
        m_lastSourceSubId = sourceSubId;
        m_useHistory = true;
        m_historyCompleted = false;
        m_isReceiverDead = false;
      }
    }

    void End()
    {
      if (!m_locked)
      {
        m_lastSourceId = 0;
        m_useHistory = false;
        m_historyCompleted = true;
      }
    }

    void Lock()
    {
      m_locked = true;
    }
    void Unlock()
    {
      m_locked = false;
    }
    void MarkReceiverAsDead()
    {
      m_isReceiverDead = true;
    }
    bool IsLocked() const
    {
      return m_locked;
    }
    bool IsReceiverDead() const
    {
      return m_isReceiverDead;
    }
    bool UseHistory() const
    {
      return m_useHistory;
    }
    bool IsCompleted() const
    {
      return m_historyCompleted;
    }
    int32_t LastSourceId() const
    {
      return m_lastSourceId;
    }
    int32_t LastSourceSubId() const
    {
      return m_lastSourceSubId;
    }
  };
}

#endif
