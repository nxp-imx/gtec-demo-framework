/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/Bitmap/BitmapImageConfig.hpp>
#include <FslGraphics/UnitTest/Helper/ImageTestTemplate.hpp>
#include <FslGraphics/UnitTest/Helper/Log/LogPixelUInt8.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  template <typename TCurrent>
  class TestFixtureFslGraphics_Bitmap : public TestFixtureFslGraphics
  {
  public:
    using current_t = TCurrent;
  };

  template <typename TImageConfig, BitmapOrigin TBitmapOrigin>
  struct TestConfig
  {
    using imageConfig_t = TImageConfig;

    static const BitmapOrigin Origin = TBitmapOrigin;
  };

  using TestType1A = TestConfig<BitmapImageConfig::R8G8B8, BitmapOrigin::Undefined>;
  using TestType1B = TestConfig<BitmapImageConfig::R8G8B8, BitmapOrigin::UpperLeft>;
  using TestType1C = TestConfig<BitmapImageConfig::R8G8B8, BitmapOrigin::LowerLeft>;

  using TestType2A = TestConfig<BitmapImageConfig::R8G8B8A8, BitmapOrigin::Undefined>;
  using TestType2B = TestConfig<BitmapImageConfig::R8G8B8A8, BitmapOrigin::UpperLeft>;
  using TestType2C = TestConfig<BitmapImageConfig::R8G8B8A8, BitmapOrigin::LowerLeft>;


  // Bitmap -> Texture
  using TestTypes = ::testing::Types<TestType1A, TestType1B, TestType1C, TestType2A, TestType2B, TestType2C>;

  TYPED_TEST_SUITE(TestFixtureFslGraphics_Bitmap, TestTypes);
}


TYPED_TEST(TestFixtureFslGraphics_Bitmap, CheckScopedDirectReadAccess)
{
  using current_t = typename TestFixture::current_t;

  using imageConfig_t = typename current_t::imageConfig_t;
  using template_t = ImageTestTemplate<imageConfig_t>;

  const auto pixelFormat = imageConfig_t::ActivePixelFormat;
  const auto pixelLayout = imageConfig_t::ActivePixelLayout;
  const uint32_t width = 4;
  const uint32_t height = 2;
  const auto bitmapOrigin = current_t::Origin;
  const auto minStride = PixelFormatUtil::CalcMinimumStride(width, pixelFormat);
  const auto bufferLength = height * minStride;

  Bitmap bitmap = template_t::GetBasic4X2(bitmapOrigin);
  RawBitmap rawBitmap;
  Bitmap::ScopedDirectAccess directAccess(bitmap, rawBitmap);

  // The bitmap defaults to UpperLeft if the origin is undefined.
  const auto expectedBitmapOrigin = bitmapOrigin != BitmapOrigin::Undefined ? bitmapOrigin : BitmapOrigin::UpperLeft;


  // Known value checks just to be sure
  EXPECT_TRUE(rawBitmap.IsValid());
  EXPECT_NE(rawBitmap.Content(), nullptr);
  EXPECT_EQ(rawBitmap.Width(), width);
  EXPECT_EQ(rawBitmap.Height(), height);
  EXPECT_EQ(rawBitmap.Stride(), minStride);
  EXPECT_EQ(rawBitmap.GetByteSize(), bufferLength);
  EXPECT_EQ(rawBitmap.GetExtent(), Extent2D(width, height));
  EXPECT_EQ(rawBitmap.GetOrigin(), expectedBitmapOrigin);
  EXPECT_EQ(rawBitmap.GetPixelFormat(), pixelFormat);
  EXPECT_EQ(rawBitmap.GetPixelFormatLayout(), pixelLayout);

  // Then validate against the bitmap content
  EXPECT_EQ(rawBitmap.IsValid(), bitmap.IsValid());
  EXPECT_NE(rawBitmap.Content(), nullptr);
  EXPECT_EQ(rawBitmap.Width(), bitmap.Width());
  EXPECT_EQ(rawBitmap.Height(), bitmap.Height());
  EXPECT_EQ(rawBitmap.Stride(), minStride);
  EXPECT_EQ(rawBitmap.GetByteSize(), bufferLength);
  EXPECT_EQ(rawBitmap.GetExtent(), bitmap.GetExtent());
  EXPECT_EQ(rawBitmap.GetOrigin(), bitmap.GetOrigin());
  EXPECT_EQ(rawBitmap.GetPixelFormat(), bitmap.GetPixelFormat());
  EXPECT_EQ(rawBitmap.GetPixelFormatLayout(), bitmap.GetPixelFormatLayout());

  //! Validate the image just to be sure
  ASSERT_TRUE(template_t::CheckIsBasic4X2(bitmap));
}


TYPED_TEST(TestFixtureFslGraphics_Bitmap, CheckScopedDirectReadWriteAccess)
{
  using current_t = typename TestFixture::current_t;
  using imageConfig_t = typename current_t::imageConfig_t;
  using template_t = ImageTestTemplate<imageConfig_t>;

  const auto pixelFormat = imageConfig_t::ActivePixelFormat;
  const auto pixelLayout = imageConfig_t::ActivePixelLayout;
  const uint32_t width = 4;
  const uint32_t height = 2;
  const auto bitmapOrigin = current_t::Origin;
  const auto minStride = PixelFormatUtil::CalcMinimumStride(width, pixelFormat);
  const uint32_t bufferLength = height * minStride;

  Bitmap bitmap = template_t::GetBasic4X2(bitmapOrigin);
  RawBitmapEx rawBitmap;
  Bitmap::ScopedDirectAccess directAccess(bitmap, rawBitmap);

  // The bitmap defaults to UpperLeft if the origin is undefined.
  const auto expectedBitmapOrigin = bitmapOrigin != BitmapOrigin::Undefined ? bitmapOrigin : BitmapOrigin::UpperLeft;

  // Known value checks just ot be sure
  EXPECT_TRUE(rawBitmap.IsValid());
  EXPECT_EQ(rawBitmap.Width(), width);
  EXPECT_EQ(rawBitmap.Height(), height);
  EXPECT_EQ(rawBitmap.Stride(), minStride);
  EXPECT_EQ(rawBitmap.GetByteSize(), bufferLength);
  EXPECT_EQ(rawBitmap.GetExtent(), Extent2D(width, height));
  EXPECT_EQ(rawBitmap.GetOrigin(), expectedBitmapOrigin);
  EXPECT_EQ(rawBitmap.GetPixelFormat(), pixelFormat);
  EXPECT_EQ(rawBitmap.GetPixelFormatLayout(), pixelLayout);

  // Then validate against the bitmap content
  EXPECT_EQ(rawBitmap.IsValid(), bitmap.IsValid());
  EXPECT_NE(rawBitmap.Content(), nullptr);
  EXPECT_EQ(rawBitmap.Width(), bitmap.Width());
  EXPECT_EQ(rawBitmap.Height(), bitmap.Height());
  EXPECT_EQ(rawBitmap.Stride(), minStride);
  EXPECT_EQ(rawBitmap.GetByteSize(), bufferLength);
  EXPECT_EQ(rawBitmap.GetExtent(), bitmap.GetExtent());
  EXPECT_EQ(rawBitmap.GetOrigin(), bitmap.GetOrigin());
  EXPECT_EQ(rawBitmap.GetPixelFormat(), bitmap.GetPixelFormat());
  EXPECT_EQ(rawBitmap.GetPixelFormatLayout(), bitmap.GetPixelFormatLayout());

  //! Validate the image just to be sure
  ASSERT_TRUE(template_t::CheckIsBasic4X2(bitmap));
}
