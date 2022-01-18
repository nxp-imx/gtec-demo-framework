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

#include "MeshManager.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Pixel/PxAreaRectangleF.hpp>
#include <FslBase/String/StringViewLiteUtil.hpp>
#include <FslGraphics/Sprite/BasicNineSliceSprite.hpp>
#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslGraphics/Sprite/Font/SpriteFont.hpp>
#include <FslGraphics/Sprite/ISprite.hpp>
#include <FslGraphics/Sprite/ImageSprite.hpp>
#include <FslGraphics/Sprite/NineSliceSprite.hpp>
#include <FslGraphics/Sprite/OptimizedNineSliceSprite.hpp>
#include <FslGraphics2D/Procedural/Builder/UITextMeshBuilder.hpp>
#include "Log/FmtRenderDrawSpriteType.hpp"
#include "MeshManager.hpp"
#include "HandleCoding.hpp"

//#define LOCAL_SANITY_CHECK
#ifdef LOCAL_SANITY_CHECK
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define SANITY_CHECK() SanityCheck();
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define SANITY_CHECK() \
  {                    \
  }
#endif

namespace Fsl
{
  namespace UI
  {
    namespace RenderIMBatch
    {
      namespace
      {
        namespace LocalConfig
        {
          constexpr LogType Verbosity = LogType::Verbose6;

          constexpr uint32_t InitialFontScratchpadCapacity = 256;

          constexpr uint32_t SpriteMaterialIndex0 = 0u;
        }

        namespace LocalCapacity
        {
          namespace DummySprite
          {
            constexpr uint32_t MinVertexCapacity = 1;
            constexpr uint32_t MinIndexCapacity = 1;
          }
          namespace ImageSprite
          {
            constexpr uint32_t MinVertexCapacity = 4;
            constexpr uint32_t MinIndexCapacity = 6;
          }
          namespace NineSliceSprite
          {
            constexpr uint32_t MinVertexCapacity = 4 * 4;
            constexpr uint32_t MinIndexCapacity = 18 * 4;
          }
          namespace SpriteFont
          {
            constexpr const uint32_t CharCapacity = 128;
            constexpr const uint32_t VertexCapacity = 4 * CharCapacity;
            constexpr const uint32_t IndexCapacity = 6 * CharCapacity;
          }
        }

        template <typename TMeshRecord>
        void RemoveAllMeshes(MeshManager::Capacity& rCapacity, HandleVector<TMeshRecord>& rMeshes)
        {
          FSLLOG3_WARNING_IF(rMeshes.Count() > 0, "There are still {} ui meshes allocated", rMeshes.Count());
          while (rMeshes.Count() > 0u)
          {
            const uint32_t index = rMeshes.Count() - 1u;
            const auto& record = rMeshes[index];
            assert(rCapacity.VertexCapacity >= record.VertexCapacity);
            assert(rCapacity.IndexCapacity >= record.IndexCapacity);
            rCapacity.VertexCapacity -= record.VertexCapacity;
            rCapacity.IndexCapacity -= record.IndexCapacity;
            rMeshes.FastRemoveAt(index);
          }
        }

        template <typename TMeshRecord>
        void AddVertexAndIndexCount(uint32_t& rVertexCount, uint32_t& rIndexCount, const HandleVector<TMeshRecord>& meshes)
        {
          uint32_t vertexCount = 0;
          uint32_t indexCount = 0;
          for (uint32_t i = 0; i < meshes.Count(); ++i)
          {
            const auto& record = meshes[i];
            vertexCount += record.VertexCapacity;
            indexCount += record.IndexCapacity;
          }
          rVertexCount += vertexCount;
          rIndexCount += indexCount;
        }

        template <typename TMeshRecord>
        bool DoDestroyMesh(MeshManager::Capacity& rCapacity, HandleVector<TMeshRecord>& rMeshes, const MeshHandle hMesh)
        {
          const int32_t hMeshValue = HandleCoding::GetOriginalHandle(hMesh);
          const bool found = rMeshes.IsValidHandle(hMeshValue);
          if (found)
          {
            auto index = rMeshes.FastHandleToIndex(hMeshValue);
            const auto& record = rMeshes[index];
            assert(rCapacity.VertexCapacity >= record.VertexCapacity);
            assert(rCapacity.IndexCapacity >= record.IndexCapacity);
            rCapacity.VertexCapacity -= record.VertexCapacity;
            rCapacity.IndexCapacity -= record.IndexCapacity;
            rMeshes.FastRemoveAt(index);
          }
          return found;
        }

