/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <FslBase/String/StringUtil.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestString_StringUtil = TestFixtureFslBase;
}


TEST(TestString_StringUtil, Contains_Char_Empty)
{
  std::string src;

  EXPECT_FALSE(StringUtil::Contains(src, ' '));
  EXPECT_FALSE(StringUtil::Contains(src, 'a'));
}


TEST(TestString_StringUtil, Contains_Char)
{
  std::string src("hello world");

  EXPECT_TRUE(StringUtil::Contains(src, 'h'));
  EXPECT_TRUE(StringUtil::Contains(src, ' '));
  EXPECT_TRUE(StringUtil::Contains(src, 'd'));

  EXPECT_FALSE(StringUtil::Contains(src, 'a'));
  EXPECT_FALSE(StringUtil::Contains(src, 'H'));
}


TEST(TestString_StringUtil, Contains_String_Empty)
{
  std::string src;

  EXPECT_TRUE(StringUtil::Contains(src, ""));

  EXPECT_FALSE(StringUtil::Contains(src, "a"));
  EXPECT_FALSE(StringUtil::Contains(src, "abcd"));
}


TEST(TestString_StringUtil, Contains_StringChar)
{
  std::string src("hello world");

  EXPECT_TRUE(StringUtil::Contains(src, ""));
  EXPECT_TRUE(StringUtil::Contains(src, "h"));
  EXPECT_TRUE(StringUtil::Contains(src, "hello"));
  EXPECT_TRUE(StringUtil::Contains(src, "world"));
  EXPECT_TRUE(StringUtil::Contains(src, "hello world"));

  EXPECT_FALSE(StringUtil::Contains(src, "hello world."));
  EXPECT_FALSE(StringUtil::Contains(src, "Hello world."));
  EXPECT_FALSE(StringUtil::Contains(src, "Helo"));
}


TEST(TestString_StringUtil, StartsWith_Char_Empty)
{
  std::string src;

  EXPECT_FALSE(StringUtil::StartsWith(src, ' '));
  EXPECT_FALSE(StringUtil::StartsWith(src, 'a'));
}


TEST(TestString_StringUtil, StartsWith_Char)
{
  std::string src("hello world");

  EXPECT_TRUE(StringUtil::StartsWith(src, 'h'));

  EXPECT_FALSE(StringUtil::StartsWith(src, 'H'));
  EXPECT_FALSE(StringUtil::StartsWith(src, 'e'));
  EXPECT_FALSE(StringUtil::StartsWith(src, 'd'));
}


TEST(TestString_StringUtil, StartsWith_String_Empty)
{
  std::string src;

  EXPECT_TRUE(StringUtil::StartsWith(src, ""));
  EXPECT_FALSE(StringUtil::StartsWith(src, " "));
  EXPECT_FALSE(StringUtil::StartsWith(src, "a"));
}


TEST(TestString_StringUtil, StartsWith_String)
{
  std::string src("hello world");

  EXPECT_TRUE(StringUtil::StartsWith(src, ""));
  EXPECT_TRUE(StringUtil::StartsWith(src, "h"));
  EXPECT_TRUE(StringUtil::StartsWith(src, "he"));
  EXPECT_TRUE(StringUtil::StartsWith(src, "hel"));
  EXPECT_TRUE(StringUtil::StartsWith(src, "hell"));
  EXPECT_TRUE(StringUtil::StartsWith(src, "hello"));
  EXPECT_TRUE(StringUtil::StartsWith(src, "hello "));
  EXPECT_TRUE(StringUtil::StartsWith(src, "hello w"));
  EXPECT_TRUE(StringUtil::StartsWith(src, "hello wo"));
  EXPECT_TRUE(StringUtil::StartsWith(src, "hello wor"));
  EXPECT_TRUE(StringUtil::StartsWith(src, "hello worl"));
  EXPECT_TRUE(StringUtil::StartsWith(src, "hello world"));

  EXPECT_FALSE(StringUtil::StartsWith(src, "hello world!"));
  EXPECT_FALSE(StringUtil::StartsWith(src, "e"));
}


