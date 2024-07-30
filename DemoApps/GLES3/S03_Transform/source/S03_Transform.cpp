/*
 * Copyright (c) 2012 Freescale Semiconductor, Inc.
 */

/****************************************************************************
 *
 *    Copyright 2012 Vivante Corporation, Sunnyvale, California.
 *    All Rights Reserved.
 *
 *    Permission is hereby granted, free of charge, to any person obtaining
 *    a copy of this software and associated documentation files (the
 *    'Software'), to deal in the Software without restriction, including
 *    without limitation the rights to use, copy, modify, merge, publish,
 *    distribute, sub license, and/or sell copies of the Software, and to
 *    permit persons to whom the Software is furnished to do so, subject
 *    to the following conditions:
 *
 *    The above copyright notice and this permission notice (including the
 *    next paragraph) shall be included in all copies or substantial
 *    portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 *    IN NO EVENT SHALL VIVANTE AND/OR ITS SUPPLIERS BE LIABLE FOR ANY
 *    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/

#include "S03_Transform.hpp"
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <GLES3/gl3.h>
#include <array>
#include <iostream>

namespace Fsl
{
  namespace
  {
    // Triangle Vertex positions.
    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    const GLfloat g_vertices[3][2] = {{-0.5f, -0.5f}, {0.0f, 0.5f}, {0.5f, -0.5f}};

    // Triangle Vertex colors.
    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    const GLfloat g_color[3][3] = {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};

    const std::array<GLES3::GLBindAttribLocation, 2> g_shaderAttributeArray = {GLES3::GLBindAttribLocation(0, "my_Vertex"),
                                                                               GLES3::GLBindAttribLocation(1, "my_Color")};
  }


  S03_Transform::S03_Transform(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_locVertices(0)
    , m_locColors(0)
    , m_locTransformMat(0)
    , m_angle(0)
  {
    const std::shared_ptr<IContentManager> content = GetContentManager();
    m_program.Reset(content->ReadAllText("Shader.vert"), content->ReadAllText("Shader.frag"), SpanUtil::AsReadOnlySpan(g_shaderAttributeArray));

    const GLuint hProgram = m_program.Get();

    // Grab location of shader attributes.
    GL_CHECK(m_locVertices = glGetAttribLocation(hProgram, "my_Vertex"));
    GL_CHECK(m_locColors = glGetAttribLocation(hProgram, "my_Color"));
    // Transform Matrix is uniform for all vertices here.
    GL_CHECK(m_locTransformMat = glGetUniformLocation(hProgram, "my_TransformMatrix"));
    GL_CHECK(glUseProgram(hProgram));

    // enable vertex arrays to push the data.
    GL_CHECK(glEnableVertexAttribArray(m_locVertices));
    GL_CHECK(glEnableVertexAttribArray(m_locColors));

    // set data in the arrays.
    GL_CHECK(glVertexAttribPointer(m_locVertices, 2, GL_FLOAT, GL_FALSE, 0, &g_vertices[0][0]));
    GL_CHECK(glVertexAttribPointer(m_locColors, 3, GL_FLOAT, GL_FALSE, 0, &g_color[0][0]));
    GL_CHECK(glUniformMatrix4fv(m_locTransformMat, 1, GL_FALSE, m_matTransform.DirectAccess()));

    // If enabled, do depth comparisons and update the depth buffer
    GL_CHECK(glEnable(GL_DEPTH_TEST));
  }


  S03_Transform::~S03_Transform() = default;


  void S03_Transform::Update(const DemoTime& demoTime)
  {
    // Set up rotation matrix rotating by angle around y axis.
    m_matTransform = Matrix::CreateRotationY(m_angle);

    // Use demoTime.DeltaTime to make the animation frame rate independent.
    m_angle += 0.8f * demoTime.DeltaTime;
  }


  void S03_Transform::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    const PxSize2D currentSizePx = GetWindowSizePx();
    glViewport(0, 0, currentSizePx.RawWidth(), currentSizePx.RawHeight());

    glUniformMatrix4fv(m_locTransformMat, 1, GL_FALSE, m_matTransform.DirectAccess());

    // Clear background.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    // flush all commands.
    glFlush();
  }
}
