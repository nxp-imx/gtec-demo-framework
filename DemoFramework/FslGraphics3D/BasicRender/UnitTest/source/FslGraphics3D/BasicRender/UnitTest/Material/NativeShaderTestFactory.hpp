#ifndef FSLGRAPHICS3D_BASICRENDER_UNITTEST_MATERIAL_FSLGRAPHICS3D_BASICRENDER_UNITTEST_NATIVESHADERTESTFACTORY_HPP
#define FSLGRAPHICS3D_BASICRENDER_UNITTEST_MATERIAL_FSLGRAPHICS3D_BASICRENDER_UNITTEST_NATIVESHADERTESTFACTORY_HPP
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
#include <FslBase/Exceptions.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeShaderCreateInfo.hpp>
#include <FslGraphics/Vertices/VertexAttributeDescriptionArray.hpp>
#include <FslGraphics/Vertices/VertexAttributeDescriptions.hpp>
#include <FslGraphics3D/BasicRender/Adapter/INativeShaderFactory.hpp>
#include <array>
#include <vector>

namespace Fsl
{
  class NativeShaderTestFactory final : public Graphics3D::INativeShaderFactory
  {
    struct Record
    {
      BasicShaderStageFlag Flag{BasicShaderStageFlag::Undefined};
      std::vector<uint8_t> Shader;
      VertexAttributeDescriptions VertexAttributeDesc;
    };

    HandleVector<Record> m_materials;

  public:
    ReadOnlySpan<BasicNativeShaderCreateInfo> GetPredefinedShaders() const final
    {
      static VertexAttributeDescriptionArray<3> decl = {
        VertexAttributeDescription(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0, "inVertexPosition"),
        VertexAttributeDescription(1, VertexElementFormat::Vector4, VertexElementUsage::Color, 0, "inVertexColor"),
        VertexAttributeDescription(2, VertexElementFormat::Vector2, VertexElementUsage::TextureCoordinate, 0, "inVertexTextureCoord")};
      static std::array<uint8_t, 1> empty{};
      static std::array<BasicNativeShaderCreateInfo, 3> entries = {
        BasicNativeShaderCreateInfo(BasicShaderStageFlag::Vertex, ReadOnlySpanUtil::AsSpan(empty), decl.AsReadOnlySpan()),
        BasicNativeShaderCreateInfo(BasicShaderStageFlag::Fragment, ReadOnlySpanUtil::AsSpan(empty), {}),
        BasicNativeShaderCreateInfo(BasicShaderStageFlag::Fragment, ReadOnlySpanUtil::AsSpan(empty), {}),
      };
      return ReadOnlySpanUtil::AsSpan(entries);
    }


    BasicNativeShaderHandle CreateShader(const BasicNativeShaderCreateInfo& createInfo) final
    {
      if (!createInfo.IsValid())
      {
        throw std::invalid_argument("must be valid");
      }

      auto handle = m_materials.Add(
        Record{createInfo.Flag, ReadOnlySpanUtil::ToVector(createInfo.Shader), VertexAttributeDescriptions(createInfo.VertexAttributeDescSpan)});

      return BasicNativeShaderHandle(handle);
    }

    bool DestroyShader(const BasicNativeShaderHandle hShader) noexcept final
    {
      return m_materials.Remove(hShader.Value);
    }
  };
}

#endif
