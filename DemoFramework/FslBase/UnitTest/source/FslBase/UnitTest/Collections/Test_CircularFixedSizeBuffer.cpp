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

#include <FslBase/Collections/CircularFixedSizeBuffer.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

using namespace Fsl;

namespace
{
  using TestCollections_CircularFixedSizeBuffer = TestFixtureFslBase;
}


TEST(TestCollections_CircularFixedSizeBuffer, Construct)
{
  CircularFixedSizeBuffer<uint32_t> buffer(32);
  EXPECT_TRUE(buffer.empty());
}


TEST(TestCollections_CircularFixedSizeBuffer, Construct_Invalid)
{
  EXPECT_THROW(CircularFixedSizeBuffer<uint32_t>(0), std::invalid_argument);
}


TEST(TestCollections_CircularFixedSizeBuffer, Clear_Empty)
{
  CircularFixedSizeBuffer<uint32_t> buffer(32);
  EXPECT_TRUE(buffer.empty());
  buffer.clear();
  EXPECT_TRUE(buffer.empty());
}


TEST(TestCollections_CircularFixedSizeBuffer, Clear)
{
  CircularFixedSizeBuffer<uint32_t> buffer(32);
  buffer.push_back(22u);
  buffer.push_back(42u);
  buffer.clear();
  EXPECT_TRUE(buffer.empty());
}


TEST(TestCollections_CircularFixedSizeBuffer, ClearDestructs)
{
  CircularFixedSizeBuffer<std::shared_ptr<uint32_t>> buffer(2);

  auto obj1 = std::make_shared<uint32_t>(1u);
  auto obj2 = std::make_shared<uint32_t>(2u);
  auto obj3 = std::make_shared<uint32_t>(3u);
  std::weak_ptr<uint32_t> weakObj1 = obj1;
  std::weak_ptr<uint32_t> weakObj2 = obj2;
  std::weak_ptr<uint32_t> weakObj3 = obj3;

  buffer.push_back(obj1);
  buffer.push_back(obj2);
  buffer.push_back(obj3);
  obj1.reset();
  obj2.reset();
  obj3.reset();

  EXPECT_TRUE(weakObj1.expired());
  EXPECT_FALSE(weakObj2.expired());
  EXPECT_FALSE(weakObj2.expired());

  buffer.clear();
  EXPECT_TRUE(buffer.empty());

  EXPECT_TRUE(weakObj1.expired());
  EXPECT_TRUE(weakObj2.expired());
  EXPECT_TRUE(weakObj2.expired());
}


TEST(TestCollections_CircularFixedSizeBuffer, Empty_Empty)
{
  CircularFixedSizeBuffer<uint32_t> buffer(32);
  EXPECT_TRUE(buffer.empty());
}


TEST(TestCollections_CircularFixedSizeBuffer, Empty)
{
  CircularFixedSizeBuffer<uint32_t> buffer(32);
  buffer.push_back(22u);
  buffer.push_back(42u);
  EXPECT_FALSE(buffer.empty());
}


TEST(TestCollections_CircularFixedSizeBuffer, Front)
{
  CircularFixedSizeBuffer<uint32_t> buffer(32);
  buffer.push_back(22u);
  buffer.push_back(42u);
  EXPECT_EQ(buffer.front(), 22u);
}


TEST(TestCollections_CircularFixedSizeBuffer, Front_cont)
{
  CircularFixedSizeBuffer<uint32_t> buffer(32);
  buffer.push_back(22u);
  buffer.push_back(42u);
  const auto& rBuffer = buffer;
  EXPECT_EQ(rBuffer.front(), 22u);
}


TEST(TestCollections_CircularFixedSizeBuffer, Back)
{
  CircularFixedSizeBuffer<uint32_t> buffer(32);
  buffer.push_back(22u);
  buffer.push_back(42u);
  EXPECT_EQ(buffer.back(), 42u);
}


