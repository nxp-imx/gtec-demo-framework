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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Getopt/OptionParser.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestGetOpt_OptionParser = TestFixtureFslBase;

  class DummyOptionParser : public IOptionParser
  {
  public:
    std::string GetName() const override
    {
      return {"dummy"};
    }

    void ArgumentSetup(std::deque<Option>& /*rOptions*/) override
    {
    }

    OptionParseResult Parse(const int32_t /*cmdId*/, const StringViewLite& /*strOptArg*/) override
    {
      return OptionParseResult::NotHandled;
    }

    //! @brief Parse a input option.
    //! @return true to continue, false to exit
    bool ParsingComplete() override
    {
      return true;
    }
  };
}

TEST(TestGetOpt_OptionParser, Parse0_LegacyArgs)
{
  std::array<char, 8> pszLegacySkipped = {'s', 'k', 'i', 'p', 'p', 'e', 'd', 0};
  std::array<char, 3> pszLegacyOption = {'-', 'v', 0};
  std::array<char*, 2> testArgs = {pszLegacySkipped.data(), pszLegacyOption.data()};

  auto result = OptionParser::Parse(NumericCast<int>(testArgs.size()), testArgs.data(), "help caption");

  EXPECT_EQ(OptionParser::Result::OK, result.Status);
  EXPECT_EQ(1u, result.VerbosityLevel);
}


TEST(TestGetOpt_OptionParser, Parse1_LegacyArgs)
{
  std::array<char, 8> pszLegacySkipped = {'s', 'k', 'i', 'p', 'p', 'e', 'd', 0};
  std::array<char, 3> pszLegacyOption = {'-', 'v', 0};
  std::array<char*, 2> testArgs = {pszLegacySkipped.data(), pszLegacyOption.data()};

  DummyOptionParser dummyOptionParser;
  auto result = OptionParser::Parse(NumericCast<int>(testArgs.size()), testArgs.data(), dummyOptionParser, "help caption");

  EXPECT_EQ(OptionParser::Result::OK, result.Status);
  EXPECT_EQ(1u, result.VerbosityLevel);
}


TEST(TestGetOpt_OptionParser, Parse2_LegacyArgs)
{
  std::array<char, 8> pszLegacySkipped = {'s', 'k', 'i', 'p', 'p', 'e', 'd', 0};
  std::array<char, 3> pszLegacyOption = {'-', 'v', 0};
  std::array<char*, 2> testArgs = {pszLegacySkipped.data(), pszLegacyOption.data()};

  DummyOptionParser dummyOptionParser;
  std::deque<IOptionParser*> optionParsers;
  optionParsers.push_back(&dummyOptionParser);

  auto result = OptionParser::Parse(NumericCast<int>(testArgs.size()), testArgs.data(), optionParsers, "help caption");

  EXPECT_EQ(OptionParser::Result::OK, result.Status);
  EXPECT_EQ(1u, result.VerbosityLevel);
}


TEST(TestGetOpt_OptionParser, Parse3_LegacyArgs)
{
  std::array<char, 8> pszLegacySkipped = {'s', 'k', 'i', 'p', 'p', 'e', 'd', 0};
  std::array<char, 3> pszLegacyOption = {'-', 'v', 0};
  std::array<char*, 2> testArgs = {pszLegacySkipped.data(), pszLegacyOption.data()};

  DummyOptionParser dummyOptionParser;
  std::deque<OptionParser::ParserRecord> optionParsers;
  optionParsers.emplace_back(&dummyOptionParser, 0x1000);

  auto result = OptionParser::Parse(NumericCast<int>(testArgs.size()), testArgs.data(), optionParsers, "help caption");

  EXPECT_EQ(OptionParser::Result::OK, result.Status);
  EXPECT_EQ(1u, result.VerbosityLevel);
}


TEST(TestGetOpt_OptionParser, Parse0)
{
  std::array<StringViewLite, 1> testArgs = {"-v"};

  auto result = OptionParser::Parse(ReadOnlySpanUtil::AsSpan(testArgs), "help caption");

  EXPECT_EQ(OptionParser::Result::OK, result.Status);
  EXPECT_EQ(1u, result.VerbosityLevel);
}


TEST(TestGetOpt_OptionParser, Parse1)
{
  std::array<StringViewLite, 1> testArgs = {"-v"};

  DummyOptionParser dummyOptionParser;
  auto result = OptionParser::Parse(ReadOnlySpanUtil::AsSpan(testArgs), dummyOptionParser, "help caption");

  EXPECT_EQ(OptionParser::Result::OK, result.Status);
  EXPECT_EQ(1u, result.VerbosityLevel);
}


TEST(TestGetOpt_OptionParser, Parse2)
{
  std::array<StringViewLite, 1> testArgs = {"-v"};

  DummyOptionParser dummyOptionParser;
  std::deque<IOptionParser*> optionParsers;
  optionParsers.push_back(&dummyOptionParser);

  auto result = OptionParser::Parse(ReadOnlySpanUtil::AsSpan(testArgs), optionParsers, "help caption");

  EXPECT_EQ(OptionParser::Result::OK, result.Status);
  EXPECT_EQ(1u, result.VerbosityLevel);
}


TEST(TestGetOpt_OptionParser, Parse3)
{
  std::array<StringViewLite, 1> testArgs = {"-v"};

  DummyOptionParser dummyOptionParser;
  std::deque<OptionParser::ParserRecord> optionParsers;
  optionParsers.emplace_back(&dummyOptionParser, 0x1000);

  auto result = OptionParser::Parse(ReadOnlySpanUtil::AsSpan(testArgs), optionParsers, "help caption");

  EXPECT_EQ(OptionParser::Result::OK, result.Status);
  EXPECT_EQ(1u, result.VerbosityLevel);
}


TEST(TestGetOpt_OptionParser, Parse_Help)
{
  std::array<StringViewLite, 1> testArgs = {"-h"};

  auto result = OptionParser::Parse(ReadOnlySpanUtil::AsSpan(testArgs), "help caption");

  // We expect help to cause a exit
  EXPECT_EQ(OptionParser::Result::Exit, result.Status);
  EXPECT_EQ(0u, result.VerbosityLevel);
}
