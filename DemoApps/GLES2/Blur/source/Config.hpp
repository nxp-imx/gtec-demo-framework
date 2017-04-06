#ifndef GLES2_BLUR_CONFIG_HPP
#define GLES2_BLUR_CONFIG_HPP
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

#include <FslBase/Math/Point2.hpp>
#include <FslBase/Exceptions.hpp>

namespace Fsl
{


  struct RenderMode
  {
    enum Enum
    {
      ReferenceTwoPass,
      ReferenceTwoPassLinear,
      TwoPassLinarScaled,
      ReferenceOnePass,
      COUNT
    };
  };


  struct ShaderType
  {
    enum Enum
    {
      HardCoded,
      NonDependent,
      COUNT
    };
  };

  struct CaptionType
  {
    enum Enum
    {
      Disabled,
      Algorithm,
      Description,
      COUNT
    };
  };

  class Config
  {
  public:
    RenderMode::Enum TheRenderMode;
    int32_t KernelLength;
    float Sigma;
    ShaderType::Enum TheShaderType;
    bool CompareEnabled;
    uint32_t CRMFlags;
    CaptionType::Enum TheCaptionType;
    bool UseOptimalSigma;

    Config()
      : TheRenderMode(RenderMode::TwoPassLinarScaled)
      , KernelLength(75)
      , Sigma(17)
      , TheShaderType(ShaderType::HardCoded)
      , CompareEnabled(false)
      , CRMFlags(0xFFFFFFFF & ~(1 << int(RenderMode::ReferenceOnePass)))
      , TheCaptionType(CaptionType::Algorithm)
      , UseOptimalSigma(false)
    {
    }

    void SetRenderMode(const RenderMode::Enum value)
    {
      TheRenderMode = value;
    }

    void SetRenderMode(const int32_t value)
    {
      if (value < 0 || value >= static_cast<int32_t>(RenderMode::COUNT))
        throw std::invalid_argument("invalid render mode");
      TheRenderMode = static_cast<RenderMode::Enum>(value);
    }

    void SetKernelLength(const int32_t value)
    {
      if (value < 1)
        throw std::invalid_argument("invalid kernel length");
      KernelLength = value;
    }

    void SetSigma(const float value)
    {
      Sigma = value;
    }

    void SetShaderType(const ShaderType::Enum value)
    {
      TheShaderType = value;
    }

    void SetShaderType(const int32_t value)
    {
      if (value < 0 || value >= static_cast<int32_t>(ShaderType::COUNT))
        throw std::invalid_argument("invalid shader type");
      TheShaderType = static_cast<ShaderType::Enum>(value);
    }

    void SetCompareEnabled(const bool value)
    {
      CompareEnabled = value;
    }

    void SetCRMFlags(const uint32_t value)
    {
      CRMFlags= value;
    }

    void SetCaptionType(const int32_t value)
    {
      if (value < 0 || value >= static_cast<int32_t>(CaptionType::COUNT))
        throw std::invalid_argument("invalid caption type");

      TheCaptionType = static_cast<CaptionType::Enum>(value);
    }

    void SetCaptionType(const CaptionType::Enum value)
    {
      TheCaptionType = value;
    }

    void SetUseOptimalSigma(const bool value)
    {
      UseOptimalSigma = value;
    }

  };
}

#endif
