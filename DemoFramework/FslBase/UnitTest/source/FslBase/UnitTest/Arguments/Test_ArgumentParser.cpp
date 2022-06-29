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

#include <FslBase/Arguments/ArgumentParser.hpp>
#include <FslBase/Log/String/LogStringViewLite.hpp>
#include <FslUnitTest/TestFixture.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <array>
#include <limits>
#include <ostream>
#include <vector>
#include "ArgumentParserTestHelper.hpp"

using namespace Fsl;
using namespace Fsl::Arguments;

namespace
{
  using Test_ArgumentParser = TestFixture;
}


// TEST(Test_ArgumentParser, ArgsNullptr)
//{
//  std::deque<Command> commands = {Command("a", 42, CommandType::Switch)};
//
//  std::deque<EncodedCommand> res;
//  ASSERT_EQ(ArgumentParser::TryParse(res, 0, nullptr, commands), ParseResult::InvalidArguments);
//}


// TEST(Test_ArgumentParser, ArgsNegative)
//{
//  std::array<StringViewLite, 1> testArgs = {"-a"};
//  std::deque<Command> commands = {Command("a", 42, CommandType::Switch)};
//
//  std::deque<EncodedCommand> res;
//  ASSERT_EQ(ArgumentParser::TryParse(res, -1, testArgs.data(), commands), ParseResult::InvalidArguments);
//}


TEST(Test_ArgumentParser, Switch_a_none)
{
  // std::array<StringViewLite, 1> testArgs = {};

  std::deque<Command> commands = {Command("a", 42, CommandType::Switch)};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(ArgumentParser::TryParse(res, ReadOnlySpan<StringViewLite>(), commands), ParseResult::Completed);

  ASSERT_EQ(res.size(), 0u);
}


TEST(Test_ArgumentParser, Switch_a_InvalidFormat)
{
  std::array<StringViewLite, 1> testArgs = {"---a"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Switch)};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::ArgumentFormatError);

  ASSERT_EQ(res.size(), 0u);
}


TEST(Test_ArgumentParser, Switch_a_EmptyArgument)
{
  std::array<StringViewLite, 2> testArgs = {"-", "a"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Switch)};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::ArgumentFormatError);

  ASSERT_EQ(res.size(), 0u);
}


TEST(Test_ArgumentParser, Switch_a_UnknownArg)
{
  std::array<StringViewLite, 1> testArgs = {"-b"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Switch)};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::UnknownArgumentError);

  ASSERT_EQ(res.size(), 0u);
}


TEST(Test_ArgumentParser, Switch_a)
{
  std::array<StringViewLite, 1> testArgs = {"-a"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Switch)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_TRUE(res[0].StrOptArg.empty());
}


TEST(Test_ArgumentParser, Switch_a_UnknownArgument)
{
  std::array<StringViewLite, 2> testArgs = {"-a", "test"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Switch)};

  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::UnknownArgumentError);
  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_TRUE(res[0].StrOptArg.empty());
}


// TEST(Test_ArgumentParser, Switch_a_nullptr)
//{
//  std::array<StringViewLite, 2> testArgs = {"-a", nullptr};
//
//  std::deque<Command> commands = {Command("a", 42, CommandType::Switch)};
//
//  std::deque<EncodedCommand> res;
//  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::ArgumentListContainedNullError);
//  ASSERT_EQ(res.size(), 1u);
//  EXPECT_EQ(res[0].Type, commands[0].Type);
//  EXPECT_EQ(res[0].Id, commands[0].Id);
//  EXPECT_EQ(res[0].Count, 1u);
//  ASSERT_TRUE(res[0].StrOptArg.empty());
//}


TEST(Test_ArgumentParser, Switch2X_aa)
{
  std::array<StringViewLite, 1> testArgs = {"-aa"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Switch)};

  std::deque<EncodedCommand> res;
  EXPECT_EQ(TryParseNow(res, testArgs, commands), ParseResult::DuplicatedSwitchArgumentError);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_TRUE(res[0].StrOptArg.empty());
}


TEST(Test_ArgumentParser, Switch2X_a_a)
{
  std::array<StringViewLite, 2> testArgs = {"-a", "-a"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Switch)};

  std::deque<EncodedCommand> res;
  EXPECT_EQ(TryParseNow(res, testArgs, commands), ParseResult::DuplicatedSwitchArgumentError);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_TRUE(res[0].StrOptArg.empty());
}


