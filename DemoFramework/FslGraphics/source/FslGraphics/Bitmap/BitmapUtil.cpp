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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
#include <FslGraphics/Bitmap/RawBitmapUtil.hpp>
#include <FslGraphics/Bitmap/SupportedConversionsHelper.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/PixelFormatUtil2.hpp>
#include <cassert>

namespace Fsl
{
  namespace
  {
    // Supported conversions: From -> to
    // - PixelFormatLayout::B8G8R8 -> PixelFormatLayout::B8G8R8A8
    // - PixelFormatLayout::B8G8R8 -> PixelFormatLayout::R8G8B8
    // - PixelFormatLayout::B8G8R8 -> PixelFormatLayout::R8G8B8A8
    // - PixelFormatLayout::B8G8R8 -> PixelFormat::EX_ALPHA8_UNORM
    // - PixelFormatLayout::B8G8R8 -> PixelFormat::EX_LUMINANCE8_UNORM

    // - PixelFormatLayout::B8G8R8A8 -> PixelFormatLayout::B8G8R8
    // - PixelFormatLayout::B8G8R8A8 -> PixelFormatLayout::R8G8B8
    // - PixelFormatLayout::B8G8R8A8 -> PixelFormatLayout::R8G8B8A8
    // - PixelFormatLayout::B8G8R8A8 -> PixelFormat::EX_ALPHA8_UNORM
    // - PixelFormatLayout::B8G8R8A8 -> PixelFormat::EX_LUMINANCE8_UNORM

    // - PixelFormatLayout::R8G8B8 -> PixelFormatLayout::B8G8R8
    // - PixelFormatLayout::R8G8B8 -> PixelFormatLayout::B8G8R8A8
    // - PixelFormatLayout::R8G8B8 -> PixelFormatLayout::R8G8B8A8
    // - PixelFormatLayout::R8G8B8 -> PixelFormat::EX_ALPHA8_UNORM
    // - PixelFormatLayout::R8G8B8 -> PixelFormat::EX_LUMINANCE8_UNORM

    // - PixelFormatLayout::R8G8B8A8 -> PixelFormatLayout::B8G8R8
    // - PixelFormatLayout::R8G8B8A8 -> PixelFormatLayout::B8G8R8A8
    // - PixelFormatLayout::R8G8B8A8 -> PixelFormatLayout::R8G8B8
    // - PixelFormatLayout::R8G8B8A8 -> PixelFormat::EX_ALPHA8_UNORM
    // - PixelFormatLayout::R8G8B8A8 -> PixelFormat::EX_LUMINANCE8_UNORM

    // - PixelFormat::EX_ALPHA8_UNORM -> PixelFormatLayout::B8G8R8A8
    // - PixelFormat::EX_ALPHA8_UNORM -> PixelFormatLayout::R8G8B8A8

    // - PixelFormat::EX_LUMINANCE8_UNORM -> PixelFormatLayout::B8G8R8A8
    // - PixelFormat::EX_LUMINANCE8_UNORM -> PixelFormatLayout::R8G8B8A8

