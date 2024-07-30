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

#include <FslBase/Bits/BitsUtil.hpp>
#include <FslBase/Collections/HandleVector.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslBase/Math/Pixel/PxViewport.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <FslGraphics/Render/BlendState.hpp>
#include <FslGraphics/Sprite/ICustomSpriteResourceManager.hpp>
#include <FslGraphics/Sprite/ISpriteResourceManager.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialInfo.hpp>
#include <FslGraphics/Sprite/SpriteManager.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslSimpleUI/App/Resource/UIAppMaterialManager.hpp>
#include <FslSimpleUI/App/Resource/UIAppTextureManager.hpp>
#include <FslSimpleUI/App/UIAppResourceFlag.hpp>
#include <FslSimpleUI/App/UIAppTextureHandle.hpp>
#include <FslSimpleUI/App/UIAppTextureResourceCreationInfo.hpp>
#include <FslSimpleUI/App/UITestPatternMode.hpp>
#include <memory>

namespace Fsl
{
  struct DemoWindowMetrics;
  class Bitmap;
  class BasicNineSliceSprite;
  class BasicSpriteMaterial;
  class IContentManager;
  class IDynamicNativeTexture2D;
  class IImageSprite;
  class IBasicRenderSystem;
  class INativeTexture2D;
  class INineSliceSprite;
  class ISpriteMaterial;
  struct PxViewport;
  class SpriteFont;
  struct SpriteFontConfig;
  struct SpriteMaterialId;

  class UIAppResourceManager final
    : public ISpriteResourceManager
    , public ICustomSpriteResourceManager
  {
    enum class SpriteType
    {
      Basic = 0,
      Normal = 1,
    };


    struct ImageRecord
    {
      SpriteType Type;
      UIAppTextureHandle TextureHandle;
      IO::Path AtlasName;
      std::shared_ptr<IImageSprite> Sprite;
    };

    struct NineSliceRecord
    {
      SpriteType Type{SpriteType::Basic};
      UIAppTextureHandle TextureHandle;
      IO::Path AtlasName;
      std::shared_ptr<INineSliceSprite> Sprite;

      NineSliceRecord() = default;
      NineSliceRecord(const SpriteType type, const UIAppTextureHandle textureHandle, IO::Path atlasName, std::shared_ptr<INineSliceSprite> sprite)
        : Type(type)
        , TextureHandle(textureHandle)
        , AtlasName(std::move(atlasName))
        , Sprite(std::move(sprite))
      {
      }
    };

    struct FontRecord
    {
      UIAppTextureHandle TextureHandle;
      IO::Path FontName;
      std::shared_ptr<SpriteFont> Font;
    };

    struct CustomTextureRecord
    {
      UIAppTextureHandle TextureHandle;
      SpriteMaterialId MaterialId;
      std::weak_ptr<IImageSprite> Sprite;
    };


    std::weak_ptr<IContentManager> m_contentManager;
    std::weak_ptr<IBasicRenderSystem> m_renderSystem;

    SimpleUIApp::UIAppTextureManager m_textureManager;
    SimpleUIApp::UIAppMaterialManager m_materialManager;

    std::vector<ImageRecord> m_images;
    std::vector<NineSliceRecord> m_nineSlices;
    std::vector<FontRecord> m_fonts;
    std::vector<CustomTextureRecord> m_customTextures;
    SpriteManager m_manager;


  public:
    UIAppResourceManager(std::weak_ptr<IContentManager> contentManager, std::weak_ptr<IBasicRenderSystem> renderSystem,
                         const DemoWindowMetrics& windowMetrics, const UITestPatternMode testPatternMode, const bool allowDepthBuffer,
                         const bool defaultToDynamicMaterials, const bool useYFlipTextureCoordinates);
    ~UIAppResourceManager() override;

    // NOLINTNEXTLINE(readability-identifier-naming)
    void SYS_SetRenderSystemViewport(const PxViewport& viewPortPx);
    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics);

    bool FontExists(const UIAppTextureHandle textureHandle, IO::PathView fontName) const;


    // --- platform dependent interface (the platform here is NativeBatch and IBasicRenderSystem)

    // [[deprecated("use the new sprites instead, as this doesnt support dp awareness")]]
    const CompatibilityTextureAtlasMap& GetLegacyTextureAtlasMap(const UIAppTextureHandle hTexture) const;


    //! The define native material signature is defined by the render engine
    UIAppTexture<INativeTexture2D> CreateTexture(const IO::PathView& atlasPath, const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                                 const UIAppResourceFlag flags = UIAppResourceFlag::Undefined);
    //! The define native material signature is defined by the render engine
    UIAppTexture<INativeTexture2D> CreateTexture(const IO::Path& atlasPath, const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                                 const UIAppResourceFlag flags = UIAppResourceFlag::Undefined)
    {
      return CreateTexture(atlasPath.AsPathView(), textureCreationInfo, flags);
    }


    //! The define native material signature is defined by the render engine
    UIAppTexture<IDynamicNativeTexture2D> CreateDynamicTexture(const IO::PathView& atlasPath,
                                                               const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                                               const UIAppResourceFlag flags = UIAppResourceFlag::Undefined);
    //! The define native material signature is defined by the render engine
    UIAppTexture<IDynamicNativeTexture2D> CreateDynamicTexture(const IO::Path& atlasPath, const UIAppTextureResourceCreationInfo& textureCreationInfo,
                                                               const UIAppResourceFlag flags = UIAppResourceFlag::Undefined)
    {
      return CreateDynamicTexture(atlasPath.AsPathView(), textureCreationInfo, flags);
    }

