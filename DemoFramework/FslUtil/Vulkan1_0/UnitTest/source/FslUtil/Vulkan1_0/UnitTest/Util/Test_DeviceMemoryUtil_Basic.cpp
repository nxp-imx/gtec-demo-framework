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

#include <FslUtil/Vulkan1_0/UnitTest/Helper/Common.hpp>
#include <FslUtil/Vulkan1_0/UnitTest/Helper/TestFixtureFslUtil_Vulkan1_0.hpp>
#include <FslUtil/Vulkan1_0/Util/DeviceMemoryUtil.hpp>

using namespace Fsl;
using namespace Fsl::Vulkan;

namespace
{
  using TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil_Basic = TestFixtureFslUtil_Vulkan1_0;
  using TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil_BasicDeathTest = TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil_Basic;
}


#ifndef NDEBUG
TEST(TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil_BasicDeathTest, CalcMemorySpan_Capacity0_PageSize0_Target0_0)
{
  ASSERT_DEATH_IF_SUPPORTED(DeviceMemoryUtil::CalcMemorySpan(0, 0, 0, 0), "pageSize\\s>\\s0");
}

TEST(TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil_BasicDeathTest, CalcMemorySpan_Capacity1_PageSize0_Target0_0)
{
  ASSERT_DEATH_IF_SUPPORTED(DeviceMemoryUtil::CalcMemorySpan(1, 0, 0, 0), "pageSize\\s>\\s0");
}

TEST(TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil_BasicDeathTest, CalcMemorySpan_Capacity1_PageSize2_Target0_0)
{
  ASSERT_DEATH_IF_SUPPORTED(DeviceMemoryUtil::CalcMemorySpan(1, 2, 0, 0), "pageSize\\s<=\\scapacity");
}

TEST(TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil_BasicDeathTest, CalcMemorySpan_Capacity1_PageSize1_Target1_0)
{
  ASSERT_DEATH_IF_SUPPORTED(DeviceMemoryUtil::CalcMemorySpan(1, 1, 2, 0), "offset\\s<=\\scapacity");
}

TEST(TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil_BasicDeathTest, CalcMemorySpan_Capacity1_PageSize1_Target0_2)
{
  ASSERT_DEATH_IF_SUPPORTED(DeviceMemoryUtil::CalcMemorySpan(1, 1, 0, 2), "length\\s<=\\s\\(capacity\\s-\\soffset\\)");
}

TEST(TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil_BasicDeathTest, CalcMemorySpan_Capacity1_PageSize1_Target1_1)
{
  ASSERT_DEATH_IF_SUPPORTED(DeviceMemoryUtil::CalcMemorySpan(1, 1, 1, 1), "length\\s<=\\s\\(capacity\\s-\\soffset\\)");
}
#endif


TEST(TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil_Basic, CalcMemorySpan_Capacity1_PageSize1_Target0_0)
{
  auto res = DeviceMemoryUtil::CalcMemorySpan(1, 1, 0, 0);
  ASSERT_EQ(0u, res.Touched.Start);
  ASSERT_EQ(0u, res.Touched.Length);
  ASSERT_EQ(0u, res.RelativeStartOffset);
}


TEST(TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil_Basic, CalcMemorySpan_Capacity16_PageSize16_Target0_0)
{
  auto res = DeviceMemoryUtil::CalcMemorySpan(16, 16, 0, 0);
  ASSERT_EQ(0u, res.Touched.Start);
  ASSERT_EQ(0u, res.Touched.Length);
  ASSERT_EQ(0u, res.RelativeStartOffset);
}


TEST(TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil_Basic, CalcMemorySpan_Capacity128_PageSize16_Target1_5)
{
  auto res = DeviceMemoryUtil::CalcMemorySpan(128, 16, 1, 5);
  ASSERT_EQ(0u, res.Touched.Start);
  ASSERT_EQ(16u, res.Touched.Length);
  ASSERT_EQ(1u, res.RelativeStartOffset);
}

TEST(TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil_Basic, CalcMemorySpan_Capacity128_PageSize16_Target15_5)
{
  auto res = DeviceMemoryUtil::CalcMemorySpan(128, 16, 15, 5);
  ASSERT_EQ(0u, res.Touched.Start);
  ASSERT_EQ(32u, res.Touched.Length);
  ASSERT_EQ(15u, res.RelativeStartOffset);
}

TEST(TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil_Basic, CalcMemorySpan_Capacity128_PageSize16_Target16_5)
{
  auto res = DeviceMemoryUtil::CalcMemorySpan(128, 16, 16, 5);
  ASSERT_EQ(16u, res.Touched.Start);
  ASSERT_EQ(16u, res.Touched.Length);
  ASSERT_EQ(0u, res.RelativeStartOffset);
}

TEST(TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil_Basic, CalcMemorySpan_Capacity128_PageSize16_Target17_5)
{
  auto res = DeviceMemoryUtil::CalcMemorySpan(128, 16, 17, 5);
  ASSERT_EQ(16u, res.Touched.Start);
  ASSERT_EQ(16u, res.Touched.Length);
  ASSERT_EQ(1u, res.RelativeStartOffset);
}

TEST(TestFixtureFslUtil_Vulkan1_0_TestDeviceMemoryUtil_Basic, CalcMemorySpan_Capacity128_PageSize16_Target17_25)
{
  auto res = DeviceMemoryUtil::CalcMemorySpan(128, 16, 17, 25);
  ASSERT_EQ(16u, res.Touched.Start);
  ASSERT_EQ(32u, res.Touched.Length);
  ASSERT_EQ(1u, res.RelativeStartOffset);
}