TEST(Test_ArgumentParser, Switch2X_aba)
{
  std::array<StringViewLite, 1> testArgs = {"-aba"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Switch), Command("b", 1, CommandType::Switch)};

  std::deque<EncodedCommand> res;
  EXPECT_EQ(TryParseNow(res, testArgs, commands), ParseResult::DuplicatedSwitchArgumentError);

  ASSERT_EQ(res.size(), 2u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_TRUE(res[0].StrOptArg.empty());

  EXPECT_EQ(res[1].Type, commands[1].Type);
  EXPECT_EQ(res[1].Id, commands[1].Id);
  EXPECT_EQ(res[1].Count, 1u);
  ASSERT_TRUE(res[1].StrOptArg.empty());
}


TEST(Test_ArgumentParser, Switch2X_a_ba)
{
  std::array<StringViewLite, 2> testArgs = {"-a", "-ba"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Switch), Command("b", 1, CommandType::Switch)};

  std::deque<EncodedCommand> res;
  EXPECT_EQ(TryParseNow(res, testArgs, commands), ParseResult::DuplicatedSwitchArgumentError);

  ASSERT_EQ(res.size(), 2u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_TRUE(res[0].StrOptArg.empty());

  EXPECT_EQ(res[1].Type, commands[1].Type);
  EXPECT_EQ(res[1].Id, commands[1].Id);
  EXPECT_EQ(res[1].Count, 1u);
  ASSERT_TRUE(res[1].StrOptArg.empty());
}


