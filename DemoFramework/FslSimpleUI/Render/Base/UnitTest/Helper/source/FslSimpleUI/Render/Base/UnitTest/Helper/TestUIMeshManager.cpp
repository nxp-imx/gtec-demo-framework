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

#include <FslSimpleUI/Render/Base/UnitTest/Helper/TestUIMeshManager.hpp>
#include <algorithm>

namespace Fsl::UI
{
  TestUIMeshManager::TestUIMeshManager()
    : m_entries(4096)
  {
  }


  MeshHandle TestUIMeshManager::CreateBasicMesh(const std::shared_ptr<ISprite>& sprite)
  {
    auto handle = m_entries.Add(Record(sprite, 1, 0));
    return MeshHandle(handle);
  }


  MeshHandle TestUIMeshManager::CreateBasicMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t vertexCapacity)
  {
    auto record = Record(sprite, 1, 0);
    record.VertexCapacity = vertexCapacity;
    auto handle = m_entries.Add(record);
    return MeshHandle(handle);
  }


  MeshHandle TestUIMeshManager::CreateMesh(const std::shared_ptr<ISprite>& sprite)
  {
    auto handle = m_entries.Add(Record(sprite, 1, 0));
    return MeshHandle(handle);
  }


  MeshHandle TestUIMeshManager::CreateMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t vertexCapacity, const uint32_t indexCapacity)
  {
    auto record = Record(sprite, 1, 0);
    record.VertexCapacity = vertexCapacity;
    record.IndexCapacity = indexCapacity;
    auto handle = m_entries.Add(record);
    return MeshHandle(handle);
  }


  MeshHandle TestUIMeshManager::CreateMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t spriteMaterialIndex)
  {
    auto handle = m_entries.Add(Record(sprite, 1, spriteMaterialIndex));
    return MeshHandle(handle);
  }


  MeshHandle TestUIMeshManager::CreateMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t spriteMaterialIndex, const uint32_t vertexCapacity,
                                           const uint32_t indexCapacity)
  {
    auto record = Record(sprite, 1, spriteMaterialIndex);
    record.VertexCapacity = vertexCapacity;
    record.IndexCapacity = indexCapacity;
    auto handle = m_entries.Add(record);
    return MeshHandle(handle);
  }


  MeshHandle TestUIMeshManager::CreateMesh(const std::shared_ptr<SpriteFont>& sprite)
  {
    auto record = Record(sprite, SpriteFontConfig(), "");
    auto handle = m_entries.Add(record);
    return MeshHandle(handle);
  }


  MeshHandle TestUIMeshManager::CreateMesh(const std::shared_ptr<SpriteFont>& sprite, const uint32_t vertexCapacity, const uint32_t indexCapacity)
  {
    auto record = Record(sprite, SpriteFontConfig(), "");
    record.VertexCapacity = vertexCapacity;
    record.IndexCapacity = indexCapacity;
    auto handle = m_entries.Add(record);
    return MeshHandle(handle);
  }


  bool TestUIMeshManager::DestroyMesh(const MeshHandle hMesh) noexcept
  {
    return m_entries.Remove(hMesh.Value);
  }


  MeshHandle TestUIMeshManager::SetBasicMeshSprite(const MeshHandle hMesh, const std::shared_ptr<ISprite>& sprite)
  {
    auto entry = m_entries.Get(hMesh.Value);
    if (entry.IsFontSprite)
    {
      throw UsageErrorException("Mesh not of the expected type");
    }
    entry.Sprite = sprite;

    m_entries.Remove(hMesh.Value);
    auto newHandle = m_entries.Add(entry);
    return MeshHandle(newHandle);
  }


  MeshHandle TestUIMeshManager::SetMeshSprite(const MeshHandle hMesh, const std::shared_ptr<ISprite>& sprite)
  {
    auto entry = m_entries.Get(hMesh.Value);
    if (entry.IsFontSprite)
    {
      throw UsageErrorException("Mesh not of the expected type");
    }
    entry.Sprite = sprite;

    m_entries.Remove(hMesh.Value);
    auto newHandle = m_entries.Add(entry);
    return MeshHandle(newHandle);
  }


  MeshHandle TestUIMeshManager::SetMeshSprite(const MeshHandle hMesh, const std::shared_ptr<SpriteFont>& sprite)
  {
    auto entry = m_entries.Get(hMesh.Value);
    if (!entry.IsFontSprite)
    {
      throw UsageErrorException("Mesh not of the expected type");
    }

    entry.Sprite = sprite;

    m_entries.Remove(hMesh.Value);
    auto newHandle = m_entries.Add(entry);
    return MeshHandle(newHandle);
  }


  MeshHandle TestUIMeshManager::SetMeshText(const MeshHandle hMesh, const StringViewLite& text)
  {
    auto entry = m_entries.Get(hMesh.Value);
    if (!entry.IsFontSprite)
    {
      throw UsageErrorException("Mesh not of the expected type");
    }
    entry.Text = text;
    // For testing purposes we just allocate a new mesh handle
    auto newHandle = m_entries.Add(entry);
    return MeshHandle(newHandle);
  }


  void TestUIMeshManager::EnsureCapacity(const MeshHandle hMesh, const uint32_t vertexCapacity, const uint32_t indexCapacity)
  {
    auto& rEntry = m_entries.Get(hMesh.Value);
    rEntry.VertexCapacity = std::max(rEntry.VertexCapacity, vertexCapacity);
    rEntry.IndexCapacity = std::max(rEntry.IndexCapacity, indexCapacity);
  }
}
