/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxExtent2D.hpp>
#include <FslGraphics/Font/BitmapFont.hpp>
#include <FslGraphics/Font/SdfFontUtil.hpp>
#include <FslGraphics/Render/Basic/IBasicRenderSystem.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialCreateInfo.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialInfo.hpp>
#include <FslGraphics/Render/BlendStateUtil.hpp>
#include <FslGraphics/Sprite/Material/Basic/BasicSpriteMaterial.hpp>
#include <FslGraphics/Vertices/VertexPositionColorTexture.hpp>
#include <FslSimpleUI/App/Resource/UIAppMaterialManager.hpp>
#include <FslSimpleUI/App/Resource/UIAppTextureInfo.hpp>
#include <FslSimpleUI/App/UIAppConfig.hpp>
#include <fmt/format.h>

namespace Fsl::SimpleUIApp
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr auto MaterialCompareOp = BasicCompareOp::Less;
    }

    void DoPatchSpriteFontMaterial(const BitmapFont& bitmapFont, BasicSpriteMaterial* const pBasicMaterial)
    {
      if (bitmapFont.GetFontType() == BitmapFontType::SDF && pBasicMaterial != nullptr)
      {
        auto sdfParams = bitmapFont.GetSdfParams();
        float sdfSmooth = SdfFontUtil::CalcSmooth(sdfParams.Spread, sdfParams.Scale);
        pBasicMaterial->Material.SetSdfSmooth(sdfSmooth);
      }
    }
  }

  UIAppMaterialManager::UIAppMaterialManager(const bool allowDepthBuffer, const bool defaultToDynamicMaterials)
    : m_vertexDeclaration(VertexPositionColorTexture::AsVertexDeclarationSpan())
    , m_options(allowDepthBuffer, defaultToDynamicMaterials)
    , m_nextDynamicId(UIAppConfig::MaterialId::DynamicOffset.Value)
  {
  }

  UIAppMaterialManager::~UIAppMaterialManager() = default;


  void UIAppMaterialManager::Clear()
  {
    m_materials.clear();
  }

  bool UIAppMaterialManager::Contains(const SpriteMaterialId spriteMaterialId) const
  {
    return m_materials.find(spriteMaterialId) != m_materials.end();
  }

  void UIAppMaterialManager::AddMaterial(IBasicRenderSystem& rRenderSystem, const SpriteMaterialId spriteMaterialId,
                                         const UIAppTextureHandle hTexture, const UIAppTextureInfo& textureInfo, const BlendState blendState)
  {
    // verify that the material isn't defined!
    if (Contains(spriteMaterialId))
    {
      throw UsageErrorException(fmt::format("Material already defined {}", spriteMaterialId.Value));
    }

    const BasicMaterialDepthInfo opaqueDepthInfo(m_options.AllowDepthBuffer, m_options.AllowDepthBuffer, LocalConfig::MaterialCompareOp);
    const BasicMaterialDepthInfo transparentDepthInfo(m_options.AllowDepthBuffer, false, LocalConfig::MaterialCompareOp);
    const BasicMaterialDepthInfo& depthInfo = blendState != BlendState::Opaque ? transparentDepthInfo : opaqueDepthInfo;

    BasicMaterialCreateInfo matCreateInfo(blendState, BasicCullMode::Back, BasicFrontFace::Clockwise, depthInfo, m_vertexDeclaration.AsSpan());
    auto basicMaterial = std::make_shared<BasicSpriteMaterial>(rRenderSystem.CreateMaterial(matCreateInfo, textureInfo.Texture, m_options.IsDynamic));

    SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureInfo.ExtentPx, BlendStateUtil::IsOpaque(blendState), basicMaterial);
    m_materials.emplace(spriteMaterialId, MaterialRecord{hTexture, basicMaterial, spriteMaterialInfo, blendState});
  }


  SpriteMaterialId UIAppMaterialManager::AddMaterial(IBasicRenderSystem& rRenderSystem, const UIAppTextureHandle hTexture,
                                                     const UIAppTextureInfo& textureInfo, const BlendState blendState)
  {
    SpriteMaterialId materialId = CreateDynamicSpriteMaterialId();
    try
    {
      AddMaterial(rRenderSystem, materialId, hTexture, textureInfo, blendState);
      return materialId;
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_ERROR("Failed to AddMaterial: {}", ex.what());
      if (materialId.Value >= UIAppConfig::MaterialId::DynamicOffset.Value)
      {
        RemoveMaterial(materialId);
        DestroyDynamicSpriteMaterialId(materialId);
      }
      throw;
    }
  }


  bool UIAppMaterialManager::RemoveMaterial(const SpriteMaterialId id)
  {
    return m_materials.erase(id) > 0;
  }


  SpriteMaterialInfo UIAppMaterialManager::GetSpriteMaterialInfo(const SpriteMaterialId spriteMaterialId) const
  {
    auto itrFind = m_materials.find(spriteMaterialId);
    if (itrFind == m_materials.end())
    {
      throw NotFoundException(fmt::format("Unknown material {}", spriteMaterialId.Value));
    }
    return itrFind->second.MaterialInfo;
  }


  UIAppMaterialInfo UIAppMaterialManager::GetMaterialInfo(const SpriteMaterialId spriteMaterialId) const
  {
    auto itrFind = m_materials.find(spriteMaterialId);
    if (itrFind == m_materials.end())
    {
      throw NotFoundException(fmt::format("Unknown material {}", spriteMaterialId.Value));
    }
    return {itrFind->second.TextureHandle, itrFind->second.MaterialInfo};
  }


  bool UIAppMaterialManager::SetOptions(IBasicRenderSystem& rRenderSystem, const bool allowDepthBuffer)
  {
    if (allowDepthBuffer == m_options.AllowDepthBuffer)
    {
      return false;
    }
    m_options.AllowDepthBuffer = allowDepthBuffer;

    const BasicMaterialDepthInfo opaqueDepthInfo(m_options.AllowDepthBuffer, m_options.AllowDepthBuffer, LocalConfig::MaterialCompareOp);
    const BasicMaterialDepthInfo transparentDepthInfo(m_options.AllowDepthBuffer, false, LocalConfig::MaterialCompareOp);

    // If we had a method like "rRenderSystem.PatchMaterial" this would be much simpler, but it has its own set of problems.
    for (auto& rMaterialEntry : m_materials)
    {
      const auto& oldInfo = rMaterialEntry.second.MaterialInfo;
      FSLLOG3_VERBOSE3("Patching material: {} allowDepthBuffer: {}", oldInfo.Id.Value, allowDepthBuffer);

      // Patch the material info with the new depth information
      BasicMaterialInfo newMaterialInfo = rRenderSystem.GetMaterialInfo(rMaterialEntry.second.BasicMaterial->Material);

      newMaterialInfo.Depth = !oldInfo.IsOpaque ? transparentDepthInfo : opaqueDepthInfo;

      // Patch the actual material with the new texture
      auto newMaterial =
        std::make_shared<BasicSpriteMaterial>(rRenderSystem.CloneMaterial(rMaterialEntry.second.BasicMaterial->Material, newMaterialInfo));

      SpriteMaterialInfo spriteMaterialInfo(oldInfo.Id, oldInfo.ExtentPx, oldInfo.IsOpaque, newMaterial);

      rMaterialEntry.second.BasicMaterial = newMaterial;
      rMaterialEntry.second.MaterialInfo = spriteMaterialInfo;
    }
    return true;
  }


  void UIAppMaterialManager::PatchMaterials(IBasicRenderSystem& rRenderSystem, const PxViewport& viewportPx)
  {
    if (m_options.UseCustomViewport && viewportPx == m_options.CustomViewportPx)
    {
      return;
    }
    m_options.UseCustomViewport = true;
    m_options.CustomViewportPx = viewportPx;

    const BasicMaterialDepthInfo opaqueDepthInfo(m_options.AllowDepthBuffer, m_options.AllowDepthBuffer, LocalConfig::MaterialCompareOp);
    const BasicMaterialDepthInfo transparentDepthInfo(m_options.AllowDepthBuffer, false, LocalConfig::MaterialCompareOp);

    for (auto& rMatrialsPair : m_materials)
    {
      const BasicMaterialInfo materialInfo = rRenderSystem.GetMaterialInfo(rMatrialsPair.second.BasicMaterial->Material);

      const BasicMaterialDepthInfo& depthInfo = materialInfo.Blend != BlendState::Opaque ? transparentDepthInfo : opaqueDepthInfo;

      // Enable custom viewport on the material
      BasicMaterialInfo newMI(materialInfo.Blend, materialInfo.CullMode, materialInfo.CullFrontFace, depthInfo,
                              BasicViewportMode::CustomOriginTopLeft, viewportPx);
      rRenderSystem.SetMaterialInfo(rMatrialsPair.second.BasicMaterial->Material, newMI);
    }
  }


  SpriteMaterialInfo UIAppMaterialManager::PatchMaterial(const SpriteMaterialId spriteMaterialId, const PxExtent2D newExtentPx,
                                                         [[maybe_unused]] const UIAppTextureHandle debugExpectedTextureHandle)
  {
    auto itrFind = m_materials.find(spriteMaterialId);
    if (itrFind == m_materials.end())
    {
      throw NotFoundException("Could not find material");
    }
    assert(itrFind->second.TextureHandle == debugExpectedTextureHandle);

    SpriteMaterialInfo newInfo(spriteMaterialId, newExtentPx, itrFind->second.MaterialInfo.IsOpaque, itrFind->second.BasicMaterial);
    itrFind->second.MaterialInfo = newInfo;
    return newInfo;
  }


  SpriteMaterialInfo UIAppMaterialManager::PatchSpriteFontMaterial(const SpriteMaterialId spriteMaterialId, const PxExtent2D newExtentPx,
                                                                   const BitmapFont& bitmapFont, const UIAppTextureHandle debugExpectedTextureHandle)
  {
    FSL_PARAM_NOT_USED(debugExpectedTextureHandle);
    auto itrFind = m_materials.find(spriteMaterialId);
    if (itrFind == m_materials.end())
    {
      throw NotFoundException(fmt::format("Unknown material {}", spriteMaterialId.Value));
    }

    SpriteMaterialInfo newInfo(spriteMaterialId, newExtentPx, itrFind->second.MaterialInfo.IsOpaque, itrFind->second.BasicMaterial);
    itrFind->second.MaterialInfo = newInfo;

    DoPatchSpriteFontMaterial(bitmapFont, itrFind->second.BasicMaterial.get());
    return newInfo;
  }


  void UIAppMaterialManager::PatchMaterials(IBasicRenderSystem& rRenderSystem, const UIAppTextureHandle srcTextureHandle,
                                            const PxExtent2D srcExtentPx, const std::shared_ptr<INativeTexture2D>& srcTexture)
  {
    for (auto& rMaterialEntry : m_materials)
    {
      if (srcTextureHandle == rMaterialEntry.second.TextureHandle)
      {
        const auto& oldInfo = rMaterialEntry.second.MaterialInfo;
        FSLLOG3_VERBOSE3("Patching material: {} newExtent: {}, oldExtent: {}", oldInfo.Id.Value, srcExtentPx, oldInfo.ExtentPx);

        // Patch the actual material with the new texture
        auto newMaterial =
          std::make_shared<BasicSpriteMaterial>(rRenderSystem.CloneMaterial(rMaterialEntry.second.BasicMaterial->Material, srcTexture));

        // Patch the material info with the new information
        SpriteMaterialInfo spriteMaterialInfo(oldInfo.Id, srcExtentPx, oldInfo.IsOpaque, newMaterial);

        rMaterialEntry.second.BasicMaterial = newMaterial;
        rMaterialEntry.second.MaterialInfo = spriteMaterialInfo;
      }
    }
  }


  void UIAppMaterialManager::ReleaseMaterialTexture(const UIAppTextureHandle hTexture)
  {
    for (auto& rMaterialEntry : m_materials)
    {
      if (hTexture == rMaterialEntry.second.TextureHandle)
      {
        rMaterialEntry.second.MaterialInfo.Material.reset();
      }
    }
  }


  SpriteMaterialId UIAppMaterialManager::CreateDynamicSpriteMaterialId()
  {
    // nasty simple algorithm to find the next non-used id
    while (m_usedDynamicIds.find(m_nextDynamicId) != m_usedDynamicIds.end())
    {
      ++m_nextDynamicId;
      if (m_nextDynamicId < UIAppConfig::MaterialId::DynamicOffset.Value)
      {
        m_nextDynamicId = UIAppConfig::MaterialId::DynamicOffset.Value;
      }
    }
    const uint32_t dynamicId = m_nextDynamicId;
    ++m_nextDynamicId;
    if (m_nextDynamicId < UIAppConfig::MaterialId::DynamicOffset.Value)
    {
      m_nextDynamicId = UIAppConfig::MaterialId::DynamicOffset.Value;
    }

    assert(dynamicId >= UIAppConfig::MaterialId::DynamicOffset.Value);
    m_usedDynamicIds.insert(dynamicId);
    return SpriteMaterialId(dynamicId);
  }


  void UIAppMaterialManager::DestroyDynamicSpriteMaterialId(const SpriteMaterialId spriteMaterialId) noexcept
  {
    if (spriteMaterialId.Value >= UIAppConfig::MaterialId::DynamicOffset.Value)
    {
      m_usedDynamicIds.erase(spriteMaterialId.Value);
    }
  }
}
