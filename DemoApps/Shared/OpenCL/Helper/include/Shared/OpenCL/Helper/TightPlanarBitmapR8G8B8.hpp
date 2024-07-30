#ifndef SHARED_OPENCL_HELPER_TIGHTPLANARBITMAPR8G8B8_HPP
#define SHARED_OPENCL_HELPER_TIGHTPLANARBITMAPR8G8B8_HPP
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
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/Span.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/BitmapMemory.hpp>
#include <FslGraphics/Bitmap/BitmapOrigin.hpp>
#include <FslGraphics/Bitmap/RawBitmapEx.hpp>
#include <FslGraphics/Bitmap/TightBitmap.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <vector>

namespace Fsl
{
  //! Experimental tightly packed planar bitmap class
  //! If we need other planar formats it would make sense to template this one so it can support 1-X planes instead of a fixed three
  class TightPlanarBitmapR8G8B8 final
  {
    std::vector<uint8_t> m_content;
    PxSize2D m_sizePx;
    uint32_t m_bytesPerPlane{0};
    PixelFormat m_pixelFormat{PixelFormat::Undefined};
    BitmapOrigin m_origin{BitmapOrigin::UpperLeft};

    TightPlanarBitmapR8G8B8(std::vector<uint8_t> content, const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin origin);

  public:
    // move assignment operator
    TightPlanarBitmapR8G8B8& operator=(TightPlanarBitmapR8G8B8&& other) noexcept;
    // move constructor
    TightPlanarBitmapR8G8B8(TightPlanarBitmapR8G8B8&& other) noexcept;

    // Request that the compiler generates a copy constructor and assignment operator
    TightPlanarBitmapR8G8B8(const TightPlanarBitmapR8G8B8&) = default;
    TightPlanarBitmapR8G8B8& operator=(const TightPlanarBitmapR8G8B8&) = default;

    TightPlanarBitmapR8G8B8() = default;

    TightPlanarBitmapR8G8B8(const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin origin);
    TightPlanarBitmapR8G8B8(const PxExtent2D extentPx, const PixelFormat pixelFormat, const BitmapOrigin origin);

    explicit TightPlanarBitmapR8G8B8(const ReadOnlyRawBitmap& content);
    explicit TightPlanarBitmapR8G8B8(const RawBitmapEx& content);
    explicit TightPlanarBitmapR8G8B8(const Bitmap& content);
    explicit TightPlanarBitmapR8G8B8(const TightBitmap& content);
    explicit TightPlanarBitmapR8G8B8(BitmapMemory&& content);
    TightPlanarBitmapR8G8B8(const ReadOnlySpan<uint8_t> content, const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin origin);
    TightPlanarBitmapR8G8B8(const ReadOnlySpan<uint8_t> content, const PxExtent2D extentPx, const PixelFormat pixelFormat, const BitmapOrigin origin);

    void Reset() noexcept;

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

    constexpr PxValueU UnsignedWidth() const noexcept
    {
      return m_sizePx.UnsignedWidth();
    }

    constexpr PxValueU UnsignedHeight() const noexcept
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

    constexpr PxValueU::raw_value_type RawUnsignedWidth() const noexcept
    {
      return m_sizePx.RawUnsignedWidth();
    }

    constexpr PxValueU::raw_value_type RawUnsignedHeight() const noexcept
    {
      return m_sizePx.RawUnsignedHeight();
    }

    PxExtent2D GetExtent() const noexcept;

    constexpr PixelFormat GetPixelFormat() const noexcept
    {
      return m_pixelFormat;
    }

    constexpr BitmapOrigin GetOrigin() const noexcept
    {
      return m_origin;
    }

    constexpr uint32_t GetBytesPerPlanePixel() const noexcept
    {
      return 1u;
    }

    uint32_t PlaneStride() const noexcept;

    ReadOnlySpan<uint8_t> AsSpanR() const noexcept;
    Span<uint8_t> AsSpanR() noexcept;

    ReadOnlySpan<uint8_t> AsSpanG() const noexcept;
    Span<uint8_t> AsSpanG() noexcept;

    ReadOnlySpan<uint8_t> AsSpanB() const noexcept;
    Span<uint8_t> AsSpanB() noexcept;

    ReadOnlyRawBitmap AsRawBitmapR() const noexcept;
    RawBitmapEx AsRawBitmapR() noexcept;

    ReadOnlyRawBitmap AsRawBitmapG() const noexcept;
    RawBitmapEx AsRawBitmapG() noexcept;

    ReadOnlyRawBitmap AsRawBitmapB() const noexcept;
    RawBitmapEx AsRawBitmapB() noexcept;

    BitmapMemory ToBitmapMemory() const;

    static TightPlanarBitmapR8G8B8 CreateFromTightPlanar(std::vector<uint8_t> content, const PxSize2D sizePx, const PixelFormat pixelFormat,
                                                         const BitmapOrigin origin);
    static TightPlanarBitmapR8G8B8 CreateFromTightPlanar(std::vector<uint8_t> content, const PxExtent2D extentPx, const PixelFormat pixelFormat,
                                                         const BitmapOrigin origin);
  };
}

#endif