    //! @brief Define a material with the given unique spriteMaterialId
    void AddSpriteMaterial(const SpriteMaterialId& spriteMaterialId, const UIAppTextureHandle& hTexture, const BlendState blendState,
                           const BasicPrimitiveTopology primitiveTopology = BasicPrimitiveTopology::TriangleList);

    // --- platform independent custom texture sprites

    //! @brief Create a custom 'texture' sprite which have a dynamic lifetime that is valid until the caller releases it's last pointer.
    //!        This means that the custom sprite and backing texture will not be kept alive by the resource manager.
    std::shared_ptr<BasicImageSprite> CreateCustomTextureSprite(const std::shared_ptr<INativeTexture2D>& nativeTexture,
                                                                const BlendState blendState) final;

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

    //! Create a basic nine slice sprite based on the given material id (platform independent)
    std::shared_ptr<OptimizedBasicNineSliceSprite> CreateOptimizedBasicNineSliceSprite(const SpriteMaterialId& opaqueSpriteMaterialId,
                                                                                       const SpriteMaterialId& transparentSpriteMaterialId,
                                                                                       const IO::PathView& atlasPathName) final;

    //! Create a basic nine slice sprite based on the given material id (platform independent)
    std::shared_ptr<OptimizedBasicNineSliceSprite> CreateOptimizedBasicNineSliceSprite(const SpriteMaterialId& opaqueSpriteMaterialId,
                                                                                       const SpriteMaterialId& transparentSpriteMaterialId,
                                                                                       const IO::Path& atlasPathName) final
    {
      return CreateOptimizedBasicNineSliceSprite(opaqueSpriteMaterialId, transparentSpriteMaterialId, atlasPathName.AsPathView());
    }

    //! Create a nine slice sprite based on the given material id (platform independent)
    std::shared_ptr<OptimizedNineSliceSprite> CreateOptimizedNineSliceSprite(const SpriteMaterialId& opaqueSpriteMaterialId,
                                                                             const SpriteMaterialId& transparentSpriteMaterialId,
                                                                             const IO::PathView& atlasPathName) final;

    //! Create a nine slice sprite based on the given material id (platform independent)
    std::shared_ptr<OptimizedNineSliceSprite> CreateOptimizedNineSliceSprite(const SpriteMaterialId& opaqueSpriteMaterialId,
                                                                             const SpriteMaterialId& transparentSpriteMaterialId,
                                                                             const IO::Path& atlasPathName) final
    {
      return CreateOptimizedNineSliceSprite(opaqueSpriteMaterialId, transparentSpriteMaterialId, atlasPathName.AsPathView());
    }

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

    //! Create a sprite font based on the given material id (platform independent)
    std::shared_ptr<SpriteFont> TryCreateSpriteFont(const SpriteMaterialId& spriteMaterialId, const IO::PathView& fontName,
                                                    const SpriteFontConfig& spriteFontConfig) final;
    //! Create a sprite font based on the given material id (platform independent)
    std::shared_ptr<SpriteFont> TryCreateSpriteFont(const SpriteMaterialId& spriteMaterialId, const IO::Path& fontName,
                                                    const SpriteFontConfig& spriteFontConfig) final
    {
      return TryCreateSpriteFont(spriteMaterialId, fontName.AsPathView(), spriteFontConfig);
    }

    void PatchSpriteFont(std::shared_ptr<SpriteFont> font, const SpriteMaterialId& spriteMaterialId, const IO::PathView& fontName) final;
    void PatchSpriteFont(std::shared_ptr<SpriteFont> font, const SpriteMaterialId& spriteMaterialId, const IO::Path& fontName) final
    {
      return PatchSpriteFont(font, spriteMaterialId, fontName.AsPathView());
    }


    //! @brief Enable / disable depth buffer various options for all materials
    bool SetOptions(const bool allowDepthBuffer);


    //! @brief Enable/disable the UI test pattern (the manager has to be created with allow switching enabled for this to work)
    //! @return true if it was modified.
    bool SetTestPattern(const bool enabled);

    const SpriteNativeAreaCalc& GetSpriteNativeAreaCalc() const
    {
      return m_manager.GetSpriteNativeAreaCalc();
    }

  private:
    //! Create a sprite font based on the given material id (platform independent)
    std::shared_ptr<SpriteFont> DoCreateSpriteFont(const SpriteMaterialId& spriteMaterialId, const IO::PathView& fontName,
                                                   const SpriteFontConfig& spriteFontConfig, const bool isLegacyFullPathFontName);


    void UpdateDpAwareSpriteMaterials(IContentManager& contentManager, IBasicRenderSystem& rRenderSystem, const uint32_t densityDpi);

    void PatchContent(const UIAppTextureHandle srcTextureHandle, const PxExtent2D srcExtentPx, IContentManager* const pContentManager);
    void PatchImages(const UIAppTextureHandle srcTextureHandle, const PxExtent2D srcExtentPx);
    void PatchNineSlices(const UIAppTextureHandle srcTextureHandle, const PxExtent2D srcExtentPx);
    void PatchFonts(const UIAppTextureHandle srcTextureHandle, const PxExtent2D srcExtentPx, IContentManager& contentManager);
    void PerformGarbageCollection();
  };
}

#endif
