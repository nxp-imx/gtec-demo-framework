#ifndef FSLGRAPHICS_BITMAP_BITMAPMEMORY_HPP
#define FSLGRAPHICS_BITMAP_BITMAPMEMORY_HPP
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
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslGraphics/Bitmap/BitmapOrigin.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <vector>

namespace Fsl
{
  //! @brief A simple safe way to transfer the 'memory/info/ between 'Bitmap like' classes.
  class BitmapMemory final
  {
    std::vector<uint8_t> m_content;
    PxSize2D m_sizePx;
    PixelFormat m_pixelFormat{PixelFormat::Undefined};
    BitmapOrigin m_origin{BitmapOrigin::UpperLeft};
    uint32_t m_stride{0};

  public:
    // move assignment operator
    BitmapMemory& operator=(BitmapMemory&& other) noexcept;
    // move constructor
    BitmapMemory(BitmapMemory&& other) noexcept;

    // Request that the compiler generates a copy constructor and assignment operator
    BitmapMemory(const BitmapMemory&) = default;
    BitmapMemory& operator=(const BitmapMemory&) = default;

    BitmapMemory() = default;


    PxSize2D GetSize() const noexcept
    {
      return m_sizePx;
    }


    PixelFormat GetPixelFormat() const noexcept
    {
      return m_pixelFormat;
    }


    BitmapOrigin GetOrigin() const noexcept
    {
      return m_origin;
    }


    //! @brief The stride of the bitmap in bytes.
    //! @warning This will be greater or equal to Width * BytesPerPixel as there might be padding bytes!
    uint32_t Stride() const noexcept
    {
      return m_stride;
    }

    uint32_t GetBytesPerPixel() const noexcept;


    //! @brief Returns true if the bitmap data is tightly packed without any padding added to the stride
    bool IsTightlyPacked() const noexcept;

    void Reset() noexcept;

    //! @brief Release the internal vector and 'reset' the class
    [[nodiscard]] std::vector<uint8_t> Release() noexcept;

    static BitmapMemory Create(const ReadOnlySpan<uint8_t> content, const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin origin);
    static BitmapMemory Create(const ReadOnlySpan<uint8_t>, const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin origin,
                               const uint32_t stride);

    static BitmapMemory Create(std::vector<uint8_t>&& content, const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin origin);
    static BitmapMemory Create(std::vector<uint8_t>&& content, const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin origin,
                               const uint32_t stride);

    static BitmapMemory UncheckedCreate(const ReadOnlySpan<uint8_t> content, const PxSize2D sizePx, const PixelFormat pixelFormat,
                                        const BitmapOrigin origin);
    static BitmapMemory UncheckedCreate(const ReadOnlySpan<uint8_t> content, const PxSize2D sizePx, const PixelFormat pixelFormat,
                                        const BitmapOrigin origin, const uint32_t stride);

    static BitmapMemory UncheckedCreate(std::vector<uint8_t>&& content, const PxSize2D sizePx, const PixelFormat pixelFormat,
                                        const BitmapOrigin origin);
    static BitmapMemory UncheckedCreate(std::vector<uint8_t>&& content, const PxSize2D sizePx, const PixelFormat pixelFormat,
                                        const BitmapOrigin origin, const uint32_t stride);

  private:
    BitmapMemory(std::vector<uint8_t> content, const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin origin,
                 const uint32_t stride);
  };
}

#endif
