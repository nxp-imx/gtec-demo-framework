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
#include <vector>
#include <utility>

using namespace Fsl;

namespace
{
  namespace LocalConfig
  {
    // constexpr auto InvalidHandle0 = HandleVectorConfig::InvalidHandle;
    // constexpr auto InvalidHandle1 = std::numeric_limits<HandleVector<std::string>::handle_type>::max();
  }


  using TestCollections_HandleVector0 = TestFixtureFslBase;
}


TEST(TestCollections_HandleVector0, Add)
{
  HandleVector<std::string> vector(10);

  const std::string val1("A");
  const std::string val2("B");
  const std::string val3("C");

  const auto h1 = vector.Add(val1);
  const auto h2 = vector.Add(val2);
  const auto h3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
  EXPECT_EQ(val1, vector.Get(h1));
  EXPECT_EQ(val2, vector.Get(h2));
  EXPECT_EQ(val3, vector.Get(h3));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, Add_Grow)
{
  HandleVector<std::string> vector(1);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  const auto h1 = vector.Add(val1);
  EXPECT_EQ(1, vector.Count());
  const auto h2 = vector.Add(val2);
  EXPECT_EQ(2, vector.Count());
  const auto h3 = vector.Add(val3);
  EXPECT_EQ(3, vector.Count());
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
  EXPECT_EQ(val1, vector.Get(h1));
  EXPECT_EQ(val2, vector.Get(h2));
  EXPECT_EQ(val3, vector.Get(h3));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, Insert_InvalidHandle)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";

  const auto h1 = vector.Add(val1);
  const auto h2 = vector.Add(val2);
  const auto h3 = vector.Add(val3);

  EXPECT_THROW(vector.Insert(std::numeric_limits<HandleVector<std::string>::handle_type>::max(), val4), std::invalid_argument);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
  EXPECT_EQ(val1, vector.Get(h1));
  EXPECT_EQ(val2, vector.Get(h2));
  EXPECT_EQ(val3, vector.Get(h3));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, Insert_0)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";

  const auto h1 = vector.Add(val1);
  const auto h2 = vector.Add(val2);
  const auto h3 = vector.Add(val3);

  const auto h4 = vector.Insert(h1, val4);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(4u, vector.Count());
  EXPECT_EQ(val4, vector[0]);
  EXPECT_EQ(val1, vector[1]);
  EXPECT_EQ(val2, vector[2]);
  EXPECT_EQ(val3, vector[3]);
  EXPECT_EQ(val1, vector.Get(h1));
  EXPECT_EQ(val2, vector.Get(h2));
  EXPECT_EQ(val3, vector.Get(h3));
  EXPECT_EQ(val4, vector.Get(h4));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, Insert_1)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";

  const auto h1 = vector.Add(val1);
  const auto h2 = vector.Add(val2);
  const auto h3 = vector.Add(val3);

  const auto h4 = vector.Insert(h2, val4);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(4u, vector.Count());
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val4, vector[1]);
  EXPECT_EQ(val2, vector[2]);
  EXPECT_EQ(val3, vector[3]);
  EXPECT_EQ(val1, vector.Get(h1));
  EXPECT_EQ(val2, vector.Get(h2));
  EXPECT_EQ(val3, vector.Get(h3));
  EXPECT_EQ(val4, vector.Get(h4));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, Insert_2)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";

  const auto h1 = vector.Add(val1);
  const auto h2 = vector.Add(val2);
  const auto h3 = vector.Add(val3);

  const auto h4 = vector.Insert(h3, val4);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(4u, vector.Count());
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val4, vector[2]);
  EXPECT_EQ(val3, vector[3]);
  EXPECT_EQ(val1, vector.Get(h1));
  EXPECT_EQ(val2, vector.Get(h2));
  EXPECT_EQ(val3, vector.Get(h3));
  EXPECT_EQ(val4, vector.Get(h4));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, InsertAt_4)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";

  const auto h1 = vector.Add(val1);
  const auto h2 = vector.Add(val2);
  const auto h3 = vector.Add(val3);

  EXPECT_THROW(vector.InsertAt(4, val4), IndexOutOfRangeException);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
  EXPECT_EQ(val1, vector.Get(h1));
  EXPECT_EQ(val2, vector.Get(h2));
  EXPECT_EQ(val3, vector.Get(h3));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, InsertAt_0)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";

  const auto h1 = vector.Add(val1);
  const auto h2 = vector.Add(val2);
  const auto h3 = vector.Add(val3);

  const auto h4 = vector.InsertAt(0, val4);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(4u, vector.Count());
  EXPECT_EQ(val4, vector[0]);
  EXPECT_EQ(val1, vector[1]);
  EXPECT_EQ(val2, vector[2]);
  EXPECT_EQ(val3, vector[3]);
  EXPECT_EQ(val1, vector.Get(h1));
  EXPECT_EQ(val2, vector.Get(h2));
  EXPECT_EQ(val3, vector.Get(h3));
  EXPECT_EQ(val4, vector.Get(h4));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, InsertAt_1)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";

  const auto h1 = vector.Add(val1);
  const auto h2 = vector.Add(val2);
  const auto h3 = vector.Add(val3);

  const auto h4 = vector.InsertAt(1, val4);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(4u, vector.Count());
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val4, vector[1]);
  EXPECT_EQ(val2, vector[2]);
  EXPECT_EQ(val3, vector[3]);
  EXPECT_EQ(val1, vector.Get(h1));
  EXPECT_EQ(val2, vector.Get(h2));
  EXPECT_EQ(val3, vector.Get(h3));
  EXPECT_EQ(val4, vector.Get(h4));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, InsertAt_2)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";

  const auto h1 = vector.Add(val1);
  const auto h2 = vector.Add(val2);
  const auto h3 = vector.Add(val3);

  const auto h4 = vector.InsertAt(1, val4);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(4u, vector.Count());
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val4, vector[1]);
  EXPECT_EQ(val2, vector[2]);
  EXPECT_EQ(val3, vector[3]);
  EXPECT_EQ(val1, vector.Get(h1));
  EXPECT_EQ(val2, vector.Get(h2));
  EXPECT_EQ(val3, vector.Get(h3));
  EXPECT_EQ(val4, vector.Get(h4));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, InsertAt_3)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";

  const auto h1 = vector.Add(val1);
  const auto h2 = vector.Add(val2);
  const auto h3 = vector.Add(val3);

  const auto h4 = vector.InsertAt(3, val4);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(4u, vector.Count());
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
  EXPECT_EQ(val4, vector[3]);
  EXPECT_EQ(val1, vector.Get(h1));
  EXPECT_EQ(val2, vector.Get(h2));
  EXPECT_EQ(val3, vector.Get(h3));
  EXPECT_EQ(val4, vector.Get(h4));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, InsertAt_4_Grow)
{
  HandleVector<std::string> vector(3);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";

  const auto h1 = vector.Add(val1);
  const auto h2 = vector.Add(val2);
  const auto h3 = vector.Add(val3);

  EXPECT_THROW(vector.InsertAt(4, val4), IndexOutOfRangeException);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
  EXPECT_EQ(val1, vector.Get(h1));
  EXPECT_EQ(val2, vector.Get(h2));
  EXPECT_EQ(val3, vector.Get(h3));
}


