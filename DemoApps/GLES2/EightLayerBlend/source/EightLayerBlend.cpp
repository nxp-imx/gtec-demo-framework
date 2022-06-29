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

#include "EightLayerBlend.hpp"
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include <GLES2/gl2.h>
#include <cmath>
#include <iostream>

namespace Fsl
{
  using namespace GLES2;


  EightLayerBlend::EightLayerBlend(const DemoAppConfig& config)
    : DemoAppGLES2(config)
    , m_locMatModelViewProj(-1)
    , m_angle(0)
  {
    const std::shared_ptr<IContentManager> contentManager = GetContentManager();

    m_program.Reset(contentManager->ReadAllText("BasicShader.vert"), contentManager->ReadAllText("BasicShader.frag"));

    {
      Bitmap bitmap;
      GLTextureParameters texParms(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
      // contentManager->Read(bitmap, "Textures/EightLayers/1_new.jpg", PixelFormat::R8G8B8_UNORM);
      contentManager->Read(bitmap, "Textures/EightLayers/1_old.jpg", PixelFormat::R8G8B8_UNORM);
      m_layers[0].Texture.Reset(bitmap, texParms);
      contentManager->Read(bitmap, "Textures/EightLayers/2.png", PixelFormat::R8G8B8A8_UNORM);
      m_layers[1].Texture.Reset(bitmap, texParms);
      contentManager->Read(bitmap, "Textures/EightLayers/3.png", PixelFormat::R8G8B8A8_UNORM);
      m_layers[2].Texture.Reset(bitmap, texParms);
      contentManager->Read(bitmap, "Textures/EightLayers/4.png", PixelFormat::R8G8B8A8_UNORM);
      m_layers[3].Texture.Reset(bitmap, texParms);
      contentManager->Read(bitmap, "Textures/EightLayers/5.png", PixelFormat::R8G8B8A8_UNORM);
      m_layers[4].Texture.Reset(bitmap, texParms);
      contentManager->Read(bitmap, "Textures/EightLayers/6.png", PixelFormat::R8G8B8A8_UNORM);
      m_layers[5].Texture.Reset(bitmap, texParms);
      contentManager->Read(bitmap, "Textures/EightLayers/7.png", PixelFormat::R8G8B8A8_UNORM);
      m_layers[6].Texture.Reset(bitmap, texParms);
      contentManager->Read(bitmap, "Textures/EightLayers/8.png", PixelFormat::R8G8B8A8_UNORM);
      m_layers[7].Texture.Reset(bitmap, texParms);
    }

    m_layerSize = Point2(3840, 1080);
    const float x1 = 0.0f;
    const float y1 = 0.0f;
    const auto x2 = static_cast<float>(m_layerSize.X);
    const auto y2 = static_cast<float>(m_layerSize.Y);
    const float z = 5;


    const float u1 = 0.0f;
    const float v1 = 0.0f;
    const float u2 = 1.0f;
    const float v2 = 1.0f;

    const std::array<VertexPositionTexture, 4> vertices = {
      VertexPositionTexture(Vector3(x1, y2, z), Vector2(u1, v2)),
      VertexPositionTexture(Vector3(x1, y1, z), Vector2(u1, v1)),
      VertexPositionTexture(Vector3(x2, y2, z), Vector2(u2, v2)),
      VertexPositionTexture(Vector3(x2, y1, z), Vector2(u2, v1)),
    };

    m_vertexBuffer.Reset(vertices, GL_STATIC_DRAW);

    m_locMatModelViewProj = GL_CHECK(glGetUniformLocation(m_program.Get(), "g_matModelViewProj"));
    GL_CHECK_FOR_ERROR();
  }


  EightLayerBlend::~EightLayerBlend() = default;


  void EightLayerBlend::Update(const DemoTime& demoTime)
  {
    float pos = std::sin(m_angle);

    float xRangeFirst = (1200.0f / 2.0f);
    float xRangeLast = (1920.0f / 2.0f);
    float xRangeAdd = (xRangeLast - xRangeFirst) / static_cast<float>(m_layers.size() - 1u);
    float xRange = xRangeFirst;
    for (std::size_t i = 0; i < m_layers.size(); ++i)
    {
      m_layers[i].Position = pos * xRange;
      xRange += xRangeAdd;
    }

    const PxSize2D res = GetWindowSizePx();
    m_matViewProj = Matrix::CreateOrthographic(static_cast<float>(res.Width()), static_cast<float>(res.Height()), 1.0f, 10.0f);

    m_angle += 0.4f * demoTime.DeltaTime;
    if (m_angle > (MathHelper::TO_RADS * 360.0f))
    {
      m_angle -= (MathHelper::TO_RADS * 360.0f);
    }
  }


  void EightLayerBlend::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    const GLuint hProgram = m_program.Get();

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    glUseProgram(hProgram);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUniformMatrix4fv(m_locMatModelViewProj, 1, GL_FALSE, m_matViewProj.DirectAccess());


    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_layers[0].Texture.Get());

    const Point2 res(1920, 1080);


    // Draw the opaque background layer
    glBindBuffer(m_vertexBuffer.GetTarget(), m_vertexBuffer.Get());
    m_vertexBuffer.EnableAttribArrays();

    auto positionX = static_cast<int>((static_cast<float>(-m_layerSize.X) / 2.0f) + m_layers[0].Position);
    Matrix matViewProj = Matrix::CreateTranslation(static_cast<float>(positionX), static_cast<float>(-m_layerSize.Y) / 2.0f, 0.0f) *
                         Matrix::CreateOrthographic(static_cast<float>(res.X), static_cast<float>(res.Y), 1.0f, 10.0f);
    glUniformMatrix4fv(m_locMatModelViewProj, 1, GL_FALSE, matViewProj.DirectAccess());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // matViewProj = Matrix::CreateTranslation(positionX - m_layerSize.X, -m_layerSize.Y / 2, 0) * Matrix::CreateOrthographic(res.X, res.Y,
    // 1.0f, 10.0f);  glUniformMatrix4fv(m_locMatModelViewProj, 1, GL_FALSE, matViewProj.DirectAccess());  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glEnable(GL_BLEND);
    for (std::size_t i = 1; i < m_layers.size(); ++i)
    {
      glBindTexture(GL_TEXTURE_2D, m_layers[i].Texture.Get());

      positionX = static_cast<int>((static_cast<float>(-m_layerSize.X) / 2.0f) + m_layers[i].Position);

      matViewProj = Matrix::CreateTranslation(static_cast<float>(positionX), static_cast<float>(-m_layerSize.Y) / 2.0f, 0.0f) *
                    Matrix::CreateOrthographic(static_cast<float>(res.X), static_cast<float>(res.Y), 1.0f, 10.0f);
      glUniformMatrix4fv(m_locMatModelViewProj, 1, GL_FALSE, matViewProj.DirectAccess());

      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

      // matViewProj = Matrix::CreateTranslation(positionX - m_layerSize.X, -m_layerSize.Y / 2, 0) * Matrix::CreateOrthographic(res.X, res.Y,
      // 1.0f, 10.0f);  glUniformMatrix4fv(m_locMatModelViewProj, 1, GL_FALSE, matViewProj.DirectAccess());

      // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    m_vertexBuffer.DisableAttribArrays();

    glBindBuffer(m_vertexBuffer.GetTarget(), 0);
    glFinish();
    // GL_CHECK_FOR_ERROR();
  }
}
