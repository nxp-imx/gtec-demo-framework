#ifndef FSLSIMPLEUI_APP_RESOURCE_UIAPPTEXTUREMANAGER_HPP
#define FSLSIMPLEUI_APP_RESOURCE_UIAPPTEXTUREMANAGER_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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
#include <FslBase/IO/Path.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslGraphics/Font/BitmapFont.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphics/TextureAtlas/AtlasTextureInfo.hpp>
#include <FslSimpleUI/App/Resource/AnalyzedPath.hpp>
#include <FslSimpleUI/App/Resource/UIAppBasicTextureInfo.hpp>
#include <FslSimpleUI/App/Resource/UIAppTextureInfo.hpp>
#include <FslSimpleUI/App/UIAppResourceFlag.hpp>
#include <FslSimpleUI/App/UIAppTexture.hpp>
#include <FslSimpleUI/App/UIAppTextureHandle.hpp>
#include <FslSimpleUI/App/UIAppTextureResourceCreationInfo.hpp>
#include <FslSimpleUI/App/UITestPatternMode.hpp>
#include <cassert>
#include <map>
#include <memory>

namespace Fsl
{
  class CompatibilityTextureAtlasMap;
  class IBasicRenderSystem;
  class IContentManager;
  class IDynamicNativeTexture2D;
  class INativeTexture2D;

  namespace SimpleUIApp
  {
    class UIAppTextureManager
    {
      enum class TextureType
      {
        Managed,
        External
      };

      struct OptionsRecord
      {
        bool TestPatternEnabled{false};
        UITestPatternMode TestPatternMode{};

        OptionsRecord(const bool testPatternEnabled, const UITestPatternMode testPatternMode)
          : TestPatternEnabled(testPatternEnabled)
          , TestPatternMode(testPatternMode)
        {
        }
      };

      struct TestPatternRecord
      {
        Texture Original;
        Texture TestPattern;
      };

      struct TextureRecord
      {
        TextureType Type{TextureType::Managed};
        int32_t ActualHandle{HandleVectorConfig::InvalidHandle};

        constexpr TextureRecord() noexcept = default;
        constexpr TextureRecord(const TextureType type, const int32_t actualHandle) noexcept
          : Type(type)
          , ActualHandle(actualHandle)
        {
        }
      };

      struct TextureDefinition
      {
        UIAppTextureResourceCreationInfo CreationInfo;
        AnalyzedPath PathInfo;
        uint32_t Dpi{0};
        UIAppTextureInfo Info;
        UIAppResourceFlag Flags;
        std::unique_ptr<CompatibilityTextureAtlasMap> Atlas;
      };

      struct ExternalTextureDefinition
      {
        PxExtent2D ExtentPx;
        std::weak_ptr<INativeTexture2D> Texture;
        UIAppTextureHandle TextureHandle;

        ExternalTextureDefinition() = default;
        ExternalTextureDefinition(const PxExtent2D extentPx, std::weak_ptr<INativeTexture2D> texture, const UIAppTextureHandle textureHandle)
          : ExtentPx(extentPx)
          , Texture(std::move(texture))
          , TextureHandle(textureHandle)
        {
        }
      };


      HandleVector<TextureRecord> m_textures;
      HandleVector<TextureDefinition> m_managedTextures;
      HandleVector<ExternalTextureDefinition> m_externalTextures;

      std::map<IO::Path, UIAppTextureHandle> m_textureLookup;
      std::map<UIAppTextureHandle, TestPatternRecord> m_testPatternTextures;
      uint32_t m_densityDpi;
      OptionsRecord m_options;

    public:
      using index_type = HandleVector<TextureDefinition>::index_type;
      using handle_type = HandleVector<TextureDefinition>::handle_type;
      using element_const_reference = HandleVector<TextureDefinition>::const_reference;

      explicit UIAppTextureManager(const uint32_t densityDpi, const UITestPatternMode testPatternMode);
      ~UIAppTextureManager();

      void Clear();

      uint32_t GetDensityDpi() const
      {
        return m_densityDpi;
      }

      void SetDensityDpi(const uint32_t value)
      {
        m_densityDpi = value;
      }

      bool IsValidHandle(const UIAppTextureHandle hTexture) const
      {
        return m_textures.IsValidHandle(hTexture.Value);
      }

      UIAppTextureHandle FastIndexToHandle(const index_type index) const
      {
        return UIAppTextureHandle(m_textures.FastIndexToHandle(index));
      }

