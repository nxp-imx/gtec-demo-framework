#ifndef FSLSIMPLEUI_RENDER_IMBATCH_MESHMANAGER_HPP
#define FSLSIMPLEUI_RENDER_IMBATCH_MESHMANAGER_HPP
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

#include <FslBase/Collections/HandleVector.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslGraphics/Sprite/Font/SpriteFontGlyphPosition.hpp>
#include <FslGraphics/Sprite/ISprite.hpp>
#include <FslGraphics/Sprite/Material/SpriteMaterialInfo.hpp>
#include <FslSimpleUI/Render/Base/DrawCommandBufferEx.hpp>
#include <FslSimpleUI/Render/Base/IMeshManager.hpp>
#include <map>
#include <memory>
#include <utility>
#include <vector>
#include "MaterialLookup.hpp"
#include "MeshTransparencyFlags.hpp"
#include "RenderDrawSpriteType.hpp"

namespace Fsl
{
  class BasicImageSprite;
  class BasicNineSliceSprite;
  struct FontGlyphPosition;
  class ImageSprite;
  class NineSliceSprite;
  class OptimizedBasicNineSliceSprite;
  class OptimizedNineSliceSprite;
  class SpriteFont;
  namespace UI
  {
    class UITextMeshBuilder;
  }
}


namespace Fsl::UI::RenderIMBatch
{
  class MeshManager final : public IMeshManager
  {
    struct AddMeshResult
    {
      RenderDrawSpriteType DrawSpriteType{RenderDrawSpriteType::Dummy};
      uint32_t VertexCapacity{0};
      uint32_t IndexCapacity{0};
      int32_t MeshValue{HandleVectorConfig::InvalidHandle};
    };

  public:
    struct Capacity
    {
      uint32_t VertexCapacity{0};
      uint32_t IndexCapacity{0};
    };


    struct DummySpriteMeshRecord
    {
      std::shared_ptr<ISprite> Sprite;
      BatchMaterialHandle MaterialHandle;
      uint32_t SpriteMaterialIndex{0};
      uint32_t VertexCapacity{0};
      uint32_t IndexCapacity{0};
      bool IsOpaque{false};

      DummySpriteMeshRecord() = default;
      DummySpriteMeshRecord(std::shared_ptr<ISprite> sprite, const BatchMaterialHandle materialHandle, const uint32_t spriteMaterialIndex,
                            const bool isOpaque, const uint32_t vertexCapacity, const uint32_t indexCapacity)
        : Sprite(std::move(sprite))
        , MaterialHandle(materialHandle)
        , SpriteMaterialIndex(spriteMaterialIndex)
        , VertexCapacity(vertexCapacity)
        , IndexCapacity(indexCapacity)
        , IsOpaque(isOpaque)
      {
      }

      void UpdateCache()
      {
      }
    };

    struct BasicImageSpriteMeshRecord
    {
      std::shared_ptr<BasicImageSprite> Sprite;
      BatchMaterialHandle MaterialHandle;
      uint32_t SpriteMaterialIndex{0};
      uint32_t VertexCapacity{0};
      uint32_t IndexCapacity{0};
      bool IsOpaque{false};

      BasicImageSpriteMeshRecord() = default;
      BasicImageSpriteMeshRecord(std::shared_ptr<BasicImageSprite> sprite, const BatchMaterialHandle materialHandle,
                                 const uint32_t spriteMaterialIndex, const bool isOpaque, const uint32_t vertexCapacity, const uint32_t indexCapacity)
        : Sprite(std::move(sprite))
        , MaterialHandle(materialHandle)
        , SpriteMaterialIndex(spriteMaterialIndex)
        , VertexCapacity(vertexCapacity)
        , IndexCapacity(indexCapacity)
        , IsOpaque(isOpaque)
      {
      }

      void UpdateCache()
      {
      }
    };

    struct BasicNineSliceSpriteMeshRecord
    {
      std::shared_ptr<BasicNineSliceSprite> Sprite;
      BatchMaterialHandle MaterialHandle;
      uint32_t SpriteMaterialIndex{0};
      uint32_t VertexCapacity{0};
      uint32_t IndexCapacity{0};
      bool IsOpaque{false};

