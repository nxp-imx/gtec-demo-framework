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

#include <FslGraphics/TextureAtlas/TextureAtlasNineSlicePatch.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  using Test_TextureAtlasNineSlicePatch = TestFixtureFslGraphics;
}


TEST(Test_TextureAtlasNineSlicePatch, Construct_Default)
{
  TextureAtlasNineSlicePatch value;

  EXPECT_EQ(AtlasNineSlicePatchInfo(), value.Patch);
  EXPECT_EQ(0u, value.TextureIndex);
}

TEST(Test_TextureAtlasNineSlicePatch, Construct)
{
  constexpr AtlasNineSliceFlags flags{AtlasNineSliceFlags::Transparent};
  constexpr AtlasNineSlicePatchInfo patch(PxThicknessU::Create(1, 2, 3, 4), PxThicknessU::Create(5, 6, 7, 8), flags);
  constexpr uint32_t textureIndex = 9u;
  TextureAtlasNineSlicePatch value(textureIndex, patch);

  EXPECT_EQ(patch, value.Patch);
  EXPECT_EQ(textureIndex, value.TextureIndex);
}
