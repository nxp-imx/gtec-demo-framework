#ifndef FSLGRAPHICS_BITMAP_BITMAP_HPP
#define FSLGRAPHICS_BITMAP_BITMAP_HPP
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

#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslGraphics/Bitmap/BitmapClearMethod.hpp>
#include <FslGraphics/Bitmap/BitmapMemory.hpp>
#include <FslGraphics/Bitmap/BitmapOrigin.hpp>
#include <FslGraphics/Bitmap/RawBitmapEx.hpp>
#include <FslGraphics/Bitmap/ReadOnlyRawBitmap.hpp>
#include <FslGraphics/StrideRequirement.hpp>
#include <cstdlib>
#include <vector>

namespace Fsl
{
  //@note All the methods that will default to using StrideRequirement::Any will use the minimum stride for allocation of the bitmap.
  //      The none part is just set to mean that there are no special requirements that should be maintained for the bitmap if its
  //      modified.
  class Bitmap
  {
    //! The raw image data
    std::vector<uint8_t> m_content;
    PxSize2D m_sizePx;
    uint32_t m_stride{0};
    uint32_t m_bytesPerPixel{0};
    PixelFormat m_pixelFormat{PixelFormat::Undefined};
    StrideRequirement m_strideRequirement{StrideRequirement::Any};
    BitmapOrigin m_origin{BitmapOrigin::UpperLeft};
    mutable bool m_isLocked{false};

  public:
    // move assignment operator
    Bitmap& operator=(Bitmap&& other) noexcept;
    // move constructor
    Bitmap(Bitmap&& other) noexcept;
    // Request that the compiler generates a copy constructor and assignment operator
    Bitmap(const Bitmap&) = default;
    Bitmap& operator=(const Bitmap&) = default;

    //! @brief Create a empty bitmap
    Bitmap();

    //! @brief Create a empty bitmap of the given width, height and pixel format (The default stride preference is StrideRequirement::Any)
    Bitmap(const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);

    //! @brief Create a empty bitmap of the given width, height, pixel format and stride requirement
    Bitmap(const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin bitmapOrigin, const StrideRequirement strideRequirement);

    //! @brief Create a empty bitmap of the given width, height, pixel format and stride (The default stride preference is StrideRequirement::Any)
    Bitmap(const PxSize2D sizePx, const PixelFormat pixelFormat, const uint32_t stride, const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);

    //! @brief Create a empty bitmap of the given width, height and pixel format (The default stride preference is StrideRequirement::Any)
    Bitmap(const PxExtent2D extent, const PixelFormat pixelFormat, const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);

    //! @brief Create a empty bitmap of the given width, height, pixel format and stride (The default stride preference is StrideRequirement::Any)
    Bitmap(const PxExtent2D extent, const PixelFormat pixelFormat, const uint32_t stride, const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);


