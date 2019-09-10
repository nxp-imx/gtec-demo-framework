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
#include <FslBase/Arguments/ArgumentParserErrorStrings.hpp>
#include <FslBase/Arguments/ParseErrorInfo.hpp>
#include <fmt/format.h>
#include <array>
#include <cassert>
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
  using Test_ArgumentParser_ErrorStrings = TestFixture;

  class FmtErrorFormatter : public ArgumentParser::ErrorFormatter
  {
  public:
    std::string Format(const char* const pszFormat, const char* const pszArg0) override
    {
      assert(pszFormat != nullptr);
      return fmt::format(pszFormat, pszArg0);
    }
    std::string Format(const char* const pszFormat, const std::string& str) override
    {
      assert(pszFormat != nullptr);
      return fmt::format(pszFormat, str);
    }
    std::string Format(const char* const pszFormat, const uint32_t arg0) override
    {
      assert(pszFormat != nullptr);
      return fmt::format(pszFormat, arg0);
    }
  };

  const auto STR_INVALID_ARGUMENTS = "Method called with invalid arguments";

  template <std::size_t TSize>
  std::string GetErrorString(const ParseResult result, const ParseErrorInfo& parseErrorInfo, const std::array<const char*, TSize>& testArgs,
                             const std::deque<Command>& commands, ArgumentParser::ErrorFormatter& formatter)
  {
    return ArgumentParser::GetErrorString(result, parseErrorInfo, static_cast<uint32_t>(testArgs.size()), testArgs.data(), commands, formatter);
  }
}


TEST(Test_ArgumentParser_ErrorStrings, ArgsNullptr)
{
  std::deque<Command> commands = {Command("a", 42, CommandType::Switch)};

  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;

  auto errorCode = ArgumentParser::TryParse(res, 0, nullptr, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::InvalidArguments);

  FmtErrorFormatter formatter;
  EXPECT_EQ(ArgumentParser::GetErrorString(errorCode, errorInfo, 0, nullptr, commands, formatter), STR_INVALID_ARGUMENTS);
}


TEST(Test_ArgumentParser_ErrorStrings, ArgsNegative)
{
  std::array<const char*, 1> testArgs = {"-a"};
  std::deque<Command> commands = {Command("a", 42, CommandType::Switch)};

  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;

  auto errorCode = ArgumentParser::TryParse(res, -1, testArgs.data(), commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::InvalidArguments);

  FmtErrorFormatter formatter;
  EXPECT_EQ(ArgumentParser::GetErrorString(errorCode, errorInfo, -1, testArgs.data(), commands, formatter), STR_INVALID_ARGUMENTS);
}


TEST(Test_ArgumentParser_ErrorStrings, Switch_a_InvalidFormat)
{
  std::array<const char*, 1> testArgs = {"---a"};

  ParseErrorInfo errorInfo;
  std::deque<Command> commands = {Command("a", 42, CommandType::Switch)};
  std::deque<EncodedCommand> res;

  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::ArgumentFormatError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Argument '---a' has a invalid format");
}

TEST(Test_ArgumentParser_ErrorStrings, Switch_a_EmptyArgument)
{
  std::array<const char*, 2> testArgs = {"-", "a"};

  ParseErrorInfo errorInfo;
  std::deque<Command> commands = {Command("a", 42, CommandType::Switch)};
  std::deque<EncodedCommand> res;

  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::ArgumentFormatError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Argument '-' has a invalid format");
}


TEST(Test_ArgumentParser_ErrorStrings, Switch_a_UnknownArg)
{
  std::array<const char*, 1> testArgs = {"-b"};

  ParseErrorInfo errorInfo;
  std::deque<Command> commands = {Command("a", 42, CommandType::Switch)};
  std::deque<EncodedCommand> res;

  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::UnknownArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Unknown argument '-b'");
}


TEST(Test_ArgumentParser_ErrorStrings, Switch_a_UnknownArgument)
{
  std::array<const char*, 2> testArgs = {"-a", "test"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Switch)};

  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::UnknownArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Unknown argument 'test'");
}