    constexpr std::size_t CalcSupportedConversions() noexcept
    {
      constexpr std::size_t NumFormatsWithR8G8B8 = PixelFormatUtil::CountEntriesWithPixelLayout(PixelFormatLayout::R8G8B8);
      constexpr std::size_t NumFormatsWithB8G8R8 = PixelFormatUtil::CountEntriesWithPixelLayout(PixelFormatLayout::B8G8R8);
      constexpr std::size_t NumFormatsWithB8G8R8A8 = PixelFormatUtil::CountEntriesWithPixelLayout(PixelFormatLayout::B8G8R8A8);
      constexpr std::size_t NumFormatsWithR8G8B8A8 = PixelFormatUtil::CountEntriesWithPixelLayout(PixelFormatLayout::R8G8B8A8);
      std::size_t total = 0;

      // PixelFormatLayout::B8G8R8 -> PixelFormatLayout::B8G8R8A8
      total += NumFormatsWithB8G8R8 * NumFormatsWithB8G8R8A8;
      // PixelFormatLayout::B8G8R8 -> PixelFormatLayout::R8G8B8
      total += NumFormatsWithB8G8R8 * NumFormatsWithR8G8B8;
      // PixelFormatLayout::B8G8R8 -> PixelFormatLayout::R8G8B8A8
      total += NumFormatsWithB8G8R8 * NumFormatsWithR8G8B8A8;
      // PixelFormatLayout::B8G8R8 -> PixelFormat::EX_ALPHA8_UNORM
      total += NumFormatsWithB8G8R8;
      // PixelFormatLayout::B8G8R8 -> PixelFormat::EX_LUMINANCE8_UNORM
      total += NumFormatsWithB8G8R8;

      // PixelFormatLayout::B8G8R8A8 -> PixelFormatLayout::B8G8R8
      total += NumFormatsWithB8G8R8A8 * NumFormatsWithB8G8R8;
      // PixelFormatLayout::B8G8R8A8 -> PixelFormatLayout::R8G8B8
      total += NumFormatsWithB8G8R8A8 * NumFormatsWithR8G8B8;
      // PixelFormatLayout::B8G8R8A8 -> PixelFormatLayout::R8G8B8A8
      total += NumFormatsWithB8G8R8A8 * NumFormatsWithR8G8B8A8;
      // PixelFormatLayout::B8G8R8A8 -> PixelFormat::EX_ALPHA8_UNORM
      total += NumFormatsWithB8G8R8A8;
      // PixelFormatLayout::B8G8R8A8 -> PixelFormat::EX_LUMINANCE8_UNORM
      total += NumFormatsWithB8G8R8A8;

      // PixelFormatLayout::R8G8B8 -> PixelFormatLayout::B8G8R8
      total += NumFormatsWithR8G8B8 * NumFormatsWithB8G8R8;
      // PixelFormatLayout::R8G8B8 -> PixelFormatLayout::B8G8R8A8
      total += NumFormatsWithR8G8B8 * NumFormatsWithB8G8R8A8;
      // PixelFormatLayout::R8G8B8 -> PixelFormatLayout::R8G8B8A8
      total += NumFormatsWithR8G8B8 * NumFormatsWithR8G8B8A8;
      // PixelFormatLayout::R8G8B8 -> PixelFormat::EX_ALPHA8_UNORM
      total += NumFormatsWithR8G8B8;
      // PixelFormatLayout::R8G8B8 -> PixelFormat::EX_LUMINANCE8_UNORM
      total += NumFormatsWithR8G8B8;

      // PixelFormatLayout::R8G8B8A8 -> PixelFormatLayout::B8G8R8
      total += NumFormatsWithR8G8B8A8 * NumFormatsWithB8G8R8;
      // PixelFormatLayout::R8G8B8A8 -> PixelFormatLayout::B8G8R8A8
      total += NumFormatsWithR8G8B8A8 * NumFormatsWithB8G8R8A8;
      // PixelFormatLayout::R8G8B8A8 -> PixelFormatLayout::R8G8B8
      total += NumFormatsWithR8G8B8A8 * NumFormatsWithR8G8B8;
      // PixelFormatLayout::R8G8B8A8 -> PixelFormat::EX_ALPHA8_UNORM
      total += NumFormatsWithR8G8B8A8;
      // PixelFormatLayout::R8G8B8A8 -> PixelFormat::EX_LUMINANCE8_UNORM
      total += NumFormatsWithR8G8B8A8;


      // PixelFormat::EX_ALPHA8_UNORM -> PixelFormatLayout::B8G8R8A8
      total += NumFormatsWithB8G8R8A8;
      // PixelFormat::EX_ALPHA8_UNORM -> PixelFormatLayout::R8G8B8A8
      total += NumFormatsWithR8G8B8A8;

      // PixelFormat::EX_LUMINANCE8_UNORM -> PixelFormatLayout::B8G8R8A8
      total += NumFormatsWithB8G8R8A8;
      // PixelFormat::EX_LUMINANCE8_UNORM -> PixelFormatLayout::R8G8B8A8
      total += NumFormatsWithR8G8B8A8;
      return total;
    }

    template <std::size_t TSupportedConversions>
    class SupportedConversionsHelperEx final : public SupportedConversionsHelper<TSupportedConversions>
    {
    public:
      using SupportedConversionsHelper<TSupportedConversions>::Write;

