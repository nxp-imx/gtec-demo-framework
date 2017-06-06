#ifndef FSLGRAPHICS_PIXELFORMATLAYOUTUTIL_HPP
#define FSLGRAPHICS_PIXELFORMATLAYOUTUTIL_HPP
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

#include <FslBase/Log/Log.hpp>
#include <FslGraphics/PixelChannelOrder.hpp>
#include <FslGraphics/PixelFormatLayout.hpp>
#include <FslGraphics/StrideRequirement.hpp>
#include <cassert>

namespace Fsl
{
  class PixelFormatLayoutUtil
  {
  public:
    //! @brief Lookup the bytes per pixel of the pixelFormatLayout, beware that for compressed formats this return 0
    static uint32_t GetBytesPerPixel(const PixelFormatLayout pixelFormatLayout)
    {
      if (GetBasicEncodedLayout(pixelFormatLayout) == PixelFormatLayoutFlags::ENCODED_LAYOUT0)
        return (static_cast<uint32_t>(pixelFormatLayout)& PixelFormatLayoutFlags::BIT_MASK_ENCODED_LAYOUT0_BYTES_PER_PIXEL) >> PixelFormatLayoutFlags::BIT_INDEX_ENCODED_LAYOUT0_BYTES_PER_PIXEL;
      else
      {
        FSLLOG_DEBUG_WARNING("PixelFormatLayoutUtil::GetBytesPerPixel called on compressed pixel format");
        return 0;
      }
    }

    static uint32_t CalcMinimumStride(const uint32_t width, const uint32_t bytesPerPixel)
    {
      return width * bytesPerPixel;
    }


    static uint32_t CalcMinimumStride(const uint32_t width, const PixelFormatLayout pixelFormatLayout);
    static uint32_t CalcMinimumStride(const uint32_t width, const PixelFormatLayout pixelFormatLayout, const StrideRequirement strideRequirement);
    static uint32_t CalcMinimumStride(const uint32_t width, const uint32_t bytesPerPixel, const StrideRequirement strideRequirement);
    static bool IsValidStride(const uint32_t width, const PixelFormatLayout pixelFormatLayout, const StrideRequirement strideRequirement, const uint32_t desiredStride);
    static bool IsValidStride(const uint32_t width, const uint32_t bytesPerPixel, const StrideRequirement strideRequirement, const uint32_t desiredStride);

    //! @brief Check if the formats can be converted to each other using a simple swizzle operation
    static bool IsSwizzleCompatible(const PixelFormatLayout lhs, const PixelFormatLayout rhs);

    //! @brief Given a pixel format id try to lookup the pixel format enum value.
    //! @note  Returns the PixelFormat value or PixelFormat::Undefined if the lookup fails (or if asked to lookup the undefined value)
    static PixelFormatLayout TryGetPixelFormatLayoutById(const uint32_t formatId);

    //! @brief Get the format-id of the pixel format (it basically strips all the extra meta data)
    static uint32_t GetId(const PixelFormatLayout pixelFormatLayout)
    {
      return (static_cast<int32_t>(pixelFormatLayout)& PixelFormatLayoutFlags::BIT_MASK_FORMAT_ID);
    }

    //! @brief Check if this is considered a compressed format
    static bool IsCompressed(const PixelFormatLayout pixelFormatLayout)
    {
      return (GetBasicEncodedLayout(pixelFormatLayout) == PixelFormatLayoutFlags::ENCODED_LAYOUT1 && GetEncodedLayout1CompressionScheme(pixelFormatLayout) != PixelFormatLayoutFlags::CS_NONE);
    }

    //! @brief Check if this is considered a packed format
    static bool IsPacked(const PixelFormatLayout pixelFormatLayout)
    {
      return (GetBasicEncodedLayout(pixelFormatLayout) == PixelFormatLayoutFlags::ENCODED_LAYOUT0 && GetEncodedLayout0Packed(pixelFormatLayout) == PixelFormatLayoutFlags::Packed);
    }

    // *** bit extraction helper methods ***

    // @brief Extract the basic encoded layout (can only differentiate between layout0 and 1, not any extended encodings)
    // @return PixelFormatLayoutFlags::ENCODED_LAYOUT0 or PixelFormatLayoutFlags::ENCODED_LAYOUT1
    static int32_t GetBasicEncodedLayout(const PixelFormatLayout pixelFormatLayout)
    {
      return (static_cast<int32_t>(pixelFormatLayout)& PixelFormatLayoutFlags::BIT_MASK_ENCODED_LAYOUT_BIT0);
    }

    // @brief Extract the encoded layout
    // @return PixelFormatLayoutFlags::ENCODED_LAYOUT0 or PixelFormatLayoutFlags::ENCODED_LAYOUT1
    static int32_t GetEncodedLayout0Packed(const PixelFormatLayout pixelFormatLayout)
    {
      assert(GetBasicEncodedLayout(pixelFormatLayout) == PixelFormatLayoutFlags::ENCODED_LAYOUT0);
      return (static_cast<int32_t>(pixelFormatLayout)& PixelFormatLayoutFlags::BIT_MASK_ENCODED_LAYOUT0_PACKED);
    }

    // @brief Extract the encoded layout
    // @return PixelFormatLayoutFlags::ENCODED_LAYOUT0 or PixelFormatLayoutFlags::ENCODED_LAYOUT1
    static int32_t GetEncodedLayout1CompressionScheme(const PixelFormatLayout pixelFormatLayout)
    {
      assert(GetBasicEncodedLayout(pixelFormatLayout) == PixelFormatLayoutFlags::ENCODED_LAYOUT1);
      return (static_cast<int32_t>(pixelFormatLayout)& PixelFormatLayoutFlags::BIT_MASK_ENCODED_LAYOUT1_COMPRESSION_SCHEME);
    }

    //! @brief Transform the given pixel format layout to the one that best matches the preferredChannelOrder.
    //! @param layout if layout == PixelFormatLayout::Undefined this returns PixelFormatLayout::Undefined
    //! @param preferredChannelOrder if preferredChannelOrder == PixelChannelOrder::Undefined this returns layout
    static PixelFormatLayout Transform(const PixelFormatLayout layout, const PixelChannelOrder preferredChannelOrder);
  };
}

#endif
