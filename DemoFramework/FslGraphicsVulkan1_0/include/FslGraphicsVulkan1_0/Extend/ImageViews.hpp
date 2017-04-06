#ifndef FSLGRAPHICSVULKAN1_0_EXTEND_IMAGEVIEWS_HPP
#define FSLGRAPHICSVULKAN1_0_EXTEND_IMAGEVIEWS_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslGraphicsVulkan1_0/Common.hpp>
#include <FslBase/Noncopyable.hpp>
#include <vulkan/vulkan.h>
#include <vector>

namespace Fsl
{
  namespace Vulkan
  {
    class ImageViews : private Noncopyable
    {
      VkDevice m_device;
      std::vector<VkImageView> m_imageViews;
    public:
      ImageViews();
      ImageViews(VkDevice device, const std::size_t count);
      ~ImageViews();

      //! @brief Release any owned resources.
      void Reset();

      //! @brief Replaces the managed object with a new one (releasing the old)
      void Reset(VkDevice device, const std::size_t count);

      VkDevice GetDevice() const
      {
        return m_device;
      }

      std::size_t Count() const
      {
        return m_imageViews.size();
      }


      const VkImageView* DirectAccess() const
      {
        return m_imageViews.data();
      }


      //! @brief Get the handle by index
      VkImageView Get(const std::size_t arrayIndex) const;

      //! @brief Reset the buffer to contain the supplied elements
      void Reset(const std::size_t arrayIndex);

      //! @brief Reset the buffer to contain the supplied elements
      //! @note  This is a very slow operation and its not recommended for updating the content of the ImageView (since it creates a new image view internally)
      void Reset(const std::size_t arrayIndex, const VkImageViewCreateInfo& imageViewCreateInfo);


      //! @brief Check if this object is valid
      bool IsValid() const { return m_imageViews.size() > 0; }

    };

  }
}

#endif