TEST(TestCollections_CircularFixedSizeBuffer, Back_cont)
{
  CircularFixedSizeBuffer<uint32_t> buffer(32);
  buffer.push_back(22u);
  buffer.push_back(42u);
  const auto& rBuffer = buffer;
  EXPECT_EQ(rBuffer.back(), 42u);
}


TEST(TestCollections_CircularFixedSizeBuffer, opIndex)
{
  CircularFixedSizeBuffer<uint32_t> buffer(32);
  buffer.push_back(22u);
  buffer.push_back(42u);
  EXPECT_EQ(buffer[0], 22u);
  EXPECT_EQ(buffer[1], 42u);
}


TEST(TestCollections_CircularFixedSizeBuffer, opIndex_cont)
{
  CircularFixedSizeBuffer<uint32_t> buffer(32);
  buffer.push_back(22u);
  buffer.push_back(42u);
  const auto& rBuffer = buffer;
  EXPECT_EQ(rBuffer[0], 22u);
  EXPECT_EQ(rBuffer[1], 42u);
}


TEST(TestCollections_CircularFixedSizeBuffer, opIndexWrap)
{
  CircularFixedSizeBuffer<uint32_t> buffer(2);
  buffer.push_back(1u);
  buffer.push_back(2u);
  buffer.push_back(3u);
  EXPECT_EQ(buffer[0], 2u);
  EXPECT_EQ(buffer[1], 3u);
}


TEST(TestCollections_CircularFixedSizeBuffer, opIndex2)
{
  CircularFixedSizeBuffer<uint32_t> buffer(32);
  buffer.push_front(22u);
  buffer.push_front(42u);
  EXPECT_EQ(buffer[0], 42u);
  EXPECT_EQ(buffer[1], 22u);
}


TEST(TestCollections_CircularFixedSizeBuffer, opIndex2_cont)
{
  CircularFixedSizeBuffer<uint32_t> buffer(32);
  buffer.push_front(22u);
  buffer.push_front(42u);
  const auto& rBuffer = buffer;
  EXPECT_EQ(rBuffer[0], 42u);
  EXPECT_EQ(rBuffer[1], 22u);
}

TEST(TestCollections_CircularFixedSizeBuffer, opIndexWrap_cont)
{
  CircularFixedSizeBuffer<uint32_t> buffer(2);
  buffer.push_front(1u);
  buffer.push_front(2u);
  buffer.push_front(3u);
  const auto& rBuffer = buffer;
  EXPECT_EQ(rBuffer[0], 3u);
  EXPECT_EQ(rBuffer[1], 2u);
}


TEST(TestCollections_CircularFixedSizeBuffer, opIndexWrap2)
{
  CircularFixedSizeBuffer<uint32_t> buffer(2);
  buffer.push_front(1u);
  buffer.push_front(2u);
  buffer.push_front(3u);
  EXPECT_EQ(buffer[0], 3u);
  EXPECT_EQ(buffer[1], 2u);
}


TEST(TestCollections_CircularFixedSizeBuffer, opIndexWrap2_cont)
{
  CircularFixedSizeBuffer<uint32_t> buffer(2);
  buffer.push_back(1u);
  buffer.push_back(2u);
  buffer.push_back(3u);
  const auto& rBuffer = buffer;
  EXPECT_EQ(rBuffer[0], 2u);
  EXPECT_EQ(rBuffer[1], 3u);
}


TEST(TestCollections_CircularFixedSizeBuffer, At)
{
  CircularFixedSizeBuffer<uint32_t> buffer(32);
  buffer.push_back(22u);
  buffer.push_back(42u);
  EXPECT_EQ(buffer.at(0), 22u);
  EXPECT_EQ(buffer.at(1), 42u);
}


