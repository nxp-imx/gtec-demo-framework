#ifndef SHARED_LINEBUILDER101_LINEBUILDEREXAMPLE_HPP
#define SHARED_LINEBUILDER101_LINEBUILDEREXAMPLE_HPP
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

#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslDemoService/Graphics/IBasic2D.hpp>
#include <FslDemoApp/Base/Service/DemoAppControl/IDemoAppControl.hpp>
#include <FslDemoApp/Base/Service/Events/Basic/MouseButtonEvent.hpp>
#include <FslDemoApp/Base/Service/Keyboard/IKeyboard.hpp>
#include <FslDemoApp/Base/Service/Mouse/IMouse.hpp>
#include <FslGraphics3D/Build/LineBuilder.hpp>
#include <FslGraphics3D/Camera/FirstPersonCamera.hpp>
#include <memory>
#include <string>

namespace Fsl
{
  class LineBuilderExample
  {
    std::shared_ptr<IBasic2D> m_basic2D;
    std::shared_ptr<IKeyboard> m_keyboard;
    std::shared_ptr<IMouse> m_mouse;
    std::shared_ptr<IDemoAppControl> m_demoAppControl;
    bool m_mouseCaptureEnabled{false};
    Graphics3D::FirstPersonCamera m_camera;

    Matrix m_matView;
    Matrix m_matProjection;

    float m_radians1{0.0f};
    float m_radians2{0.0f};
    float m_radians3{0.0f};
    float m_rotationSpeed1{0.8f};
    float m_rotationSpeed2{0.75f};
    float m_rotationSpeed3{0.7f};

    Fsl::Graphics3D::LineBuilder m_lineBuilder;

    uint32_t m_cachedLines{0};
    std::string m_strLines;

  public:
    LineBuilderExample(const ServiceProvider& serviceProvider);
    void OnMouseButtonEvent(const MouseButtonEvent& event);
    void Update(const DemoTime& demoTime, const Point2& screenResolution);
    void Draw(const DemoTime& demoTime);

    const Matrix& GetViewMatrix() const
    {
      return m_matView;
    }

    const Matrix& GetProjectionMatrix() const
    {
      return m_matProjection;
    }

    const Fsl::Graphics3D::LineBuilder& GetLineBuilder() const
    {
      return m_lineBuilder;
    }

  private:
    void UpdateInput(const DemoTime& demoTime);
    void UpdateCameraControlInput(const DemoTime& demoTime, const KeyboardState& keyboardState);
    void BuildLineScene1();
    void BuildLineScene2();
  };
}

#endif
