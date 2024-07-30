/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/Collections/VersionedHandleVector.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <fmt/format.h>
#include <array>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

using namespace Fsl;

namespace
{
  using Test_VersionedHandleVector0 = TestFixtureFslBase;
}


TEST(Test_VersionedHandleVector0, Add)
{
  VersionedHandleVector<std::string> vector(10);
  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());

  const std::string val1("A");
  const std::string val2("B");
  const std::string val3("C");

  const auto h1 = vector.Add(val1);
  const auto h2 = vector.Add(val2);
  const auto h3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
  EXPECT_EQ(val1, vector.Get(h1));
  EXPECT_EQ(val2, vector.Get(h2));
  EXPECT_EQ(val3, vector.Get(h3));

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, Add_Grow)
{
  VersionedHandleVector<std::string> vector(1);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  const auto h1 = vector.Add(val1);
  EXPECT_EQ(1u, vector.Count());
  const auto h2 = vector.Add(val2);
  EXPECT_EQ(2u, vector.Count());
  const auto h3 = vector.Add(val3);
  EXPECT_EQ(3u, vector.Count());
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
  EXPECT_EQ(val1, vector.Get(h1));
  EXPECT_EQ(val2, vector.Get(h2));
  EXPECT_EQ(val3, vector.Get(h3));

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, Insert_InvalidHandle)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";

  const auto h1 = vector.Add(val1);
  const auto h2 = vector.Add(val2);
  const auto h3 = vector.Add(val3);

  EXPECT_THROW(vector.Insert(std::numeric_limits<VersionedHandleVector<std::string>::handle_type>::max(), val4), std::invalid_argument);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);
  EXPECT_EQ(val1, vector.Get(h1));
  EXPECT_EQ(val2, vector.Get(h2));
  EXPECT_EQ(val3, vector.Get(h3));

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, Insert_0)
{
  VersionedHandleVector<std::string> vector(10);

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

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, Insert_1)
{
  VersionedHandleVector<std::string> vector(10);

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

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, Insert_2)
{
  VersionedHandleVector<std::string> vector(10);

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

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, InsertAt_4)
{
  VersionedHandleVector<std::string> vector(10);

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

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, InsertAt_0)
{
  VersionedHandleVector<std::string> vector(10);

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

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, InsertAt_1)
{
  VersionedHandleVector<std::string> vector(10);

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

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, InsertAt_2)
{
  VersionedHandleVector<std::string> vector(10);

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

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, InsertAt_3)
{
  VersionedHandleVector<std::string> vector(10);

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

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, InsertAt_4_Grow)
{
  VersionedHandleVector<std::string> vector(3);

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

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}


