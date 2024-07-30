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

#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslGraphics/Bitmap/TightBitmap.hpp>
#include <FslGraphics/Bitmap/UncheckedRawBitmapTransformer.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <array>

using namespace Fsl;

namespace
{
  using TestBitmap_UncheckedRawBitmapTransformer = TestFixtureFslGraphics;
}

TEST(TestBitmap_UncheckedRawBitmapTransformer, DoesMemoryRegionOverlap_Pointers)
{
  const std::array<uint8_t, 16> src{};
  const std::array<uint8_t, 16> dst{};

  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(dst.data(), dst.data() + dst.size(), src.data(), src.data() + src.size()));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data(), src.data() + src.size(), src.data(), src.data() + src.size()));

  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 0, src.data() + 1));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 1, src.data() + 2));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 2, src.data() + 3));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 3, src.data() + 4));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 4, src.data() + 5));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 5, src.data() + 6));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 6, src.data() + 7));

  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 0, src.data() + 2));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 1, src.data() + 3));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 2, src.data() + 4));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 3, src.data() + 5));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 4, src.data() + 6));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 5, src.data() + 7));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 6, src.data() + 8));

  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 0, src.data() + 3));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 1, src.data() + 4));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 2, src.data() + 5));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 3, src.data() + 6));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 4, src.data() + 7));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 5, src.data() + 8));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 6, src.data() + 9));

  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 0, src.data() + 4));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 1, src.data() + 5));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 2, src.data() + 6));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 3, src.data() + 7));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 4, src.data() + 8));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 5, src.data() + 9));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 6, src.data() + 10));

  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 0, src.data() + 5));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 1, src.data() + 6));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 2, src.data() + 7));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 3, src.data() + 8));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 4, src.data() + 9));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 5, src.data() + 10));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6u, src.data() + 6, src.data() + 11));


  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 0, src.data() + 1, src.data() + 4, src.data() + 6u));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 1, src.data() + 2, src.data() + 4, src.data() + 6u));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 2, src.data() + 3, src.data() + 4, src.data() + 6u));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 3, src.data() + 4, src.data() + 4, src.data() + 6u));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 5, src.data() + 4, src.data() + 6u));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 5, src.data() + 6, src.data() + 4, src.data() + 6u));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 6, src.data() + 7, src.data() + 4, src.data() + 6u));

  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 0, src.data() + 2, src.data() + 4, src.data() + 6u));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 1, src.data() + 3, src.data() + 4, src.data() + 6u));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 2, src.data() + 4, src.data() + 4, src.data() + 6u));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 3, src.data() + 5, src.data() + 4, src.data() + 6u));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 6, src.data() + 4, src.data() + 6u));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 5, src.data() + 7, src.data() + 4, src.data() + 6u));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 6, src.data() + 8, src.data() + 4, src.data() + 6u));

  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 0, src.data() + 3, src.data() + 4, src.data() + 6u));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 1, src.data() + 4, src.data() + 4, src.data() + 6u));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 2, src.data() + 5, src.data() + 4, src.data() + 6u));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 3, src.data() + 6, src.data() + 4, src.data() + 6u));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 7, src.data() + 4, src.data() + 6u));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 5, src.data() + 8, src.data() + 4, src.data() + 6u));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 6, src.data() + 9, src.data() + 4, src.data() + 6u));

  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 0, src.data() + 4, src.data() + 4, src.data() + 6u));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 1, src.data() + 5, src.data() + 4, src.data() + 6u));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 2, src.data() + 6, src.data() + 4, src.data() + 6u));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 3, src.data() + 7, src.data() + 4, src.data() + 6u));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 8, src.data() + 4, src.data() + 6u));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 5, src.data() + 9, src.data() + 4, src.data() + 6u));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 6, src.data() + 10, src.data() + 4, src.data() + 6u));

  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 0, src.data() + 5, src.data() + 4, src.data() + 6u));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 1, src.data() + 6, src.data() + 4, src.data() + 6u));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 2, src.data() + 7, src.data() + 4, src.data() + 6u));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 3, src.data() + 8, src.data() + 4, src.data() + 6u));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 4, src.data() + 9, src.data() + 4, src.data() + 6u));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 5, src.data() + 10, src.data() + 4, src.data() + 6u));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(src.data() + 6, src.data() + 11, src.data() + 4, src.data() + 6u));
}


TEST(TestBitmap_UncheckedRawBitmapTransformer, DoesMemoryRegionOverlap_RawBitmap_Empty)
{
  TightBitmap src;
  TightBitmap dst;

  // Empty bitmaps do not overlap
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(dst.AsRawBitmap(), src.AsRawBitmap()));
  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(dst.AsRawBitmap(), dst.AsRawBitmap()));
}

