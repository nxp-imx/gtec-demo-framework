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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslUtil/Vulkan1_0/Log/FmtAll.hpp>
#include <FslUtil/Vulkan1_0/Util/SurfaceFormatUtil.hpp>
#include <array>

namespace Fsl::Vulkan::SurfaceFormatUtil
{
  namespace
  {
    constexpr std::array<SurfaceFormatInfo, 3> FallbackPreferredFormats = {
      SurfaceFormatInfo(VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR),
      SurfaceFormatInfo(VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR),
      SurfaceFormatInfo(VK_FORMAT_A8B8G8R8_UNORM_PACK32, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)};
  }

  SurfaceFormatInfo TryFindPreferredFormat(const ReadOnlySpan<VkSurfaceFormatKHR> availableSurfaceFormats,
                                           const ReadOnlySpan<SurfaceFormatInfo> preferredSurfaceFormats)
  {
    for (auto preferred : preferredSurfaceFormats)
    {
      for (auto candidate : availableSurfaceFormats)
      {
        if (candidate.format == preferred.Format && candidate.colorSpace == preferred.ColorSpace)
        {
          return preferred;
        }
      }
    }
    return SurfaceFormatInfo(VK_FORMAT_UNDEFINED);
  }


  //! Find a preferred format of any color space
  SurfaceFormatInfo TryFindPreferredFormatAnyColorspace(ReadOnlySpan<VkSurfaceFormatKHR> availableSurfaceFormats,
                                                        const ReadOnlySpan<SurfaceFormatInfo> preferredSurfaceFormats)
  {
    for (auto preferred : preferredSurfaceFormats)
    {
      for (auto candidate : availableSurfaceFormats)
      {
        if (candidate.format == preferred.Format)
        {
          return {candidate.format, candidate.colorSpace};
        }
      }
    }
    return SurfaceFormatInfo(VK_FORMAT_UNDEFINED);
  }

  //! Find any format of the preferred color-space
  SurfaceFormatInfo TryFindAnyFormatWithColorSpace(ReadOnlySpan<VkSurfaceFormatKHR> availableSurfaceFormats, const VkColorSpaceKHR requiredColorSpace)
  {
    for (auto candidate : availableSurfaceFormats)
    {
      if (candidate.colorSpace == requiredColorSpace)
      {
        return {candidate.format, candidate.colorSpace};
      }
    }
    return SurfaceFormatInfo(VK_FORMAT_UNDEFINED);
  }


  SurfaceFormatInfo TryFindSurfaceFormat(ReadOnlySpan<VkSurfaceFormatKHR> availableSurfaceFormats,
                                         const ReadOnlySpan<SurfaceFormatInfo> preferredSurfaceFormats,
                                         const VkColorSpaceKHR fallbackDesiredColorSpace)
  {
    if (availableSurfaceFormats.empty())
    {
      return SurfaceFormatInfo(VK_FORMAT_UNDEFINED);
    }

    FSLLOG3_VERBOSE6("Trying to find preferred format");
    SurfaceFormatInfo selectedFormat = TryFindPreferredFormat(availableSurfaceFormats, preferredSurfaceFormats);
    if (selectedFormat.Format == VK_FORMAT_UNDEFINED)
    {
      FSLLOG3_VERBOSE6("Trying any format that matches the desired color-space");
      selectedFormat = TryFindAnyFormatWithColorSpace(availableSurfaceFormats, fallbackDesiredColorSpace);
      if (selectedFormat.Format == VK_FORMAT_UNDEFINED)
      {
        FSLLOG3_VERBOSE6("Trying a preferred format of any color space");
        selectedFormat = TryFindPreferredFormatAnyColorspace(availableSurfaceFormats, preferredSurfaceFormats);
        if (selectedFormat.Format == VK_FORMAT_UNDEFINED)
        {
          FSLLOG3_VERBOSE6("Using first found format");
          selectedFormat = SurfaceFormatInfo(availableSurfaceFormats[0].format, availableSurfaceFormats[0].colorSpace);
        }
      }
    }

    FSLLOG3_VERBOSE6("Selected format: {0} colorSpace: {1}", selectedFormat.Format, selectedFormat.ColorSpace);
    return selectedFormat;
  }


  SurfaceFormatInfo TryFindSurfaceFormat(ReadOnlySpan<VkSurfaceFormatKHR> availableSurfaceFormats)
  {
    return TryFindSurfaceFormat(availableSurfaceFormats, SpanUtil::AsReadOnlySpan(FallbackPreferredFormats), VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
  }


  ReadOnlySpan<SurfaceFormatInfo> GetSuggestedDefaultPreferredSurfaceFormats() noexcept
  {
    return SpanUtil::AsReadOnlySpan(FallbackPreferredFormats);
  }
}
