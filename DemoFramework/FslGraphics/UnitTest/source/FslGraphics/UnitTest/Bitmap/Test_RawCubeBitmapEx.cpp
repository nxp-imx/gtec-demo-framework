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

#include <FslBase/Exceptions.hpp>
#include <FslGraphics/Bitmap/RawCubeBitmapEx.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  using TestBitmap_RawCubeBitmapEx = TestFixtureFslGraphics;
}


TEST(TestBitmap_RawCubeBitmapEx, Construct_Default)
{
  RawCubeBitmapEx bitmap;

  ASSERT_FALSE(bitmap.IsValid());
  EXPECT_EQ(0u, bitmap.Width());
  EXPECT_EQ(0u, bitmap.Height());
  EXPECT_EQ(PxExtent2D(), bitmap.GetExtent());
  EXPECT_EQ(PixelFormat::Undefined, bitmap.GetPixelFormat());
  EXPECT_EQ(BitmapOrigin::UpperLeft, bitmap.GetOrigin());
  EXPECT_EQ(ReadOnlyRawBitmap(), bitmap.GetPosX());
  EXPECT_EQ(ReadOnlyRawBitmap(), bitmap.GetNegX());
  EXPECT_EQ(ReadOnlyRawBitmap(), bitmap.GetPosY());
  EXPECT_EQ(ReadOnlyRawBitmap(), bitmap.GetNegY());
  EXPECT_EQ(ReadOnlyRawBitmap(), bitmap.GetPosZ());
  EXPECT_EQ(ReadOnlyRawBitmap(), bitmap.GetNegZ());
  EXPECT_EQ(0u, bitmap.GetByteSize());
}