TEST(TestCollections_CircularFixedSizeBuffer, At_cont)
{
  CircularFixedSizeBuffer<uint32_t> buffer(32);
  buffer.push_back(22u);
  buffer.push_back(42u);
  const auto& rBuffer = buffer;
  EXPECT_EQ(rBuffer.at(0), 22u);
  EXPECT_EQ(rBuffer.at(1), 42u);
}


TEST(TestCollections_CircularFixedSizeBuffer, At_OutOfRange)
{
  CircularFixedSizeBuffer<uint32_t> buffer(32);
  buffer.push_back(22u);
  buffer.push_back(42u);
  EXPECT_EQ(buffer.at(0), 22u);
  EXPECT_EQ(buffer.at(1), 42u);
  EXPECT_THROW(buffer.at(buffer.size()), std::out_of_range);
}


TEST(TestCollections_CircularFixedSizeBuffer, At_OutOfRange_cont)
{
  CircularFixedSizeBuffer<uint32_t> buffer(32);
  buffer.push_back(22u);
  buffer.push_back(42u);
  const auto& rBuffer = buffer;
  EXPECT_EQ(rBuffer.at(0), 22u);
  EXPECT_EQ(rBuffer.at(1), 42u);
  EXPECT_THROW(rBuffer.at(rBuffer.size()), std::out_of_range);
}


TEST(TestCollections_CircularFixedSizeBuffer, AtWrap)
{
  CircularFixedSizeBuffer<uint32_t> buffer(2);
  buffer.push_back(1u);
  buffer.push_back(2u);
  buffer.push_back(3u);
  EXPECT_EQ(buffer.at(0), 2u);
  EXPECT_EQ(buffer.at(1), 3u);
}


TEST(TestCollections_CircularFixedSizeBuffer, At2)
{
  CircularFixedSizeBuffer<uint32_t> buffer(32);
  buffer.push_front(22u);
  buffer.push_front(42u);
  EXPECT_EQ(buffer.at(0), 42u);
  EXPECT_EQ(buffer.at(1), 22u);
}


TEST(TestCollections_CircularFixedSizeBuffer, At2_cont)
{
  CircularFixedSizeBuffer<uint32_t> buffer(32);
  buffer.push_front(22u);
  buffer.push_front(42u);
  const auto& rBuffer = buffer;
  EXPECT_EQ(rBuffer.at(0), 42u);
  EXPECT_EQ(rBuffer.at(1), 22u);
}

TEST(TestCollections_CircularFixedSizeBuffer, AtWrap_cont)
{
  CircularFixedSizeBuffer<uint32_t> buffer(2);
  buffer.push_front(1u);
  buffer.push_front(2u);
  buffer.push_front(3u);
  const auto& rBuffer = buffer;
  EXPECT_EQ(rBuffer.at(0), 3u);
  EXPECT_EQ(rBuffer.at(1), 2u);
}


TEST(TestCollections_CircularFixedSizeBuffer, AtWrap2)
{
  CircularFixedSizeBuffer<uint32_t> buffer(2);
  buffer.push_front(1u);
  buffer.push_front(2u);
  buffer.push_front(3u);
  EXPECT_EQ(buffer.at(0), 3u);
  EXPECT_EQ(buffer.at(1), 2u);
}


TEST(TestCollections_CircularFixedSizeBuffer, AtWrap2_cont)
{
  CircularFixedSizeBuffer<uint32_t> buffer(2);
  buffer.push_back(1u);
  buffer.push_back(2u);
  buffer.push_back(3u);
  const auto& rBuffer = buffer;
  EXPECT_EQ(rBuffer.at(0), 2u);
  EXPECT_EQ(rBuffer.at(1), 3u);
}


TEST(TestCollections_CircularFixedSizeBuffer, Size_Empty)
{
  CircularFixedSizeBuffer<uint32_t> buffer(32);
  buffer.push_back(22u);
  buffer.push_back(42u);
  EXPECT_EQ(buffer.size(), 2u);
}


