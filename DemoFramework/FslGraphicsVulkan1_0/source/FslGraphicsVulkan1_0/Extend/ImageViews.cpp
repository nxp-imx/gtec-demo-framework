/****************************************************************************************************************************************************
* Copyright (c) 2016 Freescale Semiconductor, Inc.
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
*    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslGraphicsVulkan1_0/Check.hpp>
#include <FslGraphicsVulkan1_0/Extend/ImageViews.hpp>
#include <FslGraphicsVulkan1_0/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <cassert>

namespace Fsl
{
  namespace Vulkan
  {
    ImageViews::ImageViews()
      : m_device(VK_NULL_HANDLE)
      , m_imageViews()
    {
    }


    ImageViews::ImageViews(VkDevice device, const std::size_t count)
      : m_device(VK_NULL_HANDLE)
      , m_imageViews()
    {
      Reset(device, count);
    }


    ImageViews::~ImageViews()
    {
      Reset();
    }


    VkImageView ImageViews::Get(const std::size_t arrayIndex) const
    {
      if (arrayIndex >= m_imageViews.size())
        throw std::invalid_argument("index out of bounds");

      return m_imageViews[arrayIndex];
    }


    void ImageViews::Reset()
    {
      if (m_imageViews.size() > 0)
      {
        assert(m_device != VK_NULL_HANDLE);

        // Destroy the image views one by one
        for (std::size_t i = 0; i < m_imageViews.size(); ++i)
          Reset(i);

        m_imageViews.resize(0);
        m_device = VK_NULL_HANDLE;
      }
      assert(m_device == VK_NULL_HANDLE);
    }


    void ImageViews::Reset(VkDevice device, const std::size_t count)
    {
      if (device == VK_NULL_HANDLE)
        throw std::invalid_argument("Device can not be VK_NULL_HANDLE");

      if (m_imageViews.size() > 0)
        Reset();

      if (count == 0)
      {
        FSLLOG_DEBUG_WARNING("count was zero, nothing created");
        return;
      }

      m_imageViews.resize(count);
      m_device = device;
    }


    void ImageViews::Reset(const std::size_t arrayIndex)
    {
      if (arrayIndex >= m_imageViews.size())
        throw std::invalid_argument("index out of bounds");

      if (m_imageViews[arrayIndex] == VK_NULL_HANDLE)
        return;

      vkDestroyImageView(m_device, m_imageViews[arrayIndex], nullptr);
      m_imageViews[arrayIndex] = VK_NULL_HANDLE;
    }


    void ImageViews::Reset(const std::size_t arrayIndex, const VkImageViewCreateInfo& imageViewCreateInfo)
    {
      if (arrayIndex >= m_imageViews.size())
        throw std::invalid_argument("index out of bounds");

      if (m_imageViews[arrayIndex] != VK_NULL_HANDLE)
        Reset(arrayIndex);

      // Since we want to ensure that the m_imageViews[arrayIndex] is left untouched on error we use a local
      // variable as a intermediary

      VkImageView imageView;
      FSLGRAPHICSVULKAN_CHECK(vkCreateImageView(m_device, &imageViewCreateInfo, nullptr, &imageView));

      m_imageViews[arrayIndex] = imageView;
    }

  }
}
