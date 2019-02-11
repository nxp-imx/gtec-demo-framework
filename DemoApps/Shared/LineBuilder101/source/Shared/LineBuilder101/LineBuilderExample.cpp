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

#include <Shared/LineBuilder101/LineBuilderExample.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/String/ToString.hpp>
#include <FslBase/Math/BoundingBox.hpp>
#include <FslBase/Math/BoundingFrustum.hpp>
#include <FslBase/Math/BoundingSphere.hpp>
#include <FslBase/Math/BoxF.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Ray.hpp>
#include <FslBase/Math/Rect.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/Math/Rectangle2D.hpp>
#include <FslBase/Math/Rectangle3D.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>

namespace Fsl
{
  namespace
  {
    const Vector3 DEFAULT_CAMERA_POSITION(2.0f, 2.0f, 4.0f);
    const Vector3 DEFAULT_CAMERA_TARGET(0.0f, 0.0f, 0.0f);
  }

  LineBuilderExample::LineBuilderExample(const ServiceProvider& serviceProvider)
    : m_basic2D(serviceProvider.Get<IGraphicsService>()->GetBasic2D())
    , m_keyboard(serviceProvider.Get<IKeyboard>())
    , m_mouse(serviceProvider.Get<IMouse>())
    , m_demoAppControl(serviceProvider.Get<IDemoAppControl>())
  {
    m_camera.SetPosition(DEFAULT_CAMERA_POSITION, DEFAULT_CAMERA_TARGET, Vector3::Up());
  }

  void LineBuilderExample::OnMouseButtonEvent(const MouseButtonEvent& event)
  {
    if (event.IsHandled())
    {
      return;
    }

    switch (event.GetButton())
    {
    case VirtualMouseButton::Right:
    {
      const bool mouseCapture = event.IsPressed();
      if (m_demoAppControl->TryEnableMouseCaptureMode(mouseCapture))
      {
        m_mouseCaptureEnabled = mouseCapture;
      }
      else
      {
        m_mouseCaptureEnabled = false;
      }
      event.Handled();
      break;
    }
    case VirtualMouseButton::Middle:
      if (event.IsPressed())
      {
        m_camera.SetPosition(DEFAULT_CAMERA_POSITION, DEFAULT_CAMERA_TARGET, Vector3::Up());
        event.Handled();
      }
      break;
    default:
      break;
    }
  }

