#ifndef FSLUTIL_VULKAN1_0_SURFACEFORMATINFO_HPP
#define FSLUTIL_VULKAN1_0_SURFACEFORMATINFO_HPP
/****************************************************************************************************************************************************
 * Copyright 2019, 2022 NXP
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

#include <vulkan/vulkan.h>
#include <cassert>

namespace Fsl::Vulkan
{
  struct SurfaceFormatInfo
  {
    VkFormat Format;
    VkColorSpaceKHR ColorSpace;

    constexpr SurfaceFormatInfo() noexcept
      : SurfaceFormatInfo(VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
    {
    }

    explicit constexpr SurfaceFormatInfo(const VkFormat format) noexcept
      : SurfaceFormatInfo(format, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
    {
    }

    constexpr SurfaceFormatInfo(const VkFormat format, const VkColorSpaceKHR colorSpace) noexcept
      : Format(format)
      , ColorSpace(colorSpace)
    {
    }
  };
}

#endif
