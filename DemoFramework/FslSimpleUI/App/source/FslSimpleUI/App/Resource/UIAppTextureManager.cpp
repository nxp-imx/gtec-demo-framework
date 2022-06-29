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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/IO/FmtPathView.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxExtent2D.hpp>
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslBase/String/StringToValue.hpp>
#include <FslDemoApp/Base/Service/Content/IContentManager.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <FslGraphics/Font/BitmapFontConverter.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Render/Adapter/IDynamicNativeTexture2D.hpp>
#include <FslGraphics/Render/Basic/IBasicRenderSystem.hpp>
#include <FslGraphics/Sprite/SpriteDpConfig.hpp>
#include <FslGraphics/TextureAtlas/CompatibilityTextureAtlasMap.hpp>
#include <FslGraphics/TextureAtlas/TestAtlasTextureGenerator.hpp>
#include <FslSimpleUI/App/Resource/AnalyzedPath.hpp>
#include <FslSimpleUI/App/Resource/UIAppTextureManager.hpp>
#include <FslSimpleUI/App/UIAppTextureResourceCreationInfo.hpp>
#include <array>


namespace Fsl::SimpleUIApp
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr StringViewLite AtlasExtension(".bta");
      constexpr StringViewLite FilenameDpiPostfix("dpi");
      // constexpr StringViewLite NewFontExtension(".nbf");
      constexpr StringViewLite OldFontExtension(".fbk");

      constexpr char FilenameDpStartChar = '_';

      constexpr float PreferenceWeight = 0.97f;
      constexpr float UpscalePrefScale = 1.5f;

      // This is expected to be a sorted array (low to high)
      // Density:                                   0.5,0.6, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0
      constexpr std::array<uint32_t, 9> ValidDpis = {80, 96, 160, 240, 320, 400, 480, 560, 640};
    }

    static_assert(LocalConfig::ValidDpis.size() <= 32u, "the EncodedAvailableDp encodes the indices in bits");

    struct PrepareTextureResult
    {
      Texture SrcTexture;
      std::unique_ptr<CompatibilityTextureAtlasMap> Atlas;
    };

    struct PrepareCreateResult
    {
      UIAppTextureResourceCreationInfo CreationInfo;
      AnalyzedPath PathInfo;
      uint32_t Dpi{};
      PrepareTextureResult TextureResult;
    };

    IO::Path DoBuildResourceName(const AnalyzedPath& analyzedPath, const uint32_t selectedDpi)
    {
      return !analyzedPath.AvailableDpi.Empty() ? IO::Path(fmt::format(analyzedPath.SrcPath.AsUTF8String().AsString(), selectedDpi))
                                                : analyzedPath.SrcPath;
    }


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

    EncodedAvailableDpi DetermineAvailableDp(const IContentManager& contentManager, const IO::Path& patternPath)
    {
      EncodedAvailableDpi availableDp;
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


    //! Analyze the path to see if the filename without extension ends in "_<number>dp"
    AnalyzedPath AnalyzePath(const IContentManager& contentManager, const IO::PathView pathView, const bool allowDpAware)
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
                auto patternPath = IO::Path::Combine(
                  pathDirectoryView, IO::Path(fmt::format("{}{{}}{}{}", parsedFilename, LocalConfig::FilenameDpiPostfix, pathExtView)));
                EncodedAvailableDpi availableDp(DetermineAvailableDp(contentManager, patternPath));
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
      return {pathView, EncodedAvailableDpi()};
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
      float tmp = (static_cast<float>(delta) / static_cast<float>(SpriteDpConfig::BaseDpi)) + 100.0f + LocalConfig::PreferenceWeight;
      assert(tmp >= 0.0f);
      tmp = 100.0f - std::floor(tmp);
      return tmp >= 0.0f ? tmp : (-tmp) * LocalConfig::UpscalePrefScale;
    }


    uint32_t DetermineResourceDpi(const AnalyzedPath& analyzedPath, const uint32_t densityDpi)
    {
      if (analyzedPath.AvailableDpi.Empty())
      {
        return SpriteDpConfig::BaseDpi;
      }

      float bestScore = std::numeric_limits<float>::max();
      uint32_t bestDpi = 0u;
      for (uint32_t i = 0; i < LocalConfig::ValidDpis.size(); ++i)
      {
        float score = analyzedPath.AvailableDpi.IsFlagged(i) ? CalcScore(LocalConfig::ValidDpis[i], densityDpi) : std::numeric_limits<float>::max();
        if (score < bestScore)
        {
          bestDpi = LocalConfig::ValidDpis[i];
          bestScore = score;
        }
      }
      assert(bestDpi != 0u);
      return bestDpi;
    }


    PrepareTextureResult PrepareTexture(const IContentManager& contentManager, const AnalyzedPath& pathInfo, const bool isAtlas,
                                        const uint32_t selectedDp, const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                        const UITestPatternMode testPatternMode)
    {
      IO::Path resourceName(DoBuildResourceName(pathInfo, selectedDp));

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

    PrepareCreateResult PrepareCreateTexture(const std::map<IO::Path, UIAppTextureHandle>& textureLookup, IContentManager& rContentManager,
                                             const IO::PathView& atlasPath, const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                             const UIAppResourceFlag flags, const uint32_t densityDpi, const UITestPatternMode testPatternMode)
    {
      const bool allowDpAware = !UIAppResourceFlagUtil::IsFlagged(flags, UIAppResourceFlag::NotDpAware);

      // Determine if this is a DP aware resource
      auto pathInfo = AnalyzePath(rContentManager, atlasPath, allowDpAware);
      auto selectedDp = DetermineResourceDpi(pathInfo, densityDpi);

      if (textureLookup.find(pathInfo.SrcPath) != textureLookup.end())
      {
        throw std::invalid_argument(fmt::format("The texture '{}' resolves to '{}' which has already been created.", atlasPath, pathInfo.SrcPath));
      }

      const bool isAtlas = UIAppResourceFlagUtil::IsFlagged(flags, UIAppResourceFlag::Atlas);
      return {textureCreationInfo, pathInfo, selectedDp,
              PrepareTexture(rContentManager, pathInfo, isAtlas, selectedDp, textureCreationInfo, testPatternMode)};
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

    IO::Path GetFontName(const AnalyzedPath& pathInfo, const uint32_t dpi, const IO::PathView& fontName)
    {
      auto resourceName = DoBuildResourceName(pathInfo, dpi);
      auto directoryView = IO::Path::GetDirectoryNameView(resourceName.AsPathView());
      auto filenameWithoutExtView = IO::Path::GetFileNameWithoutExtensionView(resourceName.AsPathView());
      return IO::Path::Combine(directoryView, IO::Path(fmt::format("{}_{}", filenameWithoutExtView, fontName)));
    }


    //! Run through all the available dp's for the parent texture and verify there is a sprite font for all of them
    //! This is done to help detect missing font files which would only show itself at runtime and only if that specific dp was tested.
    void ValidateSpriteFontExistence(const IContentManager& contentManager, const AnalyzedPath& texturePathInfo, const IO::PathView& fontName)
    {
      for (uint32_t i = 0; i < LocalConfig::ValidDpis.size(); ++i)
      {
        if (texturePathInfo.AvailableDpi.IsFlagged(i))
        {
          IO::Path resourceFontName(GetFontName(texturePathInfo, LocalConfig::ValidDpis[i], fontName));
          if (!contentManager.Exists(resourceFontName))
          {
            throw NotFoundException(fmt::format("Missing font file '{}'", resourceFontName));
          }
        }
      }
    }

    bool CheckSpriteFontExistence(const IContentManager& contentManager, const AnalyzedPath& texturePathInfo, const IO::PathView& fontName)
    {
      for (uint32_t i = 0; i < LocalConfig::ValidDpis.size(); ++i)
      {
        if (texturePathInfo.AvailableDpi.IsFlagged(i))
        {
          IO::Path resourceFontName(GetFontName(texturePathInfo, LocalConfig::ValidDpis[i], fontName));
          if (!contentManager.Exists(resourceFontName))
          {
            return false;
          }
        }
      }
      return true;
    }
  }


  UIAppTextureManager::UIAppTextureManager(const uint32_t densityDpi, const UITestPatternMode testPatternMode)
    : m_densityDpi(densityDpi)
    , m_options(UITestPatternModeUtil::IsFlagged(testPatternMode, UITestPatternMode::Enabled), testPatternMode)
  {
  }

  UIAppTextureManager::~UIAppTextureManager() = default;


  void UIAppTextureManager::Clear()
  {
    m_textureLookup.clear();
    m_textures.Clear();
    m_externalTextures.Clear();
    m_managedTextures.Clear();
  }


  bool UIAppTextureManager::SetTestPattern(const bool enabled)
  {
    if (!UITestPatternModeUtil::IsFlagged(m_options.TestPatternMode, UITestPatternMode::AllowSwitching))
    {
      FSLLOG3_WARNING(
        "SetTestPattern: UIAppTextureManager was not created with UITestPatternMode::DisabledAllowSwitching or "
        "UITestPatternMode::EnableAllowSwitching enabled, so request ignored");
      return false;
    }
    if (enabled == m_options.TestPatternEnabled)
    {
      return false;
    }

    m_options.TestPatternEnabled = enabled;
    ApplyTestPattern();
    return true;
  }

  UIAppTexture<INativeTexture2D> UIAppTextureManager::CreateTexture(IContentManager& rContentManager, IBasicRenderSystem& rRenderSystem,
                                                                    const IO::PathView& atlasPath,
                                                                    const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                                                    const UIAppResourceFlag flags)
  {
    // If this is a UI resource and pattern mode is set to allow switching we force this texture to be dynamic
    if (UIAppResourceFlagUtil::IsFlagged(flags, UIAppResourceFlag::UIGroup) &&
        UITestPatternModeUtil::IsFlagged(m_options.TestPatternMode, UITestPatternMode::AllowSwitching))
    {
      auto res = CreateDynamicTexture(rContentManager, rRenderSystem, atlasPath, textureCreationInfo, flags);
      return {res.Handle, res.Texture, res.ExtentPx, res.TexturePixelFormat};
    }

    PrepareCreateResult prepareCreateResult =
      PrepareCreateTexture(m_textureLookup, rContentManager, atlasPath, textureCreationInfo, flags, GetDensityDpi(), m_options.TestPatternMode);
    assert(prepareCreateResult.TextureResult.SrcTexture.GetExtent().Depth == 1u);

    const PxExtent2D extent = prepareCreateResult.TextureResult.SrcTexture.GetExtent2D();
    const PixelFormat pixelFormat = prepareCreateResult.TextureResult.SrcTexture.GetPixelFormat();
    std::shared_ptr<INativeTexture2D> texture;
    {
      RawTexture srcRawTexture;
      Texture::ScopedDirectAccess directAccess(prepareCreateResult.TextureResult.SrcTexture, srcRawTexture);
      texture = rRenderSystem.CreateTexture2D(srcRawTexture, textureCreationInfo.Texture.FilterHint, textureCreationInfo.Texture.Flags);
    }

    auto handle = DoAddTexture(prepareCreateResult.CreationInfo, prepareCreateResult.PathInfo, prepareCreateResult.Dpi,
                               prepareCreateResult.TextureResult.SrcTexture, std::move(prepareCreateResult.TextureResult.Atlas), texture, flags);
    return {handle, texture, extent, pixelFormat};
  }

  UIAppTexture<IDynamicNativeTexture2D> UIAppTextureManager::CreateDynamicTexture(IContentManager& rContentManager, IBasicRenderSystem& rRenderSystem,
                                                                                  const IO::PathView& atlasPath,
                                                                                  const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                                                                  const UIAppResourceFlag flags)
  {
    // Ensure the dynamic flag is set
    const auto modifiedFlags = flags | UIAppResourceFlag::Dynamic;

    PrepareCreateResult prepareCreateResult =
      PrepareCreateTexture(m_textureLookup, rContentManager, atlasPath, textureCreationInfo, flags, GetDensityDpi(), m_options.TestPatternMode);
    assert(prepareCreateResult.TextureResult.SrcTexture.GetExtent().Depth == 1u);

    const PxExtent2D extent = prepareCreateResult.TextureResult.SrcTexture.GetExtent2D();
    const PixelFormat pixelFormat = prepareCreateResult.TextureResult.SrcTexture.GetPixelFormat();
    std::shared_ptr<IDynamicNativeTexture2D> texture;
    {
      RawTexture srcRawTexture;
      Texture::ScopedDirectAccess directAccess(prepareCreateResult.TextureResult.SrcTexture, srcRawTexture);
      texture = rRenderSystem.CreateDynamicTexture2D(srcRawTexture, textureCreationInfo.Texture.FilterHint, textureCreationInfo.Texture.Flags);
    }
    auto handle =
      DoAddTexture(prepareCreateResult.CreationInfo, prepareCreateResult.PathInfo, prepareCreateResult.Dpi,
                   prepareCreateResult.TextureResult.SrcTexture, std::move(prepareCreateResult.TextureResult.Atlas), texture, modifiedFlags);
    return {handle, texture, extent, pixelFormat};
  }


  UIAppTextureHandle UIAppTextureManager::RegisterExternalTexture(IBasicRenderSystem& rRenderSystem, const std::shared_ptr<INativeTexture2D>& texture)
  {
    if (!texture)
    {
      throw std::invalid_argument("texture can not be null");
    }
    // Disabled because for now we track this at the global sprite level instead
    // For now we just do this every-time (this will not scale well, but should be good enough for our small demos)
    // PerformGarbageCollection();

    auto actualHandle = HandleVectorConfig::InvalidHandle;
    auto handle = HandleVectorConfig::InvalidHandle;
    try
    {
      const PxExtent2D extentPx = rRenderSystem.GetTextureExtentPx(texture);

      actualHandle = m_externalTextures.Add(ExternalTextureDefinition(extentPx, texture, UIAppTextureHandle(HandleVectorConfig::InvalidHandle)));

      handle = m_textures.Add(TextureRecord(TextureType::External, actualHandle));

      // Patch the external texture record with the actual handle so we can use it for GC.
      m_externalTextures.FastGet(actualHandle).TextureHandle = UIAppTextureHandle(handle);

      return UIAppTextureHandle(handle);
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_ERROR("Failed to add due to a exception: {}", ex.what());
      if (handle != HandleVectorConfig::InvalidHandle)
      {
        m_textures.Remove(handle);
      }
      if (actualHandle != HandleVectorConfig::InvalidHandle)
      {
        m_managedTextures.Remove(actualHandle);
      }
      throw;
    }
  }


  bool UIAppTextureManager::UnregisterTexture(const UIAppTextureHandle hTexture)
  {
    const TextureRecord* pTextureRecord = m_textures.TryGet(hTexture.Value);
    if (pTextureRecord == nullptr)
    {
      return false;
    }
    switch (pTextureRecord->Type)
    {
    case TextureType::Managed:
      m_managedTextures.Remove(pTextureRecord->ActualHandle);
      return m_textures.Remove(hTexture.Value);
    case TextureType::External:
      m_externalTextures.Remove(pTextureRecord->ActualHandle);
      return m_textures.Remove(hTexture.Value);
    default:
      throw InternalErrorException("Unhandled texture type");
    }
  }


  IO::Path UIAppTextureManager::BuildResourceName(const AnalyzedPath& analyzedPath, const uint32_t selectedDpi) const
  {
    return DoBuildResourceName(analyzedPath, selectedDpi);
  }


  const CompatibilityTextureAtlasMap& UIAppTextureManager::GetAtlas(const UIAppTextureHandle hTexture) const
  {
    const auto& textureDefinition = GetManaged(hTexture);
    if (!textureDefinition.Atlas)
    {
      throw UsageErrorException("No atlas associated");
    }
    return *textureDefinition.Atlas;
  }


  AtlasTextureInfo UIAppTextureManager::GetAtlasTextureInfo(const UIAppTextureHandle hTexture, const IO::PathView& atlasPathName) const
  {
    return GetAtlas(hTexture).GetAtlasTextureInfo(atlasPathName);
  }


  bool UIAppTextureManager::SpriteFontExists(const IContentManager& contentManager, const UIAppTextureHandle textureHandle,
                                             IO::PathView fontName) const
  {
    const auto* pTextureInfo = TryGetManaged(textureHandle);
    if (pTextureInfo == nullptr)
    {
      return false;
    }
    return CheckSpriteFontExistence(contentManager, pTextureInfo->PathInfo, fontName);
  }


  BitmapFont UIAppTextureManager::ReadSpriteFont(const IContentManager& contentManager, const UIAppTextureHandle handle, const IO::PathView& fontName,
                                                 const bool isLegacyFullPathFontName) const
  {
    // Build the font name
    const TextureDefinition& textureInfo = GetManaged(handle);

    if (!isLegacyFullPathFontName)
    {
      ValidateSpriteFontExistence(contentManager, textureInfo.PathInfo, fontName);
    }

    // fontName can not be empty
    assert(!fontName.empty());
    // fontName can not contain '/'
    assert(isLegacyFullPathFontName || fontName.find('/') == IO::PathView::npos);


    if (isLegacyFullPathFontName && !UIAppResourceFlagUtil::IsFlagged(textureInfo.Flags, UIAppResourceFlag::NotDpAware))
    {
      throw UsageErrorException("texture was not marked as a legacy texture, so can not create a legacy sprite font for it");
    }

    IO::Path resourceFontName(!isLegacyFullPathFontName ? GetFontName(textureInfo.PathInfo, textureInfo.Dpi, fontName) : fontName);
    FSLLOG3_VERBOSE2("Loading font: '{}'", resourceFontName);

    const bool isLegacyFontFormat = (isLegacyFullPathFontName && fontName.ends_with(LocalConfig::OldFontExtension));

    // Read the bitmap font file
    auto bitmapFont = !isLegacyFontFormat ? contentManager.ReadBitmapFont(resourceFontName)
                                          : LoadFromLegacyFontFormat(contentManager, resourceFontName, textureInfo.Atlas.get());

    FSLLOG3_WARNING_IF(!isLegacyFullPathFontName && bitmapFont.GetDpi() != textureInfo.Dpi,
                       "BitmapFont is not of the expected DPI of {} instead it is {}", textureInfo.Dpi, bitmapFont.GetDpi());

    return bitmapFont;
  }


  bool UIAppTextureManager::TryReleaseAtIfDifferentDpi(const index_type textureIndex, const uint32_t densityDpi)
  {
    assert(textureIndex <= m_textures.Count());
    const auto& record = m_textures[textureIndex];

    if (record.Type != TextureType::Managed)
    {
      // not a managed texture
      return false;
    }

    auto& rEntry = m_managedTextures.FastGet(record.ActualHandle);
    if (rEntry.PathInfo.AvailableDpi.Empty())
    {
      // No alternative dpi's
      return false;
    }

    const uint32_t newDpi = DetermineResourceDpi(rEntry.PathInfo, densityDpi);
    if (newDpi == rEntry.Dpi)
    {
      // No changes
      return false;
    }

    FSLLOG3_VERBOSE("Freeing dp aware material '{}'", DoBuildResourceName(rEntry.PathInfo, rEntry.Dpi))

    rEntry.Dpi = newDpi;
    rEntry.Info.Texture.reset();
    rEntry.Atlas.reset();
    return true;
  }


  UIAppTextureHandle UIAppTextureManager::ReloadTextureAt(const index_type textureIndex, IContentManager& rContentManager,
                                                          IBasicRenderSystem& rRenderSystem)
  {
    const auto& record = m_textures[textureIndex];
    const auto hTexture = m_textures.FastIndexToHandle(textureIndex);
    if (record.Type == TextureType::Managed)
    {
      auto& rEntry = m_managedTextures.FastGet(record.ActualHandle);
      const bool isAtlas = UIAppResourceFlagUtil::IsFlagged(rEntry.Flags, UIAppResourceFlag::Atlas);
      PrepareTextureResult result =
        PrepareTexture(rContentManager, rEntry.PathInfo, isAtlas, rEntry.Dpi, rEntry.CreationInfo, m_options.TestPatternMode);
      assert(result.SrcTexture.GetExtent().Depth == 1u);
      {
        RawTexture srcRawTexture;
        Texture::ScopedDirectAccess directAccess(result.SrcTexture, srcRawTexture);
        rEntry.Info.Texture =
          (!UIAppResourceFlagUtil::IsFlagged(rEntry.Flags, UIAppResourceFlag::Dynamic)
             ? rRenderSystem.CreateTexture2D(srcRawTexture, rEntry.CreationInfo.Texture.FilterHint, rEntry.CreationInfo.Texture.Flags)
             : rRenderSystem.CreateDynamicTexture2D(srcRawTexture, rEntry.CreationInfo.Texture.FilterHint, rEntry.CreationInfo.Texture.Flags));
        rEntry.Info.ExtentPx = result.SrcTexture.GetExtent2D();
      }
      rEntry.Atlas = std::move(result.Atlas);

      // Cache the current texture and test pattern (if enabled)
      AttachTestPattern(UIAppTextureHandle(hTexture), std::move(result.SrcTexture), rEntry.Flags);

      // Apply test patterns if enabled
      if (m_options.TestPatternEnabled)
      {
        ApplyTestPattern();
      }
    }
    return UIAppTextureHandle(hTexture);
  }


  void UIAppTextureManager::PerformGarbageCollection()
  {
    FSLLOG3_VERBOSE5("UIAppTextureManager::PerformGarbageCollection");

    const uint32_t count = m_externalTextures.Count();
    for (int64_t i = count - 1; i >= 0; --i)
    {
      const auto index = UncheckedNumericCast<uint32_t>(i);
      const auto& entry = m_externalTextures[index];
      if (entry.Texture.expired())
      {
        FSLLOG3_VERBOSE5("- Removing expired texture: {}", entry.TextureHandle.Value);

        // Remove the texture entry
        m_textures.Remove(entry.TextureHandle.Value);

        // Finally we remove the external entry (after this the 'entry' is invalid so it must be done last)
        m_externalTextures.RemoveAt(index);
      }
    }
  }


  const UIAppTextureManager::TextureDefinition& UIAppTextureManager::GetManaged(const UIAppTextureHandle hTexture) const
  {
    const auto& record = m_textures.Get(hTexture.Value);
    if (record.Type != TextureType::Managed)
    {
      throw std::invalid_argument("The texture is not managed");
    }
    return m_managedTextures.Get(record.ActualHandle);
  }


  const UIAppTextureManager::TextureDefinition* UIAppTextureManager::TryGetManaged(const UIAppTextureHandle hTexture) const
  {
    const auto& record = m_textures.Get(hTexture.Value);
    if (record.Type != TextureType::Managed)
    {
      return nullptr;
    }
    return &m_managedTextures.Get(record.ActualHandle);
  }


  UIAppTextureHandle UIAppTextureManager::DoAddTexture(const UIAppTextureResourceCreationInfo& creationInfo, const AnalyzedPath& pathInfo,
                                                       const uint32_t dpi, const Texture& srcTexture,
                                                       std::unique_ptr<CompatibilityTextureAtlasMap> srcAtlas,
                                                       const std::shared_ptr<INativeTexture2D>& nativeTexture, const UIAppResourceFlag flags)
  {
    const auto& key = pathInfo.SrcPath;
    auto actualHandle = HandleVectorConfig::InvalidHandle;
    auto handle = HandleVectorConfig::InvalidHandle;
    try
    {
      const PxExtent2D extentPx = srcTexture.GetExtent2D();

      actualHandle =
        m_managedTextures.Add(TextureDefinition{creationInfo, pathInfo, dpi, UIAppTextureInfo(extentPx, nativeTexture), flags, std::move(srcAtlas)});

      handle = m_textures.Add(TextureRecord(TextureType::Managed, actualHandle));
      auto hTexture = UIAppTextureHandle(handle);
      m_textureLookup[key] = hTexture;

      AttachTestPattern(hTexture, srcTexture, flags);
      return hTexture;
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_ERROR("Failed to add due to a exception: {}", ex.what());
      if (handle != HandleVectorConfig::InvalidHandle)
      {
        m_testPatternTextures.erase(UIAppTextureHandle(handle));
        m_textureLookup.erase(key);
        m_textures.Remove(handle);
      }
      if (actualHandle != HandleVectorConfig::InvalidHandle)
      {
        m_managedTextures.Remove(actualHandle);
      }
      throw;
    }
  }

  void UIAppTextureManager::AttachTestPattern(const UIAppTextureHandle hTexture, Texture texture, const UIAppResourceFlag flags)
  {
    const bool allowTestPatternSwitching = UITestPatternModeUtil::IsFlagged(m_options.TestPatternMode, UITestPatternMode::AllowSwitching);
    const bool isUIResource = UIAppResourceFlagUtil::IsFlagged(flags, UIAppResourceFlag::UIGroup);
    if (allowTestPatternSwitching && isUIResource)
    {
      FSLLOG3_VERBOSE3("Building test pattern for texture {}", hTexture.Value);
      const TextureRecord& textureRecord = m_textures.Get(hTexture.Value);
      if (textureRecord.Type == TextureType::Managed)
      {
        const TextureDefinition& textureDefinition = m_managedTextures.Get(textureRecord.ActualHandle);
        const PixelFormat pixelFormat = texture.GetPixelFormat();
        const auto origin = texture.GetBitmapOrigin();
        const auto extentPx = texture.GetExtent2D();
        Texture testTexture = textureDefinition.Atlas
                                ? TestAtlasTextureGenerator::CreateTestPatternTexture(extentPx, pixelFormat, origin, *textureDefinition.Atlas)
                                : TestAtlasTextureGenerator::CreateTestPatternTexture(extentPx, pixelFormat, origin);
        m_testPatternTextures[hTexture] = TestPatternRecord{std::move(texture), testTexture};
      }
    }
  }


  void UIAppTextureManager::ApplyTestPattern()
  {
    if (!m_testPatternTextures.empty())
    {
      FSLLOG3_VERBOSE3("Applying test patterns");
      for (const auto& entry : m_testPatternTextures)
      {
        const TextureDefinition& textureDef = GetManaged(entry.first);
        auto* pTexture = dynamic_cast<IDynamicNativeTexture2D*>(textureDef.Info.Texture.get());
        if (pTexture != nullptr)
        {
          FSLLOG3_VERBOSE3("Set texture {} test pattern {}", entry.first.Value, m_options.TestPatternEnabled);
          const auto& srcTexture = m_options.TestPatternEnabled ? entry.second.TestPattern : entry.second.Original;
          RawTexture rawTexture;
          Texture::ScopedDirectAccess directAccess(srcTexture, rawTexture);
          pTexture->SetData(rawTexture, textureDef.CreationInfo.Texture.FilterHint, textureDef.CreationInfo.Texture.Flags);
        }
      }
    }
  }

}
