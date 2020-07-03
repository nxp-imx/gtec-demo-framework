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

#include <FslUtil/Vulkan1_0/Native/DynamicNativeTexture2DLink.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <algorithm>
#include <utility>
#include "NativeTexture2DHelper.hpp"

namespace Fsl
{
  class RawTexture;

  namespace Vulkan
  {
    DynamicNativeTexture2DLink::DynamicNativeTexture2DLink(const std::shared_ptr<VulkanImageCreator>& imageCreator, const RawTexture& texture,
                                                           const Texture2DFilterHint filterHint, const TextureFlags textureFlags,
                                                           const uint32_t capacity)
      : m_imageCreator(imageCreator)
      , m_textures(std::max(capacity, 2u))
    {
      if (!imageCreator)
      {
        throw std::invalid_argument("imageCreator can not be null");
      }

      NativeTexture2DHelper::CheckTextureFlags(textureFlags);
      auto samplerCreateInfo = NativeTexture2DHelper::Convert(filterHint);

      m_textures.front().Texture = imageCreator->CreateTexture(texture, samplerCreateInfo);
      m_textures.front().IsInUse = true;
    }


    DynamicNativeTexture2DLink::~DynamicNativeTexture2DLink() = default;


    void DynamicNativeTexture2DLink::OnVulkanSwapchainEvent(const VulkanSwapchainEvent theEvent)
    {
      switch (theEvent)
      {
      case VulkanSwapchainEvent::Lost:
        m_swapchainValid = false;
        break;
      case VulkanSwapchainEvent::Recreated:
        m_swapchainValid = true;
        break;
      default:
        FSLLOG3_WARNING("Event not supported: {}", static_cast<int>(theEvent));
        break;
      }
    }


    void DynamicNativeTexture2DLink::EnsureCapacity(const uint32_t minCapacity)
    {
      auto actualCapacity = minCapacity + 1;
      if (m_textures.size() < actualCapacity)
      {
        m_textures.resize(actualCapacity);
      }
    }


    void DynamicNativeTexture2DLink::CollectGarbage()
    {
      for (std::size_t i = 0; i < m_textures.size(); ++i)
      {
        auto& rEntry = m_textures[i];
        if (rEntry.DeferredFree)
        {
          if (rEntry.DeferCount <= 1u)
          {
            // Make the texture available for reuse
            rEntry.IsInUse = false;
            rEntry.DeferredFree = false;
            rEntry.DeferCount = 0u;
            FSLLOG3_VERBOSE5("DynamicNativeTexture2DLink: InternalTexture at #{} ready for reuse", i);
          }
          else
          {
            --rEntry.DeferCount;
          }
        }
      }
    }

    // two buffers
    // 1234
    // A
    // AB
    // ABC
    //  BCA
    //   CAB
    //    ABC


    void DynamicNativeTexture2DLink::SetData(const RawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags)
    {
      if (m_isDestroyed)
      {
        FSLLOG3_ERROR("Can not SetData on a destroyed texture");
        return;
      }
      NativeTexture2DHelper::CheckTextureFlags(textureFlags);

      if (!m_swapchainValid)
      {
        if (m_activeTextureIndex > m_textures.size())
        {
          throw NotFoundException("Could not find a free texture (SetData internal error)");
        }
        SetData(m_textures[m_activeTextureIndex], *m_imageCreator, texture, filterHint, textureFlags);
        return;
      }

      // 1. locate a free texture / create a free texture
      auto itrFind = std::find_if(m_textures.begin(), m_textures.end(), [](const Record& entry) { return !entry.IsInUse; });
      if (itrFind == m_textures.end())
      {
        throw NotFoundException("Could not find a free texture (SetData internal error)");
      }

      assert(!itrFind->IsInUse);
      assert(!itrFind->DeferredFree);
      assert(itrFind->DeferCount == 0u);

      // 2. set data
      SetData(*itrFind, *m_imageCreator, texture, filterHint, textureFlags);

      // 3. tag current as 'deferred free'
      auto deferCount = static_cast<uint32_t>(m_textures.size());
      m_textures[m_activeTextureIndex].DeferredFree = true;
      m_textures[m_activeTextureIndex].DeferCount = deferCount > 0u ? deferCount - 1u : 1u;

      // 4. mark new texture as being active
      itrFind->IsInUse = true;
      m_activeTextureIndex = static_cast<uint32_t>(std::distance(m_textures.begin(), itrFind));

      FSLLOG3_VERBOSE5("DynamicNativeTexture2DLink: InternalTexture at #{} marked as active", m_activeTextureIndex);
    }

    void DynamicNativeTexture2DLink::SetData(Record& rRecord, VulkanImageCreator& rImageCreator, const RawTexture& texture,
                                             const Texture2DFilterHint filterHint, const TextureFlags /*textureFlags*/)
    {
      auto samplerCreateInfo = NativeTexture2DHelper::Convert(filterHint);
      if (!rRecord.Texture.IsValid())
      {
        // The texture is not valid, so we need to allocate it
        rRecord.Texture = rImageCreator.CreateTexture(texture, samplerCreateInfo);
      }
      else
      {
        // The texture is valid, so we need to update it

        // FIX: this is extremely inefficient
        // FIX: REUSE THE TEXTURE AND CHANGE ITS CONTENT INSTEAD!!!!!!
        rRecord.Texture.Reset();
        rRecord.Texture = rImageCreator.CreateTexture(texture, samplerCreateInfo);
      }
    }

  }
}