      static constexpr std::array<SupportedConversion, TSupportedConversions> GenerateSupportedConversions()
      {
        std::array<SupportedConversion, TSupportedConversions> conversions{};
        auto dstSpan = SpanUtil::AsSpan(conversions);

        // PixelFormatLayout::B8G8R8 -> PixelFormatLayout::B8G8R8A8
        dstSpan = Write(dstSpan, PixelFormatLayout::B8G8R8, PixelFormatLayout::B8G8R8A8);
        // PixelFormatLayout::B8G8R8 -> PixelFormatLayout::R8G8B8
        dstSpan = Write(dstSpan, PixelFormatLayout::B8G8R8, PixelFormatLayout::R8G8B8);
        // PixelFormatLayout::B8G8R8 -> PixelFormatLayout::R8G8B8A8
        dstSpan = Write(dstSpan, PixelFormatLayout::B8G8R8, PixelFormatLayout::R8G8B8A8);
        // PixelFormatLayout::B8G8R8 -> PixelFormat::EX_ALPHA8_UNORM
        dstSpan = Write(dstSpan, PixelFormatLayout::B8G8R8, PixelFormat::EX_ALPHA8_UNORM);
        // PixelFormatLayout::B8G8R8 -> PixelFormat::EX_LUMINANCE8_UNORM
        dstSpan = Write(dstSpan, PixelFormatLayout::B8G8R8, PixelFormat::EX_LUMINANCE8_UNORM);

        // PixelFormatLayout::B8G8R8A8 -> PixelFormatLayout::B8G8R8
        dstSpan = Write(dstSpan, PixelFormatLayout::B8G8R8A8, PixelFormatLayout::B8G8R8);
        // PixelFormatLayout::B8G8R8A8 -> PixelFormatLayout::R8G8B8
        dstSpan = Write(dstSpan, PixelFormatLayout::B8G8R8A8, PixelFormatLayout::R8G8B8);
        // PixelFormatLayout::B8G8R8A8 -> PixelFormatLayout::R8G8B8A8
        dstSpan = Write(dstSpan, PixelFormatLayout::B8G8R8A8, PixelFormatLayout::R8G8B8A8);
        // PixelFormatLayout::B8G8R8A8 -> PixelFormat::EX_ALPHA8_UNORM
        dstSpan = Write(dstSpan, PixelFormatLayout::B8G8R8A8, PixelFormat::EX_ALPHA8_UNORM);
        // PixelFormatLayout::B8G8R8A8 -> PixelFormat::EX_LUMINANCE8_UNORM
        dstSpan = Write(dstSpan, PixelFormatLayout::B8G8R8A8, PixelFormat::EX_LUMINANCE8_UNORM);

        // PixelFormatLayout::R8G8B8 -> PixelFormatLayout::B8G8R8
        dstSpan = Write(dstSpan, PixelFormatLayout::R8G8B8, PixelFormatLayout::B8G8R8);
        // PixelFormatLayout::R8G8B8 -> PixelFormatLayout::B8G8R8A8
        dstSpan = Write(dstSpan, PixelFormatLayout::R8G8B8, PixelFormatLayout::B8G8R8A8);
        // PixelFormatLayout::R8G8B8 -> PixelFormatLayout::R8G8B8A8
        dstSpan = Write(dstSpan, PixelFormatLayout::R8G8B8, PixelFormatLayout::R8G8B8A8);
        // PixelFormatLayout::R8G8B8 -> PixelFormat::EX_ALPHA8_UNORM
        dstSpan = Write(dstSpan, PixelFormatLayout::R8G8B8, PixelFormat::EX_ALPHA8_UNORM);
        // PixelFormatLayout::R8G8B8 -> PixelFormat::EX_LUMINANCE8_UNORM
        dstSpan = Write(dstSpan, PixelFormatLayout::R8G8B8, PixelFormat::EX_LUMINANCE8_UNORM);

        // PixelFormatLayout::R8G8B8A8 -> PixelFormatLayout::B8G8R8
        dstSpan = Write(dstSpan, PixelFormatLayout::R8G8B8A8, PixelFormatLayout::B8G8R8);
        // PixelFormatLayout::R8G8B8A8 -> PixelFormatLayout::B8G8R8A8
        dstSpan = Write(dstSpan, PixelFormatLayout::R8G8B8A8, PixelFormatLayout::B8G8R8A8);
        // PixelFormatLayout::R8G8B8A8 -> PixelFormatLayout::R8G8B8
        dstSpan = Write(dstSpan, PixelFormatLayout::R8G8B8A8, PixelFormatLayout::R8G8B8);
        // PixelFormatLayout::R8G8B8A8 -> PixelFormat::EX_ALPHA8_UNORM
        dstSpan = Write(dstSpan, PixelFormatLayout::R8G8B8A8, PixelFormat::EX_ALPHA8_UNORM);
        // PixelFormatLayout::R8G8B8A8 -> PixelFormat::EX_LUMINANCE8_UNORM
        dstSpan = Write(dstSpan, PixelFormatLayout::R8G8B8A8, PixelFormat::EX_LUMINANCE8_UNORM);

        // PixelFormat::EX_ALPHA8_UNORM -> PixelFormatLayout::B8G8R8A8
        dstSpan = Write(dstSpan, PixelFormat::EX_ALPHA8_UNORM, PixelFormatLayout::B8G8R8A8);
        // PixelFormat::EX_ALPHA8_UNORM -> PixelFormatLayout::R8G8B8A8
        dstSpan = Write(dstSpan, PixelFormat::EX_ALPHA8_UNORM, PixelFormatLayout::R8G8B8A8);

        // PixelFormat::EX_LUMINANCE8_UNORM -> PixelFormatLayout::B8G8R8A8
        dstSpan = Write(dstSpan, PixelFormat::EX_LUMINANCE8_UNORM, PixelFormatLayout::B8G8R8A8);
        // PixelFormat::EX_LUMINANCE8_UNORM -> PixelFormatLayout::R8G8B8A8
        dstSpan = Write(dstSpan, PixelFormat::EX_LUMINANCE8_UNORM, PixelFormatLayout::R8G8B8A8);

        if (!dstSpan.empty())
        {
          throw InternalErrorException("the span was not filled");
        }
        return conversions;
      }
    };