      BasicNineSliceSpriteMeshRecord() = default;
      BasicNineSliceSpriteMeshRecord(std::shared_ptr<BasicNineSliceSprite> sprite, const BatchMaterialHandle materialHandle,
                                     const uint32_t spriteMaterialIndex, const bool isOpaque, const uint32_t vertexCapacity,
                                     const uint32_t indexCapacity)
        : Sprite(std::move(sprite))
        , MaterialHandle(materialHandle)
        , SpriteMaterialIndex(spriteMaterialIndex)
        , VertexCapacity(vertexCapacity)
        , IndexCapacity(indexCapacity)
        , IsOpaque(isOpaque)
      {
      }

      void UpdateCache()
      {
      }
    };

    struct ImageSpriteMeshRecord
    {
      std::shared_ptr<ImageSprite> Sprite;
      BatchMaterialHandle MaterialHandle;
      uint32_t SpriteMaterialIndex{0};
      uint32_t VertexCapacity{0};
      uint32_t IndexCapacity{0};
      bool IsOpaque{false};

      ImageSpriteMeshRecord() = default;
      ImageSpriteMeshRecord(std::shared_ptr<ImageSprite> sprite, const BatchMaterialHandle materialHandle, const uint32_t spriteMaterialIndex,
                            const bool isOpaque, const uint32_t vertexCapacity, const uint32_t indexCapacity)
        : Sprite(std::move(sprite))
        , MaterialHandle(materialHandle)
        , SpriteMaterialIndex(spriteMaterialIndex)
        , VertexCapacity(vertexCapacity)
        , IndexCapacity(indexCapacity)
        , IsOpaque(isOpaque)
      {
      }

      void UpdateCache()
      {
      }
    };

    struct NineSliceSpriteMeshRecord
    {
      std::shared_ptr<NineSliceSprite> Sprite;
      BatchMaterialHandle MaterialHandle;
      uint32_t SpriteMaterialIndex{0};
      uint32_t VertexCapacity{0};
      uint32_t IndexCapacity{0};
      bool IsOpaque{false};

      NineSliceSpriteMeshRecord() = default;
      NineSliceSpriteMeshRecord(std::shared_ptr<NineSliceSprite> sprite, const BatchMaterialHandle materialHandle, const uint32_t spriteMaterialIndex,
                                const bool isOpaque, const uint32_t vertexCapacity, const uint32_t indexCapacity)
        : Sprite(std::move(sprite))
        , MaterialHandle(materialHandle)
        , SpriteMaterialIndex(spriteMaterialIndex)
        , VertexCapacity(vertexCapacity)
        , IndexCapacity(indexCapacity)
        , IsOpaque(isOpaque)
      {
      }

      void UpdateCache()
      {
      }
    };

    struct MaterialInfo
    {
      BatchMaterialHandle MaterialHandle;
      uint32_t SpriteMaterialIndex{0};

      constexpr MaterialInfo() noexcept = default;
      constexpr MaterialInfo(const BatchMaterialHandle materialHandle, const uint32_t spriteMaterialIndex) noexcept
        : MaterialHandle(materialHandle)
        , SpriteMaterialIndex(spriteMaterialIndex)
      {
      }
    };

    struct OptimizedNineSliceSpriteMeshRecord
    {
      std::shared_ptr<OptimizedNineSliceSprite> Sprite;
      BatchMaterialHandle OpaqueMaterialHandle;
      uint32_t OpaqueSpriteMaterialIndex{0};
      BatchMaterialHandle TransparentMaterialHandle;
      uint32_t TransparentSpriteMaterialIndex{0};
      uint32_t VertexCapacity{0};
      uint32_t IndexCapacity{0};
      MeshTransparencyFlags Transparency{MeshTransparencyFlags::Opaque};

      OptimizedNineSliceSpriteMeshRecord() = default;
      OptimizedNineSliceSpriteMeshRecord(std::shared_ptr<OptimizedNineSliceSprite> sprite, const BatchMaterialHandle opaqueMaterialHandle,
                                         const uint32_t opaqueSpriteMaterialIndex, const BatchMaterialHandle transparentMaterialHandle,
                                         const uint32_t transparentSpriteMaterialIndex, const uint32_t vertexCapacity, const uint32_t indexCapacity,
                                         const MeshTransparencyFlags meshTransparency)
        : Sprite(std::move(sprite))
        , OpaqueMaterialHandle(opaqueMaterialHandle)
        , OpaqueSpriteMaterialIndex(opaqueSpriteMaterialIndex)
        , TransparentMaterialHandle(transparentMaterialHandle)
        , TransparentSpriteMaterialIndex(transparentSpriteMaterialIndex)
        , VertexCapacity(vertexCapacity)
        , IndexCapacity(indexCapacity)
        , Transparency(meshTransparency)
      {
      }

