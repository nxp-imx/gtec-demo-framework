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

#include <FslSimpleUI/Render/Stub/MeshManager.hpp>
#include <FslBase/BasicTypes.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>

namespace Fsl
{
  namespace UI
  {
    namespace RenderStub
    {
      MeshManager::MeshManager() = default;

      MeshManager::~MeshManager()
      {
        Dispose();
      }

      void MeshManager::Dispose() noexcept
      {
        if (m_isDisposed)
        {
          return;
        }
        m_isDisposed = true;
        FSLLOG3_ERROR_IF(m_meshes.Count() > 0, "There are still {} meshes allocated as dispose time", m_meshes.Count());
        m_meshes.Clear();
      }

      MeshHandle MeshManager::CreateBasicMesh(const std::shared_ptr<ISprite>& sprite)
      {
        FSL_PARAM_NOT_USED(sprite);
        if (m_isDisposed)
        {
          throw UsageErrorException("Disposed");
        }
        return MeshHandle(m_meshes.Add(true));
      }

      MeshHandle MeshManager::CreateBasicMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t vertexCapacity)
      {
        FSL_PARAM_NOT_USED(sprite);
        FSL_PARAM_NOT_USED(vertexCapacity);
        if (m_isDisposed)
        {
          throw UsageErrorException("Disposed");
        }
        return MeshHandle(m_meshes.Add(true));
      }

      MeshHandle MeshManager::CreateMesh(const std::shared_ptr<ISprite>& sprite)
      {
        FSL_PARAM_NOT_USED(sprite);
        if (m_isDisposed)
        {
          throw UsageErrorException("Disposed");
        }
        return MeshHandle(m_meshes.Add(true));
      }

      MeshHandle MeshManager::CreateMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t vertexCapacity, const uint32_t indexCapacity)
      {
        FSL_PARAM_NOT_USED(sprite);
        FSL_PARAM_NOT_USED(vertexCapacity);
        FSL_PARAM_NOT_USED(indexCapacity);
        if (m_isDisposed)
        {
          throw UsageErrorException("Disposed");
        }
        return MeshHandle(m_meshes.Add(true));
      }

      MeshHandle MeshManager::CreateMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t spriteMaterialIndex)
      {
        FSL_PARAM_NOT_USED(sprite);
        FSL_PARAM_NOT_USED(spriteMaterialIndex);
        if (m_isDisposed)
        {
          throw UsageErrorException("Disposed");
        }
        return MeshHandle(m_meshes.Add(true));
      }

      MeshHandle MeshManager::CreateMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t spriteMaterialIndex, const uint32_t vertexCapacity,
                                         const uint32_t indexCapacity)
      {
        FSL_PARAM_NOT_USED(sprite);
        FSL_PARAM_NOT_USED(spriteMaterialIndex);
        FSL_PARAM_NOT_USED(vertexCapacity);
        FSL_PARAM_NOT_USED(indexCapacity);
        if (m_isDisposed)
        {
          throw UsageErrorException("Disposed");
        }
        return MeshHandle(m_meshes.Add(true));
      }

      MeshHandle MeshManager::CreateMesh(const std::shared_ptr<SpriteFont>& sprite)
      {
        FSL_PARAM_NOT_USED(sprite);
        if (m_isDisposed)
        {
          throw UsageErrorException("Disposed");
        }
        return MeshHandle(m_meshes.Add(true));
      }

      MeshHandle MeshManager::CreateMesh(const std::shared_ptr<SpriteFont>& sprite, const uint32_t vertexCapacity, const uint32_t indexCapacity)
      {
        FSL_PARAM_NOT_USED(sprite);
        FSL_PARAM_NOT_USED(vertexCapacity);
        FSL_PARAM_NOT_USED(indexCapacity);
        if (m_isDisposed)
        {
          throw UsageErrorException("Disposed");
        }
        return MeshHandle(m_meshes.Add(true));
      }

      bool MeshManager::DestroyMesh(const MeshHandle hMesh) noexcept
      {
        assert(m_isDisposed);
        return !m_isDisposed ? m_meshes.Remove(hMesh.Value) : false;
      }

      MeshHandle MeshManager::SetBasicMeshSprite(const MeshHandle hMesh, const std::shared_ptr<ISprite>& sprite)
      {
        FSL_PARAM_NOT_USED(sprite);
        if (m_isDisposed)
        {
          throw UsageErrorException("Disposed");
        }
        m_meshes.Get(hMesh.Value);
        return hMesh;
      }

      MeshHandle MeshManager::SetMeshSprite(const MeshHandle hMesh, const std::shared_ptr<ISprite>& sprite)
      {
        FSL_PARAM_NOT_USED(sprite);
        if (m_isDisposed)
        {
          throw UsageErrorException("Disposed");
        }
        m_meshes.Get(hMesh.Value);
        return hMesh;
      }

      MeshHandle MeshManager::SetMeshSprite(const MeshHandle hMesh, const std::shared_ptr<SpriteFont>& sprite)
      {
        FSL_PARAM_NOT_USED(sprite);
        if (m_isDisposed)
        {
          throw UsageErrorException("Disposed");
        }
        m_meshes.Get(hMesh.Value);
        return hMesh;
      }

      MeshHandle MeshManager::SetMeshText(const MeshHandle hMesh, const StringViewLite& text)
      {
        FSL_PARAM_NOT_USED(text);
        if (m_isDisposed)
        {
          throw UsageErrorException("Disposed");
        }
        m_meshes.Get(hMesh.Value);
        return hMesh;
      }


      void MeshManager::EnsureCapacity(const MeshHandle hMesh, const uint32_t vertexCapacity, const uint32_t indexCapacity)
      {
        FSL_PARAM_NOT_USED(vertexCapacity);
        FSL_PARAM_NOT_USED(indexCapacity);
        if (m_isDisposed)
        {
          throw UsageErrorException("Disposed");
        }
        m_meshes.Get(hMesh.Value);
      }

    }
  }
}
