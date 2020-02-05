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

#include "GausianHelper.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/String/StringUtil.hpp>
#include <iomanip>
#include <sstream>
#include <cassert>

namespace Fsl
{
  namespace
  {
    const int32_t MAX_KERNEL_SLICE_LENGTH = 128;

    void Normalize(std::vector<double>& rKernel, const int32_t entries, const double sum)
    {
      // Normalize the entries
      const double mod = (1.0 / sum);
      for (int i = 0; i < entries; ++i)
      {
        rKernel[i] = rKernel[i] * mod;
      }
    }

    struct LinearData
    {
      double Weight{0};
      double OffsetX{0};
      double OffsetY{0};

      LinearData() = default;

      LinearData(const double weight, const double offsetX, const double offsetY)
        : Weight(weight)
        , OffsetX(offsetX)
        , OffsetY(offsetY)
      {
      }
    };

    void CalcLinearWeightsAndOffset(LinearData* pDst, const int dstLength, const std::vector<double>& rKernel, const Vector2& texStep)
    {
      const auto halfLength = static_cast<int32_t>(rKernel.size() - 1);
      assert((halfLength & 1) == 0);
      assert(dstLength >= halfLength);

      pDst[0] = LinearData(rKernel.front(), 0, 0);

      int dstIndex = 1;
      for (int i = 1; i <= halfLength; i += 2)
      {
        const double linearWeight = rKernel[i] + rKernel[i + 1];
        const double newOffset = (((i * rKernel[i]) + ((i + 1) * rKernel[i + 1])) / linearWeight);
        assert(dstIndex < dstLength);
        pDst[dstIndex] = LinearData(linearWeight, newOffset * texStep.X, newOffset * texStep.Y);
        ++dstIndex;
      }
    }


    void GenerateNonDependentFragmentShader(std::string& rGaussianFrag, const int32_t halfLength, const LinearData* const pLinearData)
    {
      {
        std::stringstream str;
        str << std::setprecision(13) << std::fixed;

        str << "  vec4 color;\n";
        int index = 0;
        for (int i = halfLength; i > 0; --i)
        {
          str << "  color += texture2D(s_texture, v_texcoord[" << index << "]) * " << static_cast<float>(pLinearData[i].Weight) << ";\n";
          ++index;
        }
        for (int i = 0; i <= halfLength; ++i)
        {
          str << "  color += texture2D(s_texture, v_texcoord[" << index << "]) * " << static_cast<float>(pLinearData[i].Weight) << ";\n";
          ++index;
        }

        str << "  gl_FragColor = color;\n";

        // insert it into the shader template
        StringUtil::Replace(rGaussianFrag, "##REPLACE##", str.str());
      }
    }
  }

  float GausianHelper::FindOptimalSigma(const int32_t length)
  {
    std::vector<double> kernel(length);
    float sigma = 1.0f;
    do
    {
      sigma += 0.1f;
      CalculateGausianKernelSlice(kernel, length, sigma);
    } while (kernel.back() <= 0.002);
    return sigma;
  }


  void GausianHelper::CalculateGausianKernel(std::vector<double>& rKernel, const int32_t length, const double sigma)
  {
    assert((length & 1) == 1);
    rKernel.resize(length * length);

    const double calculatedEuler = 1.0 / (2.0 * MathHelper::PI * sigma * sigma);

    double mean = length / 2;    // NOLINT(bugprone-integer-division)
    double sum = 0.0;
    for (int y = 0; y < length; ++y)
    {
      for (int x = 0; x < length; ++x)
      {
        const double value = std::exp(-0.5 * (std::pow((x - mean) / sigma, 2) + std::pow((y - mean) / sigma, 2))) * calculatedEuler;
        rKernel[x + (y * length)] = value;
        // Accumulate the kernel values
        sum += value;
      }
    }
    Normalize(rKernel, length * length, sum);
  }


