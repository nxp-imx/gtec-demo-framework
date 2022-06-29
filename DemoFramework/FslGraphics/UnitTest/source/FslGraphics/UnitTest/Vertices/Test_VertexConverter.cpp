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
#include <FslBase/Log/Math/LogMatrix.hpp>
#include <FslBase/Log/Math/LogPoint2.hpp>
#include <FslBase/Log/Math/LogVector2.hpp>
#include <FslBase/Log/Math/LogVector3.hpp>
#include <FslBase/Log/Math/LogVector4.hpp>
#include <FslGraphics/Log/LogColor.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <FslGraphics/Vertices/VertexConverter.hpp>
#include <FslGraphics/Vertices/VertexPosition.hpp>
#include <FslGraphics/Vertices/VertexPositionColorF.hpp>
#include <array>

using namespace Fsl;

namespace
{
  using TestVertices_VertexConverter = TestFixtureFslGraphics;
}

TEST(TestVertices_VertexConverter, Convert_VertexPositionColorToVertexPositionColor)
{
  constexpr Vector3 pos1(1.0f, 2.0f, 3.0f);
  constexpr Vector4 col1(4.0f, 5.0f, 6.0f, 7.0f);
  constexpr Vector3 pos2(8.0f, 9.0f, 10.0f);
  constexpr Vector4 col2(11.0f, 12.0f, 13.0f, 14.0f);
  constexpr Vector3 pos3(15.0f, 16.0f, 17.0f);
  constexpr Vector4 col3(18.0f, 19.0f, 20.0f, 21.0f);
  const std::array<VertexPositionColorF, 3> src = {VertexPositionColorF(pos1, col1), VertexPositionColorF(pos2, col2),
                                                   VertexPositionColorF(pos3, col3)};
  std::array<VertexPositionColorF, 3> dst;

  VertexConverter::Convert(dst.data(), dst.size(), src.data(), src.size());

  EXPECT_EQ(src[0], dst[0]);
  EXPECT_EQ(src[1], dst[1]);
  EXPECT_EQ(src[2], dst[2]);
}


TEST(TestVertices_VertexConverter, Convert_VertexPositionColorToVertexPosition)
{
  constexpr Vector3 pos1(1.0f, 2.0f, 3.0f);
  constexpr Vector4 col1(4.0f, 5.0f, 6.0f, 7.0f);
  constexpr Vector3 pos2(8.0f, 9.0f, 10.0f);
  constexpr Vector4 col2(11.0f, 12.0f, 13.0f, 14.0f);
  constexpr Vector3 pos3(15.0f, 16.0f, 17.0f);
  constexpr Vector4 col3(18.0f, 19.0f, 20.0f, 21.0f);
  const std::array<VertexPositionColorF, 3> src = {VertexPositionColorF(pos1, col1), VertexPositionColorF(pos2, col2),
                                                   VertexPositionColorF(pos3, col3)};
  std::array<VertexPosition, 3> dst;

  VertexConverter::Convert(dst.data(), dst.size(), src.data(), src.size());

  EXPECT_EQ(src[0].Position, dst[0].Position);
  EXPECT_EQ(src[1].Position, dst[1].Position);
  EXPECT_EQ(src[2].Position, dst[2].Position);
}


TEST(TestVertices_VertexConverter, Convert_VertexPositionToVertexPositionColor)
{
  std::array<VertexPosition, 3> src = {VertexPosition(1.0f, 2.0f, 3.0f), VertexPosition(4.0f, 5.0f, 6.0f), VertexPosition(7.0f, 8.0f, 9.0f)};
  std::array<VertexPositionColorF, 3> dst;

  VertexConverter::Convert(dst.data(), dst.size(), src.data(), src.size());

  EXPECT_EQ(src[0].Position, dst[0].Position);
  EXPECT_EQ(src[1].Position, dst[1].Position);
  EXPECT_EQ(src[2].Position, dst[2].Position);
  // Fields that didn't exist
  EXPECT_EQ(Vector4(), dst[0].Color);
  EXPECT_EQ(Vector4(), dst[1].Color);
  EXPECT_EQ(Vector4(), dst[2].Color);
}
