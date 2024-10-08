#ifndef SHARED_SYSTEM_DEVBASICRENDERCUSTOMSHADER_DEVBASICRENDERCUSTOMSHADERSHARED_HPP
#define SHARED_SYSTEM_DEVBASICRENDERCUSTOMSHADER_DEVBASICRENDERCUSTOMSHADERSHARED_HPP
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

#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslDemoApp/Base/DemoAppConfig.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslGraphics/Render/Basic/BasicMaterial.hpp>
#include <FslGraphics/Render/Basic/BasicShader.hpp>
#include <Shared/System/DevBasicRenderCustomShader/DevCustomCreateInfo.hpp>
#include <memory>
#include <utility>

namespace Fsl
{
  class IBasicRenderSystem;
  class IBasicStaticBuffer;
  class IBasicDynamicBuffer;
  class IContentManager;
  class INativeTexture2D;
  class VertexDeclarationSpan;

  class DevBasicRenderCustomShaderShared
  {
    struct CustomShadersRecord
    {
      BasicShader BasicVertFixedColor;
      BasicShader BasicFragFixedColor;

      CustomShadersRecord() = default;
      CustomShadersRecord(BasicShader basicVertFixedColor, BasicShader basicFragFixedColor)
        : BasicVertFixedColor(std::move(basicVertFixedColor))
        , BasicFragFixedColor(std::move(basicFragFixedColor))
      {
      }
    };

    struct MaterialsRecord
    {
      BasicMaterial MatOpaque;
      BasicMaterial MatOpaqueBasicVertFixedColor;
      BasicMaterial MatOpaqueBasicFragFixedColor;
      BasicMaterial MatOpaqueBasicFixedColor;

      MaterialsRecord() = default;
      MaterialsRecord(BasicMaterial matOpaque, BasicMaterial matOpaqueBasicVertFixedColor, BasicMaterial matOpaqueBasicFragFixedColor,
                      BasicMaterial matOpaqueBasicFixedColor)
        : MatOpaque(std::move(matOpaque))
        , MatOpaqueBasicVertFixedColor(std::move(matOpaqueBasicVertFixedColor))
        , MatOpaqueBasicFragFixedColor(std::move(matOpaqueBasicFragFixedColor))
        , MatOpaqueBasicFixedColor(std::move(matOpaqueBasicFixedColor))
      {
      }
    };

    struct Resources
    {
      std::shared_ptr<IBasicDynamicBuffer> QuadMeshVertices;
      MaterialsRecord Materials;
    };

    struct Matrices
    {
      Matrix Model;
      Matrix Proj;
      Matrix Translate;
      Matrix Orthographic0;
      Matrix Orthographic1;
      Matrix Orthographic2;
      Matrix Orthographic3;
    };

    std::shared_ptr<IBasicRenderSystem> m_render;

    Resources m_resources;
    Vector3 m_angle;

    Matrices m_matrices;

  public:
    explicit DevBasicRenderCustomShaderShared(const DemoAppConfig& config, const DevCustomCreateInfo& devCustomCreateInfo);

    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics);
    void Update(const DemoTime& time);
    void Draw();

  private:
    static CustomShadersRecord CreateCustomShaders(IBasicRenderSystem& render, const DevCustomCreateInfo& createInfo);
    static MaterialsRecord CreateMaterials(const IContentManager& contentManager, IBasicRenderSystem& render, const CustomShadersRecord& shaders,
                                           const VertexDeclarationSpan& vertexDeclSpan);
    static Matrices CalcMatrices(const DemoWindowMetrics& metrics);
  };
}

#endif
