#ifndef FSLSIMPLEUI_BASE_SYSTEM_EVENT_ISTATEEVENTSENDER_HPP
#define FSLSIMPLEUI_BASE_SYSTEM_EVENT_ISTATEEVENTSENDER_HPP
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

#include <memory>
#include "SendResult.hpp"

namespace Fsl
{
  struct PxPoint2;

  namespace UI
  {
    struct StateEvent;
    class TreeNode;
    class WindowEventPool;

    class IStateEventSender
    {
    public:
      virtual ~IStateEventSender() = default;

      //! @brief Get the associated event pool
      virtual const std::shared_ptr<WindowEventPool>& GetEventPool() const = 0;

      //! @brief Check if there is a active event in the event sender
      virtual bool HasActiveEvent() const noexcept = 0;
      //! @brief Check if there is a active event in the event sender (that is not the given target)
      virtual bool HasActiveClickEventThatIsNot(const std::shared_ptr<TreeNode>& target) const = 0;

      //! @brief check if there is a history associated with this event.
      virtual bool HasHistory() const = 0;

      //! @brief Send a event to a window according to the rules
      virtual SendResult Send(const StateEvent& theEvent, const std::shared_ptr<TreeNode>& target) = 0;

      /// <summary>
      /// Send a event to a window according to the rules
      /// </summary>
      virtual SendResult Send(const StateEvent& theEvent, const PxPoint2& hitPositionPx) = 0;
    };
  }
}

#endif
