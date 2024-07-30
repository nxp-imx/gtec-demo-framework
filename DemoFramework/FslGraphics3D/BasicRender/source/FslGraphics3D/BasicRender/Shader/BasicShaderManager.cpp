/****************************************************************************************************************************************************
 * Copyright 2022, 2024 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslBase/Span/TypedFlexSpan.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeShaderCreateInfo.hpp>
#include <FslGraphics3D/BasicRender/Shader/BasicShaderManager.hpp>
#include <algorithm>
#include <cassert>
#include <limits>
#include <utility>

namespace Fsl::Graphics3D
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr auto LogType = Fsl::LogType::Verbose5;
    }

    void ValidateBasicShaderCreateInfo(const BasicShaderCreateInfo& createInfo)
    {
      if (!createInfo.IsValid())
      {
        throw std::invalid_argument("CreateShader: createInfo is invalid");
      }

      if (createInfo.Flag == BasicShaderStageFlag::Vertex)
      {
        if (createInfo.VertexAttributeDescSpan.Count() < 2)
        {
          throw InternalErrorException("A predefined shader did not contain the expected amount of vertex attributes.");
        }
        if (createInfo.VertexAttributeDescSpan.Count() < 3)
        {
          const int32_t indexPos = createInfo.VertexAttributeDescSpan.VertexAttributeIndexOf(VertexElementUsage::Position, 0);
          const int32_t indexColor = createInfo.VertexAttributeDescSpan.VertexAttributeIndexOf(VertexElementUsage::Color, 0);
          if (indexPos < 0 || indexColor < 0)
          {
            throw InternalErrorException("A basic shader did not contain the expected position, color and texture coordinate entries.");
          }
          if (createInfo.VertexAttributeDescSpan[indexPos].Format != VertexElementFormat::Vector3)
          {
            throw InternalErrorException("A basic shader vertex position was not of the Vector3 format");
          }
          if (createInfo.VertexAttributeDescSpan[indexColor].Format != VertexElementFormat::Vector4)
          {
            throw InternalErrorException("A basic shader vertex color was not of the Vector4 format");
          }
        }
        else
        {
          const int32_t indexPos = createInfo.VertexAttributeDescSpan.VertexAttributeIndexOf(VertexElementUsage::Position, 0);
          const int32_t indexColor = createInfo.VertexAttributeDescSpan.VertexAttributeIndexOf(VertexElementUsage::Color, 0);
          const int32_t indexTexture = createInfo.VertexAttributeDescSpan.VertexAttributeIndexOf(VertexElementUsage::TextureCoordinate, 0);
          if (indexPos < 0 || indexColor < 0 || indexTexture < 0)
          {
            throw InternalErrorException("A basic shader did not contain the expected position, color and texture coordinate entries.");
          }
          if (createInfo.VertexAttributeDescSpan[indexPos].Format != VertexElementFormat::Vector3)
          {
            throw InternalErrorException("A basic shader vertex position was not of the Vector3 format");
          }
          if (createInfo.VertexAttributeDescSpan[indexColor].Format != VertexElementFormat::Vector4)
          {
            throw InternalErrorException("A basic shader vertex color was not of the Vector4 format");
          }
          if (createInfo.VertexAttributeDescSpan[indexTexture].Format != VertexElementFormat::Vector2)
          {
            throw InternalErrorException("A basic shader vertex texture-coordinate was not of the Vector2 format");
          }
        }
      }
      else if (createInfo.Flag == BasicShaderStageFlag::Fragment)
      {
        if (!createInfo.VertexAttributeDescSpan.Empty())
        {
          throw InternalErrorException("A basic shader did not contain the expected amount of vertex attributes.");
        }
      }
    }

    void ValidatePredefinedShaders(const ReadOnlySpan<BasicNativeShaderCreateInfo> predefinedShaders)
    {
      if (predefinedShaders.size() != 5u)
      {
        throw InternalErrorException("The supplied adapter did not return the expected predefined shader count.");
      }
      if (predefinedShaders[0].Flag != BasicShaderStageFlag::Vertex)
      {
        throw InternalErrorException("The supplied adapter did not supply a vertex shader at index 0.");
      }
      if (predefinedShaders[1].Flag != BasicShaderStageFlag::Fragment)
      {
        throw InternalErrorException("The supplied adapter did not supply a vertex shader at index 1.");
      }
      if (predefinedShaders[2].Flag != BasicShaderStageFlag::Fragment)
      {
        throw InternalErrorException("The supplied adapter did not supply a vertex shader at index 2.");
      }
      if (predefinedShaders[3].Flag != BasicShaderStageFlag::Vertex)
      {
        throw InternalErrorException("The supplied adapter did not supply a vertex shader at index 3.");
      }
      if (predefinedShaders[4].Flag != BasicShaderStageFlag::Fragment)
      {
        throw InternalErrorException("The supplied adapter did not supply a vertex shader at index 4.");
      }
    }
  }

  BasicShaderManager::BasicShaderManager(std::shared_ptr<INativeShaderFactory> factory)
    : m_factory(std::move(factory))
  {
    FSLLOG3(LocalConfig::LogType, "BasicShaderManager::BasicShaderManager()");
    if (!m_factory)
    {
      throw std::invalid_argument("factory can not be null");
    }

    try
    {
      ReadOnlySpan<BasicNativeShaderCreateInfo> predefinedShaders = m_factory->GetPredefinedShaders();
      ValidatePredefinedShaders(predefinedShaders);
      if (predefinedShaders.size() != m_predefinedShaders.size())
      {
        throw InternalErrorException("The supplied adapter did not return the expected predefined shader count.");
      }

      std::size_t predefinedShaderIndex = 0;
      for (const auto& info : predefinedShaders)
      {
        BasicShaderCreateInfo createInfo(info.Flag, info.Shader, info.VertexAttributeDescSpan);
        BasicShaderHandle hShader = CreateShader(createInfo);
        m_predefinedShaders[predefinedShaderIndex] = PredefinedShaderRecord(hShader);
        ++predefinedShaderIndex;
      }
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_ERROR("BasicShaderManager::Create predefined shaders threw a exception: {}", ex.what());
      ForceFreeAll();
      throw;
    }
  }


  BasicShaderManager::~BasicShaderManager() noexcept
  {
    // Force free everything
    ForceFreeAll();
    FSLLOG3_WARNING_IF(!m_records.Empty(), "BasicShaderManager: There are still {} shader objects allocated", m_records.Count());
  }


  BasicShaderHandle BasicShaderManager::CreateShader(const BasicShaderCreateInfo& createInfo)
  {
    ValidateBasicShaderCreateInfo(createInfo);

    assert(m_factory);

    const BasicNativeShaderCreateInfo nativeCreateInfo(createInfo.Flag, createInfo.Shader, createInfo.VertexAttributeDescSpan);
    BasicNativeShaderHandle nativeHandle = m_factory->CreateShader(nativeCreateInfo);
    try
    {
      const int32_t hShader = m_records.Add(BasicShaderRecord(nativeHandle));

      FSLLOG3(LocalConfig::LogType, "BasicShaderManager: CreateShader({}) count: {}", hShader, m_records.Count());
      return BasicShaderHandle(hShader);
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_ERROR("BasicShaderManager::CreateShader failed with: {}", ex.what());
      m_factory->DestroyShader(nativeHandle);
      throw;
    }
  }


  BasicShaderHandle BasicShaderManager::QueryPredefinedShaderHandle(const PredefinedShaderType shaderType)
  {
    const auto shaderIndex = static_cast<uint16_t>(shaderType);
    if (shaderIndex > m_predefinedShaders.size())
    {
      throw std::invalid_argument("CreateShader: unsupported shader type");
    }
    return m_predefinedShaders[shaderIndex].Handle;
  }


  BasicShaderHandle BasicShaderManager::ReferenceShader(const BasicShaderHandle hShader)
  {
    BasicShaderRecord& rRecord = m_records.Get(hShader.Value);
    if (rRecord.ReferenceCount >= std::numeric_limits<uint32_t>::max())
    {
      throw NotSupportedException("Max references reached");
    }
    ++rRecord.ReferenceCount;
    return hShader;
  }


  bool BasicShaderManager::DestroyShader(const BasicShaderHandle hShader) noexcept
  {
    BasicShaderRecord* pRecord = m_records.TryGet(hShader.Value);
    if (pRecord == nullptr)
    {
      return false;
    }
    if (pRecord->ReferenceCount > 1)
    {
      --pRecord->ReferenceCount;
      return true;
    }

    // Destroy the native shader instance
    auto nativeHandle = pRecord->NativeHandle;
    pRecord->NativeHandle = BasicNativeShaderHandle::Invalid();
    pRecord->ReferenceCount = 0;

    m_factory->DestroyShader(nativeHandle);

    // Then remove the record
    return m_records.Remove(hShader.Value);
  }


  void BasicShaderManager::ForceFreeAll() noexcept
  {
    FSLLOG3(LocalConfig::LogType, "BasicShaderManager: ForceFreeAll begin {} total shaders ({} predefined count)", m_records.Count(),
            m_predefinedShaders.size());
    if (!m_records.Empty())
    {    // Force free all normal resources
      for (uint32_t i = 0; i < m_records.Count(); ++i)
      {
        FSLLOG3_WARNING_IF(m_records[i].ReferenceCount > 1, "There are still {} outstanding references to the shader", m_records[i].ReferenceCount);
        FSLLOG3(LocalConfig::LogType, "Destroying Shader ({})", m_records.FastIndexToHandle(i));
        m_factory->DestroyShader(m_records[i].NativeHandle);
        m_records[i] = {};
      }
      m_records.Clear();
      m_predefinedShaders = {};
    }
    FSLLOG3(LocalConfig::LogType, "BasicShaderManager::ForceFreeAll done {} shaders", m_records.Count());
  }
}
