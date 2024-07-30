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

#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/RawBitmapEx.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <Shared/OpenCL/Helper/TightPlanarBitmapR8G8B8.hpp>
#include <utility>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr uint16_t BytesPerPlanePixel = 1u;
      constexpr PixelFormat ChannelPixelFormat = PixelFormat::R8_UNORM;
    }

    namespace ChannelIndex
    {
      constexpr std::size_t R = 0;
      constexpr std::size_t G = 1;
      constexpr std::size_t B = 2;
    }

    TightPlanarBitmapR8G8B8 ConvertToPlanar(const ReadOnlyRawBitmap& rawBitmap)
    {
      if (PixelFormatUtil::GetPixelFormatLayout(rawBitmap.GetPixelFormat()) != PixelFormatLayout::R8G8B8)
      {
        throw NotSupportedException("Only support PixelFormatLayout::R8G8B8");
      }

      const auto minimumStride = PixelFormatUtil::CalcMinimumStride(rawBitmap.Width(), rawBitmap.GetPixelFormat());

      // Pack tightly into planes
      const std::size_t minimumSize = minimumStride * rawBitmap.RawUnsignedHeight();

      assert((minimumStride % 3) == 0);
      [[maybe_unused]] const std::size_t minimumPlaneStride = minimumStride / 3;
      assert((minimumSize % 3) == 0);
      const std::size_t minimumPlaneSize = minimumSize / 3;

      std::vector<uint8_t> tightlyPackedPlanarBitmap(minimumSize);

      const auto* pSrc = static_cast<const uint8_t*>(rawBitmap.Content());
      const uint8_t* const pSrcEnd = pSrc + rawBitmap.GetByteSize();
      const uint32_t srcStride = rawBitmap.Stride();

      uint8_t* pDstR = tightlyPackedPlanarBitmap.data() + (minimumPlaneSize * ChannelIndex::R);
      uint8_t* pDstG = tightlyPackedPlanarBitmap.data() + (minimumPlaneSize * ChannelIndex::G);
      uint8_t* pDstB = tightlyPackedPlanarBitmap.data() + (minimumPlaneSize * ChannelIndex::B);

      while (pSrc < pSrcEnd)
      {
        assert((pDstR + minimumPlaneStride) <= (tightlyPackedPlanarBitmap.data() + tightlyPackedPlanarBitmap.size()));
        assert((pDstG + minimumPlaneStride) <= (tightlyPackedPlanarBitmap.data() + tightlyPackedPlanarBitmap.size()));
        assert((pDstB + minimumPlaneStride) <= (tightlyPackedPlanarBitmap.data() + tightlyPackedPlanarBitmap.size()));
        for (uint32_t x = 0; x < minimumStride; x += 3)
        {
          *pDstR = pSrc[x];        // r
          *pDstG = pSrc[x + 1];    // g
          *pDstB = pSrc[x + 2];    // b
          ++pDstR;
          ++pDstG;
          ++pDstB;
        }
        pSrc += srcStride;
      }
      return TightPlanarBitmapR8G8B8::CreateFromTightPlanar(tightlyPackedPlanarBitmap, rawBitmap.GetSize(), rawBitmap.GetPixelFormat(),
                                                            rawBitmap.GetOrigin());
    }

    TightPlanarBitmapR8G8B8 ConvertToPlanar(BitmapMemory&& bitmapMemory)
    {
      const auto pixelFormat = bitmapMemory.GetPixelFormat();
      const auto extent = TypeConverter::UncheckedTo<PxExtent2D>(bitmapMemory.GetSize());
      const auto origin = bitmapMemory.GetOrigin();
      const auto stride = bitmapMemory.Stride();

      std::vector<uint8_t> content = bitmapMemory.Release();
      ReadOnlyRawBitmap rawBitmap(ReadOnlyRawBitmap::Create(SpanUtil::AsReadOnlySpan(content), extent, pixelFormat, stride, origin));

      return ConvertToPlanar(rawBitmap);
    }


    TightPlanarBitmapR8G8B8 ConvertToPlanar(const Bitmap& bitmap)
    {
      const Bitmap::ScopedDirectReadAccess rawAccess(bitmap);
      return ConvertToPlanar(rawAccess.AsRawBitmap());
    }


    TightPlanarBitmapR8G8B8 ConvertToPlanar(const TightBitmap& bitmap)
    {
      return ConvertToPlanar(bitmap.AsRawBitmap());
    }

    void InterleaveR8G8B8Planes(Span<uint8_t> dst, const ReadOnlySpan<uint8_t> srcPlanes, const std::size_t bytesPerPlane)
    {
      assert(dst.size() == srcPlanes.size());
      assert((dst.size() / 3) == bytesPerPlane);
      assert((dst.size() % 3) == 0u);

      const ReadOnlySpan<uint8_t> srcPlaneR = srcPlanes.subspan(bytesPerPlane * ChannelIndex::R);
      const ReadOnlySpan<uint8_t> srcPlaneG = srcPlanes.subspan(bytesPerPlane * ChannelIndex::G);
      const ReadOnlySpan<uint8_t> srcPlaneB = srcPlanes.subspan(bytesPerPlane * ChannelIndex::B);

      std::size_t dstIndex = 0;
      for (std::size_t i = 0; i < bytesPerPlane; ++i)
      {
        dst[dstIndex] = srcPlaneR[i];
        dst[dstIndex + 1] = srcPlaneG[i];
        dst[dstIndex + 2] = srcPlaneB[i];
        dstIndex += 3;
      }
    }

  }

  // move assignment operator
  TightPlanarBitmapR8G8B8& TightPlanarBitmapR8G8B8::operator=(TightPlanarBitmapR8G8B8&& other) noexcept
  {
    if (this != &other)
    {
      // Free existing resources then transfer the content of other to this one and fill other with default values
      Reset();

      // Claim ownership here
      m_content = std::move(other.m_content);
      m_sizePx = other.m_sizePx;
      m_bytesPerPlane = other.m_bytesPerPlane;
      m_pixelFormat = other.m_pixelFormat;
      m_origin = other.m_origin;

      // Remove the data from other
      other.m_sizePx = {};
      other.m_bytesPerPlane = 0;
      other.m_pixelFormat = PixelFormat::Undefined;
      other.m_origin = BitmapOrigin::UpperLeft;
    }
    return *this;
  }


  TightPlanarBitmapR8G8B8::TightPlanarBitmapR8G8B8(TightPlanarBitmapR8G8B8&& other) noexcept
    : m_content(std::move(other.m_content))
    , m_sizePx(other.m_sizePx)
    , m_bytesPerPlane(other.m_bytesPerPlane)
    , m_pixelFormat(other.m_pixelFormat)
    , m_origin(other.m_origin)
  {
    other.m_sizePx = {};
    other.m_bytesPerPlane = 0;
    other.m_pixelFormat = PixelFormat::Undefined;
    other.m_origin = BitmapOrigin::UpperLeft;
    assert((m_bytesPerPlane * 3u) == m_content.size());
  }


  TightPlanarBitmapR8G8B8::TightPlanarBitmapR8G8B8(const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin origin)
    : m_content(sizePx.RawHeight() * PixelFormatUtil::CalcMinimumStride(sizePx.Width(), pixelFormat))
    , m_sizePx(sizePx)
    , m_bytesPerPlane(UncheckedNumericCast<uint32_t>(sizePx.RawWidth() * sizePx.RawHeight() * LocalConfig::BytesPerPlanePixel))
    , m_pixelFormat(pixelFormat)
    , m_origin(origin)
  {
    assert((m_bytesPerPlane * 3u) == m_content.size());
  }


  TightPlanarBitmapR8G8B8::TightPlanarBitmapR8G8B8(const PxExtent2D extentPx, const PixelFormat pixelFormat, const BitmapOrigin origin)
    : TightPlanarBitmapR8G8B8(TypeConverter::To<PxSize2D>(extentPx), pixelFormat, origin)
  {
  }


  TightPlanarBitmapR8G8B8::TightPlanarBitmapR8G8B8(const ReadOnlyRawBitmap& content)
    : TightPlanarBitmapR8G8B8(ConvertToPlanar(content))
  {
  }


  TightPlanarBitmapR8G8B8::TightPlanarBitmapR8G8B8(const RawBitmapEx& content)
    : TightPlanarBitmapR8G8B8(ConvertToPlanar(static_cast<const ReadOnlyRawBitmap&>(content)))
  {
  }


  TightPlanarBitmapR8G8B8::TightPlanarBitmapR8G8B8(const Bitmap& content)
    : TightPlanarBitmapR8G8B8(ConvertToPlanar(content))
  {
  }


  TightPlanarBitmapR8G8B8::TightPlanarBitmapR8G8B8(const TightBitmap& content)
    : TightPlanarBitmapR8G8B8(ConvertToPlanar(content))
  {
  }


  TightPlanarBitmapR8G8B8::TightPlanarBitmapR8G8B8(BitmapMemory&& content)
    : TightPlanarBitmapR8G8B8(ConvertToPlanar(std::move(content)))
  {
  }


  TightPlanarBitmapR8G8B8::TightPlanarBitmapR8G8B8(const ReadOnlySpan<uint8_t> content, const PxSize2D sizePx, const PixelFormat pixelFormat,
                                                   const BitmapOrigin origin)
    : TightPlanarBitmapR8G8B8(ConvertToPlanar(ReadOnlyRawBitmap::Create(content, sizePx, pixelFormat, origin)))
  {
  }


  TightPlanarBitmapR8G8B8::TightPlanarBitmapR8G8B8(const ReadOnlySpan<uint8_t> content, const PxExtent2D extentPx, const PixelFormat pixelFormat,
                                                   const BitmapOrigin origin)
    : TightPlanarBitmapR8G8B8(ConvertToPlanar(ReadOnlyRawBitmap::Create(content, extentPx, pixelFormat, origin)))
  {
  }


  TightPlanarBitmapR8G8B8::TightPlanarBitmapR8G8B8(std::vector<uint8_t> content, const PxSize2D sizePx, const PixelFormat pixelFormat,
                                                   const BitmapOrigin origin)
    : m_content(std::move(content))
    , m_sizePx(sizePx)
    , m_bytesPerPlane(sizePx.RawWidth() * sizePx.RawHeight() * LocalConfig::BytesPerPlanePixel)
    , m_pixelFormat(pixelFormat)
    , m_origin(origin)
  {
    assert((m_bytesPerPlane * 3u) == m_content.size());
  }


  void TightPlanarBitmapR8G8B8::Reset() noexcept
  {
    m_content.clear();
    m_sizePx = {};
    m_bytesPerPlane = 0;
    m_pixelFormat = PixelFormat::Undefined;
    m_origin = BitmapOrigin::UpperLeft;
  }


  PxExtent2D TightPlanarBitmapR8G8B8::GetExtent() const noexcept
  {
    return TypeConverter::UncheckedTo<PxExtent2D>(m_sizePx);
  }


  uint32_t TightPlanarBitmapR8G8B8::PlaneStride() const noexcept
  {
    return TypeConverter::UncheckedTo<PxValueU>(m_sizePx.Width()).Value * GetBytesPerPlanePixel();
  }


  ReadOnlySpan<uint8_t> TightPlanarBitmapR8G8B8::AsSpanR() const noexcept
  {
    return SpanUtil::UncheckedAsReadOnlySpan(m_content, m_bytesPerPlane * ChannelIndex::R, m_bytesPerPlane);
  }


  Span<uint8_t> TightPlanarBitmapR8G8B8::AsSpanR() noexcept
  {
    return SpanUtil::UncheckedAsSpan(m_content, m_bytesPerPlane * ChannelIndex::R, m_bytesPerPlane);
  }


  ReadOnlySpan<uint8_t> TightPlanarBitmapR8G8B8::AsSpanG() const noexcept
  {
    return SpanUtil::UncheckedAsReadOnlySpan(m_content, m_bytesPerPlane * ChannelIndex::G, m_bytesPerPlane);
  }


  Span<uint8_t> TightPlanarBitmapR8G8B8::AsSpanG() noexcept
  {
    return SpanUtil::UncheckedAsSpan(m_content, m_bytesPerPlane * ChannelIndex::G, m_bytesPerPlane);
  }


  ReadOnlySpan<uint8_t> TightPlanarBitmapR8G8B8::AsSpanB() const noexcept
  {
    return SpanUtil::UncheckedAsReadOnlySpan(m_content, m_bytesPerPlane * ChannelIndex::B, m_bytesPerPlane);
  }


  Span<uint8_t> TightPlanarBitmapR8G8B8::AsSpanB() noexcept
  {
    return SpanUtil::UncheckedAsSpan(m_content, m_bytesPerPlane * ChannelIndex::B, m_bytesPerPlane);
  }


  ReadOnlyRawBitmap TightPlanarBitmapR8G8B8::AsRawBitmapR() const noexcept
  {
    return ReadOnlyRawBitmap::UncheckedCreate(AsSpanR(), GetSize(), LocalConfig::ChannelPixelFormat, GetOrigin());
  }


  RawBitmapEx TightPlanarBitmapR8G8B8::AsRawBitmapR() noexcept
  {
    return RawBitmapEx::UncheckedCreate(AsSpanR(), GetSize(), LocalConfig::ChannelPixelFormat, GetOrigin());
  }


  ReadOnlyRawBitmap TightPlanarBitmapR8G8B8::AsRawBitmapG() const noexcept
  {
    return ReadOnlyRawBitmap::UncheckedCreate(AsSpanG(), GetSize(), LocalConfig::ChannelPixelFormat, GetOrigin());
  }


  RawBitmapEx TightPlanarBitmapR8G8B8::AsRawBitmapG() noexcept
  {
    return RawBitmapEx::UncheckedCreate(AsSpanG(), GetSize(), LocalConfig::ChannelPixelFormat, GetOrigin());
  }


  ReadOnlyRawBitmap TightPlanarBitmapR8G8B8::AsRawBitmapB() const noexcept
  {
    return ReadOnlyRawBitmap::UncheckedCreate(AsSpanB(), GetSize(), LocalConfig::ChannelPixelFormat, GetOrigin());
  }


  RawBitmapEx TightPlanarBitmapR8G8B8::AsRawBitmapB() noexcept
  {
    return RawBitmapEx::UncheckedCreate(AsSpanB(), GetSize(), LocalConfig::ChannelPixelFormat, GetOrigin());
  }


  BitmapMemory TightPlanarBitmapR8G8B8::ToBitmapMemory() const
  {
    std::vector<uint8_t> content(m_content.size());
    InterleaveR8G8B8Planes(SpanUtil::AsSpan(content), SpanUtil::AsReadOnlySpan(m_content), m_bytesPerPlane);
    return BitmapMemory::UncheckedCreate(std::move(content), GetSize(), GetPixelFormat(), GetOrigin());
  }


  TightPlanarBitmapR8G8B8 TightPlanarBitmapR8G8B8::CreateFromTightPlanar(std::vector<uint8_t> content, const PxSize2D sizePx,
                                                                         const PixelFormat pixelFormat, const BitmapOrigin origin)
  {
    return {std::move(content), sizePx, pixelFormat, origin};
  }


  TightPlanarBitmapR8G8B8 TightPlanarBitmapR8G8B8::CreateFromTightPlanar(std::vector<uint8_t> content, const PxExtent2D extentPx,
                                                                         const PixelFormat pixelFormat, const BitmapOrigin origin)
  {
    return {std::move(content), TypeConverter::To<PxSize2D>(extentPx), pixelFormat, origin};
  }
}
