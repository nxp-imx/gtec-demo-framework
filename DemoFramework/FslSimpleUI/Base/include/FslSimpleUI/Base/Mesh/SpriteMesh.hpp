#ifndef FSLSIMPLEUI_BASE_MESH_SPRITEMESH_HPP
#define FSLSIMPLEUI_BASE_MESH_SPRITEMESH_HPP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslSimpleUI/Base/ItemScalePolicy.hpp>
#include <FslSimpleUI/Base/PxAvailableSize.hpp>
#include <FslSimpleUI/Render/Base/IMeshManager.hpp>
#include <FslSimpleUI/Base/UIScaleUtil.hpp>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <utility>

namespace Fsl
{
  namespace UI
  {
    template <typename TSprite>
    class SpriteMesh
    {
      using element_type = TSprite;
      using reference = element_type&;
      using const_reference = const element_type&;

      std::weak_ptr<IMeshManager> m_meshManager;
      std::shared_ptr<element_type> m_sprite;
      MeshHandle m_hMesh;

    public:
      explicit SpriteMesh(const std::shared_ptr<IMeshManager>& meshManager)
        : m_meshManager(meshManager)
        , m_sprite()
      {
      }

      SpriteMesh(const std::shared_ptr<IMeshManager>& meshManager, const std::shared_ptr<element_type>& sprite)
        : SpriteMesh(meshManager)
      {
        SetSprite(sprite);
      }


      ~SpriteMesh() noexcept
      {
        DestroyMesh();
      }

      bool IsValid() const noexcept
      {
        return m_hMesh.IsValid();
      }


      MeshHandle Get() const noexcept
      {
        return m_hMesh;
      }

      void Set(const MeshHandle hMesh) noexcept
      {
        m_hMesh = hMesh;
      }

      const std::shared_ptr<element_type>& GetSprite() const
      {
        return m_sprite;
      }

      const_reference GetSpriteObject() const
      {
        assert(m_sprite);
        return *m_sprite;
      }


      bool SetSprite(std::shared_ptr<element_type>&& sprite)
      {
        if (sprite == m_sprite)
        {
          return false;
        }

        // Update the sprite
        m_sprite = std::move(sprite);
        if (!m_sprite)
        {
          // No sprite -> destroys the mesh (if any exist)
          DestroyMesh();
        }
        else
        {
          auto meshManager = m_meshManager.lock();
          if (!meshManager)
          {
            throw UsageErrorException("MeshManager is no longer valid");
          }

          // The sprite is valid
          if (m_hMesh.IsValid())
          {
            // We have a mesh, so we just update the sprite associate with it
            m_hMesh = meshManager->SetMeshSprite(m_hMesh, m_sprite);
          }
          else
          {
            // We do not have a mesh
            m_hMesh = meshManager->CreateMesh(m_sprite);
          }
        }
        return true;
      }

      bool SetSprite(const std::shared_ptr<element_type>& sprite)
      {
        if (sprite == m_sprite)
        {
          return false;
        }

        // Update the sprite
        m_sprite = sprite;
        if (!sprite)
        {
          // No sprite -> destroys the mesh (if any exist)
          DestroyMesh();
        }
        else
        {
          auto meshManager = m_meshManager.lock();
          if (!meshManager)
          {
            throw UsageErrorException("MeshManager is no longer valid");
          }

          // The sprite is valid
          if (m_hMesh.IsValid())
          {
            // We have a mesh, so we just update the sprite associate with it
            m_hMesh = meshManager->SetMeshSprite(m_hMesh, sprite);
          }
          else
          {
            // We do not have a mesh
            m_hMesh = meshManager->CreateMesh(sprite);
          }
        }
        return true;
      }


    protected:
      std::weak_ptr<IMeshManager> GetMeshManager() const
      {
        return m_meshManager;
      }

    private:
      void DestroyMesh() noexcept
      {
        if (m_hMesh.IsValid())
        {
          auto meshManager = m_meshManager.lock();
          if (meshManager)
          {
            meshManager->DestroyMesh(m_hMesh);
          }
          else
          {
            FSLLOG3_WARNING("MeshManager no longer valid, can not destroy mesh");
          }
          m_hMesh = MeshHandle::Invalid();
        }
      }
    };
  }
}

#endif
