#ifndef FSLGRAPHICS_BITMAP_UNCHECKEDRAWBITMAPTRANSFORMER_HPP
#define FSLGRAPHICS_BITMAP_UNCHECKEDRAWBITMAPTRANSFORMER_HPP
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

#include <FslGraphics/Bitmap/RawBitmapEx.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <cassert>

namespace Fsl::UncheckedRawBitmapTransformer
{
  //! Check if the two memory blocks are overlapping pSrc <= pSrcEnd && pDst <= pDstEnd
  constexpr bool DoesMemoryRegionOverlap(const void* const pSrc, const void* const pSrcEnd, const void* const pDst,
                                         const void* const pDstEnd) noexcept
  {
    assert(pSrc <= pSrcEnd);
    assert(pDst <= pDstEnd);
    return (pSrc <= pDst && pDst < pSrcEnd) || (pDst <= pSrc && pSrc < pDstEnd);
  }


  //! Check if the two bitmaps are considered overlapping
  constexpr bool DoesMemoryRegionOverlap(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept
  {
    const auto* const pSrc = static_cast<const uint8_t*>(srcBitmap.Content());
    const auto* const pDst = static_cast<const uint8_t*>(dstBitmap.Content());
    const auto* const pSrcEnd = pSrc + srcBitmap.GetByteSize();
    const auto* const pDstEnd = pDst + dstBitmap.GetByteSize();

    assert(pSrc <= pSrcEnd);
    assert(pDst <= pDstEnd);
    return (pSrc <= pDst && pDst < pSrcEnd) || (pDst <= pSrc && pSrc < pDstEnd);
  }


  //! Check if the two bitmaps memory are not overlapping or is considered safe for in-place modification
  //! Inplace modification requires
  //! - that the src and dst start at the same memory address
  //! - that a src stride is >= the dst stride
  //! - that a src pixel byte size is >= the dst pixel byte size
  //! This allow inplace modification to occur by processing from the first pixel in the bitmap moving towards the last.
  //! @warning this assumes that src and dst size is the same, but does not check for it! (this should be done before calling this)
  constexpr bool IsSafeInplaceModificationOrNoMemoryOverlap(const RawBitmapEx& dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept
  {
    assert(dstBitmap.GetSize() == srcBitmap.GetSize());

    const auto* const pSrc = static_cast<const uint8_t*>(srcBitmap.Content());
    const auto* const pDst = static_cast<const uint8_t*>(dstBitmap.Content());
    const auto* const pSrcEnd = pSrc + srcBitmap.GetByteSize();
    const auto* const pDstEnd = pDst + dstBitmap.GetByteSize();

    assert(pSrc <= pSrcEnd);
    assert(pDst <= pDstEnd);

    // It must either be a safe inplace modification or no-overlap

    return (pSrc == pDst && srcBitmap.Stride() >= dstBitmap.Stride() &&
            PixelFormatUtil::GetBytesPerPixel(srcBitmap.GetPixelFormat()) >= PixelFormatUtil::GetBytesPerPixel(dstBitmap.GetPixelFormat())) ||
           ((pSrc > pDst || pDst >= pSrcEnd) && (pDst > pSrc || pSrc >= pDstEnd));
  }

  //! Must be true
  //! - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  template <typename TDstChannel, typename TSrcChannel, uint8_t TChannelCount, typename TUnaryOperation>
  constexpr void TransformChannelsRAWNoMemoryOverlap(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap,
                                                     TUnaryOperation fnUnaryOperation) noexcept
  {
    assert(dstBitmap.GetOrigin() == srcBitmap.GetOrigin());
    assert(dstBitmap.GetSize() == srcBitmap.GetSize());
    // The src and dst memory can only over in a 'safe' way
    assert(IsSafeInplaceModificationOrNoMemoryOverlap(dstBitmap, srcBitmap));

    // Expect the stride to be aligned to channel type size.
    assert((srcBitmap.Stride() % sizeof(TSrcChannel)) == 0);
    assert((dstBitmap.Stride() % sizeof(TDstChannel)) == 0);

    // Convert the stride to be of the channel types
    const uint32_t srcStride = srcBitmap.Stride() / sizeof(TSrcChannel);
    const uint32_t dstStride = dstBitmap.Stride() / sizeof(TDstChannel);

    const auto* pSrc = static_cast<const TSrcChannel*>(srcBitmap.Content());
    auto* pDst = static_cast<TDstChannel*>(dstBitmap.Content());
    // const TSrcChannel* const pSrcEnd = pSrc + (srcBitmap.RawUnsignedHeight() * srcStride);
    const TDstChannel* const pDstEnd = pDst + (dstBitmap.RawUnsignedHeight() * dstStride);

    // Generic slow conversion of unpacked channels
    assert(srcBitmap.RawUnsignedWidth() <= (std::numeric_limits<uint32_t>::max() / TChannelCount));
    const uint32_t channelsWidth = srcBitmap.RawUnsignedWidth() * TChannelCount;
    while (pDst < pDstEnd)
    {
      for (uint32_t x = 0; x < channelsWidth; ++x)
      {
        pDst[x] = fnUnaryOperation(pSrc[x]);
      }
      pSrc += srcStride;
      pDst += dstStride;
    }
  }


  //! Must be true
  //! - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  template <typename TDstChannel, typename TSrcChannel, typename TUnaryOperation>
  constexpr void TransformThreeChannelsRAW(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap, TUnaryOperation fnUnaryOperation) noexcept
  {
    assert(dstBitmap.GetOrigin() == srcBitmap.GetOrigin());
    assert(dstBitmap.GetSize() == srcBitmap.GetSize());
    // The src and dst memory can only over in a 'safe' way
    assert(IsSafeInplaceModificationOrNoMemoryOverlap(dstBitmap, srcBitmap));

    // Expect the stride to be aligned to channel type size.
    assert((srcBitmap.Stride() % sizeof(TSrcChannel)) == 0);
    assert((dstBitmap.Stride() % sizeof(TDstChannel)) == 0);
    // Convert the stride to be of the channel types
    const uint32_t srcStride = srcBitmap.Stride() / sizeof(TSrcChannel);
    const uint32_t dstStride = dstBitmap.Stride() / sizeof(TDstChannel);

    const auto* pSrc = static_cast<const TSrcChannel*>(srcBitmap.Content());
    auto* pDst = static_cast<TDstChannel*>(dstBitmap.Content());
    // const TSrcChannel* const pSrcEnd = pSrc + (srcBitmap.RawUnsignedHeight() * srcStride);
    const TDstChannel* const pDstEnd = pDst + (dstBitmap.RawUnsignedHeight() * dstStride);

    // Generic slow conversion of three channels
    constexpr uint32_t NumChannels = 3u;
    assert(srcBitmap.RawUnsignedWidth() <= (std::numeric_limits<uint32_t>::max() / NumChannels));
    const uint32_t width3 = srcBitmap.RawUnsignedWidth() * NumChannels;
    while (pDst < pDstEnd)
    {
      for (uint32_t x = 0; x < width3; x += NumChannels)
      {
        pDst[x] = fnUnaryOperation(pSrc[x]);
        pDst[x + 1] = fnUnaryOperation(pSrc[x + 1]);
        pDst[x + 2] = fnUnaryOperation(pSrc[x + 2]);
      }
      pSrc += srcStride;
      pDst += dstStride;
    }
  }


  //! Must be true
  //! - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  template <typename TDstChannel, typename TSrcChannel, typename TUnaryOperation>
  constexpr void TransformThreeChannelsAndAddChannelRAW(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap, TUnaryOperation fnUnaryOperation,
                                                        const TDstChannel newChannelValue) noexcept
  {
    assert(dstBitmap.GetOrigin() == srcBitmap.GetOrigin());
    assert(dstBitmap.GetSize() == srcBitmap.GetSize());
    // The src and dst memory can only over in a 'safe' way
    assert(IsSafeInplaceModificationOrNoMemoryOverlap(dstBitmap, srcBitmap));

    // Expect the stride to be aligned to channel type size.
    assert((srcBitmap.Stride() % sizeof(TSrcChannel)) == 0);
    assert((dstBitmap.Stride() % sizeof(TDstChannel)) == 0);

    // Convert the stride to be of the channel types
    const uint32_t srcStride = srcBitmap.Stride() / sizeof(TSrcChannel);
    const uint32_t dstStride = dstBitmap.Stride() / sizeof(TDstChannel);

    const auto* pSrc = static_cast<const TSrcChannel*>(srcBitmap.Content());
    auto* pDst = static_cast<TDstChannel*>(dstBitmap.Content());
    // const TSrcChannel* const pSrcEnd = pSrc + (srcBitmap.RawUnsignedHeight() * srcStride);
    const TDstChannel* const pDstEnd = pDst + (dstBitmap.RawUnsignedHeight() * dstStride);

    // Generic slow conversion of three channels
    constexpr uint32_t NumChannels = 3u;
    assert(srcBitmap.RawUnsignedWidth() <= (std::numeric_limits<uint32_t>::max() / NumChannels));

    const auto minDstStride = PixelFormatUtil::CalcMinimumStride(dstBitmap.Width(), dstBitmap.GetPixelFormat());
    assert((minDstStride % sizeof(TDstChannel)) == 0);

    const uint32_t width3 = srcBitmap.RawUnsignedWidth() * NumChannels;

    assert(dstStride >= (minDstStride / sizeof(TDstChannel)));
    const uint32_t dstStridePadding = dstStride - (minDstStride / sizeof(TDstChannel));
    while (pDst < pDstEnd)
    {
      for (uint32_t x = 0; x < width3; x += NumChannels)
      {
        pDst[0] = fnUnaryOperation(pSrc[x]);
        pDst[1] = fnUnaryOperation(pSrc[x + 1]);
        pDst[2] = fnUnaryOperation(pSrc[x + 2]);
        pDst[3] = newChannelValue;
        pDst += 4;
      }
      pSrc += srcStride;
      pDst += dstStridePadding;
    }
  }

  //! Applies fnUnaryOperation0 to channel 0-2 and fnUnaryOperation1 to channel 3.
  //! Must be true
  //! - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  template <typename TDstChannel, typename TSrcChannel, typename TUnaryOperation0, typename TUnaryOperation1>
  constexpr void TransformThreeChannelsTransformFourthRAW(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap,
                                                          TUnaryOperation0 fnUnaryOperation0, TUnaryOperation1 fnUnaryOperation1) noexcept
  {
    assert(dstBitmap.GetOrigin() == srcBitmap.GetOrigin());
    assert(dstBitmap.GetSize() == srcBitmap.GetSize());
    // The src and dst memory can only over in a 'safe' way
    assert(IsSafeInplaceModificationOrNoMemoryOverlap(dstBitmap, srcBitmap));

    // Expect the stride to be aligned to channel type size.
    assert((srcBitmap.Stride() % sizeof(TSrcChannel)) == 0);
    assert((dstBitmap.Stride() % sizeof(TDstChannel)) == 0);
    // Convert the stride to be of the channel types
    const uint32_t srcStride = srcBitmap.Stride() / sizeof(TSrcChannel);
    const uint32_t dstStride = dstBitmap.Stride() / sizeof(TDstChannel);

    const auto* pSrc = static_cast<const TSrcChannel*>(srcBitmap.Content());
    auto* pDst = static_cast<TDstChannel*>(dstBitmap.Content());
    // const TSrcChannel* const pSrcEnd = pSrc + (srcBitmap.RawUnsignedHeight() * srcStride);
    const TDstChannel* const pDstEnd = pDst + (dstBitmap.RawUnsignedHeight() * dstStride);

    // Generic slow conversion of four channels
    constexpr uint32_t NumChannels = 4u;
    assert(srcBitmap.RawUnsignedWidth() <= (std::numeric_limits<uint32_t>::max() / NumChannels));
    const uint32_t width4 = srcBitmap.RawUnsignedWidth() * NumChannels;
    while (pDst < pDstEnd)
    {
      for (uint32_t x = 0; x < width4; x += NumChannels)
      {
        pDst[x] = fnUnaryOperation0(pSrc[x]);
        pDst[x + 1] = fnUnaryOperation0(pSrc[x + 1]);
        pDst[x + 2] = fnUnaryOperation0(pSrc[x + 2]);
        pDst[x + 3] = fnUnaryOperation1(pSrc[x + 3]);
      }
      pSrc += srcStride;
      pDst += dstStride;
    }
  }

  //! Must be true
  //! - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  template <typename TDstChannel, typename TSrcChannel, typename TUnaryOperation>
  constexpr void TransformFourChannelsRAW(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap, TUnaryOperation fnUnaryOperation) noexcept
  {
    assert(dstBitmap.GetOrigin() == srcBitmap.GetOrigin());
    assert(dstBitmap.GetSize() == srcBitmap.GetSize());
    // The src and dst memory can only over in a 'safe' way
    assert(IsSafeInplaceModificationOrNoMemoryOverlap(dstBitmap, srcBitmap));

    // Expect the stride to be aligned to channel type size.
    assert((srcBitmap.Stride() % sizeof(TSrcChannel)) == 0);
    assert((dstBitmap.Stride() % sizeof(TDstChannel)) == 0);
    // Convert the stride to be of the channel types
    const uint32_t srcStride = srcBitmap.Stride() / sizeof(TSrcChannel);
    const uint32_t dstStride = dstBitmap.Stride() / sizeof(TDstChannel);

    const auto* pSrc = static_cast<const TSrcChannel*>(srcBitmap.Content());
    auto* pDst = static_cast<TDstChannel*>(dstBitmap.Content());
    // const TSrcChannel* const pSrcEnd = pSrc + (srcBitmap.RawUnsignedHeight() * srcStride);
    const TDstChannel* const pDstEnd = pDst + (dstBitmap.RawUnsignedHeight() * dstStride);

    // Generic slow conversion of four channels
    constexpr uint32_t NumChannels = 4u;
    assert(srcBitmap.RawUnsignedWidth() <= (std::numeric_limits<uint32_t>::max() / NumChannels));
    const uint32_t width4 = srcBitmap.RawUnsignedWidth() * NumChannels;
    while (pDst < pDstEnd)
    {
      for (uint32_t x = 0; x < width4; x += NumChannels)
      {
        pDst[x] = fnUnaryOperation(pSrc[x]);
        pDst[x + 1] = fnUnaryOperation(pSrc[x + 1]);
        pDst[x + 2] = fnUnaryOperation(pSrc[x + 2]);
        pDst[x + 3] = fnUnaryOperation(pSrc[x + 3]);
      }
      pSrc += srcStride;
      pDst += dstStride;
    }
  }

  //! Must be true
  //! - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  template <typename TDstChannel, PixelFormat TDstPixelFormat, typename TSrcChannel, PixelFormat TSrcPixelFormat, typename TUnaryOperation>
  void TransformThreeChannels(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap, TUnaryOperation fnUnaryOperation) noexcept
  {
    static_assert(!PixelFormatUtil::IsCompressed(TDstPixelFormat), "TDstPixelFormat PixelFormat can not be compressed");
    static_assert(!PixelFormatUtil::IsCompressed(TSrcPixelFormat), "TSrcPixelFormat PixelFormat can not be compressed");
    static_assert(!PixelFormatUtil::IsPacked(TDstPixelFormat), "TDstPixelFormat PixelFormat can not be packed");
    static_assert(!PixelFormatUtil::IsPacked(TSrcPixelFormat), "TSrcPixelFormat PixelFormat can not be packed");

    // Generic slow conversion of four channels
    constexpr uint32_t NumChannels = 3u;
    static_assert(PixelFormatUtil::GetChannelCount(TDstPixelFormat) == NumChannels);
    static_assert(PixelFormatUtil::GetChannelCount(TSrcPixelFormat) == NumChannels);

    assert(srcBitmap.GetPixelFormat() == TSrcPixelFormat);
    assert(dstBitmap.GetPixelFormat() == TDstPixelFormat);
    TransformThreeChannelsRAW<TDstChannel, TSrcChannel>(dstBitmap, srcBitmap, fnUnaryOperation);
  }

  //! Must be true
  //! - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  template <typename TDstChannel, PixelFormat TDstPixelFormat, typename TSrcChannel, PixelFormat TSrcPixelFormat, typename TUnaryOperation>
  void TransformThreeChannelsAndAddChannel(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap, TUnaryOperation fnUnaryOperation,
                                           const TDstChannel newChannelValue) noexcept
  {
    static_assert(!PixelFormatUtil::IsCompressed(TDstPixelFormat), "TDstPixelFormat PixelFormat can not be compressed");
    static_assert(!PixelFormatUtil::IsCompressed(TSrcPixelFormat), "TSrcPixelFormat PixelFormat can not be compressed");
    static_assert(!PixelFormatUtil::IsPacked(TDstPixelFormat), "TDstPixelFormat PixelFormat can not be packed");
    static_assert(!PixelFormatUtil::IsPacked(TSrcPixelFormat), "TSrcPixelFormat PixelFormat can not be packed");

    // Generic slow conversion of four channels
    constexpr uint32_t NumChannels = 3u;
    static_assert(PixelFormatUtil::GetChannelCount(TDstPixelFormat) == (NumChannels + 1u));
    static_assert(PixelFormatUtil::GetChannelCount(TSrcPixelFormat) == NumChannels);

    assert(srcBitmap.GetPixelFormat() == TSrcPixelFormat);
    assert(dstBitmap.GetPixelFormat() == TDstPixelFormat);
    TransformThreeChannelsAndAddChannelRAW<TDstChannel, TSrcChannel>(dstBitmap, srcBitmap, fnUnaryOperation, newChannelValue);
  }

  //! Must be true
  //! - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  template <typename TDstChannel, PixelFormat TDstPixelFormat, typename TSrcChannel, PixelFormat TSrcPixelFormat, typename TUnaryOperation0,
            typename TUnaryOperation1>
  void TransformThreeChannelsTransformFourth(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap, TUnaryOperation0 fnUnaryOperation0,
                                             TUnaryOperation1 fnUnaryOperation1) noexcept
  {
    static_assert(!PixelFormatUtil::IsCompressed(TDstPixelFormat), "TDstPixelFormat PixelFormat can not be compressed");
    static_assert(!PixelFormatUtil::IsCompressed(TSrcPixelFormat), "TSrcPixelFormat PixelFormat can not be compressed");
    static_assert(!PixelFormatUtil::IsPacked(TDstPixelFormat), "TDstPixelFormat PixelFormat can not be packed");
    static_assert(!PixelFormatUtil::IsPacked(TSrcPixelFormat), "TSrcPixelFormat PixelFormat can not be packed");

    // Generic slow conversion of four channels
    constexpr uint32_t NumChannels = 4u;
    static_assert(PixelFormatUtil::GetChannelCount(TDstPixelFormat) == NumChannels);
    static_assert(PixelFormatUtil::GetChannelCount(TSrcPixelFormat) == NumChannels);

    assert(srcBitmap.GetPixelFormat() == TSrcPixelFormat);
    assert(dstBitmap.GetPixelFormat() == TDstPixelFormat);
    TransformThreeChannelsTransformFourthRAW<TDstChannel, TSrcChannel>(dstBitmap, srcBitmap, fnUnaryOperation0, fnUnaryOperation1);
  }

  //! Must be true
  //! - IsSafeInplaceModificationOrNoMemoryOverlap(rDstBitmap, srcBitmap) == true
  template <typename TDstChannel, PixelFormat TDstPixelFormat, typename TSrcChannel, PixelFormat TSrcPixelFormat, typename TUnaryOperation>
  void TransformFourChannels(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap, TUnaryOperation fnUnaryOperation) noexcept
  {
    static_assert(!PixelFormatUtil::IsCompressed(TDstPixelFormat), "TDstPixelFormat PixelFormat can not be compressed");
    static_assert(!PixelFormatUtil::IsCompressed(TSrcPixelFormat), "TSrcPixelFormat PixelFormat can not be compressed");
    static_assert(!PixelFormatUtil::IsPacked(TDstPixelFormat), "TDstPixelFormat PixelFormat can not be packed");
    static_assert(!PixelFormatUtil::IsPacked(TSrcPixelFormat), "TSrcPixelFormat PixelFormat can not be packed");

    // Generic slow conversion of four channels
    constexpr uint32_t NumChannels = 4u;
    static_assert(PixelFormatUtil::GetChannelCount(TDstPixelFormat) == NumChannels);
    static_assert(PixelFormatUtil::GetChannelCount(TSrcPixelFormat) == NumChannels);

    assert(srcBitmap.GetPixelFormat() == TSrcPixelFormat);
    assert(dstBitmap.GetPixelFormat() == TDstPixelFormat);
    TransformFourChannelsRAW<TDstChannel, TSrcChannel>(dstBitmap, srcBitmap, fnUnaryOperation);
  }
}
#endif
