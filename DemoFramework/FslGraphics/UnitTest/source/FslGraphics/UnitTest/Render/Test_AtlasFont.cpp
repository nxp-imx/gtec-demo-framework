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

#include <FslGraphics/Render/AtlasFont.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Math/LogPoint2.hpp>
#include <FslBase/Log/Math/Pixel/LogPxExtent2D.hpp>
#include <FslBase/Log/Math/Pixel/LogPxExtent3D.hpp>
#include <FslBase/Log/Math/Pixel/LogPxSize2D.hpp>
#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <FslGraphics/UnitTest/Helper/Render/NativeGraphicsTestImpl.hpp>

using namespace Fsl;

namespace
{
  using TestRender_AtlasFont = TestFixtureFslGraphics;
}


TEST(TestRender_AtlasFont, Construct_Invalid)
{
  EXPECT_THROW(AtlasFont(BaseTexture2D(), TextureAtlasSpriteFont()), std::invalid_argument);
}


TEST(TestRender_AtlasFont, Construct_EmptyBitmapFont)
{
  auto nativeGraphics = std::make_shared<NativeGraphicsTestImpl>();
  const Bitmap atlasBitmap(PxExtent2D(128, 128), PixelFormat::R8G8B8A8_UNORM);
  const Texture2D atlasTex(nativeGraphics, atlasBitmap, Texture2DFilterHint::Smooth);

  AtlasFont font(atlasTex, TextureAtlasSpriteFont());

  EXPECT_EQ(atlasTex, font.GetAtlasTexture());
  // Since there are no valid chars, we expect a measure to be zero
  EXPECT_EQ(PxSize2D(0, 0), font.MeasureString(StringViewLite("Hello world")));
  EXPECT_EQ(PxSize2D(0, 0), font.MeasureString(StringViewLite("Hello world").substr(1, 2)));
}
