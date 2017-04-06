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

#include "TwoPassShaders.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslDemoApp/Service/Content/IContentManager.hpp>
#include "GausianHelper.hpp"
#include <cassert>

namespace Fsl
{
  namespace
  {
    void GenerateHardcoded(GLES2::GLProgram& rProgramBlurX, GLES2::GLProgram& rProgramBlurY,
      const std::shared_ptr<IContentManager>& contentManager, const std::vector<double>& kernelSlice,
      const Point2& sizeTextureBlurX, const Point2& sizeTextureBlurY,
      const TwoPassShaders::Enum mode, const std::string& basicShader)
    {
      const std::string fragTemplate = contentManager->ReadAllText("GaussianTemplate.frag");

      std::string gaussianFragX(fragTemplate);
      std::string gaussianFragY(fragTemplate);
      Point2 texSizes(sizeTextureBlurX.X, sizeTextureBlurY.Y);
      switch (mode)
      {
      case TwoPassShaders::Linear:
        GausianHelper::GenerateGausianFragmentShaderLinear(gaussianFragX, gaussianFragY, kernelSlice, texSizes);
        break;
      case TwoPassShaders::Normal:
      default:
        GausianHelper::GenerateGausianFragmentShader(gaussianFragX, gaussianFragY, kernelSlice, texSizes);
        break;
      }

      //FSLLOG(gaussianFragX);

      rProgramBlurX.Reset(basicShader, gaussianFragX);
      rProgramBlurY.Reset(basicShader, gaussianFragY);
    }


    void GenerateNonDependent(GLES2::GLProgram& rProgramBlurX, GLES2::GLProgram& rProgramBlurY,
                           const std::shared_ptr<IContentManager>& contentManager, const std::vector<double>& kernelSlice,
                           const Point2& sizeTextureBlurX, const Point2& sizeTextureBlurY,
                           const TwoPassShaders::Enum mode)
    {
      const std::string vertTemplate = contentManager->ReadAllText("GaussianTemplateNoDependent.vert");
      const std::string fragTemplate = contentManager->ReadAllText("GaussianTemplateNoDependent.frag");

      std::string gaussianVertX(vertTemplate);
      std::string gaussianVertY(vertTemplate);
      std::string gaussianFrag(fragTemplate);
      Point2 texSizes(sizeTextureBlurX.X, sizeTextureBlurY.Y);
      switch (mode)
      {
      case TwoPassShaders::Linear:
        GausianHelper::GenerateNonDependentShadersLinear(gaussianVertX, gaussianVertY, gaussianFrag, kernelSlice, texSizes);
        break;
      case TwoPassShaders::Normal:
      default:
        GausianHelper::GenerateNonDependentShaders(gaussianVertX, gaussianVertY, gaussianFrag, kernelSlice, texSizes);
        break;
      }

      rProgramBlurX.Reset(gaussianVertX, gaussianFrag);
      rProgramBlurY.Reset(gaussianVertY, gaussianFrag);

      //FSLLOG(gaussianVertX);
    }

  }


  TwoPassShaders::TwoPassShaders()
    : ProgramNormal()
    , ProgramBlurX()
    , ProgramBlurY()
  {

  }


  void TwoPassShaders::Reset(const std::shared_ptr<IContentManager>& contentManager, const int32_t kernelLength, const float sigma,
                             const Point2& sizeTextureBlurX, const Point2& sizeTextureBlurY,
                             const TwoPassShaders::Enum mode, const ShaderType::Enum shaderType)
  {
    float moddedSigma = sigma;
    if (moddedSigma < 0)
    {
      moddedSigma = GausianHelper::FindOptimalSigma(kernelLength);
      FSLLOG("Using sigma of " << moddedSigma);
    }

    std::vector<double> kernelSlice;
    GausianHelper::CalculateGausianKernelSlice(kernelSlice, kernelLength, moddedSigma);
    Reset(contentManager, kernelSlice, sizeTextureBlurX, sizeTextureBlurY, mode, shaderType);
  }


  void TwoPassShaders::Reset(const std::shared_ptr<IContentManager>& contentManager, const std::vector<double>& kernelSlice,
                             const Point2& sizeTextureBlurX, const Point2& sizeTextureBlurY,
                             const TwoPassShaders::Enum mode, const ShaderType::Enum shaderType)
  {
    // Load the basic shader
    const std::string basicShader = contentManager->ReadAllText("BasicShader.vert");
    ProgramNormal.Reset(basicShader, contentManager->ReadAllText("BasicShader.frag"));


    switch (shaderType)
    {
    case ShaderType::NonDependent:
      GenerateNonDependent(ProgramBlurX, ProgramBlurY, contentManager, kernelSlice, sizeTextureBlurX, sizeTextureBlurY, mode);
      break;
    case ShaderType::HardCoded:
      GenerateHardcoded(ProgramBlurX, ProgramBlurY, contentManager, kernelSlice, sizeTextureBlurX, sizeTextureBlurY, mode, basicShader);
    default:
      break;
    }

    //ProgramBlurX.Reset(basicShader, contentManager->ReadAllText("BasicShader.frag"));
    //ProgramBlurY.Reset(basicShader, contentManager->ReadAllText("BasicShader.frag"));
  }


}
