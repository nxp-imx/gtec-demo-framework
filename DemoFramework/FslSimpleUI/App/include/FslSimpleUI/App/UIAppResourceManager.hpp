#ifndef FSLSIMPLEUI_APP_UIAPPRESOURCEMANAGER_HPP
#define FSLSIMPLEUI_APP_UIAPPRESOURCEMANAGER_HPP
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

#include <FslBase/IO/Path.hpp>
#include <FslBase/Bits/BitsUtil.hpp>
#include <FslBase/Collections/HandleVector.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <FslGraphics/Render/BlendState.hpp>
#include <FslGraphics/Sprite/ISpriteResourceManager.hpp>
#include <FslGraphics/Sprite/SpriteManager.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialInfo.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslSimpleUI/App/UIAppResourceFlag.hpp>
#include <FslSimpleUI/App/UIAppTextureHandle.hpp>
#include <FslSimpleUI/App/UIAppTextureResourceCreationInfo.hpp>
#include <FslSimpleUI/App/UITestPatternMode.hpp>
#include <map>
#include <memory>

namespace Fsl
{
  class Bitmap;
  class BasicNineSliceSprite;
  struct DemoWindowMetrics;
  class IContentManager;
  class IDynamicNativeTexture2D;
  class IImageSprite;
  class INativeGraphics;
  class INativeTexture2D;
  class INineSliceSprite;
  class ISpriteMaterial;
  class SpriteFont;
  struct SpriteFontConfig;
  struct SpriteMaterialId;
  class CompatibilityTextureAtlasMap;

  class UIAppResourceManager final : public ISpriteResourceManager
  {
    enum class SpriteType
    {
      Basic = 0,
      Normal = 1,
    };

    struct EncodedAvailableDp
    {
      uint32_t EncodedValue{};

      constexpr void AddIndex(const uint32_t index)
      {
        EncodedValue |= 1 << index;
      }

      constexpr bool Empty() const
      {
        return EncodedValue == 0u;
      }

      constexpr uint32_t Count() const
      {
        return BitsUtil::Count(EncodedValue);
      }

      constexpr bool IsFlagged(const uint32_t bitIndex) const
      {
        return (EncodedValue & (1 << bitIndex)) != 0u;
      }
    };

    struct AnalyzedPath
    {
      IO::Path SrcPath;
      EncodedAvailableDp AvailableDp;
    };

    struct TextureDefinition
    {
      UIAppTextureResourceCreationInfo CreationInfo;
      AnalyzedPath PathInfo;
      uint32_t Dpi{0};
      std::shared_ptr<INativeTexture2D> Texture;
      PxExtent2D ExtentPx;
      UIAppResourceFlag Flags;
      std::unique_ptr<CompatibilityTextureAtlasMap> Atlas;
    };

    struct PrepareTextureResult
    {
      Texture SrcTexture;
      std::unique_ptr<CompatibilityTextureAtlasMap> Atlas;
    };

    struct PrepareCreateResult
    {
      UIAppTextureResourceCreationInfo CreationInfo;
      AnalyzedPath PathInfo;
      uint32_t Dpi;
      PrepareTextureResult TextureResult;
    };


    struct MaterialRecord
    {
      UIAppTextureHandle TextureHandle{};
      SpriteMaterialInfo MaterialInfo;
    };

    struct ImageRecord
    {
      SpriteType Type;
      UIAppTextureHandle TextureHandle{};
      IO::Path AtlasName;
      std::shared_ptr<IImageSprite> Sprite;
    };

    struct NineSliceRecord
    {
      SpriteType Type;
      UIAppTextureHandle TextureHandle{};
      IO::Path AtlasName;
      std::shared_ptr<INineSliceSprite> Sprite;
    };

    struct FontRecord
    {
      UIAppTextureHandle TextureHandle{};
      IO::Path FontName;
      std::shared_ptr<SpriteFont> Font;
    };

    struct TestPatternRecord
    {
      Texture Original;
      Texture TestPattern;
    };


    std::weak_ptr<IContentManager> m_contentManager;
    std::weak_ptr<INativeGraphics> m_nativeGraphics;
    uint32_t m_densityDpi{};
    UITestPatternMode m_testPatternMode{};
    HandleVector<TextureDefinition> m_textures;
    std::map<IO::Path, UIAppTextureHandle> m_textureLookup;
    std::map<SpriteMaterialId, MaterialRecord> m_materials;
    std::vector<ImageRecord> m_images;
    std::vector<NineSliceRecord> m_nineSlices;
    std::vector<FontRecord> m_fonts;
    SpriteManager m_manager;

    std::map<UIAppTextureHandle, TestPatternRecord> m_testPatternTextures;
    bool m_testPatternEnabled{false};

