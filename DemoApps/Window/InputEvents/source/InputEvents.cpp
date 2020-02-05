/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Exceptions.hpp>
#include "InputEvents.hpp"

namespace Fsl
{
  InputEvents::InputEvents(const DemoAppConfig& config)
    : DemoAppWindow(config)
    , m_logger(config.DemoServiceProvider)
  {
    // Alternative way to access the mouse state
    // std::shared_ptr<IMouse> mouse = config.DemoServiceProvider.Get<IMouse>();
    // Alternative way to access the keyboard state
    // std::shared_ptr<IKeyboard> keyboard = config.DemoServiceProvider.Get<IKeyboard>();
  }


  InputEvents::~InputEvents() = default;


  void InputEvents::OnKeyEvent(const KeyEvent& event)
  {
    m_logger.OnKeyEvent(event);
  }


  void InputEvents::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    m_logger.OnMouseButtonEvent(event);
  }


  void InputEvents::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    m_logger.OnMouseMoveEvent(event);
  }


  void InputEvents::OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    m_logger.OnMouseWheelEvent(event);
  }


  void InputEvents::OnRawMouseMoveEvent(const RawMouseMoveEvent& event)
  {
    m_logger.OnRawMouseMoveEvent(event);
  }


  void InputEvents::Update(const DemoTime& demoTime)
  {
    m_logger.UpdateGamepadStates();
  }


  void InputEvents::Draw(const DemoTime& demoTime)
  {
    m_logger.Draw(GetScreenResolution());
  }

}
