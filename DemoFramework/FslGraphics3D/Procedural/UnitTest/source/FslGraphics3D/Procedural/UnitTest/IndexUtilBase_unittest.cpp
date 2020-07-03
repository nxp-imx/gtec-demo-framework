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
#include <FslGraphics3D/Procedural/IndexUtilBase.hpp>
#include <gtest/gtest.h>


namespace
{
  using namespace Fsl;
  using namespace Fsl::Procedural;
}

// ----------------------------------------------------------------------------------

TEST(IndexUtilBase, CalcAppendMinimumCapacityLineLine)
{
  std::size_t calced = 0;

  calced = IndexUtilBase::CalcAppendMinimumCapacity(0, 10, PrimitiveType::LineList);
  EXPECT_EQ(10u, calced);
}


TEST(IndexUtilBase, CalcAppendMinimumCapacityLineStrip)
{
  std::size_t calced = 0;

  calced = IndexUtilBase::CalcAppendMinimumCapacity(0, 10, PrimitiveType::LineStrip);
  EXPECT_EQ(10u, calced);

  calced = IndexUtilBase::CalcAppendMinimumCapacity(2, 10, PrimitiveType::LineStrip);
  // 11 because for a linestrip to be mergeable it requires the first entry to be equal to what we are merging to
  EXPECT_EQ(11u, calced);
}


TEST(IndexUtilBase, CalcAppendMinimumCapacityTriangleList)
{
  std::size_t calced = 0;

  calced = IndexUtilBase::CalcAppendMinimumCapacity(0, 9, PrimitiveType::TriangleList);
  EXPECT_EQ(9u, calced);

  calced = IndexUtilBase::CalcAppendMinimumCapacity(3, 9, PrimitiveType::TriangleList);
  EXPECT_EQ(12u, calced);
}


TEST(IndexUtilBase, CalcAppendMinimumCapacityTriangleStripEven)
{
  std::size_t calced = 0;

  calced = IndexUtilBase::CalcAppendMinimumCapacity(0, 6, PrimitiveType::TriangleStrip);
  EXPECT_EQ(6u, calced);
}


TEST(IndexUtilBase, CalcAppendMinimumCapacityTriangleStripUneven)
{
  std::size_t calced = 0;

  calced = IndexUtilBase::CalcAppendMinimumCapacity(0, 7, PrimitiveType::TriangleStrip);
  EXPECT_EQ(7u, calced);
}


TEST(IndexUtilBase, CalcAppendMinimumCapacityTriangleStripEvenOnEven)
{
  std::size_t calced = 0;

  calced = IndexUtilBase::CalcAppendMinimumCapacity(10, 6, PrimitiveType::TriangleStrip);
  EXPECT_EQ(10u + 4u + 6u, calced);
}


TEST(IndexUtilBase, CalcAppendMinimumCapacityTriangleStripEvenOnUneven)
{
  std::size_t calced = 0;

  calced = IndexUtilBase::CalcAppendMinimumCapacity(11, 6, PrimitiveType::TriangleStrip);
  EXPECT_EQ(11u + 5u + 6u, calced);
}


TEST(IndexUtilBase, CalcAppendMinimumCapacityTriangleStripUnevenOnEven)
{
  std::size_t calced = 0;

  calced = IndexUtilBase::CalcAppendMinimumCapacity(10, 7, PrimitiveType::TriangleStrip);
  EXPECT_EQ(10u + 4u + 7u, calced);
}


TEST(IndexUtilBase, CalcAppendMinimumCapacityTriangleStripUnevenOnUneven)
{
  std::size_t calced = 0;

  calced = IndexUtilBase::CalcAppendMinimumCapacity(11, 7, PrimitiveType::TriangleStrip);
  EXPECT_EQ(11u + 5u + 7u, calced);
}

// ----------------------------------------------------------------------------------

TEST(IndexUtilBase, CalcAppendInstancesRequiredCapacityLineLine)
{
  std::size_t calced = 0;

  calced = IndexUtilBase::CalcAppendInstancesRequiredCapacity(0, 10, 1, PrimitiveType::LineList);
  EXPECT_EQ(10u, calced);

  calced = IndexUtilBase::CalcAppendInstancesRequiredCapacity(0, 10, 2, PrimitiveType::LineList);
  EXPECT_EQ(10u + 10u, calced);
}


TEST(IndexUtilBase, CalcAppendInstancesRequiredCapacityLineStrip)
{
  std::size_t calced = 0;

  calced = IndexUtilBase::CalcAppendInstancesRequiredCapacity(0, 10, 1, PrimitiveType::LineStrip);
  EXPECT_EQ(10u, calced);

  calced = IndexUtilBase::CalcAppendInstancesRequiredCapacity(2, 10, 1, PrimitiveType::LineStrip);
  // 11 because for a linestrip to be mergeable it requires the first entry to be equal to what we are merging to
  EXPECT_EQ(11u, calced);
}


