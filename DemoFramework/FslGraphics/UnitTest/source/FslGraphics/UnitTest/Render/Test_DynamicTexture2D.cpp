/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslBase/Log/Math/Pixel/LogPxExtent2D.hpp>
#include <FslBase/Log/Math/Pixel/LogPxExtent3D.hpp>
#include <FslBase/Log/Math/Pixel/LogPxPoint2.hpp>
#include <FslBase/Log/Math/Pixel/LogPxSize2D.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/Log/LogPixelFormat.hpp>
#include <FslGraphics/Render/DynamicTexture2D.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/Render/NativeGraphicsTestImpl.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  using TestRender_DynamicTexture2D = TestFixtureFslGraphics;
}


TEST(TestRender_DynamicTexture2D, Construct_Empty)
{
  DynamicTexture2D texture;

  EXPECT_FALSE(texture.IsValid());
  EXPECT_EQ(PxExtent2D(), texture.GetExtent());
  EXPECT_EQ(PxSize2D(), texture.GetSize());
  EXPECT_EQ(PixelFormat::Undefined, texture.GetPixelFormat());
  EXPECT_EQ(std::shared_ptr<INativeTexture2D>(), texture.TryGetNative());
  EXPECT_EQ(std::shared_ptr<IDynamicNativeTexture2D>(), texture.TryGetDynamicNativeTexture());
  EXPECT_THROW(texture.GetNative(), GraphicsException);
}


TEST(TestRender_DynamicTexture2D, Construct_Bitmap)
{
  const auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();
  const Bitmap bitmap(PxExtent2D(128, 128), PixelFormat::R8G8B8A8_UNORM);
  DynamicTexture2D texture(nativeGraphics, bitmap, Texture2DFilterHint::Smooth);

  EXPECT_TRUE(texture.IsValid());
  EXPECT_EQ(bitmap.GetExtent(), texture.GetExtent());
  EXPECT_EQ(PxSize2D(bitmap.Width(), bitmap.Height()), texture.GetSize());
  EXPECT_EQ(bitmap.GetPixelFormat(), texture.GetPixelFormat());
  EXPECT_NE(std::shared_ptr<INativeTexture2D>(), texture.TryGetNative());
  EXPECT_NE(std::shared_ptr<IDynamicNativeTexture2D>(), texture.TryGetDynamicNativeTexture());
  EXPECT_EQ(texture.TryGetNative(), texture.GetNative());
  EXPECT_EQ(texture.TryGetDynamicNativeTexture(), texture.GetDynamicNativeTexture());
}

TEST(TestRender_DynamicTexture2D, Construct_RawBitmap)
{
  const auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();
  const Bitmap bitmap(PxExtent2D(128, 128), PixelFormat::R8G8B8A8_UNORM);
  RawBitmap rawBitmap;
  Bitmap::ScopedDirectAccess directAccess(bitmap, rawBitmap);
  DynamicTexture2D texture(nativeGraphics, rawBitmap, Texture2DFilterHint::Smooth);

  EXPECT_TRUE(texture.IsValid());
  EXPECT_EQ(bitmap.GetExtent(), texture.GetExtent());
  EXPECT_EQ(PxSize2D(bitmap.Width(), bitmap.Height()), texture.GetSize());
  EXPECT_EQ(bitmap.GetPixelFormat(), texture.GetPixelFormat());
  EXPECT_NE(std::shared_ptr<INativeTexture2D>(), texture.TryGetNative());
  EXPECT_NE(std::shared_ptr<IDynamicNativeTexture2D>(), texture.TryGetDynamicNativeTexture());
  EXPECT_EQ(texture.TryGetNative(), texture.GetNative());
  EXPECT_EQ(texture.TryGetDynamicNativeTexture(), texture.GetDynamicNativeTexture());
}