    explicit Bitmap(BitmapMemory&& bitmapMemory);

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //         The input bitmap is assumed to use the minimum stride and the created bitmap will use StrideRequirement::Any to find its stride.
    Bitmap(const ReadOnlySpan<uint8_t> contentSpan, const PxSize2D sizePx, const PixelFormat pixelFormat,
           const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //         The created bitmap will use StrideRequirement::Any to find its stride.
    Bitmap(const ReadOnlySpan<uint8_t> contentSpan, const PxSize2D sizePx, const PixelFormat pixelFormat, const uint32_t stride,
           const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //         The input bitmap is assumed to use the minimum stride and the created bitmap will use StrideRequirement::Any to find its stride.
    Bitmap(const ReadOnlySpan<uint8_t> contentSpan, const PxExtent2D extent, const PixelFormat pixelFormat,
           const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //         The created bitmap will use StrideRequirement::Any to find its stride.
    Bitmap(const ReadOnlySpan<uint8_t> contentSpan, const PxExtent2D extent, const PixelFormat pixelFormat, const uint32_t stride,
           const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //         The input bitmap is assumed to use the minimum stride and the created bitmap will use StrideRequirement::Any to find its stride.
    [[deprecated("use the span version instead")]] Bitmap(const void* const pContent, const std::size_t cbContent, const PxExtent2D extent,
                                                          const PixelFormat pixelFormat, const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //         The created bitmap will use StrideRequirement::Any to find its stride.
    [[deprecated("use the span version instead")]] Bitmap(const void* const pContent, const std::size_t cbContent, const PxExtent2D extent,
                                                          const PixelFormat pixelFormat, const uint32_t stride,
                                                          const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //         The created bitmap will use StrideRequirement::Any to find its stride.
    Bitmap(const ReadOnlyRawBitmap& srcBitmap);    // NOLINT(google-explicit-constructor)

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //         The created bitmap will use StrideRequirement::Any to find its stride.
    //! @param desiredOrigin the origin that the created bitmap should use (if the src bitmap differs it is converted)
    Bitmap(const ReadOnlyRawBitmap& srcBitmap, const BitmapOrigin desiredOrigin);

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //         The created bitmap will use StrideRequirement::Any to find its stride.
    Bitmap(std::vector<uint8_t>&& content, const PxSize2D sizePx, const PixelFormat pixelFormat,
           const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //!        The created bitmap will use StrideRequirement::Any to find its stride.
    Bitmap(std::vector<uint8_t>&& content, const PxExtent2D extent, const PixelFormat pixelFormat,
           const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //!        The created bitmap will use StrideRequirement::Any to find its stride.
    Bitmap(std::vector<uint8_t>&& content, const PxSize2D sizePx, const PixelFormat pixelFormat, const uint32_t stride,
           const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //!         The created bitmap will use StrideRequirement::Any to find its stride.
    Bitmap(std::vector<uint8_t>&& content, const PxExtent2D extent, const PixelFormat pixelFormat, const uint32_t stride,
           const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);

    //! Just some ease of use overloads
    Bitmap(const int32_t width, const int32_t height, const PixelFormat pixelFormat, const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft)
      : Bitmap(PxSize2D::Create(width, height), pixelFormat, bitmapOrigin)
    {
    }

    ~Bitmap();

    bool IsValid() const noexcept

    {
      return m_content.data() != nullptr;
    }

    //! @brief The width of the bitmap in pixels
    PxSize1D Width() const noexcept
    {
      return m_sizePx.Width();
    }

    //! @brief The height of the bitmap in pixels
    PxSize1D Height() const noexcept
    {
      return m_sizePx.Height();
    }

    //! @brief The width of the bitmap in pixels
    PxValueU UnsignedWidth() const noexcept
    {
      return m_sizePx.UnsignedWidth();
    }

    //! @brief The height of the bitmap in pixels
    PxValueU UnsignedHeight() const noexcept
    {
      return m_sizePx.UnsignedHeight();
    }

    //! @brief The width of the bitmap in pixels
    int32_t RawWidth() const noexcept
    {
      return m_sizePx.RawWidth();
    }

    //! @brief The height of the bitmap in pixels
    int32_t RawHeight() const noexcept
    {
      return m_sizePx.RawHeight();
    }

    //! @brief The width of the bitmap in pixels
    uint32_t RawUnsignedWidth() const noexcept
    {
      return m_sizePx.RawUnsignedWidth();
    }

    //! @brief The height of the bitmap in pixels
    uint32_t RawUnsignedHeight() const noexcept
    {
      return m_sizePx.RawUnsignedHeight();
    }

    //! @brief The size of the bitmap in pixels
    [[deprecated("Prefer GetSize as it follows the convention used by the other bitmap related classes")]] PxSize2D Size() const noexcept
    {
      return GetSize();
    }

    //! @brief The size of the bitmap in pixels
    PxSize2D GetSize() const noexcept
    {
      return m_sizePx;
    }


    //! @brief The stride of the bitmap in bytes.
    //! @warning This will be greater or equal to Width * BytesPerPixel as there might be padding bytes!
    uint32_t Stride() const noexcept
    {
      return m_stride;
    }

    //! @brief The size of the bitmap in pixels
    PxExtent2D GetExtent() const noexcept
    {
      return {m_sizePx.UnsignedWidth(), m_sizePx.UnsignedHeight()};
    }

    //! @brief Get the origin of the bitmap
    BitmapOrigin GetOrigin() const noexcept
    {
      return m_origin;
    }

    //! @brief Get the bitmap stride preference.
    StrideRequirement GetStrideRequirement() const noexcept
    {
      return m_strideRequirement;
    }

    //! @brief Get the pixel format of the raw bitmap
    PixelFormat GetPixelFormat() const noexcept
    {
      return m_pixelFormat;
    }

    //! @brief Get the pixel format layout of the raw bitmap
    PixelFormatLayout GetPixelFormatLayout() const noexcept;

    //! @brief Get the stride that this bitmap would prefer given it had to be stored in the given pixel format.
    //! @note  This takes into account the current StrideRequirement of the bitmap.
    uint32_t GetPreferredStride(const PixelFormat pixelFormat) const;

    //! @brief Release the internal content array into the supplied vector, then reset this class
    [[deprecated("used Release instead")]] void ReleaseInto(std::vector<uint8_t>& rContentTarget);

    //! @brief Release the internal vector and information into a BitmapMemory object and 'reset' the class
    [[nodiscard]] BitmapMemory Release();

    uint8_t GetBytesPerPixel() const noexcept;

    //! @brief Get the byte size of the image
    std::size_t GetByteSize() const noexcept
    {
      const std::size_t stride = m_stride;
      return m_sizePx.RawUnsignedHeight() * stride;
    }

    void Reset();

    //! @brief Reset the image to the given dimensions.
    //! @param clearMethod defines what to do with the existing content
    //! @note This resets the stride preference to StrideRequirement::Any
    void Reset(const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft,
               const BitmapClearMethod clearMethod = BitmapClearMethod::Clear);

    //! @brief Reset the image to the given dimensions.
    //! @param clearMethod defines what to do with the existing content
    //! @note This resets the stride preference to StrideRequirement::Any
    void Reset(const PxExtent2D extent, const PixelFormat pixelFormat, const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft,
               const BitmapClearMethod clearMethod = BitmapClearMethod::Clear);

    //! @brief Reset the image to the given dimensions.
    //! @param clearMethod defines what to do with the existing content
    //! @note This resets the stride preference to StrideRequirement::Any
    void Reset(const PxSize2D sizePx, const PixelFormat pixelFormat, const uint32_t stride, const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft,
               const BitmapClearMethod clearMethod = BitmapClearMethod::Clear);

    //! @brief Reset the image to the given dimensions.
    //! @param clearMethod defines what to do with the existing content
    //! @note This resets the stride preference to StrideRequirement::Any
    void Reset(const PxExtent2D extent, const PixelFormat pixelFormat, const uint32_t stride,
               const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft, const BitmapClearMethod clearMethod = BitmapClearMethod::Clear);

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //! @note This resets the stride preference to StrideRequirement::Any
    void Reset(BitmapMemory&& bitmapMemory);

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //         The input bitmap is assumed to use the minimum stride and the created bitmap will use StrideRequirement::Any to find its stride.
    //! @note This resets the stride preference to StrideRequirement::Any
    void Reset(const ReadOnlySpan<uint8_t> contentSpan, const PxSize2D sizePx, const PixelFormat pixelFormat,
               const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //         The input bitmap is assumed to use the minimum stride and the created bitmap will use StrideRequirement::Any to find its stride.
    //! @note This resets the stride preference to StrideRequirement::Any
    void Reset(const ReadOnlySpan<uint8_t> contentSpan, const PxExtent2D extent, const PixelFormat pixelFormat,
               const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //         The created bitmap will use StrideRequirement::Any to find its stride.
    //! @note This resets the stride preference to StrideRequirement::Any
    void Reset(const ReadOnlySpan<uint8_t> contentSpan, const PxSize2D sizePx, const PixelFormat pixelFormat, const uint32_t stride,
               const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //         The created bitmap will use StrideRequirement::Any to find its stride.
    //! @note This resets the stride preference to StrideRequirement::Any
    void Reset(const ReadOnlySpan<uint8_t> contentSpan, const PxExtent2D extent, const PixelFormat pixelFormat, const uint32_t stride,
               const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //         The input bitmap is assumed to use the minimum stride and the created bitmap will use StrideRequirement::Any to find its stride.
    //! @note This resets the stride preference to StrideRequirement::Any
    [[deprecated("use the span version instead")]] void Reset(const void* const pContent, const std::size_t cbContent, const PxExtent2D extent,
                                                              const PixelFormat pixelFormat,
                                                              const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //         The created bitmap will use StrideRequirement::Any to find its stride.
    //! @note This resets the stride preference to StrideRequirement::Any
    [[deprecated("use the span version instead")]] void Reset(const void* const pContent, const std::size_t cbContent, const PxExtent2D extent,
                                                              const PixelFormat pixelFormat, const uint32_t stride,
                                                              const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //         The created bitmap will use StrideRequirement::Any to find its stride.
    //! @note This resets the stride preference to StrideRequirement::Any
    void Reset(const ReadOnlyRawBitmap& srcBitmap);


    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //         The created bitmap will use StrideRequirement::Any to find its stride.
    //! @note This resets the stride preference to StrideRequirement::Any
    void Reset(std::vector<uint8_t>&& content, const PxSize2D sizePx, const PixelFormat pixelFormat,
               const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //         The created bitmap will use StrideRequirement::Any to find its stride.
    //! @note This resets the stride preference to StrideRequirement::Any
    void Reset(std::vector<uint8_t>&& content, const PxExtent2D extent, const PixelFormat pixelFormat,
               const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);


    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //         The created bitmap will use StrideRequirement::Any to find its stride.
    //! @note This resets the stride preference to StrideRequirement::Any
    void Reset(std::vector<uint8_t>&& content, const PxSize2D sizePx, const PixelFormat pixelFormat, const uint32_t stride,
               const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);

    //! @brief Create a bitmap from the supplied content (the content will be copied).
    //         The created bitmap will use StrideRequirement::Any to find its stride.
    //! @note This resets the stride preference to StrideRequirement::Any
    void Reset(std::vector<uint8_t>&& content, const PxExtent2D extent, const PixelFormat pixelFormat, const uint32_t stride,
               const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft);


    void Reset(const int32_t width, const int32_t height, const PixelFormat pixelFormat, const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft,
               const BitmapClearMethod clearMethod = BitmapClearMethod::Clear)
    {
      Reset(PxSize2D::Create(width, height), pixelFormat, bitmapOrigin, clearMethod);
    }

    void Reset(const int32_t width, const int32_t height, const PixelFormat pixelFormat, const uint32_t stride,
               const BitmapOrigin bitmapOrigin = BitmapOrigin::UpperLeft, const BitmapClearMethod clearMethod = BitmapClearMethod::Clear)
    {
      Reset(PxSize2D::Create(width, height), pixelFormat, stride, bitmapOrigin, clearMethod);
    }

    //! @brief clear the content to zero
    void Clear();

    //! @brief set the pixel at the given coordinate (if the coordinate is outside the bitmap nothing happens)
    //! @note SetNativePixel always treats 0,0 as the upper left corner regardless of the origin
    //! @note This is a very slow operation
    void SetNativePixel(const int32_t x, const int32_t y, const uint32_t color);

    //! @brief set the pixel at the given coordinate (if the coordinate is outside the bitmap nothing happens)
    //! @note SetNativePixel always treats 0,0 as the upper left corner regardless of the origin
    //! @note This is a very slow operation
    void SetNativePixel(const uint32_t x, const uint32_t y, const uint32_t color);

    //! @brief read the pixel at the given coordinate (if the coordinate is outside the bitmap zero is returned)
    //! @note GetNativePixel always treats 0,0 as the upper left corner regardless of the origin
    //! @note This is a very slow operation
    uint32_t GetNativePixel(const int32_t x, const int32_t y) const;

    //! @brief read the pixel at the given coordinate (if the coordinate is outside the bitmap zero is returned)
    //! @note GetNativePixel always treats 0,0 as the upper left corner regardless of the origin
    //! @note This is a very slow operation
    uint32_t GetNativePixel(const uint32_t x, const uint32_t y) const;

    //! @brief Set the uint8_t at the the given position.
    //! @param x = the x byte position to read (NOT PIXEL).
    //! @param y = the y position (this is equal to a pixel Y position).
    //! @param value = the byte value to write.
    //! @note Writing outside the bitmap will do nothing (a warning will be logged in debug builds)
    void SetUInt8(const uint32_t x, const uint32_t y, const uint8_t value, const bool ignoreOrigin = false);

    //! @brief read the uint8_t at the the given position.
    //! @param x = the x byte position to read (NOT PIXEL).
    //! @param y = the y position (this is equal to a pixel Y position).
    //! @note Reading outside the bitmap will do nothing (a warning will be logged in debug builds)
    uint8_t GetUInt8(const uint32_t x, const uint32_t y, const bool ignoreOrigin = false) const;

    //! @brief Allows the bitmap pixel format to be modified as long as the pixel formats memory layout is compatible
    //! @note This only changes the 'PixelFormat' type it does not modify any bitmap pixels. So if you call this beware of this!
    void SetCompatiblePixelFormat(const PixelFormat compatiblePixelFormat);

    //! @brief Try to modify the pixel format flag.
    //! @return true if the format was changed or false if it failed
    //! @note This only changes to format flag not the actual data
    bool TrySetCompatiblePixelFormatFlag(const PixelFormatFlags::Enum flag);

    //! Provides direct access to the scoped bitmap during its lifetime.
    class ScopedDirectAccess
    {
      const Bitmap* m_pBitmap1;
      Bitmap* m_pBitmap2;
      RawBitmapEx* m_pRawBitmapEx;

    public:
      ScopedDirectAccess(const ScopedDirectAccess&) = delete;
      ScopedDirectAccess& operator=(const ScopedDirectAccess&) = delete;

      // Read only lock
      [[deprecated("Please utilize the ScopedDirectReadAccess instead")]] ScopedDirectAccess(const Bitmap& bitmap, ReadOnlyRawBitmap& rRawBitmap)
        : m_pBitmap1(&bitmap)
        , m_pBitmap2(nullptr)
        , m_pRawBitmapEx(nullptr)
      {
        rRawBitmap = bitmap.Lock();
      }

      [[deprecated("Please utilize the ScopedDirectReadWriteAccess instead")]] ScopedDirectAccess(Bitmap& bitmap, RawBitmapEx& rRawBitmap)
        : m_pBitmap1(nullptr)
        , m_pBitmap2(&bitmap)
        , m_pRawBitmapEx(&rRawBitmap)
      {
        rRawBitmap = bitmap.LockEx();
      }

      ~ScopedDirectAccess() noexcept
      {
        try
        {
          if (m_pBitmap1 != nullptr)
          {
            m_pBitmap1->Unlock();
          }
          else if (m_pBitmap2 != nullptr)
          {
            m_pBitmap2->UnlockEx(*m_pRawBitmapEx);
          }
        }
        catch (const std::exception&)
        {
          FSLLOG3_ERROR("ScopeDirectAccess unlock failed and destructor can not throw so aborting.");
          std::abort();
        }
      }
    };

    class ScopedDirectReadAccess final
    {
      const Bitmap& m_rBitmap;
      ReadOnlyRawBitmap m_rawBitmap;

    public:
      ScopedDirectReadAccess(const ScopedDirectReadAccess&) = delete;
      ScopedDirectReadAccess& operator=(const ScopedDirectReadAccess&) = delete;

      // Read only lock
      explicit ScopedDirectReadAccess(const Bitmap& bitmap)
        : m_rBitmap(bitmap)
        , m_rawBitmap(bitmap.Lock())
      {
      }

      ~ScopedDirectReadAccess() noexcept
      {
        try
        {
          m_rBitmap.Unlock();
        }
        catch (const std::exception&)
        {
          FSLLOG3_ERROR("ScopedDirectReadAccess unlock failed and destructor can not throw so aborting.");
          std::abort();
        }
      }

      const ReadOnlyRawBitmap& AsRawBitmap() const noexcept
      {
        return m_rawBitmap;
      }

      ReadOnlyRawBitmap& AsRawBitmap() noexcept
      {
        return m_rawBitmap;
      }
    };

    class ScopedDirectReadWriteAccess final
    {
      Bitmap& m_rBitmap;
      RawBitmapEx m_rawBitmap;

    public:
      ScopedDirectReadWriteAccess(const ScopedDirectReadAccess&) = delete;
      ScopedDirectReadWriteAccess& operator=(const ScopedDirectReadAccess&) = delete;

      // Read only lock
      explicit ScopedDirectReadWriteAccess(Bitmap& rBitmap)
        : m_rBitmap(rBitmap)
        , m_rawBitmap(rBitmap.LockEx())
      {
      }

      ~ScopedDirectReadWriteAccess() noexcept
      {
        try
        {
          m_rBitmap.UnlockEx(m_rawBitmap);
        }
        catch (const std::exception&)
        {
          FSLLOG3_ERROR("ScopedDirectReadWriteAccess unlock failed and destructor can not throw so aborting.");
          std::abort();
        }
      }

      const RawBitmapEx& AsRawBitmap() const noexcept
      {
        return m_rawBitmap;
      }

      RawBitmapEx& AsRawBitmap() noexcept
      {
        return m_rawBitmap;
      }
    };

  private:
    ReadOnlyRawBitmap Lock() const;
    RawBitmapEx LockEx();
    void UnlockEx(const RawBitmapEx& bitmap) noexcept;
    void Unlock() const noexcept;
    void ResizeToFit(const PxSize2D sizePx, const PixelFormat pixelFormat, const StrideRequirement strideRequirement, const uint32_t stride = 0);
    void Clear(const BitmapClearMethod clearMethod);
    void ResetNoThrow() noexcept;
  };
}

#endif
