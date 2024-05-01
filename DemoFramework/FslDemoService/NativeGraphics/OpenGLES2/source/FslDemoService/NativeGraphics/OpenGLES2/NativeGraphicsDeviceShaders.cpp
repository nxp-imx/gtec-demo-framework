/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

#include <FslBase/Log/Log3Core.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/NativeGraphicsDeviceShaders.hpp>
#include <cstring>

namespace Fsl::GLES2
{


  namespace
  {
    constexpr const char* const g_vertexShader =
      "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
      "precision highp float;\n"
      "#else\n"
      "precision mediump float;\n"
      "#endif\n"
      "uniform mat4 MatModelViewProj;\n"
      "\n"
      "attribute vec4 inVertexPosition;\n"
      "attribute vec4 inVertexColor;\n"
      "attribute vec2 inVertexTextureCoord;\n"
      "\n"
      "varying vec4 FragColor;\n"
      "varying vec2 FragTextureCoord;\n"
      "\n"
      "void main()\n"
      "{"
      "  gl_Position = MatModelViewProj * inVertexPosition;\n"
      "  FragColor = inVertexColor;\n"
      "  FragTextureCoord = inVertexTextureCoord;\n"
      "}";


    constexpr const char* const g_fragmentShader =
      "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
      "precision highp float;\n"
      "#else\n"
      "precision mediump float;\n"
      "#endif\n"
      "\n"
      "uniform sampler2D Texture;\n"
      "\n"
      "varying vec4 FragColor;\n"
      "varying vec2 FragTextureCoord;\n"
      "\n"
      "void main()\n"
      "{\n"
      "  gl_FragColor = texture2D(Texture, FragTextureCoord) * FragColor;\n"
      "}\n";


    constexpr const char* const g_fragmentSdfShader =
      "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
      "precision highp float;\n"
      "#else\n"
      "precision mediump float;\n"
      "#endif\n"
      "\n"
      "uniform sampler2D Texture;\n"
      "uniform float Smoothing;\n"
      "\n"
      "varying vec4 FragColor;\n"
      "varying vec2 FragTextureCoord;\n"
      "\n"
      "void main()\n"
      "{\n"
      "  float distance = texture2D(Texture, FragTextureCoord).a;\n"
      "  float alpha = smoothstep(0.5 - Smoothing, 0.5 + Smoothing, distance);\n"
      "  gl_FragColor = vec4(FragColor.rgb, FragColor.a * alpha);\n"
      "}\n";
  }

  ReadOnlySpan<uint8_t> NativeGraphicsDeviceShaders::GetVertexShader()
  {
    // +1 to include the zero termination.
    return ReadOnlySpan<uint8_t>(reinterpret_cast<const uint8_t*>(g_vertexShader), strlen(g_vertexShader) + 1);
  }

  ReadOnlySpan<uint8_t> NativeGraphicsDeviceShaders::GetFragmentShader()
  {
    // +1 to include the zero termination
    return ReadOnlySpan<uint8_t>(reinterpret_cast<const uint8_t*>(g_fragmentShader), strlen(g_fragmentShader) + 1);
  }

  ReadOnlySpan<uint8_t> NativeGraphicsDeviceShaders::GetSdfFragmentShader()
  {
    // +1 to include the zero termination
    return ReadOnlySpan<uint8_t>(reinterpret_cast<const uint8_t*>(g_fragmentSdfShader), strlen(g_fragmentSdfShader) + 1);
  }
}
