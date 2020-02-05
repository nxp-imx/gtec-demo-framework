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

#include <Shared/FractalShader/ShaderUtil.hpp>
#include <FslBase/String/StringUtil.hpp>
#include <FslBase/String/ToString.hpp>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace Fsl
{
  namespace
  {
    void ShaderGeneratorGray(const int32_t iterations, std::string& rShader, const std::string& fragmentName)
    {
      std::stringstream stream;
      for (int i = 0; i < iterations; ++i)
      {
        float c = (i + 0.0f) / iterations;

        stream << "  // " << (i + 1) << "\n";
        stream << "  v = c + vec2(v.x * v.x - v.y * v.y, v.x * v.y * 2.0);\n";
        stream << "  if (dot(v, v) > 4.0)\n";
        stream << "  {\n";
        stream << "    " << fragmentName << " = vec4(" << std::max(c, 0.001f) << ", " << std::max(c, 0.001f) << ", " << std::max(c, 0.001f)
               << ", 1.0);\n";
        stream << "    return;\n";
        stream << "  }\n\n";
      }
      stream << "  " << fragmentName << " = vec4(1.0, 1.0, 1.0, 1.0);\n";
      StringUtil::Replace(rShader, "##REPLACE##", stream.str());
    }


    void ShaderGeneratorCol(const int32_t iterations, std::string& rShader, const std::string& fragmentName)
    {
      std::stringstream stream;
      for (int i = 0; i < iterations; ++i)
      {
        float r = (0.5f + 0.5f * std::cos(3.0f + (i + 1) * 0.15f + 0.0f));
        float g = (0.5f + 0.5f * std::cos(3.0f + (i + 1) * 0.15f + 0.6f));
        float b = (0.5f + 0.5f * std::cos(3.0f + (i + 1) * 0.15f + 1.0f));

        stream << "  // " << (i + 1) << "\n";
        stream << "  v = c + vec2(v.x * v.x - v.y * v.y, v.x * v.y * 2.0);\n";
        stream << "  if (dot(v, v) > 4.0)\n";
        stream << "  {\n";
        stream << "    " << fragmentName << " = vec4(" << std::max(r, 0.001f) << ", " << std::max(g, 0.001f) << ", " << std::max(b, 0.001f)
               << ", 1.0);\n";
        stream << "    return;\n";
        stream << "  }\n\n";
      }

      stream << "  " << fragmentName << " = vec4(0.0, 0.0, 0.0, 1.0);\n";
      StringUtil::Replace(rShader, "##REPLACE##", stream.str());
    }


    void ShaderGeneratorSmooth(const int32_t iterations, std::string& rShader, const std::string& fragmentName)
    {
      std::stringstream stream;
      for (int i = 0; i < iterations; ++i)
      {
        stream << "  // " << (i + 1) << "\n";
        stream << "  v = c + vec2(v.x * v.x - v.y * v.y, v.x * v.y * 2.0);\n";
        stream << "  if (dot(v, v) > 4.0)\n";
        stream << "  {\n";
        stream << "    " << fragmentName << " = (0.5 * cos(vec4(3, 4, 11, 0) + 0.05*(" << (i + 1) << ".0 - log2(log2(dot(v, v)))))) + 0.5;\n";
        stream << "    return;\n";
        stream << "  }\n\n";
      }

      stream << "  " << fragmentName << " = vec4(0.0, 0.0, 0.0, 1.0);\n";
      StringUtil::Replace(rShader, "##REPLACE##", stream.str());
    }
  }


  std::string ShaderUtil::GetFragmentShader(const BasicConfig& config, const std::shared_ptr<IContentManager>& contentManager,
                                            const int32_t openGLESVersion)
  {
    std::string fragmentShader;
    if (!config.ForceUnroll)
    {
      std::string fragmentShaderFile;
      switch (config.TheRenderMode)
      {
        // case RenderMode::Tex:
        //  fragmentShaderFile = "Julia_tex.frag";
        //  break;
      case RenderMode::Smooth:
        fragmentShaderFile = "Julia_smooth.frag";
        break;
      case RenderMode::Col:
        fragmentShaderFile = "Julia_col.frag";
        break;
      case RenderMode::Gray:
      default:
        fragmentShaderFile = "Julia_gray.frag";
        break;
      }

      // Dynamically patch the fragment shader with the desired iteration count
      fragmentShader = contentManager->ReadAllText(fragmentShaderFile);
      StringUtil::Replace(fragmentShader, "##MAX_ITERATIONS##", ToString(config.IterationsJ));
    }
    else
    {
      fragmentShader = contentManager->ReadAllText("Julia_unroll.frag");

      std::string fragmentName;
      switch (openGLESVersion)
      {
      case 3:
        fragmentName = "o_fragColor";
        break;
      default:
        fragmentName = "gl_FragColor";
        break;
      }

      switch (config.TheRenderMode)
      {
        // case RenderMode::Tex:
        //  fragmentShaderFile = "Julia_tex.frag";
        //  break;
      case RenderMode::Smooth:
        ShaderGeneratorSmooth(config.IterationsJ, fragmentShader, fragmentName);
        break;
      case RenderMode::Col:
        ShaderGeneratorCol(config.IterationsJ, fragmentShader, fragmentName);
        break;
      case RenderMode::Gray:
      default:
        ShaderGeneratorGray(config.IterationsJ, fragmentShader, fragmentName);
        break;
      }
    }

    // std::cout << fragmentShader << "\n";
    return fragmentShader;
  }
}