TEST(TestCollections_CircularFixedSizeBuffer, Size)
{
  CircularFixedSizeBuffer<uint32_t> buffer(32);
  EXPECT_EQ(buffer.size(), 0u);
}

TEST(TestCollections_CircularFixedSizeBuffer, PushFront)
{
  CircularFixedSizeBuffer<uint32_t> buffer(4);
  EXPECT_TRUE(buffer.empty());
  EXPECT_EQ(buffer.size(), 0u);

  const uint32_t v1 = 1u;
  const uint32_t v2 = 2u;
  const uint32_t v3 = 3u;
  const uint32_t v4 = 4u;
  const uint32_t v5 = 5u;

  buffer.push_front(v1);
  EXPECT_FALSE(buffer.empty());
  EXPECT_EQ(buffer.size(), 1u);
  EXPECT_EQ(buffer[0], v1);

  buffer.push_front(v2);
  EXPECT_EQ(buffer.size(), 2u);
  EXPECT_EQ(buffer[0], v2);
  EXPECT_EQ(buffer[1], v1);

  buffer.push_front(v3);
  EXPECT_EQ(buffer.size(), 3u);
  EXPECT_EQ(buffer[0], v3);
  EXPECT_EQ(buffer[1], v2);
  EXPECT_EQ(buffer[2], v1);

  buffer.push_front(v4);
  EXPECT_EQ(buffer.size(), 4u);
  EXPECT_EQ(buffer[0], v4);
  EXPECT_EQ(buffer[1], v3);
  EXPECT_EQ(buffer[2], v2);
  EXPECT_EQ(buffer[3], v1);

  buffer.push_front(v5);
  EXPECT_EQ(buffer.size(), 4u);
  EXPECT_EQ(buffer[0], v5);
  EXPECT_EQ(buffer[1], v4);
  EXPECT_EQ(buffer[2], v3);
  EXPECT_EQ(buffer[3], v2);
}


TEST(TestCollections_CircularFixedSizeBuffer, PushFront_RR)
{
  CircularFixedSizeBuffer<std::unique_ptr<uint32_t>> buffer(4);
  EXPECT_TRUE(buffer.empty());
  EXPECT_EQ(buffer.size(), 0u);

  const uint32_t v1 = 1u;
  const uint32_t v2 = 2u;
  const uint32_t v3 = 3u;
  const uint32_t v4 = 4u;
  const uint32_t v5 = 5u;

  auto obj1 = std::make_unique<uint32_t>(v1);
  auto obj2 = std::make_unique<uint32_t>(v2);
  auto obj3 = std::make_unique<uint32_t>(v3);
  auto obj4 = std::make_unique<uint32_t>(v4);
  auto obj5 = std::make_unique<uint32_t>(v5);

  buffer.push_front(std::move(obj1));
  EXPECT_FALSE(buffer.empty());
  EXPECT_EQ(buffer.size(), 1u);
  EXPECT_TRUE(buffer[0].get() != nullptr);
  EXPECT_EQ(*buffer[0], v1);

  buffer.push_front(std::move(obj2));
  EXPECT_EQ(buffer.size(), 2u);
  EXPECT_TRUE(buffer[0].get() != nullptr);
  EXPECT_TRUE(buffer[1].get() != nullptr);
  EXPECT_EQ(*buffer[0], v2);
  EXPECT_EQ(*buffer[1], v1);

  buffer.push_front(std::move(obj3));
  EXPECT_EQ(buffer.size(), 3u);
  EXPECT_TRUE(buffer[0].get() != nullptr);
  EXPECT_TRUE(buffer[1].get() != nullptr);
  EXPECT_TRUE(buffer[2].get() != nullptr);
  EXPECT_EQ(*buffer[0], v3);
  EXPECT_EQ(*buffer[1], v2);
  EXPECT_EQ(*buffer[2], v1);

  buffer.push_front(std::move(obj4));
  EXPECT_EQ(buffer.size(), 4u);
  EXPECT_TRUE(buffer[0].get() != nullptr);
  EXPECT_TRUE(buffer[1].get() != nullptr);
  EXPECT_TRUE(buffer[2].get() != nullptr);
  EXPECT_TRUE(buffer[3].get() != nullptr);
  EXPECT_EQ(*buffer[0], v4);
  EXPECT_EQ(*buffer[1], v3);
  EXPECT_EQ(*buffer[2], v2);
  EXPECT_EQ(*buffer[3], v1);

  buffer.push_front(std::move(obj5));
  EXPECT_EQ(buffer.size(), 4u);
  EXPECT_TRUE(buffer[0].get() != nullptr);
  EXPECT_TRUE(buffer[1].get() != nullptr);
  EXPECT_TRUE(buffer[2].get() != nullptr);
  EXPECT_TRUE(buffer[3].get() != nullptr);
  EXPECT_EQ(*buffer[0], v5);
  EXPECT_EQ(*buffer[1], v4);
  EXPECT_EQ(*buffer[2], v3);
  EXPECT_EQ(*buffer[3], v2);
}


