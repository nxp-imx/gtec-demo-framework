/****************************************************************************************************************************************************
 * Copyright 2020, 2022 NXP
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
#include <FslBase/IO/File.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/IO/FmtPathView.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxExtent2D.hpp>
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslDemoApp/Shared/Host/DemoWindowMetrics.hpp>
#include <FslGraphics/Render/Adapter/IDynamicNativeTexture2D.hpp>
#include <FslGraphics/Render/Adapter/INativeTexture2D.hpp>
#include <FslGraphics/Render/Basic/IBasicRenderSystem.hpp>
#include <FslGraphics/Render/Basic/Material/BasicMaterialCreateInfo.hpp>
#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslGraphics/Sprite/BasicNineSliceSprite.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslGraphics/Sprite/ImageSprite.hpp>
#include <FslGraphics/Sprite/Material/Basic/BasicSpriteMaterial.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialId.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialInfo.hpp>
#include <FslGraphics/Sprite/NineSliceSprite.hpp>
#include <FslGraphics/Sprite/OptimizedBasicNineSliceSprite.hpp>
#include <FslGraphics/Sprite/OptimizedNineSliceSprite.hpp>
#include <FslGraphics/TextureAtlas/CompatibilityTextureAtlasMap.hpp>
#include <FslGraphics/TextureAtlas/TestAtlasTextureGenerator.hpp>
#include <FslSimpleUI/App/UIAppConfig.hpp>
#include <FslSimpleUI/App/UIAppResourceManager.hpp>
#include <array>
#include <cmath>
#include <limits>
#include <utility>

namespace Fsl
{
  UIAppResourceManager::UIAppResourceManager(std::weak_ptr<IContentManager> contentManager, std::weak_ptr<IBasicRenderSystem> renderSystem,
                                             const DemoWindowMetrics& windowMetrics, const UITestPatternMode testPatternMode,
                                             const bool allowDepthBuffer, const bool defaultToDynamicMaterials, const bool useYFlipTextureCoordinates)
    : m_contentManager(std::move(contentManager))
    , m_renderSystem(std::move(renderSystem))
    , m_textureManager(windowMetrics.DensityDpi, testPatternMode)
    , m_materialManager(allowDepthBuffer, defaultToDynamicMaterials)
    , m_manager(windowMetrics.DensityDpi, useYFlipTextureCoordinates)
  {
  }

  UIAppResourceManager::~UIAppResourceManager()
  {
    m_manager.Clear();
    m_fonts.clear();
    m_nineSlices.clear();
    m_images.clear();
    m_materialManager.Clear();
    m_textureManager.Clear();
  }


  void UIAppResourceManager::SYS_SetRenderSystemViewport(const PxViewport& viewportPx)
  {
    auto renderSystemPtr = m_renderSystem.lock();
    if (!renderSystemPtr)
    {
      throw UsageErrorException("renderSystem is no longer valid");
    }
    m_materialManager.PatchMaterials(*renderSystemPtr, viewportPx);
  }


  void UIAppResourceManager::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    if (windowMetrics.DensityDpi == m_textureManager.GetDensityDpi())
    {
      // Nothing important was modified
      return;
    }
    m_textureManager.SetDensityDpi(windowMetrics.DensityDpi);

    auto renderSystem = m_renderSystem.lock();
    if (!renderSystem)
    {
      throw UsageErrorException("renderSystem is no longer valid");
    }

    auto contentManager = m_contentManager.lock();
    if (!contentManager)
    {
      throw UsageErrorException("contentManager is no longer valid");
    }

    // Then reload any missing materials
    UpdateDpAwareSpriteMaterials(*contentManager, *renderSystem, m_textureManager.GetDensityDpi());

    // Finally update the sprite manager
    m_manager.Resize(windowMetrics.DensityDpi);
  }


  bool UIAppResourceManager::FontExists(const UIAppTextureHandle textureHandle, IO::PathView fontName) const
  {
    auto contentManager = m_contentManager.lock();
    if (!contentManager)
    {
      FSLLOG3_DEBUG_WARNING("content manager is no longer valid");
      return false;
    }

    return m_textureManager.SpriteFontExists(*contentManager, textureHandle, fontName);
  }


  const CompatibilityTextureAtlasMap& UIAppResourceManager::GetLegacyTextureAtlasMap(const UIAppTextureHandle hTexture) const
  {
    return m_textureManager.GetAtlas(hTexture);
  }

  UIAppTexture<INativeTexture2D> UIAppResourceManager::CreateTexture(const IO::PathView& atlasPath,
                                                                     const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                                                     const UIAppResourceFlag flags)
  {
    auto renderSystem = m_renderSystem.lock();
    if (!renderSystem)
    {
      throw UsageErrorException("renderSystem is no longer valid");
    }
    auto contentManager = m_contentManager.lock();
    if (!contentManager)
    {
      throw UsageErrorException("contentManager is no longer valid");
    }
    return m_textureManager.CreateTexture(*contentManager, *renderSystem, atlasPath, textureCreationInfo, flags);
  }


  UIAppTexture<IDynamicNativeTexture2D> UIAppResourceManager::CreateDynamicTexture(const IO::PathView& atlasPath,
                                                                                   const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                                                                   const UIAppResourceFlag flags)
  {
    auto renderSystem = m_renderSystem.lock();
    if (!renderSystem)
    {
      throw UsageErrorException("renderSystem is no longer valid");
    }
    auto contentManager = m_contentManager.lock();
    if (!contentManager)
    {
      throw UsageErrorException("contentManager is no longer valid");
    }
    return m_textureManager.CreateDynamicTexture(*contentManager, *renderSystem, atlasPath, textureCreationInfo, flags);
  }

  void UIAppResourceManager::AddSpriteMaterial(const SpriteMaterialId& spriteMaterialId, const UIAppTextureHandle& hTexture,
                                               const BlendState blendState)
  {
    auto renderSystem = m_renderSystem.lock();
    if (!renderSystem)
    {
      throw UsageErrorException("renderSystem is no longer valid");
    }

    if (!m_textureManager.IsValidHandle(hTexture))
    {
      throw std::invalid_argument("invalid texture handle");
    }
    SimpleUIApp::UIAppTextureInfo textureInfo = m_textureManager.GetTextureInfo(hTexture);
    m_materialManager.AddMaterial(*renderSystem, spriteMaterialId, hTexture, textureInfo, blendState);
  }

  // --- platform independent custom texture sprites

  std::shared_ptr<BasicImageSprite> UIAppResourceManager::CreateCustomTextureSprite(const std::shared_ptr<INativeTexture2D>& nativeTexture,
                                                                                    const BlendState blendState)
  {
    if (!nativeTexture)
    {
      throw std::invalid_argument("nativeTexture can not be null");
    }
    auto renderSystem = m_renderSystem.lock();
    if (!renderSystem)
    {
      throw UsageErrorException("renderSystem is no longer valid");
    }

    // For now we just do this every-time (this will not scale well, but should be good enough for our small demos)
    PerformGarbageCollection();

    UIAppTextureHandle hTexture;
    SpriteMaterialId dynmamicSpriteMaterialId;
    try
    {
      // Register the custom texture to generate a handle for it
      hTexture = m_textureManager.RegisterExternalTexture(*renderSystem, nativeTexture);
      // Create the material for the sprite, here we do use a material type that keeps that 'texture' object alive as
      // long as the material is registered in the manager. This is ok since we use the sprite object to keep track of the
      // lifetime requirements of it. So once the sprite is deleted we can garbage collect the 'custom' sprite
      SimpleUIApp::UIAppTextureInfo textureInfo = m_textureManager.GetTextureInfo(hTexture);
      dynmamicSpriteMaterialId = m_materialManager.AddMaterial(*renderSystem, hTexture, textureInfo, blendState);

      // Finally create a custom texture record which we store a weak pointer to
      const auto materialInfo = m_materialManager.GetMaterialInfo(dynmamicSpriteMaterialId);
      const PxRectangleU16 imageRectanglePx(0, 0, materialInfo.MaterialInfo.ExtentPx.Width, materialInfo.MaterialInfo.ExtentPx.Height);

      // a texture sprite is always showed at 1:1 pixel ratio no matter the DPI and its DPI will never be modified
      const uint32_t densityDpi = m_manager.GetDensityDpi();
      const uint32_t imageDpi = densityDpi;
      auto sprite = std::make_shared<BasicImageSprite>(m_manager.GetSpriteNativeAreaCalc(), materialInfo.MaterialInfo, imageRectanglePx, imageDpi,
                                                       "Texture", densityDpi);

      m_customTextures.push_back(CustomTextureRecord{materialInfo.TextureHandle, dynmamicSpriteMaterialId, sprite});
      return sprite;
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_ERROR("A exception occured: {}", ex.what());

      if (dynmamicSpriteMaterialId.Value != 0)
      {
        assert(dynmamicSpriteMaterialId.Value >= UIAppConfig::MaterialId::DynamicOffset.Value);
        m_materialManager.RemoveMaterial(dynmamicSpriteMaterialId);
        dynmamicSpriteMaterialId = {};
      }
      m_textureManager.UnregisterTexture(hTexture);
      throw;
    }
  }

  // --- platform independent interface


  SpriteMaterialInfo UIAppResourceManager::GetSpriteMaterialInfo(const SpriteMaterialId& spriteMaterialId)
  {
    return m_materialManager.GetSpriteMaterialInfo(spriteMaterialId);
  }


  std::shared_ptr<BasicImageSprite> UIAppResourceManager::CreateBasicImageSprite(const SpriteMaterialId& spriteMaterialId,
                                                                                 const IO::PathView& atlasPathName)
  {
    const auto materialInfo = m_materialManager.GetMaterialInfo(spriteMaterialId);

    // Lookup the atlas texture information and then add the material to the manager
    const AtlasTextureInfo atlasTextureInfo = m_textureManager.GetAtlasTextureInfo(materialInfo.TextureHandle, atlasPathName);

    auto sprite = m_manager.AddBasicImageSprite(materialInfo.MaterialInfo, atlasTextureInfo, atlasPathName);

    m_images.push_back(ImageRecord{SpriteType::Basic, materialInfo.TextureHandle, IO::Path(atlasPathName), sprite});
    return sprite;
  }


  std::shared_ptr<ImageSprite> UIAppResourceManager::CreateImageSprite(const SpriteMaterialId& spriteMaterialId, const IO::PathView& atlasPathName)
  {
    const auto materialInfo = m_materialManager.GetMaterialInfo(spriteMaterialId);

    // Lookup the atlas texture information and then add the material to the manager
    AtlasTextureInfo atlasTextureInfo = m_textureManager.GetAtlasTextureInfo(materialInfo.TextureHandle, atlasPathName);
    auto sprite = m_manager.AddImageSprite(materialInfo.MaterialInfo, atlasTextureInfo, atlasPathName);

    m_images.push_back(ImageRecord{SpriteType::Normal, materialInfo.TextureHandle, IO::Path(atlasPathName), sprite});
    return sprite;
  }

  std::shared_ptr<BasicNineSliceSprite> UIAppResourceManager::CreateBasicNineSliceSprite(const SpriteMaterialId& spriteMaterialId,
                                                                                         const IO::PathView& atlasPathName)
  {
    const auto materialInfo = m_materialManager.GetMaterialInfo(spriteMaterialId);

    // Lookup the atlas texture information and then add the material to the manager
    const auto& atlas = m_textureManager.GetAtlas(materialInfo.TextureHandle);
    const AtlasTextureInfo atlasTextureInfo = atlas.GetAtlasTextureInfo(atlasPathName);
    const AtlasNineSlicePatchInfo atlasPatchInfo = atlas.GetAtlasNineSlicePatchInfo(atlasPathName);

    auto sprite = m_manager.AddBasicNineSliceSprite(materialInfo.MaterialInfo, atlasTextureInfo, atlasPatchInfo, atlasPathName);
    m_nineSlices.emplace_back(SpriteType::Basic, materialInfo.TextureHandle, IO::Path(atlasPathName), sprite);
    return sprite;
  }


  std::shared_ptr<NineSliceSprite> UIAppResourceManager::CreateNineSliceSprite(const SpriteMaterialId& spriteMaterialId,
                                                                               const IO::PathView& atlasPathName)
  {
    const auto materialInfo = m_materialManager.GetMaterialInfo(spriteMaterialId);

    const auto& atlas = m_textureManager.GetAtlas(materialInfo.TextureHandle);

    // Lookup the atlas texture information and then add the material to the manager
    const AtlasTextureInfo atlasTextureInfo = atlas.GetAtlasTextureInfo(atlasPathName);
    const AtlasNineSlicePatchInfo atlasPatchInfo = atlas.GetAtlasNineSlicePatchInfo(atlasPathName);

    auto sprite = m_manager.AddNineSliceSprite(materialInfo.MaterialInfo, atlasTextureInfo, atlasPatchInfo, atlasPathName);
    m_nineSlices.emplace_back(SpriteType::Normal, materialInfo.TextureHandle, IO::Path(atlasPathName), sprite);
    return sprite;
  }

  std::shared_ptr<OptimizedBasicNineSliceSprite>
    UIAppResourceManager::CreateOptimizedBasicNineSliceSprite(const SpriteMaterialId& opaqueSpriteMaterialId,
                                                              const SpriteMaterialId& transparentSpriteMaterialId, const IO::PathView& atlasPathName)
  {
    const auto opaqueMaterialInfo = m_materialManager.GetMaterialInfo(opaqueSpriteMaterialId);
    const auto transparentMaterialInfo = m_materialManager.GetMaterialInfo(transparentSpriteMaterialId);
    if (!opaqueMaterialInfo.MaterialInfo.IsOpaque)
    {
      throw std::invalid_argument("the opaque material must be opaque!");
    }
    if (transparentMaterialInfo.MaterialInfo.IsOpaque)
    {
      throw std::invalid_argument("the transparent material must be transparent!");
    }
    if (opaqueMaterialInfo.TextureHandle != transparentMaterialInfo.TextureHandle)
    {
      throw std::invalid_argument("the opaque and transparent material is expected to use the same texture");
    }

    const auto& atlas = m_textureManager.GetAtlas(opaqueMaterialInfo.TextureHandle);

    // Lookup the atlas texture information and then add the material to the manager
    const AtlasTextureInfo atlasTextureInfo = atlas.GetAtlasTextureInfo(atlasPathName);
    const AtlasNineSlicePatchInfo atlasPatchInfo = atlas.GetAtlasNineSlicePatchInfo(atlasPathName);

    auto sprite = m_manager.AddOptimizedBasicNineSliceSprite(opaqueMaterialInfo.MaterialInfo, transparentMaterialInfo.MaterialInfo, atlasTextureInfo,
                                                             atlasPatchInfo, atlasPathName);

    m_nineSlices.emplace_back(SpriteType::Basic, opaqueMaterialInfo.TextureHandle, atlasPathName, sprite);
    return sprite;
  }


  std::shared_ptr<OptimizedNineSliceSprite> UIAppResourceManager::CreateOptimizedNineSliceSprite(const SpriteMaterialId& opaqueSpriteMaterialId,
                                                                                                 const SpriteMaterialId& transparentSpriteMaterialId,
                                                                                                 const IO::PathView& atlasPathName)
  {
    const auto opaqueMaterialInfo = m_materialManager.GetMaterialInfo(opaqueSpriteMaterialId);
    const auto transparentMaterialInfo = m_materialManager.GetMaterialInfo(transparentSpriteMaterialId);
    if (!opaqueMaterialInfo.MaterialInfo.IsOpaque)
    {
      throw std::invalid_argument("the opaque material must be opaque!");
    }
    if (transparentMaterialInfo.MaterialInfo.IsOpaque)
    {
      throw std::invalid_argument("the transparent material must be transparent!");
    }
    if (opaqueMaterialInfo.TextureHandle != transparentMaterialInfo.TextureHandle)
    {
      throw std::invalid_argument("the opaque and transparent material is expected to use the same texture");
    }

    const auto& atlas = m_textureManager.GetAtlas(opaqueMaterialInfo.TextureHandle);

    // Lookup the atlas texture information and then add the material to the manager
    const AtlasTextureInfo atlasTextureInfo = atlas.GetAtlasTextureInfo(atlasPathName);
    const AtlasNineSlicePatchInfo atlasPatchInfo = atlas.GetAtlasNineSlicePatchInfo(atlasPathName);

    auto sprite = m_manager.AddOptimizedNineSliceSprite(opaqueMaterialInfo.MaterialInfo, transparentMaterialInfo.MaterialInfo, atlasTextureInfo,
                                                        atlasPatchInfo, atlasPathName);
    m_nineSlices.emplace_back(SpriteType::Normal, opaqueMaterialInfo.TextureHandle, IO::Path(atlasPathName), sprite);
    return sprite;
  }

  std::shared_ptr<SpriteFont> UIAppResourceManager::CreateLegacySpriteFont(const SpriteMaterialId& spriteMaterialId, const IO::PathView& fontName,
                                                                           const SpriteFontConfig& spriteFontConfig)
  {
    return DoCreateSpriteFont(spriteMaterialId, fontName, spriteFontConfig, true);
  }

  std::shared_ptr<SpriteFont> UIAppResourceManager::CreateSpriteFont(const SpriteMaterialId& spriteMaterialId, const IO::PathView& fontName,
                                                                     const SpriteFontConfig& spriteFontConfig)
  {
    return DoCreateSpriteFont(spriteMaterialId, fontName, spriteFontConfig, false);
  }

  std::shared_ptr<SpriteFont> UIAppResourceManager::TryCreateSpriteFont(const SpriteMaterialId& spriteMaterialId, const IO::PathView& fontName,
                                                                        const SpriteFontConfig& spriteFontConfig)
  {
    try
    {
      return DoCreateSpriteFont(spriteMaterialId, fontName, spriteFontConfig, false);
    }
    catch (const std::exception& /*ex*/)
    {
      // Failed to create sprite font
      return {};
    }
  }


  void UIAppResourceManager::PatchSpriteFont(std::shared_ptr<SpriteFont> font, const SpriteMaterialId& spriteMaterialId, const IO::PathView& fontName)
  {
    if (fontName.empty())
    {
      throw std::invalid_argument("fontName can not be empty");
    }
    if (fontName.find('/') != IO::PathView::npos)
    {
      throw std::invalid_argument("fontName can not contain '/'");
    }
    auto contentManager = m_contentManager.lock();
    if (!contentManager)
    {
      throw UsageErrorException("ContentManager is no longer valid");
    }

    const auto materialInfo = m_materialManager.GetMaterialInfo(spriteMaterialId);
    auto itrFindFont = std::find_if(m_fonts.begin(), m_fonts.end(), [font](const FontRecord& entry) { return entry.Font == font; });
    if (itrFindFont == m_fonts.end())
    {
      throw NotFoundException("Unknown font");
    }

    // Read the bitmap font file
    BitmapFont bitmapFont = m_textureManager.ReadSpriteFont(*contentManager, materialInfo.TextureHandle, fontName, false);

    m_manager.PatchSpriteFont(font, materialInfo.MaterialInfo, bitmapFont, fontName);

    itrFindFont->FontName = fontName;
    itrFindFont->TextureHandle = materialInfo.TextureHandle;


    m_materialManager.PatchSpriteFontMaterial(spriteMaterialId, materialInfo.MaterialInfo.ExtentPx, bitmapFont, materialInfo.TextureHandle);
  }


  bool UIAppResourceManager::SetOptions(const bool allowDepthBuffer)
  {
    if (allowDepthBuffer == m_materialManager.IsDepthBufferAllowed())
    {
      return false;
    }

    auto renderSystem = m_renderSystem.lock();
    if (renderSystem)
    {
      m_materialManager.SetOptions(*renderSystem, allowDepthBuffer);

      {    // Patch all content with the new material
        auto contentManager = m_contentManager.lock();
        const uint32_t textureCount = m_textureManager.Count();
        for (uint32_t i = 0; i < textureCount; ++i)
        {
          const auto texInfo = m_textureManager.FastGetInfoAt(i);
          PatchContent(texInfo.TextureHandle, texInfo.ExtentPx, contentManager.get());
        }
      }
    }
    return true;
  }


  bool UIAppResourceManager::SetTestPattern(const bool enabled)
  {
    return m_textureManager.SetTestPattern(enabled);
  }


  std::shared_ptr<SpriteFont> UIAppResourceManager::DoCreateSpriteFont(const SpriteMaterialId& spriteMaterialId, const IO::PathView& fontName,
                                                                       const SpriteFontConfig& spriteFontConfig, const bool isLegacyFullPathFontName)
  {
    if (fontName.empty())
    {
      throw std::invalid_argument("fontName can not be empty");
    }
    if (!isLegacyFullPathFontName && fontName.find('/') != IO::PathView::npos)
    {
      throw std::invalid_argument("fontName can not contain '/'");
    }
    auto contentManager = m_contentManager.lock();
    if (!contentManager)
    {
      throw UsageErrorException("ContentManager is no longer valid");
    }

    const auto materialInfo = m_materialManager.GetMaterialInfo(spriteMaterialId);

    // Read the bitmap font file
    BitmapFont bitmapFont = m_textureManager.ReadSpriteFont(*contentManager, materialInfo.TextureHandle, fontName, isLegacyFullPathFontName);

    auto spriteFont = m_manager.AddSpriteFont(materialInfo.MaterialInfo, bitmapFont, spriteFontConfig, fontName);
    m_fonts.push_back(FontRecord{materialInfo.TextureHandle, fontName, spriteFont});

    m_materialManager.PatchSpriteFontMaterial(spriteMaterialId, materialInfo.MaterialInfo.ExtentPx, bitmapFont, materialInfo.TextureHandle);

    return spriteFont;
  }


  void UIAppResourceManager::UpdateDpAwareSpriteMaterials(IContentManager& contentManager, IBasicRenderSystem& rRenderSystem,
                                                          const uint32_t densityDpi)
  {
    // Step1: Free all the current textures to ensure that we can reuse the memory during loading
    //        This frees DPI aware textures which actually use a different DPI texture for the new density dpi.
    {
      const uint32_t textureCount = m_textureManager.Count();
      for (uint32_t i = 0; i < textureCount; ++i)
      {
        if (m_textureManager.TryReleaseAtIfDifferentDpi(i, densityDpi))
        {
          const auto hTexture = m_textureManager.FastIndexToHandle(i);
          // Free the texture from all sprite fonts that use it
          for (auto& rFontRecord : m_fonts)
          {
            if (hTexture == rFontRecord.TextureHandle)
            {
              // Clear all associated content
              rFontRecord.Font->ClearContent();
            }
          }

          // NOTE: disabled for now since it would not completely release it anyway (both due to material cloning and because the textures destruction
          // is deferred)
          //
          // Then free the texture from all materials that use it
          // m_materialManager.ReleaseMaterialTexture(hTexture);
        }
      }
    }

    // Step2: load all missing textures
    {
      const uint32_t textureCount = m_textureManager.Count();
      for (uint32_t i = 0; i < textureCount; ++i)
      {
        // auto& rEntry = m_textureManager.FastAt(i);
        if (m_textureManager.FastIsTextureMissing(i))
        {
          auto hTexture = m_textureManager.ReloadTextureAt(i, contentManager, rRenderSystem);

          const auto* pTextureInfo = m_textureManager.TryFastGetTextureInfoAt(i);
          if (pTextureInfo != nullptr)
          {
            // Patch all materials that are used by the texture
            m_materialManager.PatchMaterials(rRenderSystem, hTexture, pTextureInfo->ExtentPx, pTextureInfo->Texture);
            PatchContent(hTexture, pTextureInfo->ExtentPx, &contentManager);
          }
        }
      }
    }
  }

  void UIAppResourceManager::PatchContent(const UIAppTextureHandle srcTextureHandle, const PxExtent2D srcExtentPx,
                                          IContentManager* const pContentManager)
  {
    PatchImages(srcTextureHandle, srcExtentPx);
    // Patch all nine-slice sprites that used the texture
    PatchNineSlices(srcTextureHandle, srcExtentPx);
    if (pContentManager != nullptr)
    {
      // Patch all sprite fonts that used the texture
      PatchFonts(srcTextureHandle, srcExtentPx, *pContentManager);
    }
  }

  void UIAppResourceManager::PatchImages(const UIAppTextureHandle srcTextureHandle, const PxExtent2D srcExtentPx)
  {
    for (const auto& rImage : m_images)
    {
      if (srcTextureHandle == rImage.TextureHandle)
      {
        FSLLOG3_VERBOSE2("Patching image sprite: '{}' of type {}", rImage.AtlasName, uint32_t(rImage.Type));
        // Lookup the atlas texture information and then patch the basic texture
        const auto& atlas = m_textureManager.GetAtlas(rImage.TextureHandle);
        const AtlasTextureInfo atlasTextureInfo = atlas.GetAtlasTextureInfo(rImage.AtlasName.AsPathView());

        const uint32_t materialCount = rImage.Sprite->GetMaterialCount();
        if (materialCount < 1u)
        {
          throw InternalErrorException("PatchImages: Sprites are expected to contain at least one material");
        }
        const SpriteMaterialInfo newSpriteMaterialInfo0 =
          m_materialManager.PatchMaterial(rImage.Sprite->GetMaterialInfo(0u).Id, srcExtentPx, srcTextureHandle);

        switch (materialCount)
        {
        case 1u:
          // Patch the material info with the new information
          m_manager.Patch(rImage.Sprite, newSpriteMaterialInfo0, atlasTextureInfo, rImage.AtlasName.AsPathView());
          break;
        case 2u:
          {
            const SpriteMaterialInfo newSpriteMaterialInfo1 =
              m_materialManager.PatchMaterial(rImage.Sprite->GetMaterialInfo(1u).Id, srcExtentPx, srcTextureHandle);
            // Patch the material info with the new information
            m_manager.Patch(rImage.Sprite, newSpriteMaterialInfo0, newSpriteMaterialInfo1, atlasTextureInfo, rImage.AtlasName.AsPathView());
            break;
          }
        default:
          throw NotSupportedException("PatchImages of this format is not supported");
        }
      }
    }
  }


  void UIAppResourceManager::PatchNineSlices(const UIAppTextureHandle srcTextureHandle, const PxExtent2D srcExtentPx)
  {
    for (const auto& rNineSlice : m_nineSlices)
    {
      if (srcTextureHandle == rNineSlice.TextureHandle)
      {
        FSLLOG3_VERBOSE2("Patching nine-slice sprite: '{}' of type {}", rNineSlice.AtlasName, uint32_t(rNineSlice.Type));
        // Lookup the atlas texture information and then patch the basic texture
        const auto& atlas = m_textureManager.GetAtlas(rNineSlice.TextureHandle);
        const AtlasTextureInfo atlasTextureInfo = atlas.GetAtlasTextureInfo(rNineSlice.AtlasName.AsPathView());
        const AtlasNineSlicePatchInfo atlasPatchInfo = atlas.GetAtlasNineSlicePatchInfo(rNineSlice.AtlasName);

        const uint32_t materialCount = rNineSlice.Sprite->GetMaterialCount();
        if (materialCount < 1u)
        {
          throw InternalErrorException("PatchNineSlices: Sprites are expected to contain at least one material");
        }

        const SpriteMaterialInfo newSpriteMaterialInfo0 =
          m_materialManager.PatchMaterial(rNineSlice.Sprite->GetMaterialInfo(0u).Id, srcExtentPx, srcTextureHandle);

        switch (materialCount)
        {
        case 1u:
          // Patch the material info with the new information
          m_manager.Patch(rNineSlice.Sprite, newSpriteMaterialInfo0, atlasTextureInfo, atlasPatchInfo, rNineSlice.AtlasName.AsPathView());
          break;
        case 2u:
          {
            const SpriteMaterialInfo newSpriteMaterialInfo1 =
              m_materialManager.PatchMaterial(rNineSlice.Sprite->GetMaterialInfo(1u).Id, srcExtentPx, srcTextureHandle);

            // Patch the material info with the new information
            m_manager.Patch(rNineSlice.Sprite, newSpriteMaterialInfo0, newSpriteMaterialInfo1, atlasTextureInfo, atlasPatchInfo,
                            rNineSlice.AtlasName.AsPathView());
            break;
          }
        default:
          throw NotSupportedException("PatchImages of this format is not supported");
        }
      }
    }
  }


  void UIAppResourceManager::PatchFonts(const UIAppTextureHandle srcTextureHandle, const PxExtent2D srcExtentPx, IContentManager& contentManager)
  {
    for (auto& rFontRecord : m_fonts)
    {
      if (srcTextureHandle == rFontRecord.TextureHandle)
      {
        FSLLOG3_VERBOSE2("Reloading font: '{}'", rFontRecord.FontName);
        auto bitmapFont = m_textureManager.ReadSpriteFont(contentManager, rFontRecord.TextureHandle, rFontRecord.FontName.AsPathView(), false);

        const auto& oldInfo = rFontRecord.Font->GetInfo().MaterialInfo;
        const auto oldMaterialInfo = m_materialManager.GetMaterialInfo(oldInfo.Id);
        assert(oldMaterialInfo.TextureHandle == srcTextureHandle);

        SpriteMaterialInfo newMaterialInfo = m_materialManager.PatchSpriteFontMaterial(oldInfo.Id, srcExtentPx, bitmapFont, srcTextureHandle);

        // Patch the material info with the new information
        m_manager.PatchSpriteFont(rFontRecord.Font, newMaterialInfo, bitmapFont, rFontRecord.FontName.AsPathView());
      }
    }
  }


  void UIAppResourceManager::PerformGarbageCollection()
  {
    FSLLOG3_VERBOSE5("UIAppResourceManager::PerformGarbageCollection");
    auto itr = m_customTextures.begin();
    while (itr != m_customTextures.end())
    {
      if (!itr->Sprite.expired())
      {
        ++itr;
      }
      else
      {
        FSLLOG3_VERBOSE5("- Custom texture expired, removing it. SpriteMaterialId: {}, TextureId: {}", itr->MaterialId.Value,
                         itr->TextureHandle.Value);

        m_materialManager.RemoveMaterial(itr->MaterialId);
        m_textureManager.UnregisterTexture(itr->TextureHandle);

        // erase the expired sprite entry
        itr = m_customTextures.erase(itr);
      }
    }
  }
}
