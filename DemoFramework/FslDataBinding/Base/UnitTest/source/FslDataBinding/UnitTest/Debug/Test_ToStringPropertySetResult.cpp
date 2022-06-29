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
#include <FslDataBinding/Base/Debug/ToStringPropertySetResult.hpp>

namespace
{
  using TestDebug_PropertySetResult = TestFixtureFslBase;
}

TEST(TestDebug_PropertySetResult, TryToString)
{
  EXPECT_TRUE(Fsl::DataBinding::Debug::TryToString(static_cast<Fsl::DataBinding::Internal::PropertySetResult>(0xFFFFFF)).empty());
}

TEST(TestDebug_PropertySetResult, ToString)
{
  EXPECT_EQ("PropertySetResult::ValueUnchanged", Fsl::DataBinding::Debug::ToString(Fsl::DataBinding::Internal::PropertySetResult::ValueUnchanged));
  EXPECT_EQ("PropertySetResult::ValueChanged", Fsl::DataBinding::Debug::ToString(Fsl::DataBinding::Internal::PropertySetResult::ValueChanged));
  EXPECT_EQ("PropertySetResult::UnsupportedGetType",
            Fsl::DataBinding::Debug::ToString(Fsl::DataBinding::Internal::PropertySetResult::UnsupportedGetType));
  EXPECT_EQ("PropertySetResult::UnsupportedSetType",
            Fsl::DataBinding::Debug::ToString(Fsl::DataBinding::Internal::PropertySetResult::UnsupportedSetType));
  EXPECT_EQ("PropertySetResult::NotSupported", Fsl::DataBinding::Debug::ToString(Fsl::DataBinding::Internal::PropertySetResult::NotSupported));
  EXPECT_EQ("PropertySetResult::???", Fsl::DataBinding::Debug::ToString(static_cast<Fsl::DataBinding::Internal::PropertySetResult>(0xFFFFFF)));
}
