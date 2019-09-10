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

#include <FslBase/String/UTF8String.hpp>
#include <FslBase/Log/String/LogUTF8String.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <FslBase/Exceptions.hpp>
#include <cstring>
#include <utility>

using namespace Fsl;

namespace
{
  using TestString_UTF8String = TestFixtureFslBase;
}


TEST(TestString_UTF8String, Empty)
{
  UTF8String src;
}


TEST(TestString_UTF8String, Construct_CStyle)
{
  const char* psz = "old school";
  UTF8String src(psz);

  EXPECT_EQ(static_cast<std::size_t>(src.GetByteSize()), std::strlen(psz));
  EXPECT_EQ(src.ToAsciiString(), std::string(psz));
  EXPECT_EQ(src.ToUTF8String(), std::string(psz));
}

TEST(TestString_UTF8String, Construct_nullptr)
{
  const char* psz = nullptr;
  // EXPECT_THROW(UTF8String(psz), InvalidUTF8StringException);
  // EXPECT_THROW(UTF8String(nullptr), InvalidUTF8StringException);
  UTF8String n1(psz);
  UTF8String n2(nullptr);

  EXPECT_EQ(n1.GetByteSize(), 0);
  EXPECT_EQ(n2.GetByteSize(), 0);
}


TEST(TestString_UTF8String, Construct_StdString)
{
  //---------------01234567890
  std::string str("hello world");
  UTF8String src(str);

  EXPECT_EQ(static_cast<std::size_t>(src.GetByteSize()), str.size());
  EXPECT_EQ(src.ToAsciiString(), str);
  EXPECT_EQ(src.ToUTF8String(), str);
}


TEST(TestString_UTF8String, Construct_UTF8String)
{
  //-------------01234567890
  UTF8String str("hello world");
  UTF8String src(str);

  EXPECT_EQ(src.GetByteSize(), str.GetByteSize());
  EXPECT_EQ(src, str);
}


TEST(TestString_UTF8String, ConstructSubString_StdString)
{
  //---------------01234567890
  std::string str("hello world");
  UTF8String src(str, 6, 5);

  EXPECT_EQ(src.GetByteSize(), 5);
  EXPECT_EQ(src.ToAsciiString(), std::string("world"));
  EXPECT_EQ(src.ToUTF8String(), std::string("world"));
}


TEST(TestString_UTF8String, ConstructSubString_UTF8String)
{
  //-------------01234567890
  UTF8String str("hello world");
  UTF8String src(str, 6, 5);

  EXPECT_EQ(src.GetByteSize(), 5);
  EXPECT_EQ(src.ToAsciiString(), std::string("world"));
  EXPECT_EQ(src.ToUTF8String(), std::string("world"));
}


TEST(TestString_UTF8String, IsEmpty)
{
  UTF8String src;

  EXPECT_TRUE(src.IsEmpty());
}


TEST(TestString_UTF8String, IsEmpty_False)
{
  UTF8String src("hello world");

  EXPECT_FALSE(src.IsEmpty());
}


TEST(TestString_UTF8String, Clear)
{
  //-------------01234567890
  UTF8String str("hello world");
  UTF8String src(str);

  EXPECT_EQ(src.GetByteSize(), str.GetByteSize());
  EXPECT_EQ(src, str);
  src.Clear();
  EXPECT_EQ(src.GetByteSize(), 0);
  EXPECT_NE(src, str);
}


TEST(TestString_UTF8String, ContainsChar)
{
  UTF8String src("old school");

  EXPECT_TRUE(src.Contains('o'));
  EXPECT_TRUE(src.Contains('l'));
  EXPECT_TRUE(src.Contains('d'));
  EXPECT_TRUE(src.Contains(' '));
  EXPECT_TRUE(src.Contains('s'));
  EXPECT_TRUE(src.Contains('c'));
  EXPECT_TRUE(src.Contains('h'));

  EXPECT_FALSE(src.Contains('a'));
  EXPECT_FALSE(src.Contains('O'));
}


TEST(TestString_UTF8String, ContainsChar_Empty)
{
  UTF8String src("");

  EXPECT_FALSE(src.Contains('a'));
  EXPECT_FALSE(src.Contains('O'));
}


TEST(TestString_UTF8String, ContainsString)
{
  UTF8String src("old school");

  EXPECT_TRUE(src.Contains("old"));
  EXPECT_TRUE(src.Contains("school"));
  EXPECT_TRUE(src.Contains("d s"));

  EXPECT_FALSE(src.Contains("world"));
  EXPECT_FALSE(src.Contains("schoow"));
  EXPECT_FALSE(src.Contains(".old school"));
  EXPECT_FALSE(src.Contains("old school."));

  // Empty string
  EXPECT_TRUE(src.Contains(""));

  // EXPECT_THROW(src.Contains(nullptr), InvalidUTF8StringException);
  EXPECT_TRUE(src.Contains(nullptr));
}


TEST(TestString_UTF8String, ContainsString_Empty)
{
  UTF8String src("");

  EXPECT_TRUE(src.Contains(""));

  EXPECT_FALSE(src.Contains("world"));
  EXPECT_FALSE(src.Contains("schoow"));
  EXPECT_FALSE(src.Contains(".old school"));
  EXPECT_FALSE(src.Contains("old school."));
}