TEST(Test_ArgumentParser_ErrorStrings, Switch_a_nullptr)
{
  std::array<const char*, 2> testArgs = {"-a", nullptr};

  std::deque<Command> commands = {Command("a", 42, CommandType::Switch)};

  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::ArgumentListContainedNullError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "nullptr found at argument index '1'");
}


TEST(Test_ArgumentParser_ErrorStrings, Switch2X_aa)
{
  std::array<const char*, 1> testArgs = {"-aa"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Switch)};

  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  EXPECT_EQ(errorCode, ParseResult::DuplicatedSwitchArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Duplicated switch '-a'");
}


TEST(Test_ArgumentParser_ErrorStrings, Switch2X_a_a)
{
  std::array<const char*, 2> testArgs = {"-a", "-a"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Switch)};

  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  EXPECT_EQ(errorCode, ParseResult::DuplicatedSwitchArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Duplicated switch '-a'");
}


TEST(Test_ArgumentParser_ErrorStrings, Switch2X_aba)
{
  std::array<const char*, 1> testArgs = {"-aba"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Switch), Command("b", 1, CommandType::Switch)};

  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  EXPECT_EQ(errorCode, ParseResult::DuplicatedSwitchArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Duplicated switch '-a'");
}


TEST(Test_ArgumentParser_ErrorStrings, Switch2X_a_ba)
{
  std::array<const char*, 2> testArgs = {"-a", "-ba"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Switch), Command("b", 1, CommandType::Switch)};

  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  EXPECT_EQ(errorCode, ParseResult::DuplicatedSwitchArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Duplicated switch '-a'");
}


TEST(Test_ArgumentParser_ErrorStrings, Switch2X_ab_a)
{
  std::array<const char*, 2> testArgs = {"-ab", "-b"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Switch), Command("b", 1, CommandType::Switch)};

  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  EXPECT_EQ(errorCode, ParseResult::DuplicatedSwitchArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Duplicated switch '-b'");
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Value
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_ArgumentParser_ErrorStrings, Value_a_missing)
{
  std::array<const char*, 1> testArgs = {"-a"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Value)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::ArgumentMissingValueError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Missing value for argument '-a'");
}


TEST(Test_ArgumentParser_ErrorStrings, Value_AValueBValue_ValueNotLast)
{
  std::array<const char*, 2> testArgs = {"-ab", "hey"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Value), Command("b", 1, CommandType::Value)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::CombinedValueArgumentMustBeLastError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Combined short value argument '-a' must be last in sequence");
}


TEST(Test_ArgumentParser_ErrorStrings, Value_ab_ValueNotLast)
{
  std::array<const char*, 2> testArgs = {"-ab", "hey"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Value), Command("b", 1, CommandType::Switch)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::CombinedValueArgumentMustBeLastError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Combined short value argument '-a' must be last in sequence");
}


TEST(Test_ArgumentParser_ErrorStrings, Value_ab_ValueNotLastAndValueArgMissing)
{
  std::array<const char*, 1> testArgs = {"-ab"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Value), Command("b", 1, CommandType::Switch)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::CombinedValueArgumentMustBeLastError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Combined short value argument '-a' must be last in sequence");
}


TEST(Test_ArgumentParser_ErrorStrings, Value_ba_ValueArgMissing)
{
  std::array<const char*, 1> testArgs = {"-ba"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Value), Command("b", 1, CommandType::Switch)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::ArgumentMissingValueError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Missing value for argument '-a'");
}


TEST(Test_ArgumentParser_ErrorStrings, Value_a_duplicated)
{
  std::array<const char*, 4> testArgs = {"-a", "1", "-a", "2"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Value)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::DuplicatedValueArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Duplicated value '-a'");
}


TEST(Test_ArgumentParser_ErrorStrings, Value_a_duplicated2)
{
  std::array<const char*, 4> testArgs = {"-a", "1", "-ba", "2"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Value), Command("b", 1, CommandType::Switch)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::DuplicatedValueArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Duplicated value '-a'");
}


