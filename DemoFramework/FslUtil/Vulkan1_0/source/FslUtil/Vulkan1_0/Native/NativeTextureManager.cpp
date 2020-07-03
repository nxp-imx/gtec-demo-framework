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

#include <FslUtil/Vulkan1_0/Native/NativeTextureManager.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/Native/DynamicNativeTexture2DLink.hpp>
#include <FslUtil/Vulkan1_0/Native/NativeTexture2DLink.hpp>
#include <algorithm>
#include <cassert>
#include <utility>
#include "NativeTexture2DHelper.hpp"

namespace Fsl
{
  namespace Vulkan
  {
    namespace
    {
      std::shared_ptr<NativeTexture2DLink> CreateLinkTexture(VulkanImageCreator& imageCreator, const RawTexture& texture,
                                                             const Texture2DFilterHint filterHint, const TextureFlags textureFlags)
      {
        NativeTexture2DHelper::CheckTextureFlags(textureFlags);
        auto samplerCreateInfo = NativeTexture2DHelper::Convert(filterHint);
        auto nativeTexture = imageCreator.CreateTexture(texture, samplerCreateInfo);
        return std::make_shared<NativeTexture2DLink>(std::move(nativeTexture));
      }
    }


    NativeTextureManager::NativeTextureManager(std::shared_ptr<VulkanImageCreator> imageCreator)
      : m_imageCreator(std::move(imageCreator))
      , m_commandBufferCount(0u)
    {
    }


    NativeTextureManager::~NativeTextureManager()
    {
      // Force free all textures
      ForceFreeAllTextures();
      FSLLOG3_WARNING_IF(!m_nativeTextures.empty(), "NativeTextureManager: There are still {} NativeTexture2D objects allocated",
                         m_nativeTextures.size());
      FSLLOG3_WARNING_IF(!m_dynamicNativeTextures.empty(), "NativeTextureManager: There are still {} DynamicNativeTexture2D objects allocated",
                         m_dynamicNativeTextures.size());
    }


    void NativeTextureManager::VulkanCreateDependentResources(const uint32_t commandBufferCount)
    {
      FSLLOG3_VERBOSE5("NativeTextureManager::VulkanCreateDependentResources");
      m_commandBufferCount = commandBufferCount;
      CollectGarbage();
      for (auto& rEntry : m_dynamicNativeTextures)
      {
        assert(rEntry.LinkTexture);
        rEntry.LinkTexture->EnsureCapacity(commandBufferCount);
      }
    }

    void NativeTextureManager::VulkanDestroyDependentResources() noexcept
    {
      FSLLOG3_VERBOSE5("NativeTextureManager::VulkanDestroyDependentResources");
      m_commandBufferCount = 0;
      CollectGarbage();
    }


    void NativeTextureManager::OnVulkanSwapchainEvent(const VulkanSwapchainEvent theEvent)
    {
      FSLLOG3_VERBOSE5("NativeTextureManager::OnVulkanSwapchainEvent({})", static_cast<int>(theEvent));
      switch (theEvent)
      {
      case VulkanSwapchainEvent::Lost:
        // We know the device is idle when this occurs so we can just force free everything
        CollectGarbage(true);
        break;
      case VulkanSwapchainEvent::Recreated:
        break;
      default:
        FSLLOG3_WARNING("Event not supported: {}", static_cast<int>(theEvent));
        break;
      }

      for (auto& rEntry : m_dynamicNativeTextures)
      {
        assert(rEntry.LinkTexture);
        rEntry.LinkTexture->OnVulkanSwapchainEvent(theEvent);
      }
    }


    std::shared_ptr<INativeTexture2D> NativeTextureManager::CreateTexture2D(const RawTexture& texture, const Texture2DFilterHint filterHint,
                                                                            const TextureFlags textureFlags)
    {
      assert(m_imageCreator);

      auto linkTexture = CreateLinkTexture(*m_imageCreator, texture, filterHint, textureFlags);

      auto nativeTexture = std::make_shared<NativeTexture2D>(texture.GetExtent(), linkTexture);
      m_nativeTextures.emplace_back(nativeTexture, linkTexture, m_commandBufferCount);

      FSLLOG3_VERBOSE5("NativeTextureManager: CreateNativeTexture2D ({}) count: {}", reinterpret_cast<intptr_t>(linkTexture.get()),
                       m_nativeTextures.size());
      return nativeTexture;
    }