TEST(TestRender_DynamicTexture2D, Construct_Texture)
{
  const auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();
  const Texture srcTexture(PxExtent2D(128, 128), PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::UpperLeft);
  DynamicTexture2D texture(nativeGraphics, srcTexture, Texture2DFilterHint::Smooth);

  auto extent = srcTexture.GetExtent();

  EXPECT_TRUE(texture.IsValid());
  EXPECT_EQ(PxExtent2D(extent.Width, extent.Height), texture.GetExtent());
  EXPECT_EQ(PxSize2D(static_cast<int32_t>(srcTexture.GetExtent().Width), static_cast<int32_t>(srcTexture.GetExtent().Height)), texture.GetSize());
  EXPECT_EQ(srcTexture.GetPixelFormat(), texture.GetPixelFormat());
  EXPECT_NE(std::shared_ptr<INativeTexture2D>(), texture.TryGetNative());
  EXPECT_NE(std::shared_ptr<IDynamicNativeTexture2D>(), texture.TryGetDynamicNativeTexture());
  EXPECT_EQ(texture.TryGetNative(), texture.GetNative());
  EXPECT_EQ(texture.TryGetDynamicNativeTexture(), texture.GetDynamicNativeTexture());
}


TEST(TestRender_DynamicTexture2D, Construct_RawTexture)
{
  const auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();
  const Texture srcTexture(PxExtent2D(128, 128), PixelFormat::R8G8B8A8_UNORM, BitmapOrigin::UpperLeft);
  RawTexture rawTexture;
  Texture::ScopedDirectAccess directAccess(srcTexture, rawTexture);
  DynamicTexture2D texture(nativeGraphics, rawTexture, Texture2DFilterHint::Smooth);

  auto extent = srcTexture.GetExtent();

  EXPECT_TRUE(texture.IsValid());
  EXPECT_EQ(PxExtent2D(extent.Width, extent.Height), texture.GetExtent());
  EXPECT_EQ(PxSize2D(static_cast<int32_t>(srcTexture.GetExtent().Width), static_cast<int32_t>(srcTexture.GetExtent().Height)), texture.GetSize());
  EXPECT_EQ(srcTexture.GetPixelFormat(), texture.GetPixelFormat());
  EXPECT_NE(std::shared_ptr<INativeTexture2D>(), texture.TryGetNative());
  EXPECT_NE(std::shared_ptr<IDynamicNativeTexture2D>(), texture.TryGetDynamicNativeTexture());
  EXPECT_EQ(texture.TryGetNative(), texture.GetNative());
  EXPECT_EQ(texture.TryGetDynamicNativeTexture(), texture.GetDynamicNativeTexture());
}


TEST(TestRender_DynamicTexture2D, Reset)
{
  const auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();
  const Bitmap bitmap(PxExtent2D(128, 128), PixelFormat::R8G8B8A8_UNORM);
  DynamicTexture2D texture(nativeGraphics, bitmap, Texture2DFilterHint::Smooth);

  EXPECT_TRUE(texture.IsValid());

  texture.Reset();

  EXPECT_FALSE(texture.IsValid());
  EXPECT_EQ(PxExtent2D(), texture.GetExtent());
  EXPECT_EQ(PxSize2D(), texture.GetSize());
  EXPECT_EQ(PixelFormat::Undefined, texture.GetPixelFormat());
  EXPECT_EQ(std::shared_ptr<INativeTexture2D>(), texture.TryGetNative());
  EXPECT_THROW(texture.GetNative(), GraphicsException);
}


TEST(TestRender_DynamicTexture2D, Reset_EmptyWithBitmap)
{
  auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();
  DynamicTexture2D texture;
  ASSERT_FALSE(texture.IsValid());

  const Bitmap bitmap2(PxExtent2D(64, 64), PixelFormat::R8G8B8_UNORM);
  texture.Reset(nativeGraphics, bitmap2, Texture2DFilterHint::Nearest);

  EXPECT_TRUE(texture.IsValid());
  EXPECT_EQ(bitmap2.GetExtent(), texture.GetExtent());
  EXPECT_EQ(PxSize2D(bitmap2.Width(), bitmap2.Height()), texture.GetSize());
  EXPECT_EQ(bitmap2.GetPixelFormat(), texture.GetPixelFormat());
  EXPECT_NE(std::shared_ptr<INativeTexture2D>(), texture.TryGetNative());
  EXPECT_NE(std::shared_ptr<IDynamicNativeTexture2D>(), texture.TryGetDynamicNativeTexture());
  EXPECT_EQ(texture.TryGetNative(), texture.GetNative());
  EXPECT_EQ(texture.TryGetDynamicNativeTexture(), texture.GetDynamicNativeTexture());
}


