/****************************************************************************************************************************************************
* Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/RawBitmapUtil.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <utility>

namespace Fsl
{
  namespace
  {
    constexpr BitmapOrigin CheckBitmapOrigin(const BitmapOrigin origin)
    {
      return origin != BitmapOrigin::Undefined ? origin : BitmapOrigin::UpperLeft;
    }
  }


  // move assignment operator
  Bitmap& Bitmap::operator=(Bitmap&& other)
  {
    if (this != &other)
    {
      // Free existing resources then transfer the content of other to this one and fill other with default values
      ResetNoThrow();

      // Claim ownership here
      m_content = std::move(other.m_content);
      m_extent = other.m_extent;
      m_stride = other.m_stride;
      m_bytesPerPixel = other.m_bytesPerPixel;
      m_pixelFormat = other.m_pixelFormat;
      m_strideRequirement = other.m_strideRequirement;
      m_origin = other.m_origin;
      m_isLocked = other.m_isLocked;

      // Remove the data from other
      other.m_extent = Extent2D();
      other.m_stride = 0;
      other.m_bytesPerPixel = 0;
      other.m_pixelFormat = PixelFormat::Undefined;
      other.m_strideRequirement = StrideRequirement::Any;
      other.m_origin = BitmapOrigin::UpperLeft;
      other.m_isLocked = false;
    }
    return *this;
  }


  // Transfer ownership from other to this
  Bitmap::Bitmap(Bitmap&& other)
    : m_content(std::move(other.m_content))
    , m_extent(other.m_extent)
    , m_stride(other.m_stride)
    , m_bytesPerPixel(other.m_bytesPerPixel)
    , m_pixelFormat(other.m_pixelFormat)
    , m_strideRequirement(other.m_strideRequirement)
    , m_origin(other.m_origin)
    , m_isLocked(other.m_isLocked)
  {
    // Remove the data from other
    other.m_extent = Extent2D();
    other.m_stride = 0;
    other.m_bytesPerPixel = 0;
    other.m_pixelFormat = PixelFormat::Undefined;
    other.m_strideRequirement = StrideRequirement::Any;
    other.m_origin = BitmapOrigin::UpperLeft;
    other.m_isLocked = false;
  }


  Bitmap::Bitmap()
    : m_content()
    , m_extent()
    , m_stride(0)
    , m_bytesPerPixel(0)
    , m_pixelFormat(PixelFormat::Undefined)
    , m_strideRequirement(StrideRequirement::Any)
    , m_origin(BitmapOrigin::UpperLeft)
    , m_isLocked(false)
  {
  }


  Bitmap::Bitmap(const Extent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin bitmapOrigin)
    : m_content()
    , m_extent()
    , m_stride(0)
    , m_bytesPerPixel(0)
    , m_pixelFormat(pixelFormat)
    , m_strideRequirement(StrideRequirement::Any)
    , m_origin(CheckBitmapOrigin(bitmapOrigin))
    , m_isLocked(false)
  {
    ResizeToFit(extent, pixelFormat, m_strideRequirement);
  }


  Bitmap::Bitmap(const Extent2D& extent, const PixelFormat pixelFormat, const uint32_t stride, const BitmapOrigin bitmapOrigin)
    : m_content()
    , m_extent()
    , m_stride(0)
    , m_bytesPerPixel(0)
    , m_pixelFormat(pixelFormat)
    , m_strideRequirement(StrideRequirement::Any)
    , m_origin(CheckBitmapOrigin(bitmapOrigin))
    , m_isLocked(false)
  {
    ResizeToFit(extent, pixelFormat, m_strideRequirement, stride);
  }


  Bitmap::Bitmap(const void*const pContent, const std::size_t cbContent, const Extent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin bitmapOrigin)
    : m_content()
    , m_extent()
    , m_stride(0)
    , m_bytesPerPixel(0)
    , m_pixelFormat(pixelFormat)
    , m_strideRequirement(StrideRequirement::Any)
    , m_origin(CheckBitmapOrigin(bitmapOrigin))
    , m_isLocked(false)
  {
    if (pContent == nullptr)
      throw std::invalid_argument("pContent can not be null");

    ResizeToFit(extent, pixelFormat, m_strideRequirement);
    RawBitmap srcBitmap(pContent, extent, pixelFormat, m_stride, m_origin);
    RawBitmapEx dstBitmap(m_content.data(), m_extent, m_pixelFormat, m_stride, m_origin);
    RawBitmapUtil::MemoryCopy(dstBitmap, srcBitmap);
  }


  Bitmap::Bitmap(const void*const pContent, const std::size_t cbContent, const Extent2D& extent, const PixelFormat pixelFormat, const uint32_t stride, const BitmapOrigin bitmapOrigin)
    : m_content()
    , m_extent()
    , m_stride(0)
    , m_bytesPerPixel(0)
    , m_pixelFormat(pixelFormat)
    , m_strideRequirement(StrideRequirement::Any)
    , m_origin(CheckBitmapOrigin(bitmapOrigin))
    , m_isLocked(false)
  {
    if (pContent == nullptr)
      throw std::invalid_argument("pContent can not be null");
    if (stride < PixelFormatUtil::CalcMinimumStride(m_extent.Width, pixelFormat))
      throw std::invalid_argument("stride is smaller than the width allows");

    ResizeToFit(extent, pixelFormat, m_strideRequirement);
    RawBitmap srcBitmap(pContent, extent, pixelFormat, stride, m_origin);
    RawBitmapEx dstBitmap(m_content.data(), m_extent, m_pixelFormat, m_stride, m_origin);
    RawBitmapUtil::MemoryCopy(dstBitmap, srcBitmap);
  }


  Bitmap::Bitmap(const RawBitmap& srcBitmap)
    : m_content()
    , m_extent()
    , m_stride(0)
    , m_bytesPerPixel(0)
    , m_pixelFormat(srcBitmap.GetPixelFormat())
    , m_strideRequirement(StrideRequirement::Any)
    , m_origin(srcBitmap.GetOrigin())
    , m_isLocked(false)
  {
    if (srcBitmap.Content() == nullptr)
      throw std::invalid_argument("Content can not be null");

    ResizeToFit(srcBitmap.GetExtent(), srcBitmap.GetPixelFormat(), m_strideRequirement);
    RawBitmapEx dstBitmap(m_content.data(), m_extent, m_pixelFormat, m_stride, m_origin);
    RawBitmapUtil::MemoryCopy(dstBitmap, srcBitmap);
  }


  Bitmap::Bitmap(std::vector<uint8_t>&& content, const Extent2D& extent, const PixelFormat pixelFormat,
                 const BitmapOrigin bitmapOrigin)
    : Bitmap()
  {
    Reset(std::move(content), extent, pixelFormat, bitmapOrigin);
  }


  Bitmap::Bitmap(std::vector<uint8_t>&& content, const Extent2D& extent, const PixelFormat pixelFormat, const uint32_t stride,
                 const BitmapOrigin bitmapOrigin)
    : Bitmap()
  {
    Reset(std::move(content), extent, pixelFormat, stride, bitmapOrigin);
  }


  Bitmap::Bitmap(const RawBitmap& srcBitmap, const BitmapOrigin desiredOrigin)
    : m_content()
    , m_extent()
    , m_stride(0)
    , m_bytesPerPixel(0)
    , m_pixelFormat(srcBitmap.GetPixelFormat())
    , m_strideRequirement(StrideRequirement::Any)
    , m_origin(srcBitmap.GetOrigin())
    , m_isLocked(false)
  {
    if (srcBitmap.Content() == nullptr)
      throw std::invalid_argument("Content can not be null");

    ResizeToFit(srcBitmap.GetExtent(), srcBitmap.GetPixelFormat(), m_strideRequirement);
    RawBitmapEx dstBitmap(m_content.data(), m_extent, m_pixelFormat, m_stride, m_origin);
    RawBitmapUtil::MemoryCopy(dstBitmap, srcBitmap);

    if (srcBitmap.GetOrigin() != desiredOrigin)
      RawBitmapUtil::FlipHorizontal(dstBitmap);
    m_origin = dstBitmap.GetOrigin();
  }


  Bitmap::~Bitmap()
  {
    FSLLOG_WARNING_IF(m_isLocked, "Destroying a locked bitmap, the content being accessed will no longer be available");
  }


  uint32_t Bitmap::Width() const
  {
    return m_extent.Width;
  }


  uint32_t Bitmap::Height() const
  {
    return m_extent.Height;
  }


  Point2 Bitmap::Size() const
  {
    return Point2(m_extent.Width, m_extent.Height);
  }


  uint32_t Bitmap::Stride() const
  {
    return m_stride;
  }

  //! @brief The size of the bitmap in pixels
  Extent2D Bitmap::GetExtent() const
  {
    return m_extent;
  }


  PixelFormat Bitmap::GetPixelFormat() const
  {
    return m_pixelFormat;
  }


  PixelFormatLayout Bitmap::GetPixelFormatLayout() const
  {
    return PixelFormatUtil::GetPixelFormatLayout(m_pixelFormat);
  }


  StrideRequirement Bitmap::GetStrideRequirement() const
  {
    return m_strideRequirement;
  }


  BitmapOrigin Bitmap::GetOrigin() const
  {
    return m_origin;
  }


  uint32_t Bitmap::GetPreferredStride(const PixelFormat pixelFormat) const
  {
    return (pixelFormat == m_pixelFormat ? m_stride : PixelFormatUtil::CalcMinimumStride(m_extent.Width, pixelFormat, m_strideRequirement));
  }


  void Bitmap::ReleaseInto(std::vector<uint8_t>& rContentTarget)
  {
    // Reset() should not throw, but this warrants a program stop since its a critical error
    if (m_isLocked)
      throw UsageErrorException("Can not Release a locked bitmap, that would invalidate the content being accessed");

    // Get the current content array, then reset this object
    rContentTarget = std::move(m_content);
    ResetNoThrow();
  }


  void Bitmap::Reset()
  {
    // Reset() should not throw, but this warrants a program stop since its a critical error
    if (m_isLocked)
      throw UsageErrorException("Can not reset a locked bitmap, that would invalidate the content being accessed");

    ResetNoThrow();
  }


  void Bitmap::Reset(const Extent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin bitmapOrigin, const BitmapClearMethod clearMethod)
  {
    if (m_isLocked)
      throw UsageErrorException("The bitmap is locked");

    if (extent != m_extent || pixelFormat != m_pixelFormat)
    {
      ResizeToFit(extent, pixelFormat, StrideRequirement::Any);
    }
    if (clearMethod != BitmapClearMethod::DontModify)
      Clear(clearMethod);

    m_origin = CheckBitmapOrigin(bitmapOrigin);
  }


  void Bitmap::Reset(const Extent2D& extent, const PixelFormat pixelFormat, const uint32_t stride, const BitmapOrigin bitmapOrigin, const BitmapClearMethod clearMethod)
  {
    if (m_isLocked)
      throw UsageErrorException("The bitmap is locked");
    if (extent != m_extent || pixelFormat != m_pixelFormat || stride != m_stride)
    {
      ResizeToFit(extent, pixelFormat, StrideRequirement::Any, stride);
    }
    if (clearMethod != BitmapClearMethod::DontModify)
      Clear(clearMethod);

    m_origin = CheckBitmapOrigin(bitmapOrigin);
  }


  void Bitmap::Reset(const void*const pContent, const std::size_t cbContent, const Extent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin bitmapOrigin)
  {
    if (m_isLocked)
      throw UsageErrorException("The bitmap is locked");
    if (pContent == nullptr)
      throw std::invalid_argument("pContent can not be null");
    if (cbContent != (PixelFormatUtil::CalcMinimumStride(extent.Width, pixelFormat) * extent.Height))
      throw std::invalid_argument("The image buffer is not of the expected size for a image of that pixel format supplied without a stride parameter");

    ResizeToFit(extent, pixelFormat, StrideRequirement::Any);
    RawBitmap srcBitmap(pContent, extent, pixelFormat, bitmapOrigin);
    RawBitmapEx dstBitmap(m_content.data(), m_extent, m_pixelFormat, m_stride, bitmapOrigin);
    RawBitmapUtil::MemoryCopy(dstBitmap, srcBitmap);
    m_origin = dstBitmap.GetOrigin();
  }


  void Bitmap::Reset(const void*const pContent, const std::size_t cbContent, const Extent2D& extent, const PixelFormat pixelFormat, const uint32_t stride, const BitmapOrigin bitmapOrigin)
  {
    if (m_isLocked)
      throw UsageErrorException("The bitmap is locked");
    if (pContent == nullptr)
      throw std::invalid_argument("pContent can not be null");
    if (stride < PixelFormatUtil::CalcMinimumStride(extent.Width, pixelFormat))
      throw std::invalid_argument("stride is smaller than the width allows");
    if (cbContent != (stride * extent.Height))
      throw std::invalid_argument("The image buffer is not of the expected size for a image of that pixel format with the given stride");

    ResizeToFit(extent, pixelFormat, StrideRequirement::Any);
    RawBitmap srcBitmap(pContent, extent, pixelFormat, stride, bitmapOrigin);
    RawBitmapEx dstBitmap(m_content.data(), m_extent, m_pixelFormat, m_stride, bitmapOrigin);
    RawBitmapUtil::MemoryCopy(dstBitmap, srcBitmap);
    m_origin = dstBitmap.GetOrigin();
  }


  void Bitmap::Reset(const RawBitmap& srcBitmap)
  {
    if (m_isLocked)
      throw UsageErrorException("The bitmap is locked");
    if (!srcBitmap.IsValid())
      throw std::invalid_argument("invalid srcBitmap");

    Reset(srcBitmap.GetExtent(), srcBitmap.GetPixelFormat());
    RawBitmapEx dstBitmap(m_content.data(), m_extent, m_pixelFormat, m_stride, srcBitmap.GetOrigin());
    RawBitmapUtil::MemoryCopy(dstBitmap, srcBitmap);
    m_origin = dstBitmap.GetOrigin();
  }


  void Bitmap::Reset(std::vector<uint8_t>&& content, const Extent2D& extent, const PixelFormat pixelFormat,
                     const BitmapOrigin bitmapOrigin)
  {
    const uint32_t minStride = PixelFormatUtil::CalcMinimumStride(extent.Width, pixelFormat);
    Reset(std::move(content), extent, pixelFormat, minStride, bitmapOrigin);
  }


  void Bitmap::Reset(std::vector<uint8_t>&& content, const Extent2D& extent, const PixelFormat pixelFormat, const uint32_t stride,
                     const BitmapOrigin bitmapOrigin)
  {
    if (m_isLocked)
      throw UsageErrorException("The bitmap is locked");

    const uint32_t bytesPerPixel = PixelFormatUtil::GetBytesPerPixel(pixelFormat);
    const uint32_t minStride = PixelFormatUtil::CalcMinimumStride(extent.Width, bytesPerPixel);
    if (stride < minStride)
      throw std::invalid_argument("stride is smaller than the width allows");

    const std::size_t totalByteSize = (extent.Height * stride);
    if (content.size() != totalByteSize)
      throw std::invalid_argument("the content buffer size is does not match the described content");

    m_content = std::move(content);
    m_extent = extent;
    m_stride = stride;
    m_bytesPerPixel = bytesPerPixel;
    m_pixelFormat = pixelFormat;
    m_strideRequirement = StrideRequirement::Any;
    m_origin = CheckBitmapOrigin(bitmapOrigin);
  }


  void Bitmap::SetNativePixel(const int32_t x, const int32_t y, const uint32_t color)
  {
    if (x < 0 || y < 0 || static_cast<uint32_t>(x) >= m_extent.Width || static_cast<uint32_t>(y) >= m_extent.Height)
      return;

    const int actualY = (m_origin == BitmapOrigin::UpperLeft ? y : m_extent.Height - 1 - y);

    uint8_t* pDst = reinterpret_cast<uint8_t*>(m_content.data());
    switch (PixelFormatUtil::GetPixelFormatLayout(m_pixelFormat))
    {
    case PixelFormatLayout::R8:
      pDst[x + (actualY*m_stride)] = static_cast<uint8_t>(color & 0xFF);
      break;
    case PixelFormatLayout::R8G8B8:
    case PixelFormatLayout::B8G8R8:
      pDst += (x * 3) + (actualY*m_stride);
      pDst[0] = static_cast<uint8_t>(color & 0xFF);
      pDst[1] = static_cast<uint8_t>((color >> 8) & 0xFF);
      pDst[2] = static_cast<uint8_t>((color >> 16) & 0xFF);
      break;
    case PixelFormatLayout::R8G8B8A8:
    case PixelFormatLayout::B8G8R8A8:
      pDst += (x * 4) + (actualY*m_stride);
      pDst[0] = static_cast<uint8_t>(color & 0xFF);
      pDst[1] = static_cast<uint8_t>((color >> 8) & 0xFF);
      pDst[2] = static_cast<uint8_t>((color >> 16) & 0xFF);
      pDst[3] = static_cast<uint8_t>((color >> 24) & 0xFF);
      break;
    default:
      throw UnsupportedPixelFormatException(m_pixelFormat);
    }
  }


  void Bitmap::SetNativePixel(const uint32_t x, const uint32_t y, const uint32_t color)
  {
    SetNativePixel(static_cast<int32_t>(x), static_cast<int32_t>(y), color);
  }


  uint32_t Bitmap::GetNativePixel(const int32_t x, const int32_t y) const
  {
    if (x < 0 || y < 0 || static_cast<uint32_t>(x) >= m_extent.Width || static_cast<uint32_t>(y) >= m_extent.Height)
      return 0;

    const int actualY = (m_origin == BitmapOrigin::UpperLeft ? y : m_extent.Height - 1 - y);

    const uint8_t*pSrc = reinterpret_cast<const uint8_t*>(m_content.data());
    switch (PixelFormatUtil::GetPixelFormatLayout(m_pixelFormat))
    {
    case PixelFormatLayout::R8:
      return pSrc[x + (actualY*m_stride)];
    case PixelFormatLayout::R8G8B8:
    case PixelFormatLayout::B8G8R8:
      pSrc += (x * 3) + (actualY*m_stride);
      return static_cast<uint32_t>(pSrc[0]) | (static_cast<uint32_t>(pSrc[1]) << 8) | (static_cast<uint32_t>(pSrc[2]) << 16);
    case PixelFormatLayout::R8G8B8A8:
    case PixelFormatLayout::B8G8R8A8:
      pSrc += (x * 4) + (actualY*m_stride);
      return static_cast<uint32_t>(pSrc[0]) | (static_cast<uint32_t>(pSrc[1]) << 8) | (static_cast<uint32_t>(pSrc[2]) << 16) | (static_cast<uint32_t>(pSrc[3]) << 24);
    default:
      throw UnsupportedPixelFormatException(m_pixelFormat);
    }
  }


  uint32_t Bitmap::GetNativePixel(const uint32_t x, const uint32_t y) const
  {
    return GetNativePixel(static_cast<int32_t>(x), static_cast<int32_t>(y));
  }


  void Bitmap::SetUInt8(const uint32_t x, const uint32_t y, const uint8_t value, const bool ignoreOrigin)
  {
    const auto byteWidth = m_bytesPerPixel * m_extent.Width;
    if (x >= byteWidth || y >= m_extent.Height)
    {
      FSLLOG_DEBUG_WARNING("SetUInt8 out of bounds x:" << x << " y: " << y);
      return;
    }

    const uint32_t actualY = ignoreOrigin ? y : (m_origin == BitmapOrigin::UpperLeft ? y : m_extent.Height - 1 - y);
    m_content[x + (actualY*m_stride)] = value;
  }


  uint8_t Bitmap::GetUInt8(const uint32_t x, const uint32_t y, const bool ignoreOrigin) const
  {
    const auto byteWidth = m_bytesPerPixel * m_extent.Width;
    if (x >= byteWidth || y >= m_extent.Height)
    {
      FSLLOG_DEBUG_WARNING("GetUInt8 out of bounds x:" << x << " y: " << y);
      return 0;
    }

    const uint32_t actualY = ignoreOrigin ? y : (m_origin == BitmapOrigin::UpperLeft ? y : m_extent.Height - 1 - y);
    return m_content[x + (actualY*m_stride)];
  }


  void Bitmap::SetCompatiblePixelFormat(const PixelFormat compatiblePixelFormat)
  {
    if (m_isLocked)
      throw UsageErrorException("The bitmap is already locked");

    if (PixelFormatUtil::GetPixelFormatLayout(m_pixelFormat) != PixelFormatUtil::GetPixelFormatLayout(compatiblePixelFormat))
      throw UsageErrorException("The supplied pixel format was not compatible");
    m_pixelFormat = compatiblePixelFormat;
  }


  bool Bitmap::TrySetCompatiblePixelFormatFlag(const PixelFormatFlags::Enum flag)
  {
    if (m_isLocked)
    {
      FSLLOG_DEBUG_WARNING("The bitmap is already locked");
      return false;
    }

    const auto newPixelFormat = PixelFormatUtil::TrySetCompatiblePixelFormatFlag(m_pixelFormat, flag);
    if (newPixelFormat == PixelFormat::Undefined)
      return false;
    m_pixelFormat = newPixelFormat;
    return true;
  }


  RawBitmap Bitmap::Lock() const
  {
    try
    {
      if (m_isLocked)
        throw UsageErrorException("The bitmap is already locked");
      m_isLocked = true;
      return RawBitmap(m_content.data(), m_extent, m_pixelFormat, m_stride, m_origin);
    }
    catch (const std::exception&)
    {
      m_isLocked = false;
      throw;
    }
  }


  RawBitmapEx Bitmap::LockEx()
  {
    try
    {
      if (m_isLocked)
      throw UsageErrorException("The bitmap is already locked");
      m_isLocked = true;
      return RawBitmapEx(m_content.data(), m_extent, m_pixelFormat, m_stride, m_origin);
    }
    catch (const std::exception&)
    {
      m_isLocked = false;
      throw;
    }
  }


  void Bitmap::UnlockEx(const RawBitmapEx& bitmap)
  {
    if (!m_isLocked)
      throw UsageErrorException("The bitmap is not locked");

    BitmapOrigin currentOrigin = bitmap.GetOrigin();
    if (currentOrigin != m_origin)
      m_origin = CheckBitmapOrigin(currentOrigin);

    m_isLocked = false;
  }


  void Bitmap::Unlock() const
  {
    if (!m_isLocked)
      throw UsageErrorException("The bitmap is not locked");
    m_isLocked = false;
  }


  void Bitmap::ResizeToFit(const Extent2D& extent, const PixelFormat pixelFormat, const StrideRequirement strideRequirement, const uint32_t stride)
  {
    const uint32_t bytesPerPixel = PixelFormatUtil::GetBytesPerPixel(pixelFormat);
    const uint32_t minStride = PixelFormatUtil::CalcMinimumStride(extent.Width, bytesPerPixel, strideRequirement);
    uint32_t chosenStride;

    if (stride == 0)
    {
      chosenStride = minStride;
    }
    else
    {
      if( ! PixelFormatUtil::IsValidStride(extent.Width, bytesPerPixel, strideRequirement, stride) )
        throw std::invalid_argument("Supplied stride does not obey the StrideRequirement");

      chosenStride = stride;
    }

    const std::size_t totalByteSize = (extent.Height * chosenStride);
    if (m_content.size() != totalByteSize)
      m_content.resize(totalByteSize);

    // Update the members
    m_extent = extent;
    m_stride = chosenStride;
    m_bytesPerPixel = bytesPerPixel;
    m_pixelFormat = pixelFormat;
    m_strideRequirement = strideRequirement;
  }


  void Bitmap::Clear(const BitmapClearMethod clearMethod)
  {
    switch (clearMethod)
    {
    case BitmapClearMethod::Clear:
      std::fill(m_content.begin(), m_content.end(), 0);
      break;
    case BitmapClearMethod::DontClear:
      if (PixelFormatUtil::CalcMinimumStride(m_extent.Width, m_pixelFormat) != m_stride)
      {
        RawBitmapEx dstBitmap(m_content.data(), m_extent, m_pixelFormat, m_stride, m_origin);
        RawBitmapUtil::ClearPadding(dstBitmap);
      }
      break;
    case BitmapClearMethod::DontModify:
      break;
    default:
      throw NotSupportedException("Unknown BitmapClearMethod");
    }
  }


  void Bitmap::ResetNoThrow()
  {
    FSLLOG_WARNING_IF(m_isLocked, "Destroying a locked bitmap, the content being accessed will no longer be available");
    m_content.clear();
    m_extent = Extent2D();
    m_stride = 0;
    m_bytesPerPixel = 0;
    m_pixelFormat = PixelFormat::Undefined;
    m_strideRequirement = StrideRequirement::Any;
    m_origin = BitmapOrigin::UpperLeft;
    m_isLocked = false;
  }


}