TEST(TestString_StringUtil, EndsWith_Char_Empty)
{
  std::string src;

  EXPECT_FALSE(StringUtil::EndsWith(src, ' '));
  EXPECT_FALSE(StringUtil::EndsWith(src, 'a'));
}


TEST(TestString_StringUtil, EndsWith_Char)
{
  std::string src("hello world");

  EXPECT_TRUE(StringUtil::EndsWith(src, 'd'));

  EXPECT_FALSE(StringUtil::EndsWith(src, 'D'));
  EXPECT_FALSE(StringUtil::EndsWith(src, 'l'));
  EXPECT_FALSE(StringUtil::EndsWith(src, 'h'));
}


TEST(TestString_StringUtil, EndsWith_String_Empty)
{
  std::string src;

  EXPECT_TRUE(StringUtil::EndsWith(src, ""));
  EXPECT_FALSE(StringUtil::EndsWith(src, " "));
  EXPECT_FALSE(StringUtil::EndsWith(src, "a"));
}


TEST(TestString_StringUtil, EndsWith_String)
{
  std::string src("hello world");

  EXPECT_TRUE(StringUtil::EndsWith(src, ""));
  EXPECT_TRUE(StringUtil::EndsWith(src, "d"));
  EXPECT_TRUE(StringUtil::EndsWith(src, "ld"));
  EXPECT_TRUE(StringUtil::EndsWith(src, "rld"));
  EXPECT_TRUE(StringUtil::EndsWith(src, "orld"));
  EXPECT_TRUE(StringUtil::EndsWith(src, "world"));
  EXPECT_TRUE(StringUtil::EndsWith(src, " world"));
  EXPECT_TRUE(StringUtil::EndsWith(src, "o world"));
  EXPECT_TRUE(StringUtil::EndsWith(src, "lo world"));
  EXPECT_TRUE(StringUtil::EndsWith(src, "llo world"));
  EXPECT_TRUE(StringUtil::EndsWith(src, "ello world"));
  EXPECT_TRUE(StringUtil::EndsWith(src, "hello world"));

  EXPECT_FALSE(StringUtil::EndsWith(src, "!hello world"));
  EXPECT_FALSE(StringUtil::EndsWith(src, "l"));
}


TEST(TestString_StringUtil, IndexOfChar)
{
  //---------------0123456789
  std::string src("old school");

  EXPECT_EQ(0, StringUtil::IndexOf(src, 'o'));
  EXPECT_EQ(1, StringUtil::IndexOf(src, 'l'));
  EXPECT_EQ(2, StringUtil::IndexOf(src, 'd'));
  EXPECT_EQ(3, StringUtil::IndexOf(src, ' '));
  EXPECT_EQ(4, StringUtil::IndexOf(src, 's'));
  EXPECT_EQ(5, StringUtil::IndexOf(src, 'c'));
  EXPECT_EQ(6, StringUtil::IndexOf(src, 'h'));

  EXPECT_EQ(7, StringUtil::IndexOf(src, 'o', 1));
  EXPECT_EQ(7, StringUtil::IndexOf(src, 'o', 7));
  EXPECT_EQ(8, StringUtil::IndexOf(src, 'o', 8));
  EXPECT_EQ(9, StringUtil::IndexOf(src, 'l', 2));

  EXPECT_GE(0, StringUtil::IndexOf(src, 'O'));
  EXPECT_GE(0, StringUtil::IndexOf(src, 'a'));
  EXPECT_GE(0, StringUtil::IndexOf(src, 'D'));

  EXPECT_GE(0, StringUtil::IndexOf(src, 'o', 9));
  EXPECT_GE(0, StringUtil::IndexOf(src, 'o', 10));
  EXPECT_GE(0, StringUtil::IndexOf(src, 'o', 11));
  EXPECT_GE(0, StringUtil::IndexOf(src, 'o', 12));
}


