/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslUnitTest/TestFixture.hpp>
#include <FslBase/Arguments/ArgumentParser.hpp>
#include <fmt/format.h>
#include <array>
#include <limits>
#include <ostream>
#include <vector>


namespace Fsl
{
}

using namespace Fsl;
using namespace Fsl::Arguments;

namespace
{
  using Test_Command = TestFixture;

}


TEST(Test_Command, EmptySmartArgument)
{
  ASSERT_THROW(Command("", 42, CommandType::Switch), std::invalid_argument);
}


TEST(Test_Command, EmptyArgument)
{
  ASSERT_THROW(Command("", "", 42, CommandType::Switch), std::invalid_argument);
}

TEST(Test_Command, ShortArgumentTooLong)
{
  ASSERT_THROW(Command("te", "test", 42, CommandType::Switch), std::invalid_argument);
}

TEST(Test_Command, CommandType_Undefined)
{
  ASSERT_THROW(Command("t", "test", 42, CommandType::Undefined), std::invalid_argument);
}


TEST(Test_Command, SmartArgument_CommandType_Undefined1)
{
  ASSERT_THROW(Command("t", 42, CommandType::Undefined), std::invalid_argument);
}


TEST(Test_Command, SmartArgument_CommandType_Undefined2)
{
  ASSERT_THROW(Command("test", 42, CommandType::Undefined), std::invalid_argument);
}