  public:
    struct StaticTextureInfo
    {
      UIAppTextureHandle Handle;
      std::shared_ptr<INativeTexture2D> Texture;
      PxExtent2D ExtentPx;
      PixelFormat TexturePixelFormat;
    };
    struct DynamicTextureInfo
    {
      UIAppTextureHandle Handle;
      std::shared_ptr<IDynamicNativeTexture2D> Texture;
      PxExtent2D ExtentPx;
      PixelFormat TexturePixelFormat;
    };

    UIAppResourceManager(std::weak_ptr<IContentManager> contentManager, std::weak_ptr<INativeGraphics> nativeGraphics,
                         const DemoWindowMetrics& windowMetrics, const UITestPatternMode testPatternMode);
    ~UIAppResourceManager() override;

    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics);


    // --- platform dependent interface (the platform here is NativeBatch and INativeGraphics)

    // [[deprecated("use the new sprites instead, as this doesnt support dp awareness")]]
    const CompatibilityTextureAtlasMap& GetLegacyTextureAtlasMap(const UIAppTextureHandle hTexture) const;


    //! The define native material signature is defined by the render engine
    StaticTextureInfo CreateTexture(const IO::PathView& atlasPath, const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                    const UIAppResourceFlag flags = UIAppResourceFlag::Undefined);
    //! The define native material signature is defined by the render engine
    StaticTextureInfo CreateTexture(const IO::Path& atlasPath, const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                    const UIAppResourceFlag flags = UIAppResourceFlag::Undefined)
    {
      return CreateTexture(atlasPath.AsPathView(), textureCreationInfo, flags);
    }


    //! The define native material signature is defined by the render engine
    DynamicTextureInfo CreateDynamicTexture(const IO::PathView& atlasPath, const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                            const UIAppResourceFlag flags = UIAppResourceFlag::Undefined);
    //! The define native material signature is defined by the render engine
    DynamicTextureInfo CreateDynamicTexture(const IO::Path& atlasPath, const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                            const UIAppResourceFlag flags = UIAppResourceFlag::Undefined)
    {
      return CreateDynamicTexture(atlasPath.AsPathView(), textureCreationInfo, flags);
    }

    //! @brief Define a material with the given unique spriteMaterialId
    void AddSpriteMaterial(const SpriteMaterialId& spriteMaterialId, const UIAppTextureHandle& hTexture, const BlendState blendState);

    // --- platform independent interface

    //! Get the material based on the material id (platform independent)
    SpriteMaterialInfo GetSpriteMaterialInfo(const SpriteMaterialId& spriteMaterialId) final;


    std::shared_ptr<BasicImageSprite> CreateBasicImageSprite(const SpriteMaterialId& spriteMaterialId, const IO::PathView& atlasPathName) final;
    std::shared_ptr<BasicImageSprite> CreateBasicImageSprite(const SpriteMaterialId& spriteMaterialId, const IO::Path& atlasPathName) final
    {
      return CreateBasicImageSprite(spriteMaterialId, atlasPathName.AsPathView());
    }

    std::shared_ptr<ImageSprite> CreateImageSprite(const SpriteMaterialId& spriteMaterialId, const IO::PathView& atlasPathName) final;
    std::shared_ptr<ImageSprite> CreateImageSprite(const SpriteMaterialId& spriteMaterialId, const IO::Path& atlasPathName) final
    {
      return CreateImageSprite(spriteMaterialId, atlasPathName.AsPathView());
    }

    std::shared_ptr<BasicNineSliceSprite> CreateBasicNineSliceSprite(const SpriteMaterialId& spriteMaterialId,
                                                                     const IO::PathView& atlasPathName) final;
    std::shared_ptr<BasicNineSliceSprite> CreateBasicNineSliceSprite(const SpriteMaterialId& spriteMaterialId, const IO::Path& atlasPathName) final
    {
      return CreateBasicNineSliceSprite(spriteMaterialId, atlasPathName.AsPathView());
    }

    std::shared_ptr<NineSliceSprite> CreateNineSliceSprite(const SpriteMaterialId& spriteMaterialId, const IO::PathView& atlasPathName) final;

    std::shared_ptr<NineSliceSprite> CreateNineSliceSprite(const SpriteMaterialId& spriteMaterialId, const IO::Path& atlasPathName) final
    {
      return CreateNineSliceSprite(spriteMaterialId, atlasPathName.AsPathView());
    }

    // std::shared_ptr<NineSliceSprite> CreateNineSliceSprite(const SpriteMaterialId& spriteMaterialId, const IO::PathView& atlasPathName,
    //                                                       const DpThicknessU nineSlice) final;