TEST(Test_ArgumentParser, Switch2X_ab_a)
{
  std::array<StringViewLite, 2> testArgs = {"-ab", "-b"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Switch), Command("b", 1, CommandType::Switch)};

  std::deque<EncodedCommand> res;
  EXPECT_EQ(TryParseNow(res, testArgs, commands), ParseResult::DuplicatedSwitchArgumentError);

  ASSERT_EQ(res.size(), 2u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_TRUE(res[0].StrOptArg.empty());

  EXPECT_EQ(res[1].Type, commands[1].Type);
  EXPECT_EQ(res[1].Id, commands[1].Id);
  EXPECT_EQ(res[1].Count, 1u);
  ASSERT_TRUE(res[1].StrOptArg.empty());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Multi switch
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_ArgumentParser, MultiSwitch_a)
{
  std::array<StringViewLite, 1> testArgs = {"-a"};

  std::deque<Command> commands = {Command("a", 42, CommandType::MultiSwitch)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_TRUE(res[0].StrOptArg.empty());
}


TEST(Test_ArgumentParser, MultiSwitch_aa)
{
  std::array<StringViewLite, 1> testArgs = {"-aa"};

  std::deque<Command> commands = {Command("a", 42, CommandType::MultiSwitch)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 2u);
  ASSERT_TRUE(res[0].StrOptArg.empty());
}


TEST(Test_ArgumentParser, MultiSwitch_a_a)
{
  std::array<StringViewLite, 2> testArgs = {"-a", "-a"};

  std::deque<Command> commands = {Command("a", 42, CommandType::MultiSwitch)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 2u);
  ASSERT_TRUE(res[0].StrOptArg.empty());
}


TEST(Test_ArgumentParser, TwoMultiSwitch_ab)
{
  std::array<StringViewLite, 1> testArgs = {"-ab"};

  std::deque<Command> commands = {Command("a", 42, CommandType::MultiSwitch), Command("b", 1, CommandType::MultiSwitch)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 2u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_TRUE(res[0].StrOptArg.empty());

  EXPECT_EQ(res[1].Type, commands[1].Type);
  EXPECT_EQ(res[1].Id, commands[1].Id);
  EXPECT_EQ(res[1].Count, 1u);
  ASSERT_TRUE(res[1].StrOptArg.empty());
}


TEST(Test_ArgumentParser, TwoMultiSwitch_a_b)
{
  std::array<StringViewLite, 2> testArgs = {"-a", "-b"};

  std::deque<Command> commands = {Command("a", 42, CommandType::MultiSwitch), Command("b", 1, CommandType::MultiSwitch)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 2u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_TRUE(res[0].StrOptArg.empty());

  EXPECT_EQ(res[1].Type, commands[1].Type);
  EXPECT_EQ(res[1].Id, commands[1].Id);
  EXPECT_EQ(res[1].Count, 1u);
  ASSERT_TRUE(res[1].StrOptArg.empty());
}


TEST(Test_ArgumentParser, TwoMultiSwitch_aba)
{
  std::array<StringViewLite, 1> testArgs = {"-aba"};

  std::deque<Command> commands = {Command("a", 42, CommandType::MultiSwitch), Command("b", 1, CommandType::MultiSwitch)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 2u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 2u);
  ASSERT_TRUE(res[0].StrOptArg.empty());

  EXPECT_EQ(res[1].Type, commands[1].Type);
  EXPECT_EQ(res[1].Id, commands[1].Id);
  EXPECT_EQ(res[1].Count, 1u);
  ASSERT_TRUE(res[1].StrOptArg.empty());
}


TEST(Test_ArgumentParser, TwoMultiSwitch_a_ba)
{
  std::array<StringViewLite, 2> testArgs = {"-a", "-ba"};

  std::deque<Command> commands = {Command("a", 42, CommandType::MultiSwitch), Command("b", 1, CommandType::MultiSwitch)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 2u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 2u);
  ASSERT_TRUE(res[0].StrOptArg.empty());

  EXPECT_EQ(res[1].Type, commands[1].Type);
  EXPECT_EQ(res[1].Id, commands[1].Id);
  EXPECT_EQ(res[1].Count, 1u);
  ASSERT_TRUE(res[1].StrOptArg.empty());
}


TEST(Test_ArgumentParser, TwoMultiSwitch_ab_a)
{
  std::array<StringViewLite, 2> testArgs = {"-ab", "-a"};

  std::deque<Command> commands = {Command("a", 42, CommandType::MultiSwitch), Command("b", 1, CommandType::MultiSwitch)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 2u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 2u);
  ASSERT_TRUE(res[0].StrOptArg.empty());

  EXPECT_EQ(res[1].Type, commands[1].Type);
  EXPECT_EQ(res[1].Id, commands[1].Id);
  EXPECT_EQ(res[1].Count, 1u);
  ASSERT_TRUE(res[1].StrOptArg.empty());
}


TEST(Test_ArgumentParser, TwoMultiSwitch_a_b_a)
{
  std::array<StringViewLite, 3> testArgs = {"-a", "-b", "-a"};

  std::deque<Command> commands = {Command("a", 42, CommandType::MultiSwitch), Command("b", 1, CommandType::MultiSwitch)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 2u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 2u);
  ASSERT_TRUE(res[0].StrOptArg.empty());

  EXPECT_EQ(res[1].Type, commands[1].Type);
  EXPECT_EQ(res[1].Id, commands[1].Id);
  EXPECT_EQ(res[1].Count, 1u);
  ASSERT_TRUE(res[1].StrOptArg.empty());
}


TEST(Test_ArgumentParser, TwoMultiSwitch_bab)
{
  std::array<StringViewLite, 1> testArgs = {"-bab"};

  std::deque<Command> commands = {Command("a", 42, CommandType::MultiSwitch), Command("b", 1, CommandType::MultiSwitch)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 2u);
  EXPECT_EQ(res[0].Type, commands[1].Type);
  EXPECT_EQ(res[0].Id, commands[1].Id);
  EXPECT_EQ(res[0].Count, 2u);
  ASSERT_TRUE(res[0].StrOptArg.empty());

  EXPECT_EQ(res[1].Type, commands[0].Type);
  EXPECT_EQ(res[1].Id, commands[0].Id);
  EXPECT_EQ(res[1].Count, 1u);
  ASSERT_TRUE(res[1].StrOptArg.empty());
}


TEST(Test_ArgumentParser, TwoMultiSwitch_ba_b)
{
  std::array<StringViewLite, 2> testArgs = {"-ba", "-b"};

  std::deque<Command> commands = {Command("a", 42, CommandType::MultiSwitch), Command("b", 1, CommandType::MultiSwitch)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 2u);
  EXPECT_EQ(res[0].Type, commands[1].Type);
  EXPECT_EQ(res[0].Id, commands[1].Id);
  EXPECT_EQ(res[0].Count, 2u);
  ASSERT_TRUE(res[0].StrOptArg.empty());

  EXPECT_EQ(res[1].Type, commands[0].Type);
  EXPECT_EQ(res[1].Id, commands[0].Id);
  EXPECT_EQ(res[1].Count, 1u);
  ASSERT_TRUE(res[1].StrOptArg.empty());
}


TEST(Test_ArgumentParser, TwoMultiSwitch_b_ab)
{
  std::array<StringViewLite, 2> testArgs = {"-b", "-ab"};

  std::deque<Command> commands = {Command("a", 42, CommandType::MultiSwitch), Command("b", 1, CommandType::MultiSwitch)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 2u);
  EXPECT_EQ(res[0].Type, commands[1].Type);
  EXPECT_EQ(res[0].Id, commands[1].Id);
  EXPECT_EQ(res[0].Count, 2u);
  ASSERT_TRUE(res[0].StrOptArg.empty());

  EXPECT_EQ(res[1].Type, commands[0].Type);
  EXPECT_EQ(res[1].Id, commands[0].Id);
  EXPECT_EQ(res[1].Count, 1u);
  ASSERT_TRUE(res[1].StrOptArg.empty());
}


TEST(Test_ArgumentParser, TwoMultiSwitch_b_a_b)
{
  std::array<StringViewLite, 3> testArgs = {"-b", "-a", "-b"};

  std::deque<Command> commands = {Command("a", 42, CommandType::MultiSwitch), Command("b", 1, CommandType::MultiSwitch)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 2u);
  EXPECT_EQ(res[0].Type, commands[1].Type);
  EXPECT_EQ(res[0].Id, commands[1].Id);
  EXPECT_EQ(res[0].Count, 2u);
  ASSERT_TRUE(res[0].StrOptArg.empty());

  EXPECT_EQ(res[1].Type, commands[0].Type);
  EXPECT_EQ(res[1].Id, commands[0].Id);
  EXPECT_EQ(res[1].Count, 1u);
  ASSERT_TRUE(res[1].StrOptArg.empty());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_ArgumentParser, Value_a_missing)
{
  std::array<StringViewLite, 1> testArgs = {"-a"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Value)};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::ArgumentMissingValueError);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_TRUE(res[0].StrOptArg.empty());
}


TEST(Test_ArgumentParser, Value_a)
{
  std::array<StringViewLite, 2> testArgs = {"-a", "hello"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Value)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  EXPECT_EQ(res[0].StrOptArg, testArgs[1]);
}

TEST(Test_ArgumentParser, Value_test)
{
  std::array<StringViewLite, 2> testArgs = {"--test", "hello"};

  std::deque<Command> commands = {Command("test", 42, CommandType::Value)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  EXPECT_EQ(res[0].StrOptArg, testArgs[1]);
}


TEST(Test_ArgumentParser, Value_a_EmptyValue)
{
  std::array<StringViewLite, 2> testArgs = {"-a", ""};

  std::deque<Command> commands = {Command("a", 42, CommandType::Value)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  EXPECT_EQ(res[0].StrOptArg, testArgs[1]);
}


TEST(Test_ArgumentParser, Value_AValueBValue_ValueNotLast)
{
  std::array<StringViewLite, 2> testArgs = {"-ab", "hey"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Value), Command("b", 1, CommandType::Value)};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::CombinedValueArgumentMustBeLastError);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_TRUE(res[0].StrOptArg.empty());
}

TEST(Test_ArgumentParser, Value_ab_ValueNotLast)
{
  std::array<StringViewLite, 2> testArgs = {"-ab", "hey"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Value), Command("b", 1, CommandType::Switch)};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::CombinedValueArgumentMustBeLastError);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_TRUE(res[0].StrOptArg.empty());
}