TEST(Test_ArgumentParser_ErrorStrings, Value_test_duplicated)
{
  std::array<const char*, 4> testArgs = {"--test", "1", "--test", "2"};

  std::deque<Command> commands = {Command("test", 42, CommandType::Value)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::DuplicatedValueArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Duplicated value '--test'");
}


TEST(Test_ArgumentParser_ErrorStrings, Value_a_test_duplicated)
{
  std::array<const char*, 4> testArgs = {"-a", "1", "--test", "2"};

  std::deque<Command> commands = {Command("a", "test", 42, CommandType::Value)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::DuplicatedValueArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Duplicated value '--test'");
}


TEST(Test_ArgumentParser_ErrorStrings, Value_test_a_duplicated)
{
  std::array<const char*, 4> testArgs = {"--test", "1", "-a", "2"};

  std::deque<Command> commands = {Command("a", "test", 42, CommandType::Value)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::DuplicatedValueArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Duplicated value '-a'");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Required value arguments
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_ArgumentParser_ErrorStrings, RequiredValue_NotSpecified)
{
  std::array<const char*, 1> testArgs = {"-v"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Value, true), Command("v", 43, CommandType::Switch)};

  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::RequiredArgumentNotFound);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Required argument '-a' is missing");
}


TEST(Test_ArgumentParser_ErrorStrings, RequiredLongValue_NotSpecified)
{
  std::array<const char*, 1> testArgs = {"-v"};

  std::deque<Command> commands = {Command("test", 42, CommandType::Value, true), Command("v", 43, CommandType::Switch)};

  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::RequiredArgumentNotFound);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Required argument '--test' is missing");
}


TEST(Test_ArgumentParser_ErrorStrings, RequiredValue_2x_a_NotSpecified)
{
  std::array<const char*, 2> testArgs = {"-a", "1"};

  std::deque<Command> commands = {Command("a", 42, CommandType::Value, true), Command("b", 1, CommandType::Value, true)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::RequiredArgumentNotFound);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Required argument '-b' is missing");
}

