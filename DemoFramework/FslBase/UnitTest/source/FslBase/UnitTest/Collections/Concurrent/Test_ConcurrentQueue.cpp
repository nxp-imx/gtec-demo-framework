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

#include <FslBase/Collections/Concurrent/ConcurrentQueue.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestCollections_Concurrent_ConcurrentQueue = TestFixtureFslBase;
}


TEST(TestCollections_Concurrent_ConcurrentQueue, Clear_Empty)
{
  ConcurrentQueue<uint32_t> queue;
  queue.Clear();
}


TEST(TestCollections_Concurrent_ConcurrentQueue, TryDequeue_Empty)
{
  ConcurrentQueue<uint32_t> queue;

  uint32_t value = 1u;
  EXPECT_FALSE(queue.TryDequeue(value));
  EXPECT_EQ(0u, value);
}


TEST(TestCollections_Concurrent_ConcurrentQueue, TryDequeWait_Empty)
{
  ConcurrentQueue<uint32_t> queue;

  uint32_t value = 1u;
  EXPECT_FALSE(queue.TryDequeueWait(value, std::chrono::milliseconds(1)));
  EXPECT_EQ(0u, value);
}


TEST(TestCollections_Concurrent_ConcurrentQueue, TryPeek_Empty)
{
  ConcurrentQueue<uint32_t> queue;

  uint32_t value = 1u;
  EXPECT_FALSE(queue.TryPeek(value));
  EXPECT_EQ(0u, value);
}


TEST(TestCollections_Concurrent_ConcurrentQueue, Enqueue)
{
  ConcurrentQueue<uint32_t> queue;
  queue.Enqueue(0x42);
}


TEST(TestCollections_Concurrent_ConcurrentQueue, EnqueueDeque)
{
  ConcurrentQueue<uint32_t> queue;
  queue.Enqueue(0x42);
  EXPECT_EQ(0x42u, queue.Dequeue());
}

TEST(TestCollections_Concurrent_ConcurrentQueue, Enqueue_TryDeque)
{
  ConcurrentQueue<uint32_t> queue;
  queue.Enqueue(0x42);

  uint32_t value = 0u;
  EXPECT_TRUE(queue.TryDequeue(value));
  EXPECT_EQ(0x42u, value);
  EXPECT_FALSE(queue.TryDequeue(value));
  EXPECT_EQ(0u, value);
}


TEST(TestCollections_Concurrent_ConcurrentQueue, Enqueue_TryDequeWait)
{
  ConcurrentQueue<uint32_t> queue;
  queue.Enqueue(0x42);

  uint32_t value = 0;
  EXPECT_TRUE(queue.TryDequeueWait(value, std::chrono::milliseconds(1)));
  EXPECT_EQ(0x42u, value);
  EXPECT_FALSE(queue.TryDequeueWait(value, std::chrono::milliseconds(1)));
  EXPECT_EQ(0u, value);
}


TEST(TestCollections_Concurrent_ConcurrentQueue, Enqueue_TryPeek)
{
  ConcurrentQueue<uint32_t> queue;
  queue.Enqueue(0x42);

  uint32_t value = 0;
  EXPECT_TRUE(queue.TryPeek(value));
  EXPECT_EQ(0x42u, value);
  value = 0;
  EXPECT_TRUE(queue.TryPeek(value));
  EXPECT_EQ(0x42u, value);
}


TEST(TestCollections_Concurrent_ConcurrentQueue, EnqueueClear_TryDeque)
{
  ConcurrentQueue<uint32_t> queue;
  queue.Enqueue(0x42);
  queue.Clear();

  uint32_t value = 1;
  EXPECT_FALSE(queue.TryDequeue(value));
  EXPECT_EQ(0u, value);
}


TEST(TestCollections_Concurrent_ConcurrentQueue, EnqueueClear_TryDequeWait)
{
  ConcurrentQueue<uint32_t> queue;
  queue.Enqueue(0x42);
  queue.Clear();

  uint32_t value = 1;
  EXPECT_FALSE(queue.TryDequeueWait(value, std::chrono::milliseconds(1)));
  EXPECT_EQ(0u, value);
}


TEST(TestCollections_Concurrent_ConcurrentQueue, EnqueueClear_TryPeek)
{
  ConcurrentQueue<uint32_t> queue;
  queue.Enqueue(0x42);
  queue.Clear();

  uint32_t value = 1;
  EXPECT_FALSE(queue.TryPeek(value));
  EXPECT_EQ(0u, value);
}