      void UpdateCache()
      {
      }

      constexpr uint32_t GetMaterialCount() const
      {
        return 2u;
      }

      constexpr MaterialInfo GetMaterialInfo(const uint32_t index) const
      {
        switch (index)
        {
        case 0:
          return {OpaqueMaterialHandle, OpaqueSpriteMaterialIndex};
        default:
          return {TransparentMaterialHandle, TransparentSpriteMaterialIndex};
        }
      }

      void SetMaterialHandle(const uint32_t index, const BatchMaterialHandle materialHandle)
      {
        switch (index)
        {
        case 0:
          OpaqueMaterialHandle = materialHandle;
          break;
        default:
          TransparentMaterialHandle = materialHandle;
          break;
        }
      }
    };

    struct SpriteFontMeshRecord
    {
      std::shared_ptr<SpriteFont> Sprite;
      // NOLINTNEXTLINE(readability-identifier-naming)
      std::string m_text;
      // NOLINTNEXTLINE(readability-identifier-naming)
      std::vector<SpriteFontGlyphPosition> m_glyphs;
      // NOLINTNEXTLINE(readability-identifier-naming)
      uint32_t m_glyphCount{};
      BatchMaterialHandle MaterialHandle;
      uint32_t SpriteMaterialIndex{0};
      uint32_t VertexCapacity{0};
      uint32_t IndexCapacity{0};
      bool IsOpaque{false};

      SpriteFontMeshRecord() = default;
      SpriteFontMeshRecord(std::shared_ptr<SpriteFont> sprite, const BatchMaterialHandle materialHandle, const uint32_t spriteMaterialIndex,
                           const bool isOpaque, const uint32_t vertexCapacity, const uint32_t indexCapacity)
        : Sprite(std::move(sprite))
        , MaterialHandle(materialHandle)
        , SpriteMaterialIndex(spriteMaterialIndex)
        , VertexCapacity(vertexCapacity)
        , IndexCapacity(indexCapacity)
        , IsOpaque(isOpaque)
      {
      }

      bool SetSprite(const std::shared_ptr<SpriteFont>& sprite)
      {
        const bool changed = sprite != Sprite;
        if (changed)
        {
          Sprite = sprite;
          UpdateCache();
        }
        return changed;
      }

      StringViewLite GetText() const;

      bool SetText(const StringViewLite text);
      void UpdateCache();

      ReadOnlySpan<SpriteFontGlyphPosition> GetGlyphs() const noexcept
      {
        return SpanUtil::UncheckedAsReadOnlySpan(m_glyphs, 0, m_glyphCount);
      }
    };

  private:
    MaterialLookup m_materialLookup;
    HandleVector<DummySpriteMeshRecord> m_meshesDummy;
    HandleVector<BasicImageSpriteMeshRecord> m_meshesBasicImageSprite;
    HandleVector<BasicNineSliceSpriteMeshRecord> m_meshesBasicNineSliceSprite;
    HandleVector<ImageSpriteMeshRecord> m_meshesImageSprite;
    HandleVector<NineSliceSpriteMeshRecord> m_meshesNineSliceSprite;
    HandleVector<OptimizedNineSliceSpriteMeshRecord> m_meshesOptimizedNineSliceSprite;
    HandleVector<SpriteFontMeshRecord> m_meshesSpriteFont;
    Capacity m_capacity;
    std::shared_ptr<UITextMeshBuilder> m_textMeshBuilder;

  public:
    explicit MeshManager(const SpriteMaterialInfo& defaultMaterialInfo);
    ~MeshManager() final;

    void PreDraw();


    Capacity GetCapacity() const noexcept
    {
      return m_capacity;
    }

    uint32_t GetMeshCount() const noexcept
    {
      // return m_meshes.Count();
      return m_meshesBasicImageSprite.Count() + m_meshesBasicNineSliceSprite.Count() + m_meshesImageSprite.Count() + m_meshesNineSliceSprite.Count() +
             m_meshesOptimizedNineSliceSprite.Count() + m_meshesSpriteFont.Count();
    }


