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

#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <cmath>
#include <utility>
#include "../UTWrappedTypedDependencyPropertyMethods.hpp"
#include "../UTWrappedTypedDependencyPropertyRefMethods.hpp"

using namespace Fsl;

namespace
{
  using Test_Wrapped = TestFixtureFslBase;
}

TEST(Test_Wrapped, Construct)
{
  UTWrappedTypedDependencyPropertyMethods<int32_t> test;
  EXPECT_EQ(0, test.Get());
  EXPECT_EQ(0, test.Property.Get());
}

TEST(Test_Wrapped, ConstructWithValue)
{
  UTWrappedTypedDependencyPropertyMethods<int32_t> test(32);
  EXPECT_EQ(32, test.Get());
  EXPECT_EQ(32, test.Property.Get());
}

TEST(Test_Wrapped, Set)
{
  UTWrappedTypedDependencyPropertyMethods<int32_t> test(32);
  EXPECT_EQ(32, test.Get());
  EXPECT_EQ(32, test.Property.Get());

  EXPECT_TRUE(test.Property.Set(42));
  EXPECT_EQ(42, test.Get());
  EXPECT_EQ(42, test.Property.Get());
}


TEST(Test_Wrapped, ConstructRef)
{
  UTWrappedTypedDependencyPropertyRefMethods<int32_t> test;
  EXPECT_EQ(0, test.Get());
  EXPECT_EQ(0, test.Property.Get());
}

TEST(Test_Wrapped, ConstructRefWithValue)
{
  UTWrappedTypedDependencyPropertyRefMethods<int32_t> test(32);
  EXPECT_EQ(32, test.Get());
  EXPECT_EQ(32, test.Property.Get());
}

TEST(Test_Wrapped, SetRef)
{
  UTWrappedTypedDependencyPropertyRefMethods<int32_t> test(32);
  EXPECT_EQ(32, test.Get());
  EXPECT_EQ(32, test.Property.Get());

  EXPECT_TRUE(test.Property.Set(42));
  EXPECT_EQ(42, test.Get());
  EXPECT_EQ(42, test.Property.Get());
}
