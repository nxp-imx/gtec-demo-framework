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

#include "S06_Texturing.hpp"
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include <GLES2/gl2.h>
#include <array>
#include <iostream>
#include <vector>

namespace Fsl
{
  using namespace GLES2;

  namespace
  {
    // The index in these variables should match the g_pszShaderAttributeArray ordering
    constexpr GLuint VertexLoc = 0;
    constexpr GLuint ColorLoc = 1;
    constexpr GLuint VertexTexLoc = 2;
    constexpr std::array<GLES2::GLBindAttribLocation, 3> ShaderAttributeArray = {GLES2::GLBindAttribLocation(VertexLoc, "g_vPosition"),
                                                                                 GLES2::GLBindAttribLocation(ColorLoc, "g_vColor"),
                                                                                 GLES2::GLBindAttribLocation(VertexTexLoc, "g_vTexCoord")};

    const std::array<float, 24 * 3> g_vertexPositions = {
      // Draw A Quad

      // Top Right Of The Quad (Top)
      1.0f, 1.0f, -1.0f,
      // Top Left Of The Quad (Top)
      -1.0f, 1.0f, -1.0f,
      // Bottom Right Of The Quad (Top)
      1.0f, 1.0f, 1.0f,
      // Bottom Left Of The Quad (Top)
      -1.0f, 1.0f, 1.0f,
      // Top Right Of The Quad (Bottom)
      1.0f, -1.0f, 1.0f,
      // Top Left Of The Quad (Bottom)
      -1.0f, -1.0f, 1.0f,
      // Bottom Right Of The Quad (Bottom)
      1.0f, -1.0f, -1.0f,
      // Bottom Left Of The Quad (Bottom)
      -1.0f, -1.0f, -1.0f,
      // Top Right Of The Quad (Front)
      1.0f, 1.0f, 1.0f,
      // Top Left Of The Quad (Front)
      -1.0f, 1.0f, 1.0f,
      // Bottom Right Of The Quad (Front)
      1.0f, -1.0f, 1.0f,
      // Bottom Left Of The Quad (Front)
      -1.0f, -1.0f, 1.0f,

      // Top Right Of The Quad (Back)
      1.0f, -1.0f, -1.0f,
      // Top Left Of The Quad (Back)
      -1.0f, -1.0f, -1.0f,
      // Bottom Right Of The Quad (Back)
      1.0f, 1.0f, -1.0f,
      // Bottom Left Of The Quad (Back)
      -1.0f, 1.0f, -1.0f,

      // Top Right Of The Quad (Left)
      -1.0f, 1.0f, 1.0f,
      // Top Left Of The Quad (Left)
      -1.0f, 1.0f, -1.0f,
      // Bottom Right Of The Quad (Left)
      -1.0f, -1.0f, 1.0f,
      // Bottom Left Of The Quad (Left)
      -1.0f, -1.0f, -1.0f,
      // Top Right Of The Quad (Right)
      1.0f, 1.0f, -1.0f,
      // Top Left Of The Quad (Right)
      1.0f, 1.0f, 1.0f,
      // Bottom Right Of The Quad (Right)
      1.0f, -1.0f, -1.0f,
      // Bottom Left Of The Quad (Right)
      1.0f, -1.0f, 1.0f};

    const std::array<float, 24 * 2> g_vertexTexCoords = {
      // Top Face
      0.0f,
      0.0f,
      1.0f,
      0.0f,
      0.0f,
      1.0f,
      1.0f,
      1.0f,
      // Bottom Face
      1.0f,
      1.0f,
      0.0f,
      1.0f,
      1.0f,
      0.0f,
      0.0f,
      0.0f,

      // Front Face

      1.0f,
      1.0f,
      0.0f,
      1.0f,
      1.0f,
      0.0f,
      0.0f,
      0.0f,
      // Back Face
      0.0f,
      0.0f,
      1.0f,
      0.0f,
      0.0f,
      1.0f,
      1.0f,
      1.0f,
      // left face
      1.0f,
      1.0f,
      0.0f,
      1.0f,
      1.0f,
      0.0f,
      0.0f,
      0.0f,

      // Right face
      1.0f,
      1.0f,
      0.0f,
      1.0f,
      1.0f,
      0.0f,
      0.0f,
      0.0f,

    };

