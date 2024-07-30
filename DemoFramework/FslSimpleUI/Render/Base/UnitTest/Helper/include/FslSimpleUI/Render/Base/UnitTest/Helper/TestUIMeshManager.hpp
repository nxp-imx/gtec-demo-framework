#ifndef FSLSIMPLEUI_RENDER_BASE_UNITTEST_HELPER_TESTUIMESHMANAGER_HPP
#define FSLSIMPLEUI_RENDER_BASE_UNITTEST_HELPER_TESTUIMESHMANAGER_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/Collections/VersionedHandleVector.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslGraphics/Sprite/Font/SpriteFontConfig.hpp>
#include <FslGraphics/Sprite/ISprite.hpp>
#include <FslSimpleUI/Render/Base/IMeshManager.hpp>
#include <memory>
#include <string>
#include <utility>

namespace Fsl::UI
{
  class TestUIMeshManager final : public IMeshManager
  {
    struct Record
    {
      std::shared_ptr<ISprite> Sprite;
      bool IsFontSprite{false};
      uint32_t InstanceCount{0};
      uint32_t MaterialIndex{0};
      SpriteFontConfig FontConfig;
      std::string Text;
      uint32_t MaxGlyphs{0};
      uint32_t VertexCapacity{0};
      uint32_t IndexCapacity{0};

      Record() = default;

      Record(std::shared_ptr<ISprite> sprite, const uint32_t instanceCount, const uint32_t spriteMaterialIndex)
        : Sprite(std::move(sprite))
        , InstanceCount(instanceCount)
        , MaterialIndex(spriteMaterialIndex)
        , VertexCapacity(4)
        , IndexCapacity(6)
      {
        if (!Sprite)
        {
          throw std::invalid_argument("can not be null");
        }
      }

      Record(std::shared_ptr<SpriteFont> sprite, const SpriteFontConfig fontConfig, const StringViewLite text)
        : Sprite(std::move(sprite))
        , IsFontSprite(true)
        , FontConfig(fontConfig)
        , Text(text)
      {
        if (!Sprite)
        {
          throw std::invalid_argument("can not be null");
        }
      }

      Record(std::shared_ptr<SpriteFont> sprite, const SpriteFontConfig fontConfig, const StringViewLite text, const uint32_t maxGlyphs)
        : Sprite(std::move(sprite))
        , IsFontSprite(true)
        , FontConfig(fontConfig)
        , Text(text)
        , MaxGlyphs(maxGlyphs)
      {
        if (!Sprite)
        {
          throw std::invalid_argument("can not be null");
        }
      }
    };

    VersionedHandleVector<Record> m_entries;

  public:
    TestUIMeshManager();

    [[nodiscard]] MeshHandle CreateBasicMesh(const std::shared_ptr<ISprite>& sprite) final;
    [[nodiscard]] MeshHandle CreateBasicMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t vertexCapacity) final;
    [[nodiscard]] MeshHandle CreateMesh(const std::shared_ptr<ISprite>& sprite) final;
    [[nodiscard]] MeshHandle CreateMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t vertexCapacity, const uint32_t indexCapacity) final;
    [[nodiscard]] MeshHandle CreateMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t spriteMaterialIndex) final;
    [[nodiscard]] MeshHandle CreateMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t spriteMaterialIndex, const uint32_t vertexCapacity,
                                        const uint32_t indexCapacity) final;
    [[nodiscard]] MeshHandle CreateMesh(const std::shared_ptr<SpriteFont>& sprite) final;
    [[nodiscard]] MeshHandle CreateMesh(const std::shared_ptr<SpriteFont>& sprite, const uint32_t vertexCapacity, const uint32_t indexCapacity) final;
    bool DestroyMesh(const MeshHandle hMesh) noexcept override;
    [[nodiscard]] MeshHandle SetBasicMeshSprite(const MeshHandle hMesh, const std::shared_ptr<ISprite>& sprite) final;
    [[nodiscard]] MeshHandle SetMeshSprite(const MeshHandle hMesh, const std::shared_ptr<ISprite>& sprite) final;
    [[nodiscard]] MeshHandle SetMeshSprite(const MeshHandle hMesh, const std::shared_ptr<SpriteFont>& sprite) final;
    [[nodiscard]] MeshHandle SetMeshText(const MeshHandle hMesh, const StringViewLite& text) final;
    void EnsureCapacity(const MeshHandle hMesh, const uint32_t vertexCapacity, const uint32_t indexCapacity) final;
  };
}


#endif
