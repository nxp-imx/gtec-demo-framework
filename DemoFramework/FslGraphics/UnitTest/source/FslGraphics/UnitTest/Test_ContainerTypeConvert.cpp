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
#include <FslGraphics/ContainerTypeConvert.hpp>
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
  class TestFixtureFslGraphics_ContainerTypeConvert : public TestFixtureFslGraphics
  {
  public:
    using current_t = TCurrent;
  };

  template <typename TFrom, typename TTo>
  struct TypePair
  {
    using from_t = TFrom;
    using to_t = TTo;
  };


  using TestType1 = TypePair<ImageTestTemplate<BitmapImageConfig::R8G8B8>, ImageTestTemplate<TextureImageConfig::R8G8B8>>;
  using TestType2 = TypePair<ImageTestTemplate<BitmapImageConfig::R8G8B8A8>, ImageTestTemplate<TextureImageConfig::R8G8B8A8>>;
  using TestType3 = TypePair<ImageTestTemplate<TextureImageConfig::R8G8B8>, ImageTestTemplate<BitmapImageConfig::R8G8B8>>;
  using TestType4 = TypePair<ImageTestTemplate<TextureImageConfig::R8G8B8A8>, ImageTestTemplate<BitmapImageConfig::R8G8B8A8>>;

  // Bitmap -> Texture

  using TestTypes = ::testing::Types<TestType1, TestType2, TestType3, TestType4>;

  TYPED_TEST_SUITE(TestFixtureFslGraphics_ContainerTypeConvert, TestTypes);
}


TYPED_TEST(TestFixtureFslGraphics_ContainerTypeConvert, ConvertBasic4X1)
{
  using current_t = typename TestFixture::current_t;

  auto imageFrom = current_t::from_t::GetBasic4X1();
  auto imageTo = ContainerTypeConvert::Convert(std::move(imageFrom));
  ASSERT_TRUE(current_t::to_t::CheckIsBasic4X1(imageTo));
}


TYPED_TEST(TestFixtureFslGraphics_ContainerTypeConvert, ConvertBasic4X2)
{
  using current_t = typename TestFixture::current_t;

  auto imageFrom = current_t::from_t::GetBasic4X2();
  auto imageTo = ContainerTypeConvert::Convert(std::move(imageFrom));
  ASSERT_TRUE(current_t::to_t::CheckIsBasic4X2(imageTo));
}


TYPED_TEST(TestFixtureFslGraphics_ContainerTypeConvert, ConvertEmpty)
{
  using current_t = typename TestFixture::current_t;
  using fromImage_t = typename current_t::from_t::image_t;

  auto imageFrom = fromImage_t();
  auto imageTo = ContainerTypeConvert::Convert(std::move(imageFrom));
}