  void LineBuilderExample::Update(const DemoTime& demoTime, const Point2& screenResolution)
  {
    UpdateInput(demoTime);

    float aspectRatio = screenResolution.X / static_cast<float>(screenResolution.Y);
    m_matView = m_camera.GetViewMatrix();
    m_matProjection = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45.0f), aspectRatio, 0.001f, 500.0f);

    m_radians1 += m_rotationSpeed1 * demoTime.DeltaTime;
    m_radians2 += m_rotationSpeed2 * demoTime.DeltaTime;
    m_radians3 += m_rotationSpeed3 * demoTime.DeltaTime;
    m_radians1 = MathHelper::WrapAngle(m_radians1);
    m_radians2 = MathHelper::WrapAngle(m_radians2);
    m_radians3 = MathHelper::WrapAngle(m_radians3);

    m_lineBuilder.Clear();
    BuildLineScene1();
    BuildLineScene2();

    if (m_lineBuilder.LineCount() != m_cachedLines)
    {
      m_cachedLines = m_lineBuilder.LineCount();
      m_strLines = "Lines: ";
      m_strLines += ToString(m_cachedLines);
    }
  }

  void LineBuilderExample::Draw(const DemoTime& demoTime)
  {
    m_basic2D->Begin();
    m_basic2D->DrawString(m_strLines, Vector2(0, 0));
    m_basic2D->End();
  }

  void LineBuilderExample::UpdateInput(const DemoTime& demoTime)
  {
    const auto keyboardState = m_keyboard->GetState();
    UpdateCameraControlInput(demoTime, keyboardState);
  }

  void LineBuilderExample::UpdateCameraControlInput(const DemoTime& demoTime, const KeyboardState& keyboardState)
  {
    {    // Mouse camera rotation
      const auto mouseState = m_mouse->GetState();

      if (!m_mouseCaptureEnabled)
      {
        const bool rotateCamera = mouseState.IsRightButtonPressed();
        m_camera.RotateViaPosition(rotateCamera, mouseState.Position);
      }
      else
      {
        if (mouseState.IsRightButtonPressed())
        {
          const auto rawPosition = Vector2(mouseState.RawPosition.X, -mouseState.RawPosition.Y);
          m_camera.Rotate(rawPosition);
        }
      }
    }

    // Keyboard camera movement
    const float movementSpeed = 2.0f * demoTime.DeltaTime;
    if (keyboardState.IsKeyDown(VirtualKey::W))
    {
      m_camera.MoveForward(movementSpeed);
    }
    if (keyboardState.IsKeyDown(VirtualKey::S))
    {
      m_camera.MoveBackwards(movementSpeed);
    }
    if (keyboardState.IsKeyDown(VirtualKey::A))
    {
      m_camera.MoveLeft(movementSpeed);
    }
    if (keyboardState.IsKeyDown(VirtualKey::D))
    {
      m_camera.MoveRight(movementSpeed);
    }
  }

  void LineBuilderExample::BuildLineScene1()
  {
    constexpr auto testScale = 0.25f;
    const auto matTest = Matrix::CreatePerspective(3.2f * testScale, 2.4f * testScale, 1 * testScale, 3 * testScale);

    m_lineBuilder.AddGridXZ(Rect(-40.0f, -40.0f, 80.0f, 80.0f), 0.0f, 79, Color(0.5f, 0.5f, 0.5f, 1.0f));

    // Default axis coordinates at 0,0,0
    m_lineBuilder.AddAxis(Vector3(0, 0, 0), 2.5f);

    m_lineBuilder.Add(Ray(Vector3(1.5f, 0.5f, 0.5f), Vector3::Forward()), Color::Brown());
    m_lineBuilder.Add(BoundingBox(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.25f, 0.25f, 0.25f)), Color::Orange());

    m_lineBuilder.Add(BoxF(1.25f, 1.25f, 1.75f, 1.75f), Color(150u, 150u, 0u, 255u));
    m_lineBuilder.Add(Rect(1.25f, 0.25f, 0.5f, 0.5f), Color(0u, 200u, 0u, 255u));
    m_lineBuilder.Add(Rectangle(1, 0, 1, 1), Color(200u, 0u, 0u, 255u));
    m_lineBuilder.Add(Rectangle2D(1, 1, 1, 1), Color(200u, 200u, 0u, 255u));
    m_lineBuilder.Add(Rectangle3D(0, 0, 0, 1, 1, 1), Color(0u, 0u, 200u, 255u));

    m_lineBuilder.Add(BoundingFrustum(matTest), Color(200u, 0u, 200u, 255u));
    m_lineBuilder.Add(BoundingSphere(Vector3(0.5f, 0.5f, 0.5f), 0.5f), Color(0u, 200u, 200u, 255u), 128);

    m_lineBuilder.AddAxisSphere(Vector3(0.5f, 0.5f, 0.5f), 0.25f, 64);

    m_lineBuilder.Add(Vector3(1.25f, 0.0f, 1.0f), Vector3(1.25f, 0.0f, 0.0f), Color::Red(), Color::Blue());
    m_lineBuilder.Add(Vector3(1.75f, 0.0f, 1.0f), Vector3(1.75f, 0.0f, 0.0f), Color::Green());

    {    // Render three rotating axis
      auto matAxis1 = Matrix::CreateRotationX(m_radians1) * Matrix::CreateTranslation(0.0f, 0.0f, 1.5f);
      auto matAxis2 = Matrix::CreateRotationY(m_radians1) * Matrix::CreateTranslation(0.5f, 0.0f, 1.5f);
      auto matAxis3 = Matrix::CreateRotationZ(m_radians1) * Matrix::CreateTranslation(1.0f, 0.0f, 1.5f);
      auto matAxis4 = Matrix::CreateRotationX(m_radians1) * Matrix::CreateRotationY(m_radians2) * Matrix::CreateRotationZ(m_radians3) *
                      Matrix::CreateTranslation(1.5f, 0.0f, 1.5f);

      // Add a few bounding boxes for flavor
      const BoundingBox boundingBox(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.05f, 0.05f, 0.05f));
      m_lineBuilder.Add(boundingBox, Color::Orange(), matAxis1);
      m_lineBuilder.Add(boundingBox, Color::Orange(), matAxis2);
      m_lineBuilder.Add(boundingBox, Color::Orange(), matAxis3);
      m_lineBuilder.Add(boundingBox, Color::Orange(), matAxis4);

      // Add the axis
      m_lineBuilder.AddAxis(Vector3(0, 0, 0), 0.25f, matAxis1);
      m_lineBuilder.AddAxis(Vector3(0, 0, 0), 0.25f, matAxis2);
      m_lineBuilder.AddAxis(Vector3(0, 0, 0), 0.25f, matAxis3);
      m_lineBuilder.AddAxis(Vector3(0, 0, 0), 0.25f, matAxis4);

      // and some nice spheres
      m_lineBuilder.AddAxisSphere(Vector3(0.0f, 0.0f, 0.0f), 0.25f, matAxis1);
      m_lineBuilder.AddAxisSphere(Vector3(0.0f, 0.0f, 0.0f), 0.25f, matAxis2);
      m_lineBuilder.AddAxisSphere(Vector3(0.0f, 0.0f, 0.0f), 0.25f, matAxis3);
      m_lineBuilder.AddAxisSphere(Vector3(0.0f, 0.0f, 0.0f), 0.25f, matAxis4);
    }
  }


  void LineBuilderExample::BuildLineScene2()
  {
    constexpr auto testScale = 0.25f;
    const auto matTest = Matrix::CreatePerspective(3.2f * testScale, 2.4f * testScale, 1 * testScale, 3 * testScale);

    auto matRot = Matrix::CreateTranslation(-2.0f, 0.0f, 0.0f) * Matrix::CreateRotationX(m_radians1) * Matrix::CreateRotationY(m_radians2) *
                  Matrix::CreateRotationZ(m_radians3) * Matrix::CreateScale(0.25f) * Matrix::CreateTranslation(-1.0f, 0.0f, 0.0f);

    m_lineBuilder.AddGridXZ(Rect(-2.0f, -2.0f, 4.0f, 4.0f), 0.0f, 3, Color(0.5f, 0.5f, 0.5f, 1.0f), matRot);

    m_lineBuilder.Add(Ray(Vector3(1.5f, 0.5f, 0.5f), Vector3::Forward()), Color::Brown(), matRot);
    m_lineBuilder.Add(BoundingBox(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.25f, 0.25f, 0.25f)), Color::Orange(), matRot);
    m_lineBuilder.Add(BoxF(1.25f, 1.25f, 1.75f, 1.75f), Color(150u, 150u, 0u, 255u), matRot);
    m_lineBuilder.Add(Rect(1.25f, 0.25f, 0.5f, 0.5f), Color(0u, 200u, 0u, 255u), matRot);
    m_lineBuilder.Add(Rectangle(1, 0, 1, 1), Color(200u, 0u, 0u, 255u), matRot);
    m_lineBuilder.Add(Rectangle2D(1, 1, 1, 1), Color(200u, 200u, 0u, 255u), matRot);
    m_lineBuilder.Add(Rectangle3D(0, 0, 0, 1, 1, 1), Color(0u, 0u, 200u, 255u), matRot);
    m_lineBuilder.Add(BoundingFrustum(matTest), Color(200u, 0u, 200u, 255u), matRot);
    m_lineBuilder.Add(BoundingSphere(Vector3(0.5f, 0.5f, 0.5f), 0.5f), Color(0u, 200u, 200u, 255u), matRot, 128);
    m_lineBuilder.AddAxisSphere(Vector3(0.5f, 0.5f, 0.5f), 0.25f, matRot, 64);

    m_lineBuilder.Add(Vector3(1.25f, 0.0f, 1.0f), Vector3(1.25f, 0.0f, 0.0f), Color::Red(), Color::Blue(), matRot);
    m_lineBuilder.Add(Vector3(1.75f, 0.0f, 1.0f), Vector3(1.75f, 0.0f, 0.0f), Color::Green(), matRot);

    {    // Render three rotating axis
      auto matAxis1 = Matrix::CreateRotationX(m_radians1) * Matrix::CreateTranslation(0.0f, 0.0f, 1.5f) * matRot;
      auto matAxis2 = Matrix::CreateRotationY(m_radians1) * Matrix::CreateTranslation(0.5f, 0.0f, 1.5f) * matRot;
      auto matAxis3 = Matrix::CreateRotationZ(m_radians1) * Matrix::CreateTranslation(1.0f, 0.0f, 1.5f) * matRot;
      auto matAxis4 = Matrix::CreateRotationX(m_radians1) * Matrix::CreateRotationY(m_radians2) * Matrix::CreateRotationZ(m_radians3) *
                      Matrix::CreateTranslation(1.5f, 0.0f, 1.5f) * matRot;

      // Add a few bounding boxes for flavor
      const BoundingBox boundingBox(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.05f, 0.05f, 0.05f));
      m_lineBuilder.Add(boundingBox, Color::Orange(), matAxis1);
      m_lineBuilder.Add(boundingBox, Color::Orange(), matAxis2);
      m_lineBuilder.Add(boundingBox, Color::Orange(), matAxis3);
      m_lineBuilder.Add(boundingBox, Color::Orange(), matAxis4);

      // Add the axis
      m_lineBuilder.AddAxis(Vector3(0, 0, 0), 0.25f, matAxis1);
      m_lineBuilder.AddAxis(Vector3(0, 0, 0), 0.25f, matAxis2);
      m_lineBuilder.AddAxis(Vector3(0, 0, 0), 0.25f, matAxis3);
      m_lineBuilder.AddAxis(Vector3(0, 0, 0), 0.25f, matAxis4);

      // and some nice spheres
      m_lineBuilder.AddAxisSphere(Vector3(0.0f, 0.0f, 0.0f), 0.25f, matAxis1);
      m_lineBuilder.AddAxisSphere(Vector3(0.0f, 0.0f, 0.0f), 0.25f, matAxis2);
      m_lineBuilder.AddAxisSphere(Vector3(0.0f, 0.0f, 0.0f), 0.25f, matAxis3);
      m_lineBuilder.AddAxisSphere(Vector3(0.0f, 0.0f, 0.0f), 0.25f, matAxis4);
    }
  }
}
