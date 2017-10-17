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

#include <FslSimpleUI/Base/Control/Button.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslGraphics/Color.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/Event/WindowEventPool.hpp>
#include <FslSimpleUI/Base/Event/WindowEventSender.hpp>
#include <FslSimpleUI/Base/Event/WindowInputClickEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowSelectEvent.hpp>
#include <cassert>

namespace Fsl
{
  namespace UI
  {

    Button::Button(const std::shared_ptr<WindowContext>& context)
      : ContentControl(context)
      , m_isClickable(true)
      , m_isDown(false)
    {
      Enable(WindowFlags::ClickInput);
    }


    bool Button::IsClickable() const
    {
      return m_isClickable;
    }


    void Button::SetClickable(const bool value)
    {
      if (value != m_isClickable)
      {
        m_isClickable = value;
        PropertyUpdated(PropertyType::Other);
      }
    }


    void Button::OnClickInput(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent)
    {
      if (!theEvent->IsSource(this))
        return;

      if (m_isClickable && theEvent->IsBegin())
      {
        if (!theEvent->IsRepeat())
        {
          // Just for safety
          if (m_isDown)
          {
            m_isDown = false;
            Up(true);
          }
          m_isDown = true;
          theEvent->Handled();

          Down();
        }
      }
      else if (m_isDown)
      {
        m_isDown = false;
        theEvent->Handled();

        bool wasCanceled = true;
        if (m_isClickable)
        {
          // Only accept the press if the mouse/finger is still on top of the button
          auto pos = PointFromScreen(theEvent->GetScreenPosition());
          auto size = RenderSize();
          auto hitRect = Rect(0, 0, size.X, size.Y);
          if (hitRect.Contains(pos))
          {
            wasCanceled = false;
            SendEvent(GetEventPool()->AcquireWindowSelectEvent(0));
          }
        }

        Up(wasCanceled);
      }

    }
  }
}