  void GausianHelper::CalculateGausianKernelSlice(std::vector<double>& rKernel, const int32_t length, const double sigma)
  {
    assert((length & 1) == 1);
    const int kernelRadius = (length / 2) + 1;
    rKernel.resize(kernelRadius);

    const double calculatedEuler = 1.0 / (2.0 * MathHelper::PI * sigma * sigma);

    double sum = 0.0;
    for (int x = 0; x < kernelRadius; ++x)
    {
      const double value = std::exp(-0.5 * (std::pow(x / sigma, 2))) * calculatedEuler;
      rKernel[x] = value;
      // Accumulate the kernel values
      sum += (x == 0 ? value : value * 2);
    }

    Normalize(rKernel, kernelRadius, sum);
  }

  //! This shader is basically the worst case scenario for blurring. It's just a simple and slow reference implementation
  std::string GausianHelper::GenerateGausianFragmentShader(const std::string& shaderTemplate, const std::vector<double>& kernel, const int32_t length,
                                                           const Point2& texSize)
  {
    Vector2 texStep(1.0f / texSize.X, 1.0f / texSize.Y);

    int halfLength = length / 2;

    std::stringstream str;
    str << std::setprecision(13) << std::fixed;

    str << "  vec4 color;\n";
    int index = 0;
    for (int y = 0; y < length; ++y)
    {
      for (int x = 0; x < length; ++x)
      {
        str << "  color += texture2D(s_texture,v_texcoord + vec2(" << (texStep.X * (x - halfLength)) << ", " << (texStep.Y * (y - halfLength))
            << ")) * " << static_cast<float>(kernel[index]) << ";\n";
        ++index;
      }
    }

    str << "  gl_FragColor = color;\n";


    // insert it into the shader template
    std::string tmp(shaderTemplate);
    StringUtil::Replace(tmp, "##REPLACE##", str.str());
    return tmp;
  }


  void GausianHelper::GenerateGausianFragmentShader(std::string& rGaussianFragX, std::string& rGaussianFragY, const std::vector<double>& kernelSlice,
                                                    const Point2& texSize)
  {
    Vector2 texStep(1.0f / texSize.X, 1.0f / texSize.Y);

    const auto halfLength = static_cast<int32_t>(kernelSlice.size() - 1);

    std::stringstream strX, strY;
    strX << std::setprecision(13) << std::fixed;
    strY << std::setprecision(13) << std::fixed;

    strX << "  vec4 color;\n";
    strY << "  vec4 color;\n";
    for (int i = halfLength; i > 0; --i)
    {
      strX << "  color += texture2D(s_texture,v_texcoord + vec2(" << (texStep.X * -i) << ", 0.0)) * " << static_cast<float>(kernelSlice[i]) << ";\n";
      strY << "  color += texture2D(s_texture,v_texcoord + vec2(0.0, " << (texStep.Y * -i) << ")) * " << static_cast<float>(kernelSlice[i]) << ";\n";
    }
    for (int i = 0; i <= halfLength; ++i)
    {
      strX << "  color += texture2D(s_texture,v_texcoord + vec2(" << (texStep.X * i) << ", 0.0)) * " << static_cast<float>(kernelSlice[i]) << ";\n";
      strY << "  color += texture2D(s_texture,v_texcoord + vec2(0.0, " << (texStep.Y * i) << ")) * " << static_cast<float>(kernelSlice[i]) << ";\n";
    }

    strX << "  gl_FragColor = color;\n";
    strY << "  gl_FragColor = color;\n";


    // insert it into the shader template
    StringUtil::Replace(rGaussianFragX, "##REPLACE##", strX.str());
    StringUtil::Replace(rGaussianFragY, "##REPLACE##", strY.str());
  }