TEST(TestRender_DynamicTexture2D, Reset_NotEmptyWithBitmap)
{
  auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();
  const Bitmap bitmap1(PxExtent2D(128, 128), PixelFormat::R8G8B8A8_UNORM);
  DynamicTexture2D texture(nativeGraphics, bitmap1, Texture2DFilterHint::Smooth);
  ASSERT_TRUE(texture.IsValid());

  const Bitmap bitmap2(PxExtent2D(64, 64), PixelFormat::R8G8B8_UNORM);
  texture.Reset(nativeGraphics, bitmap2, Texture2DFilterHint::Nearest);

  EXPECT_TRUE(texture.IsValid());
  EXPECT_EQ(bitmap2.GetExtent(), texture.GetExtent());
  EXPECT_EQ(PxSize2D(bitmap2.Width(), bitmap2.Height()), texture.GetSize());
  EXPECT_EQ(bitmap2.GetPixelFormat(), texture.GetPixelFormat());
  EXPECT_NE(std::shared_ptr<INativeTexture2D>(), texture.TryGetNative());
  EXPECT_NE(std::shared_ptr<IDynamicNativeTexture2D>(), texture.TryGetDynamicNativeTexture());
  EXPECT_EQ(texture.TryGetNative(), texture.GetNative());
  EXPECT_EQ(texture.TryGetDynamicNativeTexture(), texture.GetDynamicNativeTexture());
}


TEST(TestRender_DynamicTexture2D, Reset_EmptyWithTexture)
{
  auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();
  DynamicTexture2D texture;
  ASSERT_FALSE(texture.IsValid());

  const Texture srcTexture(PxExtent2D(64, 64), PixelFormat::R8G8B8_UNORM, BitmapOrigin::UpperLeft);
  texture.Reset(nativeGraphics, srcTexture, Texture2DFilterHint::Nearest);

  auto extent = srcTexture.GetExtent();

  EXPECT_TRUE(texture.IsValid());
  EXPECT_EQ(PxExtent2D(extent.Width, extent.Height), texture.GetExtent());
  EXPECT_EQ(PxSize2D(static_cast<int32_t>(srcTexture.GetExtent().Width), static_cast<int32_t>(srcTexture.GetExtent().Height)), texture.GetSize());
  EXPECT_EQ(srcTexture.GetPixelFormat(), texture.GetPixelFormat());
  EXPECT_NE(std::shared_ptr<INativeTexture2D>(), texture.TryGetNative());
  EXPECT_NE(std::shared_ptr<IDynamicNativeTexture2D>(), texture.TryGetDynamicNativeTexture());
  EXPECT_EQ(texture.TryGetNative(), texture.GetNative());
  EXPECT_EQ(texture.TryGetDynamicNativeTexture(), texture.GetDynamicNativeTexture());
}


TEST(TestRender_DynamicTexture2D, Reset_NotEmptyWithTexture)
{
  auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();
  const Bitmap bitmap1(PxExtent2D(128, 128), PixelFormat::R8G8B8A8_UNORM);
  DynamicTexture2D texture(nativeGraphics, bitmap1, Texture2DFilterHint::Smooth);
  ASSERT_TRUE(texture.IsValid());

  const Texture srcTexture(PxExtent2D(64, 64), PixelFormat::R8G8B8_UNORM, BitmapOrigin::UpperLeft);
  texture.Reset(nativeGraphics, srcTexture, Texture2DFilterHint::Nearest);

  auto extent = srcTexture.GetExtent();

  EXPECT_TRUE(texture.IsValid());
  EXPECT_EQ(PxExtent2D(extent.Width, extent.Height), texture.GetExtent());
  EXPECT_EQ(PxSize2D(static_cast<int32_t>(srcTexture.GetExtent().Width), static_cast<int32_t>(srcTexture.GetExtent().Height)), texture.GetSize());
  EXPECT_EQ(srcTexture.GetPixelFormat(), texture.GetPixelFormat());
  EXPECT_NE(std::shared_ptr<INativeTexture2D>(), texture.TryGetNative());
  EXPECT_NE(std::shared_ptr<IDynamicNativeTexture2D>(), texture.TryGetDynamicNativeTexture());
  EXPECT_EQ(texture.TryGetNative(), texture.GetNative());
  EXPECT_EQ(texture.TryGetDynamicNativeTexture(), texture.GetDynamicNativeTexture());
}