TEST(TestBitmap_UncheckedRawBitmapTransformer, DoesMemoryRegionOverlap_RawBitmap)
{
  TightBitmap src(PxSize2D::Create(2, 3), PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::UpperLeft);
  TightBitmap dst(PxSize2D::Create(2, 3), PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::UpperLeft);

  EXPECT_FALSE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(dst.AsRawBitmap(), src.AsRawBitmap()));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(dst.AsRawBitmap(), dst.AsRawBitmap()));
}


TEST(TestBitmap_UncheckedRawBitmapTransformer, IsSafeInplaceModificationOrNoMemoryOverlap_Empty)
{
  TightBitmap src;
  TightBitmap dst;

  // Empty bitmaps are considered safe (it can satisfy both no overlap and safe inplace)
  EXPECT_TRUE(UncheckedRawBitmapTransformer::IsSafeInplaceModificationOrNoMemoryOverlap(dst.AsRawBitmap(), src.AsRawBitmap()));
  EXPECT_TRUE(UncheckedRawBitmapTransformer::IsSafeInplaceModificationOrNoMemoryOverlap(dst.AsRawBitmap(), dst.AsRawBitmap()));
}

TEST(TestBitmap_UncheckedRawBitmapTransformer, IsSafeInplaceModificationOrNoMemoryOverlap)
{
  TightBitmap src(PxSize2D::Create(2, 3), PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::UpperLeft);
  TightBitmap dst(PxSize2D::Create(2, 3), PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::UpperLeft);

  // No overlap
  EXPECT_TRUE(UncheckedRawBitmapTransformer::IsSafeInplaceModificationOrNoMemoryOverlap(dst.AsRawBitmap(), src.AsRawBitmap()));
  // Safe overlap
  EXPECT_TRUE(UncheckedRawBitmapTransformer::IsSafeInplaceModificationOrNoMemoryOverlap(dst.AsRawBitmap(), dst.AsRawBitmap()));

  {
    constexpr auto SizePx = PxSize2D::Create(10, 10);
    constexpr auto Origin = BitmapOrigin::UpperLeft;
    std::array<uint8_t, 100 * 100> bitmapData{};
    auto bitmapDataSpan = SpanUtil::AsSpan(bitmapData);

    auto src = RawBitmapEx::Create(bitmapDataSpan.subspan(100, 4 * 10 * 10), SizePx, PixelFormat::R8G8B8A8_UNORM, 4 * 10, Origin);
    // Same start location, same pixel byte size and stride
    auto safeOverlap1 = RawBitmapEx::Create(bitmapDataSpan.subspan(100, 4 * 10 * 10), SizePx, PixelFormat::R8G8B8A8_UNORM, 4 * 10, Origin);
    // Same start location, dst pixel byte size < src pixel byte size (same stride)
    auto safeOverlap2 = RawBitmapEx::Create(bitmapDataSpan.subspan(100, 4 * 10 * 10), SizePx, PixelFormat::R8G8B8_UNORM, 4 * 10, Origin);
    // Same start location, dst pixel byte size < src pixel byte size, dst stride < src stride
    auto safeOverlap3 = RawBitmapEx::Create(bitmapDataSpan.subspan(100, 3 * 10 * 10), SizePx, PixelFormat::R8G8B8_UNORM, 3 * 10, Origin);

    // Unsafe inplace overlap due to start address not being the same
    auto unsafeOverlap1 = RawBitmapEx::Create(bitmapDataSpan.subspan(99, 4 * 10 * 10), SizePx, PixelFormat::R8G8B8A8_UNORM, 4 * 10, Origin);
    auto unsafeOverlap2 = RawBitmapEx::Create(bitmapDataSpan.subspan(101, 4 * 10 * 10), SizePx, PixelFormat::R8G8B8A8_UNORM, 4 * 10, Origin);
    // Same start location, dst pixel byte size == src pixel byte size, dst stride > srcStride
    auto unsafeOverlap3 = RawBitmapEx::Create(bitmapDataSpan.subspan(100, 5 * 10 * 10), SizePx, PixelFormat::R8G8B8A8_UNORM, 5 * 10, Origin);
    // Same start location, dst pixel byte size > src pixel byte size, dst stride > srcStride
    auto unsafeOverlap4 = RawBitmapEx::Create(bitmapDataSpan.subspan(100, 8 * 10 * 10), SizePx, PixelFormat::R16G16B16A16_UNORM, 8 * 10, Origin);


    EXPECT_TRUE(UncheckedRawBitmapTransformer::IsSafeInplaceModificationOrNoMemoryOverlap(safeOverlap1, src));
    EXPECT_TRUE(UncheckedRawBitmapTransformer::IsSafeInplaceModificationOrNoMemoryOverlap(safeOverlap2, src));
    EXPECT_TRUE(UncheckedRawBitmapTransformer::IsSafeInplaceModificationOrNoMemoryOverlap(safeOverlap3, src));

    EXPECT_FALSE(UncheckedRawBitmapTransformer::IsSafeInplaceModificationOrNoMemoryOverlap(unsafeOverlap1, src));
    EXPECT_FALSE(UncheckedRawBitmapTransformer::IsSafeInplaceModificationOrNoMemoryOverlap(unsafeOverlap2, src));
    EXPECT_FALSE(UncheckedRawBitmapTransformer::IsSafeInplaceModificationOrNoMemoryOverlap(unsafeOverlap3, src));
    EXPECT_FALSE(UncheckedRawBitmapTransformer::IsSafeInplaceModificationOrNoMemoryOverlap(unsafeOverlap4, src));
  }
}

