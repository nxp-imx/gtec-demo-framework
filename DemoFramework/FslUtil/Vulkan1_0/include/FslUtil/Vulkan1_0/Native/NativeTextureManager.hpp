#ifndef FSLUTIL_VULKAN1_0_NATIVE_NATIVETEXTUREMANAGER_HPP
#define FSLUTIL_VULKAN1_0_NATIVE_NATIVETEXTUREMANAGER_HPP
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
#include <FslGraphics/Texture/RawTexture.hpp>
#include <FslUtil/Vulkan1_0/Native/DynamicNativeTexture2D.hpp>
#include <FslUtil/Vulkan1_0/Native/NativeTexture2D.hpp>
#include <FslUtil/Vulkan1_0/Native/VulkanSwapchainEvent.hpp>
#include <memory>
#include <utility>

namespace Fsl
{
  namespace Vulkan
  {
    class VulkanImageCreator;

    //! Manage the lifetime of all native textures.
    //! This is needed to ensure that we don't destroy any native texture that might still be in use by a command queue.
    class NativeTextureManager final
    {
      struct Record
      {
        std::weak_ptr<NativeTexture2D> NativeTexture;
        std::shared_ptr<NativeTexture2DLink> LinkTexture;
        uint32_t DeferCount{0};

        Record() = default;

        explicit Record(std::weak_ptr<NativeTexture2D> nativeTexture, std::shared_ptr<NativeTexture2DLink> linkTexture, const uint32_t deferCount)
          : NativeTexture(std::move(nativeTexture))
          , LinkTexture(std::move(linkTexture))
          , DeferCount(deferCount)
        {
        }
      };
      struct DynamicRecord
      {
        std::weak_ptr<DynamicNativeTexture2D> NativeTexture;
        std::shared_ptr<DynamicNativeTexture2DLink> LinkTexture;
        uint32_t DeferCount{0};

        DynamicRecord() = default;

        explicit DynamicRecord(std::weak_ptr<DynamicNativeTexture2D> nativeTexture, std::shared_ptr<DynamicNativeTexture2DLink> linkTexture,
                               const uint32_t deferCount)
          : NativeTexture(std::move(nativeTexture))
          , LinkTexture(std::move(linkTexture))
          , DeferCount(deferCount)
        {
        }
      };

      std::shared_ptr<VulkanImageCreator> m_imageCreator;
      uint32_t m_commandBufferCount;
      std::vector<Record> m_nativeTextures;
      std::vector<DynamicRecord> m_dynamicNativeTextures;

    public:
      explicit NativeTextureManager(std::shared_ptr<VulkanImageCreator> imageCreator);
      ~NativeTextureManager();

      void VulkanCreateDependentResources(const uint32_t commandBufferCount);
      void OnVulkanSwapchainEvent(const VulkanSwapchainEvent theEvent);
      void VulkanDestroyDependentResources() noexcept;

      std::shared_ptr<INativeTexture2D> CreateTexture2D(const RawTexture& texture, const Texture2DFilterHint filterHint,
                                                        const TextureFlags textureFlags);

      std::shared_ptr<IDynamicNativeTexture2D> CreateDynamicTexture2D(const RawTexture& texture, const Texture2DFilterHint filterHint,
                                                                      const TextureFlags textureFlags);

      void CollectGarbage(const bool force = false);

    private:
      void ForceFreeAllTextures();
    };
  }
}

#endif
