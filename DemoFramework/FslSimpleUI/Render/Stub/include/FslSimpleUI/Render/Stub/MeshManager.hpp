#ifndef FSLSIMPLEUI_RENDER_STUB_MESHMANAGER_HPP
#define FSLSIMPLEUI_RENDER_STUB_MESHMANAGER_HPP
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
#include <FslSimpleUI/Render/Base/IMeshManager.hpp>

namespace Fsl::UI::RenderStub
{
  class MeshManager final : public IMeshManager
  {
    bool m_isDisposed{false};
    HandleVector<bool> m_meshes;

  public:
    MeshManager();
    ~MeshManager() override;

    void Dispose() noexcept;

    MeshHandle CreateBasicMesh(const std::shared_ptr<ISprite>& sprite) final;
    MeshHandle CreateBasicMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t vertexCapacity) final;
    MeshHandle CreateMesh(const std::shared_ptr<ISprite>& sprite) final;
    MeshHandle CreateMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t vertexCapacity, const uint32_t indexCapacity) final;
    MeshHandle CreateMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t spriteMaterialIndex) final;
    MeshHandle CreateMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t spriteMaterialIndex, const uint32_t vertexCapacity,
                          const uint32_t indexCapacity) final;
    MeshHandle CreateMesh(const std::shared_ptr<SpriteFont>& sprite) final;
    MeshHandle CreateMesh(const std::shared_ptr<SpriteFont>& sprite, const uint32_t vertexCapacity, const uint32_t indexCapacity) final;

    bool DestroyMesh(const MeshHandle hMesh) noexcept final;
    // std::shared_ptr<ISprite> TryGetMeshSprite(const MeshHandle hMesh) final;
    [[nodiscard]] MeshHandle SetBasicMeshSprite(const MeshHandle hMesh, const std::shared_ptr<ISprite>& sprite) final;
    [[nodiscard]] MeshHandle SetMeshSprite(const MeshHandle hMesh, const std::shared_ptr<ISprite>& sprite) final;
    [[nodiscard]] MeshHandle SetMeshSprite(const MeshHandle hMesh, const std::shared_ptr<SpriteFont>& sprite) final;

    [[nodiscard]] MeshHandle SetMeshText(const MeshHandle hMesh, const StringViewLite& text) final;

    void EnsureCapacity(const MeshHandle hMesh, const uint32_t vertexCapacity, const uint32_t indexCapacity) final;
  };
}

#endif
