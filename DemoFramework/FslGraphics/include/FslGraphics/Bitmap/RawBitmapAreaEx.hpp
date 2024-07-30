#ifndef FSLGRAPHICS_BITMAP_RAWBITMAPAREAEX_HPP
#define FSLGRAPHICS_BITMAP_RAWBITMAPAREAEX_HPP
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

#include <FslBase/Span/Span.hpp>
#include <FslGraphics/Bitmap/ReadOnlyRawBitmapArea.hpp>

namespace Fsl
{
  //! @brief Very basic structure intended to contain low level information about a bitmap area
  //! @note  It is expected that all other necessary data about the bitmap has been acquired elsewhere
  struct RawBitmapAreaEx
  {
    uint8_t* ContentPtr{nullptr};

    std::size_t ByteSize{0};

    PxSize2D SizePx;

    uint32_t Stride{0};

  private:
    constexpr RawBitmapAreaEx(Span<uint8_t> span, const PxSize2D sizePx, const uint32_t stride) noexcept
      : ContentPtr(span.data())
      , ByteSize(span.size())
      , SizePx(sizePx)
      , Stride(stride)
    {
      assert(ContentPtr != nullptr || ByteSize == 0u);
      assert(Stride > 0);
      // Since we don't know the number of bytes per pixel, this is just a bare minimum check
      assert((SizePx.RawUnsignedWidth() * SizePx.RawUnsignedHeight()) <= ByteSize);
    }

    constexpr RawBitmapAreaEx(uint8_t* pContent, const std::size_t byteSize, const PxSize2D sizePx, const uint32_t stride) noexcept
      : ContentPtr(pContent)
      , ByteSize(byteSize)
      , SizePx(sizePx)
      , Stride(stride)
    {
      assert(pContent != nullptr || ByteSize == 0u);
      assert(Stride > 0);
      // Since we don't know the number of bytes per pixel, this is just a bare minimum check
      assert((SizePx.RawUnsignedWidth() * SizePx.RawUnsignedHeight()) <= ByteSize);
    }

  public:
    constexpr RawBitmapAreaEx() noexcept = default;

    constexpr bool IsValid() const noexcept
    {
      return ContentPtr != nullptr && Stride > 0;
    }

    constexpr PxExtent2D Extent() const noexcept
    {
      return {SizePx.UnsignedWidth(), SizePx.UnsignedHeight()};
    }

    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr operator ReadOnlyRawBitmapArea() const noexcept
    {
      return ReadOnlyRawBitmapArea::UncheckedCreate(ContentPtr, ByteSize, SizePx, Stride);
    }


    static constexpr RawBitmapAreaEx UncheckedCreate(Span<uint8_t> span, const PxSize2D sizePx, const uint32_t stride) noexcept
    {
      return {span, sizePx, stride};
    }

    static constexpr RawBitmapAreaEx UncheckedCreate(uint8_t* pContent, const std::size_t byteSize, const PxSize2D sizePx,
                                                     const uint32_t stride) noexcept
    {
      return {pContent, byteSize, sizePx, stride};
    }
  };
}

#endif