TEST(TestString_UTF8String, StartsWithChar)
{
  UTF8String src("old school");

  EXPECT_TRUE(src.StartsWith('o'));

  EXPECT_FALSE(src.StartsWith('a'));
  EXPECT_FALSE(src.StartsWith('O'));
  EXPECT_FALSE(src.StartsWith('1'));
}


TEST(TestString_UTF8String, StartsWithChar_Empty)
{
  UTF8String src("");

  EXPECT_FALSE(src.StartsWith('a'));
  EXPECT_FALSE(src.StartsWith('O'));
  EXPECT_FALSE(src.StartsWith('1'));
}


TEST(TestString_UTF8String, StartsWithString)
{
  UTF8String src("old school");

  EXPECT_TRUE(src.StartsWith(""));
  EXPECT_TRUE(src.StartsWith("o"));
  EXPECT_TRUE(src.StartsWith("ol"));
  EXPECT_TRUE(src.StartsWith("old"));
  EXPECT_TRUE(src.StartsWith("old "));
  EXPECT_TRUE(src.StartsWith("old s"));
  EXPECT_TRUE(src.StartsWith("old sc"));
  EXPECT_TRUE(src.StartsWith("old sch"));
  EXPECT_TRUE(src.StartsWith("old scho"));
  EXPECT_TRUE(src.StartsWith("old schoo"));
  EXPECT_TRUE(src.StartsWith("old school"));

  EXPECT_FALSE(src.StartsWith("O"));
  EXPECT_FALSE(src.StartsWith("l"));
  EXPECT_FALSE(src.StartsWith("ld"));
  EXPECT_FALSE(src.StartsWith("school"));

  // Controversial, but its because nullptr results in a empty string
  EXPECT_TRUE(src.StartsWith(nullptr));
}


TEST(TestString_UTF8String, StartsWithString_Empty)
{
  UTF8String src("");

  EXPECT_TRUE(src.StartsWith(""));

  EXPECT_FALSE(src.StartsWith("O"));
  EXPECT_FALSE(src.StartsWith("l"));
  EXPECT_FALSE(src.StartsWith("ld"));
  EXPECT_FALSE(src.StartsWith("school"));
}


TEST(TestString_UTF8String, EndsWithChar)
{
  UTF8String src("old school");

  EXPECT_TRUE(src.EndsWith('l'));

  EXPECT_FALSE(src.EndsWith('L'));
  EXPECT_FALSE(src.EndsWith('o'));
  EXPECT_FALSE(src.EndsWith('d'));
}


TEST(TestString_UTF8String, EndsWithChar_Empty)
{
  UTF8String src("");

  EXPECT_FALSE(src.EndsWith('l'));
  EXPECT_FALSE(src.EndsWith('L'));
  EXPECT_FALSE(src.EndsWith('o'));
  EXPECT_FALSE(src.EndsWith('d'));
}


TEST(TestString_UTF8String, EndsWithString)
{
  UTF8String src("old school");

  EXPECT_TRUE(src.EndsWith(""));
  EXPECT_TRUE(src.EndsWith("l"));
  EXPECT_TRUE(src.EndsWith("ol"));
  EXPECT_TRUE(src.EndsWith("ool"));
  EXPECT_TRUE(src.EndsWith("hool"));
  EXPECT_TRUE(src.EndsWith("chool"));
  EXPECT_TRUE(src.EndsWith("school"));
  EXPECT_TRUE(src.EndsWith(" school"));
  EXPECT_TRUE(src.EndsWith("d school"));
  EXPECT_TRUE(src.EndsWith("ld school"));
  EXPECT_TRUE(src.EndsWith("old school"));

  EXPECT_FALSE(src.EndsWith("L"));
  EXPECT_FALSE(src.EndsWith("o"));
  EXPECT_FALSE(src.EndsWith(" "));
  EXPECT_FALSE(src.EndsWith("old"));

  // Controversial, but its because nullptr results in a empty string
  EXPECT_TRUE(src.EndsWith(nullptr));
}


TEST(TestString_UTF8String, EndsWithString_Empty)
{
  UTF8String src("");

  EXPECT_TRUE(src.EndsWith(""));
  EXPECT_FALSE(src.EndsWith("l"));
}


TEST(TestString_UTF8String, ReplaceChar)
{
  UTF8String src("old school");
  src.Replace('o', 'O');

  EXPECT_EQ(src, UTF8String("Old schOOl"));

  src.Replace('q', 'Q');

  EXPECT_EQ(src, UTF8String("Old schOOl"));

  UTF8String src2("");
  src.Replace('o', 'O');
  EXPECT_EQ(src2, UTF8String(""));
}


