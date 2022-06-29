/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

#include <FslBase/Collections/TightHostedVector.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestCollections_TightHostedVector = TestFixtureFslBase;

  namespace HostedVectorIndex
  {
    constexpr std::size_t First = 0;
    constexpr std::size_t Second = 1;
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Construct
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_TightHostedVector, Construct)
{
  TightHostedVector<uint32_t, 2> buffer;

  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::Second));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ClearAll
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_TightHostedVector, ClearAllH0H0)
{
  TightHostedVector<uint32_t, 2> buffer;

  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::Second));

  buffer.ClearAll();

  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::Second));
}


TEST(TestCollections_TightHostedVector, ClearAll_H2H3)
{
  TightHostedVector<uint32_t, 2> buffer;
  std::array<uint32_t, 2> test0 = {42, 1337};
  std::array<uint32_t, 3> test1 = {1, 2, 3};
  for (const auto value : test0)
  {
    buffer.PushBack(HostedVectorIndex::First, value);
  }
  for (const auto value : test1)
  {
    buffer.PushBack(HostedVectorIndex::Second, value);
  }

  ASSERT_EQ(2u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(3u, buffer.Size(HostedVectorIndex::Second));

  buffer.ClearAll();

  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::Second));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Clear hosted
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_TightHostedVector, Clear_0_H0H0)
{
  TightHostedVector<uint32_t, 2> buffer;

  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::Second));

  buffer.Clear(HostedVectorIndex::First);

  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::Second));
}

TEST(TestCollections_TightHostedVector, Clear_1_H0H0)
{
  TightHostedVector<uint32_t, 2> buffer;

  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::Second));

  buffer.Clear(HostedVectorIndex::Second);

  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::Second));
}

