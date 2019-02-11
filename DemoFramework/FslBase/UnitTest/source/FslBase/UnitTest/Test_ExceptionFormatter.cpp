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

#include <FslBase/ExceptionMessageFormatter.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  const auto MAGIC_FORMATTED_EXCEPTION = "Custom Exception Message";

  using Test_ExceptionMessageFormatter = TestFixtureFslBase;

  bool TryFormatException(const std::exception& ex, std::string& rMessage)
  {
    auto pEx = dynamic_cast<const IndexOutOfRangeException*>(&ex);
    if (pEx == nullptr)
    {
      return false;
    }
    rMessage = MAGIC_FORMATTED_EXCEPTION;
    return true;
  }
}


TEST(Test_ExceptionMessageFormatter, TryFormatException_Empty)
{
  ExceptionMessageFormatter formatter;

  IndexOutOfRangeException ex;
  std::string message("foo");
  EXPECT_FALSE(formatter.TryFormatException(ex, message));
  EXPECT_EQ("foo", message);
}


TEST(Test_ExceptionMessageFormatter, TryFormatException)
{
  ExceptionMessageFormatter formatter;
  formatter.Add(TryFormatException);

  IndexOutOfRangeException ex;
  std::string message("foo");
  EXPECT_TRUE(formatter.TryFormatException(ex, message));
  EXPECT_EQ(MAGIC_FORMATTED_EXCEPTION, message);
}


TEST(Test_ExceptionMessageFormatter, TryFormatException_NoMatch)
{
  ExceptionMessageFormatter formatter;
  formatter.Add(TryFormatException);

  UsageErrorException ex;
  std::string message("foo");
  EXPECT_FALSE(formatter.TryFormatException(ex, message));
  EXPECT_EQ("foo", message);
}