    const std::array<float, 24 * 4> g_vertexColors = {
      // Red
      1.0f, 0.0f, 0.0f, 1.0f,
      // Red
      1.0f, 0.0f, 0.0f, 1.0f,

      // Green
      0.0f, 1.0f, 0.0f, 1.0f,

      // Green
      0.0f, 1.0f, 0.0f, 1.0f,

      // Blue
      0.0f, 0.0f, 1.0f, 1.0f,

      // Blue
      0.0f, 0.0f, 1.0f, 1.0f,

      // Red
      1.0f, 0.0, 0.0f, 1.0f,
      // Red
      1.0f, 0.0, 0.0f, 1.0f,
      // Blue
      0.0f, 0.0f, 1.0f, 1.0f,
      // Blue
      0.0f, 0.0f, 1.0f, 1.0f,
      // Green
      0.0f, 1.0f, 0.0f, 1.0f,
      // Green
      0.0f, 1.0f, 0.0f, 1.0f,
      // Red
      1.0f, 0.0f, 0.0f, 1.0f,
      // Red
      1.0f, 0.0f, 0.0f, 1.0f,
      // Green
      0.0f, 1.0f, 0.0f, 1.0f,
      // Green
      0.0f, 1.0f, 0.0f, 1.0f,
      // Blue
      0.0f, 0.0f, 1.0f, 1.0f,
      // Blue
      0.0f, 0.0f, 1.0f, 1.0f,
      // Red
      1.0f, 0.0f, 0.0f, 1.0f,
      // Red
      1.0f, 0.0f, 0.0f, 1.0f,
      // Blue
      0.0f, 0.0f, 1.0f, 1.0f,
      // Green
      0.0f, 1.0f, 0.0f, 1.0f,
      // Blue
      0.0f, 0.0f, 1.0f, 1.0f,

      // Green
      0.0f, 1.0f, 0.0f, 1.0f};
  }


  S06_Texturing::S06_Texturing(const DemoAppConfig& config)
    : DemoAppGLES2(config)
    , m_hModelViewMatrixLoc(0)
    , m_hProjMatrixLoc(0)
  {
    const std::shared_ptr<IContentManager> content = GetContentManager();
    m_program.Reset(content->ReadAllText("Shader.vert"), content->ReadAllText("Shader.frag"), SpanUtil::AsReadOnlySpan(ShaderAttributeArray));

    {    // Load the texture (we use a scope here, so the bitmap objects is thrown away as soon as we dont need it)
      Bitmap bitmap;
      content->Read(bitmap, "GPUSdk/SquareLogo512x512.jpg", PixelFormat::R8G8B8_UNORM);
      GLTextureParameters params(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
      m_texture.SetData(bitmap, params);
    }

    const GLuint hProgram = m_program.Get();

    // Get uniform locations
    m_hModelViewMatrixLoc = glGetUniformLocation(hProgram, "g_matModelView");
    m_hProjMatrixLoc = glGetUniformLocation(hProgram, "g_matProj");

    const float aspectRatio = GetWindowAspectRatio();
    m_matProj = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(60.0f), aspectRatio, 1.0f, 1000.0f);
    m_matTranslate = Matrix::CreateTranslation(0.0f, 0.0f, -6.0f);
  }


  S06_Texturing::~S06_Texturing() = default;


  void S06_Texturing::Update(const DemoTime& demoTime)
  {
    m_angle.X -= 0.60f * demoTime.DeltaTime;
    m_angle.Y -= 0.50f * demoTime.DeltaTime;
    m_angle.Z -= 0.40f * demoTime.DeltaTime;
  }


  void S06_Texturing::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    // Rotate and translate the model view matrix
    const Matrix matModel =
      Matrix::CreateRotationX(m_angle.X) * Matrix::CreateRotationY(m_angle.Y) * Matrix::CreateRotationZ(m_angle.Z) * m_matTranslate;

    const PxSize2D currentSizePx = GetWindowSizePx();
    glViewport(0, 0, currentSizePx.RawWidth(), currentSizePx.RawHeight());

    // Clear the color-buffer and depth-buffer
    glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // If enabled, do depth comparisons and update the depth buffer
    glEnable(GL_DEPTH_TEST);
    // If enabled, cull polygons based on their winding in window coordinates
    glEnable(GL_CULL_FACE);
    // Culls back face
    glCullFace(GL_BACK);

    // Set the shader program
    glUseProgram(m_program.Get());

    // Load the matrices
    glUniformMatrix4fv(m_hModelViewMatrixLoc, 1, 0, matModel.DirectAccess());
    glUniformMatrix4fv(m_hProjMatrixLoc, 1, 0, m_matProj.DirectAccess());

    // Bind the vertex attributes
    glVertexAttribPointer(VertexLoc, 3, GL_FLOAT, 0, 0, g_vertexPositions.data());
    glEnableVertexAttribArray(VertexLoc);

    glVertexAttribPointer(ColorLoc, 4, GL_FLOAT, 0, 0, g_vertexColors.data());
    glEnableVertexAttribArray(ColorLoc);

    glVertexAttribPointer(VertexTexLoc, 2, GL_FLOAT, 0, 0, g_vertexTexCoords.data());
    glEnableVertexAttribArray(VertexTexLoc);

    // Select Our Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture.Get());

    // Drawing Using Triangle strips, draw triangle strips using 4 vertices
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

    // Cleanup
    glDisableVertexAttribArray(VertexLoc);
    glDisableVertexAttribArray(ColorLoc);
    glDisableVertexAttribArray(VertexTexLoc);
  }
}