    // std::shared_ptr<NineSliceSprite> CreateNineSliceSprite(const SpriteMaterialId& spriteMaterialId, const IO::Path& atlasPathName,
    //                                                       const PxThicknessU nineSlice) final
    //{
    //  return CreateNineSliceSprite(spriteMaterialId, atlasPathName.AsPathView(), nineSlice);
    //}

    std::shared_ptr<SpriteFont> CreateLegacySpriteFont(const SpriteMaterialId& spriteMaterialId, const IO::PathView& fontName,
                                                       const SpriteFontConfig& spriteFontConfig) final;
    std::shared_ptr<SpriteFont> CreateLegacySpriteFont(const SpriteMaterialId& spriteMaterialId, const IO::Path& fontName,
                                                       const SpriteFontConfig& spriteFontConfig) final
    {
      return CreateLegacySpriteFont(spriteMaterialId, fontName.AsPathView(), spriteFontConfig);
    }

    //! Create a sprite font based on the given material id (platform independent)
    std::shared_ptr<SpriteFont> CreateSpriteFont(const SpriteMaterialId& spriteMaterialId, const IO::PathView& fontName,
                                                 const SpriteFontConfig& spriteFontConfig) final;
    //! Create a sprite font based on the given material id (platform independent)
    std::shared_ptr<SpriteFont> CreateSpriteFont(const SpriteMaterialId& spriteMaterialId, const IO::Path& fontName,
                                                 const SpriteFontConfig& spriteFontConfig) final
    {
      return CreateSpriteFont(spriteMaterialId, fontName.AsPathView(), spriteFontConfig);
    }


    //! @brief Enable/disable the UI test pattern (the manager has to be created with allow switching enabled for this to work)
    //! @return true if it was modified.
    bool SetTestPattern(const bool enabled);

  private:
    //! Create a sprite font based on the given material id (platform independent)
    std::shared_ptr<SpriteFont> DoCreateSpriteFont(const SpriteMaterialId& spriteMaterialId, const IO::PathView& fontName,
                                                   const SpriteFontConfig& spriteFontConfig, const bool isLegacyFullPathFontName);


    void UpdateDpAwareSpriteMaterials(IContentManager& contentManager, INativeGraphics& rNativeGraphics, const uint32_t densityDpi);

    void PatchMaterials(const HandleVector<TextureDefinition>::handle_type hTexture, HandleVector<TextureDefinition>::const_reference rEntry);
    void PatchImages(const HandleVector<TextureDefinition>::handle_type hTexture, HandleVector<TextureDefinition>::const_reference rEntry);
    void PatchNineSlices(const HandleVector<TextureDefinition>::handle_type hTexture, HandleVector<TextureDefinition>::const_reference rEntry);
    void PatchFonts(const HandleVector<TextureDefinition>::handle_type hTexture, IContentManager& contentManager,
                    HandleVector<TextureDefinition>::const_reference rEntry);

    PrepareCreateResult PrepareCreateTexture(const IO::PathView& atlasPath, const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                             const UIAppResourceFlag flags);
    UIAppTextureHandle DoAddTexture(PrepareCreateResult prepareCreateResult, const std::shared_ptr<INativeTexture2D>& nativeTexture,
                                    const UIAppResourceFlag flags);

    static PrepareTextureResult PrepareTexture(const IContentManager& contentManager, const AnalyzedPath& pathInfo, const bool isAtlas,
                                               const uint32_t selectedDp, const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                               const UITestPatternMode testPatternMode);

    static AnalyzedPath AnalyzePath(const IContentManager& contentManager, const IO::PathView pathView, const bool allowDpAware);
    static EncodedAvailableDp DetermineAvailableDp(const IContentManager& contentManager, const IO::Path& patternPath);
    static uint32_t DetermineResourceDp(const AnalyzedPath& path, const uint32_t densityDpi);
    static IO::Path BuildResourceName(const AnalyzedPath& analyzedPath, const uint32_t selectedDp);

    void ValidateSpriteFontExistence(const IContentManager& contentManager, const UIAppTextureHandle handle, const IO::PathView& fontName) const;
    BitmapFont ReadSpriteFont(const IContentManager& contentManager, const UIAppTextureHandle handle, const IO::PathView& fontName,
                              const bool isLegacyFullPathFontName) const;

    static IO::Path GetFontName(const TextureDefinition& textureInfo, const uint32_t dpi, const IO::PathView& fontName);

    void AttachTestPattern(const UIAppTextureHandle hTexture, Texture texture, const UIAppResourceFlag flags);
    void ApplyTestPattern();

    static AtlasTextureInfo GetAtlasTextureInfo(const TextureDefinition& textureDefinition, const IO::PathView& atlasPathName);
  };
}

#endif
