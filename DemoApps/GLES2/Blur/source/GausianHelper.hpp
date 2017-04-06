#ifndef GLES2_BLUR_GAUSIANHELPER_HPP
#define GLES2_BLUR_GAUSIANHELPER_HPP
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
#include <string>
#include <vector>

namespace Fsl
{
  class GausianHelper
  {
  public:
    //! @brief Try to find the optimal sigma for a kernel length. Warning this can be slow.
    static float FindOptimalSigma(const int32_t length);
    static void CalculateGausianKernel(std::vector<double>& rKernel, const int32_t length, const double sigma);
    static void CalculateGausianKernelSlice(std::vector<double>& rKernel, const int32_t length, const double sigma);
    static std::string GenerateGausianFragmentShader(const std::string& shaderTemplate, const std::vector<double>& kernel, const int32_t length, const Point2& texSize);
    static void GenerateGausianFragmentShader(std::string& rGaussianFragX, std::string& rGaussianFragY, const std::vector<double>& kernelSlice, const Point2& texSize);
    static void GenerateGausianFragmentShaderLinear(std::string& rGaussianFragX, std::string& rGaussianFragY, const std::vector<double>& kernelSlice, const Point2& texSize);

    static void GenerateNonDependentShaders(std::string& rGaussianVertX, std::string& rGaussianVertY, std::string& rGaussianFrag, const std::vector<double>& kernelSlice, const Point2& texSize);
    static void GenerateNonDependentShadersLinear(std::string& rGaussianVertX, std::string& rGaussianVertY, std::string& rGaussianFrag, const std::vector<double>& kernelSlice, const Point2& texSize);

    static void DebugDumpKernel2D(const std::vector<double>& kernel, const int32_t length);
  };
}

#endif
