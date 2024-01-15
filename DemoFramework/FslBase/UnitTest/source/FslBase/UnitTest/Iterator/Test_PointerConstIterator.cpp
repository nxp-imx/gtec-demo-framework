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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Iterator/PointerConstIterator.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>

using namespace Fsl;

namespace
{
  using TestIterator_PointerConstIterator = TestFixtureFslBase;

  template <typename T, std::size_t TSize>
  PointerConstIterator<T> CreateBegin(const std::array<T, TSize>& source)
  {
#ifndef FSL_CUSTOM_ITERATOR_CHECK_ENABLED
    return PointerConstIterator<T>(source.data());
#else
    return {source.data(), source.data(), source.size()};
#endif
  }

  template <typename T, std::size_t TSize>
  PointerConstIterator<T> CreateEnd(const std::array<T, TSize>& source)
  {
#ifndef FSL_CUSTOM_ITERATOR_CHECK_ENABLED
    return PointerConstIterator<T>(source.data() + source.size());
#else
    return {source.data() + source.size(), source.data(), source.size()};
#endif
  }

}


TEST(TestIterator_PointerConstIterator, ConstructEmpty)
{
  PointerConstIterator<int32_t> itr;
  FSL_PARAM_NOT_USED(itr);
}


TEST(TestIterator_PointerConstIterator, Construct)
{
  const std::array<int32_t, 3> testData = {1, 2, 3};
  auto itr = CreateBegin(testData);
  FSL_PARAM_NOT_USED(itr);
}


TEST(TestIterator_PointerConstIterator, OpStar)
{
  const std::array<int32_t, 3> testData = {1, 2, 3};
  auto itr = CreateBegin(testData);

  EXPECT_EQ(testData[0], *itr);
}

TEST(TestIterator_PointerConstIterator, OpArrow)
{
  struct Tester
  {
    int32_t Val{};
  };
  const std::array<Tester, 3> testData = {Tester{1}, Tester{2}, Tester{3}};
  auto itr = CreateBegin(testData);

  EXPECT_EQ(testData[0].Val, itr->Val);
}


TEST(TestIterator_PointerConstIterator, OpAccess)
{
  const std::array<int32_t, 3> testData = {1, 2, 3};
  auto itr = CreateBegin(testData);

  EXPECT_EQ(testData[0], itr[0]);
}


TEST(TestIterator_PointerConstIterator, OpAddPre)
{
  const std::array<int32_t, 3> testData = {1, 2, 3};
  auto itr = CreateBegin(testData);

  ASSERT_EQ(testData[0], *itr);
  ++itr;
  ASSERT_EQ(testData[1], *itr);
  auto itrCopy = itr;
  ++itrCopy;
  ASSERT_EQ(itrCopy, ++itr);
}


TEST(TestIterator_PointerConstIterator, OpAddPost)
{
  const std::array<int32_t, 3> testData = {1, 2, 3};
  auto itr = CreateBegin(testData);

  ASSERT_EQ(testData[0], *itr);
  itr++;
  ASSERT_EQ(testData[1], *itr);
  auto itrCopy = itr;
  ASSERT_EQ(itrCopy, itr++);
}

TEST(TestIterator_PointerConstIterator, OpSubPre)
{
  const std::array<int32_t, 3> testData = {1, 2, 3};
  auto itr = CreateEnd(testData);

  --itr;
  ASSERT_EQ(testData[2], *itr);
  auto itrCopy = itr;
  --itrCopy;
  ASSERT_EQ(itrCopy, --itr);
}


TEST(TestIterator_PointerConstIterator, OpSubPost)
{
  const std::array<int32_t, 3> testData = {1, 2, 3};
  auto itr = CreateEnd(testData);

  itr--;
  ASSERT_EQ(testData[2], *itr);
  auto itrCopy = itr;
  ASSERT_EQ(itrCopy, itr--);
}


TEST(TestIterator_PointerConstIterator, OpAddEqual_Value)
{
  const std::array<int32_t, 3> testData = {1, 2, 3};
  auto itr = CreateBegin(testData);

  itr += 2u;

  ASSERT_EQ(testData[2], *itr);
}


TEST(TestIterator_PointerConstIterator, OpSubEqual_Value)
{
  const std::array<int32_t, 3> testData = {1, 2, 3};
  auto itr = CreateEnd(testData);

  itr -= 2u;

  ASSERT_EQ(testData[1], *itr);
}