TEST(TestCollections_CircularFixedSizeBuffer, PushBack)
{
  CircularFixedSizeBuffer<uint32_t> buffer(4);
  EXPECT_TRUE(buffer.empty());
  EXPECT_EQ(buffer.size(), 0u);

  const uint32_t v1 = 1u;
  const uint32_t v2 = 2u;
  const uint32_t v3 = 3u;
  const uint32_t v4 = 4u;
  const uint32_t v5 = 5u;

  buffer.push_back(v1);
  EXPECT_FALSE(buffer.empty());
  EXPECT_EQ(buffer.size(), 1u);
  EXPECT_EQ(buffer[0], v1);

  buffer.push_back(v2);
  EXPECT_EQ(buffer.size(), 2u);
  EXPECT_EQ(buffer[0], v1);
  EXPECT_EQ(buffer[1], v2);

  buffer.push_back(v3);
  EXPECT_EQ(buffer.size(), 3u);
  EXPECT_EQ(buffer[0], v1);
  EXPECT_EQ(buffer[1], v2);
  EXPECT_EQ(buffer[2], v3);

  buffer.push_back(v4);
  EXPECT_EQ(buffer.size(), 4u);
  EXPECT_EQ(buffer[0], v1);
  EXPECT_EQ(buffer[1], v2);
  EXPECT_EQ(buffer[2], v3);
  EXPECT_EQ(buffer[3], v4);

  buffer.push_back(v5);
  EXPECT_EQ(buffer.size(), 4u);
  EXPECT_EQ(buffer[0], v2);
  EXPECT_EQ(buffer[1], v3);
  EXPECT_EQ(buffer[2], v4);
  EXPECT_EQ(buffer[3], v5);
}