    // Compile time generate the list of actually supported conversions
    constexpr auto SupportedConversions = SupportedConversionsHelperEx<CalcSupportedConversions()>::GenerateSupportedConversions();


    void Swizzle24To32Shrinking(RawBitmapEx& rBitmap, const std::size_t dstStride, const int srcIdx0, const int srcIdx1, const int srcIdx2,
                                const uint8_t alpha)
    {
      assert(rBitmap.IsValid());
      assert(rBitmap.GetPixelFormatLayout() == PixelFormatLayout::R8G8B8 || rBitmap.GetPixelFormatLayout() == PixelFormatLayout::B8G8R8);

      assert(srcIdx0 >= 0 && srcIdx0 <= 2);
      assert(srcIdx1 >= 0 && srcIdx1 <= 2);
      assert(srcIdx2 >= 0 && srcIdx2 <= 2);
      assert(PixelFormatUtil::GetBytesPerPixel(rBitmap.GetPixelFormat()) == 3);
      // The Swizzle24To32Shrinking method relies on the dstStride being <= the existing stride to support inplace swizzle
      assert(dstStride > 0 && dstStride <= rBitmap.Stride());

      // Generic slow swizzle for 24bpp to 32bpp formats
      const std::size_t srcStride = rBitmap.Stride();
      auto* pDstBitmap = static_cast<uint8_t*>(rBitmap.Content());
      const uint8_t* const pDstBitmapEnd = pDstBitmap + (rBitmap.RawUnsignedHeight() * dstStride);
      const uint8_t* pSrcBitmap = pDstBitmap;

      const int32_t width = rBitmap.RawWidth() - 1;
      while (pDstBitmap < pDstBitmapEnd)
      {
        for (int32_t x = width; x >= 0; --x)
        {
          const uint8_t b0 = pSrcBitmap[(x * 3) + srcIdx0];
          const uint8_t b1 = pSrcBitmap[(x * 3) + srcIdx1];
          const uint8_t b2 = pSrcBitmap[(x * 3) + srcIdx2];
          pDstBitmap[(x * 4) + 0] = b0;
          pDstBitmap[(x * 4) + 1] = b1;
          pDstBitmap[(x * 4) + 2] = b2;
          pDstBitmap[(x * 4) + 3] = alpha;
        }
        pSrcBitmap += srcStride;
        pDstBitmap += dstStride;
      }
    }

