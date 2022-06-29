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

#include <FslDemoService/NativeGraphics/OpenGLES3/NativeGraphicsShaderManager.hpp>
#include <fmt/format.h>


namespace Fsl::GLES3
{
  namespace
  {
    GLint ToShaderType(const BasicShaderStageFlag flag)
    {
      switch (flag)
      {
      case BasicShaderStageFlag::Undefined:
        throw NotSupportedException("Shader type can not be 'Undefined'");
      case BasicShaderStageFlag::Fragment:
        return GL_FRAGMENT_SHADER;
      case BasicShaderStageFlag::Vertex:
        return GL_VERTEX_SHADER;
      }
      // Done like this instead of a 'default' to allow the compiler to give a warning during compile
      throw NotSupportedException(fmt::format("shader type: {}", static_cast<uint32_t>(flag)));
    }

    const char* AsZeroTerminatedString(const ReadOnlySpan<uint8_t> content)
    {
      if (content.empty())
      {
        throw std::invalid_argument("Not a valid shader as it was empty");
      }

      if (content.back() != 0u)
      {
        throw std::invalid_argument("Not a valid shader as it was not zero terminated");
      }

      const std::size_t length = content.size() - 1;
      for (std::size_t i = 0; i < length; ++i)
      {
        if (content[i] == 0u)
        {
          throw std::invalid_argument("Not a valid shader as it contained zero terminations mid string");
        }
      }
      return reinterpret_cast<const char* const>(content.data());
    }
  }

  NativeGraphicsShaderManager::NativeGraphicsShaderManager()
    : m_records(16)
  {
  }

  BasicNativeShaderHandle NativeGraphicsShaderManager::Create(const BasicNativeShaderCreateInfo& createInfo)
  {
    if (!createInfo.IsValid())
    {
      throw std::invalid_argument("createInfo must be valid");
    }

    const GLint shaderType = ToShaderType(createInfo.Flag);
    const char* const pszShader = AsZeroTerminatedString(createInfo.Shader);

    const int32_t handleValue =
      m_records.Add(ShaderRecord(createInfo.Flag, GLShader(shaderType, pszShader), VertexAttributeDescriptions(createInfo.VertexAttributeDescSpan)));

    return BasicNativeShaderHandle(handleValue);
  }


  bool NativeGraphicsShaderManager::Destroy(const BasicNativeShaderHandle handle)
  {
    return m_records.Remove(handle.Value);
  }
}
