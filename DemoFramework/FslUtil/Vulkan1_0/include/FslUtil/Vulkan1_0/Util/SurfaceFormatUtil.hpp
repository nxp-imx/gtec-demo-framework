#ifndef FSLUTIL_VULKAN1_0_UTIL_SURFACEFORMATUTIL_HPP
#define FSLUTIL_VULKAN1_0_UTIL_SURFACEFORMATUTIL_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <FslUtil/Vulkan1_0/SurfaceFormatInfo.hpp>

namespace Fsl::Vulkan::SurfaceFormatUtil
{
  //! @brief Find a preferred format of a specific color space
  //! @return res.format == VK_FORMAT_UNDEFINED if no format was found
  SurfaceFormatInfo TryFindPreferredFormat(const ReadOnlySpan<VkSurfaceFormatKHR> availableSurfaceFormats,
                                           const ReadOnlySpan<SurfaceFormatInfo> preferredSurfaceFormats);

  //! @brief Find a preferred format of any color space
  //! @return res.format == VK_FORMAT_UNDEFINED if no format was found
  SurfaceFormatInfo TryFindPreferredFormatAnyColorspace(ReadOnlySpan<VkSurfaceFormatKHR> availableSurfaceFormats,
                                                        const ReadOnlySpan<SurfaceFormatInfo> preferredSurfaceFormats);

  //! @brief Find any format with the given color-space
  //! @return res.format == VK_FORMAT_UNDEFINED if no format was found
  SurfaceFormatInfo TryFindAnyFormatWithColorSpace(ReadOnlySpan<VkSurfaceFormatKHR> availableSurfaceFormats,
                                                   const VkColorSpaceKHR requiredColorSpace);


  //! @brief Find a surface format using preferred formats and color space as guide but will return format if availableSurfaceFormats contains any.
  //!        - Find any preferred format with the desired color-space
  //!        - Find any format with the desired color-space
  //!        - Find any preferred format with any color-space
  //!        - use the first entry of the availableSurfaceFormats if not empty
  //!        - give up and return res.format == VK_FORMAT_UNDEFINED
  SurfaceFormatInfo TryFindSurfaceFormat(ReadOnlySpan<VkSurfaceFormatKHR> availableSurfaceFormats,
                                         const ReadOnlySpan<SurfaceFormatInfo> preferredSurfaceFormats,
                                         const VkColorSpaceKHR fallbackDesiredColorSpace);

  //! @brief Find a surface format using preferred formats and color space as guide but will return format if availableSurfaceFormats contains any.
  //!        This method has a baked in preferred format list
  //!        - Find any preferred format with the desired color-space
  //!        - Find any format with the desired color-space
  //!        - Find any preferred format with any color-space
  //!        - use the first entry of the availableSurfaceFormats if not empty
  //!        - give up and return res.format == VK_FORMAT_UNDEFINED
  SurfaceFormatInfo TryFindSurfaceFormat(ReadOnlySpan<VkSurfaceFormatKHR> availableSurfaceFormats);

  //! @brief Get the suggested default preferred surface formats
  ReadOnlySpan<SurfaceFormatInfo> GetSuggestedDefaultPreferredSurfaceFormats() noexcept;
}

#endif