        void UpdateConfiguration(MaterialLookup& rMaterialLookup, HandleVector<MeshManager::OptimizedNineSliceSpriteMeshRecord>& rMeshes)
        {
          // This update configuration is only intended to handle meshes with sprite material index 0
          const uint32_t count = rMeshes.Count();
          for (uint32_t i = 0; i < count; ++i)
          {
            auto& rRecord = rMeshes[i];
            const ISprite* const pSprite = rRecord.Sprite.get();
            // Try to acquire the batch material handle for the given sprite and compare it to the one we stored,
            // if it is different we release the old one and acquire the new one
            const uint32_t materialCount = rRecord.GetMaterialCount();
            for (uint32_t materialIndex = 0; materialIndex <= materialCount; ++materialIndex)
            {
              const auto materialInfo = rRecord.GetMaterialInfo(materialIndex);
              const BatchMaterialHandle materialHandle = rMaterialLookup.TryGetHandle(pSprite, materialInfo.SpriteMaterialIndex);
              if (materialHandle != materialInfo.MaterialHandle)
              {
                rMaterialLookup.Release(materialInfo.MaterialHandle);
                rRecord.SetMaterialHandle(materialIndex, rMaterialLookup.Acquire(pSprite, materialInfo.SpriteMaterialIndex));
              }
              else
              {
                rMaterialLookup.SetSpriteMaterialInfo(materialHandle, pSprite->GetMaterialInfo(materialInfo.SpriteMaterialIndex));
              }
            }
          }
        }

        template <typename TMeshRecord>
        void UpdateConfiguration(MaterialLookup& rMaterialLookup, HandleVector<TMeshRecord>& rMeshes)
        {
          // This update configuration is only intended to handle meshes with sprite material index 0
          const uint32_t count = rMeshes.Count();
          for (uint32_t i = 0; i < count; ++i)
          {
            TMeshRecord& rRecord = rMeshes[i];
            const ISprite* const pSprite = rRecord.Sprite.get();
            // Try to acquire the batch material handle for the given sprite and compare it to the one we stored,
            // if it is different we release the old one and acquire the new one
            const BatchMaterialHandle batchMaterialHandle = rMaterialLookup.TryGetHandle(pSprite, rRecord.SpriteMaterialIndex);
            if (batchMaterialHandle != rRecord.MaterialHandle)
            {
              rMaterialLookup.Release(rRecord.MaterialHandle);
              rRecord.MaterialHandle = rMaterialLookup.Acquire(pSprite, rRecord.SpriteMaterialIndex);
            }
            else
            {
              rMaterialLookup.SetSpriteMaterialInfo(batchMaterialHandle, pSprite->GetMaterialInfo(rRecord.SpriteMaterialIndex));
            }
          }
        }

        template <typename TMeshRecord>
        void DoEnsureCapacity(MeshManager::Capacity& rCapacity, HandleVector<TMeshRecord>& rMeshes, const MeshHandle hMesh,
                              const uint32_t vertexCapacity, const uint32_t indexCapacity)
        {
          auto& rRecord = rMeshes.Get(HandleCoding::GetOriginalHandle(hMesh));
          if ((rRecord.IndexCapacity > 0 || indexCapacity > 0) && vertexCapacity > 0xFFFF)
          {
            throw NotSupportedException("vertex capacity exceeded");
          }

          if (vertexCapacity > rRecord.VertexCapacity)
          {
            rCapacity.VertexCapacity -= rRecord.VertexCapacity;
            rCapacity.VertexCapacity += vertexCapacity;
            rRecord.VertexCapacity = vertexCapacity;
          }
          if (indexCapacity > rRecord.IndexCapacity)
          {
            rCapacity.IndexCapacity -= rRecord.IndexCapacity;
            rCapacity.IndexCapacity += indexCapacity;
            rRecord.IndexCapacity = indexCapacity;
          }
        }

      }

