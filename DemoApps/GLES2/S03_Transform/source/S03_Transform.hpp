#ifndef GLES2_S03_TRANSFORM_S03_TRANSFORM_HPP
#define GLES2_S03_TRANSFORM_S03_TRANSFORM_HPP

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

#include <FslBase/Math/Matrix.hpp>
#include <FslDemoApp/OpenGLES2/DemoAppGLES2.hpp>
#include <FslUtil/OpenGLES2/GLProgram.hpp>

namespace Fsl
{
  class S03_Transform : public DemoAppGLES2
  {
    GLES2::GLProgram m_program;

    // Global Variables, attribute and uniform
    GLint m_locVertices;
    GLint m_locColors;
    GLint m_locTransformMat;

    float m_angle;
    Matrix m_matTransform;

  public:
    S03_Transform(const DemoAppConfig& config);
    ~S03_Transform() override;

  protected:
    void Update(const DemoTime& demoTime) override;
    void Draw(const DemoTime& demoTime) override;
  };
}

#endif
