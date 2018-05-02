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

#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <Shared/FractalShader/JuliaHelper.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Color.hpp>
#include <algorithm>
#include <FslGraphics/TextureAtlas/TextureAtlasHelper.hpp>
#include "MainAtlas.hpp"

namespace Fsl
{
  JuliaHelper::JuliaHelper(const BasicConfig& config, const AnimationMode animationMode, const ServiceProvider& serviceProvider, const bool allowBanner)
    : m_config(config)
    , m_animationMode(animationMode)
    , m_basic2D(serviceProvider.Get<IGraphicsService>()->GetBasic2D())
    , m_nativeBatch2D(serviceProvider.Get<IGraphicsService>()->GetNativeBatch2D())
    , m_atlasTexBanner()
    , m_angle1()
    , m_angle2()
    , m_angle3()
    , m_location()
    , m_angle(0.0f)
  {
    if (allowBanner)
    {
      const std::shared_ptr<IContentManager> contentManager = serviceProvider.Get<IContentManager>();
      { // Get the banner atlas texture
        MainAtlas textureAtlas;
        AtlasTextureInfo texInfo = TextureAtlasHelper::GetAtlasTextureInfo(textureAtlas, "Banner_Julia");
        Bitmap bitmap;
        contentManager->Read(bitmap, textureAtlas.GetName(), PixelFormat::R8G8B8A8_UNORM);
        Texture2D texAtlas(serviceProvider.Get<IGraphicsService>()->GetNativeGraphics(), bitmap, Texture2DFilterHint::Smooth);
        m_atlasTexBanner.Reset(texAtlas, texInfo);
      }
    }
  }

  void JuliaHelper::Update(const DemoTime& demoTime)
  {
    switch (m_animationMode)
    {
    case AnimationMode::ConstantLoad:
      UpdateConstantLoad(demoTime);
      break;
    case AnimationMode::ConstantLoad2:
      UpdateConstantLoad2(demoTime);
      break;
    case AnimationMode::ConstantLoad3:
      UpdateConstantLoad3(demoTime);
      break;
    case AnimationMode::ComplexMove:
      UpdateComplexMove(demoTime);
      break;
    case AnimationMode::SlowAndComplex:
      UpdateSlowAndComplex(demoTime);
      break;
    case AnimationMode::Stopped:
      m_location.X = 0.35f;
      m_location.Y = 0.25f;
      break;
    default:
      UpdateDSin(demoTime);
      break;
    }
  }

  void JuliaHelper::UpdateConstantLoad(const DemoTime& demoTime)
  {
    m_location = Vector2(0.0f, 0.0f);

    const float s1x = 0.1f * 0.4f;
    const float s2x = 0.1f * 0.4f;
    const float s1y = 0.08f * 0.2f;
    const float s2y = 0.08f * 0.2f;

    float xMod = (std::sin(m_angle1.X) * s1x) + (std::cos(m_angle2.X) * s2x);
    float yMod = (std::sin(m_angle1.Y) * s1y) + (std::cos(m_angle2.X) * s2y);
    m_location.X += xMod;
    m_location.Y += yMod;

    m_location.X -= 0.65f;
    m_location.Y -= 0.75f;


    m_angle1.X += 0.040f * 40.0f * demoTime.DeltaTime;
    m_angle2.X -= 0.022f * 40.0f * demoTime.DeltaTime;

    m_angle1.Y -= 0.038f * 40.0f * demoTime.DeltaTime;
    m_angle2.Y += 0.023f * 40.0f * demoTime.DeltaTime;
  }



  void JuliaHelper::UpdateConstantLoad2(const DemoTime& demoTime)
  {
    m_location = Vector2(0.0f, 0.0f);

    const float s1x = 0.1f * 0.5f;
    const float s2x = 0.1f * 0.5f;
    const float s1y = 0.08f * 0.5f;
    const float s2y = 0.08f * 0.5f;

    float xMod = (std::sin(m_angle1.X) * s1x) + (std::cos(m_angle2.X) * s2x);
    float yMod = (std::sin(m_angle1.Y) * s1y) + (std::cos(m_angle2.X) * s2y);
    m_location.X += xMod;
    m_location.Y += yMod;

    m_location.X -= 0.65f;
    m_location.Y -= 0.65f;


    m_angle1.X += 0.040f * 40.0f * demoTime.DeltaTime;
    m_angle2.X -= 0.022f * 40.0f * demoTime.DeltaTime;

    m_angle1.Y -= 0.038f * 40.0f * demoTime.DeltaTime;
    m_angle2.Y += 0.023f * 40.0f * demoTime.DeltaTime;
  }


  void JuliaHelper::UpdateConstantLoad3(const DemoTime& demoTime)
  {
    m_location = Vector2(0.0f, 0.0f);

    const float s1x = 0.4f * 0.9f;
    const float s2x = 0.3f * 0.9f;
    const float s1y = 0.08f * 0.4f;
    const float s2y = 0.08f * 0.4f;

    float xMod = (std::sin(m_angle1.X) * s1x) + (std::cos(m_angle2.X) * s2x);
    float yMod = (std::sin(m_angle1.Y) * s1y) + (std::cos(m_angle2.X) * s2y);
    m_location.X += xMod;
    m_location.Y += yMod;

    m_location.Y -= 0.70f;

    const float speedScale = 0.25f;

    m_angle1.X += 0.040f * 20.0f * speedScale * demoTime.DeltaTime;
    m_angle2.X += 0.022f * 20.0f * speedScale * demoTime.DeltaTime;

    m_angle1.Y -= 0.038f * 40.0f * speedScale * demoTime.DeltaTime;
    m_angle2.Y += 0.023f * 40.0f * speedScale * demoTime.DeltaTime;
  }


