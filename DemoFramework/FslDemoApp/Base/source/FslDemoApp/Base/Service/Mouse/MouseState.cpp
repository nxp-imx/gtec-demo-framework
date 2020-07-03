/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <FslDemoApp/Base/Service/Mouse/MouseState.hpp>


namespace Fsl
{
  MouseState::MouseState()
    : m_mouseButtonFlags(0)
  {
  }


  MouseState::MouseState(const uint32_t buttonState)
    : m_mouseButtonFlags(buttonState)
  {
  }


  MouseState::MouseState(const uint32_t buttonState, const PxPoint2& position)
    : m_mouseButtonFlags(buttonState)
    , Position(position)
  {
  }


  MouseState::MouseState(const uint32_t buttonState, const PxPoint2& position, const PxPoint2& rawPosition)
    : m_mouseButtonFlags(buttonState)
    , Position(position)
    , RawPosition(rawPosition)
  {
  }


  MouseState::MouseState(const VirtualMouseButtonFlags& mouseButtonFlags)
    : m_mouseButtonFlags(mouseButtonFlags)

  {
  }


  MouseState::MouseState(const VirtualMouseButtonFlags& mouseButtonFlags, const PxPoint2& position)
    : m_mouseButtonFlags(mouseButtonFlags)
    , Position(position)
  {
  }


  MouseState::MouseState(const VirtualMouseButtonFlags& mouseButtonFlags, const PxPoint2& position, const PxPoint2& rawPosition)
    : m_mouseButtonFlags(mouseButtonFlags)
    , Position(position)
    , RawPosition(rawPosition)
  {
  }


  bool MouseState::IsLeftButtonPressed() const
  {
    return m_mouseButtonFlags.IsLeftButtonPressed();
  }


  bool MouseState::IsMiddleButtonPressed() const
  {
    return m_mouseButtonFlags.IsMiddleButtonPressed();
  }


  bool MouseState::IsRightButtonPressed() const
  {
    return m_mouseButtonFlags.IsRightButtonPressed();
  }


  bool MouseState::IsXButton1Pressed() const
  {
    return m_mouseButtonFlags.IsXButton1Pressed();
  }


  bool MouseState::IsXButton2Pressed() const
  {
    return m_mouseButtonFlags.IsXButton2Pressed();
  }


  bool MouseState::IsXButton3Pressed() const
  {
    return m_mouseButtonFlags.IsXButton3Pressed();
  }


  bool MouseState::IsXButton4Pressed() const
  {
    return m_mouseButtonFlags.IsXButton4Pressed();
  }


  bool MouseState::IsXButton5Pressed() const
  {
    return m_mouseButtonFlags.IsXButton5Pressed();
  }
}
