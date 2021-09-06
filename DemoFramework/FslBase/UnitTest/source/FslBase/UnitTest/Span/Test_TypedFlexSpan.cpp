/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslBase/Span/TypedFlexSpan.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <cstddef>
#include <cstring>

using namespace Fsl;

namespace
{
  using Test_TypedFlexSpan = TestFixtureFslBase;

  struct TestRecord
  {
    uint32_t Val0{0};

    constexpr bool operator==(const TestRecord& rhs) const noexcept
    {
      return Val0 == rhs.Val0;
    }

    constexpr bool operator!=(const TestRecord& rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };

  struct ComplexRecord
  {
    uint32_t Val0{0x12345678};
    TestRecord Test{};
    uint32_t Val2{0x87654321};

    ComplexRecord() = default;
    explicit ComplexRecord(const TestRecord& val1)
      : Test(val1)
    {
    }
  };

  TestRecord* GetTestRecordPointer(ComplexRecord* const pData)
  {
    return reinterpret_cast<TestRecord*>((reinterpret_cast<uint8_t*>(pData) + offsetof(ComplexRecord, Test)));
  }

  TypedFlexSpan<TestRecord> AsSpan(std::vector<ComplexRecord>& vec)
  {
    return TypedFlexSpan<TestRecord>(GetTestRecordPointer(vec.data()), vec.size(), sizeof(ComplexRecord));
  }

  const std::vector<ComplexRecord> g_content0 = {};

  const std::vector<ComplexRecord> g_content4 = {ComplexRecord(TestRecord{0x500}), ComplexRecord(TestRecord{0x601}), ComplexRecord(TestRecord{0x702}),
                                                 ComplexRecord(TestRecord{0x8003})};

  const std::vector<ComplexRecord> g_content10 = {ComplexRecord(TestRecord{0}), ComplexRecord(TestRecord{1}), ComplexRecord(TestRecord{2}),
                                                  ComplexRecord(TestRecord{3}), ComplexRecord(TestRecord{4}), ComplexRecord(TestRecord{5}),
                                                  ComplexRecord(TestRecord{6}), ComplexRecord(TestRecord{7}), ComplexRecord(TestRecord{8}),
                                                  ComplexRecord(TestRecord{9})};

}


TEST(Test_TypedFlexSpan, Construct_Empty)
{
  TypedFlexSpan<TestRecord> span;

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.flex_data(), nullptr);
  EXPECT_EQ(span.size(), 0u);
  EXPECT_EQ(span.stride(), 0u);
}

TEST(Test_TypedFlexSpan, Construct_Empty_constexpr)
{
  constexpr TypedFlexSpan<TestRecord> span;

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.flex_data(), nullptr);
  EXPECT_EQ(span.size(), 0u);
  EXPECT_EQ(span.stride(), 0u);
}


