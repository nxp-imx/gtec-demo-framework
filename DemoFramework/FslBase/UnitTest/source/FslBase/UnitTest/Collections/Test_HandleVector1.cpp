/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslBase/Collections/HandleVector.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

using namespace Fsl;

namespace
{
  namespace LocalConfig
  {
    constexpr auto InvalidHandle0 = HandleVectorConfig::InvalidHandle;
    constexpr auto InvalidHandle1 = std::numeric_limits<HandleVector<std::string>::handle_type>::max();
  }

  using TestCollections_HandleVector1 = TestFixtureFslBase;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, RemoveRange_3_0_0)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.RemoveRange(0, 0);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, RemoveRange_3_0_1)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.RemoveRange(0, 1);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());

  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val2, vector[0]);
  EXPECT_EQ(val3, vector[1]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, RemoveRange_3_1_1)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.RemoveRange(1, 1);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val3, vector[1]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, RemoveRange_3_2_1)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.RemoveRange(2, 1);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, RemoveRange_4_OutOfBoundsStartIndex)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  auto item4 = vector.Add(val4);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(4u, vector.Count());

  EXPECT_THROW(vector.RemoveRange(4, 2), std::invalid_argument);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(4u, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));
  EXPECT_EQ(val4, vector.Get(item4));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
  EXPECT_EQ(val4, vector[3]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, RemoveRange_4_OutOfBoundsLength)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  auto item4 = vector.Add(val4);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(4u, vector.Count());

  EXPECT_THROW(vector.RemoveRange(0, 5), std::invalid_argument);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(4u, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));
  EXPECT_EQ(val4, vector.Get(item4));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
  EXPECT_EQ(val4, vector[3]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, RemoveRange_4_0_2)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";

  vector.Add(val1);
  vector.Add(val2);
  auto item3 = vector.Add(val3);
  auto item4 = vector.Add(val4);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(4u, vector.Count());

  vector.RemoveRange(0, 2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());

  EXPECT_EQ(val3, vector.Get(item3));
  EXPECT_EQ(val4, vector.Get(item4));

  EXPECT_EQ(val3, vector[0]);
  EXPECT_EQ(val4, vector[1]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, RemoveRange_4_1_2)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";

  auto item1 = vector.Add(val1);
  vector.Add(val2);
  vector.Add(val3);
  auto item4 = vector.Add(val4);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(4u, vector.Count());

  vector.RemoveRange(1, 2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val4, vector.Get(item4));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val4, vector[1]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, RemoveRange_4_2_2)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  vector.Add(val3);
  vector.Add(val4);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(4u, vector.Count());

  vector.RemoveRange(2, 2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, RemoveRange_5_0_2)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";
  const std::string val5 = "E";

  vector.Add(val1);
  vector.Add(val2);
  auto item3 = vector.Add(val3);
  auto item4 = vector.Add(val4);
  auto item5 = vector.Add(val5);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(5u, vector.Count());

  vector.RemoveRange(0, 2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  EXPECT_EQ(val3, vector.Get(item3));
  EXPECT_EQ(val4, vector.Get(item4));
  EXPECT_EQ(val5, vector.Get(item5));

  EXPECT_EQ(val3, vector[0]);
  EXPECT_EQ(val4, vector[1]);
  EXPECT_EQ(val5, vector[2]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, RemoveRange_5_1_2)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";
  const std::string val5 = "E";

  auto item1 = vector.Add(val1);
  vector.Add(val2);
  vector.Add(val3);
  auto item4 = vector.Add(val4);
  auto item5 = vector.Add(val5);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(5u, vector.Count());

  vector.RemoveRange(1, 2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val4, vector.Get(item4));
  EXPECT_EQ(val5, vector.Get(item5));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val4, vector[1]);
  EXPECT_EQ(val5, vector[2]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, RemoveRange_5_2_2)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";
  const std::string val5 = "E";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  vector.Add(val3);
  vector.Add(val4);
  auto item5 = vector.Add(val5);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(5u, vector.Count());

  vector.RemoveRange(2, 2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val5, vector.Get(item5));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val5, vector[2]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, RemoveRange_5_3_2)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";
  const std::string val5 = "E";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  vector.Add(val4);
  vector.Add(val5);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(5u, vector.Count());

  vector.RemoveRange(3, 2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, Reserve_Empty)
{
  HandleVector<std::string> vector(10);

  EXPECT_EQ(10u, vector.Capacity());

  // This should do nothing
  vector.Reserve(9);
  EXPECT_EQ(10u, vector.Capacity());
  EXPECT_EQ(0u, vector.Count());

  // This should grow the vector
  vector.Reserve(11);
  EXPECT_GE(vector.Capacity(), 11u);
  EXPECT_EQ(vector.Count(), 0u);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, Reserve)
{
  HandleVector<std::string> vector(10);
  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);

  EXPECT_EQ(10u, vector.Capacity());
  EXPECT_EQ(3u, vector.Count());

  // This should do nothing
  vector.Reserve(9);
  EXPECT_EQ(vector.Capacity(), 10u);
  EXPECT_EQ(vector.Count(), 3u);

  // This should grow the vector
  vector.Reserve(11);
  EXPECT_GE(vector.Capacity(), 11u);
  EXPECT_EQ(vector.Count(), 3u);

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, Swap)
{
  HandleVector<std::string> vector(10);
  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);

  EXPECT_EQ(10u, vector.Capacity());
  EXPECT_EQ(3u, vector.Count());

  // Check that everything is at the correct location
  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);

  vector.Swap(item1, item3);

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val3, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val1, vector[2]);

  vector.Swap(item2, item1);

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val3, vector[0]);
  EXPECT_EQ(val1, vector[1]);
  EXPECT_EQ(val2, vector[2]);

  vector.Swap(item3, item1);

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val3, vector[1]);
  EXPECT_EQ(val2, vector[2]);

  vector.Swap(item2, item3);

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, Swap_Self)
{
  HandleVector<std::string> vector(10);
  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);

  // Check that everything is at the correct location
  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);

  vector.Swap(item1, item1);

  // Check that everything is at the correct location
  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);

  vector.Swap(item2, item2);

  // Check that everything is at the correct location
  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);

  vector.Swap(item3, item3);

  // Check that everything is at the correct location
  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, Swap_Handle1Invalid)
{
  HandleVector<std::string> vector(10);
  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  vector.Add(val2);
  vector.Add(val3);

  EXPECT_THROW(vector.Swap(LocalConfig::InvalidHandle0, item1), std::invalid_argument);
  EXPECT_THROW(vector.Swap(LocalConfig::InvalidHandle1, item1), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, Swap_Handle2Invalid)
{
  HandleVector<std::string> vector(10);
  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  vector.Add(val2);
  vector.Add(val3);

  EXPECT_THROW(vector.Swap(item1, LocalConfig::InvalidHandle0), std::invalid_argument);
  EXPECT_THROW(vector.Swap(item1, LocalConfig::InvalidHandle1), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, SwapAt)
{
  HandleVector<std::string> vector(10);
  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);

  EXPECT_EQ(10u, vector.Capacity());
  EXPECT_EQ(3u, vector.Count());

  // Check that everything is at the correct location
  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);

  vector.SwapAt(0, 2);

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val3, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val1, vector[2]);

  vector.SwapAt(2, 1);

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val3, vector[0]);
  EXPECT_EQ(val1, vector[1]);
  EXPECT_EQ(val2, vector[2]);

  vector.SwapAt(1, 0);

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val3, vector[1]);
  EXPECT_EQ(val2, vector[2]);

  vector.SwapAt(1, 2);

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, SwapAt_Self)
{
  HandleVector<std::string> vector(10);
  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);

  // Check that everything is at the correct location
  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);

  vector.SwapAt(0, 0);

  // Check that everything is at the correct location
  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);

  vector.SwapAt(1, 1);

  // Check that everything is at the correct location
  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);

  vector.SwapAt(2, 2);

  // Check that everything is at the correct location
  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, SwapAt_Index0OutOfBounds)
{
  HandleVector<std::string> vector(10);
  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  vector.Add(val1);
  vector.Add(val2);
  vector.Add(val3);

  EXPECT_THROW(vector.SwapAt(3, 0), IndexOutOfRangeException);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, SwapAt_Index1OutOfBounds)
{
  HandleVector<std::string> vector(10);
  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  vector.Add(val1);
  vector.Add(val2);
  vector.Add(val3);

  EXPECT_THROW(vector.SwapAt(0, 3), IndexOutOfRangeException);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, MoveFromTo)
{
  HandleVector<std::string> vector(10);
  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);

  EXPECT_EQ(10u, vector.Capacity());
  EXPECT_EQ(3u, vector.Count());

  // Check that everything is at the correct location
  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);

  vector.MoveFromTo(item1, item3);
  // New order: BCA

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val2, vector[0]);
  EXPECT_EQ(val3, vector[1]);
  EXPECT_EQ(val1, vector[2]);

  vector.MoveFromTo(item1, item2);
  // New order: ABC

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);

  vector.MoveFromTo(item2, item1);
  // New order: BAC

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val2, vector[0]);
  EXPECT_EQ(val1, vector[1]);
  EXPECT_EQ(val3, vector[2]);

  vector.MoveFromTo(item3, item1);
  // New order: BCA

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val2, vector[0]);
  EXPECT_EQ(val3, vector[1]);
  EXPECT_EQ(val1, vector[2]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, MoveFromTo_Self)
{
  HandleVector<std::string> vector(10);
  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);

  // Check that everything is at the correct location
  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);

  vector.MoveFromTo(item1, item1);

  // Check that everything is at the correct location
  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);

  vector.MoveFromTo(item2, item2);

  // Check that everything is at the correct location
  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);

  vector.MoveFromTo(item3, item3);

  // Check that everything is at the correct location
  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, MoveFromTo_Handle1Invalid)
{
  HandleVector<std::string> vector(10);
  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  vector.Add(val2);
  vector.Add(val3);

  EXPECT_THROW(vector.MoveFromTo(LocalConfig::InvalidHandle0, item1), std::invalid_argument);
  EXPECT_THROW(vector.MoveFromTo(LocalConfig::InvalidHandle1, item1), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, MoveFromTo_Handle2Invalid)
{
  HandleVector<std::string> vector(10);
  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  vector.Add(val2);
  vector.Add(val3);

  EXPECT_THROW(vector.MoveFromTo(item1, LocalConfig::InvalidHandle0), std::invalid_argument);
  EXPECT_THROW(vector.MoveFromTo(item1, LocalConfig::InvalidHandle1), std::invalid_argument);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, MoveAtFromTo)
{
  HandleVector<std::string> vector(10);
  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);

  EXPECT_EQ(10u, vector.Capacity());
  EXPECT_EQ(3u, vector.Count());

  // Check that everything is at the correct location
  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);

  vector.MoveAtFromTo(0, 2);
  // New order: BCA

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val2, vector[0]);
  EXPECT_EQ(val3, vector[1]);
  EXPECT_EQ(val1, vector[2]);

  vector.MoveAtFromTo(2, 0);
  // New order: ABC

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);

  vector.MoveAtFromTo(1, 0);
  // New order: BAC

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val2, vector[0]);
  EXPECT_EQ(val1, vector[1]);
  EXPECT_EQ(val3, vector[2]);

  vector.MoveAtFromTo(1, 2);
  // New order: BCA

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val2, vector[0]);
  EXPECT_EQ(val3, vector[1]);
  EXPECT_EQ(val1, vector[2]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, MoveAtFromTo_Self)
{
  HandleVector<std::string> vector(10);
  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);

  // Check that everything is at the correct location
  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);

  vector.MoveAtFromTo(0, 0);

  // Check that everything is at the correct location
  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);

  vector.MoveAtFromTo(1, 1);

  // Check that everything is at the correct location
  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);

  vector.MoveAtFromTo(2, 2);

  // Check that everything is at the correct location
  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, MoveAtFromTo_Index0OutOfBounds)
{
  HandleVector<std::string> vector(10);
  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  vector.Add(val1);
  vector.Add(val2);
  vector.Add(val3);

  EXPECT_THROW(vector.MoveAtFromTo(3, 0), IndexOutOfRangeException);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, MoveAtFromTo_Index1OutOfBounds)
{
  HandleVector<std::string> vector(10);
  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  vector.Add(val1);
  vector.Add(val2);
  vector.Add(val3);

  EXPECT_THROW(vector.MoveAtFromTo(0, 3), IndexOutOfRangeException);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, TestHandles)
{
  HandleVector<std::string> vector(10);

  auto item1 = vector.Add("A");
  EXPECT_TRUE(vector.DEBUG_IsValid());
  auto item2 = vector.Add("B");
  EXPECT_TRUE(vector.DEBUG_IsValid());
  auto item3 = vector.Add("C");
  EXPECT_TRUE(vector.DEBUG_IsValid());
  auto item4 = vector.Add("D");
  EXPECT_TRUE(vector.DEBUG_IsValid());
  auto item5 = vector.Add("E");
  EXPECT_TRUE(vector.DEBUG_IsValid());

  EXPECT_TRUE(vector.IsValidHandle(item1));
  EXPECT_TRUE(vector.IsValidHandle(item2));
  EXPECT_TRUE(vector.IsValidHandle(item3));
  EXPECT_TRUE(vector.IsValidHandle(item4));
  EXPECT_TRUE(vector.IsValidHandle(item5));

  vector.RemoveBySwap(item4);
  EXPECT_TRUE(vector.DEBUG_IsValid());

  EXPECT_TRUE(vector.IsValidHandle(item1));
  EXPECT_TRUE(vector.IsValidHandle(item2));
  EXPECT_TRUE(vector.IsValidHandle(item3));
  EXPECT_FALSE(vector.IsValidHandle(item4));
  EXPECT_TRUE(vector.IsValidHandle(item5));

  auto item4a = vector.Add("D2");
  EXPECT_TRUE(vector.DEBUG_IsValid());

  // Since we are reusing handles
  EXPECT_EQ(item4, item4a);

  EXPECT_TRUE(vector.IsValidHandle(item1));
  EXPECT_TRUE(vector.IsValidHandle(item2));
  EXPECT_TRUE(vector.IsValidHandle(item3));
  // We are reusing handles
  EXPECT_TRUE(vector.IsValidHandle(item4));
  EXPECT_TRUE(vector.IsValidHandle(item4a));
  EXPECT_TRUE(vector.IsValidHandle(item5));

  vector.RemoveBySwap(item2);
  EXPECT_TRUE(vector.DEBUG_IsValid());

  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_TRUE(vector.IsValidHandle(item1));
  EXPECT_FALSE(vector.IsValidHandle(item2));
  EXPECT_TRUE(vector.IsValidHandle(item3));
  // We are reusing handles
  EXPECT_TRUE(vector.IsValidHandle(item4));
  EXPECT_TRUE(vector.IsValidHandle(item4a));
  EXPECT_TRUE(vector.IsValidHandle(item5));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, RemoveThenAdd)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "D";
  const std::string val2 = "E";
  const std::string val3 = "F";

  const auto h1 = vector.Add("A");
  const auto h2 = vector.Add("B");
  const auto h3 = vector.Add("C");
  // Remove in reverse order to ensure that we reorder the indices
  vector.Remove(h1);
  vector.Remove(h2);
  vector.Remove(h3);
  const auto h4 = vector.Add(val1);
  const auto h5 = vector.Add(val2);
  const auto h6 = vector.Add(val3);

  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
  EXPECT_EQ(val1, vector.Get(h4));
  EXPECT_EQ(val2, vector.Get(h5));
  EXPECT_EQ(val3, vector.Get(h6));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, RemoveThenAdd2)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "D";
  const std::string val2 = "E";
  const std::string val3 = "F";

  const auto h1 = vector.Add("A");
  const auto h2 = vector.Add("B");
  const auto h3 = vector.Add("C");
  // Remove some to reorder the indices
  vector.Remove(h2);
  vector.Remove(h1);
  const auto h4 = vector.Add(val1);
  const auto h5 = vector.Add(val2);
  const auto h6 = vector.Add(val3);
  vector.Remove(h3);

  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
  EXPECT_EQ(val1, vector.Get(h4));
  EXPECT_EQ(val2, vector.Get(h5));
  EXPECT_EQ(val3, vector.Get(h6));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector1, Random)
{
  // HandleVector<std::string> vector(10);
  // SplitMix64 seedGen(1337);
  // Xoshiro256StarStar random(seedGen.Next());
  // var truth = new List<string>();
  // for (int i = 0; i < 1024; ++i)
  //{
  //  var entry = $ "{i}";

  //  if (vector.Count <= 0)
  //  {
  //    vector.Add(entry);
  //    truth.Add(entry);
  //  }
  //  else
  //  {
  //    switch (random.Next(3))
  //    {
  //    case 0:
  //      vector.Add(entry);
  //      truth.Add(entry);
  //      break;
  //    case 1:
  //    {
  //      var index = random.Next(vector.Count + 1);
  //      vector.InsertAt(index, entry);
  //      truth.Insert(index, entry);
  //    }
  //    break;
  //    case 2:
  //    {
  //      var index = random.Next(vector.Count());
  //      vector.RemoveAt(index);
  //      truth.RemoveAt(index);
  //    }
  //    break;
  //    default:
  //      break;
  //    }
  //  }
  //  EXPECT_TRUE(vector.DEBUG_IsValid());
  //  EXPECT_EQ(truth.Count, vector.Count());
  //  for (int testIndex = 0; testIndex < truth.Count; ++testIndex)
  //  {
  //    EXPECT_EQ(truth[testIndex], vector[testIndex]);
  //  }
  //}
}
