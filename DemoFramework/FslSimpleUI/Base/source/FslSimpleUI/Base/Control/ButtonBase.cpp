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
#include <FslBase/Math/Pixel/PxRectangle2D.hpp>
#include <FslGraphics/Color.hpp>
#include <FslSimpleUI/Base/Control/ButtonBase.hpp>
#include <FslSimpleUI/Base/Event/WindowEventPool.hpp>
#include <FslSimpleUI/Base/Event/WindowEventSender.hpp>
#include <FslSimpleUI/Base/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <cassert>

namespace Fsl::UI
{
  ButtonBase::ButtonBase(const std::shared_ptr<BaseWindowContext>& context)
    : BaseWindow(context)
  {
    Enable(WindowFlags::ClickInput);
  }


  void ButtonBase::SetEnabled(const bool enabled)
  {
    if (enabled != m_isEnabled)
    {
      m_isEnabled = enabled;
      if (!enabled && m_isDown)
      {
        CancelButtonDown();
      }
      PropertyUpdated(PropertyType::Other);
    }
  }


  void ButtonBase::OnClickInput(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent)
  {
    FSL_PARAM_NOT_USED(args);

    if (!theEvent->IsSource(this))
    {
      return;
    }

    if (m_isEnabled && theEvent->IsBegin())
    {
      if (!theEvent->IsRepeat())
      {
        // Just for safety
        if (m_isDown)
        {
          CancelButtonDown();
        }
        m_isDown = true;
        theEvent->Handled();

        // always set the state before calling 'pressed'
        Pressed(ButtonPressState::Down);
      }
    }
    else if (m_isDown)
    {
      m_isDown = false;
      theEvent->Handled();

      if (m_isEnabled)
      {
        bool wasCanceled = true;
        // Only accept the press if the mouse/finger is still on top of the button
        auto pos = PointFromScreen(theEvent->GetScreenPosition());
        auto size = RenderExtentPx();
        PxRectangle2D hitRect(PxValue(0), PxValue(0), size.Width, size.Height);
        if (hitRect.Contains(pos))
        {
          wasCanceled = false;
          SendEvent(GetEventPool()->AcquireWindowSelectEvent(0));
        }
        // always set the state before calling 'pressed'
        Pressed(!wasCanceled ? ButtonPressState::Up : ButtonPressState::UpCancelled);
      }
    }
  }

  void ButtonBase::CancelButtonDown()
  {
    if (!m_isDown)
    {
      return;
    }
    // always set the state before calling 'pressed'
    m_isDown = false;
    Pressed(ButtonPressState::UpCancelled);
  }

}
