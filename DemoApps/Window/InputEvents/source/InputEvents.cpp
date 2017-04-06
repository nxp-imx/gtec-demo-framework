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

#include <FslBase/Log/Log.hpp>
#include <FslBase/Exceptions.hpp>
#include "InputEvents.hpp"

namespace Fsl
{
  namespace
  {
  }


  InputEvents::InputEvents(const DemoAppConfig& config)
    : DemoAppWindow(config)
  {

    // Alternative way to access the mouse state
    //std::shared_ptr<IMouse> mouse = config.DemoServiceProvider.Get<IMouse>();
    // Alternative way to access the keyboard state
    //std::shared_ptr<IKeyboard> keyboard = config.DemoServiceProvider.Get<IKeyboard>();
  }


  InputEvents::~InputEvents()
  {

  }


  void InputEvents::OnKeyEvent(const KeyEvent& event)
  {
    std::stringstream stream;
    stream << "OnKeyEvent key: " << event.GetKey() << " pressed: " << event.IsPressed();
    auto str = stream.str();
    FSLLOG(str);
  }


  void InputEvents::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    std::stringstream stream;
    stream << "OnMouseButtonEvent key: " << event.GetButton() << " pressed: " << event.IsPressed() << " position: " << event.GetPosition().X << "," << event.GetPosition().Y;
    auto str = stream.str();
    FSLLOG(str);
  }


  void InputEvents::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    std::stringstream stream;
    stream << "OnMouseMoveEvent position: " << event.GetPosition().X << "," << event.GetPosition().Y;
    auto str = stream.str();
    FSLLOG(str);
  }


  void InputEvents::OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    std::stringstream stream;
    stream << "OnMouseWheelEvent delta: " << event.GetDelta() << " position: " << event.GetPosition().X << "," << event.GetPosition().Y;
    auto str = stream.str();
    FSLLOG(str);
  }


  void InputEvents::Update(const DemoTime& demoTime)
  {
  }


  void InputEvents::Draw(const DemoTime& demoTime)
  {
  }

}