      MeshManager::MeshManager(const SpriteMaterialInfo& defaultMaterialInfo)
        : m_materialLookup(defaultMaterialInfo)
        , m_textMeshBuilder(std::make_shared<UITextMeshBuilder>(LocalConfig::InitialFontScratchpadCapacity))
      {
      }

      MeshManager::~MeshManager()
      {
        SANITY_CHECK();
        RemoveAllMeshes(m_capacity, m_meshesDummy);
        SANITY_CHECK();
        RemoveAllMeshes(m_capacity, m_meshesBasicImageSprite);
        SANITY_CHECK();
        RemoveAllMeshes(m_capacity, m_meshesBasicNineSliceSprite);
        SANITY_CHECK();
        RemoveAllMeshes(m_capacity, m_meshesImageSprite);
        SANITY_CHECK();
        RemoveAllMeshes(m_capacity, m_meshesNineSliceSprite);
        SANITY_CHECK();
        RemoveAllMeshes(m_capacity, m_meshesOptimizedNineSliceSprite);
        SANITY_CHECK();
        RemoveAllMeshes(m_capacity, m_meshesSpriteFont);
        SANITY_CHECK();
        assert(m_capacity.VertexCapacity == 0);
        assert(m_capacity.IndexCapacity == 0);
      }


      void MeshManager::OnConfigurationChanged()
      {
        // The configuration changed which means that the sprites materials could have been reconfigured.
        UpdateConfiguration(m_materialLookup, m_meshesDummy);
        UpdateConfiguration(m_materialLookup, m_meshesBasicImageSprite);
        UpdateConfiguration(m_materialLookup, m_meshesBasicNineSliceSprite);
        UpdateConfiguration(m_materialLookup, m_meshesImageSprite);
        UpdateConfiguration(m_materialLookup, m_meshesNineSliceSprite);
        UpdateConfiguration(m_materialLookup, m_meshesOptimizedNineSliceSprite);
        UpdateConfiguration(m_materialLookup, m_meshesSpriteFont);
      }


      MeshHandle MeshManager::CreateBasicMesh(const std::shared_ptr<ISprite>& sprite)
      {
        return CreateBasicMesh(sprite, 0);
      }

      MeshHandle MeshManager::CreateBasicMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t vertexCapacity)
      {
        if (!sprite)
        {
          throw std::invalid_argument("sprite can not be null");
        }

        const uint32_t spriteMaterialIndex = LocalConfig::SpriteMaterialIndex0;
        const BatchMaterialHandle batchMaterialHandle = m_materialLookup.Acquire(sprite.get(), spriteMaterialIndex);
        try
        {
          SANITY_CHECK();
          const AddMeshResult result = AddSpriteBasicMesh(sprite, batchMaterialHandle, spriteMaterialIndex,
                                                          m_materialLookup.GetSpriteMaterialInfo(batchMaterialHandle).IsOpaque, vertexCapacity);
          assert(result.IndexCapacity == 0u);
          FSLLOG3(LocalConfig::Verbosity, "MeshManager::CreateBasicMesh() MeshHandle:{} DrawSpriteType:{}", result.MeshValue, result.DrawSpriteType);
          m_capacity.VertexCapacity += result.VertexCapacity;

          SANITY_CHECK();
          return HandleCoding::EncodeHandle(result.DrawSpriteType, result.MeshValue);
        }
        catch (const std::exception& ex)
        {
          m_materialLookup.Release(batchMaterialHandle);
          FSLLOG3_ERROR("Failed to create basic mesh record: {}", ex.what());
          throw;
        }
      }

      MeshHandle MeshManager::CreateMesh(const std::shared_ptr<ISprite>& sprite)
      {
        return CreateMesh(sprite, LocalConfig::SpriteMaterialIndex0, 0u, 0u);
      }