TEST(TestString_UTF8String, IndexOfChar)
{
  //--------------0123456789
  UTF8String src("old school");

  EXPECT_EQ(src.IndexOf('o'), 0);
  EXPECT_EQ(src.IndexOf('l'), 1);
  EXPECT_EQ(src.IndexOf('d'), 2);
  EXPECT_EQ(src.IndexOf(' '), 3);
  EXPECT_EQ(src.IndexOf('s'), 4);
  EXPECT_EQ(src.IndexOf('c'), 5);
  EXPECT_EQ(src.IndexOf('h'), 6);

  EXPECT_EQ(src.IndexOf('o', 1), 7);
  EXPECT_EQ(src.IndexOf('o', 7), 7);
  EXPECT_EQ(src.IndexOf('o', 8), 8);
  EXPECT_EQ(src.IndexOf('l', 2), 9);

  EXPECT_LT(src.IndexOf('O'), 0);
  EXPECT_LT(src.IndexOf('a'), 0);
  EXPECT_LT(src.IndexOf('D'), 0);

  EXPECT_LT(src.IndexOf('o', 9), 0);
  EXPECT_LT(src.IndexOf('o', 10), 0);
  EXPECT_LT(src.IndexOf('o', 11), 0);
  EXPECT_LT(src.IndexOf('o', 12), 0);
}


TEST(TestString_UTF8String, LastIndexOfChar)
{
  //--------------0123456789
  UTF8String src("old school");

  EXPECT_EQ(src.LastIndexOf('l'), 9);
  EXPECT_EQ(src.LastIndexOf('o'), 8);
  EXPECT_EQ(src.LastIndexOf('h'), 6);
  EXPECT_EQ(src.LastIndexOf('c'), 5);
  EXPECT_EQ(src.LastIndexOf('s'), 4);
  EXPECT_EQ(src.LastIndexOf(' '), 3);
  EXPECT_EQ(src.LastIndexOf('d'), 2);


  EXPECT_LT(src.LastIndexOf('O'), 0);
  EXPECT_LT(src.LastIndexOf('a'), 0);
  EXPECT_LT(src.LastIndexOf('D'), 0);
}


TEST(TestString_UTF8String, Reset)
{
  //--------------0123456789
  UTF8String src("old school");
  //---------0123456789
  src.Reset("the quick brown fox", 4, 5);
  EXPECT_EQ(src, UTF8String("quick"));
}


TEST(TestString_UTF8String, OperatorLessThan)
{
  EXPECT_LT(UTF8String("A"), UTF8String("B"));
  EXPECT_LT(UTF8String("A"), UTF8String("AB"));
  EXPECT_LT(UTF8String(""), UTF8String("AB"));

  EXPECT_FALSE(UTF8String("A") < UTF8String("A"));
}


TEST(TestString_UTF8String, OperatorLessThanOrEqual)
{
  EXPECT_LE(UTF8String("A"), UTF8String("B"));
  EXPECT_LE(UTF8String("A"), UTF8String("AB"));
  EXPECT_LE(UTF8String(""), UTF8String("AB"));
  EXPECT_LE(UTF8String("A"), UTF8String("A"));
}


TEST(TestString_UTF8String, OperatorGreaterThan)
{
  EXPECT_GT(UTF8String("B"), UTF8String("A"));
  EXPECT_GT(UTF8String("AB"), UTF8String("A"));
  EXPECT_GT(UTF8String("AB"), UTF8String(""));

  EXPECT_FALSE(UTF8String("A") > UTF8String("A"));
}


TEST(TestString_UTF8String, OperatorGreaterThanOrEqual)
{
  EXPECT_GE(UTF8String("B"), UTF8String("A"));
  EXPECT_GE(UTF8String("AB"), UTF8String("A"));
  EXPECT_GE(UTF8String("AB"), UTF8String(""));
  EXPECT_GE(UTF8String("A"), UTF8String("A"));
}


TEST(TestString_UTF8String, OperatorEqual)
{
  EXPECT_TRUE(UTF8String("A") == UTF8String("A"));
  EXPECT_TRUE(UTF8String("") == UTF8String(""));
  EXPECT_FALSE(UTF8String("A") == UTF8String(""));
  EXPECT_FALSE(UTF8String("A") == UTF8String("B"));
  EXPECT_FALSE(UTF8String("A") == UTF8String("a"));
}


TEST(TestString_UTF8String, OperatorNotEqual)
{
  EXPECT_FALSE(UTF8String("A") != UTF8String("A"));
  EXPECT_FALSE(UTF8String("") != UTF8String(""));
  EXPECT_TRUE(UTF8String("A") != UTF8String(""));
  EXPECT_TRUE(UTF8String("A") != UTF8String("B"));
  EXPECT_TRUE(UTF8String("A") != UTF8String("a"));
}


TEST(TestString_UTF8String, MoveAssignment)
{
  UTF8String str1("hello");
  UTF8String str2("hey");

  str2 = std::move(str1);
  // Move does not have to change the content of str1 so this check could fail
  // EXPECT_EQ(str1, "");
  EXPECT_EQ(str2, "hello");
}


TEST(TestString_UTF8String, MoveConstruct)
{
  UTF8String str1("hello");
  UTF8String str2(std::move(str1));

  // Move does not have to change the content of str1 so this check could fail
  // EXPECT_EQ(str1, "");
  EXPECT_EQ(str2, "hello");
}
