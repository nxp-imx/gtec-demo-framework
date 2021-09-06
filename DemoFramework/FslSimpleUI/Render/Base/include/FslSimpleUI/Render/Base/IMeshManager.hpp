#ifndef FSLSIMPLEUI_RENDER_BASE_IMESHMANAGER_HPP
#define FSLSIMPLEUI_RENDER_BASE_IMESHMANAGER_HPP
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

#include <FslBase/Attributes.hpp>
#include <FslSimpleUI/Render/Base/MeshHandle.hpp>
#include <memory>

namespace Fsl
{
  class ISprite;
  class SpriteFont;
  class StringViewLite;

  namespace UI
  {
    class IMeshManager
    {
    public:
      virtual ~IMeshManager() = default;

      virtual MeshHandle CreateBasicMesh(const std::shared_ptr<ISprite>& sprite) = 0;
      virtual MeshHandle CreateBasicMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t vertexCapacity) = 0;
      virtual MeshHandle CreateMesh(const std::shared_ptr<ISprite>& sprite) = 0;
      //! @param vertexCapacity the desired vertex capacity the final result with be std::max(vertexCapacity, sprite type vertex requirement)
      //! @param indexCapacity the desired index capacity the final result with be std::max(indexCapacity, sprite type index requirement)
      virtual MeshHandle CreateMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t vertexCapacity, const uint32_t indexCapacity) = 0;
      //! @brief Create a mesh based on the given sprites material at materialIndex.
      virtual MeshHandle CreateMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t spriteMaterialIndex) = 0;
      //! @brief Create a mesh based on the given sprites material at materialIndex.
      virtual MeshHandle CreateMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t spriteMaterialIndex, const uint32_t vertexCapacity,
                                    const uint32_t indexCapacity) = 0;
      virtual MeshHandle CreateMesh(const std::shared_ptr<SpriteFont>& sprite) = 0;
      virtual MeshHandle CreateMesh(const std::shared_ptr<SpriteFont>& sprite, const uint32_t vertexCapacity, const uint32_t indexCapacity) = 0;
      virtual bool DestroyMesh(const MeshHandle hMesh) noexcept = 0;

      // virtual std::shared_ptr<ISprite> TryGetMeshSprite(const MeshHandle hMesh) = 0;
      FSL_FUNC_WARN_UNUSED_RESULT virtual MeshHandle SetBasicMeshSprite(const MeshHandle hMesh, const std::shared_ptr<ISprite>& sprite) = 0;
      FSL_FUNC_WARN_UNUSED_RESULT virtual MeshHandle SetMeshSprite(const MeshHandle hMesh, const std::shared_ptr<ISprite>& sprite) = 0;
      FSL_FUNC_WARN_UNUSED_RESULT virtual MeshHandle SetMeshSprite(const MeshHandle hMesh, const std::shared_ptr<SpriteFont>& sprite) = 0;
      FSL_FUNC_WARN_UNUSED_RESULT virtual MeshHandle SetMeshText(const MeshHandle hMesh, const StringViewLite& text) = 0;

      virtual void EnsureCapacity(const MeshHandle hMesh, const uint32_t vertexCapacity, const uint32_t indexCapacity) = 0;
    };
  }
}

#endif