TEST(TestRender_DynamicTexture2D, SetData_EmptyWithBitmap)
{
  auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();
  DynamicTexture2D texture;
  ASSERT_FALSE(texture.IsValid());

  const Bitmap bitmap2(texture.GetExtent(), texture.GetPixelFormat());
  // EXPECT_THROW(texture.SetData(bitmap2, Texture2DFilterHint::Nearest), UsageErrorException);
  EXPECT_THROW(texture.SetData(bitmap2, Texture2DFilterHint::Nearest), std::invalid_argument);
  // texture.SetData(bitmap2, Texture2DFilterHint::Nearest);
}


TEST(TestRender_DynamicTexture2D, SetData_EmptyWithBitmap_MoreRealistic)
{
  auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();
  DynamicTexture2D texture;
  ASSERT_FALSE(texture.IsValid());

  const Bitmap bitmap2(PxExtent2D(64, 64), PixelFormat::R8G8B8_UNORM);
  EXPECT_THROW(texture.SetData(bitmap2, Texture2DFilterHint::Nearest), UsageErrorException);
}


TEST(TestRender_DynamicTexture2D, SetData_NotEmptyWithBitmap_IncorrectSize)
{
  auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();
  const Bitmap bitmap1(PxExtent2D(128, 128), PixelFormat::R8G8B8A8_UNORM);
  DynamicTexture2D texture(nativeGraphics, bitmap1, Texture2DFilterHint::Smooth);
  ASSERT_TRUE(texture.IsValid());

  const Bitmap bitmap2(PxExtent2D(64, 64), bitmap1.GetPixelFormat());
  EXPECT_THROW(texture.SetData(bitmap2, Texture2DFilterHint::Nearest), UsageErrorException);
}


TEST(TestRender_DynamicTexture2D, SetData_NotEmptyWithBitmap_IncorrectPixelFormat)
{
  auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();
  const Bitmap bitmap1(PxExtent2D(128, 128), PixelFormat::R8G8B8A8_UNORM);
  DynamicTexture2D texture(nativeGraphics, bitmap1, Texture2DFilterHint::Smooth);
  ASSERT_TRUE(texture.IsValid());

  const Bitmap bitmap2(bitmap1.GetExtent(), PixelFormat::R8G8B8_UNORM);
  EXPECT_THROW(texture.SetData(bitmap2, Texture2DFilterHint::Nearest), UsageErrorException);
}


TEST(TestRender_DynamicTexture2D, SetData_NotEmptyWithBitmap)
{
  auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();
  const Bitmap bitmap1(PxExtent2D(128, 128), PixelFormat::R8G8B8A8_UNORM);
  DynamicTexture2D texture(nativeGraphics, bitmap1, Texture2DFilterHint::Smooth);
  ASSERT_TRUE(texture.IsValid());

  const Bitmap bitmap2(bitmap1.GetExtent(), bitmap1.GetPixelFormat());
  texture.SetData(bitmap2, Texture2DFilterHint::Nearest);

  EXPECT_TRUE(texture.IsValid());
  EXPECT_EQ(bitmap2.GetExtent(), texture.GetExtent());
  EXPECT_EQ(PxSize2D(bitmap2.Width(), bitmap2.Height()), texture.GetSize());
  EXPECT_EQ(bitmap2.GetPixelFormat(), texture.GetPixelFormat());
  EXPECT_NE(std::shared_ptr<INativeTexture2D>(), texture.TryGetNative());
  EXPECT_NE(std::shared_ptr<IDynamicNativeTexture2D>(), texture.TryGetDynamicNativeTexture());
  EXPECT_EQ(texture.TryGetNative(), texture.GetNative());
  EXPECT_EQ(texture.TryGetDynamicNativeTexture(), texture.GetDynamicNativeTexture());
}