  void JuliaHelper::UpdateComplexMove(const DemoTime& demoTime)
  {
    m_location = Vector2(0.4f, 0.45f);

    const float s1x = 0.24f * 0.5f;
    const float s2x = 0.39f * 0.5f;
    const float s1y = 0.20f * 0.5f;
    const float s2y = 0.32f * 0.5f;
    const float maxValueX = (s1x + s2x);
    const float maxValueY = (s1y + s2y);

    m_location.X += (std::sin(m_angle1.X) * s1x) + (std::cos(m_angle2.X) * s2x) + maxValueX;
    m_location.Y += (std::sin(m_angle1.Y) * s1y) + (std::cos(m_angle2.X) * s2y) + maxValueY;
    //m_location.X = 0;

    Matrix matrix;

    matrix = Matrix::CreateRotationZ(m_angle);
    Vector4 tmp = Vector4::Transform(Vector4(m_location.X, m_location.Y, 0.0f, 0.0f), matrix);

    m_location.X = tmp.X;
    m_location.Y = tmp.Y;

    m_angle1.X += 0.040f * 40.0f * demoTime.DeltaTime;
    m_angle2.X += 0.021f * 40.0f * demoTime.DeltaTime;
    m_angle1.Y -= 0.038f * 60.0f * demoTime.DeltaTime;
    m_angle2.Y += 0.023f * 60.0f * demoTime.DeltaTime;

    m_angle3.X += 0.009f * 10.0f * demoTime.DeltaTime;
    m_angle3.Y += 0.014f * 15.0f * demoTime.DeltaTime;

    m_angle = std::sin(m_angle3.X) * (MathHelper::TO_RADS * 180) + std::cos(m_angle3.Y) * (MathHelper::TO_RADS * 180);
  }


  void JuliaHelper::UpdateSlowAndComplex(const DemoTime& demoTime)
  {
    m_location = Vector2();

    const float s1x = 0.24f * 0.5f;
    const float s2x = 0.39f * 0.5f;
    const float s1y = 0.20f * 0.5f;
    const float s2y = 0.32f * 0.5f;
    const float maxValueX = (s1x + s2x);
    const float maxValueY = (s1y + s2y);

    m_location.X += (std::sin(m_angle1.X) * s1x) + (std::cos(m_angle2.X) * s2x) + maxValueX;
    m_location.Y += (std::sin(m_angle1.Y) * s1y) + (std::cos(m_angle2.X) * s2y) + maxValueY;

    Matrix matrix;

    matrix = Matrix::CreateRotationZ(m_angle);
    Vector4 tmp = Vector4::Transform(Vector4(m_location.X, m_location.Y, 0.0f, 0.0f), matrix);

    m_location.X = (tmp.X * 0.4f) - 0.7f;
    m_location.Y = (tmp.Y * 0.2f) - 0.6f;

    m_angle1.X += 0.040f * 40.0f * demoTime.DeltaTime;
    m_angle2.X += 0.021f * 40.0f * demoTime.DeltaTime;
    m_angle1.Y -= 0.038f * 60.0f * demoTime.DeltaTime;
    m_angle2.Y += 0.023f * 60.0f * demoTime.DeltaTime;

    m_angle3.X += 0.009f * 30.0f * demoTime.DeltaTime;
    m_angle3.Y += 0.014f * 15.0f * demoTime.DeltaTime;

    m_angle = std::sin(m_angle3.X) * (MathHelper::TO_RADS * 180) + std::cos(m_angle3.Y) * (MathHelper::TO_RADS * 180);
  }


  void JuliaHelper::UpdateDSin(const DemoTime& demoTime)
  {
    m_location = Vector2(0.285f, 0.01f);

    const float s1x = 0.4f;
    const float s2x = 0.4f;
    const float s1y = 0.4f;
    const float s2y = 0.4f;

    m_location.X += (std::sin(m_angle1.X) * s1x) + (std::cos(m_angle2.X) * s2x);
    m_location.Y += (std::sin(m_angle1.Y) * s1y) + (std::cos(m_angle2.X) * s2y);

    m_location.X -= 0.5f;
    m_location.Y -= 0.5f;

    m_angle1.X += 0.04f * 20.0f * demoTime.DeltaTime;
    m_angle2.X -= 0.022f * 20.0f * demoTime.DeltaTime;

    m_angle1.Y -= 0.038f * 20.0f * demoTime.DeltaTime;
    m_angle2.Y += 0.023f * 20.0f * demoTime.DeltaTime;
  }


  void JuliaHelper::Draw(const Point2& screenResolution)
  {
    if (m_atlasTexBanner.IsValid())
    {
      m_nativeBatch2D->Begin();
      m_nativeBatch2D->Draw(m_atlasTexBanner, Vector2(), Color::White());
      m_nativeBatch2D->End();
    }

    if (m_config.Show)
    {
      const int val = std::min(screenResolution.X, screenResolution.Y);
      Vector2 pos = m_location;
      pos.X *= (val * 0.5f);
      pos.Y *= (val * 0.5f);
      pos.X += (screenResolution.X * 0.5f);
      pos.Y += (screenResolution.Y * 0.5f);
      m_basic2D->Begin();
      m_basic2D->DrawPoints(&pos, 1, Color::Cyan());
      m_basic2D->End();
    }
  }
}