    void Swizzle24To32Growing(RawBitmapEx& rBitmap, const std::size_t srcStride, const int srcIdx0, const int srcIdx1, const int srcIdx2,
                              const uint8_t alpha)
    {
      assert(rBitmap.IsValid());
      // the bitmap is pointing to the dst format information since its growing
      assert(rBitmap.GetPixelFormatLayout() == PixelFormatLayout::R8G8B8A8 || rBitmap.GetPixelFormatLayout() == PixelFormatLayout::B8G8R8A8);

      assert(srcIdx0 >= 0 && srcIdx0 <= 2);
      assert(srcIdx1 >= 0 && srcIdx1 <= 2);
      assert(srcIdx2 >= 0 && srcIdx2 <= 2);
      // the bitmap is pointing to the dst format information since its growing
      assert(PixelFormatUtil::GetBytesPerPixel(rBitmap.GetPixelFormat()) == 4);
      // The Swizzle24To32Shrinking method relies on the dstStride being <= the existing stride to support inplace swizzle
      assert(srcStride > 0 && rBitmap.Stride() > srcStride);

      // Generic slow swizzle for 32bpp to 24bpp formats
      if (rBitmap.RawUnsignedHeight() > 0u)
      {
        const std::size_t dstStride = rBitmap.Stride();
        auto* const pDstBitmapStart = static_cast<uint8_t*>(rBitmap.Content());
        uint8_t* pDstBitmap = pDstBitmapStart + ((rBitmap.RawUnsignedHeight() - 1u) * dstStride);
        const uint8_t* pSrcBitmap = pDstBitmapStart + ((rBitmap.RawUnsignedHeight() - 1u) * srcStride);
        const int32_t width = rBitmap.RawWidth() - 1;

        while (pDstBitmap >= pDstBitmapStart)
        {
          for (int32_t x = width; x >= 0; --x)
          {
            const uint8_t b0 = pSrcBitmap[(x * 3) + srcIdx0];
            const uint8_t b1 = pSrcBitmap[(x * 3) + srcIdx1];
            const uint8_t b2 = pSrcBitmap[(x * 3) + srcIdx2];
            pDstBitmap[(x * 4) + 0] = b0;
            pDstBitmap[(x * 4) + 1] = b1;
            pDstBitmap[(x * 4) + 2] = b2;
            pDstBitmap[(x * 4) + 3] = alpha;
          }
          pSrcBitmap -= srcStride;
          pDstBitmap -= dstStride;
        }
      }
    }


    void ExpandAlpha8To32Growing(RawBitmapEx& rBitmap, const std::size_t srcStride)
    {
      assert(rBitmap.IsValid());
      // the bitmap is pointing to the dst format information since its growing
      assert(rBitmap.GetPixelFormatLayout() == PixelFormatLayout::R8G8B8A8 || rBitmap.GetPixelFormatLayout() == PixelFormatLayout::B8G8R8A8);

      // the bitmap is pointing to the dst format information since its growing
      assert(PixelFormatUtil::GetBytesPerPixel(rBitmap.GetPixelFormat()) == 4);
      // The ExpandAlpha8To32Growing method relies on the dstStride being <= the existing stride to support inplace 'expand'
      assert(srcStride > 0 && rBitmap.Stride() > srcStride);

      // Generic slow swizzle for 32bpp to 24bpp formats
      if (rBitmap.RawUnsignedHeight() > 0u)
      {
        const std::size_t dstStride = rBitmap.Stride();
        auto* const pDstBitmapStart = static_cast<uint8_t*>(rBitmap.Content());
        uint8_t* pDstBitmap = pDstBitmapStart + ((rBitmap.RawUnsignedHeight() - 1u) * dstStride);
        const uint8_t* pSrcBitmap = pDstBitmapStart + ((rBitmap.RawUnsignedHeight() - 1u) * srcStride);
        const int32_t width = rBitmap.RawWidth() - 1;

        while (pDstBitmap >= pDstBitmapStart)
        {
          for (int32_t x = width; x >= 0; --x)
          {
            const uint8_t col = pSrcBitmap[x];
            pDstBitmap[(x * 4) + 0] = col;
            pDstBitmap[(x * 4) + 1] = col;
            pDstBitmap[(x * 4) + 2] = col;
            pDstBitmap[(x * 4) + 3] = col;
          }
          pSrcBitmap -= srcStride;
          pDstBitmap -= dstStride;
        }
      }
    }
  }

  ReadOnlySpan<SupportedConversion> BitmapUtil::GetSupportedConversions() noexcept
  {
    return SpanUtil::AsReadOnlySpan(SupportedConversions);
  }