      MeshHandle MeshManager::CreateMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t vertexCapacity, const uint32_t indexCapacity)
      {
        return CreateMesh(sprite, LocalConfig::SpriteMaterialIndex0, vertexCapacity, indexCapacity);
      }


      MeshHandle MeshManager::CreateMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t spriteMaterialIndex)
      {
        return CreateMesh(sprite, spriteMaterialIndex, 0, 0);
      }

      MeshHandle MeshManager::CreateMesh(const std::shared_ptr<ISprite>& sprite, const uint32_t spriteMaterialIndex, const uint32_t vertexCapacity,
                                         const uint32_t indexCapacity)
      {
        if (!sprite)
        {
          throw std::invalid_argument("sprite can not be null");
        }
        if (vertexCapacity > 0xFFFF)
        {
          throw std::invalid_argument("vertexCapacity can not be greater than 0xFFFF");
        }

        const BatchMaterialHandle batchMaterialHandle = m_materialLookup.Acquire(sprite.get(), spriteMaterialIndex);
        try
        {
          SANITY_CHECK();
          const AddMeshResult result =
            AddSpriteMesh(sprite, batchMaterialHandle, spriteMaterialIndex, m_materialLookup.GetSpriteMaterialInfo(batchMaterialHandle).IsOpaque,
                          vertexCapacity, indexCapacity);
          FSLLOG3(LocalConfig::Verbosity, "MeshManager::CreateMesh() MeshHandle:{} DrawSpriteType:{}", result.MeshValue, result.DrawSpriteType);
          m_capacity.VertexCapacity += result.VertexCapacity;
          m_capacity.IndexCapacity += result.IndexCapacity;
          SANITY_CHECK();
          return HandleCoding::EncodeHandle(result.DrawSpriteType, result.MeshValue);
        }
        catch (const std::exception& ex)
        {
          m_materialLookup.Release(batchMaterialHandle);
          FSLLOG3_ERROR("Failed to create mesh record: {}", ex.what());
          throw;
        }
      }


      MeshHandle MeshManager::CreateMesh(const std::shared_ptr<SpriteFont>& sprite)
      {
        return CreateMesh(sprite, LocalCapacity::SpriteFont::VertexCapacity, LocalCapacity::SpriteFont::IndexCapacity);
      }


      MeshHandle MeshManager::CreateMesh(const std::shared_ptr<SpriteFont>& sprite, const uint32_t vertexCapacity, const uint32_t indexCapacity)
      {
        if (!sprite)
        {
          throw std::invalid_argument("sprite can not be null");
        }

        const uint32_t spriteMaterialIndex = LocalConfig::SpriteMaterialIndex0;
        const BatchMaterialHandle batchMaterialHandle = m_materialLookup.Acquire(sprite.get(), spriteMaterialIndex);
        try
        {
          SANITY_CHECK();

          const uint32_t finalVertexCapacity = std::max(LocalCapacity::SpriteFont::VertexCapacity, vertexCapacity);
          const uint32_t finalIndexCapacity = std::max(LocalCapacity::SpriteFont::IndexCapacity, indexCapacity);

          const auto hMeshValue = m_meshesSpriteFont.Add(SpriteFontMeshRecord(sprite, batchMaterialHandle, spriteMaterialIndex,
                                                                              m_materialLookup.GetSpriteMaterialInfo(batchMaterialHandle).IsOpaque,
                                                                              finalVertexCapacity, finalIndexCapacity));

          FSLLOG3(LocalConfig::Verbosity, "MeshManager::CreateMesh() MeshHandle:{} SpriteFont", hMeshValue);
          m_capacity.VertexCapacity += finalVertexCapacity;
          m_capacity.IndexCapacity += finalIndexCapacity;
          SANITY_CHECK();
          return HandleCoding::EncodeHandle(RenderDrawSpriteType::SpriteFont, hMeshValue);
        }
        catch (const std::exception& ex)
        {
          m_materialLookup.Release(batchMaterialHandle);
          FSLLOG3_ERROR("Failed to create mesh record: {}", ex.what());
          throw;
        }
      }


      bool MeshManager::DestroyMesh(const MeshHandle hMesh) noexcept
      {
        FSLLOG3(LocalConfig::Verbosity, "MeshManager::DestroyMesh({})", hMesh.Value);
        SANITY_CHECK();
        bool found = false;
        switch (HandleCoding::GetType(hMesh))
        {
        case RenderDrawSpriteType::Dummy:
          found = DoDestroyMesh(m_capacity, m_meshesDummy, hMesh);
          break;
        case RenderDrawSpriteType::BasicImageSprite:
          found = DoDestroyMesh(m_capacity, m_meshesBasicImageSprite, hMesh);
          break;
        case RenderDrawSpriteType::BasicNineSliceSprite:
          found = DoDestroyMesh(m_capacity, m_meshesBasicNineSliceSprite, hMesh);
          break;
        case RenderDrawSpriteType::ImageSprite:
          found = DoDestroyMesh(m_capacity, m_meshesImageSprite, hMesh);
          break;
        case RenderDrawSpriteType::NineSliceSprite:
          found = DoDestroyMesh(m_capacity, m_meshesNineSliceSprite, hMesh);
          break;
        case RenderDrawSpriteType::OptimizedNineSliceSprite:
          found = DoDestroyMesh(m_capacity, m_meshesOptimizedNineSliceSprite, hMesh);
          break;
        case RenderDrawSpriteType::SpriteFont:
          found = DoDestroyMesh(m_capacity, m_meshesSpriteFont, hMesh);
          break;
        default:
          FSLLOG3_ERROR("Handle has unknown/unsupported mesh type");
          break;
        }
        SANITY_CHECK();
        return found;
      }

      MeshHandle MeshManager::SetBasicMeshSprite(const MeshHandle hMesh, const std::shared_ptr<ISprite>& sprite)
      {
        FSLLOG3(LocalConfig::Verbosity, "MeshManager::SetMeshSprite({})", hMesh.Value);
        if (!sprite)
        {
          throw std::invalid_argument("sprite can not be null");
        }
        SANITY_CHECK();

        // The simple implementation is to just destroy the old mesh and create a new one
        DestroyMesh(hMesh);
        return CreateMesh(sprite);
      }


      MeshHandle MeshManager::SetMeshSprite(const MeshHandle hMesh, const std::shared_ptr<ISprite>& sprite)
      {
        FSLLOG3(LocalConfig::Verbosity, "MeshManager::SetMeshSprite({})", hMesh.Value);
        if (!sprite)
        {
          throw std::invalid_argument("sprite can not be null");
        }
        SANITY_CHECK();
        // The simple implementation is to just destroy the old mesh and create a new one
        DestroyMesh(hMesh);
        return CreateMesh(sprite);
      }

      MeshHandle MeshManager::SetMeshSprite(const MeshHandle hMesh, const std::shared_ptr<SpriteFont>& sprite)
      {
        FSLLOG3(LocalConfig::Verbosity, "MeshManager::SetMeshSpriteFont({})", hMesh.Value);
        if (!sprite)
        {
          throw std::invalid_argument("font can not be null");
        }
        const RenderDrawSpriteType drawSpriteType = HandleCoding::GetType(hMesh);
        if (drawSpriteType != RenderDrawSpriteType::SpriteFont)
        {
          throw std::invalid_argument("Mesh does not represent a sprite font");
        }
        SANITY_CHECK();

        // Patch the mesh record
        SpriteFontMeshRecord& rMeshRecord = m_meshesSpriteFont.Get(HandleCoding::GetOriginalHandle(hMesh));

        m_materialLookup.Release(rMeshRecord.MaterialHandle);
        rMeshRecord.MaterialHandle = m_materialLookup.Acquire(sprite.get(), rMeshRecord.SpriteMaterialIndex);
        rMeshRecord.Sprite = sprite;

        assert(m_capacity.VertexCapacity >= rMeshRecord.VertexCapacity);
        assert(m_capacity.IndexCapacity >= rMeshRecord.IndexCapacity);

        // Since we just change between two font materials the required vertex and index capacity should not change!
        SANITY_CHECK();
        return hMesh;
      }


      MeshHandle MeshManager::SetMeshText(const MeshHandle hMesh, const StringViewLite& text)
      {
        SANITY_CHECK();

        const RenderDrawSpriteType drawSpriteType = HandleCoding::GetType(hMesh);
        if (drawSpriteType != RenderDrawSpriteType::SpriteFont)
        {
          throw std::invalid_argument("Mesh does not represent a sprite font");
        }
        SpriteFontMeshRecord& rMeshRecord = m_meshesSpriteFont.Get(HandleCoding::GetOriginalHandle(hMesh));
        StringViewLiteUtil::Set(rMeshRecord.Text, text);
        SANITY_CHECK();
        return hMesh;
      }


      void MeshManager::EnsureCapacity(const MeshHandle hMesh, const uint32_t vertexCapacity, const uint32_t indexCapacity)
      {
        SANITY_CHECK();
        switch (HandleCoding::GetType(hMesh))
        {
        case RenderDrawSpriteType::Dummy:
          DoEnsureCapacity(m_capacity, m_meshesDummy, hMesh, vertexCapacity, indexCapacity);
          break;
        case RenderDrawSpriteType::BasicImageSprite:
          DoEnsureCapacity(m_capacity, m_meshesBasicImageSprite, hMesh, vertexCapacity, indexCapacity);
          break;
        case RenderDrawSpriteType::BasicNineSliceSprite:
          DoEnsureCapacity(m_capacity, m_meshesBasicNineSliceSprite, hMesh, vertexCapacity, indexCapacity);
          break;
        case RenderDrawSpriteType::ImageSprite:
          DoEnsureCapacity(m_capacity, m_meshesImageSprite, hMesh, vertexCapacity, indexCapacity);
          break;
        case RenderDrawSpriteType::NineSliceSprite:
          DoEnsureCapacity(m_capacity, m_meshesNineSliceSprite, hMesh, vertexCapacity, indexCapacity);
          break;
        case RenderDrawSpriteType::OptimizedNineSliceSprite:
          DoEnsureCapacity(m_capacity, m_meshesOptimizedNineSliceSprite, hMesh, vertexCapacity, indexCapacity);
          break;
        case RenderDrawSpriteType::SpriteFont:
          DoEnsureCapacity(m_capacity, m_meshesSpriteFont, hMesh, vertexCapacity, indexCapacity);
          break;
        default:
          FSLLOG3_ERROR("Handle has unknown/unsupported mesh type");
          break;
        }
        SANITY_CHECK();
      }


      void MeshManager::PreDraw()
      {
        SANITY_CHECK();
      }

      MeshManager::AddMeshResult MeshManager::AddSpriteBasicMesh(const std::shared_ptr<ISprite>& sprite,
                                                                 const BatchMaterialHandle batchMaterialHandle, const uint32_t spriteMaterialIndex,
                                                                 const bool isOpaque, const uint32_t vertexCapacity)
      {
        {
          auto spriteEx = std::dynamic_pointer_cast<BasicImageSprite>(sprite);
          if (spriteEx)
          {
            const uint32_t finalVertexCapacity = std::max(LocalCapacity::ImageSprite::MinVertexCapacity, vertexCapacity);
            const auto hMeshValue = m_meshesBasicImageSprite.Add(
              BasicImageSpriteMeshRecord(std::move(spriteEx), batchMaterialHandle, spriteMaterialIndex, isOpaque, finalVertexCapacity, 0));
            return {RenderDrawSpriteType::BasicImageSprite, finalVertexCapacity, 0u, hMeshValue};
          }
        }
        // Nothing else matched, so we fall-back to a dummy render record
        {
          FSLLOG3_WARNING("Unsupported sprite type, using dummy render");
          const uint32_t finalVertexCapacity = std::max(LocalCapacity::DummySprite::MinVertexCapacity, vertexCapacity);
          const auto hMeshValue =
            m_meshesDummy.Add(DummySpriteMeshRecord(sprite, batchMaterialHandle, spriteMaterialIndex, isOpaque, finalVertexCapacity, 0));
          return {RenderDrawSpriteType::Dummy, finalVertexCapacity, 0u, hMeshValue};
        }
      }


      MeshManager::AddMeshResult MeshManager::AddSpriteMesh(const std::shared_ptr<ISprite>& sprite, const BatchMaterialHandle batchMaterialHandle,
                                                            const uint32_t spriteMaterialIndex, const bool isOpaque, const uint32_t vertexCapacity,
                                                            const uint32_t indexCapacity)
      {
        assert(sprite);
        {
          auto spriteEx = std::dynamic_pointer_cast<ImageSprite>(sprite);
          if (spriteEx)
          {
            const uint32_t finalVertexCapacity = std::max(LocalCapacity::ImageSprite::MinVertexCapacity, vertexCapacity);
            const uint32_t finalIndexCapacity = std::max(LocalCapacity::ImageSprite::MinIndexCapacity, indexCapacity);
            const auto hMeshValue = m_meshesImageSprite.Add(ImageSpriteMeshRecord(std::move(spriteEx), batchMaterialHandle, spriteMaterialIndex,
                                                                                  isOpaque, finalVertexCapacity, finalIndexCapacity));
            return {RenderDrawSpriteType::ImageSprite, finalVertexCapacity, finalIndexCapacity, hMeshValue};
          }
        }
        {
          auto spriteEx = std::dynamic_pointer_cast<BasicImageSprite>(sprite);
          if (spriteEx)
          {
            const uint32_t finalVertexCapacity = std::max(LocalCapacity::ImageSprite::MinVertexCapacity, vertexCapacity);
            const uint32_t finalIndexCapacity = std::max(LocalCapacity::ImageSprite::MinIndexCapacity, indexCapacity);
            const auto hMeshValue = m_meshesBasicImageSprite.Add(BasicImageSpriteMeshRecord(
              std::move(spriteEx), batchMaterialHandle, spriteMaterialIndex, isOpaque, finalVertexCapacity, finalIndexCapacity));
            return {RenderDrawSpriteType::BasicImageSprite, finalVertexCapacity, finalIndexCapacity, hMeshValue};
          }
        }
        {
          auto spriteEx = std::dynamic_pointer_cast<NineSliceSprite>(sprite);
          if (spriteEx)
          {
            const uint32_t finalVertexCapacity = std::max(LocalCapacity::NineSliceSprite::MinVertexCapacity, vertexCapacity);
            const uint32_t finalIndexCapacity = std::max(LocalCapacity::NineSliceSprite::MinIndexCapacity, indexCapacity);
            const auto hMeshValue = m_meshesNineSliceSprite.Add(NineSliceSpriteMeshRecord(
              std::move(spriteEx), batchMaterialHandle, spriteMaterialIndex, isOpaque, finalVertexCapacity, finalIndexCapacity));
            return {RenderDrawSpriteType::NineSliceSprite, finalVertexCapacity, finalIndexCapacity, hMeshValue};
          }
        }
        {
          auto spriteEx = std::dynamic_pointer_cast<BasicNineSliceSprite>(sprite);
          if (spriteEx)
          {
            const uint32_t finalVertexCapacity = std::max(LocalCapacity::NineSliceSprite::MinVertexCapacity, vertexCapacity);
            const uint32_t finalIndexCapacity = std::max(LocalCapacity::NineSliceSprite::MinIndexCapacity, indexCapacity);
            const auto hMeshValue = m_meshesBasicNineSliceSprite.Add(BasicNineSliceSpriteMeshRecord(
              std::move(spriteEx), batchMaterialHandle, spriteMaterialIndex, isOpaque, finalVertexCapacity, finalIndexCapacity));
            return {RenderDrawSpriteType::BasicNineSliceSprite, finalVertexCapacity, finalIndexCapacity, hMeshValue};
          }
        }
        // Complex multi material sprites
        {
          auto spriteEx = std::dynamic_pointer_cast<OptimizedNineSliceSprite>(sprite);
          if (spriteEx)
          {
            if (spriteMaterialIndex != 0u)
            {
              throw NotSupportedException("The spriteMaterialIndex was not zero as expected");
            }
            if (sprite->GetMaterialCount() < 2u)
            {
              throw NotSupportedException("The spriteMaterialIndex was not zero as expected");
            }
            constexpr uint32_t spriteMaterialIndex1 = 1u;
            const BatchMaterialHandle batchMaterialHandle1 = m_materialLookup.Acquire(sprite.get(), spriteMaterialIndex1);
            try
            {
              const auto transparencyFlags = AtlasNineSliceFlagsUtil::GetTransparencyFlags(spriteEx->GetRenderInfo().Flags);
              const MeshTransparencyFlags hasOpaqueParts =
                transparencyFlags != AtlasNineSliceFlags::Transparent ? MeshTransparencyFlags::Opaque : MeshTransparencyFlags::NoFlags;
              const MeshTransparencyFlags hasTransparentParts =
                transparencyFlags != AtlasNineSliceFlags::Opaque ? MeshTransparencyFlags::Transparent : MeshTransparencyFlags::NoFlags;
              // At least one of these needs to be true
              assert(hasOpaqueParts != MeshTransparencyFlags::NoFlags || hasTransparentParts != MeshTransparencyFlags::NoFlags);
              const uint32_t minVertexCapacity =
                (hasOpaqueParts != MeshTransparencyFlags::NoFlags ? LocalCapacity::NineSliceSprite::MinVertexCapacity : 0) +
                (hasTransparentParts != MeshTransparencyFlags::NoFlags ? LocalCapacity::NineSliceSprite::MinVertexCapacity : 0);
              const uint32_t finalVertexCapacity = std::max(minVertexCapacity, vertexCapacity);
              const uint32_t finalIndexCapacity = std::max(LocalCapacity::NineSliceSprite::MinIndexCapacity, indexCapacity);

              const MeshTransparencyFlags meshType = hasOpaqueParts | hasTransparentParts;

              const auto hMeshValue = m_meshesOptimizedNineSliceSprite.Add(
                OptimizedNineSliceSpriteMeshRecord(std::move(spriteEx), batchMaterialHandle, spriteMaterialIndex, batchMaterialHandle1,
                                                   spriteMaterialIndex1, finalVertexCapacity, finalIndexCapacity, meshType));
              return {RenderDrawSpriteType::OptimizedNineSliceSprite, finalVertexCapacity, finalIndexCapacity, hMeshValue};
            }
            catch (const std::exception& ex)
            {
              m_materialLookup.Release(batchMaterialHandle1);
              FSLLOG3_ERROR("Failed to create mesh record: {}", ex.what());
              throw;
            }
          }
        }


        // Nothing else matched, so we fall-back to a dummy render record
        {
          FSLLOG3_WARNING("Unsupported sprite type, using dummy render");
          const uint32_t finalVertexCapacity = std::max(LocalCapacity::DummySprite::MinVertexCapacity, vertexCapacity);
          const uint32_t finalIndexCapacity = std::max(LocalCapacity::DummySprite::MinIndexCapacity, indexCapacity);
          const auto hMeshValue = m_meshesDummy.Add(
            DummySpriteMeshRecord(sprite, batchMaterialHandle, spriteMaterialIndex, isOpaque, finalVertexCapacity, finalIndexCapacity));
          return {RenderDrawSpriteType::Dummy, finalVertexCapacity, finalIndexCapacity, hMeshValue};
        }
      }

      void MeshManager::SanityCheck() noexcept
      {
        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;
        AddVertexAndIndexCount(vertexCount, indexCount, m_meshesDummy);
        AddVertexAndIndexCount(vertexCount, indexCount, m_meshesBasicImageSprite);
        AddVertexAndIndexCount(vertexCount, indexCount, m_meshesBasicNineSliceSprite);
        AddVertexAndIndexCount(vertexCount, indexCount, m_meshesImageSprite);
        AddVertexAndIndexCount(vertexCount, indexCount, m_meshesNineSliceSprite);
        AddVertexAndIndexCount(vertexCount, indexCount, m_meshesOptimizedNineSliceSprite);
        AddVertexAndIndexCount(vertexCount, indexCount, m_meshesSpriteFont);
        assert(vertexCount == m_capacity.VertexCapacity);
        assert(indexCount == m_capacity.IndexCapacity);
      }

    }
  }
}
