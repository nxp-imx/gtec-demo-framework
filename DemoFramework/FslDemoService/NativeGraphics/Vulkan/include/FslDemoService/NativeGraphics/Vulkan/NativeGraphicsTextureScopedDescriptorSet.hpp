#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_VULKAN_NATIVEGRAPHICSTEXTURESCOPEDDESCRIPTORSET_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_VULKAN_NATIVEGRAPHICSTEXTURESCOPEDDESCRIPTORSET_HPP
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

#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsDescriptorSetManager.hpp>

namespace Fsl::Vulkan
{
  struct NativeGraphicsTextureScopedDescriptorSet
  {
    // NOLINTNEXTLINE(readability-identifier-naming)
    NativeGraphicsDescriptorSetManager* m_pManager{nullptr};
    // NOLINTNEXTLINE(readability-identifier-naming)
    VkDescriptorSet m_descriptorSet{VK_NULL_HANDLE};

    NativeGraphicsTextureScopedDescriptorSet() noexcept = default;

    NativeGraphicsTextureScopedDescriptorSet(const NativeGraphicsTextureScopedDescriptorSet&) = delete;
    NativeGraphicsTextureScopedDescriptorSet& operator=(const NativeGraphicsTextureScopedDescriptorSet&) = delete;

    NativeGraphicsTextureScopedDescriptorSet(NativeGraphicsTextureScopedDescriptorSet&& other) noexcept
      : m_pManager(other.m_pManager)
      , m_descriptorSet(other.m_descriptorSet)
    {
      other.m_pManager = nullptr;
      other.m_descriptorSet = VK_NULL_HANDLE;
    }

    NativeGraphicsTextureScopedDescriptorSet& operator=(NativeGraphicsTextureScopedDescriptorSet&& other) noexcept
    {
      if (&other != this)
      {
        Reset();
        m_pManager = other.m_pManager;
        m_descriptorSet = other.m_descriptorSet;
        other.m_pManager = nullptr;
        other.m_descriptorSet = VK_NULL_HANDLE;
      }
      return *this;
    }

    NativeGraphicsTextureScopedDescriptorSet(NativeGraphicsDescriptorSetManager& rManager, VkDescriptorSet descriptorSet) noexcept
      : m_pManager(&rManager)
      , m_descriptorSet(descriptorSet)
    {
    }

    ~NativeGraphicsTextureScopedDescriptorSet() noexcept
    {
      Reset();
    }

    VkDescriptorSet Get() const
    {
      return m_descriptorSet;
    }

  private:
    void Reset() noexcept
    {
      if (m_pManager != nullptr && m_descriptorSet != VK_NULL_HANDLE)
      {
        m_pManager->ReleaseDescriptorSet(m_descriptorSet);
        m_descriptorSet = VK_NULL_HANDLE;
      }
    }
  };

}

#endif