TEST(TestBitmap_UncheckedRawBitmapTransformer, IsSafeInplaceModificationOrNoMemoryOverlap2)
{
  TightBitmap src(PxSize2D::Create(2, 3), PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::UpperLeft);
  TightBitmap dst(PxSize2D::Create(2, 3), PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::UpperLeft);

  // No overlap
  EXPECT_TRUE(UncheckedRawBitmapTransformer::IsSafeInplaceModificationOrNoMemoryOverlap(dst.AsRawBitmap(), src.AsRawBitmap()));
  // Safe overlap
  EXPECT_TRUE(UncheckedRawBitmapTransformer::IsSafeInplaceModificationOrNoMemoryOverlap(dst.AsRawBitmap(), dst.AsRawBitmap()));

  {
    constexpr auto SizePx = PxSize2D::Create(10, 10);
    constexpr auto Origin = BitmapOrigin::UpperLeft;
    std::array<uint8_t, 100 * 100> bitmapData{};
    auto bitmapDataSpan = SpanUtil::AsSpan(bitmapData);

    auto src = RawBitmapEx::Create(bitmapDataSpan.subspan(100, 9 * 10 * 10), SizePx, PixelFormat::R8G8B8A8_UNORM, 9 * 10, Origin);
    // Same start location, same pixel byte size and stride
    auto safeOverlap1 = RawBitmapEx::Create(bitmapDataSpan.subspan(100, 9 * 10 * 10), SizePx, PixelFormat::R8G8B8A8_UNORM, 9 * 10, Origin);
    // Same start location, dst pixel byte size < src pixel byte size (same stride)
    auto safeOverlap2 = RawBitmapEx::Create(bitmapDataSpan.subspan(100, 9 * 10 * 10), SizePx, PixelFormat::R8G8B8_UNORM, 9 * 10, Origin);
    // Same start location, dst pixel byte size < src pixel byte size, dst stride < src stride
    auto safeOverlap3 = RawBitmapEx::Create(bitmapDataSpan.subspan(100, 3 * 10 * 10), SizePx, PixelFormat::R8G8B8_UNORM, 3 * 10, Origin);

    // Unsafe inplace overlap due to start address not being the same
    auto unsafeOverlap1 = RawBitmapEx::Create(bitmapDataSpan.subspan(99, 9 * 10 * 10), SizePx, PixelFormat::R8G8B8A8_UNORM, 9 * 10, Origin);
    auto unsafeOverlap2 = RawBitmapEx::Create(bitmapDataSpan.subspan(101, 9 * 10 * 10), SizePx, PixelFormat::R8G8B8A8_UNORM, 9 * 10, Origin);
    // Same start location, dst pixel byte size == src pixel byte size, dst stride > srcStride
    auto unsafeOverlap3 = RawBitmapEx::Create(bitmapDataSpan.subspan(100, 10 * 10 * 10), SizePx, PixelFormat::R8G8B8A8_UNORM, 10 * 10, Origin);
    // Same start location, dst pixel byte size > src pixel byte size, dst stride < srcStride
    auto unsafeOverlap4 = RawBitmapEx::Create(bitmapDataSpan.subspan(100, 8 * 10 * 10), SizePx, PixelFormat::R16G16B16A16_UNORM, 8 * 10, Origin);


    EXPECT_TRUE(UncheckedRawBitmapTransformer::IsSafeInplaceModificationOrNoMemoryOverlap(safeOverlap1, src));
    EXPECT_TRUE(UncheckedRawBitmapTransformer::IsSafeInplaceModificationOrNoMemoryOverlap(safeOverlap2, src));
    EXPECT_TRUE(UncheckedRawBitmapTransformer::IsSafeInplaceModificationOrNoMemoryOverlap(safeOverlap3, src));

    EXPECT_FALSE(UncheckedRawBitmapTransformer::IsSafeInplaceModificationOrNoMemoryOverlap(unsafeOverlap1, src));
    EXPECT_FALSE(UncheckedRawBitmapTransformer::IsSafeInplaceModificationOrNoMemoryOverlap(unsafeOverlap2, src));
    EXPECT_FALSE(UncheckedRawBitmapTransformer::IsSafeInplaceModificationOrNoMemoryOverlap(unsafeOverlap3, src));
    EXPECT_FALSE(UncheckedRawBitmapTransformer::IsSafeInplaceModificationOrNoMemoryOverlap(unsafeOverlap4, src));
  }
}