TEST(TestCollections_CircularFixedSizeBuffer, PushBack_RR)
{
  CircularFixedSizeBuffer<std::unique_ptr<uint32_t>> buffer(4);
  EXPECT_TRUE(buffer.empty());
  EXPECT_EQ(buffer.size(), 0u);

  const uint32_t v1 = 1u;
  const uint32_t v2 = 2u;
  const uint32_t v3 = 3u;
  const uint32_t v4 = 4u;
  const uint32_t v5 = 5u;

  auto obj1 = std::make_unique<uint32_t>(v1);
  auto obj2 = std::make_unique<uint32_t>(v2);
  auto obj3 = std::make_unique<uint32_t>(v3);
  auto obj4 = std::make_unique<uint32_t>(v4);
  auto obj5 = std::make_unique<uint32_t>(v5);

  buffer.push_back(std::move(obj1));
  EXPECT_FALSE(buffer.empty());
  EXPECT_EQ(buffer.size(), 1u);
  EXPECT_TRUE(buffer[0].get() != nullptr);
  EXPECT_EQ(*buffer[0], v1);

  buffer.push_back(std::move(obj2));
  EXPECT_EQ(buffer.size(), 2u);
  EXPECT_TRUE(buffer[0].get() != nullptr);
  EXPECT_TRUE(buffer[1].get() != nullptr);
  EXPECT_EQ(*buffer[0], v1);
  EXPECT_EQ(*buffer[1], v2);

  buffer.push_back(std::move(obj3));
  EXPECT_EQ(buffer.size(), 3u);
  EXPECT_TRUE(buffer[0].get() != nullptr);
  EXPECT_TRUE(buffer[1].get() != nullptr);
  EXPECT_TRUE(buffer[2].get() != nullptr);
  EXPECT_EQ(*buffer[0], v1);
  EXPECT_EQ(*buffer[1], v2);
  EXPECT_EQ(*buffer[2], v3);

  buffer.push_back(std::move(obj4));
  EXPECT_EQ(buffer.size(), 4u);
  EXPECT_TRUE(buffer[0].get() != nullptr);
  EXPECT_TRUE(buffer[1].get() != nullptr);
  EXPECT_TRUE(buffer[2].get() != nullptr);
  EXPECT_TRUE(buffer[3].get() != nullptr);
  EXPECT_EQ(*buffer[0], v1);
  EXPECT_EQ(*buffer[1], v2);
  EXPECT_EQ(*buffer[2], v3);
  EXPECT_EQ(*buffer[3], v4);

  buffer.push_back(std::move(obj5));
  EXPECT_EQ(buffer.size(), 4u);
  EXPECT_TRUE(buffer[0].get() != nullptr);
  EXPECT_TRUE(buffer[1].get() != nullptr);
  EXPECT_TRUE(buffer[2].get() != nullptr);
  EXPECT_TRUE(buffer[3].get() != nullptr);
  EXPECT_EQ(*buffer[0], v2);
  EXPECT_EQ(*buffer[1], v3);
  EXPECT_EQ(*buffer[2], v4);
  EXPECT_EQ(*buffer[3], v5);
}


TEST(TestCollections_CircularFixedSizeBuffer, PopFront)
{
  CircularFixedSizeBuffer<uint32_t> buffer(2);

  auto v1 = 1u;
  buffer.push_back(v1);

  EXPECT_TRUE(!buffer.empty());

  buffer.pop_front();

  EXPECT_TRUE(buffer.empty());
}


TEST(TestCollections_CircularFixedSizeBuffer, PopFrontDestructs)
{
  CircularFixedSizeBuffer<std::shared_ptr<uint32_t>> buffer(2);

  auto obj1 = std::make_shared<uint32_t>(1u);
  std::weak_ptr<uint32_t> weakObj1 = obj1;

  buffer.push_back(obj1);
  obj1.reset();

  EXPECT_FALSE(weakObj1.expired());

  buffer.pop_front();

  EXPECT_TRUE(weakObj1.expired());
}


TEST(TestCollections_CircularFixedSizeBuffer, PopBack)
{
  CircularFixedSizeBuffer<uint32_t> buffer(2);

  auto v1 = 1u;
  buffer.push_back(v1);

  EXPECT_TRUE(!buffer.empty());

  buffer.pop_back();

  EXPECT_TRUE(buffer.empty());
}


TEST(TestCollections_CircularFixedSizeBuffer, PopBackDestructs)
{
  CircularFixedSizeBuffer<std::shared_ptr<uint32_t>> buffer(2);

  auto obj1 = std::make_shared<uint32_t>(1u);
  std::weak_ptr<uint32_t> weakObj1 = obj1;

  buffer.push_back(obj1);
  obj1.reset();

  EXPECT_FALSE(weakObj1.expired());

  buffer.pop_back();

  EXPECT_TRUE(weakObj1.expired());
}


