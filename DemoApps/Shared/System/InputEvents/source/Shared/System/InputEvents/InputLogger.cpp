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

#include <Shared/System/InputEvents/InputLogger.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxPoint2.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <fmt/format.h>

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
    FSLLOG3_INFO("Max gamepads: {}", maxGamepads);
    UpdateGamepadStates();
  }


  void InputLogger::OnKeyEvent(const KeyEvent& event)
  {
    auto str = fmt::format("OnKeyEvent key: {} pressed: {}", event.GetKey(), event.IsPressed());
    FSLLOG3_INFO(str);
    m_console.push_back(str);
  }


  void InputLogger::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    auto str = fmt::format("OnMouseButtonEvent key: {} pressed: {} position: {}", event.GetButton(), event.IsPressed(), event.GetPosition());
    FSLLOG3_INFO(str);
    m_console.push_back(str);
  }


  void InputLogger::OnMouseMoveEvent(const MouseMoveEvent& event)
  {
    auto str = fmt::format("OnMouseMoveEvent position: {}", event.GetPosition());
    FSLLOG3_INFO(str);
    m_console.push_back(str);
  }


  void InputLogger::OnMouseWheelEvent(const MouseWheelEvent& event)
  {
    auto str = fmt::format("OnMouseWheelEvent delta: {} position: {}", event.GetDelta(), event.GetPosition());
    FSLLOG3_INFO(str);
    m_console.push_back(str);
  }


  void InputLogger::OnRawMouseMoveEvent(const RawMouseMoveEvent& event)
  {
    auto str = fmt::format("OnRawMouseMoveEvent position: {}", event.GetPosition());
    FSLLOG3_INFO(str);
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

        FSLLOG3_INFO("Id: {} IsConnected: {} Buttons: {} LeftTrigger: {} RightTrigger: {} LeftThumb: {},{} RightThumb: {},{}", i,
                     newState.IsConnected, newState.Buttons.State, static_cast<uint32_t>(newState.LeftTrigger.Value),
                     static_cast<uint32_t>(newState.RightTrigger.Value), newState.LeftThumb.X, newState.LeftThumb.Y, newState.RightThumb.X,
                     newState.RightThumb.Y);
      }
    }
  }


  void InputLogger::Draw(const PxSize2D& sizePx)
  {
    if (!m_basic2D)
    {
      return;
    }

    const auto fontSize = m_basic2D->FontSize();
    if (fontSize.Height() > 0)
    {
      // This is not a efficient way to render the a 'console' but its simple
      const auto maxLines = static_cast<uint32_t>(sizePx.Width() / fontSize.Height());

      while (m_console.size() > maxLines)
      {
        m_console.pop_front();
      }

      m_basic2D->Begin();


      PxPoint2 posPx(0, sizePx.Height() - fontSize.Height() * static_cast<int32_t>(m_console.size()));
      for (uint32_t i = 0; i < m_console.size(); ++i)
      {
        m_basic2D->DrawString(m_console[i], posPx);
        posPx.Y += fontSize.Height();
      }

      m_basic2D->End();
    }
  }
}
