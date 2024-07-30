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
#include <FslBase/NumericCast.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/RawBitmapEx.hpp>
#include <FslGraphics/Bitmap/TightBitmap.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <utility>

namespace Fsl
{
  namespace
  {
    TightBitmap Convert(const ReadOnlyRawBitmap& rawBitmap)
    {
      const auto minimumStride = PixelFormatUtil::CalcMinimumStride(rawBitmap.Width(), rawBitmap.GetPixelFormat());
      // Check if the source is tightly packed
      if (minimumStride == rawBitmap.Stride())
      {
        assert(rawBitmap.IsTightlyPacked());
        ReadOnlySpan<uint8_t> rawSpan(static_cast<const uint8_t*>(rawBitmap.Content()), rawBitmap.GetByteSize());
        return {rawSpan, rawBitmap.GetExtent(), rawBitmap.GetPixelFormat(), rawBitmap.GetOrigin()};
      }

      {    // Pack tightly
        const std::size_t minimumSize = minimumStride * rawBitmap.RawUnsignedHeight();
        std::vector<uint8_t> tightlyPackedBitmap(minimumSize);

        const auto* pSrc = static_cast<const uint8_t*>(rawBitmap.Content());
        const uint8_t* const pSrcEnd = pSrc + rawBitmap.GetByteSize();
        const uint32_t srcStride = rawBitmap.Stride();

        uint8_t* pDst = tightlyPackedBitmap.data();
        while (pSrc < pSrcEnd)
        {
          assert((pDst + minimumStride) <= (tightlyPackedBitmap.data() + tightlyPackedBitmap.size()));
          for (uint32_t x = 0; x < minimumStride; ++x)
          {
            pDst[x] = pSrc[x];
          }
          pSrc += srcStride;
          pDst += minimumStride;
        }
        return {std::move(tightlyPackedBitmap), rawBitmap.GetExtent(), rawBitmap.GetPixelFormat(), rawBitmap.GetOrigin()};
      }
    }

    TightBitmap Convert(BitmapMemory&& bitmapMemory)
    {
      const PxSize2D sizePx = bitmapMemory.GetSize();
      const PixelFormat pixelFormat = bitmapMemory.GetPixelFormat();
      const BitmapOrigin origin = bitmapMemory.GetOrigin();
      const uint32_t stride = bitmapMemory.Stride();

      const auto minimumStride = PixelFormatUtil::CalcMinimumStride(sizePx.Width(), pixelFormat);
      // Check if the source is tightly packed
      if (minimumStride == stride)
      {
        return {bitmapMemory.Release(), sizePx, pixelFormat, origin};
      }
      {    // Pack tightly 'in-place'
        assert(stride > minimumStride);
        std::vector<uint8_t> bitmapContent = bitmapMemory.Release();
        // The released bitmap content should be able to contain its bitmap
        assert(bitmapContent.size() == (stride * sizePx.RawUnsignedHeight()));

        const auto* pSrc = bitmapContent.data();
        const uint8_t* const pSrcEnd = pSrc + bitmapContent.size();

        uint8_t* pDst = bitmapContent.data();
        while (pSrc < pSrcEnd)
        {
          assert((pDst + minimumStride) <= (bitmapContent.data() + bitmapContent.size()));
          for (uint32_t x = 0; x < minimumStride; ++x)
          {
            pDst[x] = pSrc[x];
          }
          pSrc += stride;
          pDst += minimumStride;
        }

        // Resize the vector so it only fits the tightly packed bitmap size
        bitmapContent.resize(minimumStride * sizePx.RawUnsignedHeight());
        return {std::move(bitmapContent), sizePx, pixelFormat, origin};
      }
    }
  }

  // move assignment operator
  TightBitmap& TightBitmap::operator=(TightBitmap&& other) noexcept
  {
    if (this != &other)
    {
      // Free existing resources then transfer the content of other to this one and fill other with default values
      Reset();

      // Claim ownership here
      m_content = std::move(other.m_content);
      m_sizePx = other.m_sizePx;
      m_pixelFormat = other.m_pixelFormat;
      m_origin = other.m_origin;
      m_bytesPerPixel = other.m_bytesPerPixel;

      // Remove the data from other
      other.m_sizePx = {};
      other.m_pixelFormat = PixelFormat::Undefined;
      other.m_origin = BitmapOrigin::UpperLeft;
      other.m_bytesPerPixel = 0;
    }
    return *this;
  }

  TightBitmap::TightBitmap(TightBitmap&& other) noexcept
    : m_content(std::move(other.m_content))
    , m_sizePx(other.m_sizePx)
    , m_pixelFormat(other.m_pixelFormat)
    , m_origin(other.m_origin)
    , m_bytesPerPixel(other.m_bytesPerPixel)
  {
    other.m_sizePx = {};
    other.m_pixelFormat = PixelFormat::Undefined;
    other.m_origin = BitmapOrigin::UpperLeft;
    other.m_bytesPerPixel = 0;
  }


