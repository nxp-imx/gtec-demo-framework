/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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

#include <FslBase/Collections/Concurrent/ConcurrentBasicVector.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <vector>

using namespace Fsl;

namespace
{
  using TestCollections_Concurrent_ConcurrentBasicVector = TestFixtureFslBase;
}


TEST(TestCollections_Concurrent_ConcurrentBasicVector, Clear_Empty)
{
  ConcurrentBasicVector<uint32_t> vector;
  vector.Clear();

  std::vector<uint32_t> dst;
  vector.ExtractTo(dst);
  EXPECT_EQ(0u, dst.size());
}

TEST(TestCollections_Concurrent_ConcurrentBasicVector, Clear_NonEmpty)
{
  ConcurrentBasicVector<uint32_t> vector;
  vector.Add(0x42);
  vector.Clear();

  std::vector<uint32_t> dst;
  vector.ExtractTo(dst);
  EXPECT_EQ(0u, dst.size());
}

TEST(TestCollections_Concurrent_ConcurrentBasicVector, Add)
{
  ConcurrentBasicVector<uint32_t> vector;
  vector.Add(0x42);
}


TEST(TestCollections_Concurrent_ConcurrentBasicVector, AddExtract1)
{
  ConcurrentBasicVector<uint32_t> vector;
  vector.Add(0x42);

  std::vector<uint32_t> dst;
  vector.ExtractTo(dst);

  EXPECT_EQ(1u, dst.size());
  EXPECT_EQ(0x42u, dst[0]);
}


TEST(TestCollections_Concurrent_ConcurrentBasicVector, AddExtract2)
{
  ConcurrentBasicVector<uint32_t> vector;
  vector.Add(0x42);
  vector.Add(0x43);

  std::vector<uint32_t> dst;
  vector.ExtractTo(dst);

  EXPECT_EQ(2u, dst.size());
  EXPECT_EQ(0x42u, dst[0]);
  EXPECT_EQ(0x43u, dst[1]);
}


TEST(TestCollections_Concurrent_ConcurrentBasicVector, AddExtract2_NonEmptyDst)
{
  ConcurrentBasicVector<uint32_t> vector;
  vector.Add(0x42);
  vector.Add(0x43);

  std::vector<uint32_t> dst;
  dst.push_back(1);

  vector.ExtractTo(dst);

  EXPECT_EQ(3u, dst.size());
  EXPECT_EQ(1u, dst[0]);
  EXPECT_EQ(0x42u, dst[1]);
  EXPECT_EQ(0x43u, dst[2]);
}