TEST(TestCollections_CircularFixedSizeBuffer, Grow_Empty)
{
  CircularFixedSizeBuffer<uint32_t> buffer(2);
  EXPECT_TRUE(buffer.empty());
  buffer.grow(1);
  EXPECT_TRUE(buffer.empty());
}

TEST(TestCollections_CircularFixedSizeBuffer, Grow_OneSegment)
{
  CircularFixedSizeBuffer<uint32_t> buffer(2);
  buffer.push_back(1);
  buffer.push_back(2);
  EXPECT_EQ(1u, buffer.segment_count());
  EXPECT_EQ(2u, buffer.size());
  EXPECT_EQ(2u, buffer.capacity());

  buffer.grow(1);
  EXPECT_EQ(1u, buffer.segment_count());
  EXPECT_EQ(2u, buffer.size());
  EXPECT_EQ(3u, buffer.capacity());
}

TEST(TestCollections_CircularFixedSizeBuffer, Grow_TwoSegment_Simple)
{
  const uint32_t v1 = 1u;
  const uint32_t v2 = 2u;
  const uint32_t v3 = 3u;

  CircularFixedSizeBuffer<uint32_t> buffer(2);
  buffer.push_back(v1);
  buffer.push_back(v2);
  buffer.push_back(v3);
  EXPECT_EQ(2u, buffer.segment_count());
  EXPECT_EQ(2u, buffer.size());
  EXPECT_EQ(buffer[0], v2);
  EXPECT_EQ(buffer[1], v3);
  {
    auto seg0 = buffer.AsReadOnlySpan(0);
    auto seg1 = buffer.AsReadOnlySpan(1);
    EXPECT_EQ(1u, seg0.size());
    EXPECT_EQ(1u, seg1.size());
    EXPECT_EQ(seg0[0], v2);
    EXPECT_EQ(seg1[0], v3);
  }
  EXPECT_EQ(2u, buffer.capacity());

  buffer.grow(1);

  EXPECT_EQ(1u, buffer.segment_count());
  EXPECT_EQ(2u, buffer.size());
  EXPECT_EQ(buffer[0], v2);
  EXPECT_EQ(buffer[1], v3);
  {
    auto seg0 = buffer.AsReadOnlySpan(0);
    EXPECT_EQ(2u, seg0.size());
    EXPECT_EQ(seg0[0], v2);
    EXPECT_EQ(seg0[1], v3);
  }
  EXPECT_EQ(3u, buffer.capacity());
}

TEST(TestCollections_CircularFixedSizeBuffer, Grow_TwoSegment)
{
  const uint32_t v1 = 1u;
  const uint32_t v2 = 2u;
  const uint32_t v3 = 3u;
  const uint32_t v4 = 4u;
  const uint32_t v5 = 5u;

  CircularFixedSizeBuffer<uint32_t> buffer(3);
  buffer.push_back(v1);
  buffer.push_back(v2);
  buffer.push_back(v3);
  buffer.push_back(v4);
  buffer.push_back(v5);
  EXPECT_EQ(2u, buffer.segment_count());
  EXPECT_EQ(3u, buffer.size());
  EXPECT_EQ(buffer[0], v3);
  EXPECT_EQ(buffer[1], v4);
  EXPECT_EQ(buffer[2], v5);
  {
    auto seg0 = buffer.AsReadOnlySpan(0);
    auto seg1 = buffer.AsReadOnlySpan(1);
    EXPECT_EQ(1u, seg0.size());
    EXPECT_EQ(2u, seg1.size());
    EXPECT_EQ(seg0[0], v3);
    EXPECT_EQ(seg1[0], v4);
    EXPECT_EQ(seg1[1], v5);
  }
  EXPECT_EQ(3u, buffer.capacity());

  buffer.grow(1);

  EXPECT_EQ(2u, buffer.segment_count());
  EXPECT_EQ(3u, buffer.size());
  EXPECT_EQ(buffer[0], v3);
  EXPECT_EQ(buffer[1], v4);
  EXPECT_EQ(buffer[2], v5);
  {
    auto seg0 = buffer.AsReadOnlySpan(0);
    auto seg1 = buffer.AsReadOnlySpan(1);
    EXPECT_EQ(2u, seg0.size());
    EXPECT_EQ(1u, seg1.size());
    EXPECT_EQ(seg0[0], v3);
    EXPECT_EQ(seg0[1], v4);
    EXPECT_EQ(seg1[0], v5);
  }
  EXPECT_EQ(4u, buffer.capacity());
}


