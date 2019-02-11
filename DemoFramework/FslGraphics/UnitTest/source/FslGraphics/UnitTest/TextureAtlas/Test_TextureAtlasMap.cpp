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

#include <FslGraphics/TextureAtlas/TextureAtlasMap.hpp>
#include <FslBase/Log/Math/LogRectangle.hpp>
#include <FslBase/Log/Math/LogPoint2.hpp>
#include <FslBase/Log/Math/LogThickness.hpp>
#include <FslBase/Log/String/LogUTF8String.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  using TestTextureAtlas_TextureAtlasMap = TestFixtureFslGraphics;
}


TEST(TestTextureAtlas_TextureAtlasMap, Construct_Default)
{
  TextureAtlasMap map;
  EXPECT_THROW(map.GetAtlasTextureInfo("hello"), NotFoundException);
}


TEST(TestTextureAtlas_TextureAtlasMap, Construct)
{
  BasicTextureAtlas atlas;
  atlas.Reset(1);
  atlas.SetEntry(0, Rectangle(1, 2, 20, 30), Rectangle(4, 6, 8, 12), UTF8String("hello"));

  EXPECT_NO_THROW(TextureAtlasMap(atlas));
}


TEST(TestTextureAtlas_TextureAtlasMap, GetAtlasTextureInfo)
{
  BasicTextureAtlas atlas;
  atlas.Reset(1);
  atlas.SetEntry(0, Rectangle(1, 2, 20, 30), Rectangle(4, 6, 8, 12), UTF8String("hello"));

  TextureAtlasMap map(atlas);
  auto textureInfo = map.GetAtlasTextureInfo("hello");
  EXPECT_EQ(Point2(1, 2), textureInfo.Offset);
  EXPECT_EQ(Point2(20, 30), textureInfo.OriginalSize);
  EXPECT_EQ(Thickness(3, 4, 9, 14), textureInfo.TrimMargin);
  EXPECT_EQ(Rectangle(4, 6, 8, 12), textureInfo.TrimmedRect);
}


TEST(TestTextureAtlas_TextureAtlasMap, NotFound)
{
  BasicTextureAtlas atlas;
  atlas.Reset(1);
  atlas.SetEntry(0, Rectangle(1, 2, 20, 30), Rectangle(4, 6, 8, 12), UTF8String("hello"));

  TextureAtlasMap map(atlas);
  EXPECT_THROW(map.GetAtlasTextureInfo("/hello"), NotFoundException);
  EXPECT_THROW(map.GetAtlasTextureInfo(""), NotFoundException);
  EXPECT_THROW(map.GetAtlasTextureInfo("hello/"), NotFoundException);
  EXPECT_THROW(map.GetAtlasTextureInfo("Hello"), NotFoundException);
}
