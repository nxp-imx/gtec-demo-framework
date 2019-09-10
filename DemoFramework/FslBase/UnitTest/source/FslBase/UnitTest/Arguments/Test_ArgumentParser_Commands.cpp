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
#include "ArgumentParserTestHelper.hpp"


namespace Fsl
{
}

using namespace Fsl;
using namespace Fsl::Arguments;

namespace
{
  using Test_ArgumentParser_Commands = TestFixture;

}


TEST(Test_ArgumentParser_Commands, ArgumentInvalidNames)
{
  std::deque<Command> commands = {Command("t", 42, CommandType::Switch)};
  // Force the argument to become invalid
  commands[0].Name = "";
  commands[0].ShortName = "";
  ASSERT_FALSE(commands[0].IsValid());

  std::array<const char*, 1> testArgs = {"-t"};

  std::deque<EncodedCommand> res;
  ASSERT_EQ(ArgumentParser::TryParse(res, 0, testArgs.data(), commands), ParseResult::CommandListIsInvalidError);
}


TEST(Test_ArgumentParser_Commands, DuplicatedCommandId_Switch_Switch)
{
  std::deque<Command> commands = {Command("a", 42, CommandType::Switch), Command("b", 42, CommandType::Switch)};

  std::array<const char*, 1> testArgs = {"-t"};

  std::deque<EncodedCommand> res;
  ASSERT_EQ(ArgumentParser::TryParse(res, 0, testArgs.data(), commands), ParseResult::CommandListIsInvalidError);
}


TEST(Test_ArgumentParser_Commands, DuplicatedCommandId_Switch_MultiSwitch)
{
  std::deque<Command> commands = {Command("a", 42, CommandType::Switch), Command("b", 42, CommandType::MultiSwitch)};

  std::array<const char*, 1> testArgs = {"-t"};

  std::deque<EncodedCommand> res;
  ASSERT_EQ(ArgumentParser::TryParse(res, 0, testArgs.data(), commands), ParseResult::CommandListIsInvalidError);
}


TEST(Test_ArgumentParser_Commands, DuplicatedCommandId_PositionalValue_RequiredLabeledPositionalValue)
{
  std::deque<Command> commands = {Command("a", 42, CommandType::Switch), Command("b", 42, CommandType::MultiSwitch)};

  std::array<const char*, 1> testArgs = {"-t"};

  std::deque<EncodedCommand> res;
  ASSERT_EQ(ArgumentParser::TryParse(res, 0, testArgs.data(), commands), ParseResult::CommandListIsInvalidError);
}


// We only allow one unhandled argument type
TEST(Test_ArgumentParser_Commands, MultipleUnhandled)
{
  std::deque<Command> commands = {Command("a", 42, CommandType::Unhandled), Command("b", 1, CommandType::Unhandled)};

  std::array<const char*, 1> testArgs = {"-t"};

  std::deque<EncodedCommand> res;
  ASSERT_EQ(ArgumentParser::TryParse(res, 0, testArgs.data(), commands), ParseResult::CommandListIsInvalidError);
}
