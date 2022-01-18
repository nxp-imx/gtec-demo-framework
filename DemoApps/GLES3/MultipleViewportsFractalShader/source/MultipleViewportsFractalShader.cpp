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

#include "MultipleViewportsFractalShader.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <Shared/FractalShader/OptionParser.hpp>
#include <FslBase/String/StringUtil.hpp>
#include <GLES3/gl3.h>
#include <iostream>
#include <string>

namespace Fsl
{
  namespace
  {
  }


  MultipleViewportsFractalShader::MultipleViewportsFractalShader(const DemoAppConfig& config)
    : DemoAppGLES3(config)
    , m_scene0(config, 0)
    , m_scene1(config, 1)
    , m_scene2(config, 0)
    , m_scene3(config, 1)
  {
  }

  MultipleViewportsFractalShader::~MultipleViewportsFractalShader() = default;


  void MultipleViewportsFractalShader::Update(const DemoTime& demoTime)
  {
    m_scene0.Update(demoTime);
    m_scene1.Update(demoTime);
    m_scene2.Update(demoTime);
    m_scene3.Update(demoTime);
  }


  void MultipleViewportsFractalShader::Draw(const FrameInfo& frameInfo)
  {
    FSL_PARAM_NOT_USED(frameInfo);

    glClear(GL_COLOR_BUFFER_BIT);

    const auto extent = GetScreenExtent();

    const uint32_t x0 = 0;
    const uint32_t x1 = extent.Width / 2;
    const uint32_t x2 = extent.Width;
    const uint32_t y0 = 0;
    const uint32_t y1 = extent.Height / 2;
    const uint32_t y2 = extent.Height;

    // Top left
    glViewport(x0, y1, x1 - x0, y2 - y1);
    m_scene0.Draw();

    // Bottom left
    glViewport(x0, y0, x1 - x0, y1 - y0);
    m_scene1.Draw();

    // Top right
    glViewport(x1, y1, x2 - x1, y2 - y1);
    m_scene2.Draw();

    // Bottom right
    glViewport(x1, y0, x2 - x1, y1 - y0);
    m_scene3.Draw();

    // Restore the viewport
    glViewport(0, 0, extent.Width, extent.Height);
  }
}
