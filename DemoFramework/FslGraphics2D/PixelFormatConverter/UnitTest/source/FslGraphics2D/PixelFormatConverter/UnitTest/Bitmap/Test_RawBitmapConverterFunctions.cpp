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
#include <FslBase/Log/Math/Pixel/FmtPxExtent2D.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslGraphics/Bitmap/TightBitmap.hpp>
#include <FslGraphics/ColorSpaceConversion.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <FslGraphics2D/PixelFormatConverter/Bitmap/RawBitmapConverterFunctions.hpp>
#include <half.hpp>
#include <array>
#include "UnitTestRawBitmapHelper.hpp"

using namespace Fsl;

namespace
{
  using TestBitmap_RawBitmapConverterFunctions = TestFixtureFslGraphics;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestBitmap_RawBitmapConverterFunctions, DoesMemoryRegionOverlap_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R8G8B8_SRGB, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16_UNORM, BitmapOrigin::UpperLeft);

  // Empty never overlaps!
  ASSERT_FALSE(FslGraphics2D::RawBitmapConverterFunctions::DoesMemoryRegionOverlap(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap()));
  ASSERT_FALSE(FslGraphics2D::RawBitmapConverterFunctions::DoesMemoryRegionOverlap(dstBitmap.AsRawBitmap(), dstBitmap.AsRawBitmap()));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestBitmap_RawBitmapConverterFunctions, DoesMemoryRegionOverlap)
{
  const TightBitmap srcBitmap(PxSize2D::Create(3, 2), PixelFormat::R8G8B8_SRGB, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16_UNORM, BitmapOrigin::UpperLeft);

  ASSERT_FALSE(FslGraphics2D::RawBitmapConverterFunctions::DoesMemoryRegionOverlap(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap()));
  ASSERT_TRUE(FslGraphics2D::RawBitmapConverterFunctions::DoesMemoryRegionOverlap(dstBitmap.AsRawBitmap(), dstBitmap.AsRawBitmap()));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestBitmap_RawBitmapConverterFunctions, IsSafeInplaceModificationOrNoMemoryOverlap_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R8G8B8_SRGB, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16_UNORM, BitmapOrigin::UpperLeft);

  // Empty is always considered safe!
  ASSERT_TRUE(
    FslGraphics2D::RawBitmapConverterFunctions::IsSafeInplaceModificationOrNoMemoryOverlap(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap()));
  ASSERT_TRUE(
    FslGraphics2D::RawBitmapConverterFunctions::IsSafeInplaceModificationOrNoMemoryOverlap(dstBitmap.AsRawBitmap(), dstBitmap.AsRawBitmap()));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestBitmap_RawBitmapConverterFunctions, IsSafeInplaceModificationOrNoMemoryOverlap)
{
  const TightBitmap srcBitmap(PxSize2D::Create(3, 2), PixelFormat::R8G8B8_SRGB, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16_UNORM, BitmapOrigin::UpperLeft);

  // No overlap so safe
  ASSERT_TRUE(
    FslGraphics2D::RawBitmapConverterFunctions::IsSafeInplaceModificationOrNoMemoryOverlap(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap()));
  // Safe inplace modification
  ASSERT_TRUE(
    FslGraphics2D::RawBitmapConverterFunctions::IsSafeInplaceModificationOrNoMemoryOverlap(dstBitmap.AsRawBitmap(), dstBitmap.AsRawBitmap()));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR8G8B8SrgbToR16G16B16UNorm_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R8G8B8_SRGB, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16_UNORM, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8SrgbToR16G16B16UNorm(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<uint8_t, uint16_t>(
    srcBitmap.AsSpan(), UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(dstBitmap.AsSpan()), UnitTestRawBitmapHelper::ConvertSrg8ToLinearUInt16);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR8G8B8SrgbToR16G16B16UNorm)
{
  const TightBitmap srcBitmap(UnitTestRawBitmapHelper::CreateR8G8B8Srgb());
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16_UNORM, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8SrgbToR16G16B16UNorm(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<uint8_t, uint16_t>(
    srcBitmap.AsSpan(), UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(dstBitmap.AsSpan()), UnitTestRawBitmapHelper::ConvertSrg8ToLinearUInt16);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR8G8B8A8SrgbToR16G16B16A16UNorm_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R8G8B8A8_SRGB, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16A16_UNORM, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8A8SrgbToR16G16B16A16UNorm(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch3And1<uint8_t, uint16_t>(
    srcBitmap.AsSpan(), UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(dstBitmap.AsSpan()), UnitTestRawBitmapHelper::ConvertSrg8ToLinearUInt16,
    UnitTestRawBitmapHelper::ConvertLinearUInt8ToLinearUInt16);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR8G8B8A8SrgbToR16G16B16A16UNorm)
{
  const TightBitmap srcBitmap(UnitTestRawBitmapHelper::CreateR8G8B8A8Srgb());
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16A16_UNORM, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8A8SrgbToR16G16B16A16UNorm(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch3And1<uint8_t, uint16_t>(
    srcBitmap.AsSpan(), UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(dstBitmap.AsSpan()), UnitTestRawBitmapHelper::ConvertSrg8ToLinearUInt16,
    UnitTestRawBitmapHelper::ConvertLinearUInt8ToLinearUInt16);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR8G8B8SrgbToR16G16B16Float_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R8G8B8_SRGB, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8SrgbToR16G16B16Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<uint8_t, uint16_t>(
    srcBitmap.AsSpan(), UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(dstBitmap.AsSpan()), UnitTestRawBitmapHelper::ConvertSrgb8ToLinearFp16);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR8G8B8SrgbToR16G16B16Float)
{
  const TightBitmap srcBitmap(UnitTestRawBitmapHelper::CreateR8G8B8Srgb());
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8SrgbToR16G16B16Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<uint8_t, uint16_t>(
    srcBitmap.AsSpan(), UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(dstBitmap.AsSpan()), UnitTestRawBitmapHelper::ConvertSrgb8ToLinearFp16);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR8G8B8A8SrgbToR16G16B16A16Float_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R8G8B8A8_SRGB, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16A16_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8A8SrgbToR16G16B16A16Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch3And1<uint8_t, uint16_t>(
    srcBitmap.AsSpan(), UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(dstBitmap.AsSpan()), UnitTestRawBitmapHelper::ConvertSrgb8ToLinearFp16,
    UnitTestRawBitmapHelper::ConvertLinearUInt8ToLinearFp16);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR8G8B8A8SrgbToR16G16B16A16Float)
{
  const TightBitmap srcBitmap(UnitTestRawBitmapHelper::CreateR8G8B8A8Srgb());
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16A16_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8A8SrgbToR16G16B16A16Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch3And1<uint8_t, uint16_t>(
    srcBitmap.AsSpan(), UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(dstBitmap.AsSpan()), UnitTestRawBitmapHelper::ConvertSrgb8ToLinearFp16,
    UnitTestRawBitmapHelper::ConvertLinearUInt8ToLinearFp16);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR8G8B8SrgbToR32G32B32Float_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R8G8B8_SRGB, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R32G32B32_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8SrgbToR32G32B32Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<uint8_t, float>(
    srcBitmap.AsSpan(), UnitTestRawBitmapHelper::ReinterpretSpanToFloat(dstBitmap.AsSpan()), UnitTestRawBitmapHelper::ConvertSRGB8ToLinearFloat);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR8G8B8SrgbToR32G32B32Float)
{
  const TightBitmap srcBitmap(UnitTestRawBitmapHelper::CreateR8G8B8Srgb());
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R32G32B32_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8SrgbToR32G32B32Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<uint8_t, float>(
    srcBitmap.AsSpan(), UnitTestRawBitmapHelper::ReinterpretSpanToFloat(dstBitmap.AsSpan()), UnitTestRawBitmapHelper::ConvertSRGB8ToLinearFloat);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR8G8B8A8SrgbToR32G32B32A32Float_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R8G8B8A8_SRGB, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R32G32B32A32_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8A8SrgbToR32G32B32A32Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch3And1<uint8_t, float>(
    srcBitmap.AsSpan(), UnitTestRawBitmapHelper::ReinterpretSpanToFloat(dstBitmap.AsSpan()), UnitTestRawBitmapHelper::ConvertSRGB8ToLinearFloat,
    UnitTestRawBitmapHelper::ConvertLinearUInt8ToLinearFloat);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR8G8B8A8SrgbToR32G32B32A32Float)
{
  const TightBitmap srcBitmap(UnitTestRawBitmapHelper::CreateR8G8B8A8Srgb());
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R32G32B32A32_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR8G8B8A8SrgbToR32G32B32A32Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch3And1<uint8_t, float>(
    srcBitmap.AsSpan(), UnitTestRawBitmapHelper::ReinterpretSpanToFloat(dstBitmap.AsSpan()), UnitTestRawBitmapHelper::ConvertSRGB8ToLinearFloat,
    UnitTestRawBitmapHelper::ConvertLinearUInt8ToLinearFloat);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR16G16B16UNormToR8G8B8Srgb_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R16G16B16_UNORM, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R8G8B8_SRGB, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR16G16B16UNormToR8G8B8Srgb(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<uint16_t, uint8_t>(UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(srcBitmap.AsSpan()),
                                                                    dstBitmap.AsSpan(), UnitTestRawBitmapHelper::ConvertLinearUInt16ToSrg8);
}

TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR16G16B16UNormToR8G8B8Srgb)
{
  const TightBitmap srcBitmap(UnitTestRawBitmapHelper::CreateR16G16B16UNorm());
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R8G8B8_SRGB, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR16G16B16UNormToR8G8B8Srgb(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<uint16_t, uint8_t>(UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(srcBitmap.AsSpan()),
                                                                    dstBitmap.AsSpan(), UnitTestRawBitmapHelper::ConvertLinearUInt16ToSrg8);
}

TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR16G16B16A16UNormToR8G8B8A8Srgb_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R16G16B16A16_UNORM, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R8G8B8A8_SRGB, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR16G16B16A16UNormToR8G8B8A8Srgb(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch3And1<uint16_t, uint8_t>(UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(srcBitmap.AsSpan()),
                                                                         dstBitmap.AsSpan(), UnitTestRawBitmapHelper::ConvertLinearUInt16ToSrg8,
                                                                         UnitTestRawBitmapHelper::ConvertLinearUInt16ToLinearUInt8);
}

TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR16G16B16A16UNormToR8G8B8A8Srgb)
{
  const TightBitmap srcBitmap(UnitTestRawBitmapHelper::CreateR16G16B16A16UNorm());
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R8G8B8A8_SRGB, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR16G16B16A16UNormToR8G8B8A8Srgb(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch3And1<uint16_t, uint8_t>(UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(srcBitmap.AsSpan()),
                                                                         dstBitmap.AsSpan(), UnitTestRawBitmapHelper::ConvertLinearUInt16ToSrg8,
                                                                         UnitTestRawBitmapHelper::ConvertLinearUInt16ToLinearUInt8);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR16G16B16FloatToR8G8B8Srgb_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R16G16B16_SFLOAT, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R8G8B8_SRGB, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR16G16B16FloatToR8G8B8Srgb(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<uint16_t, uint8_t>(UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(srcBitmap.AsSpan()),
                                                                    dstBitmap.AsSpan(), UnitTestRawBitmapHelper::ConvertLinearFP16ToSrg8);
}

TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR16G16B16FloatToR8G8B8Srgb)
{
  const TightBitmap srcBitmap(UnitTestRawBitmapHelper::CreateR16G16B16Float());
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R8G8B8_SRGB, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR16G16B16FloatToR8G8B8Srgb(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<uint16_t, uint8_t>(UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(srcBitmap.AsSpan()),
                                                                    dstBitmap.AsSpan(), UnitTestRawBitmapHelper::ConvertLinearFP16ToSrg8);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR16G16B16A16FloatToR8G8B8A8Srgb_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R16G16B16A16_SFLOAT, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R8G8B8A8_SRGB, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR16G16B16A16FloatToR8G8B8A8Srgb(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch3And1<uint16_t, uint8_t>(UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(srcBitmap.AsSpan()),
                                                                         dstBitmap.AsSpan(), UnitTestRawBitmapHelper::ConvertLinearFP16ToSrg8,
                                                                         UnitTestRawBitmapHelper::ConvertLinearFP16ToLinearUInt8);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR16G16B16A16FloatToR8G8B8A8Srgb)
{
  const TightBitmap srcBitmap(UnitTestRawBitmapHelper::CreateR16G16B16A16Float());
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R8G8B8A8_SRGB, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR16G16B16A16FloatToR8G8B8A8Srgb(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch3And1<uint16_t, uint8_t>(UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(srcBitmap.AsSpan()),
                                                                         dstBitmap.AsSpan(), UnitTestRawBitmapHelper::ConvertLinearFP16ToSrg8,
                                                                         UnitTestRawBitmapHelper::ConvertLinearFP16ToLinearUInt8);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR32G32B32FloatToR8G8B8Srgb_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R32G32B32_SFLOAT, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R8G8B8_SRGB, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR32G32B32FloatToR8G8B8Srgb(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<float, uint8_t>(UnitTestRawBitmapHelper::ReinterpretSpanToFloat(srcBitmap.AsSpan()),
                                                                 dstBitmap.AsSpan(), UnitTestRawBitmapHelper::ConvertLinearFloatToSrg8);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR32G32B32FloatToR8G8B8Srgb)
{
  const TightBitmap srcBitmap(UnitTestRawBitmapHelper::CreateR32G32B32Float());
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R8G8B8_SRGB, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR32G32B32FloatToR8G8B8Srgb(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<float, uint8_t>(UnitTestRawBitmapHelper::ReinterpretSpanToFloat(srcBitmap.AsSpan()),
                                                                 dstBitmap.AsSpan(), UnitTestRawBitmapHelper::ConvertLinearFloatToSrg8);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR32G32B32A32FloatToR8G8B8A8Srgb_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R32G32B32A32_SFLOAT, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R8G8B8A8_SRGB, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR32G32B32A32FloatToR8G8B8A8Srgb(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch3And1<float, uint8_t>(UnitTestRawBitmapHelper::ReinterpretSpanToFloat(srcBitmap.AsSpan()),
                                                                      dstBitmap.AsSpan(), UnitTestRawBitmapHelper::ConvertLinearFloatToSrg8,
                                                                      UnitTestRawBitmapHelper::ConvertLinearFloatToLinearUInt8);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR32G32B32A32FloatToR8G8B8A8Srgb)
{
  const TightBitmap srcBitmap(UnitTestRawBitmapHelper::CreateR32G32B32A32Float());
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R8G8B8A8_SRGB, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR32G32B32A32FloatToR8G8B8A8Srgb(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch3And1<float, uint8_t>(UnitTestRawBitmapHelper::ReinterpretSpanToFloat(srcBitmap.AsSpan()),
                                                                      dstBitmap.AsSpan(), UnitTestRawBitmapHelper::ConvertLinearFloatToSrg8,
                                                                      UnitTestRawBitmapHelper::ConvertLinearFloatToLinearUInt8);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR16G16B16FloatToR32G32B32Float_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R16G16B16_SFLOAT, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R32G32B32_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR16G16B16FloatToR32G32B32Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<uint16_t, float>(UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(srcBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ReinterpretSpanToFloat(dstBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ConvertLinearFP16ToLinearFloat);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR16G16B16FloatToR32G32B32Float)
{
  const TightBitmap srcBitmap(UnitTestRawBitmapHelper::CreateR16G16B16Float());
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R32G32B32_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR16G16B16FloatToR32G32B32Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<uint16_t, float>(UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(srcBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ReinterpretSpanToFloat(dstBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ConvertLinearFP16ToLinearFloat);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR16G16B16A16FloatToR32G32B32A32Float_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R16G16B16A16_SFLOAT, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R32G32B32A32_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR16G16B16A16FloatToR32G32B32A32Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<uint16_t, float>(UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(srcBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ReinterpretSpanToFloat(dstBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ConvertLinearFP16ToLinearFloat);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR16G16B16A16FloatToR32G32B32A32Float)
{
  const TightBitmap srcBitmap(UnitTestRawBitmapHelper::CreateR16G16B16A16Float());
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R32G32B32A32_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR16G16B16A16FloatToR32G32B32A32Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<uint16_t, float>(UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(srcBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ReinterpretSpanToFloat(dstBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ConvertLinearFP16ToLinearFloat);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR32G32B32FloatToR16G16B16Float_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R32G32B32_SFLOAT, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR32G32B32FloatToR16G16B16Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<float, uint16_t>(UnitTestRawBitmapHelper::ReinterpretSpanToFloat(srcBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(dstBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ConvertLinearFloatToLinearFp16);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR32G32B32FloatToR16G16B16Float)
{
  const TightBitmap srcBitmap(UnitTestRawBitmapHelper::CreateR32G32B32Float());
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR32G32B32FloatToR16G16B16Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<float, uint16_t>(UnitTestRawBitmapHelper::ReinterpretSpanToFloat(srcBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(dstBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ConvertLinearFloatToLinearFp16);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR32G32B32A32FloatToR16G16B16A16Float_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R32G32B32A32_SFLOAT, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16A16_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR32G32B32A32FloatToR16G16B16A16Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<float, uint16_t>(UnitTestRawBitmapHelper::ReinterpretSpanToFloat(srcBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(dstBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ConvertLinearFloatToLinearFp16);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR32G32B32A32FloatToR16G16B16A16Float)
{
  const TightBitmap srcBitmap(UnitTestRawBitmapHelper::CreateR32G32B32A32Float());
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16A16_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR32G32B32A32FloatToR16G16B16A16Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<float, uint16_t>(UnitTestRawBitmapHelper::ReinterpretSpanToFloat(srcBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(dstBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ConvertLinearFloatToLinearFp16);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR16G16B16UNormToR32G32B32Float_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R16G16B16_UNORM, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R32G32B32_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR16G16B16UNormToR32G32B32Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<uint16_t, float>(UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(srcBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ReinterpretSpanToFloat(dstBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ConvertLinearUInt16ToLinearFloat);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR16G16B16UNormToR32G32B32Float)
{
  const TightBitmap srcBitmap(UnitTestRawBitmapHelper::CreateR16G16B16UNorm());
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R32G32B32_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR16G16B16UNormToR32G32B32Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<uint16_t, float>(UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(srcBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ReinterpretSpanToFloat(dstBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ConvertLinearUInt16ToLinearFloat);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR16G16B16A16UNormToR32G32B32A32Float_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R16G16B16A16_UNORM, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R32G32B32A32_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR16G16B16A16UNormToR32G32B32A32Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<uint16_t, float>(UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(srcBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ReinterpretSpanToFloat(dstBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ConvertLinearUInt16ToLinearFloat);
}

TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR16G16B16A16UNormToR32G32B32A32Float)
{
  const TightBitmap srcBitmap(UnitTestRawBitmapHelper::CreateR16G16B16A16UNorm());
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R32G32B32A32_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR16G16B16A16UNormToR32G32B32A32Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<uint16_t, float>(UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(srcBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ReinterpretSpanToFloat(dstBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ConvertLinearUInt16ToLinearFloat);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR32G32B32FloatToR16G16B16UNorm_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R32G32B32_SFLOAT, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16_UNORM, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR32G32B32FloatToR16G16B16UNorm(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<float, uint16_t>(UnitTestRawBitmapHelper::ReinterpretSpanToFloat(srcBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(dstBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ConvertLinearFloatToLinearUInt16);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR32G32B32FloatToR16G16B16UNorm)
{
  const TightBitmap srcBitmap(UnitTestRawBitmapHelper::CreateR32G32B32Float());
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16_UNORM, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR32G32B32FloatToR16G16B16UNorm(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<float, uint16_t>(UnitTestRawBitmapHelper::ReinterpretSpanToFloat(srcBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(dstBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ConvertLinearFloatToLinearUInt16);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR32G32B32A32FloatToR16G16B16A16UNorm_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R32G32B32A32_SFLOAT, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16A16_UNORM, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR32G32B32A32FloatToR16G16B16A16UNorm(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<float, uint16_t>(UnitTestRawBitmapHelper::ReinterpretSpanToFloat(srcBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(dstBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ConvertLinearFloatToLinearUInt16);
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR32G32B32A32FloatToR16G16B16A16UNorm)
{
  const TightBitmap srcBitmap(UnitTestRawBitmapHelper::CreateR32G32B32A32Float());
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16A16_UNORM, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR32G32B32A32FloatToR16G16B16A16UNorm(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransformMatch<float, uint16_t>(UnitTestRawBitmapHelper::ReinterpretSpanToFloat(srcBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(dstBitmap.AsSpan()),
                                                                  UnitTestRawBitmapHelper::ConvertLinearFloatToLinearUInt16);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Special conversion functions
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR32G32B32FloatToR16G16B16A16Float_Empty)
{
  const TightBitmap srcBitmap(PxSize2D(), PixelFormat::R32G32B32_SFLOAT, BitmapOrigin::UpperLeft);
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16A16_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR32G32B32FloatToR16G16B16A16Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransform3To4Match<float, uint16_t>(
    UnitTestRawBitmapHelper::ReinterpretSpanToFloat(srcBitmap.AsSpan()), UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(dstBitmap.AsSpan()),
    srcBitmap.RawUnsignedWidth(), UnitTestRawBitmapHelper::ConvertLinearFloatToLinearFp16,
    UnitTestRawBitmapHelper::ConvertLinearFloatToLinearFp16(1.0f));
}


TEST(TestBitmap_RawBitmapConverterFunctions, UncheckedR32G32B32FloatToR16G16B16A16Float)
{
  const TightBitmap srcBitmap(UnitTestRawBitmapHelper::CreateR32G32B32Float());
  TightBitmap dstBitmap(srcBitmap.GetSize(), PixelFormat::R16G16B16A16_SFLOAT, BitmapOrigin::UpperLeft);

  FslGraphics2D::RawBitmapConverterFunctions::UncheckedR32G32B32FloatToR16G16B16A16Float(dstBitmap.AsRawBitmap(), srcBitmap.AsRawBitmap());

  UnitTestRawBitmapHelper::CheckIfTransform3To4Match<float, uint16_t>(
    UnitTestRawBitmapHelper::ReinterpretSpanToFloat(srcBitmap.AsSpan()), UnitTestRawBitmapHelper::ReinterpretSpanToUInt16(dstBitmap.AsSpan()),
    srcBitmap.RawUnsignedWidth(), UnitTestRawBitmapHelper::ConvertLinearFloatToLinearFp16,
    UnitTestRawBitmapHelper::ConvertLinearFloatToLinearFp16(1.0f));
}