//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, InsertAt_0_Grow)
{
  VersionedHandleVector<std::string> vector(3);

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

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, InsertAt_1_Grow)
{
  VersionedHandleVector<std::string> vector(3);

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

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, InsertAt_2_Grow)
{
  VersionedHandleVector<std::string> vector(3);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";

  const auto h1 = vector.Add(val1);
  const auto h2 = vector.Add(val2);
  const auto h3 = vector.Add(val3);

  const auto h4 = vector.InsertAt(2, val4);
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

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, InsertAt_3_Grow)
{
  VersionedHandleVector<std::string> vector(3);

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

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, Capacity)
{
  VersionedHandleVector<std::string> vector(10);

  EXPECT_EQ(10u, vector.Capacity());

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, Count)
{
  VersionedHandleVector<std::string> vector(10);
  vector.Add("A");
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(1u, vector.Count());
  vector.Add("B");
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  vector.Add("C");
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, Clear_true)
{
  VersionedHandleVector<std::string> vector(10);
  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  const auto h1 = vector.Add(val1);
  const auto h2 = vector.Add(val2);
  const auto h3 = vector.Add(val3);

  EXPECT_EQ(10u, vector.Capacity());
  EXPECT_EQ(3u, vector.Count());

  vector.Clear(true);

  EXPECT_EQ(10u, vector.Capacity());
  EXPECT_EQ(0u, vector.Count());

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());

  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(h1));
  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(h2));
  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(h3));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, Clear_false)
{
  VersionedHandleVector<std::string> vector(10);
  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  const auto h1 = vector.Add(val1);
  const auto h2 = vector.Add(val2);
  const auto h3 = vector.Add(val3);

  EXPECT_EQ(10u, vector.Capacity());
  EXPECT_EQ(3u, vector.Count());

  vector.Clear(false);

  EXPECT_EQ(10u, vector.Capacity());
  EXPECT_EQ(0u, vector.Count());

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());

  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(h1));
  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(h2));
  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(h3));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, OpIndex)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  vector.Add(val1);
  vector.Add(val2);
  vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

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

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, OpIndex_Ref)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";

  vector.Add(val1);

  EXPECT_EQ(val1, vector[0]);
  EXPECT_TRUE(vector.DEBUG_IsValid());

  auto& rIndex0 = vector[0];
  rIndex0 = val2;

  EXPECT_EQ(val2, vector[0]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, At_OutOfBounds)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  vector.Add(val1);
  vector.Add(val2);
  vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  EXPECT_THROW(vector.At(3), IndexOutOfRangeException);
  EXPECT_THROW(vector.At(-1), IndexOutOfRangeException);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, At)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  vector.Add(val1);
  vector.Add(val2);
  vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());
  EXPECT_EQ(val1, vector.At(0));
  EXPECT_EQ(val2, vector.At(1));
  EXPECT_EQ(val3, vector.At(2));

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, SetAt)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  vector.Add(val1);
  vector.Add(val2);
  vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

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

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, Get)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

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

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, Get_Ref)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";

  auto item1 = vector.Add(val1);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(1u, vector.Count());
  EXPECT_EQ(val1, vector.Get(item1));

  auto& rItem1 = vector.Get(item1);
  rItem1 = val2;

  EXPECT_EQ(val2, vector.Get(item1));

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, Remove_First)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.Remove(item1);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  ASSERT_TRUE(vector.DEBUG_CheckVersionBumped(item1));

  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val2, vector[0]);
  EXPECT_EQ(val3, vector[1]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, Remove_Middle)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.Remove(item2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  ASSERT_TRUE(vector.DEBUG_CheckVersionBumped(item2));

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val3, vector[1]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, Remove_Last)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.Remove(item3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  ASSERT_TRUE(vector.DEBUG_CheckVersionBumped(item3));

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, Remove_LastAtCapacity)
{
  VersionedHandleVector<std::string> vector(3);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.Remove(item3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  ASSERT_TRUE(vector.DEBUG_CheckVersionBumped(item3));

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);

  EXPECT_EQ(3u, vector.Capacity());

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveAt_First)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.RemoveAt(0);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  ASSERT_TRUE(vector.DEBUG_CheckVersionBumped(item1));

  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val2, vector[0]);
  EXPECT_EQ(val3, vector[1]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveAt_Middle)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.RemoveAt(1);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  ASSERT_TRUE(vector.DEBUG_CheckVersionBumped(item2));

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val3, vector[1]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveAt_Last)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.RemoveAt(2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  ASSERT_TRUE(vector.DEBUG_CheckVersionBumped(item3));

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveBySwap_First)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.RemoveBySwap(item1);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  ASSERT_TRUE(vector.DEBUG_CheckVersionBumped(item1));

  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  // The remove fast command swaps the removed element with the last element
  EXPECT_EQ(val3, vector[0]);
  EXPECT_EQ(val2, vector[1]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveBySwap_Middle)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.RemoveBySwap(item2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  ASSERT_TRUE(vector.DEBUG_CheckVersionBumped(item2));

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val3, vector.Get(item3));

  // The remove fast command swaps the removed element with the last element
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val3, vector[1]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, FOO)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.RemoveBySwap(item3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  ASSERT_TRUE(vector.DEBUG_CheckVersionBumped(item3));

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));

  // The remove fast command swaps the removed element with the last element
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveBySwap_LastAtCapacity)
{
  VersionedHandleVector<std::string> vector(3);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.RemoveBySwap(item3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  ASSERT_TRUE(vector.DEBUG_CheckVersionBumped(item3));

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));

  // The remove fast command swaps the removed element with the last element
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);

  EXPECT_EQ(3u, vector.Capacity());

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveBySwapAt_First)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.RemoveBySwapAt(0);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  ASSERT_TRUE(vector.DEBUG_CheckVersionBumped(item1));

  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  // The remove fast command swaps the removed element with the last element
  EXPECT_EQ(val3, vector[0]);
  EXPECT_EQ(val2, vector[1]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveBySwapAt_Middle)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.RemoveBySwapAt(1);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  ASSERT_TRUE(vector.DEBUG_CheckVersionBumped(item2));

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val3, vector.Get(item3));

  // The remove fast command swaps the removed element with the last element
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val3, vector[1]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveBySwapAt_Last)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.RemoveBySwapAt(2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  ASSERT_TRUE(vector.DEBUG_CheckVersionBumped(item3));

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));

  // The remove fast command swaps the removed element with the last element
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveBySwapAt_LastAtCapacity)
{
  VersionedHandleVector<std::string> vector(3);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.RemoveBySwapAt(2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  ASSERT_TRUE(vector.DEBUG_CheckVersionBumped(item3));

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));

  // The remove fast command swaps the removed element with the last element
  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);

  EXPECT_EQ(3u, vector.Capacity());

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveRange_3_0_0)
{
  VersionedHandleVector<std::string> vector(10);

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

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveRange_3_0_1)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);

  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.RemoveRange(0, 1);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(item1));

  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val2, vector[0]);
  EXPECT_EQ(val3, vector[1]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveRange_3_1_1)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.RemoveRange(1, 1);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(item2));

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val3, vector[1]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveRange_3_2_1)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());

  vector.RemoveRange(2, 1);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(item3));

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveRange_4_OutOfBoundsStartIndex)
{
  VersionedHandleVector<std::string> vector(10);

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

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveRange_4_OutOfBoundsLength)
{
  VersionedHandleVector<std::string> vector(10);

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

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveRange_4_0_2)
{
  VersionedHandleVector<std::string> vector(10);

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

  vector.RemoveRange(0, 2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(item1));
  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(item2));

  EXPECT_EQ(val3, vector.Get(item3));
  EXPECT_EQ(val4, vector.Get(item4));

  EXPECT_EQ(val3, vector[0]);
  EXPECT_EQ(val4, vector[1]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveRange_4_1_2)
{
  VersionedHandleVector<std::string> vector(10);

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

  vector.RemoveRange(1, 2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(item2));
  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(item3));

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val4, vector.Get(item4));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val4, vector[1]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveRange_4_2_2)
{
  VersionedHandleVector<std::string> vector(10);

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

  vector.RemoveRange(2, 2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(2u, vector.Count());
  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(item3));
  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(item4));

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveRange_5_0_2)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";
  const std::string val5 = "E";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  auto item4 = vector.Add(val4);
  auto item5 = vector.Add(val5);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(5u, vector.Count());

  vector.RemoveRange(0, 2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());
  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(item1));
  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(item2));

  EXPECT_EQ(val3, vector.Get(item3));
  EXPECT_EQ(val4, vector.Get(item4));
  EXPECT_EQ(val5, vector.Get(item5));

  EXPECT_EQ(val3, vector[0]);
  EXPECT_EQ(val4, vector[1]);
  EXPECT_EQ(val5, vector[2]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveRange_5_1_2)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";
  const std::string val5 = "E";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  auto item4 = vector.Add(val4);
  auto item5 = vector.Add(val5);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(5u, vector.Count());

  vector.RemoveRange(1, 2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());
  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(item2));
  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(item3));

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val4, vector.Get(item4));
  EXPECT_EQ(val5, vector.Get(item5));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val4, vector[1]);
  EXPECT_EQ(val5, vector[2]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveRange_5_2_2)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";
  const std::string val5 = "E";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  auto item4 = vector.Add(val4);
  auto item5 = vector.Add(val5);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(5u, vector.Count());

  vector.RemoveRange(2, 2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());
  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(item3));
  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(item4));

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val5, vector.Get(item5));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val5, vector[2]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_VersionedHandleVector0, RemoveRange_5_3_2)
{
  VersionedHandleVector<std::string> vector(10);

  const std::string val1 = "A";
  const std::string val2 = "B";
  const std::string val3 = "C";
  const std::string val4 = "D";
  const std::string val5 = "E";

  auto item1 = vector.Add(val1);
  auto item2 = vector.Add(val2);
  auto item3 = vector.Add(val3);
  auto item4 = vector.Add(val4);
  auto item5 = vector.Add(val5);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(5u, vector.Count());

  vector.RemoveRange(3, 2);
  EXPECT_TRUE(vector.DEBUG_IsValid());
  EXPECT_EQ(3u, vector.Count());
  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(item4));
  EXPECT_TRUE(vector.DEBUG_CheckVersionBumped(item5));

  EXPECT_EQ(val1, vector.Get(item1));
  EXPECT_EQ(val2, vector.Get(item2));
  EXPECT_EQ(val3, vector.Get(item3));

  EXPECT_EQ(val1, vector[0]);
  EXPECT_EQ(val2, vector[1]);
  EXPECT_EQ(val3, vector[2]);

  ASSERT_NO_THROW(vector.DEBUG_SanityCheck());
}