      UIAppTextureInfo GetTextureInfo(const UIAppTextureHandle hTexture) const
      {
        const auto& record = m_textures.Get(hTexture.Value);
        if (record.Type == TextureType::Managed)
        {
          return m_managedTextures.Get(record.ActualHandle).Info;
        }
        assert(record.Type == TextureType::External);
        const auto& customRecord = m_externalTextures.Get(record.ActualHandle);
        auto texture = customRecord.Texture.lock();
        if (!texture)
        {
          throw NotFoundException("texture has expired");
        }
        return {customRecord.ExtentPx, std::move(texture)};
      }


      uint32_t Count() const
      {
        return m_textures.Count();
      }


      UIAppBasicTextureInfo FastGetInfoAt(const index_type index) const
      {
        const auto& record = m_textures[index];
        assert(record.Type == TextureType::Managed || record.Type == TextureType::External);
        return {UIAppTextureHandle(m_textures.FastIndexToHandle(index)), record.Type == TextureType::Managed
                                                                           ? m_managedTextures.FastGet(record.ActualHandle).Info.ExtentPx
                                                                           : m_externalTextures.FastGet(record.ActualHandle).ExtentPx};
      }

      const UIAppTextureInfo* TryFastGetTextureInfoAt(const index_type index) const
      {
        const auto& record = m_textures[index];
        assert(record.Type == TextureType::Managed || record.Type == TextureType::External);
        return record.Type == TextureType::Managed ? &m_managedTextures.FastGet(record.ActualHandle).Info : nullptr;
      }

      bool SetTestPattern(const bool enabled);

      UIAppTexture<INativeTexture2D> CreateTexture(IContentManager& rContentManager, IBasicRenderSystem& rRenderSystem, const IO::PathView& atlasPath,
                                                   const UIAppTextureResourceCreationInfo& textureCreationInfo, const UIAppResourceFlag flags);
      UIAppTexture<IDynamicNativeTexture2D> CreateDynamicTexture(IContentManager& rContentManager, IBasicRenderSystem& rRenderSystem,
                                                                 const IO::PathView& atlasPath,
                                                                 const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                                                 const UIAppResourceFlag flags);

      //! @brief Register a external texture so we can get a UIAppTextureHandle for it.
      UIAppTextureHandle RegisterExternalTexture(IBasicRenderSystem& rRenderSystem, const std::shared_ptr<INativeTexture2D>& texture);
      bool UnregisterTexture(const UIAppTextureHandle hTexture);

      IO::Path BuildResourceName(const AnalyzedPath& analyzedPath, const uint32_t selectedDpi) const;

      const CompatibilityTextureAtlasMap& GetAtlas(const UIAppTextureHandle hTexture) const;

      AtlasTextureInfo GetAtlasTextureInfo(const UIAppTextureHandle hTexture, const IO::PathView& atlasPathName) const;


      bool SpriteFontExists(const IContentManager& contentManager, const UIAppTextureHandle textureHandle, IO::PathView fontName) const;

      BitmapFont ReadSpriteFont(const IContentManager& contentManager, const UIAppTextureHandle handle, const IO::PathView& fontName,
                                const bool isLegacyFullPathFontName) const;

      bool TryReleaseAtIfDifferentDpi(const index_type textureIndex, const uint32_t densityDpi);

      bool FastIsTextureMissing(const index_type textureIndex) const
      {
        const auto& entry = m_textures[textureIndex];
        return entry.Type == TextureType::Managed && IsManagedTextureMissing(entry.ActualHandle);
      }

      UIAppTextureHandle ReloadTextureAt(const index_type textureIndex, IContentManager& rContentManager, IBasicRenderSystem& rRenderSystem);

      void PerformGarbageCollection();

    private:
      const TextureDefinition& GetManaged(const UIAppTextureHandle hTexture) const;
      const TextureDefinition* TryGetManaged(const UIAppTextureHandle hTexture) const;

      UIAppTextureHandle DoAddTexture(const UIAppTextureResourceCreationInfo& creationInfo, const AnalyzedPath& pathInfo, const uint32_t dpi,
                                      const Texture& srcTexture, std::unique_ptr<CompatibilityTextureAtlasMap> srcAtlas,
                                      const std::shared_ptr<INativeTexture2D>& nativeTexture, const UIAppResourceFlag flags);

      void AttachTestPattern(const UIAppTextureHandle hTexture, Texture texture, const UIAppResourceFlag flags);
      void ApplyTestPattern();

      bool IsManagedTextureMissing(const int32_t actualHandle) const
      {
        const auto& entry = m_managedTextures.FastGet(actualHandle);
        return (!entry.PathInfo.AvailableDpi.Empty() && !entry.Info.Texture);
      }
    };
  }
}

#endif
