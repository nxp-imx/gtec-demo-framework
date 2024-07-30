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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Bitmap/BitmapMemory.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <cassert>
#include <utility>

namespace Fsl
{
  BitmapMemory& BitmapMemory::operator=(BitmapMemory&& other) noexcept
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
      m_stride = other.m_stride;

      // Remove the data from other
      other.m_sizePx = {};
      other.m_pixelFormat = PixelFormat::Undefined;
      other.m_origin = BitmapOrigin::UpperLeft;
      other.m_stride = 0;
    }
    return *this;
  }


  BitmapMemory::BitmapMemory(BitmapMemory&& other) noexcept
    : m_content(std::move(other.m_content))
    , m_sizePx(other.m_sizePx)
    , m_pixelFormat(other.m_pixelFormat)
    , m_origin(other.m_origin)
    , m_stride(other.m_stride)
  {
    other.m_sizePx = {};
    other.m_pixelFormat = PixelFormat::Undefined;
    other.m_origin = BitmapOrigin::UpperLeft;
    other.m_stride = 0;
  }


  uint32_t BitmapMemory::GetBytesPerPixel() const noexcept
  {
    return PixelFormatUtil::GetBytesPerPixel(m_pixelFormat);
  }


  std::vector<uint8_t> BitmapMemory::Release() noexcept
  {
    auto res = std::move(m_content);
    Reset();
    return res;
  }


  bool BitmapMemory::IsTightlyPacked() const noexcept
  {
    return PixelFormatUtil::CalcMinimumStride(m_sizePx.Width(), m_pixelFormat) == m_stride;
  }


  void BitmapMemory::Reset() noexcept
  {
    m_content.clear();
    m_sizePx = {};
    m_pixelFormat = PixelFormat::Undefined;
    m_origin = BitmapOrigin::UpperLeft;
    m_stride = 0;
  }

  BitmapMemory BitmapMemory::Create(const ReadOnlySpan<uint8_t> content, const PxSize2D sizePx, const PixelFormat pixelFormat,
                                    const BitmapOrigin origin)
  {
    const uint32_t minimumStride = PixelFormatUtil::CalcMinimumStride(sizePx.Width(), pixelFormat);

    if ((minimumStride * UncheckedNumericCast<uint64_t>(sizePx.RawHeight())) > content.size())
    {
      throw std::invalid_argument("The content is not of the expected size for a bitmap of that pixel format with the given stride");
    }

    return {SpanUtil::ToVector(content), sizePx, pixelFormat, origin, minimumStride};
  }


  BitmapMemory BitmapMemory::Create(const ReadOnlySpan<uint8_t> content, const PxSize2D sizePx, const PixelFormat pixelFormat,
                                    const BitmapOrigin origin, const uint32_t stride)
  {
    if (stride < PixelFormatUtil::CalcMinimumStride(sizePx.Width(), pixelFormat))
    {
      throw std::invalid_argument("stride is smaller than the width allows");
    }

    if ((stride * UncheckedNumericCast<uint64_t>(sizePx.RawHeight())) > content.size())
    {
      throw std::invalid_argument("The content is not of the expected size for a bitmap of that pixel format with the given stride");
    }

    return {SpanUtil::ToVector(content), sizePx, pixelFormat, origin, stride};
  }

  BitmapMemory BitmapMemory::Create(std::vector<uint8_t>&& content, const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin origin)
  {
    const uint32_t minimumStride = PixelFormatUtil::CalcMinimumStride(sizePx.Width(), pixelFormat);

    if ((minimumStride * UncheckedNumericCast<uint64_t>(sizePx.RawHeight())) > content.size())
    {
      throw std::invalid_argument("The content is not of the expected size for a bitmap of that pixel format with the given stride");
    }

    return {std::move(content), sizePx, pixelFormat, origin, minimumStride};
  }


  BitmapMemory BitmapMemory::Create(std::vector<uint8_t>&& content, const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin origin,
                                    const uint32_t stride)
  {
    if (stride < PixelFormatUtil::CalcMinimumStride(sizePx.Width(), pixelFormat))
    {
      throw std::invalid_argument("stride is smaller than the width allows");
    }

    if ((stride * UncheckedNumericCast<uint64_t>(sizePx.RawHeight())) > content.size())
    {
      throw std::invalid_argument("The content is not of the expected size for a bitmap of that pixel format with the given stride");
    }

    return {std::move(content), sizePx, pixelFormat, origin, stride};
  }


  BitmapMemory BitmapMemory::UncheckedCreate(const ReadOnlySpan<uint8_t> content, const PxSize2D sizePx, const PixelFormat pixelFormat,
                                             const BitmapOrigin origin)
  {
    const uint32_t minimumStride = PixelFormatUtil::CalcMinimumStride(sizePx.Width(), pixelFormat);
    return {SpanUtil::ToVector(content), sizePx, pixelFormat, origin, minimumStride};
  }


  BitmapMemory BitmapMemory::UncheckedCreate(const ReadOnlySpan<uint8_t> content, const PxSize2D sizePx, const PixelFormat pixelFormat,
                                             const BitmapOrigin origin, const uint32_t stride)
  {
    return {SpanUtil::ToVector(content), sizePx, pixelFormat, origin, stride};
  }


  BitmapMemory BitmapMemory::UncheckedCreate(std::vector<uint8_t>&& content, const PxSize2D sizePx, const PixelFormat pixelFormat,
                                             const BitmapOrigin origin)
  {
    const uint32_t minimumStride = PixelFormatUtil::CalcMinimumStride(sizePx.Width(), pixelFormat);
    return {std::move(content), sizePx, pixelFormat, origin, minimumStride};
  }


  BitmapMemory BitmapMemory::UncheckedCreate(std::vector<uint8_t>&& content, const PxSize2D sizePx, const PixelFormat pixelFormat,
                                             const BitmapOrigin origin, const uint32_t stride)
  {
    return {std::move(content), sizePx, pixelFormat, origin, stride};
  }


  BitmapMemory::BitmapMemory(std::vector<uint8_t> content, const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin origin,
                             const uint32_t stride)
    : m_content(std::move(content))
    , m_sizePx(sizePx)
    , m_pixelFormat(pixelFormat)
    , m_origin(origin)
    , m_stride(stride)
  {
    assert(stride >= PixelFormatUtil::CalcMinimumStride(m_sizePx.Width(), pixelFormat));
    assert((stride * UncheckedNumericCast<uint64_t>(sizePx.RawHeight())) <= m_content.size());
  }
}