TEST(Test_ArgumentParser_ErrorStrings, RequiredLongValue_2x_test_NotSpecified)
{
  std::array<const char*, 2> testArgs = {"--test", "1"};

  std::deque<Command> commands = {Command("test", 42, CommandType::Value, true), Command("best", 1, CommandType::Value, true)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::RequiredArgumentNotFound);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Required argument '--best' is missing");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Long switch
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_ArgumentParser_ErrorStrings, LongSwitch_test_AsShortSwitch)
{
  std::array<const char*, 1> testArgs = {"-test"};

  std::deque<Command> commands = {Command("test", 42, CommandType::Switch)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::UnknownArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Unknown argument '-test'");
}


TEST(Test_ArgumentParser_ErrorStrings, LongSwitch_test_Unknown)
{
  std::array<const char*, 1> testArgs = {"--test2"};

  std::deque<Command> commands = {Command("test", 42, CommandType::Switch)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::UnknownArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Unknown argument '--test2'");
}


TEST(Test_ArgumentParser_ErrorStrings, LongSwitch_test_test)
{
  std::array<const char*, 2> testArgs = {"--test", "--test"};

  std::deque<Command> commands = {Command("test", 42, CommandType::Switch)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::DuplicatedSwitchArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Duplicated switch '--test'");
}


TEST(Test_ArgumentParser_ErrorStrings, LongSwitch_test_test_invalid)
{
  std::array<const char*, 1> testArgs = {"--test --test"};

  std::deque<Command> commands = {Command("test", 42, CommandType::Switch)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::UnknownArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Unknown argument '--test --test'");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Required MultiSwitch arguments
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_ArgumentParser_ErrorStrings, RequiredMultiSwitch_NotSpecified)
{
  std::array<const char*, 1> testArgs = {"-v"};

  std::deque<Command> commands = {Command("a", 42, CommandType::MultiSwitch, true), Command("v", 43, CommandType::Switch)};

  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::RequiredArgumentNotFound);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Required argument '-a' is missing");
}


TEST(Test_ArgumentParser_ErrorStrings, RequiredMultiSwitch_2x_a_NotSpecified)
{
  std::array<const char*, 1> testArgs = {"-a"};

  std::deque<Command> commands = {Command("a", 42, CommandType::MultiSwitch, true), Command("b", 1, CommandType::MultiSwitch, true)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::RequiredArgumentNotFound);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Required argument '-b' is missing");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Long multi switch
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_ArgumentParser_ErrorStrings, LongMultiSwitch_test_AsShortSwitch)
{
  std::array<const char*, 1> testArgs = {"-test"};

  std::deque<Command> commands = {Command("test", 42, CommandType::MultiSwitch)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::UnknownArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Unknown argument '-test'");
}


TEST(Test_ArgumentParser_ErrorStrings, LongMultiSwitch_test_Unknown)
{
  std::array<const char*, 1> testArgs = {"--test2"};

  std::deque<Command> commands = {Command("test", 42, CommandType::MultiSwitch)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::UnknownArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Unknown argument '--test2'");
}


TEST(Test_ArgumentParser_ErrorStrings, LongMultiSwitch_test_test_invalid)
{
  std::array<const char*, 1> testArgs = {"--test --test"};

  std::deque<Command> commands = {Command("test", 42, CommandType::MultiSwitch)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::UnknownArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Unknown argument '--test --test'");
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Required Long MultiSwitch arguments
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_ArgumentParser_ErrorStrings, RequiredLongMultiSwitch_NotSpecified)
{
  std::array<const char*, 1> testArgs = {"-v"};

  std::deque<Command> commands = {Command("test", 42, CommandType::MultiSwitch, true), Command("v", 43, CommandType::Switch)};

  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::RequiredArgumentNotFound);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Required argument '--test' is missing");
}


TEST(Test_ArgumentParser_ErrorStrings, RequiredLongMultiSwitch_2x_test_NotSpecified)
{
  std::array<const char*, 1> testArgs = {"--test"};

  std::deque<Command> commands = {Command("test", 42, CommandType::MultiSwitch, true), Command("best", 1, CommandType::MultiSwitch, true)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::RequiredArgumentNotFound);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Required argument '--best' is missing");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Positional arguments
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_ArgumentParser_ErrorStrings, Positional_NotAllowed)
{
  std::array<const char*, 1> testArgs = {"hello"};

  std::deque<Command> commands = {};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::UnknownArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Unknown argument 'hello'");
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Required PositionalValue arguments
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_ArgumentParser_ErrorStrings, RequiredPositionalValue_NotSpecified)
{
  std::array<const char*, 1> testArgs = {"-v"};

  std::deque<Command> commands = {Command("pos1", 42, CommandType::PositionalValue, true), Command("v", 43, CommandType::Switch)};

  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::RequiredArgumentNotFound);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Required positional argument 'pos1' is missing");
}


TEST(Test_ArgumentParser_ErrorStrings, RequiredPositionalValue_2x_hello_NotSpecified)
{
  std::array<const char*, 1> testArgs = {"hello"};

  std::deque<Command> commands = {Command("pos1", 42, CommandType::PositionalValue, true), Command("pos2", 1, CommandType::PositionalValue, true)};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::RequiredArgumentNotFound);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Required positional argument 'pos2' is missing");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Unhandled arguments
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_ArgumentParser_ErrorStrings, UnhandledArgument_NotAllowed1)
{
  std::array<const char*, 2> testArgs = {"--", "hello"};

  std::deque<Command> commands = {};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::UnknownArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Unknown argument 'hello'");
}

TEST(Test_ArgumentParser_ErrorStrings, UnhandledArgument_NotAllowed2)
{
  std::array<const char*, 3> testArgs = {"--", "hello", "world"};

  std::deque<Command> commands = {};
  ParseErrorInfo errorInfo;
  std::deque<EncodedCommand> res;
  auto errorCode = TryParseNow(res, testArgs, commands, &errorInfo);
  ASSERT_EQ(errorCode, ParseResult::UnknownArgumentError);

  FmtErrorFormatter formatter;
  EXPECT_EQ(GetErrorString(errorCode, errorInfo, testArgs, commands, formatter), "Unknown argument 'hello'");
}
