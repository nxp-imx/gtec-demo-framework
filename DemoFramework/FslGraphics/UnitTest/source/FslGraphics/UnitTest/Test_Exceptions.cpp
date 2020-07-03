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

#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using Test_Exceptions = TestFixtureFslGraphics;
}


TEST(Test_Exceptions, UnsupportedStrideException_Construct1)
{
  const std::string message("hello");
  int32_t stride = 42;
  UnsupportedStrideException ex(message, stride);

  EXPECT_EQ(message, ex.what());
  EXPECT_EQ(stride, ex.GetStride());
}


TEST(Test_Exceptions, UnsupportedStrideException_Construct2)
{
  int32_t stride = 42;
  UnsupportedStrideException ex(stride);

  EXPECT_EQ(stride, ex.GetStride());
}


TEST(Test_Exceptions, UnsupportedStrideExceptionEx_Construct1)
{
  const std::string message("hello");
  std::size_t stride = 42;
  UnsupportedStrideExceptionEx ex(message, stride);

  EXPECT_EQ(message, ex.what());
  EXPECT_EQ(stride, ex.GetStride());
}


TEST(Test_Exceptions, UnsupportedStrideExceptionEx_Construct2)
{
  std::size_t stride = 42;
  UnsupportedStrideExceptionEx ex(stride);

  EXPECT_EQ(stride, ex.GetStride());
}


TEST(Test_Exceptions, UnsupportedAlignmentException_Construct1)
{
  const std::string message("hello");
  int alignment = 42;
  UnsupportedAlignmentException ex(message, alignment);

  EXPECT_EQ(message, ex.what());
  EXPECT_EQ(alignment, ex.GetAlignment());
}


TEST(Test_Exceptions, UnsupportedAlignmentException_Construct2)
{
  int alignment = 42;
  UnsupportedAlignmentException ex(alignment);

  EXPECT_EQ(alignment, ex.GetAlignment());
}


TEST(Test_Exceptions, UnsupportedPixelFormatException_Construct1)
{
  const std::string message("hello");
  PixelFormat pixelFormat = PixelFormat::A1R5G5B5_UNORM_PACK16;
  UnsupportedPixelFormatException ex(message, pixelFormat);

  EXPECT_EQ(message, ex.what());
  EXPECT_EQ(pixelFormat, ex.GetPixelFormat());
}


TEST(Test_Exceptions, UnsupportedPixelFormatException_Construct2)
{
  PixelFormat pixelFormat = PixelFormat::A1R5G5B5_UNORM_PACK16;
  UnsupportedPixelFormatException ex(pixelFormat);

  EXPECT_EQ(pixelFormat, ex.GetPixelFormat());
}


TEST(Test_Exceptions, UnsupportedVertexElementFormatException_Construct1)
{
  const std::string message("hello");
  auto value = VertexElementFormat::Vector2;
  UnsupportedVertexElementFormatException ex(message, value);

  EXPECT_EQ(message, ex.what());
  EXPECT_EQ(value, ex.GetVertexElementFormat());
}


TEST(Test_Exceptions, UnsupportedVertexElementFormatException_Construct2)
{
  auto value = VertexElementFormat::Vector2;
  UnsupportedVertexElementFormatException ex(value);

  EXPECT_EQ(value, ex.GetVertexElementFormat());
}


TEST(Test_Exceptions, UnsupportedPixelFormatConversionException_Construct1)
{
  const std::string message("hello");
  auto value1 = PixelFormat::A8B8G8R8_SINT_PACK32;
  auto value2 = PixelFormat::BC1_RGB_SRGB_BLOCK;
  UnsupportedPixelFormatConversionException ex(message, value1, value2);

  EXPECT_EQ(message, ex.what());
  EXPECT_EQ(value1, ex.GetFromPixelFormat());
  EXPECT_EQ(value2, ex.GetToPixelFormat());
}


TEST(Test_Exceptions, UnsupportedPixelFormatConversionException_Construct2)
{
  auto value1 = PixelFormat::A8B8G8R8_SINT_PACK32;
  auto value2 = PixelFormat::BC1_RGB_SRGB_BLOCK;
  UnsupportedPixelFormatConversionException ex(value1, value2);

  EXPECT_EQ(value1, ex.GetFromPixelFormat());
  EXPECT_EQ(value2, ex.GetToPixelFormat());
}