TEST(TestIterator_PointerConstIterator, OpAdd_Itr_Value)
{
  const std::array<int32_t, 3> testData = {1, 2, 3};
  auto itr = CreateBegin(testData);

  itr = itr + 2u;

  ASSERT_EQ(testData[2], *itr);
}


TEST(TestIterator_PointerConstIterator, OpSub_Itr_Value)
{
  const std::array<int32_t, 3> testData = {1, 2, 3};
  auto itr = CreateEnd(testData);

  itr = itr - 2u;

  ASSERT_EQ(testData[1], *itr);
}


TEST(TestIterator_PointerConstIterator, OpAdd_Value_Itr)
{
  const std::array<int32_t, 3> testData = {1, 2, 3};
  auto itr = CreateBegin(testData);

  itr = 2u + itr;

  ASSERT_EQ(testData[2], *itr);
}


TEST(TestIterator_PointerConstIterator, OpSub_Itrs)
{
  const std::array<int32_t, 3> testData = {1, 2, 3};
  auto itrBegin = CreateBegin(testData);
  auto itrEnd = CreateEnd(testData);
  ASSERT_EQ(UncheckedNumericCast<int64_t>(testData.size()), UncheckedNumericCast<int64_t>(itrEnd - itrBegin));
}


TEST(TestIterator_PointerConstIterator, OpEqual)
{
  const std::array<int32_t, 3> testData = {1, 2, 3};

  auto itrBegin = CreateBegin(testData);
  auto itrEnd = CreateEnd(testData);

  EXPECT_TRUE(itrBegin == itrBegin);
  EXPECT_FALSE(itrBegin == itrEnd);
  EXPECT_FALSE(itrEnd == itrBegin);
  EXPECT_TRUE(itrEnd == itrEnd);
}

TEST(TestIterator_PointerConstIterator, OpNotEqual)
{
  const std::array<int32_t, 3> testData = {1, 2, 3};

  auto itrBegin = CreateBegin(testData);
  auto itrEnd = CreateEnd(testData);

  EXPECT_FALSE(itrBegin != itrBegin);
  EXPECT_TRUE(itrBegin != itrEnd);
  EXPECT_TRUE(itrEnd != itrBegin);
  EXPECT_FALSE(itrEnd != itrEnd);
}


TEST(TestIterator_PointerConstIterator, OpLess)
{
  const std::array<int32_t, 3> testData = {1, 2, 3};

  auto itrBegin = CreateBegin(testData);
  auto itrEnd = CreateEnd(testData);

  EXPECT_FALSE(itrBegin < itrBegin);
  EXPECT_TRUE(itrBegin < itrEnd);
  EXPECT_FALSE(itrEnd < itrBegin);
  EXPECT_FALSE(itrEnd < itrEnd);
}


TEST(TestIterator_PointerConstIterator, OpLessOrEqual)
{
  const std::array<int32_t, 3> testData = {1, 2, 3};

  auto itrBegin = CreateBegin(testData);
  auto itrEnd = CreateEnd(testData);

  EXPECT_TRUE(itrBegin <= itrBegin);
  EXPECT_TRUE(itrBegin <= itrEnd);
  EXPECT_FALSE(itrEnd <= itrBegin);
  EXPECT_TRUE(itrEnd <= itrEnd);
}


TEST(TestIterator_PointerConstIterator, OpGreater)
{
  const std::array<int32_t, 3> testData = {1, 2, 3};

  auto itrBegin = CreateBegin(testData);
  auto itrEnd = CreateEnd(testData);

  EXPECT_FALSE(itrBegin > itrBegin);
  EXPECT_FALSE(itrBegin > itrEnd);
  EXPECT_TRUE(itrEnd > itrBegin);
  EXPECT_FALSE(itrEnd > itrEnd);
}


TEST(TestIterator_PointerConstIterator, OpGreaterOrEqual)
{
  const std::array<int32_t, 3> testData = {1, 2, 3};

  auto itrBegin = CreateBegin(testData);
  auto itrEnd = CreateEnd(testData);

  EXPECT_TRUE(itrBegin >= itrBegin);
  EXPECT_FALSE(itrBegin >= itrEnd);
  EXPECT_TRUE(itrEnd >= itrBegin);
  EXPECT_TRUE(itrEnd >= itrEnd);
}
