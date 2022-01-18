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

#include "Scissor101.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <GLES3/gl3.h>
#include <algorithm>
#include <cmath>

namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    const float SPEED = 0.8f;

    // B D
    // |\|
    // A C
    // A = 1.0
    const float CUBE_DIMENSIONS = 100.0f;

    const float CUBE_CEILING = CUBE_DIMENSIONS;
    const float CUBE_FLOOR = -CUBE_DIMENSIONS;

    const float CUBE_LEFT = -CUBE_DIMENSIONS;
    const float CUBE_RIGHT = CUBE_DIMENSIONS;
    const float CUBE_BACK = CUBE_DIMENSIONS;      // zBack
    const float CUBE_FRONT = -CUBE_DIMENSIONS;    // zFront
    const float CUBE_U0 = 0.0f;
    const float CUBE_U1 = 1.0f;
    const float CUBE_V0 = 1.0f;
    const float CUBE_V1 = 0.0f;


    const std::array<VertexPositionTexture, 6 * 6> g_vertices = {
      // Floor
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_BACK), Vector2(CUBE_U1, CUBE_V1)),      // LB
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_FRONT), Vector2(CUBE_U0, CUBE_V1)),     // LF
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_FRONT), Vector2(CUBE_U0, CUBE_V0)),    // RF

      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_BACK), Vector2(CUBE_U1, CUBE_V1)),      // LB
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_FRONT), Vector2(CUBE_U0, CUBE_V0)),    // RF
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_BACK), Vector2(CUBE_U1, CUBE_V0)),     // RB

      // Ceiling
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_FRONT), Vector2(CUBE_U1, CUBE_V1)),     // LF
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_BACK), Vector2(CUBE_U0, CUBE_V1)),      // LB
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_FRONT), Vector2(CUBE_U1, CUBE_V0)),    // RF

      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_FRONT), Vector2(CUBE_U1, CUBE_V0)),    // RF
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_BACK), Vector2(CUBE_U0, CUBE_V1)),      // LB
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_BACK), Vector2(CUBE_U0, CUBE_V0)),     // RB

      // Back wall
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_BACK), Vector2(CUBE_U1, CUBE_V0)),
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_BACK), Vector2(CUBE_U1, CUBE_V1)),
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_BACK), Vector2(CUBE_U0, CUBE_V1)),

      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_BACK), Vector2(CUBE_U1, CUBE_V0)),
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_BACK), Vector2(CUBE_U0, CUBE_V1)),
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_BACK), Vector2(CUBE_U0, CUBE_V0)),

      // Front wall
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_FRONT), Vector2(CUBE_U0, CUBE_V0)),
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_FRONT), Vector2(CUBE_U0, CUBE_V1)),
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_FRONT), Vector2(CUBE_U1, CUBE_V1)),

      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_FRONT), Vector2(CUBE_U0, CUBE_V0)),
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_FRONT), Vector2(CUBE_U1, CUBE_V1)),
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_FRONT), Vector2(CUBE_U1, CUBE_V0)),

      //// Right wall
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_BACK), Vector2(CUBE_U1, CUBE_V1)),       // FB
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_FRONT), Vector2(CUBE_U0, CUBE_V1)),      // FF
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_FRONT), Vector2(CUBE_U0, CUBE_V0)),    // CF

      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_BACK), Vector2(CUBE_U1, CUBE_V1)),       // FB
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_FRONT), Vector2(CUBE_U0, CUBE_V0)),    // CF
      VertexPositionTexture(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_BACK), Vector2(CUBE_U1, CUBE_V0)),     // CB

      // Left wall
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_FRONT), Vector2(CUBE_U1, CUBE_V1)),      // FF
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_BACK), Vector2(CUBE_U0, CUBE_V1)),       // FB
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_FRONT), Vector2(CUBE_U1, CUBE_V0)),    // CF

      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_FRONT), Vector2(CUBE_U1, CUBE_V0)),    // CF
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_BACK), Vector2(CUBE_U0, CUBE_V1)),       // FB
      VertexPositionTexture(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_BACK), Vector2(CUBE_U0, CUBE_V0)),     // CB
    };

    float Clamp(const float value)
    {
      // Basic clamping that works ok at low speeds
      if (value >= MathHelper::ToRadians(360))
      {
        return value - MathHelper::ToRadians(360);
      }
      if (value <= -MathHelper::ToRadians(360))
      {
        return value + MathHelper::ToRadians(360);
      }
      return value;
    }

    Vector2 Clamp(const Vector2& value)
    {
      return {Clamp(value.X), Clamp(value.Y)};
    }
  }


  Scissor101::Scissor101(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_options(config.GetOptions<OptionParser>())
    , m_speed1A(SPEED * -2.0f, SPEED * -0.9f)
    , m_speed1B(SPEED * 1.4f, SPEED * 1.4f)
    , m_speed2A(SPEED * -1.7f, SPEED * -2.2f)
    , m_speed2B(SPEED * 2.5f, SPEED * 1.6f)
    , m_angle1A(0.0f, 0.0f)
    , m_angle1B(MathHelper::ToRadians(90.0f), MathHelper::ToRadians(90.0f))
    , m_angle2A(0.0f, 0.0f)
    , m_angle2B(0.0f, 0.0f)
    , m_clipX(m_options->ScissorX)
    , m_clipY(m_options->ScissorY)
  {
    const std::shared_ptr<IContentManager> content = GetContentManager();
    m_programInfo = CreateProgram(content);
    m_texture = CreateTexture(content);
    m_vertexBufferInfo = CreateVertexBuffer(m_programInfo.Program);


    const float aspectRatio = GetWindowAspectRatio();
    m_matProj = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(75.0f), aspectRatio, 1.0f, 1000.0f);
    m_matTranslate = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
  }


  Scissor101::~Scissor101() = default;


  void Scissor101::OnKeyEvent(const KeyEvent& event)
  {
    if (event.IsHandled() || !event.IsPressed())
    {
      return;
    }

    switch (event.GetKey())
    {
    case VirtualKey::Code1:
      m_clipX = !m_clipX;
      break;
    case VirtualKey::Code2:
      m_clipY = !m_clipY;
      break;
    case VirtualKey::Code3:
      m_clipX = true;
      m_clipY = true;
      break;
    case VirtualKey::Code4:
      m_clipX = false;
      m_clipY = false;
      break;
    default:
      break;
    }
  }


  void Scissor101::Update(const DemoTime& demoTime)
  {
    m_angle.X -= 0.60f * demoTime.DeltaTime;
    m_angle.Y -= 0.50f * demoTime.DeltaTime;
    m_angle.Z -= 0.40f * demoTime.DeltaTime;

    // Rotate and translate the model view matrix
    m_matModel = Matrix::CreateRotationX(m_angle.X) * Matrix::CreateRotationY(m_angle.Y) * Matrix::CreateRotationZ(m_angle.Z) * m_matTranslate;

    {    // Do some funky double sinus movement we can use for a bit of unpredictable clipping
      const auto resolutionPx = GetWindowSizePx();
      const Vector2 dist(TypeConverter::UncheckedTo<Vector2>(resolutionPx));

      const auto clip1X = (2.0f + (std::sin(m_angle1A.X) + std::sin(m_angle1B.X))) * dist.X / 4.0f;
      const auto clip1Y = (2.0f + (std::sin(m_angle1A.Y) + std::sin(m_angle1B.Y))) * dist.Y / 4.0f;
      const auto clip2X = (2.0f + (std::sin(m_angle2A.X) + std::sin(m_angle2B.X))) * dist.X / 4.0f;
      const auto clip2Y = (2.0f + (std::sin(m_angle2A.Y) + std::sin(m_angle2B.Y))) * dist.Y / 4.0f;
      m_clip1.X = std::min(std::max(static_cast<int32_t>(clip1X), 0), resolutionPx.Width());
      m_clip1.Y = std::min(std::max(static_cast<int32_t>(clip1Y), 0), resolutionPx.Height());
      m_clip2.X = std::min(std::max(static_cast<int32_t>(clip2X), 0), resolutionPx.Width());
      m_clip2.Y = std::min(std::max(static_cast<int32_t>(clip2Y), 0), resolutionPx.Height());

      m_angle1A += m_speed1A * demoTime.DeltaTime;
      m_angle1B += m_speed1B * demoTime.DeltaTime;
      m_angle2A += m_speed2A * demoTime.DeltaTime;
      m_angle2B += m_speed2B * demoTime.DeltaTime;

      // Clamp the angles to prevent weird issues when the float angles grow
      m_angle1A = Clamp(m_angle1A);
      m_angle1B = Clamp(m_angle1B);
      m_angle2A = Clamp(m_angle2A);
      m_angle2B = Clamp(m_angle2B);
    }
  }


  void Scissor101::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    const auto windowSizePx = GetWindowSizePx();
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);

    assert(m_clip1.X >= 0 && m_clip1.X <= windowSizePx.Width());
    assert(m_clip1.Y >= 0 && m_clip1.Y <= windowSizePx.Height());
    assert(m_clip2.X >= 0 && m_clip2.X <= windowSizePx.Width());
    assert(m_clip2.Y >= 0 && m_clip2.Y <= windowSizePx.Height());

    Point2 nearClip = m_clip1;
    Point2 farClip = m_clip2;
    if (m_clip1.X > m_clip2.X)
    {
      farClip.X = m_clip1.X;
      nearClip.X = m_clip2.X;
    }
    if (m_clip1.Y > m_clip2.Y)
    {
      farClip.Y = m_clip1.Y;
      nearClip.Y = m_clip2.Y;
    }

    int32_t clipX = m_clipX ? nearClip.X : 0;
    int32_t clipY = m_clipY ? nearClip.Y : 0;
    int32_t clipWidth = m_clipX ? (farClip.X - nearClip.X) : windowSizePx.Width();
    int32_t clipHeight = m_clipY ? (farClip.Y - nearClip.Y) : windowSizePx.Height();

    if (clipX < 0 || (clipX + clipWidth) > windowSizePx.Width() || clipY < 0 || (clipY + clipHeight) > windowSizePx.Height())
    {
      throw std::runtime_error("Scissor rect out of bounds");
    }

    glEnable(GL_SCISSOR_TEST);

    if (m_options->ForceInvalidWidth)
    {
      clipWidth += (windowSizePx.Width() - (clipX + clipWidth)) + 10;
    }
    if (m_options->ForceInvalidHeight)
    {
      clipHeight += (windowSizePx.Height() - (clipY + clipHeight)) + 10;
    }

    glScissor(clipX, clipY, clipWidth, clipHeight);

    DrawCube(m_programInfo, m_matModel);

    glDisable(GL_SCISSOR_TEST);
  }


  void Scissor101::DrawCube(const ProgramInfo& programInfo, const Matrix& matModel)
  {
    FSL_PARAM_NOT_USED(matModel);

    const auto& vertexBuffer = m_vertexBufferInfo.VertexBuffer;

    // Set the shader program
    glUseProgram(programInfo.Program.Get());

    // Load the matrices
    glUniformMatrix4fv(programInfo.Location.ModelViewMatrix, 1, 0, m_matModel.DirectAccess());
    glUniformMatrix4fv(programInfo.Location.ProjMatrix, 1, 0, m_matProj.DirectAccess());

    glBindBuffer(vertexBuffer.GetTarget(), vertexBuffer.Get());

    // Bind the vertex attributes
    vertexBuffer.EnableAttribArrays(m_vertexBufferInfo.AttribLink.data(), m_vertexBufferInfo.AttribLink.size());

    // Select Our Texture
    glActiveTexture(GL_TEXTURE0);
    glDrawArrays(GL_TRIANGLES, 0, vertexBuffer.GetCapacity());

    // Cleanup
    vertexBuffer.DisableAttribArrays();
  }


  Scissor101::ProgramInfo Scissor101::CreateProgram(const std::shared_ptr<IContentManager>& contentManager)
  {
    ProgramInfo info;
    info.Program.Reset(contentManager->ReadAllText("Shader.vert"), contentManager->ReadAllText("Shader.frag"));

    // Get uniform locations
    info.Location.ModelViewMatrix = info.Program.GetUniformLocation("g_matModelView");
    info.Location.ProjMatrix = info.Program.GetUniformLocation("g_matProj");
    return info;
  }


  GLES3::GLTexture Scissor101::CreateTexture(const std::shared_ptr<IContentManager>& contentManager)
  {
    // Load the texture (we use a scope here, so the bitmap objects is thrown away as soon as we dont need it)
    Texture texture = contentManager->ReadTexture("Texturing.png", PixelFormat::R8G8B8_UNORM);
    GLTextureParameters params(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
    return GLTexture(texture, params, TextureFlags::GenerateMipMaps);
  }


  Scissor101::VertexBufferInfo Scissor101::CreateVertexBuffer(const GLES3::GLProgram& program)
  {
    VertexBufferInfo info;
    // Prepare the vertex buffer
    info.VertexBuffer.Reset(g_vertices.data(), g_vertices.size(), GL_STATIC_DRAW);

    constexpr auto vertexDecl = VertexPositionTexture::GetVertexDeclarationArray();
    info.AttribLink[0] =
      GLVertexAttribLink(program.GetAttribLocation("VertexPosition"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
    info.AttribLink[1] =
      GLVertexAttribLink(program.GetAttribLocation("VertexTexCoord"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0));
    return info;
  }
}