  void GausianHelper::GenerateGausianFragmentShaderLinear(std::string& rGaussianFragX, std::string& rGaussianFragY,
                                                          const std::vector<double>& kernelSlice, const Point2& texSize)
  {
    Vector2 texStep(1.0f / texSize.X, 1.0f / texSize.Y);
    if ((kernelSlice.size() & 1) == 0)
    {
      throw std::invalid_argument("Kernel radius must be odd not even");
    }

    const int halfLength = static_cast<int32_t>(kernelSlice.size()) / 2;
    if (halfLength > MAX_KERNEL_SLICE_LENGTH)
    {
      throw std::invalid_argument("Kernel size is too large");
    }

    LinearData linearData[MAX_KERNEL_SLICE_LENGTH];
    CalcLinearWeightsAndOffset(linearData, MAX_KERNEL_SLICE_LENGTH, kernelSlice, texStep);

    std::stringstream strX, strY;
    strX << std::setprecision(13) << std::fixed;
    strY << std::setprecision(13) << std::fixed;

    strX << "  vec4 color;\n";
    strY << "  vec4 color;\n";
    for (int i = halfLength; i > 0; --i)
    {
      strX << "  color += texture2D(s_texture,v_texcoord + vec2(" << static_cast<float>(-linearData[i].OffsetX) << ", 0.0)) * "
           << static_cast<float>(linearData[i].Weight) << ";\n";
      strY << "  color += texture2D(s_texture,v_texcoord + vec2(0.0, " << static_cast<float>(-linearData[i].OffsetY) << ")) * "
           << static_cast<float>(linearData[i].Weight) << ";\n";
    }
    for (int i = 0; i <= halfLength; ++i)
    {
      strX << "  color += texture2D(s_texture,v_texcoord + vec2(" << static_cast<float>(linearData[i].OffsetX) << ", 0.0)) * "
           << static_cast<float>(linearData[i].Weight) << ";\n";
      strY << "  color += texture2D(s_texture,v_texcoord + vec2(0.0, " << static_cast<float>(linearData[i].OffsetY) << ")) * "
           << static_cast<float>(linearData[i].Weight) << ";\n";
    }

    strX << "  gl_FragColor = color;\n";
    strY << "  gl_FragColor = color;\n";

    // insert it into the shader template
    StringUtil::Replace(rGaussianFragX, "##REPLACE##", strX.str());
    StringUtil::Replace(rGaussianFragY, "##REPLACE##", strY.str());
  }


  void GausianHelper::GenerateNonDependentShaders(std::string& rGaussianVertX, std::string& rGaussianVertY, std::string& rGaussianFrag,
                                                  const std::vector<double>& kernelSlice, const Point2& texSize)
  {
    Vector2 texStep(1.0f / texSize.X, 1.0f / texSize.Y);

    const auto halfLength = static_cast<int32_t>(kernelSlice.size() - 1);

    {
      std::stringstream str2;
      str2 << ((halfLength * 2) + 1);
      StringUtil::Replace(rGaussianVertX, "##BLUR_COUNT##", str2.str());
      StringUtil::Replace(rGaussianVertY, "##BLUR_COUNT##", str2.str());
      StringUtil::Replace(rGaussianFrag, "##BLUR_COUNT##", str2.str());
    }

    // Generate the fragment shader
    {
      std::stringstream strX, strY;
      strX << std::setprecision(13) << std::fixed;
      strY << std::setprecision(13) << std::fixed;

      int index = 0;
      for (int i = halfLength; i > 0; --i)
      {
        strX << "  v_texcoord[" << index << "] = TexCoord.xy + vec2(" << (-i * texStep.X) << ", 0.0);\n";
        strY << "  v_texcoord[" << index << "] = TexCoord.xy + vec2(0.0, " << (-i * texStep.Y) << ");\n";
        ++index;
      }
      strX << "  v_texcoord[" << index << "] = TexCoord.xy;\n";
      strY << "  v_texcoord[" << index << "] = TexCoord.xy;\n";
      ++index;
      for (int i = 1; i <= halfLength; ++i)
      {
        strX << "  v_texcoord[" << index << "] = TexCoord.xy + vec2(" << (i * texStep.X) << ", 0.0);\n";
        strY << "  v_texcoord[" << index << "] = TexCoord.xy + vec2(0.0, " << (i * texStep.Y) << ");\n";
        ++index;
      }

      // insert it into the shader template
      StringUtil::Replace(rGaussianVertX, "##REPLACE##", strX.str());
      StringUtil::Replace(rGaussianVertY, "##REPLACE##", strY.str());
    }


    // Generate the on dependent fragment shader
    {
      std::stringstream str;
      str << std::setprecision(13) << std::fixed;

      str << "  vec4 color;\n";
      int index = 0;
      for (int i = halfLength; i > 0; --i)
      {
        str << "  color += texture2D(s_texture, v_texcoord[" << index << "]) * " << static_cast<float>(kernelSlice[i]) << ";\n";
        ++index;
      }
      for (int i = 0; i <= halfLength; ++i)
      {
        str << "  color += texture2D(s_texture, v_texcoord[" << index << "]) * " << static_cast<float>(kernelSlice[i]) << ";\n";
        ++index;
      }
      str << "  gl_FragColor = color;\n";

      // insert it into the shader template
      StringUtil::Replace(rGaussianFrag, "##REPLACE##", str.str());
    }
  }


