#ifndef FSLSIMPLEUI_APP_RESOURCE_UIAPPMATERIALMANAGER_HPP
#define FSLSIMPLEUI_APP_RESOURCE_UIAPPMATERIALMANAGER_HPP
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

#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxViewport.hpp>
#include <FslGraphics/Render/BlendState.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialId.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialInfo.hpp>
#include <FslGraphics/Vertices/VertexDeclaration.hpp>
#include <FslSimpleUI/App/UIAppTextureHandle.hpp>
#include <FslSimpleUI/App/Resource/UIAppMaterialInfo.hpp>
#include <map>
#include <memory>
#include <unordered_set>

namespace Fsl
{
  class BasicSpriteMaterial;
  class BitmapFont;
  class IBasicRenderSystem;
  class INativeTexture2D;
  struct SpriteMaterialId;

  namespace SimpleUIApp
  {
    struct UIAppTextureInfo;

    class UIAppMaterialManager
    {
      struct Options
      {
        bool AllowDepthBuffer{false};
        bool IsDynamic{false};
        bool UseCustomViewport{false};
        PxViewport CustomViewportPx;

        Options() = default;
        Options(const bool allowDepthBuffer, const bool isDynamic)
          : Options(allowDepthBuffer, isDynamic, false, PxViewport())
        {
        }

        Options(const bool allowDepthBuffer, const bool isDynamic, const bool useCustomViewport, const PxViewport& customViewportPx)
          : AllowDepthBuffer(allowDepthBuffer)
          , IsDynamic(isDynamic)
          , UseCustomViewport(useCustomViewport)
          , CustomViewportPx(customViewportPx)
        {
        }
      };

      struct MaterialRecord
      {
        UIAppTextureHandle TextureHandle{};
        std::shared_ptr<BasicSpriteMaterial> BasicMaterial;
        SpriteMaterialInfo MaterialInfo;
        BlendState SourceBlendState{BlendState::Opaque};
      };

      VertexDeclaration m_vertexDeclaration;
      std::map<SpriteMaterialId, MaterialRecord> m_materials;
      Options m_options;

      std::unordered_set<int32_t> m_usedDynamicIds;
      uint32_t m_nextDynamicId;

    public:
      UIAppMaterialManager(const bool allowDepthBuffer, const bool defaultToDynamicMaterials);
      ~UIAppMaterialManager();

      bool IsDepthBufferAllowed() const
      {
        return m_options.AllowDepthBuffer;
      }

      void Clear();

      bool Contains(const SpriteMaterialId spriteMaterialId) const;

      void AddMaterial(IBasicRenderSystem& rRenderSystem, const SpriteMaterialId spriteMaterialId, const UIAppTextureHandle hTexture,
                       const UIAppTextureInfo& textureInfo, const BlendState blendState);

      //! @brief Add a material with a dynamic sprite id (the actual id is managed by this class)
      SpriteMaterialId AddMaterial(IBasicRenderSystem& rRenderSystem, const UIAppTextureHandle hTexture, const UIAppTextureInfo& textureInfo,
                                   const BlendState blendState);

      bool RemoveMaterial(const SpriteMaterialId id);

      SpriteMaterialInfo GetSpriteMaterialInfo(const SpriteMaterialId spriteMaterialId) const;

      UIAppMaterialInfo GetMaterialInfo(const SpriteMaterialId spriteMaterialId) const;

      bool SetOptions(IBasicRenderSystem& rRenderSystem, const bool allowDepthBuffer);


      //! @brief Patch the material with a new size
      //! @param debugExpectedTextureHandle is the texture handle that we expect the material to have
      SpriteMaterialInfo PatchMaterial(const SpriteMaterialId spriteMaterialId, const PxExtent2D newExtentPx,
                                       const UIAppTextureHandle debugExpectedTextureHandle);

      SpriteMaterialInfo PatchSpriteFontMaterial(const SpriteMaterialId spriteMaterialId, const PxExtent2D newExtentPx, const BitmapFont& bitmapFont,
                                                 const UIAppTextureHandle debugExpectedTextureHandle);

      void PatchMaterials(IBasicRenderSystem& rRenderSystem, const PxViewport& viewportPx);

      void PatchMaterials(IBasicRenderSystem& rRenderSystem, const UIAppTextureHandle srcTextureHandle, const PxExtent2D srcExtentPx,
                          const std::shared_ptr<INativeTexture2D>& srcTexture);

      /// <summary>
      /// Release the texture from any material that use it.
      /// </summary>
      /// <param name="TextureHandle"></param>
      void ReleaseMaterialTexture(const UIAppTextureHandle hTexture);

    private:
      SpriteMaterialId CreateDynamicSpriteMaterialId();
      void DestroyDynamicSpriteMaterialId(const SpriteMaterialId spriteMaterialId) noexcept;
    };
  }
}

#endif