  TightBitmap::TightBitmap(const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin origin)
    : m_content(sizePx.RawHeight() * PixelFormatUtil::CalcMinimumStride(sizePx.Width(), pixelFormat))
    , m_sizePx(sizePx)
    , m_pixelFormat(pixelFormat)
    , m_origin(origin)
    , m_bytesPerPixel(UncheckedNumericCast<uint8_t>(PixelFormatUtil::GetBytesPerPixel(pixelFormat)))
  {
  }


  TightBitmap::TightBitmap(const PxExtent2D extentPx, const PixelFormat pixelFormat, const BitmapOrigin origin)
    : TightBitmap(TypeConverter::To<PxSize2D>(extentPx), pixelFormat, origin)
  {
  }


  TightBitmap::TightBitmap(const ReadOnlyRawBitmap& content)
    : TightBitmap(Convert(content))
  {
  }


  TightBitmap::TightBitmap(const RawBitmapEx& content)
    : TightBitmap(static_cast<const ReadOnlyRawBitmap&>(content))
  {
  }


  TightBitmap::TightBitmap(BitmapMemory&& content)
    : TightBitmap(Convert(std::move(content)))
  {
  }


  TightBitmap::TightBitmap(std::vector<uint8_t> content, const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin origin)
    : m_content(std::move(content))
    , m_sizePx(sizePx)
    , m_pixelFormat(pixelFormat)
    , m_origin(origin)
    , m_bytesPerPixel(UncheckedNumericCast<uint8_t>(PixelFormatUtil::GetBytesPerPixel(pixelFormat)))
  {
    if (m_content.size() != (m_bytesPerPixel * TypeConverter::UncheckedTo<PxValueU>(sizePx.Width() * sizePx.Height()).Value))
    {
      throw std::invalid_argument("The content does not match the tightly packed bitmap size");
    }
  }


  TightBitmap::TightBitmap(std::vector<uint8_t> content, const PxExtent2D extentPx, const PixelFormat pixelFormat, const BitmapOrigin origin)
    : TightBitmap(std::move(content), TypeConverter::To<PxSize2D>(extentPx), pixelFormat, origin)
  {
    if (m_content.size() != (m_bytesPerPixel * (extentPx.Width * extentPx.Height).Value))
    {
      throw std::invalid_argument("The content does not match the tightly packed bitmap size");
    }
  }


  void TightBitmap::Reset() noexcept
  {
    m_content.clear();
    m_sizePx = {};
    m_pixelFormat = PixelFormat::Undefined;
    m_origin = BitmapOrigin::UpperLeft;
    m_bytesPerPixel = 0;
  }


  ReadOnlySpan<uint8_t> TightBitmap::AsSpan() const noexcept
  {
    assert((m_bytesPerPixel * UncheckedNumericCast<std::size_t>(RawUnsignedWidth()) * RawUnsignedHeight()) == m_content.size());
    return SpanUtil::AsReadOnlySpan(m_content);
  }


  Span<uint8_t> TightBitmap::AsSpan() noexcept
  {
    assert((m_bytesPerPixel * UncheckedNumericCast<std::size_t>(RawUnsignedWidth()) * RawUnsignedHeight()) == m_content.size());
    return SpanUtil::AsSpan(m_content);
  }


  ReadOnlyRawBitmap TightBitmap::AsRawBitmap() const noexcept
  {
    return ReadOnlyRawBitmap::UncheckedCreate(SpanUtil::AsReadOnlySpan(m_content), GetSize(), GetPixelFormat(), GetOrigin());
  }


  RawBitmapEx TightBitmap::AsRawBitmap() noexcept
  {
    return RawBitmapEx::UncheckedCreate(SpanUtil::AsSpan(m_content), GetSize(), GetPixelFormat(), GetOrigin());
  }


  BitmapMemory TightBitmap::ToBitmapMemory() const
  {
    return BitmapMemory::UncheckedCreate(AsSpan(), m_sizePx, m_pixelFormat, m_origin);
  }


  BitmapMemory TightBitmap::Release() noexcept
  {
    std::vector<uint8_t> content = std::move(m_content);
    const PxSize2D sizePx = m_sizePx;
    const PixelFormat pixelFormat = m_pixelFormat;
    const BitmapOrigin origin = m_origin;

    Reset();
    return BitmapMemory::UncheckedCreate(std::move(content), sizePx, pixelFormat, origin);
  }


  std::vector<uint8_t> TightBitmap::ReleaseAsVector() noexcept
  {
    auto content = std::move(m_content);
    Reset();
    return content;
  }
}
