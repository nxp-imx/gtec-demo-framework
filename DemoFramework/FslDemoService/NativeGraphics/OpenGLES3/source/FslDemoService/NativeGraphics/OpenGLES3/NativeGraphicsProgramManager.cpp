/****************************************************************************************************************************************************
 * Copyright 2021-2022, 2024 NXP
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

#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES3/NativeGraphicsProgramManager.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES3/NativeGraphicsShaderManager.hpp>

namespace Fsl::GLES3
{
  namespace
  {
    ReadOnlySpan<GLBindAttribLocation> ConvertBindAttribs(std::vector<GLBindAttribLocation>& rScratchpad, VertexAttributeDescriptionSpan srcSpan)
    {
      if (rScratchpad.size() < srcSpan.size())
      {
        rScratchpad.resize(srcSpan.size() + 32);
      }
      for (std::size_t i = 0; i < srcSpan.size(); ++i)
      {
        assert(!srcSpan[i].Name.empty());
        rScratchpad[i] = GLBindAttribLocation(srcSpan[i].Location, srcSpan[i].Name);
      }
      return SpanUtil::AsReadOnlySpan(rScratchpad, 0, srcSpan.size());
    }

    GLProgram CreateProgram(std::vector<GLBindAttribLocation>& rScratchpad, const NativeGraphicsShaderManager::ShaderRecord& vertShader,
                            const NativeGraphicsShaderManager::ShaderRecord& fragShader)
    {
      ReadOnlySpan<GLBindAttribLocation> bindAttribLocationSpan =
        ConvertBindAttribs(rScratchpad, vertShader.ShaderVertexAttributeDescriptions.AsSpan());

      GLProgram program(vertShader.Shader, fragShader.Shader, bindAttribLocationSpan);

      // Clear the scratchpad
      for (std::size_t i = 0; i < bindAttribLocationSpan.size(); ++i)
      {
        rScratchpad[i] = {};
      }
      return program;
    }
  }

  NativeGraphicsProgramManager::NativeGraphicsProgramManager()
    : m_records(32)
    , m_bindAttribScratchpad(32)
  {
  }

  NativeProgramHandle NativeGraphicsProgramManager::Acquire(NativeGraphicsShaderManager& rShaderManager, const BasicNativeShaderHandle hVertexShader,
                                                            const BasicNativeShaderHandle hFragmentShader)
  {
    const NativeShaderComboKey comboKey(hVertexShader, hFragmentShader);
    const auto itrFind = m_programDict.find(comboKey);
    if (itrFind != m_programDict.end())
    {
      Record& rRecord = m_records.Get(itrFind->second.Value);
      ++rRecord.ReferenceCount;
      return itrFind->second;
    }

    const NativeGraphicsShaderManager::ShaderRecord& vertShader = rShaderManager.Get(hVertexShader);
    const NativeGraphicsShaderManager::ShaderRecord& fragShader = rShaderManager.Get(hFragmentShader);

    GLProgram program(CreateProgram(m_bindAttribScratchpad, vertShader, fragShader));

    const auto vertexAttributeDescSpan = vertShader.ShaderVertexAttributeDescriptions.AsSpan();

    int32_t handleValue = HandleVectorConfig::InvalidHandle;
    if (vertexAttributeDescSpan.size() == 3u)
    {
      // Uniform locations
      const GLint locMatModelViewProj = program.GetUniformLocation("MatModelViewProj");
      const GLint locTexture = program.GetUniformLocation("Texture");
      const GLint locSmoothing = program.TryGetUniformLocation("Smoothing");

      const auto& vertexPosition = vertexAttributeDescSpan.VertexAttributeGet(VertexElementUsage::Position, 0);
      const auto& vertexColor = vertexAttributeDescSpan.VertexAttributeGet(VertexElementUsage::Color, 0);
      const auto& vertexTextureCoord = vertexAttributeDescSpan.VertexAttributeGet(VertexElementUsage::TextureCoordinate, 0);

      // Verify that the attributes are at the requested locations
      assert(program.GetAttribLocation(vertexPosition.Name.c_str()) == UncheckedNumericCast<GLint>(vertexPosition.Location));
      assert(program.GetAttribLocation(vertexColor.Name.c_str()) == UncheckedNumericCast<GLint>(vertexColor.Location));
      assert(program.GetAttribLocation(vertexTextureCoord.Name.c_str()) == UncheckedNumericCast<GLint>(vertexTextureCoord.Location));

      handleValue =
        m_records.Add(Record(std::move(program), locMatModelViewProj, locTexture, locSmoothing, UncheckedNumericCast<GLint>(vertexPosition.Location),
                             UncheckedNumericCast<GLint>(vertexColor.Location), UncheckedNumericCast<GLint>(vertexTextureCoord.Location), comboKey));
    }
    else if (vertexAttributeDescSpan.size() == 2u)
    {
      // Uniform locations
      const GLint locMatModelViewProj = program.GetUniformLocation("MatModelViewProj");
      const GLint locSmoothing = program.TryGetUniformLocation("Smoothing");

      const auto& vertexPosition = vertexAttributeDescSpan.VertexAttributeGet(VertexElementUsage::Position, 0);
      const auto& vertexColor = vertexAttributeDescSpan.VertexAttributeGet(VertexElementUsage::Color, 0);

      // Verify that the attributes are at the requested locations
      assert(program.GetAttribLocation(vertexPosition.Name.c_str()) == UncheckedNumericCast<GLint>(vertexPosition.Location));
      assert(program.GetAttribLocation(vertexColor.Name.c_str()) == UncheckedNumericCast<GLint>(vertexColor.Location));

      handleValue = m_records.Add(Record(std::move(program), locMatModelViewProj, GLValues::InvalidLocation, locSmoothing,
                                         UncheckedNumericCast<GLint>(vertexPosition.Location), UncheckedNumericCast<GLint>(vertexColor.Location),
                                         GLValues::InvalidLocation, comboKey));
    }
    else
    {
      throw NotSupportedException("shader did not the three expected attributes");
    }

    try
    {
      m_programDict.emplace(comboKey, handleValue);
      return NativeProgramHandle(handleValue);
    }
    catch (const std::exception&)
    {
      m_records.Remove(handleValue);
      throw;
    }
  }


  bool NativeGraphicsProgramManager::Release(const NativeProgramHandle handle) noexcept
  {
    Record* pRecord = m_records.TryGet(handle.Value);
    const bool released = pRecord != nullptr;
    if (released)
    {
      assert(pRecord->ReferenceCount > 0u);
      if (pRecord->ReferenceCount > 1)
      {
        --pRecord->ReferenceCount;
      }
      else
      {
        const NativeShaderComboKey comboKey = pRecord->ComboKey;
        pRecord->ReferenceCount = 0;
        m_records.Remove(handle.Value);
        m_programDict.erase(comboKey);
      }
    }
    return released;
  }
}
