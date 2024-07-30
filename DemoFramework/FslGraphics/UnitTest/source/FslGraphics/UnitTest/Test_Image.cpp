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

#include <FslGraphics/Bitmap/BitmapOrigin.hpp>
#include <FslGraphics/UnitTest/Helper/Bitmap/BitmapImageConfig.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/ImageTestTemplate.hpp>
#include <FslGraphics/UnitTest/Helper/Log/LogPixelUInt8.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <FslGraphics/UnitTest/Helper/Texture/TextureImageConfig.hpp>

using namespace Fsl;

namespace
{
  template <typename TCurrent>
  // NOLINTNEXTLINE(readability-identifier-naming)
  class Test_Image : public TestFixtureFslGraphics
  {
  public:
    using current_t = TCurrent;
  };

  using TestType1 = ImageTestTemplate<BitmapImageConfig::R8G8B8>;
  using TestType2 = ImageTestTemplate<BitmapImageConfig::R8G8B8A8>;
  using TestType3 = ImageTestTemplate<TextureImageConfig::R8G8B8>;
  using TestType4 = ImageTestTemplate<TextureImageConfig::R8G8B8A8>;


  using TestTypes = ::testing::Types<TestType1, TestType2, TestType3, TestType4>;

  TYPED_TEST_SUITE(Test_Image, TestTypes);
}


TYPED_TEST(Test_Image, Create4X1)
{
  using current_t = typename TestFixture::current_t;

  auto image = current_t::GetBasic4X1();
  ASSERT_TRUE(current_t::CheckIsBasic4X1(image));
}


TYPED_TEST(Test_Image, Create4X1_OriginUpperLeft)
{
  using current_t = typename TestFixture::current_t;

  auto image = current_t::GetBasic4X1(BitmapOrigin::UpperLeft);
  ASSERT_TRUE(current_t::CheckIsBasic4X1(image));
}


TYPED_TEST(Test_Image, Create4X1_OriginLowerLeft)
{
  using current_t = typename TestFixture::current_t;

  auto image = current_t::GetBasic4X1(BitmapOrigin::LowerLeft);
  ASSERT_TRUE(current_t::CheckIsBasic4X1(image));
}


TYPED_TEST(Test_Image, Create4X2)
{
  using current_t = typename TestFixture::current_t;

  auto image = current_t::GetBasic4X2();
  ASSERT_TRUE(current_t::CheckIsBasic4X2(image));
}


TYPED_TEST(Test_Image, Create4X2_OriginUpperLeft)
{
  using current_t = typename TestFixture::current_t;

  auto image = current_t::GetBasic4X2(BitmapOrigin::UpperLeft);
  ASSERT_TRUE(current_t::CheckIsBasic4X2(image));
}


TYPED_TEST(Test_Image, Create4X2_OriginLowerLeft)
{
  using current_t = typename TestFixture::current_t;

  auto image = current_t::GetBasic4X2(BitmapOrigin::LowerLeft);
  ASSERT_TRUE(current_t::CheckIsBasic4X2(image));
}


TYPED_TEST(Test_Image, Create4X2_OriginLowerLeft_CheckOrigin)
{
  using current_t = typename TestFixture::current_t;

  auto image = current_t::GetBasic4X2(BitmapOrigin::LowerLeft);
  ASSERT_TRUE(current_t::CheckIsBasic4X2(image, BitmapOrigin::LowerLeft));
}


TYPED_TEST(Test_Image, CheckIsBasic4X1_TestCheckInvalidSize)
{
  using current_t = typename TestFixture::current_t;

  auto image = current_t::GetBasic4X2();
  ASSERT_FALSE(current_t::CheckIsBasic4X1(image));
}


TYPED_TEST(Test_Image, CheckIsBasic4X2_TestCheckInvalidSize)
{
  using current_t = typename TestFixture::current_t;

  auto image = current_t::GetBasic4X1();
  ASSERT_FALSE(current_t::CheckIsBasic4X2(image));
}