  bool BitmapUtil::TryConvert(Bitmap& rBitmap, const PixelFormat desiredPixelFormat)
  {
    const PixelFormat currentPixelFormat = rBitmap.GetPixelFormat();
    // Nothing to do
    if (currentPixelFormat == desiredPixelFormat)
    {
      return true;
    }

    if (rBitmap.RawWidth() == 0 || rBitmap.RawHeight() == 0)
    {
      rBitmap.Reset(rBitmap.GetSize(), desiredPixelFormat, rBitmap.GetPreferredStride(desiredPixelFormat), rBitmap.GetOrigin(),
                    BitmapClearMethod::DontModify);
      return true;
    }


    const PixelFormatLayout srcPixelFormatLayout = rBitmap.GetPixelFormatLayout();
    const PixelFormatLayout desiredPixelFormatLayout = PixelFormatUtil::GetPixelFormatLayout(desiredPixelFormat);

    if (srcPixelFormatLayout == desiredPixelFormatLayout)
    {
      // The pixel format layout does not need to be changed, so just update the pixel format
      rBitmap.SetCompatiblePixelFormat(desiredPixelFormat);
      return true;
    }

    // We only support a few hard-coded conversions
    if ((srcPixelFormatLayout == PixelFormatLayout::R8G8B8 && desiredPixelFormatLayout == PixelFormatLayout::B8G8R8) ||
        (srcPixelFormatLayout == PixelFormatLayout::B8G8R8 && desiredPixelFormatLayout == PixelFormatLayout::R8G8B8))
    {
      // 012    012
      // RGB -> BGR
      // BGR -> RGB
      {
        Bitmap::ScopedDirectReadWriteAccess directAccess(rBitmap);
        RawBitmapUtil::Swizzle24(directAccess.AsRawBitmap(), 2, 1, 0);
      }
      // Update the bitmap with the new pixel format
      // We use the unsafe variant here since that doesn't modify any padding (we are using the original content)
      rBitmap.Reset(rBitmap.GetSize(), desiredPixelFormat, rBitmap.Stride(), rBitmap.GetOrigin(), BitmapClearMethod::DontModify);
      return true;
    }

    if ((srcPixelFormatLayout == PixelFormatLayout::R8G8B8A8 && desiredPixelFormatLayout == PixelFormatLayout::B8G8R8A8) ||
        (srcPixelFormatLayout == PixelFormatLayout::B8G8R8A8 && desiredPixelFormatLayout == PixelFormatLayout::R8G8B8A8))
    {
      // 0123    0123
      // RGBA -> BGRA
      // BGRA -> RGBA
      {
        Bitmap::ScopedDirectReadWriteAccess directAccess(rBitmap);
        RawBitmapUtil::Swizzle32(directAccess.AsRawBitmap(), 2, 1, 0, 3);
      }
      // Update the bitmap with the new pixel format
      // We use the unsafe variant here since that doesn't modify any padding (we are using the original content)
      rBitmap.Reset(rBitmap.GetSize(), desiredPixelFormat, rBitmap.Stride(), rBitmap.GetOrigin(), BitmapClearMethod::DontModify);
      return true;
    }

    if ((srcPixelFormatLayout == PixelFormatLayout::R8G8B8A8 && desiredPixelFormatLayout == PixelFormatLayout::B8G8R8) ||
        (srcPixelFormatLayout == PixelFormatLayout::B8G8R8A8 && desiredPixelFormatLayout == PixelFormatLayout::R8G8B8))
    {
      // 0123    012
      // RGBA -> BGR
      // BGRA -> RGB
      const auto desiredStride = rBitmap.GetPreferredStride(desiredPixelFormat);
      if (desiredStride <= rBitmap.Stride())
      {
        // The swizzle32to24 method relies on the dstStride being <= the existing stride to support inplace swizzle
        {
          Bitmap::ScopedDirectReadWriteAccess directAccess(rBitmap);
          RawBitmapUtil::Swizzle32To24(directAccess.AsRawBitmap(), desiredPixelFormat, desiredStride, 2, 1, 0);
        }
        // Update the bitmap with the new pixel format
        // We use the unsafe variant here since that doesn't modify any padding (we are using the original content)
        rBitmap.Reset(rBitmap.GetSize(), desiredPixelFormat, desiredStride, rBitmap.GetOrigin(), BitmapClearMethod::DontModify);
        return true;
      }
    }
    else if ((srcPixelFormatLayout == PixelFormatLayout::R8G8B8A8 && desiredPixelFormatLayout == PixelFormatLayout::R8G8B8) ||
             (srcPixelFormatLayout == PixelFormatLayout::B8G8R8A8 && desiredPixelFormatLayout == PixelFormatLayout::B8G8R8))
    {
      // 0123    012
      // RGBA -> RGB
      // BGRA -> BG
      const auto desiredStride = rBitmap.GetPreferredStride(desiredPixelFormat);
      if (desiredStride <= rBitmap.Stride())
      {
        // The swizzle32to24 method relies on the dstStride being <= the existing stride to support inplace swizzle
        {
          Bitmap::ScopedDirectReadWriteAccess directAccess(rBitmap);
          RawBitmapUtil::Swizzle32To24(directAccess.AsRawBitmap(), desiredPixelFormat, desiredStride, 0, 1, 2);
        }
        // Update the bitmap with the new pixel format
        // We use the unsafe variant here since that doesn't modify any padding (we are using the original content)
        rBitmap.Reset(rBitmap.GetSize(), desiredPixelFormat, desiredStride, rBitmap.GetOrigin(), BitmapClearMethod::DontModify);
        return true;
      }
    }
    else if ((srcPixelFormatLayout == PixelFormatLayout::R8G8B8 && desiredPixelFormatLayout == PixelFormatLayout::B8G8R8A8) ||
             (srcPixelFormatLayout == PixelFormatLayout::B8G8R8 && desiredPixelFormatLayout == PixelFormatLayout::R8G8B8A8))
    {
      // 012    0123
      // RGB -> BGRA
      // BGR -> RGBA
      const auto desiredStride = rBitmap.GetPreferredStride(desiredPixelFormat);
      if (desiredStride <= rBitmap.Stride())
      {
        {
          Bitmap::ScopedDirectReadWriteAccess directAccess(rBitmap);
          Swizzle24To32Shrinking(directAccess.AsRawBitmap(), desiredStride, 2, 1, 0, 0xFF);
        }
        // This shrinks the bitmap size after the conversion since the desired stride is smaller than the existing one
        // Update the bitmap with the new pixel format
        // We use the unsafe variant here since that doesn't modify any padding (we are using the original content)
        rBitmap.Reset(rBitmap.GetSize(), desiredPixelFormat, desiredStride, rBitmap.GetOrigin(), BitmapClearMethod::DontModify);
        return true;
      }


      // This grows the bitmap size before the conversion since the desired stride is larger than the existing one
      // Update the bitmap with the new pixel format
      // We use the unsafe variant here since that doesn't modify any padding (we are using the original content)
      const auto srcStride = rBitmap.Stride();
      rBitmap.Reset(rBitmap.GetSize(), desiredPixelFormat, desiredStride, rBitmap.GetOrigin(), BitmapClearMethod::DontModify);
      {
        Bitmap::ScopedDirectReadWriteAccess directAccess(rBitmap);
        Swizzle24To32Growing(directAccess.AsRawBitmap(), srcStride, 2, 1, 0, 0xFF);
      }
      return true;
    }
    else if ((srcPixelFormatLayout == PixelFormatLayout::R8G8B8 && desiredPixelFormatLayout == PixelFormatLayout::R8G8B8A8) ||
             (srcPixelFormatLayout == PixelFormatLayout::B8G8R8 && desiredPixelFormatLayout == PixelFormatLayout::B8G8R8A8))
    {
      // 012    0123
      // RGB -> BGRA
      // BGR -> RGBA
      const auto desiredStride = rBitmap.GetPreferredStride(desiredPixelFormat);
      if (desiredStride <= rBitmap.Stride())
      {
        {
          Bitmap::ScopedDirectReadWriteAccess directAccess(rBitmap);
          Swizzle24To32Shrinking(directAccess.AsRawBitmap(), desiredStride, 0, 1, 2, 0xFF);
        }
        // This shrinks the bitmap size after the conversion since the desired stride is smaller than the existing one
        // Update the bitmap with the new pixel format
        // We use the unsafe variant here since that doesn't modify any padding (we are using the original content)
        rBitmap.Reset(rBitmap.GetSize(), desiredPixelFormat, desiredStride, rBitmap.GetOrigin(), BitmapClearMethod::DontModify);
        return true;
      }


      // This grows the bitmap size before the conversion since the desired stride is larger than the existing one
      // Update the bitmap with the new pixel format
      // We use the unsafe variant here since that doesn't modify any padding (we are using the original content)
      const auto srcStride = rBitmap.Stride();
      rBitmap.Reset(rBitmap.GetSize(), desiredPixelFormat, desiredStride, rBitmap.GetOrigin(), BitmapClearMethod::DontModify);
      {
        Bitmap::ScopedDirectReadWriteAccess directAccess(rBitmap);
        Swizzle24To32Growing(directAccess.AsRawBitmap(), srcStride, 0, 1, 2, 0xFF);
      }
      return true;
    }
    else if (desiredPixelFormat == PixelFormat::EX_ALPHA8_UNORM || desiredPixelFormat == PixelFormat::EX_LUMINANCE8_UNORM)
    {
      const auto desiredStride = rBitmap.GetPreferredStride(desiredPixelFormat);
      if (srcPixelFormatLayout == PixelFormatLayout::R8G8B8 || srcPixelFormatLayout == PixelFormatLayout::B8G8R8)
      {
        {
          Bitmap::ScopedDirectReadWriteAccess directAccess(rBitmap);
          RawBitmapUtil::Average24To8(directAccess.AsRawBitmap(), desiredPixelFormat, desiredStride, 0, 1, 2);
        }
        // This shrinks the bitmap size after the conversion since the desired stride is smaller than the existing one
        // Update the bitmap with the new pixel format
        // We use the unsafe variant here since that doesn't modify any padding (we are using the original content)
        rBitmap.Reset(rBitmap.GetSize(), desiredPixelFormat, desiredStride, rBitmap.GetOrigin(), BitmapClearMethod::DontModify);
        return true;
      }
      if (srcPixelFormatLayout == PixelFormatLayout::R8G8B8A8 || srcPixelFormatLayout == PixelFormatLayout::B8G8R8A8)
      {
        {
          Bitmap::ScopedDirectReadWriteAccess directAccess(rBitmap);
          RawBitmapUtil::Average32To8(directAccess.AsRawBitmap(), desiredPixelFormat, desiredStride, 0, 1, 2);
        }
        // This shrinks the bitmap size after the conversion since the desired stride is smaller than the existing one
        // Update the bitmap with the new pixel format
        // We use the unsafe variant here since that doesn't modify any padding (we are using the original content)
        rBitmap.Reset(rBitmap.GetSize(), desiredPixelFormat, desiredStride, rBitmap.GetOrigin(), BitmapClearMethod::DontModify);
        return true;
      }
    }
    else if ((currentPixelFormat == PixelFormat::EX_ALPHA8_UNORM || currentPixelFormat == PixelFormat::EX_LUMINANCE8_UNORM) &&
             (desiredPixelFormatLayout == PixelFormatLayout::B8G8R8A8 || desiredPixelFormatLayout == PixelFormatLayout::R8G8B8A8))
    {
      const auto desiredStride = rBitmap.GetPreferredStride(desiredPixelFormat);

      // This grows the bitmap size before the conversion since the desired stride is larger than the existing one
      // Update the bitmap with the new pixel format
      // We use the unsafe variant here since that doesn't modify any padding (we are using the original content)
      const auto srcStride = rBitmap.Stride();
      rBitmap.Reset(rBitmap.GetSize(), desiredPixelFormat, desiredStride, rBitmap.GetOrigin(), BitmapClearMethod::DontModify);
      {
        Bitmap::ScopedDirectReadWriteAccess directAccess(rBitmap);
        ExpandAlpha8To32Growing(directAccess.AsRawBitmap(), srcStride);
      }
      return true;
    }
    return false;
  }

  void BitmapUtil::Convert(Bitmap& rBitmap, const PixelFormat desiredPixelFormat)
  {
    if (!TryConvert(rBitmap, desiredPixelFormat))
    {
      throw UnsupportedPixelFormatConversionException("Conversion not supported", rBitmap.GetPixelFormat(), desiredPixelFormat);
    }
  }


  void BitmapUtil::FlipHorizontal(Bitmap& rBitmap)
  {
    Bitmap::ScopedDirectReadWriteAccess directAccess(rBitmap);
    RawBitmapUtil::FlipHorizontal(directAccess.AsRawBitmap());
  }
}
