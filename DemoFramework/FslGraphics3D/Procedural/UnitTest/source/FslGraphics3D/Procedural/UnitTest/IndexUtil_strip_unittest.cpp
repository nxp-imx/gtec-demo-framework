/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslGraphics/Vertices/VertexPosition.hpp>
#include <FslGraphics3D/Procedural/IndexUtil.hpp>
#include <gtest/gtest.h>


namespace
{
  using namespace Fsl;
  using namespace Fsl::Procedural;

  uint16_t g_indices[] = {0, 1, 2, 3};

  const size_t g_indexCount = sizeof(g_indices) / sizeof(g_indices[0]);
  const PrimitiveType g_primitiveType = PrimitiveType::TriangleStrip;
  const PrimitiveType g_otherPrimitiveType = PrimitiveType::TriangleList;

  std::vector<uint16_t> g_srcIndices(g_indices, g_indices + g_indexCount);


  void CheckIndices(const std::vector<uint16_t>& src, const int srcOffset, const int srcMod)
  {
    for (std::size_t i = 0; i < g_indexCount; ++i)
    {
      EXPECT_EQ(g_indices[i], src[srcOffset + i] - srcMod);
    }
  }
}


TEST(IndexUtil, AppendTriangleStrip_TooSmallDstVector)
{
  std::vector<uint16_t> dstIndices;
  EXPECT_THROW(IndexUtil::Merge<uint16_t>(dstIndices, 0, g_srcIndices, 0, g_primitiveType), std::invalid_argument);
}


TEST(IndexUtil, AppendTriangleStrip_OutOfBounds)
{
  std::vector<uint16_t> dstIndices(g_indexCount);
  EXPECT_THROW(IndexUtil::Merge<uint16_t>(dstIndices, -1, g_srcIndices, 0, g_primitiveType), std::invalid_argument);
  EXPECT_THROW(IndexUtil::Merge<uint16_t>(dstIndices, 1, g_srcIndices, 0, g_primitiveType), std::invalid_argument);
  EXPECT_THROW(IndexUtil::Merge<uint16_t>(dstIndices, g_indexCount, g_srcIndices, 0, g_primitiveType), std::invalid_argument);
  EXPECT_THROW(IndexUtil::Merge<uint16_t>(dstIndices, g_indexCount - 1, g_srcIndices, 0, g_primitiveType), std::invalid_argument);
}


TEST(IndexUtil, AppendTriangleStrip_OneAppendEnoughRoom)
{
  std::vector<uint16_t> dstIndices(g_indexCount);
  const auto written = IndexUtil::Merge<uint16_t>(dstIndices, 0, g_srcIndices, 0, g_primitiveType);

  EXPECT_EQ(g_indexCount, written);
  CheckIndices(dstIndices, 0, 0);
}


TEST(IndexUtil, AppendTriangleStrip_TwoAppendsEnoughRoom)
{
  std::vector<uint16_t> dstIndices((g_indexCount * 2) + 4);
  auto written = IndexUtil::Merge<uint16_t>(dstIndices, 0, g_srcIndices, 0, g_primitiveType);

  EXPECT_EQ(g_indexCount, written);
  CheckIndices(dstIndices, 0, 0);

  written = IndexUtil::Merge<uint16_t>(dstIndices, g_indexCount, g_srcIndices, 0, g_primitiveType);

  EXPECT_EQ(g_indexCount + 4, written);
  CheckIndices(dstIndices, g_indexCount + 4, 0);
}


TEST(IndexUtil, AppendTriangleStrip_OneAppendEnoughRoomWithMod)
{
  const int32_t mod = 50;

  std::vector<uint16_t> dstIndices(g_indexCount);
  const auto written = IndexUtil::Merge<uint16_t>(dstIndices, 0, g_srcIndices, mod, g_primitiveType);

  EXPECT_EQ(g_indexCount, written);
  CheckIndices(dstIndices, 0, mod);
}


TEST(IndexUtil, AppendTriangleStrip_TwoAppendsEnoughRoomWithMod)
{
  const std::size_t vertexCount = 50;
  std::vector<uint16_t> dstIndices((g_indexCount * 2) + 4);
  auto written = IndexUtil::Merge<uint16_t>(dstIndices, 0, g_srcIndices, 0, g_primitiveType);

  EXPECT_EQ(g_indexCount, written);
  CheckIndices(dstIndices, 0, 0);

  written = IndexUtil::Merge<uint16_t>(dstIndices, g_indexCount, g_srcIndices, vertexCount, g_primitiveType);

  EXPECT_EQ(g_indexCount + 4, written);
  CheckIndices(dstIndices, g_indexCount + 4, vertexCount);
}