TEST(Test_ArgumentParser, Value_ab_ValueNotLastAndValueArgMissing)
{
  std::array<StringViewLite, 1> testArgs = {"-ab"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Value), Command("b", 1, CommandType::Switch)};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::CombinedValueArgumentMustBeLastError);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_TRUE(res[0].StrOptArg.empty());
}

TEST(Test_ArgumentParser, Value_ba_ValueArgMissing)
{
  std::array<StringViewLite, 1> testArgs = {"-ba"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Value), Command("b", 1, CommandType::Switch)};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::ArgumentMissingValueError);

  ASSERT_EQ(res.size(), 2u);
  EXPECT_EQ(res[0].Type, commands[1].Type);
  EXPECT_EQ(res[0].Id, commands[1].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_TRUE(res[0].StrOptArg.empty());

  EXPECT_EQ(res[1].Type, commands[0].Type);
  EXPECT_EQ(res[1].Id, commands[0].Id);
  EXPECT_EQ(res[1].Count, 1u);
  ASSERT_TRUE(res[1].StrOptArg.empty());
}


TEST(Test_ArgumentParser, Value_ba)
{
  std::array<StringViewLite, 2> testArgs = {"-ba", "hey"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Value), Command("b", 1, CommandType::Switch)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 2u);

  EXPECT_EQ(res[0].Type, commands[1].Type);
  EXPECT_EQ(res[0].Id, commands[1].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_TRUE(res[0].StrOptArg.empty());

  EXPECT_EQ(res[1].Type, commands[0].Type);
  EXPECT_EQ(res[1].Id, commands[0].Id);
  EXPECT_EQ(res[1].Count, 1u);
  EXPECT_EQ(res[1].StrOptArg, testArgs[1]);
}


