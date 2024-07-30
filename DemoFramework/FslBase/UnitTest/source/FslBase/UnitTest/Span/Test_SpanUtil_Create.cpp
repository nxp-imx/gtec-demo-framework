/****************************************************************************************************************************************************
 * Copyright 2019, 2024 NXP
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

#include <FslBase/Span/SpanUtil_Create.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>

using namespace Fsl;

namespace
{
  using TestSpanUtil_Create = TestFixtureFslBase;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// CheckedCreateReadOnly
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Create, CheckedCheckedCreateReadOnly_PointerLength)
{
  constexpr std::array<uint16_t, 10> Data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::CheckedCreateReadOnly(Data.data(), Data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), Data.data());
  EXPECT_EQ(span.size(), Data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Create, CheckedCreateReadOnly_PointerLength_NullPtr)
{
  EXPECT_THROW(SpanUtil::CheckedCreateReadOnly<uint16_t>(nullptr, 0u), std::invalid_argument);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Create, CheckedCreateReadOnly_PointerLength_NullPtrInvalidLength)
{
  constexpr std::array<uint16_t, 10> Data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_THROW(SpanUtil::CheckedCreateReadOnly<uint16_t>(nullptr, Data.size()), std::invalid_argument);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// CreateReadOnly
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestSpanUtil_Create, CheckedCreateReadOnly_PointerLength)
{
  constexpr std::array<uint16_t, 10> Data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::CreateReadOnly(Data.data(), Data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), Data.data());
  EXPECT_EQ(span.size(), Data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Create, CreateReadOnly_PointerLength_NullPtr)
{
  ReadOnlySpan<uint16_t> span = SpanUtil::CreateReadOnly<uint16_t>(nullptr, 0);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), 0u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Create, CreateReadOnly_PointerLength_NullPtrInvalidLength)
{
  constexpr std::array<uint16_t, 10> Data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  ReadOnlySpan<uint16_t> span = SpanUtil::CreateReadOnly<uint16_t>(nullptr, Data.size());

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), 0u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// UncheckedCreateReadOnly
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestSpanUtil_Create, UncheckedCreateReadOnly_FromPointerLength_NoCheck)
{
  constexpr std::array<uint16_t, 10> Data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::UncheckedCreateReadOnly(Data.data(), Data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), Data.data());
  EXPECT_EQ(span.size(), Data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// CheckedCreate
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Create, CheckedCreate_PointerLength)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::CheckedCreate(data.data(), data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Create, CheckedCreate_PointerLength_NullPtr)
{
  EXPECT_THROW(SpanUtil::CheckedCreate<uint16_t>(nullptr, 0), std::invalid_argument);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Create, CheckedCreate_PointerLength_NullPtrInvalidLength)
{
  constexpr std::array<uint16_t, 10> Data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_THROW(SpanUtil::CheckedCreate<uint16_t>(nullptr, Data.size()), std::invalid_argument);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Create
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Create, Create_PointerLength)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::Create(data.data(), data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Create, Create_PointerLength_NullPtr)
{
  Span<uint16_t> span = SpanUtil::Create<uint16_t>(nullptr, 0);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), 0u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Create, Create_PointerLength_NullPtrInvalidLength)
{
  constexpr std::array<uint16_t, 10> Data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  auto span = SpanUtil::Create<uint16_t>(nullptr, Data.size());

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), 0u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// UncheckedCreate
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Create, UncheckedCreate_PointerLength_NoCheck)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::UncheckedCreate(data.data(), data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}