TEST(TestCollections_TightHostedVector, Clear_0_H2H3)
{
  TightHostedVector<uint32_t, 2> buffer;
  std::array<uint32_t, 2> test0 = {42, 1337};
  std::array<uint32_t, 3> test1 = {1, 2, 3};
  for (const auto value : test0)
  {
    buffer.PushBack(HostedVectorIndex::First, value);
  }
  for (const auto value : test1)
  {
    buffer.PushBack(HostedVectorIndex::Second, value);
  }

  ASSERT_EQ(2u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(3u, buffer.Size(HostedVectorIndex::Second));

  buffer.Clear(HostedVectorIndex::First);

  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(3u, buffer.Size(HostedVectorIndex::Second));
}

TEST(TestCollections_TightHostedVector, Clear_1_H2H3)
{
  TightHostedVector<uint32_t, 2> buffer;
  std::array<uint32_t, 2> test0 = {42, 1337};
  std::array<uint32_t, 3> test1 = {1, 2, 3};
  for (const auto value : test0)
  {
    buffer.PushBack(HostedVectorIndex::First, value);
  }
  for (const auto value : test1)
  {
    buffer.PushBack(HostedVectorIndex::Second, value);
  }

  ASSERT_EQ(2u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(3u, buffer.Size(HostedVectorIndex::Second));

  buffer.Clear(HostedVectorIndex::Second);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(2u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::Second));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// PushBack
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_TightHostedVector, PushBack_To0_H0H0)
{
  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t value0 = 42;
  buffer.PushBack(HostedVectorIndex::First, value0);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(value0, buffer.UncheckedAt(HostedVectorIndex::First, 0));
}


TEST(TestCollections_TightHostedVector, PushBack_To1_H0H0)
{
  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t value0 = 42;
  buffer.PushBack(HostedVectorIndex::Second, value0);

  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(value0, buffer.UncheckedAt(HostedVectorIndex::Second, 0));
}


TEST(TestCollections_TightHostedVector, PushBack_To0_H0H1)
{
  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueA = 1337;
  constexpr uint32_t valueB = 42;
  // Ensure we have one value in the back array
  buffer.PushBack(HostedVectorIndex::Second, valueB);
  // Push to the first array
  buffer.PushBack(HostedVectorIndex::First, valueA);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(valueA, buffer.UncheckedAt(HostedVectorIndex::First, 0));
  ASSERT_EQ(valueB, buffer.UncheckedAt(HostedVectorIndex::Second, 0));
}

TEST(TestCollections_TightHostedVector, PushBack_To1_H1H0)
{
  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueA = 1337;
  constexpr uint32_t valueB = 42;
  // Ensure we have one value in the first array
  buffer.PushBack(HostedVectorIndex::First, valueA);
  // Push to the last array
  buffer.PushBack(HostedVectorIndex::Second, valueB);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(valueA, buffer.UncheckedAt(HostedVectorIndex::First, 0));
  ASSERT_EQ(valueB, buffer.UncheckedAt(HostedVectorIndex::Second, 0));
}


TEST(TestCollections_TightHostedVector, PushBack_Push1Push1Push0)
{
  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueA = 1337;
  constexpr uint32_t valueB = 42;
  constexpr uint32_t valueC = 31;
  buffer.PushBack(HostedVectorIndex::Second, valueB);
  buffer.PushBack(HostedVectorIndex::Second, valueC);
  // Push to the first array
  buffer.PushBack(HostedVectorIndex::First, valueA);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(2u, buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(valueA, buffer.UncheckedAt(HostedVectorIndex::First, 0));
  ASSERT_EQ(valueB, buffer.UncheckedAt(HostedVectorIndex::Second, 0));
  ASSERT_EQ(valueC, buffer.UncheckedAt(HostedVectorIndex::Second, 1));
}

TEST(TestCollections_TightHostedVector, PushBack_Push0Push0Push1)
{
  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueA = 1337;
  constexpr uint32_t valueB = 42;
  constexpr uint32_t valueC = 31;
  buffer.PushBack(HostedVectorIndex::First, valueA);
  buffer.PushBack(HostedVectorIndex::First, valueB);
  // Push to the first array
  buffer.PushBack(HostedVectorIndex::Second, valueC);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(2u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(valueA, buffer.UncheckedAt(HostedVectorIndex::First, 0));
  ASSERT_EQ(valueB, buffer.UncheckedAt(HostedVectorIndex::First, 1));
  ASSERT_EQ(valueC, buffer.UncheckedAt(HostedVectorIndex::Second, 0));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// PushBack ReadOnlySpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_TightHostedVector, PushBack_EmptySpan_To0_H0H0)
{
  const std::array<uint32_t, 3> newValues = {};
  const auto newValueSpan = ReadOnlySpanUtil::AsSpan(newValues);
  TightHostedVector<uint32_t, 2> buffer;

  buffer.PushBack(HostedVectorIndex::First, newValueSpan);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(newValueSpan.size(), buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::Second));

  EXPECT_EQ(newValueSpan, buffer.AsReadOnlySpan(HostedVectorIndex::First));
}


TEST(TestCollections_TightHostedVector, PushBack_EmptySpan_To1_H0H0)
{
  const std::array<uint32_t, 3> newValues = {};
  const auto newValueSpan = ReadOnlySpanUtil::AsSpan(newValues);
  TightHostedVector<uint32_t, 2> buffer;

  buffer.PushBack(HostedVectorIndex::Second, newValueSpan);

  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(newValueSpan.size(), buffer.Size(HostedVectorIndex::Second));

  EXPECT_EQ(newValueSpan, buffer.AsReadOnlySpan(HostedVectorIndex::Second));
}


TEST(TestCollections_TightHostedVector, PushBack_EmptySpan_To0_H0H1)
{
  const std::array<uint32_t, 3> newValues = {};
  const auto newValueSpan = ReadOnlySpanUtil::AsSpan(newValues);

  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueB = 42;
  // Ensure we have one value in the back array
  buffer.PushBack(HostedVectorIndex::Second, valueB);
  // Push to the first array
  buffer.PushBack(HostedVectorIndex::First, newValueSpan);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(newValueSpan.size(), buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::Second));

  EXPECT_EQ(newValueSpan, buffer.AsReadOnlySpan(HostedVectorIndex::First));
  ASSERT_EQ(valueB, buffer.UncheckedAt(HostedVectorIndex::Second, 0));
}

TEST(TestCollections_TightHostedVector, PushBack_EmptySpan_To1_H1H0)
{
  const std::array<uint32_t, 3> newValues = {};
  const auto newValueSpan = ReadOnlySpanUtil::AsSpan(newValues);

  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueA = 1337;
  // Ensure we have one value in the first array
  buffer.PushBack(HostedVectorIndex::First, valueA);
  // Push to the last array
  buffer.PushBack(HostedVectorIndex::Second, newValueSpan);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(newValueSpan.size(), buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(valueA, buffer.UncheckedAt(HostedVectorIndex::First, 0));
  EXPECT_EQ(newValueSpan, buffer.AsReadOnlySpan(HostedVectorIndex::Second));
}


TEST(TestCollections_TightHostedVector, PushBack_EmptySpan_Push1Push1Push0)
{
  const std::array<uint32_t, 3> newValues = {};
  const auto newValueSpan = ReadOnlySpanUtil::AsSpan(newValues);

  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueB = 42;
  constexpr uint32_t valueC = 31;
  buffer.PushBack(HostedVectorIndex::Second, valueB);
  buffer.PushBack(HostedVectorIndex::Second, valueC);
  // Push to the first array
  buffer.PushBack(HostedVectorIndex::First, newValueSpan);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(newValueSpan.size(), buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(2u, buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(newValueSpan, buffer.AsReadOnlySpan(HostedVectorIndex::First));
  ASSERT_EQ(valueB, buffer.UncheckedAt(HostedVectorIndex::Second, 0));
  ASSERT_EQ(valueC, buffer.UncheckedAt(HostedVectorIndex::Second, 1));
}

TEST(TestCollections_TightHostedVector, PushBack_EmptySpan_Push0Push0Push1)
{
  const std::array<uint32_t, 3> newValues = {};
  const auto newValueSpan = ReadOnlySpanUtil::AsSpan(newValues);

  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueA = 1337;
  constexpr uint32_t valueB = 42;
  buffer.PushBack(HostedVectorIndex::First, valueA);
  buffer.PushBack(HostedVectorIndex::First, valueB);
  // Push to the first array
  buffer.PushBack(HostedVectorIndex::Second, newValueSpan);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(2u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(newValueSpan.size(), buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(valueA, buffer.UncheckedAt(HostedVectorIndex::First, 0));
  ASSERT_EQ(valueB, buffer.UncheckedAt(HostedVectorIndex::First, 1));
  ASSERT_EQ(newValueSpan, buffer.AsReadOnlySpan(HostedVectorIndex::Second));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// PushBack ReadOnlySpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_TightHostedVector, PushBack_Span_To0_H0H0)
{
  const std::array<uint32_t, 3> newValues = {901, 902, 903};
  const auto newValueSpan = ReadOnlySpanUtil::AsSpan(newValues);
  TightHostedVector<uint32_t, 2> buffer;

  buffer.PushBack(HostedVectorIndex::First, newValueSpan);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(newValueSpan.size(), buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::Second));

  EXPECT_EQ(newValueSpan, buffer.AsReadOnlySpan(HostedVectorIndex::First));
}


TEST(TestCollections_TightHostedVector, PushBack_Span_To1_H0H0)
{
  const std::array<uint32_t, 3> newValues = {901, 902, 903};
  const auto newValueSpan = ReadOnlySpanUtil::AsSpan(newValues);
  TightHostedVector<uint32_t, 2> buffer;

  buffer.PushBack(HostedVectorIndex::Second, newValueSpan);

  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(newValueSpan.size(), buffer.Size(HostedVectorIndex::Second));

  EXPECT_EQ(newValueSpan, buffer.AsReadOnlySpan(HostedVectorIndex::Second));
}


TEST(TestCollections_TightHostedVector, PushBack_Span_To0_H0H1)
{
  const std::array<uint32_t, 3> newValues = {901, 902, 903};
  const auto newValueSpan = ReadOnlySpanUtil::AsSpan(newValues);

  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueB = 42;
  // Ensure we have one value in the back array
  buffer.PushBack(HostedVectorIndex::Second, valueB);
  // Push to the first array
  buffer.PushBack(HostedVectorIndex::First, newValueSpan);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(newValueSpan.size(), buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::Second));

  EXPECT_EQ(newValueSpan, buffer.AsReadOnlySpan(HostedVectorIndex::First));
  ASSERT_EQ(valueB, buffer.UncheckedAt(HostedVectorIndex::Second, 0));
}

TEST(TestCollections_TightHostedVector, PushBack_Span_To1_H1H0)
{
  const std::array<uint32_t, 3> newValues = {901, 902, 903};
  const auto newValueSpan = ReadOnlySpanUtil::AsSpan(newValues);

  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueA = 1337;
  // Ensure we have one value in the first array
  buffer.PushBack(HostedVectorIndex::First, valueA);
  // Push to the last array
  buffer.PushBack(HostedVectorIndex::Second, newValueSpan);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(newValueSpan.size(), buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(valueA, buffer.UncheckedAt(HostedVectorIndex::First, 0));
  EXPECT_EQ(newValueSpan, buffer.AsReadOnlySpan(HostedVectorIndex::Second));
}


TEST(TestCollections_TightHostedVector, PushBack_Span_Push1Push1Push0)
{
  const std::array<uint32_t, 3> newValues = {901, 902, 903};
  const auto newValueSpan = ReadOnlySpanUtil::AsSpan(newValues);

  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueB = 42;
  constexpr uint32_t valueC = 31;
  buffer.PushBack(HostedVectorIndex::Second, valueB);
  buffer.PushBack(HostedVectorIndex::Second, valueC);
  // Push to the first array
  buffer.PushBack(HostedVectorIndex::First, newValueSpan);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(newValueSpan.size(), buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(2u, buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(newValueSpan, buffer.AsReadOnlySpan(HostedVectorIndex::First));
  ASSERT_EQ(valueB, buffer.UncheckedAt(HostedVectorIndex::Second, 0));
  ASSERT_EQ(valueC, buffer.UncheckedAt(HostedVectorIndex::Second, 1));
}

TEST(TestCollections_TightHostedVector, PushBack_Span_Push0Push0Push1)
{
  const std::array<uint32_t, 3> newValues = {901, 902, 903};
  const auto newValueSpan = ReadOnlySpanUtil::AsSpan(newValues);

  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueA = 1337;
  constexpr uint32_t valueB = 42;
  buffer.PushBack(HostedVectorIndex::First, valueA);
  buffer.PushBack(HostedVectorIndex::First, valueB);
  // Push to the first array
  buffer.PushBack(HostedVectorIndex::Second, newValueSpan);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(2u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(newValueSpan.size(), buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(valueA, buffer.UncheckedAt(HostedVectorIndex::First, 0));
  ASSERT_EQ(valueB, buffer.UncheckedAt(HostedVectorIndex::First, 1));
  ASSERT_EQ(newValueSpan, buffer.AsReadOnlySpan(HostedVectorIndex::Second));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// UncheckedPopBack
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_TightHostedVector, UncheckedPopBack0_AB0C1)
{
  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueA = 1337;
  constexpr uint32_t valueB = 42;
  constexpr uint32_t valueC = 31;

  buffer.PushBack(HostedVectorIndex::First, valueA);
  buffer.PushBack(HostedVectorIndex::First, valueB);
  buffer.PushBack(HostedVectorIndex::Second, valueC);

  buffer.UncheckedPopBack(0);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(valueA, buffer.UncheckedAt(HostedVectorIndex::First, 0));
  ASSERT_EQ(valueC, buffer.UncheckedAt(HostedVectorIndex::Second, 0));
}


TEST(TestCollections_TightHostedVector, UncheckedPopBack_0All_AB0C1)
{
  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueA = 1337;
  constexpr uint32_t valueB = 42;
  constexpr uint32_t valueC = 31;

  buffer.PushBack(HostedVectorIndex::First, valueA);
  buffer.PushBack(HostedVectorIndex::First, valueB);
  buffer.PushBack(HostedVectorIndex::Second, valueC);

  buffer.UncheckedPopBack(0);
  buffer.UncheckedPopBack(0);

  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(valueC, buffer.UncheckedAt(HostedVectorIndex::Second, 0));
}


TEST(TestCollections_TightHostedVector, UncheckedPopBack1_A0BC1)
{
  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueA = 1337;
  constexpr uint32_t valueB = 42;
  constexpr uint32_t valueC = 31;

  buffer.PushBack(HostedVectorIndex::First, valueA);
  buffer.PushBack(HostedVectorIndex::Second, valueB);
  buffer.PushBack(HostedVectorIndex::Second, valueC);

  buffer.UncheckedPopBack(1);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(valueA, buffer.UncheckedAt(HostedVectorIndex::First, 0));
  ASSERT_EQ(valueB, buffer.UncheckedAt(HostedVectorIndex::Second, 0));
}


TEST(TestCollections_TightHostedVector, UncheckedPopBack_1All_A0BC1)
{
  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueA = 1337;
  constexpr uint32_t valueB = 42;
  constexpr uint32_t valueC = 31;

  buffer.PushBack(HostedVectorIndex::First, valueA);
  buffer.PushBack(HostedVectorIndex::Second, valueB);
  buffer.PushBack(HostedVectorIndex::Second, valueC);

  buffer.UncheckedPopBack(1);
  buffer.UncheckedPopBack(1);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(valueA, buffer.UncheckedAt(HostedVectorIndex::First, 0));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// UncheckedRemoveAt
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_TightHostedVector, UncheckedRemoveAt0_0_AB0C1)
{
  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueA = 1337;
  constexpr uint32_t valueB = 42;
  constexpr uint32_t valueC = 31;

  buffer.PushBack(HostedVectorIndex::First, valueA);
  buffer.PushBack(HostedVectorIndex::First, valueB);
  buffer.PushBack(HostedVectorIndex::Second, valueC);

  buffer.UncheckedRemoveAt(HostedVectorIndex::First, 0);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(valueB, buffer.UncheckedAt(HostedVectorIndex::First, 0));
  ASSERT_EQ(valueC, buffer.UncheckedAt(HostedVectorIndex::Second, 0));
}


TEST(TestCollections_TightHostedVector, UncheckedRemoveAt0_1_AB0C1)
{
  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueA = 1337;
  constexpr uint32_t valueB = 42;
  constexpr uint32_t valueC = 31;

  buffer.PushBack(HostedVectorIndex::First, valueA);
  buffer.PushBack(HostedVectorIndex::First, valueB);
  buffer.PushBack(HostedVectorIndex::Second, valueC);

  buffer.UncheckedRemoveAt(HostedVectorIndex::First, 1);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(valueA, buffer.UncheckedAt(HostedVectorIndex::First, 0));
  ASSERT_EQ(valueC, buffer.UncheckedAt(HostedVectorIndex::Second, 0));
}


TEST(TestCollections_TightHostedVector, UncheckedRemoveAt_0All_AB0C1)
{
  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueA = 1337;
  constexpr uint32_t valueB = 42;
  constexpr uint32_t valueC = 31;

  buffer.PushBack(HostedVectorIndex::First, valueA);
  buffer.PushBack(HostedVectorIndex::First, valueB);
  buffer.PushBack(HostedVectorIndex::Second, valueC);

  buffer.UncheckedRemoveAt(HostedVectorIndex::First, 0);
  buffer.UncheckedRemoveAt(HostedVectorIndex::First, 0);

  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(valueC, buffer.UncheckedAt(HostedVectorIndex::Second, 0));
}


TEST(TestCollections_TightHostedVector, UncheckedRemoveAt_0AllReversed_AB0C1)
{
  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueA = 1337;
  constexpr uint32_t valueB = 42;
  constexpr uint32_t valueC = 31;

  buffer.PushBack(HostedVectorIndex::First, valueA);
  buffer.PushBack(HostedVectorIndex::First, valueB);
  buffer.PushBack(HostedVectorIndex::Second, valueC);

  buffer.UncheckedRemoveAt(HostedVectorIndex::First, 1);
  buffer.UncheckedRemoveAt(HostedVectorIndex::First, 0);

  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(valueC, buffer.UncheckedAt(HostedVectorIndex::Second, 0));
}


TEST(TestCollections_TightHostedVector, UncheckedRemoveAt1_0_A0BC1)
{
  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueA = 1337;
  constexpr uint32_t valueB = 42;
  constexpr uint32_t valueC = 31;

  buffer.PushBack(HostedVectorIndex::First, valueA);
  buffer.PushBack(HostedVectorIndex::Second, valueB);
  buffer.PushBack(HostedVectorIndex::Second, valueC);

  buffer.UncheckedRemoveAt(HostedVectorIndex::Second, 0);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(valueA, buffer.UncheckedAt(HostedVectorIndex::First, 0));
  ASSERT_EQ(valueC, buffer.UncheckedAt(HostedVectorIndex::Second, 0));
}


TEST(TestCollections_TightHostedVector, UncheckedRemoveAt1_1_A0BC1)
{
  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueA = 1337;
  constexpr uint32_t valueB = 42;
  constexpr uint32_t valueC = 31;

  buffer.PushBack(HostedVectorIndex::First, valueA);
  buffer.PushBack(HostedVectorIndex::Second, valueB);
  buffer.PushBack(HostedVectorIndex::Second, valueC);

  buffer.UncheckedRemoveAt(HostedVectorIndex::Second, 1);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(valueA, buffer.UncheckedAt(HostedVectorIndex::First, 0));
  ASSERT_EQ(valueB, buffer.UncheckedAt(HostedVectorIndex::Second, 0));
}


TEST(TestCollections_TightHostedVector, UncheckedRemoveAt_1All_A0BC1)
{
  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueA = 1337;
  constexpr uint32_t valueB = 42;
  constexpr uint32_t valueC = 31;

  buffer.PushBack(HostedVectorIndex::First, valueA);
  buffer.PushBack(HostedVectorIndex::Second, valueB);
  buffer.PushBack(HostedVectorIndex::Second, valueC);

  buffer.UncheckedRemoveAt(HostedVectorIndex::Second, 0);
  buffer.UncheckedRemoveAt(HostedVectorIndex::Second, 0);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(valueA, buffer.UncheckedAt(HostedVectorIndex::First, 0));
}


TEST(TestCollections_TightHostedVector, UncheckedRemoveAt_1AllReversed_A0BC1)
{
  TightHostedVector<uint32_t, 2> buffer;

  constexpr uint32_t valueA = 1337;
  constexpr uint32_t valueB = 42;
  constexpr uint32_t valueC = 31;

  buffer.PushBack(HostedVectorIndex::First, valueA);
  buffer.PushBack(HostedVectorIndex::Second, valueB);
  buffer.PushBack(HostedVectorIndex::Second, valueC);

  buffer.UncheckedRemoveAt(HostedVectorIndex::Second, 1);
  buffer.UncheckedRemoveAt(HostedVectorIndex::Second, 0);

  ASSERT_FALSE(buffer.Empty(HostedVectorIndex::First));
  ASSERT_TRUE(buffer.Empty(HostedVectorIndex::Second));

  ASSERT_EQ(1u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::Second));

  ASSERT_EQ(valueA, buffer.UncheckedAt(HostedVectorIndex::First, 0));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// AsReadOnlySpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_TightHostedVector, AsReadOnlySpan_0_H2H0)
{
  TightHostedVector<uint32_t, 2> buffer;
  std::array<uint32_t, 2> test = {42, 1337};
  for (const auto value : test)
  {
    buffer.PushBack(HostedVectorIndex::First, value);
  }

  ASSERT_EQ(test.size(), buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::Second));

  {
    auto span = buffer.AsReadOnlySpan(HostedVectorIndex::First);
    ASSERT_EQ(test.size(), span.size());
    EXPECT_EQ(test[0], span[0]);
    EXPECT_EQ(test[1], span[1]);
  }
  {
    auto span = buffer.AsReadOnlySpan(HostedVectorIndex::Second);
    ASSERT_TRUE(span.empty());
  }
}


TEST(TestCollections_TightHostedVector, AsReadOnlySpan_0_H2H3)
{
  TightHostedVector<uint32_t, 2> buffer;
  std::array<uint32_t, 2> test0 = {42, 1337};
  std::array<uint32_t, 3> test1 = {1, 2, 3};
  for (const auto value : test0)
  {
    buffer.PushBack(HostedVectorIndex::First, value);
  }
  for (const auto value : test1)
  {
    buffer.PushBack(HostedVectorIndex::Second, value);
  }

  ASSERT_EQ(test0.size(), buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(test1.size(), buffer.Size(HostedVectorIndex::Second));

  {
    auto span = buffer.AsReadOnlySpan(HostedVectorIndex::First);
    ASSERT_EQ(test0.size(), span.size());
    EXPECT_EQ(test0[0], span[0]);
    EXPECT_EQ(test0[1], span[1]);
  }
  {
    auto span = buffer.AsReadOnlySpan(HostedVectorIndex::Second);
    ASSERT_EQ(test1.size(), span.size());
    EXPECT_EQ(test1[0], span[0]);
    EXPECT_EQ(test1[1], span[1]);
    EXPECT_EQ(test1[2], span[2]);
  }
}

TEST(TestCollections_TightHostedVector, AsReadOnlySpan_1_H0H2)
{
  TightHostedVector<uint32_t, 2> buffer;
  std::array<uint32_t, 2> test = {42, 1337};
  for (const auto value : test)
  {
    buffer.PushBack(HostedVectorIndex::Second, value);
  }
  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(test.size(), buffer.Size(HostedVectorIndex::Second));

  {
    auto span = buffer.AsReadOnlySpan(HostedVectorIndex::First);
    ASSERT_TRUE(span.empty());
  }
  {
    auto span = buffer.AsReadOnlySpan(HostedVectorIndex::Second);
    ASSERT_EQ(test.size(), span.size());
    EXPECT_EQ(test[0], span[0]);
    EXPECT_EQ(test[1], span[1]);
  }
}

TEST(TestCollections_TightHostedVector, AsReadOnlySpan_1_H3H2)
{
  TightHostedVector<uint32_t, 2> buffer;
  std::array<uint32_t, 3> test0 = {1, 2, 3};
  std::array<uint32_t, 2> test1 = {42, 1337};
  for (const auto value : test0)
  {
    buffer.PushBack(HostedVectorIndex::First, value);
  }
  for (const auto value : test1)
  {
    buffer.PushBack(HostedVectorIndex::Second, value);
  }

  {
    auto span = buffer.AsReadOnlySpan(HostedVectorIndex::First);
    ASSERT_EQ(test0.size(), span.size());
    EXPECT_EQ(test0[0], span[0]);
    EXPECT_EQ(test0[1], span[1]);
    EXPECT_EQ(test0[2], span[2]);
  }
  {
    auto span = buffer.AsReadOnlySpan(HostedVectorIndex::Second);
    ASSERT_EQ(test1.size(), span.size());
    EXPECT_EQ(test1[0], span[0]);
    EXPECT_EQ(test1[1], span[1]);
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// begin
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_TightHostedVector, begin_0_H2H0)
{
  TightHostedVector<uint32_t, 2> buffer;
  std::array<uint32_t, 2> test = {42, 1337};
  for (const auto value : test)
  {
    buffer.PushBack(HostedVectorIndex::First, value);
  }

  ASSERT_EQ(test.size(), buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::Second));


  auto itr = buffer.Begin(0);
  const auto itrEnd = buffer.End(0);
  ASSERT_NE(itrEnd, itr);
  EXPECT_EQ(test[0], *itr);
  ++itr;
  ASSERT_NE(itrEnd, itr);
  EXPECT_EQ(test[1], *itr);
  ++itr;
  ASSERT_EQ(itrEnd, itr);
}


TEST(TestCollections_TightHostedVector, begin_0_H2H3)
{
  TightHostedVector<uint32_t, 2> buffer;
  std::array<uint32_t, 2> test0 = {42, 1337};
  std::array<uint32_t, 3> test1 = {1, 2, 3};
  for (const auto value : test0)
  {
    buffer.PushBack(HostedVectorIndex::First, value);
  }
  for (const auto value : test1)
  {
    buffer.PushBack(HostedVectorIndex::Second, value);
  }

  ASSERT_EQ(test0.size(), buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(test1.size(), buffer.Size(HostedVectorIndex::Second));

  auto itr = buffer.Begin(0);
  const auto itrEnd = buffer.End(0);
  ASSERT_NE(itrEnd, itr);
  EXPECT_EQ(test0[0], *itr);
  ++itr;
  ASSERT_NE(itrEnd, itr);
  EXPECT_EQ(test0[1], *itr);
  ++itr;
  ASSERT_EQ(itrEnd, itr);
}

TEST(TestCollections_TightHostedVector, begin_1_H0H2)
{
  TightHostedVector<uint32_t, 2> buffer;
  std::array<uint32_t, 2> test = {42, 1337};
  for (const auto value : test)
  {
    buffer.PushBack(HostedVectorIndex::Second, value);
  }

  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(test.size(), buffer.Size(HostedVectorIndex::Second));

  auto itr = buffer.Begin(1);
  const auto itrEnd = buffer.End(1);
  ASSERT_NE(itrEnd, itr);
  EXPECT_EQ(test[0], *itr);
  ++itr;
  ASSERT_NE(itrEnd, itr);
  EXPECT_EQ(test[1], *itr);
  ++itr;
  ASSERT_EQ(itrEnd, itr);
}

TEST(TestCollections_TightHostedVector, begin_1_H3H2)
{
  TightHostedVector<uint32_t, 2> buffer;
  std::array<uint32_t, 3> test0 = {1, 2, 3};
  std::array<uint32_t, 2> test1 = {42, 1337};
  for (const auto value : test0)
  {
    buffer.PushBack(HostedVectorIndex::First, value);
  }
  for (const auto value : test1)
  {
    buffer.PushBack(HostedVectorIndex::Second, value);
  }

  ASSERT_EQ(test0.size(), buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(test1.size(), buffer.Size(HostedVectorIndex::Second));

  auto itr = buffer.Begin(1);
  const auto itrEnd = buffer.End(1);
  ASSERT_NE(itrEnd, itr);
  EXPECT_EQ(test1[0], *itr);
  ++itr;
  ASSERT_NE(itrEnd, itr);
  EXPECT_EQ(test1[1], *itr);
  ++itr;
  ASSERT_EQ(itrEnd, itr);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// cbegin
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestCollections_TightHostedVector, cbegin_0_H2H0)
{
  TightHostedVector<uint32_t, 2> buffer;
  std::array<uint32_t, 2> test = {42, 1337};
  for (const auto value : test)
  {
    buffer.PushBack(HostedVectorIndex::First, value);
  }

  ASSERT_EQ(test.size(), buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::Second));


  auto itr = buffer.CBegin(0);
  const auto itrEnd = buffer.CEnd(0);
  ASSERT_NE(itrEnd, itr);
  EXPECT_EQ(test[0], *itr);
  ++itr;
  ASSERT_NE(itrEnd, itr);
  EXPECT_EQ(test[1], *itr);
  ++itr;
  ASSERT_EQ(itrEnd, itr);
}


TEST(TestCollections_TightHostedVector, cbegin_0_H2H3)
{
  TightHostedVector<uint32_t, 2> buffer;
  std::array<uint32_t, 2> test0 = {42, 1337};
  std::array<uint32_t, 3> test1 = {1, 2, 3};
  for (const auto value : test0)
  {
    buffer.PushBack(HostedVectorIndex::First, value);
  }
  for (const auto value : test1)
  {
    buffer.PushBack(HostedVectorIndex::Second, value);
  }

  ASSERT_EQ(test0.size(), buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(test1.size(), buffer.Size(HostedVectorIndex::Second));

  auto itr = buffer.CBegin(0);
  const auto itrEnd = buffer.CEnd(0);
  ASSERT_NE(itrEnd, itr);
  EXPECT_EQ(test0[0], *itr);
  ++itr;
  ASSERT_NE(itrEnd, itr);
  EXPECT_EQ(test0[1], *itr);
  ++itr;
  ASSERT_EQ(itrEnd, itr);
}

TEST(TestCollections_TightHostedVector, cbegin_1_H0H2)
{
  TightHostedVector<uint32_t, 2> buffer;
  std::array<uint32_t, 2> test = {42, 1337};
  for (const auto value : test)
  {
    buffer.PushBack(HostedVectorIndex::Second, value);
  }

  ASSERT_EQ(0u, buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(test.size(), buffer.Size(HostedVectorIndex::Second));

  auto itr = buffer.CBegin(1);
  const auto itrEnd = buffer.CEnd(1);
  ASSERT_NE(itrEnd, itr);
  EXPECT_EQ(test[0], *itr);
  ++itr;
  ASSERT_NE(itrEnd, itr);
  EXPECT_EQ(test[1], *itr);
  ++itr;
  ASSERT_EQ(itrEnd, itr);
}

TEST(TestCollections_TightHostedVector, cbegin_1_H3H2)
{
  TightHostedVector<uint32_t, 2> buffer;
  std::array<uint32_t, 3> test0 = {1, 2, 3};
  std::array<uint32_t, 2> test1 = {42, 1337};
  for (const auto value : test0)
  {
    buffer.PushBack(HostedVectorIndex::First, value);
  }
  for (const auto value : test1)
  {
    buffer.PushBack(HostedVectorIndex::Second, value);
  }

  ASSERT_EQ(test0.size(), buffer.Size(HostedVectorIndex::First));
  ASSERT_EQ(test1.size(), buffer.Size(HostedVectorIndex::Second));

  auto itr = buffer.CBegin(1);
  const auto itrEnd = buffer.CEnd(1);
  ASSERT_NE(itrEnd, itr);
  EXPECT_EQ(test1[0], *itr);
  ++itr;
  ASSERT_NE(itrEnd, itr);
  EXPECT_EQ(test1[1], *itr);
  ++itr;
  ASSERT_EQ(itrEnd, itr);
}
