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

#include <FslBase/Log/IO/LogPath.hpp>
#include <FslBase/Log/Math/LogPoint2.hpp>
#include <FslBase/Log/Math/LogRectangle.hpp>
#include <FslBase/Log/Math/LogThickness.hpp>
#include <FslBase/Log/String/LogUTF8String.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/BinaryTextureAtlasLoader.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphicsContent.hpp>

using namespace Fsl;

namespace
{
  constexpr const uint32_t DEFAULT_DP = 160;

  class TestTextureAtlas_BinaryTextureAtlasLoader : public TestFixtureFslGraphicsContent
  {
  protected:
    IO::Path m_smallAtlasFilename;
    IO::Path m_notExistingFilename;

  public:
    TestTextureAtlas_BinaryTextureAtlasLoader()
      : m_smallAtlasFilename(IO::Path::Combine(GetContentPath(), "SmallAtlas.bta"))
      , m_notExistingFilename(IO::Path::Combine(GetContentPath(), "ThisIsNotAFile.txt"))
    {
    }
  };
}


TEST_F(TestTextureAtlas_BinaryTextureAtlasLoader, Load)
{
  BasicTextureAtlas atlas;
  BinaryTextureAtlasLoader::Load(atlas, m_smallAtlasFilename);

  EXPECT_EQ(1, atlas.Count());

  const auto& entry0 = atlas.GetEntry(0);
  EXPECT_EQ(UTF8String("Banners"), entry0.Name);
  EXPECT_EQ(PxPoint2::Create(-15, -7), entry0.TextureInfo.OffsetPx);
  EXPECT_EQ(PxExtent2D::Create(1920, 1080), entry0.TextureInfo.ExtentPx);
  EXPECT_EQ(PxThicknessU::Create(17, 9, 1426, 873), entry0.TextureInfo.TrimMarginPx);
  EXPECT_EQ(PxRectangleU32::Create(2, 2, 477, 198), entry0.TextureInfo.TrimmedRectPx);
  EXPECT_EQ(DEFAULT_DP, entry0.TextureInfo.Dpi);
}


TEST_F(TestTextureAtlas_BinaryTextureAtlasLoader, Load_NotFound)
{
  BasicTextureAtlas atlas;

  EXPECT_THROW(BinaryTextureAtlasLoader::Load(atlas, m_notExistingFilename), FormatException);
  EXPECT_EQ(0, atlas.Count());
}
