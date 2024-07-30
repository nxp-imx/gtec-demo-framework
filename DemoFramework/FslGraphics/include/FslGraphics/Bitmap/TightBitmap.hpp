#ifndef FSLGRAPHICS_BITMAP_TIGHTBITMAP_HPP
#define FSLGRAPHICS_BITMAP_TIGHTBITMAP_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Span/Span.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslGraphics/Bitmap/BitmapMemory.hpp>
#include <FslGraphics/Bitmap/BitmapOrigin.hpp>
#include <FslGraphics/Bitmap/RawBitmapEx.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <vector>

namespace Fsl
{
  class ReadOnlyRawBitmap;

  //! @brief Represents a Bitmap that is tightly packed in memory (No padding applied stride).
  class TightBitmap final
  {
    std::vector<uint8_t> m_content;
    PxSize2D m_sizePx;
    PixelFormat m_pixelFormat{PixelFormat::Undefined};
    BitmapOrigin m_origin{BitmapOrigin::UpperLeft};
    uint8_t m_bytesPerPixel{0};

  public:
    // move assignment operator
    TightBitmap& operator=(TightBitmap&& other) noexcept;
    // move constructor
    TightBitmap(TightBitmap&& other) noexcept;

    // Request that the compiler generates a copy constructor and assignment operator
    TightBitmap(const TightBitmap&) = default;
    TightBitmap& operator=(const TightBitmap&) = default;

    TightBitmap() = default;

    TightBitmap(const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin origin);
    TightBitmap(const PxExtent2D extentPx, const PixelFormat pixelFormat, const BitmapOrigin origin);

    explicit TightBitmap(const ReadOnlyRawBitmap& content);
    explicit TightBitmap(const RawBitmapEx& content);
    explicit TightBitmap(BitmapMemory&& content);

    TightBitmap(std::vector<uint8_t> content, const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin origin);
    TightBitmap(std::vector<uint8_t> content, const PxExtent2D extentPx, const PixelFormat pixelFormat, const BitmapOrigin origin);

    TightBitmap(const ReadOnlySpan<uint8_t> content, const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin origin)
      : TightBitmap(SpanUtil::ToVector(content), sizePx, pixelFormat, origin)
    {
    }

    TightBitmap(const ReadOnlySpan<uint8_t> content, const PxExtent2D extentPx, const PixelFormat pixelFormat, const BitmapOrigin origin)
      : TightBitmap(SpanUtil::ToVector(content), extentPx, pixelFormat, origin)
    {
    }

    void Reset() noexcept;
    // void Reset(const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin origin);
    // void Reset(const PxExtent2D extentPx, const PixelFormat pixelFormat, const BitmapOrigin origin);

    // void Reset(const ReadOnlyRawBitmap& content);
    // void Reset(const RawBitmapEx& content);
    // void Reset(const Bitmap& content);

    // void Reset(std::vector<uint8_t> content, const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin origin);
    // void Reset(std::vector<uint8_t> content, const PxExtent2D extentPx, const PixelFormat pixelFormat, const BitmapOrigin origin);

    // void Reset(const ReadOnlySpan<uint8_t> content, const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin origin)
    //{
    //   Reset(SpanUtil::ToVector(content), sizePx, pixelFormat, origin);
    // }

    // void Reset(const ReadOnlySpan<uint8_t> content, const PxExtent2D extentPx, const PixelFormat pixelFormat, const BitmapOrigin origin)
    //{
    //   Reset(SpanUtil::ToVector(content), extentPx, pixelFormat, origin)
    // }

    constexpr PxSize2D GetSize() const noexcept
    {
      return m_sizePx;
    }

    constexpr PxSize1D Width() const noexcept
    {
      return m_sizePx.Width();
    }

    constexpr PxSize1D Height() const noexcept
    {
      return m_sizePx.Height();
    }

    constexpr PxSize1D::unsigned_value_type UnsignedWidth() const noexcept
    {
      return m_sizePx.UnsignedWidth();
    }

    constexpr PxSize1D::unsigned_value_type UnsignedHeight() const noexcept
    {
      return m_sizePx.UnsignedHeight();
    }

    constexpr PxSize1D::raw_value_type RawWidth() const noexcept
    {
      return m_sizePx.RawWidth();
    }

    constexpr PxSize1D::raw_value_type RawHeight() const noexcept
    {
      return m_sizePx.RawHeight();
    }

    constexpr PxSize1D::raw_unsigned_value_type RawUnsignedWidth() const noexcept
    {
      return m_sizePx.RawUnsignedWidth();
    }

    constexpr PxSize1D::raw_unsigned_value_type RawUnsignedHeight() const noexcept
    {
      return m_sizePx.RawUnsignedHeight();
    }


    constexpr PxExtent2D GetExtent() const noexcept
    {
      return {UnsignedWidth(), UnsignedHeight()};
    }

    constexpr PixelFormat GetPixelFormat() const noexcept
    {
      return m_pixelFormat;
    }

    constexpr BitmapOrigin GetOrigin() const noexcept
    {
      return m_origin;
    }

    //! @brief Return the number of pixels in the bitmap
    constexpr PxSize1D GetPixelCount() const noexcept
    {
      return m_sizePx.Width() * m_sizePx.Height();
    }

    //! @brief Return the number of bytes per pixel
    constexpr uint8_t GetBytesPerPixel() const noexcept
    {
      return m_bytesPerPixel;
    }

    //! @brief Return the number of bytes required to store the bitmap
    std::size_t GetByteSize() const noexcept
    {
      return m_content.size();
    }

    //! @brief The tightly packed stride of one scan-line (no padding)
    constexpr uint32_t Stride() const noexcept
    {
      return RawUnsignedWidth() * m_bytesPerPixel;
    }

    ReadOnlySpan<uint8_t> AsSpan() const noexcept;
    Span<uint8_t> AsSpan() noexcept;

    ReadOnlyRawBitmap AsRawBitmap() const noexcept;
    RawBitmapEx AsRawBitmap() noexcept;

    BitmapMemory ToBitmapMemory() const;

    //! @brief Release into a BitmapMemory object and 'Reset' the class
    [[nodiscard]] BitmapMemory Release() noexcept;

    //! @brief Release the internal vector and 'Reset' the class
    [[nodiscard]] std::vector<uint8_t> ReleaseAsVector() noexcept;
  };
}

#endif
