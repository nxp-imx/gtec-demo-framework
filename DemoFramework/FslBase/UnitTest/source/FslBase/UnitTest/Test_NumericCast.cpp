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

#include <FslBase/NumericCast.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <limits>

namespace
{
  using Test_NumericCast = TestFixtureFslBase;
}

// int32_t

TEST(Test_NumericCast, ToInt32_UInt32)
{
  EXPECT_EQ(0, Fsl::NumericCast<int32_t>(uint32_t(0)));
  EXPECT_EQ(1, Fsl::NumericCast<int32_t>(uint32_t(1)));
  EXPECT_EQ(std::numeric_limits<int32_t>::max(), Fsl::NumericCast<int32_t>(uint32_t(std::numeric_limits<int32_t>::max())));
}


TEST(Test_NumericCast, ToInt32_UInt32_Overflow0)
{
  EXPECT_THROW(Fsl::NumericCast<int32_t>(uint32_t(std::numeric_limits<int32_t>::max()) + 1), Fsl::ConversionException);
}

TEST(Test_NumericCast, ToInt32_UInt32_Overflow1)
{
  EXPECT_THROW(Fsl::NumericCast<int32_t>(std::numeric_limits<uint32_t>::max()), Fsl::ConversionException);
}

// uint32_t

TEST(Test_NumericCast, ToUInt32_Int32)
{
  EXPECT_EQ(0u, Fsl::NumericCast<uint32_t>(int32_t(0)));
  EXPECT_EQ(1u, Fsl::NumericCast<uint32_t>(int32_t(1)));
  EXPECT_EQ(uint32_t(std::numeric_limits<int32_t>::max()), Fsl::NumericCast<uint32_t>(std::numeric_limits<int32_t>::max()));
}

TEST(Test_NumericCast, ToUInt32_Int32_Underflow)
{
  EXPECT_THROW(Fsl::NumericCast<uint32_t>(int32_t(-1)), Fsl::ConversionException);
  EXPECT_THROW(Fsl::NumericCast<uint32_t>(std::numeric_limits<int32_t>::min()), Fsl::ConversionException);
}

TEST(Test_NumericCast, ToUInt32_SizeT)
{
  EXPECT_EQ(0u, Fsl::NumericCast<uint32_t>(std::size_t(0)));
  EXPECT_EQ(1u, Fsl::NumericCast<uint32_t>(std::size_t(1)));
  EXPECT_EQ(std::numeric_limits<uint32_t>::max(), Fsl::NumericCast<uint32_t>(std::size_t(std::numeric_limits<uint32_t>::max())));
}

TEST(Test_NumericCast, ToUInt32_SizeT_Overflow0)
{
  EXPECT_THROW(Fsl::NumericCast<uint32_t>(std::size_t(std::numeric_limits<uint32_t>::max()) + 1u), Fsl::ConversionException);
}

TEST(Test_NumericCast, ToUInt32_SizeT_Overflow1)
{
  EXPECT_THROW(Fsl::NumericCast<uint32_t>(std::numeric_limits<std::size_t>::max()), Fsl::ConversionException);
}