TEST(TestCollections_CircularFixedSizeBuffer, ResizePopFront_Empty_Shrink)
{
  CircularFixedSizeBuffer<uint32_t> buffer(2);
  EXPECT_TRUE(buffer.empty());
  buffer.resize_pop_front(1);
  EXPECT_TRUE(buffer.empty());
  EXPECT_EQ(1u, buffer.capacity());
}

TEST(TestCollections_CircularFixedSizeBuffer, ResizePopFront_Empty_SameSize)
{
  CircularFixedSizeBuffer<uint32_t> buffer(2);
  EXPECT_TRUE(buffer.empty());
  buffer.resize_pop_front(2);
  EXPECT_TRUE(buffer.empty());
  EXPECT_EQ(2u, buffer.capacity());
}

TEST(TestCollections_CircularFixedSizeBuffer, ResizePopFront_Empty_Grow)
{
  CircularFixedSizeBuffer<uint32_t> buffer(2);
  EXPECT_TRUE(buffer.empty());
  buffer.resize_pop_front(3);
  EXPECT_TRUE(buffer.empty());
  EXPECT_EQ(3u, buffer.capacity());
}

TEST(TestCollections_CircularFixedSizeBuffer, ResizePopFront_TwoSegments_Shrink0)
{
  const uint32_t v1 = 1u;
  const uint32_t v2 = 2u;
  const uint32_t v3 = 3u;
  const uint32_t v4 = 4u;

  CircularFixedSizeBuffer<uint32_t> buffer(3);
  buffer.push_back(v1);
  buffer.push_back(v2);
  buffer.push_back(v3);
  buffer.push_back(v4);
  EXPECT_EQ(2u, buffer.segment_count());
  EXPECT_EQ(3u, buffer.size());
  EXPECT_EQ(buffer[0], v2);
  EXPECT_EQ(buffer[1], v3);
  EXPECT_EQ(buffer[2], v4);

  buffer.resize_pop_front(2);

  EXPECT_EQ(1u, buffer.segment_count());
  EXPECT_EQ(2u, buffer.size());
  EXPECT_EQ(buffer[0], v3);
  EXPECT_EQ(buffer[1], v4);
}

TEST(TestCollections_CircularFixedSizeBuffer, ResizePopFront_TwoSegments_Shrink1)
{
  const uint32_t v1 = 1u;
  const uint32_t v2 = 2u;
  const uint32_t v3 = 3u;
  const uint32_t v4 = 4u;
  const uint32_t v5 = 5u;

  CircularFixedSizeBuffer<uint32_t> buffer(3);
  buffer.push_back(v1);
  buffer.push_back(v2);
  buffer.push_back(v3);
  buffer.push_back(v4);
  buffer.push_back(v5);
  EXPECT_EQ(2u, buffer.segment_count());
  EXPECT_EQ(3u, buffer.size());
  EXPECT_EQ(buffer[0], v3);
  EXPECT_EQ(buffer[1], v4);
  EXPECT_EQ(buffer[2], v5);

  buffer.resize_pop_front(2);

  EXPECT_EQ(1u, buffer.segment_count());
  EXPECT_EQ(2u, buffer.size());
  EXPECT_EQ(buffer[0], v4);
  EXPECT_EQ(buffer[1], v5);
}
