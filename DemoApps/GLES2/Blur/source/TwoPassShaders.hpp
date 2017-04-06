#ifndef GLES2_BLUR_TWOPASSSHADERS_HPP
#define GLES2_BLUR_TWOPASSSHADERS_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslGraphicsGLES2/GLProgram.hpp>
#include <memory>
#include <vector>
#include "Config.hpp"

namespace Fsl
{
  class IContentManager;

  class TwoPassShaders
  {
  public:
    enum Enum
    {
      Normal,
      Linear,
    };

    GLES2::GLProgram ProgramNormal;
    GLES2::GLProgram ProgramBlurX;
    GLES2::GLProgram ProgramBlurY;

    TwoPassShaders();
    void Reset(const std::shared_ptr<IContentManager>& contentManager, const int32_t kernelLength, const float sigma,
               const Point2& sizeTextureBlurX, const Point2& sizeTextureBlurY,
               const TwoPassShaders::Enum mode, const ShaderType::Enum shaderType);
    void Reset(const std::shared_ptr<IContentManager>& contentManager, const std::vector<double>& kernelSlice,
               const Point2& sizeTextureBlurX, const Point2& sizeTextureBlurY,
               const TwoPassShaders::Enum mode, const ShaderType::Enum shaderType);
  };
}

#endif