//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, InsertAt_0_Grow)
{
  HandleVector<std::string> vector(3);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";

  const auto h1 = vector.Add(val1);
  const auto h2 = vector.Add(val2);
  const auto h3 = vector.Add(val3);

  const auto h4 = vector.InsertAt(0, val4);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(4, vector.Count());
  EXPECT_EQ(val4, vector[0]);
  EXPECT_EQ(val1, vector[1]);
  EXPECT_EQ(val2, vector[2]);
  EXPECT_EQ(val3, vector[3]);
  EXPECT_EQ(val1, vector.Get(h1));
  EXPECT_EQ(val2, vector.Get(h2));
  EXPECT_EQ(val3, vector.Get(h3));
  EXPECT_EQ(val4, vector.Get(h4));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, InsertAt_1_Grow)
{
  HandleVector<std::string> vector(3);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";

  const auto h1 = vector.Add(val1);
  const auto h2 = vector.Add(val2);
  const auto h3 = vector.Add(val3);

  const auto h4 = vector.InsertAt(1, val4);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(4, vector.Count());
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val4, vector[1]);
  EXPECT_EQ(val2, vector[2]);
  EXPECT_EQ(val3, vector[3]);
  EXPECT_EQ(val1, vector.Get(h1));
  EXPECT_EQ(val2, vector.Get(h2));
  EXPECT_EQ(val3, vector.Get(h3));
  EXPECT_EQ(val4, vector.Get(h4));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, InsertAt_2_Grow)
{
  HandleVector<std::string> vector(3);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";

  const auto h1 = vector.Add(val1);
  const auto h2 = vector.Add(val2);
  const auto h3 = vector.Add(val3);

  const auto h4 = vector.InsertAt(2, val4);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(4, vector.Count());
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val4, vector[2]);
  EXPECT_EQ(val3, vector[3]);
  EXPECT_EQ(val1, vector.Get(h1));
  EXPECT_EQ(val2, vector.Get(h2));
  EXPECT_EQ(val3, vector.Get(h3));
  EXPECT_EQ(val4, vector.Get(h4));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, InsertAt_3_Grow)
{
  HandleVector<std::string> vector(3);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";

  const auto h1 = vector.Add(val1);
  const auto h2 = vector.Add(val2);
  const auto h3 = vector.Add(val3);

  const auto h4 = vector.InsertAt(3, val4);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(4, vector.Count());
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
  EXPECT_EQ(val4, vector[3]);
  EXPECT_EQ(val1, vector.Get(h1));
  EXPECT_EQ(val2, vector.Get(h2));
  EXPECT_EQ(val3, vector.Get(h3));
  EXPECT_EQ(val4, vector.Get(h4));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, Capacity)
{
  HandleVector<std::string> vector(10);

  EXPECT_EQ(10, vector.Capacity());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, Count)
{
  HandleVector<std::string> vector(10);
  vector.Add("A");
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(1, vector.Count());
  vector.Add("B");
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2, vector.Count());
  vector.Add("C");
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, Clear)
{
  HandleVector<std::string> vector(10);
  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  vector.Add(val1);
  vector.Add(val2);
  vector.Add(val3);

  EXPECT_EQ(10, vector.Capacity());
  EXPECT_EQ(3, vector.Count());

  vector.Clear();

  EXPECT_EQ(10, vector.Capacity());
  EXPECT_EQ(0, vector.Count());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, OpIndex)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  vector.Add(val1);
  vector.Add(val2);
  vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
  EXPECT_TRUE(vector.DEBUG_IsValid());

  const std::string val4 = "D";
  const std::string val5 = "E";
  const std::string val6 = "F";
  vector[0] = val4;
  vector[1] = val5;
  vector[2] = val6;
  EXPECT_TRUE(vector.DEBUG_IsValid());

  const auto& vector2 = vector;
  EXPECT_EQ(val4, vector2[0]);
  EXPECT_EQ(val5, vector2[1]);
  EXPECT_EQ(val6, vector2[2]);
  EXPECT_TRUE(vector.DEBUG_IsValid());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, OpIndex_Ref)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";

  vector.Add(val1);

  EXPECT_EQ(val1, vector[0]);
  EXPECT_TRUE(vector.DEBUG_IsValid());

  auto& rIndex0 = vector[0];
  rIndex0 = val2;

  EXPECT_EQ(val2, vector[0]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, At_OutOfBounds)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  vector.Add(val1);
  vector.Add(val2);
  vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());

  EXPECT_THROW(vector.At(3), IndexOutOfRangeException);
  EXPECT_THROW(vector.At(-1), IndexOutOfRangeException);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, At)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  vector.Add(val1);
  vector.Add(val2);
  vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());
  EXPECT_EQ(val1, vector.At(0));
  EXPECT_EQ(val2, vector.At(1));
  EXPECT_EQ(val3, vector.At(2));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, SetAt)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  vector.Add(val1);
  vector.Add(val2);
  vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());

  EXPECT_EQ(val1, vector.At(0));
  EXPECT_EQ(val2, vector.At(1));
  EXPECT_EQ(val3, vector.At(2));
  EXPECT_TRUE(vector.DEBUG_IsValid());

  const std::string val4 = "D";
  const std::string val5 = "E";
  const std::string val6 = "F";
  vector.SetAt(0, val4);
  vector.SetAt(1, val5);
  vector.SetAt(2, val6);
  EXPECT_TRUE(vector.DEBUG_IsValid());

  EXPECT_EQ(val4, vector.At(0));
  EXPECT_EQ(val5, vector.At(1));
  EXPECT_EQ(val6, vector.At(2));
  EXPECT_TRUE(vector.DEBUG_IsValid());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, Get)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));
  EXPECT_TRUE(vector.DEBUG_IsValid());

  const std::string val4 = "D";
  const std::string val5 = "E";
  const std::string val6 = "F";
  vector.Set(item1, val4);
  vector.Set(item2, val5);
  vector.Set(item3, val6);
  EXPECT_TRUE(vector.DEBUG_IsValid());

  const auto& vector2 = vector;
  EXPECT_EQ(val4, vector2.Get(item1));
  EXPECT_EQ(val5, vector2.Get(item2));
  EXPECT_EQ(val6, vector2.Get(item3));
  EXPECT_TRUE(vector2.DEBUG_IsValid());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, Get_Ref)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";

  auto item1 = vector.Add(val1);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(1, vector.Count());
  EXPECT_EQ(val1, vector.Get(item1));

  auto& rItem1 = vector.Get(item1);
  rItem1 = val2;

  EXPECT_EQ(val2, vector.Get(item1));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, Remove_First)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());

  vector.Remove(item1);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2, vector.Count());

  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val2, vector[0]);
  EXPECT_EQ(val3, vector[1]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, Remove_Middle)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());

  vector.Remove(item2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val3, vector[1]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, Remove_Last)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());

  vector.Remove(item3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, Remove_LastAtCapacity)
{
  HandleVector<std::string> vector(3);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());

  vector.Remove(item3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);

  EXPECT_EQ(3, vector.Capacity());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, RemoveAt_First)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());

  vector.RemoveAt(0);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2, vector.Count());

  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val2, vector[0]);
  EXPECT_EQ(val3, vector[1]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, RemoveAt_Middle)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());

  vector.RemoveAt(1);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val3, vector[1]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, RemoveAt_Last)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());

  vector.RemoveAt(2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, RemoveFast_First)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());

  vector.RemoveFast(item1);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2, vector.Count());

  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  // The remove fast command swaps the removed element with the last element
  EXPECT_EQ(val3, vector[0]);
  EXPECT_EQ(val2, vector[1]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, RemoveFast_Middle)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());

  vector.RemoveFast(item2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val3, vector.Get(item3));

  // The remove fast command swaps the removed element with the last element
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val3, vector[1]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, FOO)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());

  vector.RemoveFast(item3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));

  // The remove fast command swaps the removed element with the last element
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, RemoveFast_LastAtCapacity)
{
  HandleVector<std::string> vector(3);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());

  vector.RemoveFast(item3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));

  // The remove fast command swaps the removed element with the last element
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);

  EXPECT_EQ(3, vector.Capacity());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, RemoveFastAt_First)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());

  vector.RemoveFastAt(0);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2, vector.Count());

  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  // The remove fast command swaps the removed element with the last element
  EXPECT_EQ(val3, vector[0]);
  EXPECT_EQ(val2, vector[1]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, RemoveFastAt_Middle)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());

  vector.RemoveFastAt(1);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val3, vector.Get(item3));

  // The remove fast command swaps the removed element with the last element
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val3, vector[1]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, RemoveFastAt_Last)
{
  HandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());

  vector.RemoveFastAt(2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));

  // The remove fast command swaps the removed element with the last element
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_HandleVector0, RemoveFastAt_LastAtCapacity)
{
  HandleVector<std::string> vector(3);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3, vector.Count());

  vector.RemoveFastAt(2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2, vector.Count());

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));

  // The remove fast command swaps the removed element with the last element
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);

  EXPECT_EQ(3, vector.Capacity());
}