    const BasicImageSpriteMeshRecord& UncheckedGetBasicImageSprite(const int32_t handle) const noexcept
    {
      return m_meshesBasicImageSprite.FastGet(handle);
    }

    const BasicNineSliceSpriteMeshRecord& UncheckedGetBasicNineSliceSprite(const int32_t handle) const noexcept
    {
      return m_meshesBasicNineSliceSprite.FastGet(handle);
    }

    const ImageSpriteMeshRecord& UncheckedGetImageSprite(const int32_t handle) const noexcept
    {
      return m_meshesImageSprite.FastGet(handle);
    }

    const NineSliceSpriteMeshRecord& UncheckedGetNineSliceSprite(const int32_t handle) const noexcept
    {
      return m_meshesNineSliceSprite.FastGet(handle);
    }

    const OptimizedNineSliceSpriteMeshRecord& UncheckedGetOptimizedNineSliceSprite(const int32_t handle) const noexcept
    {
      return m_meshesOptimizedNineSliceSprite.FastGet(handle);
    }

    const SpriteFontMeshRecord& UncheckedGetSpriteFont(const int32_t handle) const noexcept
    {
      return m_meshesSpriteFont.FastGet(handle);
    }

    // const MeshRecord& UncheckedGet(const int32_t handle) const noexcept
    //{
    //  return m_meshes.FastGet(handle);
    //}

    const MaterialLookup& GetMaterialLookup() const
    {
      return m_materialLookup;
    }

    UITextMeshBuilder& GetTextMeshBuilder()
    {
      return *m_textMeshBuilder;
    }

    void OnConfigurationChanged();

    // IMeshManager
    [[nodiscard]] MeshHandle CreateBasicMesh(const std::shared_ptr<ISprite>& sprite) final;
    [[nodiscard]] MeshHandle CreateBasicMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t vertexCapacity) final;
    [[nodiscard]] MeshHandle CreateMesh(const std::shared_ptr<ISprite>& sprite) final;
    [[nodiscard]] MeshHandle CreateMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t vertexCapacity, const uint32_t indexCapacity) final;
    [[nodiscard]] MeshHandle CreateMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t spriteMaterialIndex) final;
    [[nodiscard]] MeshHandle CreateMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t spriteMaterialIndex, const uint32_t vertexCapacity,
                                        const uint32_t indexCapacity) final;
    [[nodiscard]] MeshHandle CreateMesh(const std::shared_ptr<SpriteFont>& sprite) final;
    [[nodiscard]] MeshHandle CreateMesh(const std::shared_ptr<SpriteFont>& sprite, const uint32_t vertexCapacity, const uint32_t indexCapacity) final;
    bool DestroyMesh(const MeshHandle hMesh) noexcept final;
    [[nodiscard]] MeshHandle SetBasicMeshSprite(const MeshHandle hMesh, const std::shared_ptr<ISprite>& sprite) final;
    [[nodiscard]] MeshHandle SetMeshSprite(const MeshHandle hMesh, const std::shared_ptr<ISprite>& sprite) final;
    [[nodiscard]] MeshHandle SetMeshSprite(const MeshHandle hMesh, const std::shared_ptr<SpriteFont>& sprite) final;
    [[nodiscard]] MeshHandle SetMeshText(const MeshHandle hMesh, const StringViewLite& text) final;
    void EnsureCapacity(const MeshHandle hMesh, const uint32_t vertexCapacity, const uint32_t indexCapacity) final;

  private:
    AddMeshResult AddSpriteBasicMesh(const std::shared_ptr<ISprite>& sprite, const BatchMaterialHandle batchMaterialHandle,
                                     const uint32_t spriteMaterialIndex, const bool isOpaque, const uint32_t vertexCapacity);
    AddMeshResult AddSpriteMesh(const std::shared_ptr<ISprite>& sprite, const BatchMaterialHandle batchMaterialHandle,
                                const uint32_t spriteMaterialIndex, const bool isOpaque, const uint32_t vertexCapacity, const uint32_t indexCapacity);
    void SanityCheckAll() noexcept;
    void SanityCheck() noexcept;
    void SanityCheckMats() noexcept;
  };
}

#endif
