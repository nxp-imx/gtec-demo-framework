#ifndef FSLUTIL_VULKAN1_0_NATIVE_DYNAMICNATIVETEXTURE2DLINK_HPP
#define FSLUTIL_VULKAN1_0_NATIVE_DYNAMICNATIVETEXTURE2DLINK_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <FslGraphics/Render/Texture2DFilterHint.hpp>
#include <FslGraphics/TextureFlags.hpp>
#include <FslUtil/Vulkan1_0/Native/VulkanSwapchainEvent.hpp>
#include <FslUtil/Vulkan1_0/VUTexture.hpp>
#include <cassert>
#include <memory>
#include <utility>
#include <vector>

namespace Fsl
{
  class RawTexture;

  namespace Vulkan
  {
    class VulkanImageCreator;

    class DynamicNativeTexture2DLink final
    {
      struct Record
      {
        VUTexture Texture;
        uint32_t DeferCount{0};
        bool IsInUse{false};
        bool DeferredFree{false};
      };

      std::shared_ptr<VulkanImageCreator> m_imageCreator;
      std::vector<Record> m_textures;
      uint32_t m_activeTextureIndex{0};
      bool m_swapchainValid{true};
      bool m_isDestroyed{false};

    public:
      DynamicNativeTexture2DLink(const std::shared_ptr<VulkanImageCreator>& imageCreator, const RawTexture& texture,
                                 const Texture2DFilterHint filterHint, const TextureFlags textureFlags, const uint32_t capacity);
      ~DynamicNativeTexture2DLink();

      void OnVulkanSwapchainEvent(const VulkanSwapchainEvent theEvent);

      void EnsureCapacity(const uint32_t minCapacity);

      void CollectGarbage();

      void Destroy()
      {
        m_textures.clear();
        m_activeTextureIndex = 0u;
        m_isDestroyed = true;
      }

      //! @brief Get the native texture handle
      VkDescriptorImageInfo GetDescriptorImageInfo() const
      {
        if (m_activeTextureIndex < m_textures.size())
        {
          assert(m_textures[m_activeTextureIndex].IsInUse);
          assert(!m_textures[m_activeTextureIndex].DeferredFree);
          assert(m_textures[m_activeTextureIndex].Texture.IsValid());
          return m_textures[m_activeTextureIndex].Texture.GetDescriptorImageInfo();
        }
        return {};
      }

      void SetData(const RawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags);

    private:
      static void SetData(Record& rRecord, VulkanImageCreator& rImageCreator, const RawTexture& texture, const Texture2DFilterHint filterHint,
                          const TextureFlags textureFlags);
    };
  }
}

#endif
