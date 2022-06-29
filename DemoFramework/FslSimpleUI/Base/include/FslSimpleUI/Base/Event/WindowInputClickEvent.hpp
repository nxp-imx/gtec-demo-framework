#ifndef FSLSIMPLEUI_BASE_EVENT_WINDOWINPUTCLICKEVENT_HPP
#define FSLSIMPLEUI_BASE_EVENT_WINDOWINPUTCLICKEVENT_HPP
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

#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslSimpleUI/Base/Event/WindowInputEvent.hpp>

namespace Fsl::UI
{
  class WindowInputClickEvent : public WindowInputEvent
  {
    PxPoint2 m_screenPositionPx;

  public:
    WindowInputClickEvent();

    //! @brief Return the screen position in pixels.
    //! @warning This is not the window position so convert it to window coordinates before using it!!!!)
    PxPoint2 GetScreenPosition() const
    {
      return m_screenPositionPx;
    }

  protected:
    void SYS_Construct(const int32_t sourceId, const int32_t sourceSubId, const EventTransactionState state, const bool isRepeat,
                       const PxPoint2& screenPositionPx);
    void SYS_Destruct() override;
    friend class WindowEventPool;
  };
}

#endif
