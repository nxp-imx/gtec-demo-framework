#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_NATIVEGRAPHICSSHADERMANAGER_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_NATIVEGRAPHICSSHADERMANAGER_HPP
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

#include <FslBase/Collections/HandleVector.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeShaderCreateInfo.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeShaderHandle.hpp>
#include <FslGraphics/Render/Basic/BasicShaderStageFlag.hpp>
#include <FslGraphics/Vertices/VertexAttributeDescriptions.hpp>
#include <FslUtil/OpenGLES2/GLShader.hpp>
#include <utility>

namespace Fsl::GLES2
{
  class NativeGraphicsShaderManager
  {
  public:
    struct ShaderRecord
    {
      BasicShaderStageFlag Flag{BasicShaderStageFlag::Undefined};
      GLShader Shader;
      VertexAttributeDescriptions ShaderVertexAttributeDescriptions;

      ShaderRecord() = default;
      ShaderRecord(const BasicShaderStageFlag flag, GLShader shader, VertexAttributeDescriptions shaderVertexAttributeDescriptions)
        : Flag(flag)
        , Shader(std::move(shader))
        , ShaderVertexAttributeDescriptions(std::move(shaderVertexAttributeDescriptions))
      {
      }
    };

  private:
    HandleVector<ShaderRecord> m_records;

  public:
    NativeGraphicsShaderManager();

    BasicNativeShaderHandle Create(const BasicNativeShaderCreateInfo& createInfo);

    bool Destroy(const BasicNativeShaderHandle handle);

    const ShaderRecord& Get(const BasicNativeShaderHandle hShader) const
    {
      return m_records.Get(hShader.Value);
    }

  private:
  };
}

#endif