TEST(TestString_StringUtil, LastIndexOfChar)
{
  //--------------0123456789
  std::string src("old school");

  EXPECT_EQ(9, StringUtil::LastIndexOf(src, 'l'));
  EXPECT_EQ(8, StringUtil::LastIndexOf(src, 'o'));
  EXPECT_EQ(6, StringUtil::LastIndexOf(src, 'h'));
  EXPECT_EQ(5, StringUtil::LastIndexOf(src, 'c'));
  EXPECT_EQ(4, StringUtil::LastIndexOf(src, 's'));
  EXPECT_EQ(3, StringUtil::LastIndexOf(src, ' '));
  EXPECT_EQ(2, StringUtil::LastIndexOf(src, 'd'));

  EXPECT_GE(0, StringUtil::LastIndexOf(src, 'O'));
  EXPECT_GE(0, StringUtil::LastIndexOf(src, 'a'));
  EXPECT_GE(0, StringUtil::LastIndexOf(src, 'D'));
}


TEST(TestString_StringUtil, Split_Empty)
{
  std::string src;
  const auto result = StringUtil::Split(src, ' ', false);

  ASSERT_EQ(result.size(), 0u);
}


TEST(TestString_StringUtil, Split_HelloWorld)
{
  std::string src = "hello world";
  const auto result = StringUtil::Split(src, ' ', false);

  ASSERT_EQ(result.size(), 2u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "world");
}


TEST(TestString_StringUtil, Split_HelloWorld2_KeepEmpty)
{
  std::string src = "hello  world";
  const auto result = StringUtil::Split(src, ' ', false);

  ASSERT_EQ(result.size(), 3u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "");
  ASSERT_EQ(result[2], "world");
}


TEST(TestString_StringUtil, Split_HelloWorld2_RemoveEmpty)
{
  std::string src = "hello  world";
  const auto result = StringUtil::Split(src, ' ', true);

  ASSERT_EQ(result.size(), 2u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "world");
}


TEST(TestString_StringUtil, Split_MoreComplex)
{
  //                 0     1      2    3  4 5    6           7
  std::string src = "hello world, this is a more complicated example.";
  const auto result = StringUtil::Split(src, ' ', false);

  ASSERT_EQ(result.size(), 8u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "world,");
  ASSERT_EQ(result[2], "this");
  ASSERT_EQ(result[3], "is");
  ASSERT_EQ(result[4], "a");
  ASSERT_EQ(result[5], "more");
  ASSERT_EQ(result[6], "complicated");
  ASSERT_EQ(result[7], "example.");
}


TEST(TestString_StringUtil, Replace_Char)
{
  std::string src("old school");
  StringUtil::Replace(src, 'o', 'O');

  EXPECT_EQ(std::string("Old schOOl"), src);

  StringUtil::Replace(src, 'q', 'Q');

  EXPECT_EQ(std::string("Old schOOl"), src);

  std::string src2;
  StringUtil::Replace(src2, 'o', 'O');
  EXPECT_EQ(std::string(""), src2);
}


TEST(TestString_StringUtil, Replace_StringEmpty)
{
  std::string src("old school");
  StringUtil::Replace(src, "", "Hey");

  EXPECT_EQ(std::string("old school"), src);
}


TEST(TestString_StringUtil, Replace_StringSingleChar)
{
  std::string src("old school");
  StringUtil::Replace(src, "o", "O");

  EXPECT_EQ(std::string("Old schOOl"), src);
}


TEST(TestString_StringUtil, Replace_String1)
{
  std::string src("old school");
  StringUtil::Replace(src, "o", "O1");

  EXPECT_EQ(std::string("O1ld schO1O1l"), src);
}


TEST(TestString_StringUtil, Replace_String2)
{
  std::string src("old school");
  StringUtil::Replace(src, "l", "2");

  EXPECT_EQ(std::string("o2d schoo2"), src);
}


TEST(TestString_StringUtil, Replace_String3)
{
  std::string src("old school");
  StringUtil::Replace(src, "ool", "1234");

  EXPECT_EQ(std::string("old sch1234"), src);
}


TEST(TestString_StringUtil, Replace_String3a)
{
  std::string src("old school");
  StringUtil::Replace(src, "ool", "ool1");

  EXPECT_EQ(std::string("old school1"), src);
}

TEST(TestString_StringUtil, Replace_String4)
{
  std::string src("old school");
  StringUtil::Replace(src, "ool", "");

  EXPECT_EQ(std::string("old sch"), src);
}