TEST(Test_ArgumentParser, Value_a_duplicated)
{
  std::array<StringViewLite, 4> testArgs = {"-a", "1", "-a", "2"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Value)};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::DuplicatedValueArgumentError);

  ASSERT_EQ(res.size(), 1u);

  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_EQ(res[0].StrOptArg, testArgs[1]);
}


TEST(Test_ArgumentParser, Value_test_duplicated)
{
  std::array<StringViewLite, 4> testArgs = {"--test", "1", "--test", "2"};

  std::deque<Command> commands = {Command("test", 42, CommandType::Value)};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::DuplicatedValueArgumentError);

  ASSERT_EQ(res.size(), 1u);

  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_EQ(res[0].StrOptArg, testArgs[1]);
}


TEST(Test_ArgumentParser, Value_a_test_duplicated)
{
  std::array<StringViewLite, 4> testArgs = {"-a", "1", "--test", "2"};

  std::deque<Command> commands = {Command("a", "test", 42, CommandType::Value)};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::DuplicatedValueArgumentError);

  ASSERT_EQ(res.size(), 1u);

  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_EQ(res[0].StrOptArg, testArgs[1]);
}


TEST(Test_ArgumentParser, Value_test_a_duplicated)
{
  std::array<StringViewLite, 4> testArgs = {"--test", "1", "-a", "2"};

  std::deque<Command> commands = {Command("a", "test", 42, CommandType::Value)};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::DuplicatedValueArgumentError);

  ASSERT_EQ(res.size(), 1u);

  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_EQ(res[0].StrOptArg, testArgs[1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Long switch
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_ArgumentParser, LongSwitch_test_AsShortSwitch)
{
  std::array<StringViewLite, 1> testArgs = {"-test"};

  std::deque<Command> commands = {Command("test", 42, CommandType::Switch)};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::UnknownArgumentError);

  ASSERT_EQ(res.size(), 0u);
}


TEST(Test_ArgumentParser, LongSwitch_test_Unknown)
{
  std::array<StringViewLite, 1> testArgs = {"--test2"};

  std::deque<Command> commands = {Command("test", 42, CommandType::Switch)};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::UnknownArgumentError);

  ASSERT_EQ(res.size(), 0u);
}


TEST(Test_ArgumentParser, LongSwitch_test)
{
  std::array<StringViewLite, 1> testArgs = {"--test"};

  std::deque<Command> commands = {Command("test", 42, CommandType::Switch)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_TRUE(res[0].StrOptArg.empty());
}


TEST(Test_ArgumentParser, LongSwitch_test_test)
{
  std::array<StringViewLite, 2> testArgs = {"--test", "--test"};

  std::deque<Command> commands = {Command("test", 42, CommandType::Switch)};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::DuplicatedSwitchArgumentError);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_TRUE(res[0].StrOptArg.empty());
}


TEST(Test_ArgumentParser, LongSwitch_test_test_invalid)
{
  std::array<StringViewLite, 1> testArgs = {"--test --test"};

  std::deque<Command> commands = {Command("test", 42, CommandType::Switch)};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::UnknownArgumentError);

  ASSERT_EQ(res.size(), 0u);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Long multi switch
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_ArgumentParser, LongMultiSwitch_test_AsShortSwitch)
{
  std::array<StringViewLite, 1> testArgs = {"-test"};

  std::deque<Command> commands = {Command("test", 42, CommandType::MultiSwitch)};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::UnknownArgumentError);

  ASSERT_EQ(res.size(), 0u);
}