TEST(IndexUtilBase, CalcAppendInstancesRequiredCapacityTriangleList)
{
  std::size_t calced = 0;

  calced = IndexUtilBase::CalcAppendInstancesRequiredCapacity(0, 9, 1, PrimitiveType::TriangleList);
  EXPECT_EQ(9u, calced);

  calced = IndexUtilBase::CalcAppendInstancesRequiredCapacity(3, 9, 1, PrimitiveType::TriangleList);
  EXPECT_EQ(12u, calced);

  calced = IndexUtilBase::CalcAppendInstancesRequiredCapacity(0, 9, 2, PrimitiveType::TriangleList);
  EXPECT_EQ(9u + 9u, calced);

  calced = IndexUtilBase::CalcAppendInstancesRequiredCapacity(3, 9, 2, PrimitiveType::TriangleList);
  EXPECT_EQ(3u + 9u + 9u, calced);
}


TEST(IndexUtilBase, CalcAppendInstancesRequiredCapacityTriangleStripEven)
{
  std::size_t calced = 0;

  calced = IndexUtilBase::CalcAppendInstancesRequiredCapacity(0, 6, 1, PrimitiveType::TriangleStrip);
  EXPECT_EQ(6u, calced);

  calced = IndexUtilBase::CalcAppendInstancesRequiredCapacity(0, 6, 2, PrimitiveType::TriangleStrip);
  EXPECT_EQ(6u + 4u + 6u, calced);
}


TEST(IndexUtilBase, CalcAppendInstancesRequiredCapacityTriangleStripUneven)
{
  std::size_t calced = 0;

  calced = IndexUtilBase::CalcAppendInstancesRequiredCapacity(0, 7, 1, PrimitiveType::TriangleStrip);
  EXPECT_EQ(7u, calced);

  calced = IndexUtilBase::CalcAppendInstancesRequiredCapacity(0, 7, 2, PrimitiveType::TriangleStrip);
  EXPECT_EQ(7u + 5u + 7u, calced);
}


TEST(IndexUtilBase, CalcAppendInstancesRequiredCapacityTriangleStripEvenOnEven)
{
  std::size_t calced = 0;

  calced = IndexUtilBase::CalcAppendInstancesRequiredCapacity(10, 6, 1, PrimitiveType::TriangleStrip);
  EXPECT_EQ(10u + 4u + 6u, calced);

  calced = IndexUtilBase::CalcAppendInstancesRequiredCapacity(10, 6, 2, PrimitiveType::TriangleStrip);
  EXPECT_EQ(10u + 4u + 6u + 4u + 6u, calced);
}


TEST(IndexUtilBase, CalcAppendInstancesRequiredCapacityTriangleStripEvenOnUneven)
{
  std::size_t calced = 0;

  calced = IndexUtilBase::CalcAppendInstancesRequiredCapacity(11, 6, 1, PrimitiveType::TriangleStrip);
  EXPECT_EQ(11u + 5u + 6u, calced);

  calced = IndexUtilBase::CalcAppendInstancesRequiredCapacity(11, 6, 2, PrimitiveType::TriangleStrip);
  EXPECT_EQ(11u + 5u + 6u + 4u + 6u, calced);
}


TEST(IndexUtilBase, CalcAppendInstancesRequiredCapacityTriangleStripUnevenOnEven)
{
  std::size_t calced = 0;

  calced = IndexUtilBase::CalcAppendInstancesRequiredCapacity(10, 7, 1, PrimitiveType::TriangleStrip);
  EXPECT_EQ(10u + 4 + 7, calced);

  calced = IndexUtilBase::CalcAppendInstancesRequiredCapacity(10, 7, 2, PrimitiveType::TriangleStrip);
  EXPECT_EQ(10u + 4 + 7 + 5 + 7, calced);
}


TEST(IndexUtilBase, CalcAppendInstancesRequiredCapacityTriangleStripUnevenOnUneven)
{
  std::size_t calced = 0;

  calced = IndexUtilBase::CalcAppendInstancesRequiredCapacity(11, 7, 1, PrimitiveType::TriangleStrip);
  EXPECT_EQ(11u + 5 + 7, calced);

  calced = IndexUtilBase::CalcAppendInstancesRequiredCapacity(11, 7, 2, PrimitiveType::TriangleStrip);
  EXPECT_EQ(11u + 5 + 7 + 5 + 7, calced);
}


// ----------------------------------------------------------------------------------

TEST(IndexUtilBase, CalcAppendMaxInstancesThatFitLines)
{
  std::size_t calced = 0;

  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(0, 10, 100, PrimitiveType::LineList);
  EXPECT_EQ(100u / 10u, calced);

  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(0, 10, 100, PrimitiveType::LineStrip);
  EXPECT_EQ((100u / 10u) + 1, calced);
}


