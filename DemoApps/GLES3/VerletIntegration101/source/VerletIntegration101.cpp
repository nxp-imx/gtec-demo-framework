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

#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <FslBase/Math/VectorHelper.hpp>
#include <FslDemoApp/Base/Service/Graphics/IGraphicsService.hpp>
#include "VerletIntegration101.hpp"
#include <GLES3/gl3.h>
#include <iostream>

namespace Fsl
{
  using namespace GLES3;
  using namespace UI;

  namespace
  {
    float Distance(const Vector2& value0, const Vector2& value1)
    {
      return (value1 - value0).Length();
    }
  }


  // Verlet integration code based on the tutorial here:
  // YouTube: Coding Math: Episode 36 - Verlet Integration Part I+IV
  // https://www.youtube.com/watch?v=3HjO_RGIjCU&index=1&list=PL_-Pk4fSWzGv_EAW9hW2ioo9Xtr-9DuoZ

  VerletIntegration101::VerletIntegration101(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_uiEventListener(this)
    , m_uiExtension(std::make_shared<UIDemoAppExtension>(config, m_uiEventListener.GetListener(), "MainAtlas"))
    , m_batch(std::dynamic_pointer_cast<NativeBatch2D>(config.DemoServiceProvider.Get<IGraphicsService>()->GetNativeBatch2D()))
    , m_boundaryRect()
    , m_rotation(0)
  {
    RegisterExtension(m_uiExtension);

    auto context = m_uiExtension->GetContext();
    m_texFill = m_uiExtension->GetAtlasTexture2D("Fill");
    m_texBall = m_uiExtension->GetAtlasTexture2D("SliderCursor");
    m_texTest = m_uiExtension->GetAtlasTexture2D("Player/Stop");


    auto screenResolution = config.ScreenResolution;
    const int32_t safeX = static_cast<int32_t>(screenResolution.X * 0.10f);
    const int32_t safeY = static_cast<int32_t>(screenResolution.Y * 0.10f);
    m_boundaryRect = Rectangle(safeX, safeY, screenResolution.X - (2 * safeX), screenResolution.Y - (2 * safeY));

    float offsetX = static_cast<float>(safeX);
    float offsetY = static_cast<float>(safeY);
    auto p0 = Particle(offsetX + 100.0f, offsetY + 100.0f, offsetX + 85.0f, offsetY + 95.0f);
    auto p1 = Particle(offsetX + 200.0f, offsetY + 100.0f, offsetX + 200.0f, offsetY + 100.0f);
    auto p2 = Particle(offsetX + 200.0f, offsetY + 200.0f, offsetX + 200.0f, offsetY + 200.0f);
    auto p3 = Particle(offsetX + 100.0f, offsetY + 200.0f, offsetX + 100.0f, offsetY + 200.0f);

    m_particles.push_back(p0);
    m_particles.push_back(p1);
    m_particles.push_back(p2);
    m_particles.push_back(p3);
    m_sticks.push_back(Stick(0, 1, Distance(p0.Position, p1.Position)));
    m_sticks.push_back(Stick(1, 2, Distance(p1.Position, p2.Position)));
    m_sticks.push_back(Stick(2, 3, Distance(p2.Position, p3.Position)));
    m_sticks.push_back(Stick(3, 0, Distance(p3.Position, p0.Position)));
    m_sticks.push_back(Stick(0, 2, Distance(p0.Position, p2.Position)));
  }


  VerletIntegration101::~VerletIntegration101()
  {

  }


  void VerletIntegration101::FixedUpdate(const DemoTime& demoTime)
  {
    const float friction = 0.999f;
    UpdateParticles(m_particles, friction);
    for (std::size_t i = 0; i < 3; ++i)
    {
      UpdateSticks(m_particles, m_sticks);
      ConstrainPoints(m_particles, m_boundaryRect, friction);
    }
  }


  void VerletIntegration101::Update(const DemoTime& demoTime)
  {
    m_rotation += 0.01f;
  }



  void VerletIntegration101::Draw(const DemoTime& demoTime)
  {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_batch->Begin();

    m_batch->DebugDrawRectangle(m_texFill, m_boundaryRect, Color(64, 64, 64, 255));

    //m_batch->DebugDrawLine(m_texFill, dstFrom, dstRotated, Color::White());

    DrawSticks(m_particles, m_sticks);
    DrawParticles(m_particles);

    m_batch->End();


    m_uiExtension->Draw();
  }


  void VerletIntegration101::UpdateParticles(std::deque<Particle>& particles, const float friction)
  {
    const float gravity = 0.5f;

    for (auto itr = particles.begin(); itr != particles.end(); ++itr)
    {
      auto velocity = (itr->Position - itr->OldPosition) * friction;
      itr->OldPosition = itr->Position;
      itr->Position += velocity;
      itr->Position.Y += gravity;
    }
  }


  void VerletIntegration101::UpdateSticks(std::deque<Particle>& particles, std::deque<Stick>& sticks)
  {
    for (auto itr = sticks.begin(); itr != sticks.end(); ++itr)
    {
      auto delta = particles[itr->PointIndex1].Position - particles[itr->PointIndex0].Position;
      auto distance = delta.Length();
      auto difference = itr->Length - distance;
      auto percent = (difference / distance) * 0.5f;
      auto offset = delta * percent;
      particles[itr->PointIndex0].Position -= offset;
      particles[itr->PointIndex1].Position += offset;
    }
  }



  void VerletIntegration101::ConstrainPoints(std::deque<Particle>& particles, const Rectangle& boundaryRect, const float friction)
  {
    const float boundaryLeft = static_cast<float>(boundaryRect.Left());
    const float boundaryTop = static_cast<float>(boundaryRect.Top());
    const float boundaryRight = static_cast<float>(boundaryRect.Right() - 1);
    const float boundaryBottom = static_cast<float>(boundaryRect.Bottom() - 1);

    const float bounce = 0.90f;

    for (auto itr = particles.begin(); itr != particles.end(); ++itr)
    {
      auto velocity = (itr->Position - itr->OldPosition) * friction;
      if (itr->Position.X > boundaryRight)
      {
        itr->Position.X = boundaryRight;
        itr->OldPosition.X = itr->Position.X + (velocity.X * bounce);
      }
      else if (itr->Position.X < boundaryLeft)
      {
        itr->Position.X = boundaryLeft;
        itr->OldPosition.X = itr->Position.X + (velocity.X * bounce);
      }
      if (itr->Position.Y > boundaryBottom)
      {
        itr->Position.Y = boundaryBottom;
        itr->OldPosition.Y = itr->Position.Y + (velocity.Y * bounce);
      }
      else if (itr->Position.Y < boundaryTop)
      {
        itr->Position.Y = boundaryTop;
        itr->OldPosition.Y = itr->Position.Y + (velocity.Y * bounce);
      }
    }
  }


  void VerletIntegration101::DrawSticks(const std::deque<Particle>& particles, const std::deque<Stick>& sticks)
  {
    const auto color = Color::White();
    for (auto itr = sticks.begin(); itr != sticks.end(); ++itr)
    {
      auto from = particles[itr->PointIndex0].Position;
      auto to= particles[itr->PointIndex1].Position;
      m_batch->DebugDrawLine(m_texFill, from, to, color);
    }

  }


  void VerletIntegration101::DrawParticles(const std::deque<Particle>& particles)
  {
    const Vector2 scale(0.2f, 0.2f);
    const Vector2 origin(m_texBall.GetSize().X * 0.5f, m_texBall.GetSize().Y * 0.5f);
    const auto color = Color::White();

    for (auto itr = particles.begin(); itr != particles.end(); ++itr)
    {
      m_batch->Draw(m_texBall, itr->Position, color, origin, scale);
    }
  }


}