TEST(Test_ArgumentParser, LongMultiSwitch_test_Unknown)
{
  std::array<StringViewLite, 1> testArgs = {"--test2"};

  std::deque<Command> commands = {Command("test", 42, CommandType::MultiSwitch)};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::UnknownArgumentError);

  ASSERT_EQ(res.size(), 0u);
}


TEST(Test_ArgumentParser, LongMultiSwitch_test)
{
  std::array<StringViewLite, 1> testArgs = {"--test"};

  std::deque<Command> commands = {Command("test", 42, CommandType::MultiSwitch)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  ASSERT_TRUE(res[0].StrOptArg.empty());
}


TEST(Test_ArgumentParser, LongMultiSwitch_test_test)
{
  std::array<StringViewLite, 2> testArgs = {"--test", "--test"};

  std::deque<Command> commands = {Command("test", 42, CommandType::MultiSwitch)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 2u);
  ASSERT_TRUE(res[0].StrOptArg.empty());
}


TEST(Test_ArgumentParser, LongMultiSwitch_test_test_invalid)
{
  std::array<StringViewLite, 1> testArgs = {"--test --test"};

  std::deque<Command> commands = {Command("test", 42, CommandType::MultiSwitch)};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::UnknownArgumentError);

  ASSERT_EQ(res.size(), 0u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// PositionalValue arguments
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_ArgumentParser, PositionalValue_hello)
{
  std::array<StringViewLite, 1> testArgs = {"hello"};

  std::deque<Command> commands = {Command("pos1", 42, CommandType::PositionalValue)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  EXPECT_EQ(res[0].StrOptArg, testArgs[0]);
}


TEST(Test_ArgumentParser, PositionalValue_hello_world)
{
  std::array<StringViewLite, 2> testArgs = {"hello", "world"};

  std::deque<Command> commands = {Command("pos1", 42, CommandType::PositionalValue), Command("pos2", 1, CommandType::PositionalValue)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 2u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  EXPECT_EQ(res[0].StrOptArg, testArgs[0]);

  EXPECT_EQ(res[1].Type, commands[1].Type);
  EXPECT_EQ(res[1].Id, commands[1].Id);
  EXPECT_EQ(res[1].Count, 1u);
  EXPECT_EQ(res[1].StrOptArg, testArgs[1]);
}


TEST(Test_ArgumentParser, PositionalValue_NotAllowed)
{
  std::array<StringViewLite, 1> testArgs = {"hello"};

  std::deque<Command> commands = {};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::UnknownArgumentError);

  ASSERT_EQ(res.size(), 0u);
}

TEST(Test_ArgumentParser, PositionalValue_NotSpecified)
{
  std::array<StringViewLite, 1> testArgs = {"-v"};

  std::deque<Command> commands = {Command("pos1", 42, CommandType::PositionalValue), Command("v", 43, CommandType::Switch)};

  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[1].Type);
  EXPECT_EQ(res[0].Id, commands[1].Id);
  EXPECT_EQ(res[0].Count, 1u);
  EXPECT_EQ(res[0].StrOptArg, nullptr);
}


TEST(Test_ArgumentParser, PositionalValue_2x_hello_NotSpecified)
{
  std::array<StringViewLite, 1> testArgs = {"hello"};

  std::deque<Command> commands = {Command("pos1", 42, CommandType::PositionalValue), Command("pos2", 1, CommandType::PositionalValue)};

  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  EXPECT_EQ(res[0].StrOptArg, testArgs[0]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// RequiredPositionalValue arguments
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_ArgumentParser, RequiredPositionalValue_hello)
{
  std::array<StringViewLite, 1> testArgs = {"hello"};

  std::deque<Command> commands = {Command("pos1", 42, CommandType::PositionalValue, true)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  EXPECT_EQ(res[0].StrOptArg, testArgs[0]);
}


TEST(Test_ArgumentParser, RequiredPositionalValue_hello_world)
{
  std::array<StringViewLite, 2> testArgs = {"hello", "world"};

  std::deque<Command> commands = {Command("pos1", 42, CommandType::PositionalValue, true), Command("pos2", 1, CommandType::PositionalValue, true)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 2u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  EXPECT_EQ(res[0].StrOptArg, testArgs[0]);

  EXPECT_EQ(res[1].Type, commands[1].Type);
  EXPECT_EQ(res[1].Id, commands[1].Id);
  EXPECT_EQ(res[1].Count, 1u);
  EXPECT_EQ(res[1].StrOptArg, testArgs[1]);
}


TEST(Test_ArgumentParser, RequiredPositionalValue_NotSpecified)
{
  std::array<StringViewLite, 1> testArgs = {"-v"};

  std::deque<Command> commands = {Command("pos1", 42, CommandType::PositionalValue, true), Command("v", 43, CommandType::Switch)};

  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::RequiredArgumentNotFound);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[1].Type);
  EXPECT_EQ(res[0].Id, commands[1].Id);
  EXPECT_EQ(res[0].Count, 1u);
  EXPECT_TRUE(res[0].StrOptArg.empty());
}


TEST(Test_ArgumentParser, RequiredPositionalValue_2x_hello_NotSpecified)
{
  std::array<StringViewLite, 1> testArgs = {"hello"};

  std::deque<Command> commands = {Command("pos1", 42, CommandType::PositionalValue, true), Command("pos2", 1, CommandType::PositionalValue, true)};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::RequiredArgumentNotFound);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  EXPECT_EQ(res[0].StrOptArg, testArgs[0]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Unhandled arguments
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_ArgumentParser, UnhandledArgument_NotAllowed1)
{
  std::array<StringViewLite, 2> testArgs = {"--", "hello"};

  std::deque<Command> commands = {};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::UnknownArgumentError);
  ASSERT_EQ(res.size(), 0u);
}

TEST(Test_ArgumentParser, UnhandledArgument_NotAllowed2)
{
  std::array<StringViewLite, 3> testArgs = {"--", "hello", "world"};

  std::deque<Command> commands = {};
  std::deque<EncodedCommand> res;
  ASSERT_EQ(TryParseNow(res, testArgs, commands), ParseResult::UnknownArgumentError);
  ASSERT_EQ(res.size(), 0u);
}


TEST(Test_ArgumentParser, UnhandledArgument_Hello)
{
  std::array<StringViewLite, 2> testArgs = {"--", "hello"};

  std::deque<Command> commands = {Command("Unhandled", 42, CommandType::Unhandled)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 1u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  EXPECT_EQ(res[0].StrOptArg, testArgs[1]);
}


TEST(Test_ArgumentParser, UnhandledArgument_Hello_World)
{
  std::array<StringViewLite, 3> testArgs = {"--", "hello", "world"};

  std::deque<Command> commands = {Command("Unhandled", 42, CommandType::Unhandled)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 2u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  EXPECT_EQ(res[0].StrOptArg, testArgs[1]);

  EXPECT_EQ(res[1].Type, commands[0].Type);
  EXPECT_EQ(res[1].Id, commands[0].Id);
  EXPECT_EQ(res[1].Count, 1u);
  EXPECT_EQ(res[1].StrOptArg, testArgs[2]);
}


TEST(Test_ArgumentParser, UnhandledArgument_Hello_World_Switch)
{
  std::array<StringViewLite, 4> testArgs = {"--", "hello", "world", "-a"};

  std::deque<Command> commands = {Command("Unhandled", 42, CommandType::Unhandled)};
  auto res = ParseNow(testArgs, commands);

  ASSERT_EQ(res.size(), 3u);
  EXPECT_EQ(res[0].Type, commands[0].Type);
  EXPECT_EQ(res[0].Id, commands[0].Id);
  EXPECT_EQ(res[0].Count, 1u);
  EXPECT_EQ(res[0].StrOptArg, testArgs[1]);

  EXPECT_EQ(res[1].Type, commands[0].Type);
  EXPECT_EQ(res[1].Id, commands[0].Id);
  EXPECT_EQ(res[1].Count, 1u);
  EXPECT_EQ(res[1].StrOptArg, testArgs[2]);

  EXPECT_EQ(commands[0].Type, res[2].Type);
  EXPECT_EQ(commands[0].Id, res[2].Id);
  EXPECT_EQ(res[2].Count, 1u);
  EXPECT_EQ(res[2].StrOptArg, testArgs[3]);
}