TEST(IndexUtilBase, CalcAppendMaxInstancesThatFitLinesTriangleList)
{
  std::size_t calced = 0;

  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(0, 10, 100, PrimitiveType::TriangleList);
  EXPECT_EQ((100u / 10u), calced);
}

TEST(IndexUtilBase, CalcAppendMaxInstancesThatFitStripEven)
{
  std::size_t calced = 0;

  // Triangle strip calculations
  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(0, 6, 6 + 4 + 6, PrimitiveType::TriangleStrip);
  EXPECT_EQ(2u, calced);

  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(0, 6, 6 + 4 + 6 + 4 + 6, PrimitiveType::TriangleStrip);
  EXPECT_EQ(3u, calced);
}

TEST(IndexUtilBase, CalcAppendMaxInstancesThatFitStripEvenOnEvenBuffer)
{
  std::size_t calced = 0;

  // Triangle strip calculations on 'even' buffer
  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(6, 6, 6 + 3 + 6, PrimitiveType::TriangleStrip);
  EXPECT_EQ(0u, calced);

  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(6, 6, 6 + 4 + 6, PrimitiveType::TriangleStrip);
  EXPECT_EQ(1u, calced);

  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(6, 6, 6 + 3 + 6 + 4 + 6, PrimitiveType::TriangleStrip);
  EXPECT_EQ(1u, calced);

  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(6, 6, 6 + 4 + 6 + 4 + 6, PrimitiveType::TriangleStrip);
  EXPECT_EQ(2u, calced);
}


TEST(IndexUtilBase, CalcAppendMaxInstancesThatFitStripEvenOnUnevenBuffer)
{
  std::size_t calced = 0;

  // Triangle strip calculations on 'uneven' buffer
  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(5, 6, 5 + 3 + 6, PrimitiveType::TriangleStrip);
  EXPECT_EQ(0u, calced);

  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(5, 6, 5 + 4 + 6, PrimitiveType::TriangleStrip);
  EXPECT_EQ(0u, calced);

  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(5, 6, 5 + 5 + 6, PrimitiveType::TriangleStrip);
  EXPECT_EQ(1u, calced);

  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(5, 6, 5 + 4 + 6 + 4 + 6, PrimitiveType::TriangleStrip);
  EXPECT_EQ(1u, calced);

  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(5, 6, 5 + 5 + 6 + 4 + 6, PrimitiveType::TriangleStrip);
  EXPECT_EQ(2u, calced);
}


TEST(IndexUtilBase, CalcAppendMaxInstancesThatFitStripUneven)
{
  std::size_t calced = 0;

  // Uneven triangle strip calculations
  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(0, 7, 7 + 5 + 7, PrimitiveType::TriangleStrip);
  EXPECT_EQ(2u, calced);

  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(0, 7, 7 + 5 + 7 + 5 + 7, PrimitiveType::TriangleStrip);
  EXPECT_EQ(3u, calced);
}


TEST(IndexUtilBase, CalcAppendMaxInstancesThatFitStripUnevenOnEvenBuffer)
{
  std::size_t calced = 0;

  // Triangle strip calculations on 'even' buffer
  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(6, 7, 6 + 3 + 7, PrimitiveType::TriangleStrip);
  EXPECT_EQ(0u, calced);

  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(6, 7, 6 + 4 + 7, PrimitiveType::TriangleStrip);
  EXPECT_EQ(1u, calced);

  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(6, 7, 6 + 3 + 7 + 5 + 7, PrimitiveType::TriangleStrip);
  EXPECT_EQ(1u, calced);

  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(6, 7, 6 + 4 + 7 + 5 + 7, PrimitiveType::TriangleStrip);
  EXPECT_EQ(2u, calced);

  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(6, 7, 6 + 5 + 7 + 5 + 7, PrimitiveType::TriangleStrip);
  EXPECT_EQ(2u, calced);
}


TEST(IndexUtilBase, CalcAppendMaxInstancesThatFitStripUnevenOnUnevenBuffer)
{
  std::size_t calced = 0;

  // Triangle strip calculations on 'even' buffer
  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(11, 7, 11 + 4 + 7, PrimitiveType::TriangleStrip);
  EXPECT_EQ(0u, calced);

  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(11, 7, 11 + 5 + 7, PrimitiveType::TriangleStrip);
  EXPECT_EQ(1u, calced);

  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(11, 7, 11 + 4 + 7 + 5 + 7, PrimitiveType::TriangleStrip);
  EXPECT_EQ(1u, calced);

  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(11, 7, 11 + 5 + 7 + 5 + 7, PrimitiveType::TriangleStrip);
  EXPECT_EQ(2u, calced);

  calced = IndexUtilBase::CalcAppendMaxInstancesThatFit(11, 7, 11 + 6 + 7 + 5 + 7, PrimitiveType::TriangleStrip);
  EXPECT_EQ(2u, calced);
}