TEST(TestRender_DynamicTexture2D, SetData_EmptyWithTexture)
{
  auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();
  DynamicTexture2D texture;
  ASSERT_FALSE(texture.IsValid());

  const Texture srcTexture(PxExtent2D(0, 0), texture.GetPixelFormat(), BitmapOrigin::UpperLeft);

  // Since no native graphics has been set on a empty object this should throw
  // - however this attempt to trigger it will be prevented by RawTexture not allowing a undefined pixel format
  EXPECT_THROW(texture.SetData(srcTexture, Texture2DFilterHint::Nearest), std::invalid_argument);
}


TEST(TestRender_DynamicTexture2D, SetData_EmptyWithTexture_MoreRealistic)
{
  auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();
  DynamicTexture2D texture;
  ASSERT_FALSE(texture.IsValid());

  const Texture srcTexture(PxExtent2D(64, 64), PixelFormat::R8G8B8_UNORM, BitmapOrigin::UpperLeft);

  // Since no native graphics has been set on a empty object this should throw
  EXPECT_THROW(texture.SetData(srcTexture, Texture2DFilterHint::Nearest), UsageErrorException);
}


TEST(TestRender_DynamicTexture2D, SetData_NotEmptyWithTexture_IncorrectSize)
{
  auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();
  const Bitmap bitmap1(PxExtent2D(128, 128), PixelFormat::R8G8B8A8_UNORM);
  DynamicTexture2D texture(nativeGraphics, bitmap1, Texture2DFilterHint::Smooth);
  ASSERT_TRUE(texture.IsValid());

  const Texture srcTexture(PxExtent2D(64, 64), bitmap1.GetPixelFormat(), bitmap1.GetOrigin());
  EXPECT_THROW(texture.SetData(srcTexture, Texture2DFilterHint::Nearest), UsageErrorException);
}


TEST(TestRender_DynamicTexture2D, SetData_NotEmptyWithTexture_IncorrectPixelFormat)
{
  auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();
  const Bitmap bitmap1(PxExtent2D(128, 128), PixelFormat::R8G8B8A8_UNORM);
  DynamicTexture2D texture(nativeGraphics, bitmap1, Texture2DFilterHint::Smooth);
  ASSERT_TRUE(texture.IsValid());

  const Texture srcTexture(bitmap1.GetExtent(), PixelFormat::R8G8B8_UNORM, bitmap1.GetOrigin());

  EXPECT_THROW(texture.SetData(srcTexture, Texture2DFilterHint::Nearest), UsageErrorException);
}


TEST(TestRender_DynamicTexture2D, SetData_NotEmptyWithTexture)
{
  auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();
  const Bitmap bitmap1(PxExtent2D(128, 128), PixelFormat::R8G8B8A8_UNORM);
  DynamicTexture2D texture(nativeGraphics, bitmap1, Texture2DFilterHint::Smooth);
  ASSERT_TRUE(texture.IsValid());

  const Texture srcTexture(bitmap1.GetExtent(), bitmap1.GetPixelFormat(), bitmap1.GetOrigin());
  texture.SetData(srcTexture, Texture2DFilterHint::Nearest);

  auto extent = srcTexture.GetExtent();

  EXPECT_TRUE(texture.IsValid());
  EXPECT_EQ(PxExtent2D(extent.Width, extent.Height), texture.GetExtent());
  EXPECT_EQ(PxSize2D(static_cast<int32_t>(srcTexture.GetExtent().Width), static_cast<int32_t>(srcTexture.GetExtent().Height)), texture.GetSize());
  EXPECT_EQ(srcTexture.GetPixelFormat(), texture.GetPixelFormat());
  EXPECT_NE(std::shared_ptr<INativeTexture2D>(), texture.TryGetNative());
  EXPECT_NE(std::shared_ptr<IDynamicNativeTexture2D>(), texture.TryGetDynamicNativeTexture());
  EXPECT_EQ(texture.TryGetNative(), texture.GetNative());
  EXPECT_EQ(texture.TryGetDynamicNativeTexture(), texture.GetDynamicNativeTexture());
}
