#ifndef FSLBASE_UNITTEST_HELPER_TEST_TESTHELPERTEMPLATE_HPP
#define FSLBASE_UNITTEST_HELPER_TEST_TESTHELPERTEMPLATE_HPP
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

#include <gtest/gtest.h>
// Prevent clang-tidy from a error under Ubuntu
#include <FslBase/UnitTest/Helper/Check/Math/CheckFloat.hpp>
#include <limits>
#include <iomanip>

namespace Fsl
{
  namespace Test
  {
    template <typename T>
    inline ::testing::AssertionResult IsFloatEqual(const T& lhs, const T& rhs)
    {
      if (::Fsl::TestCheck::AlmostEqual(lhs, rhs))
      {
        return ::testing::AssertionSuccess();
      }
      return ::testing::AssertionFailure() << std::setprecision(std::numeric_limits<float>::digits10 + 2) << lhs << " != " << rhs;
    }

    template <typename T>
    inline ::testing::AssertionResult IsFloatNear(const T& lhs, const T& rhs, const float absError)
    {
      if (::Fsl::TestCheck::IsNear(lhs, rhs, absError))
      {
        return ::testing::AssertionSuccess();
      }
      return ::testing::AssertionFailure() << std::setprecision(std::numeric_limits<float>::digits10 + 2) << lhs << " != " << rhs;
    }
  }
}

#endif
