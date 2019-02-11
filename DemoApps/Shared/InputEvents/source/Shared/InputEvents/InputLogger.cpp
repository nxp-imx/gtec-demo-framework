/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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
 *    * Neither the name of the NXP. nor the names of
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

#include <Shared/InputEvents/InputLogger.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <iostream>
#include <sstream>

namespace Fsl
{
  InputLogger::InputLogger(const ServiceProvider& serviceProvider)
    : m_gamepads(serviceProvider.Get<IGamepads>())
  {
    auto graphicsService = serviceProvider.TryGet<IGraphicsService>();
    if (graphicsService)
    {
      m_basic2D = graphicsService->GetBasic2D();
    }

    // Alternative way to access the mouse state
    // std::shared_ptr<IMouse> mouse = serviceProvider.Get<IMouse>();
    // Alternative way to access the keyboard state
    // std::shared_ptr<IKeyboard> keyboard = serviceProvider.Get<IKeyboard>();

    const auto maxGamepads = m_gamepads->GetCount();
    m_gamepadStates.resize(maxGamepads);
    FSLLOG("Max gamepads: " << maxGamepads);
    UpdateGamepadStates();
  }


  void InputLogger::OnKeyEvent(const KeyEvent& event)
  {
    std::stringstream stream;
    stream << "OnKeyEvent key: " << event.GetKey() << " pressed: " << event.IsPressed();
    auto str = stream.str();
    FSLLOG(str);
    m_console.push_back(str);
  }


  void InputLogger::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    std::stringstream stream;
    stream << "OnMouseButtonEvent key: " << event.GetButton() << " pressed: " << event.IsPressed() << " position: " << event.GetPosition().X << ","
           << event.GetPosition().Y;
    auto str = stream.str();
    FSLLOG(str);
    m_console.push_back(str);
  }


  void InputLogger::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    std::stringstream stream;
    stream << "OnMouseMoveEvent position: " << event.GetPosition().X << "," << event.GetPosition().Y;
    auto str = stream.str();
    FSLLOG(str);
    m_console.push_back(str);
  }


  void InputLogger::OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    std::stringstream stream;
    stream << "OnMouseWheelEvent delta: " << event.GetDelta() << " position: " << event.GetPosition().X << "," << event.GetPosition().Y;
    auto str = stream.str();
    FSLLOG(str);
    m_console.push_back(str);
  }


  void InputLogger::OnRawMouseMoveEvent(const RawMouseMoveEvent& event)
  {
    std::stringstream stream;
    stream << "OnRawMouseMoveEvent position: " << event.GetPosition().X << "," << event.GetPosition().Y;
    auto str = stream.str();
    FSLLOG(str);
    m_console.push_back(str);
  }


  void InputLogger::UpdateGamepadStates()
  {
    for (uint32_t i = 0; i < m_gamepadStates.size(); ++i)
    {
      auto newState = m_gamepads->GetState(i);

      if (newState != m_gamepadStates[i])
      {
        m_gamepadStates[i] = newState;

        FSLLOG("Id: " << i << " IsConnected: " << newState.IsConnected << " Buttons: " << newState.Buttons.State
                      << " LeftTrigger: " << static_cast<uint32_t>(newState.LeftTrigger.Value)
                      << " RightTrigger: " << static_cast<uint32_t>(newState.RightTrigger.Value) << " LeftThumb: " << newState.LeftThumb.X << ","
                      << newState.LeftThumb.Y << " RightThumb: " << newState.RightThumb.X << "," << newState.RightThumb.Y);
      }
    }
  }


  void InputLogger::Draw(const Point2& resolution)
  {
    if (!m_basic2D)
    {
      return;
    }

    const auto fontSize = m_basic2D->FontSize();
    if (fontSize.Y > 0)
    {
      // This is not a efficient way to render the a 'console' but its simple
      const auto maxLines = static_cast<uint32_t>(resolution.Y / fontSize.Y);

      while (m_console.size() > maxLines)
      {
        m_console.pop_front();
      }

      m_basic2D->Begin();

      Vector2 pos(0, resolution.Y - fontSize.Y * static_cast<int32_t>(m_console.size()));
      for (uint32_t i = 0; i < m_console.size(); ++i)
      {
        m_basic2D->DrawString(m_console[i], pos);
        pos.Y += fontSize.Y;
      }

      m_basic2D->End();
    }
  }
}