TEST(Test_TypedFlexSpan, Construct_Data)
{
  std::vector<ComplexRecord> content4 = g_content4;
  TypedFlexSpan<TestRecord> span(AsSpan(content4));

  EXPECT_FALSE(span.empty());
  EXPECT_NE(span.flex_data(), nullptr);
  EXPECT_EQ(span.size(), content4.size());
  EXPECT_EQ(span.stride(), sizeof(ComplexRecord));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_TypedFlexSpan, SubSpan)
{
  std::vector<ComplexRecord> content10 = g_content10;
  TypedFlexSpan<TestRecord> span(AsSpan(content10));

  {
    TypedFlexSpan<TestRecord> res = span.subspan();
    EXPECT_EQ(content10.size(), res.size());
    EXPECT_EQ(&content10[0].Test, res.flex_data());
    EXPECT_EQ(sizeof(ComplexRecord), res.stride());
  }

  {
    TypedFlexSpan<TestRecord> res = span.subspan(1u);
    EXPECT_EQ(9u, res.size());
    EXPECT_EQ(&content10[1].Test, res.flex_data());
    EXPECT_EQ(sizeof(ComplexRecord), res.stride());
  }
  {
    TypedFlexSpan<TestRecord> res = span.subspan(9u);
    EXPECT_EQ(1u, res.size());
    EXPECT_EQ(&content10[9].Test, res.flex_data());
    EXPECT_EQ(sizeof(ComplexRecord), res.stride());
  }
  {
    TypedFlexSpan<TestRecord> res = span.subspan(1u, 2u);
    EXPECT_EQ(2u, res.size());
    EXPECT_EQ(&content10[1].Test, res.flex_data());
    EXPECT_EQ(sizeof(ComplexRecord), res.stride());
  }
  {
    TypedFlexSpan<TestRecord> res = span.subspan(9u, 2u);
    EXPECT_EQ(1u, res.size());
    EXPECT_EQ(&content10[9].Test, res.flex_data());
    EXPECT_EQ(sizeof(ComplexRecord), res.stride());
  }
  // its ok to read the last entry
  {
    TypedFlexSpan<TestRecord> res = span.subspan(content10.size());
    EXPECT_EQ(0u, res.size());
    EXPECT_EQ(sizeof(ComplexRecord), res.stride());
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_TypedFlexSpan, SubSpan_Const)
{
  std::vector<ComplexRecord> content10 = g_content10;
  const TypedFlexSpan<TestRecord> span(AsSpan(content10));

  {
    ReadOnlyTypedFlexSpan<TestRecord> res = span.subspan();
    EXPECT_EQ(content10.size(), res.size());
    EXPECT_EQ(&content10[0].Test, res.flex_data());
    EXPECT_EQ(sizeof(ComplexRecord), res.stride());
  }

  {
    ReadOnlyTypedFlexSpan<TestRecord> res = span.subspan(1u);
    EXPECT_EQ(9u, res.size());
    EXPECT_EQ(&content10[1].Test, res.flex_data());
    EXPECT_EQ(sizeof(ComplexRecord), res.stride());
  }
  {
    ReadOnlyTypedFlexSpan<TestRecord> res = span.subspan(9u);
    EXPECT_EQ(1u, res.size());
    EXPECT_EQ(&content10[9].Test, res.flex_data());
    EXPECT_EQ(sizeof(ComplexRecord), res.stride());
  }
  {
    ReadOnlyTypedFlexSpan<TestRecord> res = span.subspan(1u, 2u);
    EXPECT_EQ(2u, res.size());
    EXPECT_EQ(&content10[1].Test, res.flex_data());
    EXPECT_EQ(sizeof(ComplexRecord), res.stride());
  }
  {
    ReadOnlyTypedFlexSpan<TestRecord> res = span.subspan(9u, 2u);
    EXPECT_EQ(1u, res.size());
    EXPECT_EQ(&content10[9].Test, res.flex_data());
    EXPECT_EQ(sizeof(ComplexRecord), res.stride());
  }
  // its ok to read the last entry
  {
    ReadOnlyTypedFlexSpan<TestRecord> res = span.subspan(content10.size());
    EXPECT_EQ(0u, res.size());
    EXPECT_EQ(sizeof(ComplexRecord), res.stride());
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_TypedFlexSpan, SubSpan_Empty)
{
  TypedFlexSpan<TestRecord> span;
  {
    TypedFlexSpan<TestRecord> res = span.subspan();
    EXPECT_EQ(0u, res.size());
    EXPECT_EQ(0u, res.stride());
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_TypedFlexSpan, SubSpan_Const_Empty)
{
  const TypedFlexSpan<TestRecord> span;
  {
    ReadOnlyTypedFlexSpan<TestRecord> res = span.subspan();
    EXPECT_EQ(0u, res.size());
    EXPECT_EQ(0u, res.stride());
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_TypedFlexSpan, SubSpan_InvalidPos)
{
  auto content10 = g_content10;
  TypedFlexSpan<TestRecord> span(AsSpan(content10));

  EXPECT_THROW(span.subspan(11u), std::out_of_range);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_TypedFlexSpan, SubSpan_Const_InvalidPos)
{
  auto content10 = g_content10;
  const TypedFlexSpan<TestRecord> span(AsSpan(content10));

  EXPECT_THROW(span.subspan(11u), std::out_of_range);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_TypedFlexSpan, front)
{
  std::vector<ComplexRecord> content10 = g_content10;
  auto span = AsSpan(content10);

  EXPECT_EQ(span.front(), content10[0].Test);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_TypedFlexSpan, front_Const)
{
  std::vector<ComplexRecord> content10 = g_content10;
  const auto span = AsSpan(content10);

  EXPECT_EQ(span.front(), content10[0].Test);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_TypedFlexSpan, back)
{
  std::vector<ComplexRecord> content10 = g_content10;
  auto span = AsSpan(content10);

  EXPECT_EQ(span.back(), content10[content10.size() - 1].Test);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_TypedFlexSpan, back_Const)
{
  std::vector<ComplexRecord> content10 = g_content10;
  const auto span = AsSpan(content10);

  EXPECT_EQ(span.back(), content10[content10.size() - 1].Test);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_TypedFlexSpan, opIndex)
{
  std::vector<ComplexRecord> content10 = g_content10;
  auto span = AsSpan(content10);

  for (std::size_t i = 0; i < span.size(); ++i)
  {
    EXPECT_EQ(span[i], content10[i].Test);
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_TypedFlexSpan, opIndex_Const)
{
  std::vector<ComplexRecord> content10 = g_content10;
  const auto span = AsSpan(content10);

  for (std::size_t i = 0; i < span.size(); ++i)
  {
    EXPECT_EQ(span[i], content10[i].Test);
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_TypedFlexSpan, At)
{
  std::vector<ComplexRecord> content10 = g_content10;
  auto span = AsSpan(content10);

  for (std::size_t i = 0; i < span.size(); ++i)
  {
    EXPECT_EQ(span.at(i), content10.at(i).Test);
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_TypedFlexSpan, At_Const)
{
  std::vector<ComplexRecord> content10 = g_content10;
  const auto span = AsSpan(content10);

  for (std::size_t i = 0; i < span.size(); ++i)
  {
    EXPECT_EQ(span.at(i), content10.at(i).Test);
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_TypedFlexSpan, At_OutOfBounds)
{
  std::vector<ComplexRecord> content10 = g_content10;
  auto span = AsSpan(content10);

  EXPECT_THROW(span.at(span.size()), std::out_of_range);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_TypedFlexSpan, At_OutOfBounds_Const)
{
  std::vector<ComplexRecord> content10 = g_content10;
  const auto span = AsSpan(content10);

  EXPECT_THROW(span.at(span.size()), std::out_of_range);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_TypedFlexSpan, At_Empty_OutOfBounds)
{
  std::vector<ComplexRecord> content0 = g_content0;
  auto span = AsSpan(content0);

  EXPECT_THROW(span.at(span.size()), std::out_of_range);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_TypedFlexSpan, At_Empty_OutOfBounds_Const)
{
  std::vector<ComplexRecord> content0 = g_content0;
  const auto span = AsSpan(content0);

  EXPECT_THROW(span.at(span.size()), std::out_of_range);
}