TEST(TestCollections_Concurrent_ConcurrentQueue, SwapQueue)
{
  ConcurrentQueue<uint32_t> queue;

  const uint32_t val0 = 42;
  const uint32_t val1 = 43;
  const uint32_t val2 = 44;

  queue.Enqueue(val0);
  queue.Enqueue(val1);
  queue.Enqueue(val2);

  std::queue<uint32_t> swapQueue;
  queue.SwapQueue(swapQueue);

  EXPECT_EQ(3u, swapQueue.size());
  EXPECT_EQ(val0, swapQueue.front());
  swapQueue.pop();
  EXPECT_EQ(val1, swapQueue.front());
  swapQueue.pop();
  EXPECT_EQ(val2, swapQueue.front());
  swapQueue.pop();
}


TEST(TestCollections_Concurrent_ConcurrentQueue, SwapEmpty)
{
  ConcurrentQueue<uint32_t> queue;

  std::queue<uint32_t> swapQueue;
  queue.SwapQueue(swapQueue);

  EXPECT_TRUE(swapQueue.empty());
}


TEST(TestCollections_Concurrent_ConcurrentQueue, Extract_Queue)
{
  ConcurrentQueue<uint32_t> queue;

  const uint32_t val0 = 42;
  const uint32_t val1 = 43;
  const uint32_t val2 = 44;

  queue.Enqueue(val0);
  queue.Enqueue(val1);
  queue.Enqueue(val2);

  std::queue<uint32_t> swapQueue;
  queue.Extract(swapQueue);

  EXPECT_EQ(3u, swapQueue.size());
  EXPECT_EQ(val0, swapQueue.front());
  swapQueue.pop();
  EXPECT_EQ(val1, swapQueue.front());
  swapQueue.pop();
  EXPECT_EQ(val2, swapQueue.front());
  swapQueue.pop();
}


TEST(TestCollections_Concurrent_ConcurrentQueue, ExtractEmpty_Queue)
{
  ConcurrentQueue<uint32_t> queue;

  std::queue<uint32_t> swapQueue;
  queue.SwapQueue(swapQueue);

  EXPECT_TRUE(swapQueue.empty());
}


TEST(TestCollections_Concurrent_ConcurrentQueue, Extract_SpanEqual)
{
  ConcurrentQueue<uint32_t> queue;

  const uint32_t val0 = 42;
  const uint32_t val1 = 43;
  const uint32_t val2 = 44;

  queue.Enqueue(val0);
  queue.Enqueue(val1);
  queue.Enqueue(val2);

  std::vector<uint32_t> dst(3);
  const auto written = queue.Extract(SpanUtil::AsSpan(dst));

  EXPECT_EQ(3u, written);
  EXPECT_EQ(val0, dst[0]);
  EXPECT_EQ(val1, dst[1]);
  EXPECT_EQ(val2, dst[2]);
}


TEST(TestCollections_Concurrent_ConcurrentQueue, Extract_SpanLarger)
{
  ConcurrentQueue<uint32_t> queue;

  const uint32_t val0 = 42;
  const uint32_t val1 = 43;
  const uint32_t val2 = 44;

  queue.Enqueue(val0);
  queue.Enqueue(val1);
  queue.Enqueue(val2);

  std::vector<uint32_t> dst(4);
  const auto written = queue.Extract(SpanUtil::AsSpan(dst));

  EXPECT_EQ(3u, written);
  EXPECT_EQ(val0, dst[0]);
  EXPECT_EQ(val1, dst[1]);
  EXPECT_EQ(val2, dst[2]);
}


TEST(TestCollections_Concurrent_ConcurrentQueue, Extract_SpanSmaller)
{
  ConcurrentQueue<uint32_t> queue;

  const uint32_t val0 = 42;
  const uint32_t val1 = 43;
  const uint32_t val2 = 44;

  queue.Enqueue(val0);
  queue.Enqueue(val1);
  queue.Enqueue(val2);

  std::vector<uint32_t> dst(2);
  const auto written = queue.Extract(SpanUtil::AsSpan(dst));

  EXPECT_EQ(2u, written);
  EXPECT_EQ(val0, dst[0]);
  EXPECT_EQ(val1, dst[1]);
}


TEST(TestCollections_Concurrent_ConcurrentQueue, ExtractEmpty_Span)
{
  ConcurrentQueue<uint32_t> queue;

  std::vector<uint32_t> dst(3);
  const auto written = queue.Extract(SpanUtil::AsSpan(dst));

  EXPECT_EQ(0u, written);
}
