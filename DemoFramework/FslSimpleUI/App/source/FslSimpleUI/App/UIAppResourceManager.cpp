/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslSimpleUI/App/UIAppResourceManager.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/IO/File.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/IO/FmtPathView.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/String/StringToValue.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoApp/Shared/Host/DemoWindowMetrics.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Font/BitmapFontConverter.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Render/BlendStateUtil.hpp>
#include <FslGraphics/Render/Adapter/IDynamicNativeTexture2D.hpp>
#include <FslGraphics/Render/Adapter/INativeGraphics.hpp>
#include <FslGraphics/Render/Adapter/INativeTexture2D.hpp>
#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslGraphics/Sprite/BasicNineSliceSprite.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslGraphics/Sprite/ImageSprite.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialId.hpp>
#include <FslGraphics/Sprite/NineSliceSprite.hpp>
#include <FslGraphics/Sprite/SpriteDpConfig.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialInfo.hpp>
#include <FslGraphics/TextureAtlas/CompatibilityTextureAtlasMap.hpp>
#include <FslGraphics/TextureAtlas/TestAtlasTextureGenerator.hpp>
#include <array>
#include <cmath>
#include <limits>
#include <utility>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr StringViewLite AtlasExtension(".bta");
      // constexpr StringViewLite NewFontExtension(".nbf");
      constexpr StringViewLite OldFontExtension(".fbk");
      constexpr StringViewLite FilenameDpiPostfix("dpi");
      constexpr char FilenameDpStartChar = '_';

      // This is expected to be a sorted array (low to high)
      // Density:                                   0.5,0.6, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0
      constexpr std::array<uint32_t, 9> ValidDpis = {80, 96, 160, 240, 320, 400, 480, 560, 640};

      constexpr float PreferenceWeight = 0.97f;
      constexpr float UpscalePrefScale = 1.5f;

      constexpr uint32_t BaseDp = 160;
    }

    static_assert(LocalConfig::ValidDpis.size() <= 32u, "the EncodedAvailableDp encodes the indices in bits");

    bool IsValidSupportedDp(const uint64_t dp)
    {
      for (auto entry : LocalConfig::ValidDpis)
      {
        if (dp == entry)
        {
          return true;
        }
      }
      return false;
    }

    // 0         160        320        480        640
    // A          B          C          D          E
    // |----------|----------|----------|----------|
    //
    // |BBBBBBBBBB|BCCCCCCCCC|CDDDDDDDDD|EEEEEEEEEE|E

    float CalcScore(const uint32_t dpToScore, const uint32_t densityDpi)
    {
      const int32_t delta = (UncheckedNumericCast<int32_t>(densityDpi) - UncheckedNumericCast<int32_t>(dpToScore));
      static_assert(LocalConfig::PreferenceWeight >= 0.0f && LocalConfig::PreferenceWeight <= 1.0f, "PreferenceWeight not in a valid range");
      // 100.0f is just used to ensure that the we are flooring positive values
      float tmp = (float(delta) / float(LocalConfig::BaseDp)) + 100.0f + LocalConfig::PreferenceWeight;
      assert(tmp >= 0.0f);
      tmp = 100.0f - std::floor(tmp);
      return tmp >= 0.0f ? tmp : (-tmp) * LocalConfig::UpscalePrefScale;
    }

    BitmapFont LoadFromLegacyFontFormat(const IContentManager& contentManager, const IO::Path& path,
                                        const CompatibilityTextureAtlasMap* const pTextureAtlasMap)
    {
      if (pTextureAtlasMap == nullptr)
      {
        throw std::invalid_argument("no texture atlas available");
      }

      BasicFontKerning kerning;
      contentManager.Read(kerning, path);
      return BitmapFontConverter::ToBitmapFont(*pTextureAtlasMap, kerning, BitmapFontType::Bitmap);
    }
  }

  UIAppResourceManager::UIAppResourceManager(std::weak_ptr<IContentManager> contentManager, std::weak_ptr<INativeGraphics> nativeGraphics,
                                             const DemoWindowMetrics& windowMetrics, const UITestPatternMode testPatternMode)
    : m_contentManager(std::move(contentManager))
    , m_nativeGraphics(std::move(nativeGraphics))
    , m_densityDpi(windowMetrics.DensityDpi)
    , m_testPatternMode(testPatternMode)
    , m_manager(windowMetrics.DensityDpi)
    , m_testPatternEnabled(UITestPatternModeUtil::IsFlagged(testPatternMode, UITestPatternMode::Enabled))
  {
  }

  UIAppResourceManager::~UIAppResourceManager()
  {
    m_manager.Clear();
    m_fonts.clear();
    m_nineSlices.clear();
    m_images.clear();
    m_materials.clear();
    m_textureLookup.clear();
    m_textures.Clear();
  }


  void UIAppResourceManager::ConfigurationChanged(const DemoWindowMetrics& windowMetrics)
  {
    if (windowMetrics.DensityDpi == m_densityDpi)
    {
      // Nothing important was modified
      return;
    }
    m_densityDpi = windowMetrics.DensityDpi;

    auto nativeGraphics = m_nativeGraphics.lock();
    if (!nativeGraphics)
    {
      throw UsageErrorException("nativeGraphics is no longer valid");
    }

    auto contentManager = m_contentManager.lock();
    if (!contentManager)
    {
      throw UsageErrorException("contentManager is no longer valid");
    }

    // Then reload any missing materials
    UpdateDpAwareSpriteMaterials(*contentManager, *nativeGraphics, m_densityDpi);

    // Finally update the sprite manager
    m_manager.Resize(windowMetrics.DensityDpi);
  }


  const CompatibilityTextureAtlasMap& UIAppResourceManager::GetLegacyTextureAtlasMap(const UIAppTextureHandle hTexture) const
  {
    const TextureDefinition& textureRecord = m_textures.Get(hTexture.Value);
    if (!textureRecord.Atlas)
    {
      throw NotFoundException("No texture atlas associated with the given texture");
    }
    return *textureRecord.Atlas;
  }

  UIAppResourceManager::StaticTextureInfo UIAppResourceManager::CreateTexture(const IO::PathView& atlasPath,
                                                                              const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                                                              const UIAppResourceFlag flags)
  {
    auto nativeGraphics = m_nativeGraphics.lock();
    if (!nativeGraphics)
    {
      throw UsageErrorException("nativeGraphics is no longer valid");
    }
    // If this is a UI resource and pattern mode is set to allow switching we force this texture to be dynamic
    if (UIAppResourceFlagUtil::IsFlagged(flags, UIAppResourceFlag::UIGroup) &&
        UITestPatternModeUtil::IsFlagged(m_testPatternMode, UITestPatternMode::AllowSwitching))
    {
      auto res = CreateDynamicTexture(atlasPath, textureCreationInfo, flags);
      return {res.Handle, res.Texture, res.ExtentPx, res.TexturePixelFormat};
    }

    PrepareCreateResult prepareCreateResult = PrepareCreateTexture(atlasPath, textureCreationInfo, flags);
    assert(prepareCreateResult.TextureResult.SrcTexture.GetExtent().Depth == 1u);

    const PxExtent2D extent = prepareCreateResult.TextureResult.SrcTexture.GetExtent2D();
    const PixelFormat pixelFormat = prepareCreateResult.TextureResult.SrcTexture.GetPixelFormat();
    std::shared_ptr<INativeTexture2D> texture;
    {
      RawTexture srcRawTexture;
      Texture::ScopedDirectAccess directAccess(prepareCreateResult.TextureResult.SrcTexture, srcRawTexture);
      texture = nativeGraphics->CreateTexture2D(srcRawTexture, textureCreationInfo.Texture.FilterHint, textureCreationInfo.Texture.Flags);
    }

    auto handle = DoAddTexture(std::move(prepareCreateResult), texture, flags);
    return {handle, texture, extent, pixelFormat};
  }


  UIAppResourceManager::DynamicTextureInfo UIAppResourceManager::CreateDynamicTexture(const IO::PathView& atlasPath,
                                                                                      const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                                                                      const UIAppResourceFlag flags)
  {
    auto nativeGraphics = m_nativeGraphics.lock();
    if (!nativeGraphics)
    {
      throw UsageErrorException("nativeGraphics is no longer valid");
    }
    // Ensure the dynamic flag is set
    const auto modifiedFlags = flags | UIAppResourceFlag::Dynamic;

    PrepareCreateResult prepareCreateResult = PrepareCreateTexture(atlasPath, textureCreationInfo, flags);
    assert(prepareCreateResult.TextureResult.SrcTexture.GetExtent().Depth == 1u);

    const PxExtent2D extent = prepareCreateResult.TextureResult.SrcTexture.GetExtent2D();
    const PixelFormat pixelFormat = prepareCreateResult.TextureResult.SrcTexture.GetPixelFormat();
    std::shared_ptr<IDynamicNativeTexture2D> texture;
    {
      RawTexture srcRawTexture;
      Texture::ScopedDirectAccess directAccess(prepareCreateResult.TextureResult.SrcTexture, srcRawTexture);
      texture = nativeGraphics->CreateDynamicTexture2D(srcRawTexture, textureCreationInfo.Texture.FilterHint, textureCreationInfo.Texture.Flags);
    }
    auto handle = DoAddTexture(std::move(prepareCreateResult), texture, modifiedFlags);
    return {handle, texture, extent, pixelFormat};
  }


  void UIAppResourceManager::AddSpriteMaterial(const SpriteMaterialId& spriteMaterialId, const UIAppTextureHandle& hTexture,
                                               const BlendState blendState)
  {
    if (!m_textures.IsValidHandle(hTexture.Value))
    {
      throw std::invalid_argument("invalid texture handle");
    }

    // verify that the material isn't defined!
    auto itrFind = m_materials.find(spriteMaterialId);
    if (itrFind != m_materials.end())
    {
      throw UsageErrorException(fmt::format("Material already defined {}", spriteMaterialId.Value));
    }

    const TextureDefinition& textureRecord = m_textures.Get(hTexture.Value);

    const auto nativeMaterialFlags = static_cast<uint32_t>(blendState);
    SpriteMaterialInfo spriteMaterialInfo(spriteMaterialId, textureRecord.ExtentPx, BlendStateUtil::IsOpaque(blendState), textureRecord.Texture,
                                          nativeMaterialFlags);
    m_materials.emplace(spriteMaterialId, MaterialRecord{hTexture, spriteMaterialInfo});
  }


  // --- platform independent interface


  SpriteMaterialInfo UIAppResourceManager::GetSpriteMaterialInfo(const SpriteMaterialId& spriteMaterialId)
  {
    auto itrFind = m_materials.find(spriteMaterialId);
    if (itrFind == m_materials.end())
    {
      throw NotFoundException(fmt::format("Unknown material {}", spriteMaterialId.Value));
    }
    return itrFind->second.MaterialInfo;
  }


  std::shared_ptr<BasicImageSprite> UIAppResourceManager::CreateBasicImageSprite(const SpriteMaterialId& spriteMaterialId,
                                                                                 const IO::PathView& atlasPathName)
  {
    auto itrFind = m_materials.find(spriteMaterialId);
    if (itrFind == m_materials.end())
    {
      throw NotFoundException(fmt::format("Unknown material {}", spriteMaterialId.Value));
    }

    const TextureDefinition& textureDefinition = m_textures.Get(itrFind->second.TextureHandle.Value);

    // Lookup the atlas texture information and then add the material to the manager
    const AtlasTextureInfo& atlasTextureInfo = GetAtlasTextureInfo(textureDefinition, atlasPathName);

    auto sprite = m_manager.AddBasicImageSprite(itrFind->second.MaterialInfo, atlasTextureInfo, atlasPathName);

    m_images.push_back(ImageRecord{SpriteType::Basic, itrFind->second.TextureHandle, IO::Path(atlasPathName), sprite});
    return sprite;
  }


  std::shared_ptr<ImageSprite> UIAppResourceManager::CreateImageSprite(const SpriteMaterialId& spriteMaterialId, const IO::PathView& atlasPathName)
  {
    auto itrFind = m_materials.find(spriteMaterialId);
    if (itrFind == m_materials.end())
    {
      throw NotFoundException(fmt::format("Unknown material {}", spriteMaterialId.Value));
    }

    const TextureDefinition& textureDefinition = m_textures.Get(itrFind->second.TextureHandle.Value);

    // Lookup the atlas texture information and then add the material to the manager
    AtlasTextureInfo atlasTextureInfo = GetAtlasTextureInfo(textureDefinition, atlasPathName);
    auto sprite = m_manager.AddImageSprite(itrFind->second.MaterialInfo, atlasTextureInfo, atlasPathName);

    m_images.push_back(ImageRecord{SpriteType::Normal, itrFind->second.TextureHandle, IO::Path(atlasPathName), sprite});
    return sprite;
  }

  std::shared_ptr<BasicNineSliceSprite> UIAppResourceManager::CreateBasicNineSliceSprite(const SpriteMaterialId& spriteMaterialId,
                                                                                         const IO::PathView& atlasPathName)
  {
    auto itrFind = m_materials.find(spriteMaterialId);
    if (itrFind == m_materials.end())
    {
      throw NotFoundException(fmt::format("Unknown material {}", spriteMaterialId.Value));
    }

    const TextureDefinition& textureDefinition = m_textures.Get(itrFind->second.TextureHandle.Value);

    // Lookup the atlas texture information and then add the material to the manager
    const AtlasTextureInfo& atlasTextureInfo = GetAtlasTextureInfo(textureDefinition, atlasPathName);
    // We rely on GetAtlasTextureInfo to complain if the atlas is missing
    assert(textureDefinition.Atlas);
    const AtlasNineSlicePatchInfo& atlasPatchInfo = textureDefinition.Atlas->GetAtlasNineSlicePatchInfo(atlasPathName);

    auto sprite = m_manager.AddBasicNineSliceSprite(itrFind->second.MaterialInfo, atlasTextureInfo, atlasPatchInfo, atlasPathName);
    m_nineSlices.push_back(NineSliceRecord{SpriteType::Basic, itrFind->second.TextureHandle, IO::Path(atlasPathName), sprite});
    return sprite;
  }


  std::shared_ptr<NineSliceSprite> UIAppResourceManager::CreateNineSliceSprite(const SpriteMaterialId& spriteMaterialId,
                                                                               const IO::PathView& atlasPathName)
  {
    auto itrFind = m_materials.find(spriteMaterialId);
    if (itrFind == m_materials.end())
    {
      throw NotFoundException(fmt::format("Unknown material {}", spriteMaterialId.Value));
    }

    const TextureDefinition& textureDefinition = m_textures.Get(itrFind->second.TextureHandle.Value);

    // Lookup the atlas texture information and then add the material to the manager
    const AtlasTextureInfo& atlasTextureInfo = GetAtlasTextureInfo(textureDefinition, atlasPathName);
    // We rely on GetAtlasTextureInfo to complain if the atlas is missing
    assert(textureDefinition.Atlas);
    const AtlasNineSlicePatchInfo& atlasPatchInfo = textureDefinition.Atlas->GetAtlasNineSlicePatchInfo(atlasPathName);

    auto sprite = m_manager.AddNineSliceSprite(itrFind->second.MaterialInfo, atlasTextureInfo, atlasPatchInfo, atlasPathName);
    m_nineSlices.push_back(NineSliceRecord{SpriteType::Normal, itrFind->second.TextureHandle, IO::Path(atlasPathName), sprite});
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


  bool UIAppResourceManager::SetTestPattern(const bool enabled)
  {
    if (!UITestPatternModeUtil::IsFlagged(m_testPatternMode, UITestPatternMode::AllowSwitching))
    {
      FSLLOG3_WARNING(
        "SetTestPattern: UIAppResourceManager was not created with UITestPatternMode::DisabledAllowSwitching or "
        "UITestPatternMode::EnableAllowSwitching enabled, so request ignored");
      return false;
    }
    if (enabled == m_testPatternEnabled)
    {
      return false;
    }

    m_testPatternEnabled = enabled;
    ApplyTestPattern();
    return true;
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

    auto itrFind = m_materials.find(spriteMaterialId);
    if (itrFind == m_materials.end())
    {
      throw NotFoundException(fmt::format("Unknown material {}", spriteMaterialId.Value));
    }

    if (!isLegacyFullPathFontName)
    {
      ValidateSpriteFontExistence(*contentManager, itrFind->second.TextureHandle, fontName);
    }

    // Read the bitmap font file
    BitmapFont bitmapFont = ReadSpriteFont(*contentManager, itrFind->second.TextureHandle, fontName, isLegacyFullPathFontName);

    auto spriteFont = m_manager.AddSpriteFont(itrFind->second.MaterialInfo, bitmapFont, spriteFontConfig, fontName);
    m_fonts.push_back(FontRecord{itrFind->second.TextureHandle, fontName, spriteFont});
    return spriteFont;
  }


  void UIAppResourceManager::UpdateDpAwareSpriteMaterials(IContentManager& contentManager, INativeGraphics& rNativeGraphics,
                                                          const uint32_t densityDpi)
  {
    // Step1: Free all the current textures to ensure that we can reuse the memory during loading
    //        This frees dp aware textures which actually use a different dp texture for the new density dpi.
    for (std::size_t i = 0; i < m_textures.Count(); ++i)
    {
      auto& rEntry = m_textures[i];
      if (!rEntry.PathInfo.AvailableDp.Empty())
      {
        const uint32_t newDp = DetermineResourceDp(rEntry.PathInfo, densityDpi);
        if (newDp != rEntry.Dpi)
        {
          FSLLOG3_VERBOSE("Freeing dp aware material '{}'", BuildResourceName(rEntry.PathInfo, rEntry.Dpi))

          rEntry.Dpi = newDp;
          rEntry.Texture.reset();
          rEntry.Atlas.reset();

          const auto hTexture = m_textures.IndexToHandle(i);
          // Free the texture from all sprite fonts that use it
          for (auto& rFontRecord : m_fonts)
          {
            if (hTexture == rFontRecord.TextureHandle.Value)
            {
              // Clear all associated content
              rFontRecord.Font->ClearContent();
            }
          }

          // Disabled because the textures backing the images are 'async freed' anyway
          // Free the texture from all image sprites that use it
          // for (auto& rImageRecord : m_images)
          //{
          //  if (hTexture == rImageRecord.TextureHandle.Value)
          //  {
          //    // Clear all associated content
          //    rImageRecord.Sprite->ClearContent();
          //  }
          //}

          // Then free the texture from all materials that use it
          for (auto& rMaterialEntry : m_materials)
          {
            if (hTexture == rMaterialEntry.second.TextureHandle.Value)
            {
              rMaterialEntry.second.MaterialInfo.Material.reset();
            }
          }
        }
      }
    }

    // Step2: load all missing textures
    for (std::size_t i = 0; i < m_textures.Count(); ++i)
    {
      auto& rEntry = m_textures[i];
      if (!rEntry.PathInfo.AvailableDp.Empty() && !rEntry.Texture)
      {
        const bool isAtlas = UIAppResourceFlagUtil::IsFlagged(rEntry.Flags, UIAppResourceFlag::Atlas);
        PrepareTextureResult result = PrepareTexture(contentManager, rEntry.PathInfo, isAtlas, rEntry.Dpi, rEntry.CreationInfo, m_testPatternMode);
        assert(result.SrcTexture.GetExtent().Depth == 1u);
        {
          RawTexture srcRawTexture;
          Texture::ScopedDirectAccess directAccess(result.SrcTexture, srcRawTexture);
          rEntry.Texture =
            (!UIAppResourceFlagUtil::IsFlagged(rEntry.Flags, UIAppResourceFlag::Dynamic)
               ? rNativeGraphics.CreateTexture2D(srcRawTexture, rEntry.CreationInfo.Texture.FilterHint, rEntry.CreationInfo.Texture.Flags)
               : rNativeGraphics.CreateDynamicTexture2D(srcRawTexture, rEntry.CreationInfo.Texture.FilterHint, rEntry.CreationInfo.Texture.Flags));
        }
        rEntry.ExtentPx = result.SrcTexture.GetExtent2D();
        rEntry.Atlas = std::move(result.Atlas);

        const auto hTexture = m_textures.IndexToHandle(i);
        // Cache the current texture and test pattern (if enabled)
        AttachTestPattern(UIAppTextureHandle(hTexture), std::move(result.SrcTexture), rEntry.Flags);

        // Apply test patterns if enabled
        if (m_testPatternEnabled)
        {
          ApplyTestPattern();
        }

        // Patch all materials that used the texture
        PatchMaterials(hTexture, rEntry);
        // Patch all image sprites that used the texture
        PatchImages(hTexture, rEntry);
        // Patch all nineslice sprites that used the texture
        PatchNineSlices(hTexture, rEntry);
        // Patch all sprite fonts that used the texture
        PatchFonts(hTexture, contentManager, rEntry);
      }
    }
  }

  void UIAppResourceManager::PatchMaterials(const HandleVector<TextureDefinition>::handle_type hTexture,
                                            HandleVector<TextureDefinition>::const_reference rEntry)
  {
    for (auto& rMaterialEntry : m_materials)
    {
      if (hTexture == rMaterialEntry.second.TextureHandle.Value)
      {
        const auto& oldInfo = rMaterialEntry.second.MaterialInfo;
        FSLLOG3_VERBOSE3("Patching material: {}", oldInfo.Id.Value);
        // Patch the material info with the new information
        SpriteMaterialInfo spriteMaterialInfo(oldInfo.Id, oldInfo.ExtentPx, oldInfo.IsOpaque, rEntry.Texture, oldInfo.NativeMaterialFlags);
        rMaterialEntry.second.MaterialInfo = spriteMaterialInfo;
      }
    }
  }

  void UIAppResourceManager::PatchImages(const HandleVector<TextureDefinition>::handle_type hTexture,
                                         HandleVector<TextureDefinition>::const_reference rEntry)
  {
    for (const auto& rImage : m_images)
    {
      if (hTexture == rImage.TextureHandle.Value)
      {
        FSLLOG3_VERBOSE2("Patching image sprite: '{}' of type {}", rImage.AtlasName, uint32_t(rImage.Type));
        // Lookup the atlas texture information and then patch the basic texture
        const TextureDefinition& textureDefinition = m_textures.Get(rImage.TextureHandle.Value);
        const AtlasTextureInfo& atlasTextureInfo = GetAtlasTextureInfo(textureDefinition, rImage.AtlasName.AsPathView());

        const auto& oldInfo = rImage.Sprite->GetMaterialInfo();

        // Patch the material info with the new information
        m_manager.Patch(rImage.Sprite, SpriteMaterialInfo(oldInfo.Id, rEntry.ExtentPx, oldInfo.IsOpaque, rEntry.Texture, oldInfo.NativeMaterialFlags),
                        atlasTextureInfo, rImage.AtlasName.AsPathView());
      }
    }
  }


  void UIAppResourceManager::PatchNineSlices(const HandleVector<TextureDefinition>::handle_type hTexture,
                                             HandleVector<TextureDefinition>::const_reference rEntry)
  {
    for (const auto& rNineSlice : m_nineSlices)
    {
      if (hTexture == rNineSlice.TextureHandle.Value)
      {
        FSLLOG3_VERBOSE2("Patching nine-slice sprite: '{}' of type {}", rNineSlice.AtlasName, uint32_t(rNineSlice.Type));
        // Lookup the atlas texture information and then patch the basic texture
        const TextureDefinition& textureDefinition = m_textures.Get(rNineSlice.TextureHandle.Value);
        const AtlasTextureInfo& atlasTextureInfo = GetAtlasTextureInfo(textureDefinition, rNineSlice.AtlasName.AsPathView());
        // We rely on GetAtlasTextureInfo to complain if the atlas is missing
        assert(textureDefinition.Atlas);
        const AtlasNineSlicePatchInfo& atlasPatchInfo = textureDefinition.Atlas->GetAtlasNineSlicePatchInfo(rNineSlice.AtlasName);

        const auto& oldInfo = rNineSlice.Sprite->GetMaterialInfo();

        // Patch the material info with the new information
        m_manager.Patch(rNineSlice.Sprite,
                        SpriteMaterialInfo(oldInfo.Id, rEntry.ExtentPx, oldInfo.IsOpaque, rEntry.Texture, oldInfo.NativeMaterialFlags),
                        atlasTextureInfo, atlasPatchInfo, rNineSlice.AtlasName.AsPathView());
      }
    }
  }


  void UIAppResourceManager::PatchFonts(const HandleVector<TextureDefinition>::handle_type hTexture, IContentManager& contentManager,
                                        HandleVector<TextureDefinition>::const_reference rEntry)
  {
    for (auto& rFontRecord : m_fonts)
    {
      if (hTexture == rFontRecord.TextureHandle.Value)
      {
        FSLLOG3_VERBOSE2("Reloading font: '{}'", rFontRecord.FontName);
        auto bitmapFont = ReadSpriteFont(contentManager, rFontRecord.TextureHandle, rFontRecord.FontName.AsPathView(), false);

        const auto& oldInfo = rFontRecord.Font->GetInfo().MaterialInfo;
        // Patch the material info with the new information
        m_manager.PatchSpriteFont(rFontRecord.Font,
                                  SpriteMaterialInfo(oldInfo.Id, rEntry.ExtentPx, oldInfo.IsOpaque, rEntry.Texture, oldInfo.NativeMaterialFlags),
                                  bitmapFont, rFontRecord.FontName.AsPathView());
      }
    }
  }


  UIAppResourceManager::PrepareCreateResult UIAppResourceManager::PrepareCreateTexture(const IO::PathView& atlasPath,
                                                                                       const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                                                                       const UIAppResourceFlag flags)
  {
    auto contentManager = m_contentManager.lock();
    if (!contentManager)
    {
      throw UsageErrorException("ContentManager is no longer valid");
    }
    const bool allowDpAware = !UIAppResourceFlagUtil::IsFlagged(flags, UIAppResourceFlag::NotDpAware);


    // Determine if this is a DP aware resource
    auto pathInfo = AnalyzePath(*contentManager, atlasPath, allowDpAware);
    auto selectedDp = DetermineResourceDp(pathInfo, m_densityDpi);

    auto itrFind = m_textureLookup.find(pathInfo.SrcPath);
    if (itrFind != m_textureLookup.end())
    {
      throw std::invalid_argument(fmt::format("The texture '{}' resolves to '{}' which has already been created.", atlasPath, pathInfo.SrcPath));
    }

    const bool isAtlas = UIAppResourceFlagUtil::IsFlagged(flags, UIAppResourceFlag::Atlas);
    return {textureCreationInfo, pathInfo, selectedDp,
            PrepareTexture(*contentManager, pathInfo, isAtlas, selectedDp, textureCreationInfo, m_testPatternMode)};
  }


  UIAppTextureHandle UIAppResourceManager::DoAddTexture(PrepareCreateResult prepareCreateResult,
                                                        const std::shared_ptr<INativeTexture2D>& nativeTexture, const UIAppResourceFlag flags)
  {
    const auto& key = prepareCreateResult.PathInfo.SrcPath;
    auto handle = HandleVectorConfig::InvalidHandle;
    try
    {
      const PxExtent2D extentPx = prepareCreateResult.TextureResult.SrcTexture.GetExtent2D();

      handle = m_textures.Add(TextureDefinition{prepareCreateResult.CreationInfo, prepareCreateResult.PathInfo, prepareCreateResult.Dpi,
                                                nativeTexture, extentPx, flags, std::move(prepareCreateResult.TextureResult.Atlas)});
      UIAppTextureHandle hTexture(handle);
      m_textureLookup[key] = hTexture;

      AttachTestPattern(hTexture, std::move(prepareCreateResult.TextureResult.SrcTexture), flags);
      return hTexture;
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_ERROR("Failed to add due to a exception. {}", ex.what());
      if (handle != HandleVectorConfig::InvalidHandle)
      {
        m_testPatternTextures.erase(UIAppTextureHandle(handle));
        m_textureLookup.erase(key);
        m_textures.Remove(handle);
      }
      throw;
    }
  }


  UIAppResourceManager::PrepareTextureResult UIAppResourceManager::PrepareTexture(const IContentManager& contentManager, const AnalyzedPath& pathInfo,
                                                                                  const bool isAtlas, const uint32_t selectedDp,
                                                                                  const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                                                                  const UITestPatternMode testPatternMode)
  {
    IO::Path resourceName(BuildResourceName(pathInfo, selectedDp));

    FSLLOG3_VERBOSE("Loading texture '{0}'", resourceName)

    auto texture = contentManager.ReadTexture(resourceName, textureCreationInfo.DesiredPixelFormat, textureCreationInfo.DesiredOrigin,
                                              textureCreationInfo.PreferredChannelOrder);

    const auto textureExtentPx = texture.GetExtent();
    if (textureExtentPx.Depth != 1u)
    {
      throw std::invalid_argument("Must be a 2d texture");
    }
    std::unique_ptr<CompatibilityTextureAtlasMap> textureAtlasMap;
    if (isAtlas)
    {
      auto directoryname = IO::Path::GetDirectoryNameView(resourceName.AsPathView());
      auto filenameWithoutExt = IO::Path::GetFileNameWithoutExtensionView(resourceName.AsPathView());
      IO::Path atlasPath(IO::Path::Combine(directoryname, IO::Path(fmt::format("{}{}", filenameWithoutExt, LocalConfig::AtlasExtension))));
      FSLLOG3_VERBOSE("Loading texture atlas '{0}'", atlasPath)
      BasicTextureAtlas textureAtlas;
      contentManager.Read(textureAtlas, atlasPath);
      textureAtlasMap = std::make_unique<CompatibilityTextureAtlasMap>(std::move(textureAtlas));
    }

    if (UITestPatternModeUtil::IsFlagged(testPatternMode, UITestPatternMode::Enabled))
    {
      if (!PixelFormatUtil::IsCompressed(texture.GetPixelFormat()))
      {
        if (textureAtlasMap)
        {
          assert(isAtlas);
          TestAtlasTextureGenerator::PatchWithTestPattern(texture, *textureAtlasMap);
        }
        else
        {
          assert(!isAtlas);
          TestAtlasTextureGenerator::PatchWithTestPattern(texture);
        }
      }
    }
    return {texture, std::move(textureAtlasMap)};
  }

  //! Analyze the path to see if the filename without extension ends in "_<number>dp"
  UIAppResourceManager::AnalyzedPath UIAppResourceManager::AnalyzePath(const IContentManager& contentManager, const IO::PathView pathView,
                                                                       const bool allowDpAware)
  {
    if (allowDpAware)
    {
      auto pathExtView = IO::Path::GetExtensionView(pathView);
      auto pathFilenameView = IO::Path::GetFileNameWithoutExtensionView(pathView);
      auto pathDirectoryView = IO::Path::GetDirectoryNameView(pathView);

      if (pathFilenameView.ends_with(LocalConfig::FilenameDpiPostfix))
      {
        // we know the filename ends with 'dp', so lets try to see if we can find a '_'
        auto parseFilenameView(pathFilenameView);
        parseFilenameView.remove_suffix(LocalConfig::FilenameDpiPostfix.size());
        const auto index = parseFilenameView.rfind(LocalConfig::FilenameDpStartChar);
        if (index != IO::PathView::npos)
        {
          const auto parsedFilename = parseFilenameView.substr(0, index + 1u);
          auto parsedDpPart = parseFilenameView.substr(index + 1u);
          if (!parsedFilename.empty() && !parsedDpPart.empty())
          {
            // We found a possible match, so lets try to parse it as a number
            uint64_t value = 0;
            if (StringToValue::TryParse(value, parsedDpPart) && IsValidSupportedDp(value))
            {
              // We successfully parsed the value and it was a supported dp, so this is a valid "_<num>dp" pattern
              auto patternPath = IO::Path::Combine(pathDirectoryView,
                                                   IO::Path(fmt::format("{}{{}}{}{}", parsedFilename, LocalConfig::FilenameDpiPostfix, pathExtView)));
              EncodedAvailableDp availableDp(DetermineAvailableDp(contentManager, patternPath));
              // If there are less than two available graphics resources we just treat the path as a normal one and disable the pattern
              if (availableDp.Count() > 1u)
              {
                return {patternPath, availableDp};
              }
              FSLLOG3_WARNING_IF(availableDp.Count() <= 0u, "File pattern '{}' did not contain any of the supported dp's, disabling dp awareness",
                                 patternPath);
            }
          }
        }
      }
    }
    return {pathView, EncodedAvailableDp()};
  }

  UIAppResourceManager::EncodedAvailableDp UIAppResourceManager::DetermineAvailableDp(const IContentManager& contentManager,
                                                                                      const IO::Path& patternPath)
  {
    EncodedAvailableDp availableDp;
    static_assert(LocalConfig::ValidDpis.size() <= std::numeric_limits<uint32_t>::max(), "expectation failed");
    for (uint32_t i = 0; i < LocalConfig::ValidDpis.size(); ++i)
    {
      IO::Path filename(fmt::format(patternPath.AsUTF8String().AsString(), LocalConfig::ValidDpis[i]));
      if (contentManager.Exists(filename))
      {
        availableDp.AddIndex(i);
      }
    }
    return availableDp;
  }

  uint32_t UIAppResourceManager::DetermineResourceDp(const AnalyzedPath& analyzedPath, const uint32_t densityDpi)
  {
    if (analyzedPath.AvailableDp.Empty())
    {
      return SpriteDpConfig::BaseDpi;
    }

    float bestScore = std::numeric_limits<float>::max();
    uint32_t bestDp = 0u;
    for (uint32_t i = 0; i < LocalConfig::ValidDpis.size(); ++i)
    {
      float score = analyzedPath.AvailableDp.IsFlagged(i) ? CalcScore(LocalConfig::ValidDpis[i], densityDpi) : std::numeric_limits<float>::max();
      if (score < bestScore)
      {
        bestDp = LocalConfig::ValidDpis[i];
        bestScore = score;
      }
    }
    assert(bestDp != 0u);
    return bestDp;
  }


  IO::Path UIAppResourceManager::BuildResourceName(const AnalyzedPath& analyzedPath, const uint32_t selectedDp)
  {
    return !analyzedPath.AvailableDp.Empty() ? IO::Path(fmt::format(analyzedPath.SrcPath.AsUTF8String().AsString(), selectedDp))
                                             : analyzedPath.SrcPath;
  }

  //! Run through all the available dp's for the parent texture and verify there is a sprite font for all of them
  //! This is done to help detect missing font files which would only show itself at runtime and only if that specific dp was tested.
  void UIAppResourceManager::ValidateSpriteFontExistence(const IContentManager& contentManager, const UIAppTextureHandle handle,
                                                         const IO::PathView& fontName) const
  {
    const TextureDefinition& textureInfo = m_textures.Get(handle.Value);

    for (uint32_t i = 0; i < LocalConfig::ValidDpis.size(); ++i)
    {
      if (textureInfo.PathInfo.AvailableDp.IsFlagged(i))
      {
        IO::Path resourceFontName(GetFontName(textureInfo, LocalConfig::ValidDpis[i], fontName));
        if (!contentManager.Exists(resourceFontName))
        {
          throw NotFoundException(fmt::format("Missing font file '{}'", resourceFontName));
        }
      }
    }
  }

  BitmapFont UIAppResourceManager::ReadSpriteFont(const IContentManager& contentManager, const UIAppTextureHandle handle,
                                                  const IO::PathView& fontName, const bool isLegacyFullPathFontName) const
  {
    // fontName can not be empty
    assert(!fontName.empty());
    // fontName can not contain '/'
    assert(isLegacyFullPathFontName || fontName.find('/') == IO::PathView::npos);

    // Build the font name
    const TextureDefinition& textureInfo = m_textures.Get(handle.Value);

    if (isLegacyFullPathFontName && !UIAppResourceFlagUtil::IsFlagged(textureInfo.Flags, UIAppResourceFlag::NotDpAware))
    {
      throw UsageErrorException("texture was not marked as a legacy texture, so can not create a legacy sprite font for it");
    }

    IO::Path resourceFontName(!isLegacyFullPathFontName ? GetFontName(textureInfo, textureInfo.Dpi, fontName) : fontName);
    FSLLOG3_VERBOSE2("Loading font: '{}'", resourceFontName);

    const bool isLegacyFontFormat = (isLegacyFullPathFontName && fontName.ends_with(LocalConfig::OldFontExtension));

    // Read the bitmap font file
    auto bitmapFont = !isLegacyFontFormat ? contentManager.ReadBitmapFont(resourceFontName)
                                          : LoadFromLegacyFontFormat(contentManager, resourceFontName, textureInfo.Atlas.get());

    FSLLOG3_WARNING_IF(!isLegacyFullPathFontName && bitmapFont.GetDpi() != textureInfo.Dpi,
                       "BitmapFont is not of the expected DPI of {} instead it is {}", textureInfo.Dpi, bitmapFont.GetDpi());

    return bitmapFont;
  }

  IO::Path UIAppResourceManager::GetFontName(const TextureDefinition& textureInfo, const uint32_t dpi, const IO::PathView& fontName)
  {
    auto resourceName = BuildResourceName(textureInfo.PathInfo, dpi);
    auto directoryView = IO::Path::GetDirectoryNameView(resourceName.AsPathView());
    auto filenameWithoutExtView = IO::Path::GetFileNameWithoutExtensionView(resourceName.AsPathView());
    return IO::Path::Combine(directoryView, IO::Path(fmt::format("{}_{}", filenameWithoutExtView, fontName)));
  }

  void UIAppResourceManager::AttachTestPattern(const UIAppTextureHandle hTexture, Texture texture, const UIAppResourceFlag flags)
  {
    const bool allowTestPatternSwitching = UITestPatternModeUtil::IsFlagged(m_testPatternMode, UITestPatternMode::AllowSwitching);
    const bool isUIResource = UIAppResourceFlagUtil::IsFlagged(flags, UIAppResourceFlag::UIGroup);
    if (allowTestPatternSwitching && isUIResource)
    {
      FSLLOG3_VERBOSE3("Building test pattern for texture {}", hTexture.Value);
      const TextureDefinition& textureRecord = m_textures.Get(hTexture.Value);
      const PixelFormat pixelFormat = texture.GetPixelFormat();
      const auto origin = texture.GetBitmapOrigin();
      const auto extentPx = texture.GetExtent2D();
      Texture testTexture = textureRecord.Atlas
                              ? TestAtlasTextureGenerator::CreateTestPatternTexture(extentPx, pixelFormat, origin, *textureRecord.Atlas)
                              : TestAtlasTextureGenerator::CreateTestPatternTexture(extentPx, pixelFormat, origin);
      m_testPatternTextures[hTexture] = TestPatternRecord{std::move(texture), testTexture};
    }
  }

  void UIAppResourceManager::ApplyTestPattern()
  {
    if (!m_testPatternTextures.empty())
    {
      FSLLOG3_VERBOSE3("Applying test patterns");
      for (const auto& entry : m_testPatternTextures)
      {
        const TextureDefinition& textureRecord = m_textures.Get(entry.first.Value);
        auto* pTexture = dynamic_cast<IDynamicNativeTexture2D*>(textureRecord.Texture.get());
        if (pTexture != nullptr)
        {
          FSLLOG3_VERBOSE3("Set texture {} test pattern {}", entry.first.Value, m_testPatternEnabled);
          const auto& srcTexture = m_testPatternEnabled ? entry.second.TestPattern : entry.second.Original;
          RawTexture rawTexture;
          Texture::ScopedDirectAccess directAccess(srcTexture, rawTexture);
          pTexture->SetData(rawTexture, textureRecord.CreationInfo.Texture.FilterHint, textureRecord.CreationInfo.Texture.Flags);
        }
      }
    }
  }

  AtlasTextureInfo UIAppResourceManager::GetAtlasTextureInfo(const TextureDefinition& textureDefinition, const IO::PathView& atlasPathName)
  {
    if (!textureDefinition.Atlas)
    {
      throw UsageErrorException("No atlas associated");
    }
    return textureDefinition.Atlas->GetAtlasTextureInfo(atlasPathName);
  }
}