  void GausianHelper::GenerateNonDependentShadersLinear(std::string& rGaussianVertX, std::string& rGaussianVertY, std::string& rGaussianFrag,
                                                        const std::vector<double>& kernelSlice, const Point2& texSize)
  {
    if ((kernelSlice.size() & 1) == 0)
    {
      throw std::invalid_argument("Kernel radius must be odd not even");
    }

    Vector2 texStep(1.0f / texSize.X, 1.0f / texSize.Y);

    int halfLength = static_cast<int32_t>(kernelSlice.size()) / 2;
    if (halfLength > MAX_KERNEL_SLICE_LENGTH)
    {
      throw std::invalid_argument("Kernel size is too large");
    }

    LinearData linearData[MAX_KERNEL_SLICE_LENGTH];
    CalcLinearWeightsAndOffset(linearData, MAX_KERNEL_SLICE_LENGTH, kernelSlice, texStep);

    {
      std::stringstream str2;
      str2 << ((halfLength * 2) + 1);
      StringUtil::Replace(rGaussianVertX, "##BLUR_COUNT##", str2.str());
      StringUtil::Replace(rGaussianVertY, "##BLUR_COUNT##", str2.str());
      StringUtil::Replace(rGaussianFrag, "##BLUR_COUNT##", str2.str());
    }

    // Generate the vertex shader
    {
      std::stringstream strX, strY;
      strX << std::setprecision(13) << std::fixed;
      strY << std::setprecision(13) << std::fixed;

      int index = 0;
      for (int i = halfLength; i > 0; --i)
      {
        strX << "  v_texcoord[" << index << "] = TexCoord.xy + vec2(" << static_cast<float>(-linearData[i].OffsetX) << ", 0.0);\n";
        strY << "  v_texcoord[" << index << "] = TexCoord.xy + vec2(0.0, " << static_cast<float>(-linearData[i].OffsetY) << ");\n";
        ++index;
      }
      strX << "  v_texcoord[" << index << "] = TexCoord.xy;\n";
      strY << "  v_texcoord[" << index << "] = TexCoord.xy;\n";
      ++index;
      for (int i = 1; i <= halfLength; ++i)
      {
        strX << "  v_texcoord[" << index << "] = TexCoord.xy + vec2(" << static_cast<float>(linearData[i].OffsetX) << ", 0.0);\n";
        strY << "  v_texcoord[" << index << "] = TexCoord.xy + vec2(0.0, " << static_cast<float>(linearData[i].OffsetY) << ");\n";
        ++index;
      }

      // insert it into the shader template
      StringUtil::Replace(rGaussianVertX, "##REPLACE##", strX.str());
      StringUtil::Replace(rGaussianVertY, "##REPLACE##", strY.str());
    }

    // Generate the fragment shader
    GenerateNonDependentFragmentShader(rGaussianFrag, halfLength, linearData);
  }


  void GausianHelper::DebugDumpKernel2D(const std::vector<double>& kernel, const int32_t length)
  {
    std::stringstream str;
    str << std::setprecision(13) << std::fixed;

    for (int y = 0; y < length; ++y)
    {
      for (int x = 0; x < length; ++x)
      {
        str << kernel[x + y * length] << ",";
      }
      str << "\n";
    }
    FSLLOG3_INFO(str.str());
  }
}
