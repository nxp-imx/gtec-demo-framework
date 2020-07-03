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

#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <FslUtil/OpenGLES2/GLCheck.hpp>
#include "FractalShader.hpp"
#include "FractalShaderJulia.hpp"
#include "FractalShaderMandelbrot.hpp"
#include <Shared/FractalShader/OptionParser.hpp>
#include <FslBase/String/StringUtil.hpp>
#include <GLES2/gl2.h>
#include <iostream>
#include <memory>
#include <string>

namespace Fsl
{
  namespace
  {
  }


  FractalShader::FractalShader(const DemoAppConfig& config)
    : DemoAppGLES2(config)
  {
    std::shared_ptr<OptionParser> options = config.GetOptions<OptionParser>();
    Config cfg = options->GetConfig();

    switch (cfg.Scene)
    {
    case 1:
      m_scene = std::make_shared<FractalShaderMandelbrot>(config);
      break;
    case 0:
    default:
      m_scene = std::make_shared<FractalShaderJulia>(config);
      break;
    }
  }


  FractalShader::~FractalShader() = default;


  void FractalShader::Update(const DemoTime& demoTime)
  {
    m_scene->Update(demoTime);
  }


  void FractalShader::Draw(const DemoTime& /*demoTime*/)
  {
    m_scene->Draw();
  }
}
