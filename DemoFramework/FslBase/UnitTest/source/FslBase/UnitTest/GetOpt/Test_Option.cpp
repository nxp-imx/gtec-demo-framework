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
#include <FslBase/Getopt/Option.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestGetOpt_Option = TestFixtureFslBase;
}


TEST(TestGetOpt_Option, Construct_Default)
{
  int32_t cmdId = 0;
  Option option;

  EXPECT_EQ(std::string(""), option.ShortName);
  EXPECT_EQ(std::string(""), option.Name);
  EXPECT_EQ(OptionArgument::OptionNone, option.HasArg);
  EXPECT_EQ(cmdId, option.CmdId);
  EXPECT_EQ(std::string(""), option.Description);
  EXPECT_EQ(OptionGroup::Default, option.Group);
  EXPECT_EQ(OptionType::Default, option.Type);
  EXPECT_FALSE(option.IsPositional);
}


TEST(TestGetOpt_Option, Construct_Smartname1)
{
  std::string smartName("h");
  auto hasArg = OptionArgument::OptionRequired;
  int32_t cmdId = 42;
  std::string description("world");

  Option option(smartName, hasArg, cmdId, description);

  EXPECT_EQ(smartName, option.ShortName);
  EXPECT_EQ(std::string(""), option.Name);
  EXPECT_EQ(hasArg, option.HasArg);
  EXPECT_EQ(cmdId, option.CmdId);
  EXPECT_EQ(description, option.Description);
  EXPECT_EQ(OptionGroup::Default, option.Group);
  EXPECT_EQ(OptionType::Default, option.Type);
  EXPECT_FALSE(option.IsPositional);
}


TEST(TestGetOpt_Option, Construct_Smartname2)
{
  std::string smartName("hello");
  auto hasArg = OptionArgument::OptionRequired;
  int32_t cmdId = 42;
  std::string description("world");

  Option option(smartName, hasArg, cmdId, description);

  EXPECT_EQ(std::string(""), option.ShortName);
  EXPECT_EQ(smartName, option.Name);
  EXPECT_EQ(hasArg, option.HasArg);
  EXPECT_EQ(cmdId, option.CmdId);
  EXPECT_EQ(description, option.Description);
  EXPECT_EQ(OptionGroup::Default, option.Group);
  EXPECT_EQ(OptionType::Default, option.Type);
  EXPECT_FALSE(option.IsPositional);
}


TEST(TestGetOpt_Option, Construct_Smartname1_Group)
{
  std::string smartName("h");
  auto hasArg = OptionArgument::OptionRequired;
  int32_t cmdId = 42;
  std::string description("world");
  OptionGroup::Enum group = OptionGroup::Custom1;

  Option option(smartName, hasArg, cmdId, description, group);

  EXPECT_EQ(smartName, option.ShortName);
  EXPECT_EQ(std::string(""), option.Name);
  EXPECT_EQ(hasArg, option.HasArg);
  EXPECT_EQ(cmdId, option.CmdId);
  EXPECT_EQ(description, option.Description);
  EXPECT_EQ(group, option.Group);
  EXPECT_EQ(OptionType::Default, option.Type);
  EXPECT_FALSE(option.IsPositional);
}


TEST(TestGetOpt_Option, Construct_Smartname2_Group)
{
  std::string smartName("hello");
  auto hasArg = OptionArgument::OptionRequired;
  int32_t cmdId = 42;
  std::string description("world");
  OptionGroup::Enum group = OptionGroup::Custom1;

  Option option(smartName, hasArg, cmdId, description, group);

  EXPECT_EQ(std::string(""), option.ShortName);
  EXPECT_EQ(smartName, option.Name);
  EXPECT_EQ(hasArg, option.HasArg);
  EXPECT_EQ(cmdId, option.CmdId);
  EXPECT_EQ(description, option.Description);
  EXPECT_EQ(group, option.Group);
  EXPECT_EQ(OptionType::Default, option.Type);
  EXPECT_FALSE(option.IsPositional);
}


TEST(TestGetOpt_Option, Construct_TwoNames)
{
  std::string shortName("h");
  std::string name("hello");
  auto hasArg = OptionArgument::OptionRequired;
  int32_t cmdId = 42;
  std::string description("world");

  Option option(shortName, name, hasArg, cmdId, description);

  EXPECT_EQ(shortName, option.ShortName);
  EXPECT_EQ(name, option.Name);
  EXPECT_EQ(hasArg, option.HasArg);
  EXPECT_EQ(cmdId, option.CmdId);
  EXPECT_EQ(description, option.Description);
  EXPECT_EQ(OptionGroup::Default, option.Group);
  EXPECT_EQ(OptionType::Default, option.Type);
  EXPECT_FALSE(option.IsPositional);
}


TEST(TestGetOpt_Option, Construct_TwoNamesAndGroup)
{
  std::string shortName("h");
  std::string name("hello");
  auto hasArg = OptionArgument::OptionRequired;
  int32_t cmdId = 42;
  std::string description("world");
  OptionGroup::Enum group = OptionGroup::Custom1;

  Option option(shortName, name, hasArg, cmdId, description, group);

  EXPECT_EQ(shortName, option.ShortName);
  EXPECT_EQ(name, option.Name);
  EXPECT_EQ(hasArg, option.HasArg);
  EXPECT_EQ(cmdId, option.CmdId);
  EXPECT_EQ(description, option.Description);
  EXPECT_EQ(group, option.Group);
  EXPECT_EQ(OptionType::Default, option.Type);
  EXPECT_FALSE(option.IsPositional);
}


TEST(TestGetOpt_Option, Construct_PositionalOption)
{
  std::string name("hello");
  auto hasArg = OptionArgument::OptionRequired;
  int32_t cmdId = 42;
  std::string description("world");

  PositionalOption option(name, hasArg, cmdId, description);

  EXPECT_EQ(std::string(""), option.ShortName);
  EXPECT_EQ(name, option.Name);
  EXPECT_EQ(hasArg, option.HasArg);
  EXPECT_EQ(cmdId, option.CmdId);
  EXPECT_EQ(description, option.Description);
  EXPECT_EQ(OptionGroup::Default, option.Group);
  EXPECT_EQ(OptionType::Default, option.Type);
  EXPECT_TRUE(option.IsPositional);
}


TEST(TestGetOpt_Option, Construct_PositionalOption_Group)
{
  std::string name("hello");
  auto hasArg = OptionArgument::OptionRequired;
  int32_t cmdId = 42;
  std::string description("world");
  OptionGroup::Enum group = OptionGroup::Custom1;

  PositionalOption option(name, hasArg, cmdId, description, group);

  EXPECT_EQ(std::string(""), option.ShortName);
  EXPECT_EQ(name, option.Name);
  EXPECT_EQ(hasArg, option.HasArg);
  EXPECT_EQ(cmdId, option.CmdId);
  EXPECT_EQ(description, option.Description);
  EXPECT_EQ(group, option.Group);
  EXPECT_EQ(OptionType::Default, option.Type);
  EXPECT_TRUE(option.IsPositional);
}
