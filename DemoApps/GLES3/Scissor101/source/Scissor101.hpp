#ifndef GLES3_SCISSOR101_SCISSOR101_HPP
#define GLES3_SCISSOR101_SCISSOR101_HPP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslDemoApp/OpenGLES3/DemoAppGLES3.hpp>
#include <FslUtil/OpenGLES3/GLProgram.hpp>
#include <FslUtil/OpenGLES3/GLTexture.hpp>
#include <FslUtil/OpenGLES3/GLVertexBuffer.hpp>
#include <FslUtil/OpenGLES3/GLValues.hpp>
#include <Shared/Scissor101/OptionParser.hpp>
#include <array>


namespace Fsl
{
  class Scissor101 : public DemoAppGLES3
  {
    struct ProgramLocation
    {
      GLint ModelViewMatrix;
      GLint ProjMatrix;

      ProgramLocation()
        : ModelViewMatrix(GLES3::GLValues::INVALID_LOCATION)
        , ProjMatrix(GLES3::GLValues::INVALID_LOCATION)
      {
      }
    };

    struct ProgramInfo
    {
      GLES3::GLProgram Program;
      ProgramLocation Location;
    };

    struct VertexBufferInfo
    {
      GLES3::GLVertexBuffer VertexBuffer;
      std::array<GLES3::GLVertexAttribLink, 2> AttribLink;
    };

    std::shared_ptr<OptionParser> m_options;
    ProgramInfo m_programInfo;
    GLES3::GLTexture m_texture;
    VertexBufferInfo m_vertexBufferInfo;
    Matrix m_matModel;
    Matrix m_matProj;
    Matrix m_matTranslate;
    Vector3 m_angle;
    Vector2 m_speed1A;
    Vector2 m_speed1B;
    Vector2 m_speed2A;
    Vector2 m_speed2B;
    Vector2 m_angle1A;
    Vector2 m_angle1B;
    Vector2 m_angle2A;
    Vector2 m_angle2B;
    Point2 m_clip1;
    Point2 m_clip2;
    bool m_clipX;
    bool m_clipY;

  public:
    explicit Scissor101(const DemoAppConfig& config);
    ~Scissor101() override;

  protected:
    void OnKeyEvent(const KeyEvent& event) override;
    void Update(const DemoTime& demoTime) override;
    void Draw(const DemoTime& demoTime) override;

  private:
    void DrawCube(const ProgramInfo& programInfo, const Matrix& matModel);
    ProgramInfo CreateProgram(const std::shared_ptr<IContentManager>& contentManager);
    GLES3::GLTexture CreateTexture(const std::shared_ptr<IContentManager>& contentManager);
    VertexBufferInfo CreateVertexBuffer(const GLES3::GLProgram& program);
  };
}

#endif