    std::shared_ptr<IDynamicNativeTexture2D>
      NativeTextureManager::CreateDynamicTexture2D(const RawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags)
    {
      assert(m_imageCreator);

      auto linkTexture = std::make_shared<DynamicNativeTexture2DLink>(m_imageCreator, texture, filterHint, textureFlags, m_commandBufferCount);
      auto nativeTexture = std::make_shared<DynamicNativeTexture2D>(texture.GetExtent(), linkTexture);
      m_dynamicNativeTextures.emplace_back(nativeTexture, linkTexture, m_commandBufferCount);

      FSLLOG3_VERBOSE5("NativeTextureManager: CreateDynamicTexture2D ({}) count: {}", reinterpret_cast<intptr_t>(linkTexture.get()),
                       m_dynamicNativeTextures.size());
      return nativeTexture;
    }


    void NativeTextureManager::CollectGarbage(const bool force)
    {
      bool freedTexture = false;
      {
        auto itr = m_nativeTextures.begin();
        while (itr != m_nativeTextures.end())
        {
          if (!itr->NativeTexture.expired())
          {
            itr->DeferCount = m_commandBufferCount;
            ++itr;
          }
          else if (!force && itr->DeferCount > 1u)
          {
            FSLLOG3_VERBOSE5("Deferring texture destruction ({}) numFrames {}", reinterpret_cast<intptr_t>(itr->LinkTexture.get()),
                             (itr->DeferCount - 1));
            --itr->DeferCount;
            ++itr;
          }
          else
          {
            FSLLOG3_VERBOSE5("Destroying texture ({})", reinterpret_cast<intptr_t>(itr->LinkTexture.get()));
            itr->LinkTexture->Destroy();
            itr = m_nativeTextures.erase(itr);
            freedTexture = true;
          }
        }
      }
      {
        auto itr = m_dynamicNativeTextures.begin();
        while (itr != m_dynamicNativeTextures.end())
        {
          if (!itr->NativeTexture.expired())
          {
            itr->LinkTexture->CollectGarbage();
            itr->DeferCount = m_commandBufferCount;
            ++itr;
          }
          else if (!force && itr->DeferCount > 0)
          {
            FSLLOG3_VERBOSE5("Deferring dynamic texture destruction ({}) numFrames {}", reinterpret_cast<intptr_t>(itr->LinkTexture.get()),
                             itr->DeferCount);
            --itr->DeferCount;
            ++itr;
          }
          else
          {
            FSLLOG3_VERBOSE5("Destroying texture ({})", reinterpret_cast<intptr_t>(itr->LinkTexture.get()));
            itr->LinkTexture->Destroy();
            itr = m_dynamicNativeTextures.erase(itr);
            freedTexture = true;
          }
        }
      }

      FSLLOG3_VERBOSE4_IF(freedTexture, "NativeTextureManager: After GC {} normal, {} dynamic", m_nativeTextures.size(),
                          m_dynamicNativeTextures.size());
    }

    void NativeTextureManager::ForceFreeAllTextures()
    {
      FSLLOG3_VERBOSE5("NativeTextureManager: ForceFreeAllTextures begin {} normal, {} dynamic", m_nativeTextures.size(),
                       m_dynamicNativeTextures.size());
      {
        auto itr = m_nativeTextures.begin();
        while (itr != m_nativeTextures.end())
        {
          FSLLOG3_VERBOSE5("Destroying texture ({})", reinterpret_cast<intptr_t>(itr->LinkTexture.get()));
          itr->LinkTexture->Destroy();
          itr = m_nativeTextures.erase(itr);
        }
      }
      {
        auto itr = m_dynamicNativeTextures.begin();
        while (itr != m_dynamicNativeTextures.end())
        {
          itr->LinkTexture->Destroy();
          itr = m_dynamicNativeTextures.erase(itr);
        }
      }
      FSLLOG3_VERBOSE5("NativeTextureManager: ForceFreeAllTextures done {} normal, {} dynamic", m_nativeTextures.size(),
                       m_dynamicNativeTextures.size());
    }
  }
}
