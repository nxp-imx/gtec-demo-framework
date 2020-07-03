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
#include <FslBase/Log/String/LogStringViewLite.hpp>
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


TEST(TestString_UTF8String, Construct_CString)
{
  //---------------01234567890
  const char* const psz = "hello world";
  UTF8String src(psz);

  std::string str(psz);
  EXPECT_EQ(static_cast<std::size_t>(src.GetByteSize()), str.size());
  EXPECT_EQ(src.ToAsciiString(), str);
  EXPECT_EQ(src.ToUTF8String(), str);
}

TEST(TestString_UTF8String, Construct_StringViewLite)
{
  //---------------01234567890
  constexpr StringViewLite str("hello world");
  UTF8String src(str);

  EXPECT_EQ(static_cast<std::size_t>(src.GetByteSize()), str.size());
  EXPECT_EQ(src.ToAsciiString(), str);
  EXPECT_EQ(src.ToUTF8String(), str);
}

TEST(TestString_UTF8String, Construct_StringViewLite_Empty)
{
  //---------------01234567890
  constexpr StringViewLite str;
  UTF8String src(str);

  EXPECT_EQ(static_cast<std::size_t>(src.GetByteSize()), str.size());
  EXPECT_EQ(src.ToAsciiString(), str);
  EXPECT_EQ(src.ToUTF8String(), str);
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
  const UTF8String& src(str);

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

TEST(TestString_UTF8String, ConstructSubString_StdString_InvalidStartIndex)
{
  //---------------01234567890
  std::string str("hello world");

  EXPECT_THROW(UTF8String(str, 100, 5), std::invalid_argument);
}

TEST(TestString_UTF8String, ConstructSubString_StdString_InvalidLength)
{
  //---------------01234567890
  std::string str("hello world");

  EXPECT_THROW(UTF8String(str, 6, 6), std::invalid_argument);
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

TEST(TestString_UTF8String, ConstructSubString_UTF8String_InvalidStartIndex)
{
  //---------------01234567890
  UTF8String str("hello world");

  EXPECT_THROW(UTF8String(str, 100, 5), std::invalid_argument);
}

TEST(TestString_UTF8String, ConstructSubString_UTF8String_InvalidLength)
{
  //---------------01234567890
  UTF8String str("hello world");

  EXPECT_THROW(UTF8String(str, 6, 6), std::invalid_argument);
}

TEST(TestString_UTF8String, Append_InvalidChar)
{
  //---------------01234567890
  UTF8String str("hello world");

  EXPECT_THROW(str.Append(1, static_cast<char>(0xFF)), std::invalid_argument);
}


TEST(TestString_UTF8String, Append_Char)
{
  //---------------01234567890
  UTF8String str("hello world");
  str.Append(1, '!');

  EXPECT_EQ(StringViewLite("hello world!"), str);

  str.Append(2, '!');
  EXPECT_EQ(StringViewLite("hello world!!!"), str);
}


TEST(TestString_UTF8String, Append_CString)
{
  //---------------01234567890
  UTF8String str("hello");

  str.Append(" world");

  EXPECT_EQ(StringViewLite("hello world"), str);
}

TEST(TestString_UTF8String, Append_CString_nullptr)
{
  //---------------01234567890
  UTF8String str("hello");

  const char* const pszNull = nullptr;

  str.Append(pszNull);

  EXPECT_EQ(StringViewLite("hello"), str);
}


TEST(TestString_UTF8String, Append_StringViewLite)
{
  //---------------01234567890
  UTF8String str("hello");

  str.Append(StringViewLite(" world"));

  EXPECT_EQ(StringViewLite("hello world"), str);
}

TEST(TestString_UTF8String, Append_StringViewLite_Empty)
{
  //---------------01234567890
  UTF8String str("hello");

  str.Append(StringViewLite());

  EXPECT_EQ(StringViewLite("hello"), str);
}


TEST(TestString_UTF8String, Append_UTF8String)
{
  //---------------01234567890
  UTF8String str("hello");

  str.Append(UTF8String(" world"));

  EXPECT_EQ(StringViewLite("hello world"), str);
}


TEST(TestString_UTF8String, Append_UTF8String_empty)
{
  //---------------01234567890
  UTF8String str("hello");

  UTF8String empty;
  str.Append(empty);

  EXPECT_EQ(StringViewLite("hello"), str);
}


TEST(TestString_UTF8String, Prepend_Char_InvalidChar)
{
  //---------------01234567890
  UTF8String str("hello world");

  EXPECT_THROW(str.Prepend(1, static_cast<char>(0xFF)), std::invalid_argument);
}


TEST(TestString_UTF8String, Prepend_Char)
{
  //---------------01234567890
  UTF8String str("hello world");
  str.Prepend(1, '!');

  EXPECT_EQ(StringViewLite("!hello world"), str);

  str.Prepend(2, '!');
  EXPECT_EQ(StringViewLite("!!!hello world"), str);
}


TEST(TestString_UTF8String, Prepend_CString)
{
  //---------------01234567890
  UTF8String str("world");

  str.Prepend("hello ");

  EXPECT_EQ(StringViewLite("hello world"), str);
}


TEST(TestString_UTF8String, Prepend_CString_nullptr)
{
  //---------------01234567890
  UTF8String str("world");

  const char* const pszNull = nullptr;

  str.Prepend(pszNull);

  EXPECT_EQ(StringViewLite("world"), str);
}


TEST(TestString_UTF8String, Prepend_StringViewLite)
{
  //---------------01234567890
  UTF8String str("world");

  str.Prepend(StringViewLite("hello "));

  EXPECT_EQ(StringViewLite("hello world"), str);
}


TEST(TestString_UTF8String, Prepend_StringViewLite_Empty)
{
  //---------------01234567890
  UTF8String str("world");

  str.Prepend(StringViewLite());

  EXPECT_EQ(StringViewLite("world"), str);
}


TEST(TestString_UTF8String, Prepend_UTF8String)
{
  //---------------01234567890
  UTF8String str("world");

  str.Prepend(UTF8String("hello "));

  EXPECT_EQ(StringViewLite("hello world"), str);
}


TEST(TestString_UTF8String, Prepend_UTF8String_Empty)
{
  //---------------01234567890
  UTF8String str("world");

  UTF8String empty;
  str.Prepend(empty);

  EXPECT_EQ(StringViewLite("world"), str);
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


TEST(TestString_UTF8String, ContainsChar_InvalidChar)
{
  UTF8String src("old school");

  EXPECT_THROW(src.Contains(static_cast<char>(0xFF)), std::invalid_argument);
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


TEST(TestString_UTF8String, StartsWithChar_InvalidChar)
{
  UTF8String src("old school");

  EXPECT_THROW(src.StartsWith(static_cast<char>(0xFF)), std::invalid_argument);
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


TEST(TestString_UTF8String, StartsWith_CString)
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
}


TEST(TestString_UTF8String, StartsWith_StringViewLite)
{
  UTF8String src("old school");

  EXPECT_TRUE(src.StartsWith(StringViewLite("")));
  EXPECT_TRUE(src.StartsWith(StringViewLite("o")));
  EXPECT_TRUE(src.StartsWith(StringViewLite("ol")));
  EXPECT_TRUE(src.StartsWith(StringViewLite("old")));
  EXPECT_TRUE(src.StartsWith(StringViewLite("old ")));
  EXPECT_TRUE(src.StartsWith(StringViewLite("old s")));
  EXPECT_TRUE(src.StartsWith(StringViewLite("old sc")));
  EXPECT_TRUE(src.StartsWith(StringViewLite("old sch")));
  EXPECT_TRUE(src.StartsWith(StringViewLite("old scho")));
  EXPECT_TRUE(src.StartsWith(StringViewLite("old schoo")));
  EXPECT_TRUE(src.StartsWith(StringViewLite("old school")));

  EXPECT_FALSE(src.StartsWith(StringViewLite("O")));
  EXPECT_FALSE(src.StartsWith(StringViewLite("l")));
  EXPECT_FALSE(src.StartsWith(StringViewLite("ld")));
  EXPECT_FALSE(src.StartsWith(StringViewLite("school")));
}


TEST(TestString_UTF8String, StartsWith_UTF8String)
{
  UTF8String src("old school");

  EXPECT_TRUE(src.StartsWith(UTF8String("")));
  EXPECT_TRUE(src.StartsWith(UTF8String("o")));
  EXPECT_TRUE(src.StartsWith(UTF8String("ol")));
  EXPECT_TRUE(src.StartsWith(UTF8String("old")));
  EXPECT_TRUE(src.StartsWith(UTF8String("old ")));
  EXPECT_TRUE(src.StartsWith(UTF8String("old s")));
  EXPECT_TRUE(src.StartsWith(UTF8String("old sc")));
  EXPECT_TRUE(src.StartsWith(UTF8String("old sch")));
  EXPECT_TRUE(src.StartsWith(UTF8String("old scho")));
  EXPECT_TRUE(src.StartsWith(UTF8String("old schoo")));
  EXPECT_TRUE(src.StartsWith(UTF8String("old school")));

  EXPECT_FALSE(src.StartsWith(UTF8String("O")));
  EXPECT_FALSE(src.StartsWith(UTF8String("l")));
  EXPECT_FALSE(src.StartsWith(UTF8String("ld")));
  EXPECT_FALSE(src.StartsWith(UTF8String("school")));
}

TEST(TestString_UTF8String, StartsWithString_Empty_CString)
{
  UTF8String src;

  EXPECT_TRUE(src.StartsWith(""));

  EXPECT_FALSE(src.StartsWith("O"));
  EXPECT_FALSE(src.StartsWith("l"));
  EXPECT_FALSE(src.StartsWith("ld"));
  EXPECT_FALSE(src.StartsWith("school"));
}

TEST(TestString_UTF8String, StartsWithString_Empty_StringViewLite)
{
  UTF8String src;

  EXPECT_TRUE(src.StartsWith(StringViewLite("")));

  EXPECT_FALSE(src.StartsWith(StringViewLite("O")));
  EXPECT_FALSE(src.StartsWith(StringViewLite("l")));
  EXPECT_FALSE(src.StartsWith(StringViewLite("ld")));
  EXPECT_FALSE(src.StartsWith(StringViewLite("school")));
}

TEST(TestString_UTF8String, StartsWithString_Empty_UTF8String)
{
  UTF8String src;

  EXPECT_TRUE(src.StartsWith(UTF8String("")));

  EXPECT_FALSE(src.StartsWith(UTF8String("O")));
  EXPECT_FALSE(src.StartsWith(UTF8String("l")));
  EXPECT_FALSE(src.StartsWith(UTF8String("ld")));
  EXPECT_FALSE(src.StartsWith(UTF8String("school")));
}


TEST(TestString_UTF8String, StartsWith_CString_Null)
{
  UTF8String src("old school");

  const char* const pszNull = nullptr;

  EXPECT_TRUE(src.StartsWith(pszNull));
}


TEST(TestString_UTF8String, StartsWith_StringViewLite_Empty)
{
  UTF8String src("old school");

  StringViewLite strEmpty;

  EXPECT_TRUE(src.StartsWith(strEmpty));
}


TEST(TestString_UTF8String, EndsWithChar_InvalidChar)
{
  UTF8String src("old school");

  EXPECT_THROW(src.EndsWith(static_cast<char>(0xFF)), std::invalid_argument);
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


TEST(TestString_UTF8String, EndsWith_CString)
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
}


TEST(TestString_UTF8String, EndsWith_StringViewLite)
{
  UTF8String src("old school");

  EXPECT_TRUE(src.EndsWith(StringViewLite("")));
  EXPECT_TRUE(src.EndsWith(StringViewLite("l")));
  EXPECT_TRUE(src.EndsWith(StringViewLite("ol")));
  EXPECT_TRUE(src.EndsWith(StringViewLite("ool")));
  EXPECT_TRUE(src.EndsWith(StringViewLite("hool")));
  EXPECT_TRUE(src.EndsWith(StringViewLite("chool")));
  EXPECT_TRUE(src.EndsWith(StringViewLite("school")));
  EXPECT_TRUE(src.EndsWith(StringViewLite(" school")));
  EXPECT_TRUE(src.EndsWith(StringViewLite("d school")));
  EXPECT_TRUE(src.EndsWith(StringViewLite("ld school")));
  EXPECT_TRUE(src.EndsWith(StringViewLite("old school")));

  EXPECT_FALSE(src.EndsWith(StringViewLite("L")));
  EXPECT_FALSE(src.EndsWith(StringViewLite("o")));
  EXPECT_FALSE(src.EndsWith(StringViewLite(" ")));
  EXPECT_FALSE(src.EndsWith(StringViewLite("old")));
}


TEST(TestString_UTF8String, EndsWith_UTF8String)
{
  UTF8String src("old school");

  EXPECT_TRUE(src.EndsWith(UTF8String("")));
  EXPECT_TRUE(src.EndsWith(UTF8String("l")));
  EXPECT_TRUE(src.EndsWith(UTF8String("ol")));
  EXPECT_TRUE(src.EndsWith(UTF8String("ool")));
  EXPECT_TRUE(src.EndsWith(UTF8String("hool")));
  EXPECT_TRUE(src.EndsWith(UTF8String("chool")));
  EXPECT_TRUE(src.EndsWith(UTF8String("school")));
  EXPECT_TRUE(src.EndsWith(UTF8String(" school")));
  EXPECT_TRUE(src.EndsWith(UTF8String("d school")));
  EXPECT_TRUE(src.EndsWith(UTF8String("ld school")));
  EXPECT_TRUE(src.EndsWith(UTF8String("old school")));

  EXPECT_FALSE(src.EndsWith(UTF8String("L")));
  EXPECT_FALSE(src.EndsWith(UTF8String("o")));
  EXPECT_FALSE(src.EndsWith(UTF8String(" ")));
  EXPECT_FALSE(src.EndsWith(UTF8String("old")));
}


TEST(TestString_UTF8String, EndsWithString_Empty_CString)
{
  UTF8String src("");

  EXPECT_TRUE(src.EndsWith(""));
  EXPECT_FALSE(src.EndsWith("l"));
}


TEST(TestString_UTF8String, EndsWith_CString_Null)
{
  UTF8String src("old school");

  const char* const pszNull = nullptr;

  EXPECT_TRUE(src.EndsWith(pszNull));
}


TEST(TestString_UTF8String, EndsWith_StringViewLite_Empty)
{
  UTF8String src("old school");

  StringViewLite strEmpty;

  EXPECT_TRUE(src.EndsWith(strEmpty));
}


TEST(TestString_UTF8String, EndsWith_UTF8String_Empty)
{
  UTF8String src("old school");

  UTF8String strEmpty;

  EXPECT_TRUE(src.EndsWith(strEmpty));
}


TEST(TestString_UTF8String, ReplaceChar_InvalidFromChar)
{
  UTF8String src("old school");
  EXPECT_THROW(src.Replace(static_cast<char>(0xFF), 'O'), std::invalid_argument);
}

TEST(TestString_UTF8String, ReplaceChar_InvalidToChar)
{
  UTF8String src("old school");
  EXPECT_THROW(src.Replace('o', static_cast<char>(0xFF)), std::invalid_argument);
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


TEST(TestString_UTF8String, IndexOfChar_InvalidChar)
{
  //--------------0123456789
  UTF8String src("old school");

  EXPECT_THROW(src.IndexOf(static_cast<char>(0xFF)), std::invalid_argument);
}

TEST(TestString_UTF8String, IndexOfChar2_InvalidChar)
{
  //--------------0123456789
  UTF8String src("old school");

  EXPECT_THROW(src.IndexOf(static_cast<char>(0xFF), 2u), std::invalid_argument);
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


TEST(TestString_UTF8String, LastIndexOfChar_InvalidChar)
{
  //--------------0123456789
  UTF8String src("old school");

  EXPECT_THROW(src.LastIndexOf(static_cast<char>(0xFF)), std::invalid_argument);
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

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator=
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestString_UTF8String, OpAssign_CString)
{
  //--------------0123456789
  UTF8String src("old school");
  //---------0123456789

  const char* const pszNew = "the quick brown fox";
  src = pszNew;
  EXPECT_EQ(pszNew, src);
}

TEST(TestString_UTF8String, OpAssign_StringViewLite)
{
  //--------------0123456789
  UTF8String src("old school");
  //---------0123456789

  constexpr StringViewLite strNew("the quick brown fox");
  src = strNew;
  EXPECT_EQ(strNew, src);
}

TEST(TestString_UTF8String, OpAssign_UTF8String)
{
  //--------------0123456789
  UTF8String src("old school");
  //---------0123456789

  UTF8String strNew("the quick brown fox");

  src = strNew;
  EXPECT_EQ(strNew, src);
}

TEST(TestString_UTF8String, OpAssign_CString_Null)
{
  //--------------0123456789
  UTF8String src("old school");
  //---------0123456789

  constexpr const char* const pszNull = nullptr;
  src = pszNull;

  EXPECT_EQ("", src);
}


TEST(TestString_UTF8String, OpAssign_StringViewLite_Empty)
{
  //--------------0123456789
  UTF8String src("old school");
  //---------0123456789

  constexpr StringViewLite strNew;
  src = strNew;
  EXPECT_EQ(strNew, src);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_UTF8String, OperatorLessThan)
{
  EXPECT_LT(UTF8String("A"), UTF8String("B"));
  EXPECT_LT(UTF8String("A"), UTF8String("AB"));
  EXPECT_LT(UTF8String(""), UTF8String("AB"));

  EXPECT_TRUE(UTF8String("A") < UTF8String("B"));
  EXPECT_FALSE(UTF8String("B") < UTF8String("B"));
  EXPECT_FALSE(UTF8String("C") < UTF8String("B"));
  EXPECT_FALSE(UTF8String("B") < UTF8String("A"));
  EXPECT_FALSE(UTF8String("B") < UTF8String("B"));
  EXPECT_TRUE(UTF8String("B") < UTF8String("C"));

  EXPECT_TRUE(UTF8String("A") < UTF8String("BA"));
  EXPECT_TRUE(UTF8String("B") < UTF8String("BA"));
  EXPECT_FALSE(UTF8String("C") < UTF8String("BA"));
  EXPECT_FALSE(UTF8String("BA") < UTF8String("A"));
  EXPECT_FALSE(UTF8String("BA") < UTF8String("B"));
  EXPECT_TRUE(UTF8String("BA") < UTF8String("C"));

  EXPECT_TRUE(UTF8String("AA") < UTF8String("B"));
  EXPECT_FALSE(UTF8String("BA") < UTF8String("B"));
  EXPECT_FALSE(UTF8String("CA") < UTF8String("B"));
  EXPECT_FALSE(UTF8String("B") < UTF8String("AA"));
  EXPECT_TRUE(UTF8String("B") < UTF8String("BA"));
  EXPECT_TRUE(UTF8String("B") < UTF8String("CA"));
}

TEST(TestString_UTF8String, OperatorLessThan_CString_LHS)
{
  EXPECT_LT("A", UTF8String("B"));
  EXPECT_LT("A", UTF8String("AB"));
  EXPECT_LT("", UTF8String("AB"));

  EXPECT_TRUE("A" < UTF8String("B"));
  EXPECT_FALSE("B" < UTF8String("B"));
  EXPECT_FALSE("C" < UTF8String("B"));
  EXPECT_FALSE("B" < UTF8String("A"));
  EXPECT_FALSE("B" < UTF8String("B"));
  EXPECT_TRUE("B" < UTF8String("C"));

  EXPECT_TRUE("A" < UTF8String("BA"));
  EXPECT_TRUE("B" < UTF8String("BA"));
  EXPECT_FALSE("C" < UTF8String("BA"));
  EXPECT_FALSE("BA" < UTF8String("A"));
  EXPECT_FALSE("BA" < UTF8String("B"));
  EXPECT_TRUE("BA" < UTF8String("C"));

  EXPECT_TRUE("AA" < UTF8String("B"));
  EXPECT_FALSE("BA" < UTF8String("B"));
  EXPECT_FALSE("CA" < UTF8String("B"));
  EXPECT_FALSE("B" < UTF8String("AA"));
  EXPECT_TRUE("B" < UTF8String("BA"));
  EXPECT_TRUE("B" < UTF8String("CA"));
}

TEST(TestString_UTF8String, OperatorLessThan_CString_RHS)
{
  EXPECT_LT(UTF8String("A"), "B");
  EXPECT_LT(UTF8String("A"), "AB");
  EXPECT_LT(UTF8String(""), "AB");

  EXPECT_TRUE(UTF8String("A") < "B");
  EXPECT_FALSE(UTF8String("B") < "B");
  EXPECT_FALSE(UTF8String("C") < "B");
  EXPECT_FALSE(UTF8String("B") < "A");
  EXPECT_FALSE(UTF8String("B") < "B");
  EXPECT_TRUE(UTF8String("B") < "C");

  EXPECT_TRUE(UTF8String("A") < "BA");
  EXPECT_TRUE(UTF8String("B") < "BA");
  EXPECT_FALSE(UTF8String("C") < "BA");
  EXPECT_FALSE(UTF8String("BA") < "A");
  EXPECT_FALSE(UTF8String("BA") < "B");
  EXPECT_TRUE(UTF8String("BA") < "C");

  EXPECT_TRUE(UTF8String("AA") < "B");
  EXPECT_FALSE(UTF8String("BA") < "B");
  EXPECT_FALSE(UTF8String("CA") < "B");
  EXPECT_FALSE(UTF8String("B") < "AA");
  EXPECT_TRUE(UTF8String("B") < "BA");
  EXPECT_TRUE(UTF8String("B") < "CA");
}

TEST(TestString_UTF8String, OperatorLessThan_StringViewLite_LHS)
{
  EXPECT_LT(StringViewLite("A"), UTF8String("B"));
  EXPECT_LT(StringViewLite("A"), UTF8String("AB"));
  EXPECT_LT(StringViewLite(""), UTF8String("AB"));

  EXPECT_TRUE(StringViewLite("A") < UTF8String("B"));
  EXPECT_FALSE(StringViewLite("B") < UTF8String("B"));
  EXPECT_FALSE(StringViewLite("C") < UTF8String("B"));
  EXPECT_FALSE(StringViewLite("B") < UTF8String("A"));
  EXPECT_FALSE(StringViewLite("B") < UTF8String("B"));
  EXPECT_TRUE(StringViewLite("B") < UTF8String("C"));

  EXPECT_TRUE(StringViewLite("A") < UTF8String("BA"));
  EXPECT_TRUE(StringViewLite("B") < UTF8String("BA"));
  EXPECT_FALSE(StringViewLite("C") < UTF8String("BA"));
  EXPECT_FALSE(StringViewLite("BA") < UTF8String("A"));
  EXPECT_FALSE(StringViewLite("BA") < UTF8String("B"));
  EXPECT_TRUE(StringViewLite("BA") < UTF8String("C"));

  EXPECT_TRUE(StringViewLite("AA") < UTF8String("B"));
  EXPECT_FALSE(StringViewLite("BA") < UTF8String("B"));
  EXPECT_FALSE(StringViewLite("CA") < UTF8String("B"));
  EXPECT_FALSE(StringViewLite("B") < UTF8String("AA"));
  EXPECT_TRUE(StringViewLite("B") < UTF8String("BA"));
  EXPECT_TRUE(StringViewLite("B") < UTF8String("CA"));
}

TEST(TestString_UTF8String, OperatorLessThan_StringViewLite_RHS)
{
  EXPECT_LT(UTF8String("A"), StringViewLite("B"));
  EXPECT_LT(UTF8String("A"), StringViewLite("AB"));
  EXPECT_LT(UTF8String(""), StringViewLite("AB"));

  EXPECT_TRUE(UTF8String("A") < StringViewLite("B"));
  EXPECT_FALSE(UTF8String("B") < StringViewLite("B"));
  EXPECT_FALSE(UTF8String("C") < StringViewLite("B"));
  EXPECT_FALSE(UTF8String("B") < StringViewLite("A"));
  EXPECT_FALSE(UTF8String("B") < StringViewLite("B"));
  EXPECT_TRUE(UTF8String("B") < StringViewLite("C"));

  EXPECT_TRUE(UTF8String("A") < StringViewLite("BA"));
  EXPECT_TRUE(UTF8String("B") < StringViewLite("BA"));
  EXPECT_FALSE(UTF8String("C") < StringViewLite("BA"));
  EXPECT_FALSE(UTF8String("BA") < StringViewLite("A"));
  EXPECT_FALSE(UTF8String("BA") < StringViewLite("B"));
  EXPECT_TRUE(UTF8String("BA") < StringViewLite("C"));

  EXPECT_TRUE(UTF8String("AA") < StringViewLite("B"));
  EXPECT_FALSE(UTF8String("BA") < StringViewLite("B"));
  EXPECT_FALSE(UTF8String("CA") < StringViewLite("B"));
  EXPECT_FALSE(UTF8String("B") < StringViewLite("AA"));
  EXPECT_TRUE(UTF8String("B") < StringViewLite("BA"));
  EXPECT_TRUE(UTF8String("B") < StringViewLite("CA"));
}

TEST(TestString_UTF8String, OperatorLessThan_String_LHS)
{
  EXPECT_LT(std::string("A"), UTF8String("B"));
  EXPECT_LT(std::string("A"), UTF8String("AB"));
  EXPECT_LT(std::string(""), UTF8String("AB"));


  EXPECT_TRUE(std::string("A") < UTF8String("B"));
  EXPECT_FALSE(std::string("B") < UTF8String("B"));
  EXPECT_FALSE(std::string("C") < UTF8String("B"));
  EXPECT_FALSE(std::string("B") < UTF8String("A"));
  EXPECT_FALSE(std::string("B") < UTF8String("B"));
  EXPECT_TRUE(std::string("B") < UTF8String("C"));

  EXPECT_TRUE(std::string("A") < UTF8String("BA"));
  EXPECT_TRUE(std::string("B") < UTF8String("BA"));
  EXPECT_FALSE(std::string("C") < UTF8String("BA"));
  EXPECT_FALSE(std::string("BA") < UTF8String("A"));
  EXPECT_FALSE(std::string("BA") < UTF8String("B"));
  EXPECT_TRUE(std::string("BA") < UTF8String("C"));

  EXPECT_TRUE(std::string("AA") < UTF8String("B"));
  EXPECT_FALSE(std::string("BA") < UTF8String("B"));
  EXPECT_FALSE(std::string("CA") < UTF8String("B"));
  EXPECT_FALSE(std::string("B") < UTF8String("AA"));
  EXPECT_TRUE(std::string("B") < UTF8String("BA"));
  EXPECT_TRUE(std::string("B") < UTF8String("CA"));
}

TEST(TestString_UTF8String, OperatorLessThan_String_RHS)
{
  EXPECT_LT(UTF8String("A"), std::string("B"));
  EXPECT_LT(UTF8String("A"), std::string("AB"));
  EXPECT_LT(UTF8String(""), std::string("AB"));

  EXPECT_TRUE(UTF8String("A") < std::string("B"));
  EXPECT_FALSE(UTF8String("B") < std::string("B"));
  EXPECT_FALSE(UTF8String("C") < std::string("B"));
  EXPECT_FALSE(UTF8String("B") < std::string("A"));
  EXPECT_FALSE(UTF8String("B") < std::string("B"));
  EXPECT_TRUE(UTF8String("B") < std::string("C"));

  EXPECT_TRUE(UTF8String("A") < std::string("BA"));
  EXPECT_TRUE(UTF8String("B") < std::string("BA"));
  EXPECT_FALSE(UTF8String("C") < std::string("BA"));
  EXPECT_FALSE(UTF8String("BA") < std::string("A"));
  EXPECT_FALSE(UTF8String("BA") < std::string("B"));
  EXPECT_TRUE(UTF8String("BA") < std::string("C"));

  EXPECT_TRUE(UTF8String("AA") < std::string("B"));
  EXPECT_FALSE(UTF8String("BA") < std::string("B"));
  EXPECT_FALSE(UTF8String("CA") < std::string("B"));
  EXPECT_FALSE(UTF8String("B") < std::string("AA"));
  EXPECT_TRUE(UTF8String("B") < std::string("BA"));
  EXPECT_TRUE(UTF8String("B") < std::string("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_UTF8String, OperatorLessThanOrEqual)
{
  EXPECT_LE(UTF8String("A"), UTF8String("B"));
  EXPECT_LE(UTF8String("A"), UTF8String("AB"));
  EXPECT_LE(UTF8String(""), UTF8String("AB"));
  EXPECT_LE(UTF8String("A"), UTF8String("A"));

  EXPECT_TRUE(UTF8String("A") <= UTF8String("B"));
  EXPECT_TRUE(UTF8String("B") <= UTF8String("B"));
  EXPECT_FALSE(UTF8String("C") <= UTF8String("B"));
  EXPECT_FALSE(UTF8String("B") <= UTF8String("A"));
  EXPECT_TRUE(UTF8String("B") <= UTF8String("B"));
  EXPECT_TRUE(UTF8String("B") <= UTF8String("C"));

  EXPECT_TRUE(UTF8String("A") <= UTF8String("BA"));
  EXPECT_TRUE(UTF8String("B") <= UTF8String("BA"));
  EXPECT_FALSE(UTF8String("C") <= UTF8String("BA"));
  EXPECT_FALSE(UTF8String("BA") <= UTF8String("A"));
  EXPECT_FALSE(UTF8String("BA") <= UTF8String("B"));
  EXPECT_TRUE(UTF8String("BA") <= UTF8String("C"));

  EXPECT_TRUE(UTF8String("AA") <= UTF8String("B"));
  EXPECT_FALSE(UTF8String("BA") <= UTF8String("B"));
  EXPECT_FALSE(UTF8String("CA") <= UTF8String("B"));
  EXPECT_FALSE(UTF8String("B") <= UTF8String("AA"));
  EXPECT_TRUE(UTF8String("B") <= UTF8String("BA"));
  EXPECT_TRUE(UTF8String("B") <= UTF8String("CA"));
}

TEST(TestString_UTF8String, OperatorLessThanOrEqual_CString_LHS)
{
  EXPECT_LE("A", UTF8String("B"));
  EXPECT_LE("A", UTF8String("AB"));
  EXPECT_LE("", UTF8String("AB"));
  EXPECT_LE("A", UTF8String("A"));

  EXPECT_TRUE("A" <= UTF8String("B"));
  EXPECT_TRUE("B" <= UTF8String("B"));
  EXPECT_FALSE("C" <= UTF8String("B"));
  EXPECT_FALSE("B" <= UTF8String("A"));
  EXPECT_TRUE("B" <= UTF8String("B"));
  EXPECT_TRUE("B" <= UTF8String("C"));

  EXPECT_TRUE("A" <= UTF8String("BA"));
  EXPECT_TRUE("B" <= UTF8String("BA"));
  EXPECT_FALSE("C" <= UTF8String("BA"));
  EXPECT_FALSE("BA" <= UTF8String("A"));
  EXPECT_FALSE("BA" <= UTF8String("B"));
  EXPECT_TRUE("BA" <= UTF8String("C"));

  EXPECT_TRUE("AA" <= UTF8String("B"));
  EXPECT_FALSE("BA" <= UTF8String("B"));
  EXPECT_FALSE("CA" <= UTF8String("B"));
  EXPECT_FALSE("B" <= UTF8String("AA"));
  EXPECT_TRUE("B" <= UTF8String("BA"));
  EXPECT_TRUE("B" <= UTF8String("CA"));
}

TEST(TestString_UTF8String, OperatorLessThanOrEqual_CString_RHS)
{
  EXPECT_LE(UTF8String("A"), "B");
  EXPECT_LE(UTF8String("A"), "AB");
  EXPECT_LE(UTF8String(""), "AB");
  EXPECT_LE(UTF8String("A"), "A");

  EXPECT_TRUE(UTF8String("A") <= "B");
  EXPECT_TRUE(UTF8String("B") <= "B");
  EXPECT_FALSE(UTF8String("C") <= "B");
  EXPECT_FALSE(UTF8String("B") <= "A");
  EXPECT_TRUE(UTF8String("B") <= "B");
  EXPECT_TRUE(UTF8String("B") <= "C");

  EXPECT_TRUE(UTF8String("A") <= "BA");
  EXPECT_TRUE(UTF8String("B") <= "BA");
  EXPECT_FALSE(UTF8String("C") <= "BA");
  EXPECT_FALSE(UTF8String("BA") <= "A");
  EXPECT_FALSE(UTF8String("BA") <= "B");
  EXPECT_TRUE(UTF8String("BA") <= "C");

  EXPECT_TRUE(UTF8String("AA") <= "B");
  EXPECT_FALSE(UTF8String("BA") <= "B");
  EXPECT_FALSE(UTF8String("CA") <= "B");
  EXPECT_FALSE(UTF8String("B") <= "AA");
  EXPECT_TRUE(UTF8String("B") <= "BA");
  EXPECT_TRUE(UTF8String("B") <= "CA");
}

TEST(TestString_UTF8String, OperatorLessThanOrEqual_StringViewLite_LHS)
{
  EXPECT_LE(StringViewLite("A"), UTF8String("B"));
  EXPECT_LE(StringViewLite("A"), UTF8String("AB"));
  EXPECT_LE(StringViewLite(""), UTF8String("AB"));
  EXPECT_LE(StringViewLite("A"), UTF8String("A"));

  EXPECT_TRUE(StringViewLite("A") <= UTF8String("B"));
  EXPECT_TRUE(StringViewLite("B") <= UTF8String("B"));
  EXPECT_FALSE(StringViewLite("C") <= UTF8String("B"));
  EXPECT_FALSE(StringViewLite("B") <= UTF8String("A"));
  EXPECT_TRUE(StringViewLite("B") <= UTF8String("B"));
  EXPECT_TRUE(StringViewLite("B") <= UTF8String("C"));

  EXPECT_TRUE(StringViewLite("A") <= UTF8String("BA"));
  EXPECT_TRUE(StringViewLite("B") <= UTF8String("BA"));
  EXPECT_FALSE(StringViewLite("C") <= UTF8String("BA"));
  EXPECT_FALSE(StringViewLite("BA") <= UTF8String("A"));
  EXPECT_FALSE(StringViewLite("BA") <= UTF8String("B"));
  EXPECT_TRUE(StringViewLite("BA") <= UTF8String("C"));

  EXPECT_TRUE(StringViewLite("AA") <= UTF8String("B"));
  EXPECT_FALSE(StringViewLite("BA") <= UTF8String("B"));
  EXPECT_FALSE(StringViewLite("CA") <= UTF8String("B"));
  EXPECT_FALSE(StringViewLite("B") <= UTF8String("AA"));
  EXPECT_TRUE(StringViewLite("B") <= UTF8String("BA"));
  EXPECT_TRUE(StringViewLite("B") <= UTF8String("CA"));
}

TEST(TestString_UTF8String, OperatorLessThanOrEqual_StringViewLite_RHS)
{
  EXPECT_LE(UTF8String("A"), StringViewLite("B"));
  EXPECT_LE(UTF8String("A"), StringViewLite("AB"));
  EXPECT_LE(UTF8String(""), StringViewLite("AB"));
  EXPECT_LE(UTF8String("A"), StringViewLite("A"));

  EXPECT_TRUE(UTF8String("A") <= StringViewLite("B"));
  EXPECT_TRUE(UTF8String("B") <= StringViewLite("B"));
  EXPECT_FALSE(UTF8String("C") <= StringViewLite("B"));
  EXPECT_FALSE(UTF8String("B") <= StringViewLite("A"));
  EXPECT_TRUE(UTF8String("B") <= StringViewLite("B"));
  EXPECT_TRUE(UTF8String("B") <= StringViewLite("C"));

  EXPECT_TRUE(UTF8String("A") <= StringViewLite("BA"));
  EXPECT_TRUE(UTF8String("B") <= StringViewLite("BA"));
  EXPECT_FALSE(UTF8String("C") <= StringViewLite("BA"));
  EXPECT_FALSE(UTF8String("BA") <= StringViewLite("A"));
  EXPECT_FALSE(UTF8String("BA") <= StringViewLite("B"));
  EXPECT_TRUE(UTF8String("BA") <= StringViewLite("C"));

  EXPECT_TRUE(UTF8String("AA") <= StringViewLite("B"));
  EXPECT_FALSE(UTF8String("BA") <= StringViewLite("B"));
  EXPECT_FALSE(UTF8String("CA") <= StringViewLite("B"));
  EXPECT_FALSE(UTF8String("B") <= StringViewLite("AA"));
  EXPECT_TRUE(UTF8String("B") <= StringViewLite("BA"));
  EXPECT_TRUE(UTF8String("B") <= StringViewLite("CA"));
}

TEST(TestString_UTF8String, OperatorLessThanOrEqual_String_LHS)
{
  EXPECT_LE(std::string("A"), UTF8String("B"));
  EXPECT_LE(std::string("A"), UTF8String("AB"));
  EXPECT_LE(std::string(""), UTF8String("AB"));
  EXPECT_LE(std::string("A"), UTF8String("A"));

  EXPECT_TRUE(std::string("A") <= UTF8String("B"));
  EXPECT_TRUE(std::string("B") <= UTF8String("B"));
  EXPECT_FALSE(std::string("C") <= UTF8String("B"));
  EXPECT_FALSE(std::string("B") <= UTF8String("A"));
  EXPECT_TRUE(std::string("B") <= UTF8String("B"));
  EXPECT_TRUE(std::string("B") <= UTF8String("C"));

  EXPECT_TRUE(std::string("A") <= UTF8String("BA"));
  EXPECT_TRUE(std::string("B") <= UTF8String("BA"));
  EXPECT_FALSE(std::string("C") <= UTF8String("BA"));
  EXPECT_FALSE(std::string("BA") <= UTF8String("A"));
  EXPECT_FALSE(std::string("BA") <= UTF8String("B"));
  EXPECT_TRUE(std::string("BA") <= UTF8String("C"));

  EXPECT_TRUE(std::string("AA") <= UTF8String("B"));
  EXPECT_FALSE(std::string("BA") <= UTF8String("B"));
  EXPECT_FALSE(std::string("CA") <= UTF8String("B"));
  EXPECT_FALSE(std::string("B") <= UTF8String("AA"));
  EXPECT_TRUE(std::string("B") <= UTF8String("BA"));
  EXPECT_TRUE(std::string("B") <= UTF8String("CA"));
}

TEST(TestString_UTF8String, OperatorLessThanOrEqual_String_RHS)
{
  EXPECT_LE(UTF8String("A"), std::string("B"));
  EXPECT_LE(UTF8String("A"), std::string("AB"));
  EXPECT_LE(UTF8String(""), std::string("AB"));
  EXPECT_LE(UTF8String("A"), std::string("A"));

  EXPECT_TRUE(UTF8String("A") <= std::string("B"));
  EXPECT_TRUE(UTF8String("B") <= std::string("B"));
  EXPECT_FALSE(UTF8String("C") <= std::string("B"));
  EXPECT_FALSE(UTF8String("B") <= std::string("A"));
  EXPECT_TRUE(UTF8String("B") <= std::string("B"));
  EXPECT_TRUE(UTF8String("B") <= std::string("C"));

  EXPECT_TRUE(UTF8String("A") <= std::string("BA"));
  EXPECT_TRUE(UTF8String("B") <= std::string("BA"));
  EXPECT_FALSE(UTF8String("C") <= std::string("BA"));
  EXPECT_FALSE(UTF8String("BA") <= std::string("A"));
  EXPECT_FALSE(UTF8String("BA") <= std::string("B"));
  EXPECT_TRUE(UTF8String("BA") <= std::string("C"));

  EXPECT_TRUE(UTF8String("AA") <= std::string("B"));
  EXPECT_FALSE(UTF8String("BA") <= std::string("B"));
  EXPECT_FALSE(UTF8String("CA") <= std::string("B"));
  EXPECT_FALSE(UTF8String("B") <= std::string("AA"));
  EXPECT_TRUE(UTF8String("B") <= std::string("BA"));
  EXPECT_TRUE(UTF8String("B") <= std::string("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_UTF8String, OperatorGreaterThan)
{
  EXPECT_GT(UTF8String("B"), UTF8String("A"));
  EXPECT_GT(UTF8String("AB"), UTF8String("A"));
  EXPECT_GT(UTF8String("AB"), UTF8String(""));

  EXPECT_FALSE(UTF8String("A") > UTF8String("B"));
  EXPECT_FALSE(UTF8String("B") > UTF8String("B"));
  EXPECT_TRUE(UTF8String("C") > UTF8String("B"));
  EXPECT_TRUE(UTF8String("B") > UTF8String("A"));
  EXPECT_FALSE(UTF8String("B") > UTF8String("B"));
  EXPECT_FALSE(UTF8String("B") > UTF8String("C"));

  EXPECT_FALSE(UTF8String("A") > UTF8String("BA"));
  EXPECT_FALSE(UTF8String("B") > UTF8String("BA"));
  EXPECT_TRUE(UTF8String("C") > UTF8String("BA"));
  EXPECT_TRUE(UTF8String("BA") > UTF8String("A"));
  EXPECT_TRUE(UTF8String("BA") > UTF8String("B"));
  EXPECT_FALSE(UTF8String("BA") > UTF8String("C"));

  EXPECT_FALSE(UTF8String("AA") > UTF8String("B"));
  EXPECT_TRUE(UTF8String("BA") > UTF8String("B"));
  EXPECT_TRUE(UTF8String("CA") > UTF8String("B"));
  EXPECT_TRUE(UTF8String("B") > UTF8String("AA"));
  EXPECT_FALSE(UTF8String("B") > UTF8String("BA"));
  EXPECT_FALSE(UTF8String("B") > UTF8String("CA"));
}

TEST(TestString_UTF8String, OperatorGreaterThan_CString_LHS)
{
  EXPECT_GT("B", UTF8String("A"));
  EXPECT_GT("AB", UTF8String("A"));
  EXPECT_GT("AB", UTF8String(""));

  EXPECT_FALSE("A" > UTF8String("B"));
  EXPECT_FALSE("B" > UTF8String("B"));
  EXPECT_TRUE("C" > UTF8String("B"));
  EXPECT_TRUE("B" > UTF8String("A"));
  EXPECT_FALSE("B" > UTF8String("B"));
  EXPECT_FALSE("B" > UTF8String("C"));

  EXPECT_FALSE("A" > UTF8String("BA"));
  EXPECT_FALSE("B" > UTF8String("BA"));
  EXPECT_TRUE("C" > UTF8String("BA"));
  EXPECT_TRUE("BA" > UTF8String("A"));
  EXPECT_TRUE("BA" > UTF8String("B"));
  EXPECT_FALSE("BA" > UTF8String("C"));

  EXPECT_FALSE("AA" > UTF8String("B"));
  EXPECT_TRUE("BA" > UTF8String("B"));
  EXPECT_TRUE("CA" > UTF8String("B"));
  EXPECT_TRUE("B" > UTF8String("AA"));
  EXPECT_FALSE("B" > UTF8String("BA"));
  EXPECT_FALSE("B" > UTF8String("CA"));
}

TEST(TestString_UTF8String, OperatorGreaterThan_CString_RHS)
{
  EXPECT_GT(UTF8String("B"), "A");
  EXPECT_GT(UTF8String("AB"), "A");
  EXPECT_GT(UTF8String("AB"), "");

  EXPECT_FALSE(UTF8String("A") > "B");
  EXPECT_FALSE(UTF8String("B") > "B");
  EXPECT_TRUE(UTF8String("C") > "B");
  EXPECT_TRUE(UTF8String("B") > "A");
  EXPECT_FALSE(UTF8String("B") > "B");
  EXPECT_FALSE(UTF8String("B") > "C");

  EXPECT_FALSE(UTF8String("A") > "BA");
  EXPECT_FALSE(UTF8String("B") > "BA");
  EXPECT_TRUE(UTF8String("C") > "BA");
  EXPECT_TRUE(UTF8String("BA") > "A");
  EXPECT_TRUE(UTF8String("BA") > "B");
  EXPECT_FALSE(UTF8String("BA") > "C");

  EXPECT_FALSE(UTF8String("AA") > "B");
  EXPECT_TRUE(UTF8String("BA") > "B");
  EXPECT_TRUE(UTF8String("CA") > "B");
  EXPECT_TRUE(UTF8String("B") > "AA");
  EXPECT_FALSE(UTF8String("B") > "BA");
  EXPECT_FALSE(UTF8String("B") > "CA");
}

TEST(TestString_UTF8String, OperatorGreaterThan_StringViewLite_LHS)
{
  EXPECT_GT(StringViewLite("B"), UTF8String("A"));
  EXPECT_GT(StringViewLite("AB"), UTF8String("A"));
  EXPECT_GT(StringViewLite("AB"), UTF8String(""));

  EXPECT_FALSE(StringViewLite("A") > UTF8String("B"));
  EXPECT_FALSE(StringViewLite("B") > UTF8String("B"));
  EXPECT_TRUE(StringViewLite("C") > UTF8String("B"));
  EXPECT_TRUE(StringViewLite("B") > UTF8String("A"));
  EXPECT_FALSE(StringViewLite("B") > UTF8String("B"));
  EXPECT_FALSE(StringViewLite("B") > UTF8String("C"));

  EXPECT_FALSE(StringViewLite("A") > UTF8String("BA"));
  EXPECT_FALSE(StringViewLite("B") > UTF8String("BA"));
  EXPECT_TRUE(StringViewLite("C") > UTF8String("BA"));
  EXPECT_TRUE(StringViewLite("B") > UTF8String("AA"));
  EXPECT_FALSE(StringViewLite("B") > UTF8String("BA"));
  EXPECT_FALSE(StringViewLite("B") > UTF8String("CA"));

  EXPECT_FALSE(StringViewLite("AA") > UTF8String("B"));
  EXPECT_TRUE(StringViewLite("BA") > UTF8String("B"));
  EXPECT_TRUE(StringViewLite("CA") > UTF8String("B"));
  EXPECT_TRUE(StringViewLite("B") > UTF8String("AA"));
  EXPECT_FALSE(StringViewLite("B") > UTF8String("BA"));
  EXPECT_FALSE(StringViewLite("B") > UTF8String("CA"));
}

TEST(TestString_UTF8String, OperatorGreaterThan_StringViewLite_RHS)
{
  EXPECT_GT(UTF8String("B"), StringViewLite("A"));
  EXPECT_GT(UTF8String("AB"), StringViewLite("A"));
  EXPECT_GT(UTF8String("AB"), StringViewLite(""));

  EXPECT_FALSE(UTF8String("A") > StringViewLite("B"));
  EXPECT_FALSE(UTF8String("B") > StringViewLite("B"));
  EXPECT_TRUE(UTF8String("C") > StringViewLite("B"));
  EXPECT_TRUE(UTF8String("B") > StringViewLite("A"));
  EXPECT_FALSE(UTF8String("B") > StringViewLite("B"));
  EXPECT_FALSE(UTF8String("B") > StringViewLite("C"));

  EXPECT_FALSE(UTF8String("A") > StringViewLite("BA"));
  EXPECT_FALSE(UTF8String("B") > StringViewLite("BA"));
  EXPECT_TRUE(UTF8String("C") > StringViewLite("BA"));
  EXPECT_TRUE(UTF8String("BA") > StringViewLite("A"));
  EXPECT_TRUE(UTF8String("BA") > StringViewLite("B"));
  EXPECT_FALSE(UTF8String("BA") > StringViewLite("C"));

  EXPECT_FALSE(UTF8String("AA") > StringViewLite("B"));
  EXPECT_TRUE(UTF8String("BA") > StringViewLite("B"));
  EXPECT_TRUE(UTF8String("CA") > StringViewLite("B"));
  EXPECT_TRUE(UTF8String("B") > StringViewLite("AA"));
  EXPECT_FALSE(UTF8String("B") > StringViewLite("BA"));
  EXPECT_FALSE(UTF8String("B") > StringViewLite("CA"));
}

TEST(TestString_UTF8String, OperatorGreaterThan_String_LHS)
{
  EXPECT_GT(std::string("B"), UTF8String("A"));
  EXPECT_GT(std::string("AB"), UTF8String("A"));
  EXPECT_GT(std::string("AB"), UTF8String(""));

  EXPECT_FALSE(std::string("A") > UTF8String("B"));
  EXPECT_FALSE(std::string("B") > UTF8String("B"));
  EXPECT_TRUE(std::string("C") > UTF8String("B"));
  EXPECT_TRUE(std::string("B") > UTF8String("A"));
  EXPECT_FALSE(std::string("B") > UTF8String("B"));
  EXPECT_FALSE(std::string("B") > UTF8String("C"));

  EXPECT_FALSE(std::string("A") > UTF8String("BA"));
  EXPECT_FALSE(std::string("B") > UTF8String("BA"));
  EXPECT_TRUE(std::string("C") > UTF8String("BA"));
  EXPECT_TRUE(std::string("BA") > UTF8String("A"));
  EXPECT_TRUE(std::string("BA") > UTF8String("B"));
  EXPECT_FALSE(std::string("BA") > UTF8String("C"));

  EXPECT_FALSE(std::string("AA") > UTF8String("B"));
  EXPECT_TRUE(std::string("BA") > UTF8String("B"));
  EXPECT_TRUE(std::string("CA") > UTF8String("B"));
  EXPECT_TRUE(std::string("B") > UTF8String("AA"));
  EXPECT_FALSE(std::string("B") > UTF8String("BA"));
  EXPECT_FALSE(std::string("B") > UTF8String("CA"));
}

TEST(TestString_UTF8String, OperatorGreaterThan_String_RHS)
{
  EXPECT_GT(UTF8String("B"), std::string("A"));
  EXPECT_GT(UTF8String("AB"), std::string("A"));
  EXPECT_GT(UTF8String("AB"), std::string(""));

  EXPECT_FALSE(UTF8String("A") > std::string("B"));
  EXPECT_FALSE(UTF8String("B") > std::string("B"));
  EXPECT_TRUE(UTF8String("C") > std::string("B"));
  EXPECT_TRUE(UTF8String("B") > std::string("A"));
  EXPECT_FALSE(UTF8String("B") > std::string("B"));
  EXPECT_FALSE(UTF8String("B") > std::string("C"));

  EXPECT_FALSE(UTF8String("A") > std::string("BA"));
  EXPECT_FALSE(UTF8String("B") > std::string("BA"));
  EXPECT_TRUE(UTF8String("C") > std::string("BA"));
  EXPECT_TRUE(UTF8String("BA") > std::string("A"));
  EXPECT_TRUE(UTF8String("BA") > std::string("B"));
  EXPECT_FALSE(UTF8String("BA") > std::string("C"));

  EXPECT_FALSE(UTF8String("AA") > std::string("B"));
  EXPECT_TRUE(UTF8String("BA") > std::string("B"));
  EXPECT_TRUE(UTF8String("CA") > std::string("B"));
  EXPECT_TRUE(UTF8String("B") > std::string("AA"));
  EXPECT_FALSE(UTF8String("B") > std::string("BA"));
  EXPECT_FALSE(UTF8String("B") > std::string("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_UTF8String, OperatorGreaterThanOrEqual)
{
  EXPECT_GE(UTF8String("B"), UTF8String("A"));
  EXPECT_GE(UTF8String("AB"), UTF8String("A"));
  EXPECT_GE(UTF8String("AB"), UTF8String(""));
  EXPECT_GE(UTF8String("A"), UTF8String("A"));

  EXPECT_FALSE(UTF8String("A") >= UTF8String("B"));
  EXPECT_TRUE(UTF8String("B") >= UTF8String("B"));
  EXPECT_TRUE(UTF8String("C") >= UTF8String("B"));
  EXPECT_TRUE(UTF8String("B") >= UTF8String("A"));
  EXPECT_TRUE(UTF8String("B") >= UTF8String("B"));
  EXPECT_FALSE(UTF8String("B") >= UTF8String("C"));

  EXPECT_FALSE(UTF8String("A") >= UTF8String("BA"));
  EXPECT_FALSE(UTF8String("B") >= UTF8String("BA"));
  EXPECT_TRUE(UTF8String("C") >= UTF8String("BA"));
  EXPECT_TRUE(UTF8String("BA") >= UTF8String("A"));
  EXPECT_TRUE(UTF8String("BA") >= UTF8String("B"));
  EXPECT_FALSE(UTF8String("BA") >= UTF8String("C"));

  EXPECT_FALSE(UTF8String("AA") >= UTF8String("B"));
  EXPECT_TRUE(UTF8String("BA") >= UTF8String("B"));
  EXPECT_TRUE(UTF8String("CA") >= UTF8String("B"));
  EXPECT_TRUE(UTF8String("B") >= UTF8String("AA"));
  EXPECT_FALSE(UTF8String("B") >= UTF8String("BA"));
  EXPECT_FALSE(UTF8String("B") >= UTF8String("CA"));
}

TEST(TestString_UTF8String, OperatorGreaterThanOrEqual_CString_LHS)
{
  EXPECT_GE("B", UTF8String("A"));
  EXPECT_GE("AB", UTF8String("A"));
  EXPECT_GE("AB", UTF8String(""));
  EXPECT_GE("A", UTF8String("A"));

  EXPECT_FALSE("A" >= UTF8String("B"));
  EXPECT_TRUE("B" >= UTF8String("B"));
  EXPECT_TRUE("C" >= UTF8String("B"));
  EXPECT_TRUE("B" >= UTF8String("A"));
  EXPECT_TRUE("B" >= UTF8String("B"));
  EXPECT_FALSE("B" >= UTF8String("C"));

  EXPECT_FALSE("A" >= UTF8String("BA"));
  EXPECT_FALSE("B" >= UTF8String("BA"));
  EXPECT_TRUE("C" >= UTF8String("BA"));
  EXPECT_TRUE("BA" >= UTF8String("A"));
  EXPECT_TRUE("BA" >= UTF8String("B"));
  EXPECT_FALSE("BA" >= UTF8String("C"));

  EXPECT_FALSE("AA" >= UTF8String("B"));
  EXPECT_TRUE("BA" >= UTF8String("B"));
  EXPECT_TRUE("CA" >= UTF8String("B"));
  EXPECT_TRUE("B" >= UTF8String("AA"));
  EXPECT_FALSE("B" >= UTF8String("BA"));
  EXPECT_FALSE("B" >= UTF8String("CA"));
}

TEST(TestString_UTF8String, OperatorGreaterThanOrEqual_CString_RHS)
{
  EXPECT_GE(UTF8String("B"), "A");
  EXPECT_GE(UTF8String("AB"), "A");
  EXPECT_GE(UTF8String("AB"), "");
  EXPECT_GE(UTF8String("A"), "A");

  EXPECT_FALSE(UTF8String("A") >= "B");
  EXPECT_TRUE(UTF8String("B") >= "B");
  EXPECT_TRUE(UTF8String("C") >= "B");
  EXPECT_TRUE(UTF8String("B") >= "A");
  EXPECT_TRUE(UTF8String("B") >= "B");
  EXPECT_FALSE(UTF8String("B") >= "C");

  EXPECT_FALSE(UTF8String("A") >= "BA");
  EXPECT_FALSE(UTF8String("B") >= "BA");
  EXPECT_TRUE(UTF8String("C") >= "BA");
  EXPECT_TRUE(UTF8String("BA") >= "A");
  EXPECT_TRUE(UTF8String("BA") >= "B");
  EXPECT_FALSE(UTF8String("BA") >= "C");

  EXPECT_FALSE(UTF8String("AA") >= "B");
  EXPECT_TRUE(UTF8String("BA") >= "B");
  EXPECT_TRUE(UTF8String("CA") >= "B");
  EXPECT_TRUE(UTF8String("B") >= "AA");
  EXPECT_FALSE(UTF8String("B") >= "BA");
  EXPECT_FALSE(UTF8String("B") >= "CA");
}

TEST(TestString_UTF8String, OperatorGreaterThanOrEqual_StringViewLite_LHS)
{
  EXPECT_GE(StringViewLite("B"), UTF8String("A"));
  EXPECT_GE(StringViewLite("AB"), UTF8String("A"));
  EXPECT_GE(StringViewLite("AB"), UTF8String(""));
  EXPECT_GE(StringViewLite("A"), UTF8String("A"));

  EXPECT_FALSE(StringViewLite("A") >= UTF8String("B"));
  EXPECT_TRUE(StringViewLite("B") >= UTF8String("B"));
  EXPECT_TRUE(StringViewLite("C") >= UTF8String("B"));
  EXPECT_TRUE(StringViewLite("B") >= UTF8String("A"));
  EXPECT_TRUE(StringViewLite("B") >= UTF8String("B"));
  EXPECT_FALSE(StringViewLite("B") >= UTF8String("C"));

  EXPECT_FALSE(StringViewLite("A") >= UTF8String("BA"));
  EXPECT_FALSE(StringViewLite("B") >= UTF8String("BA"));
  EXPECT_TRUE(StringViewLite("C") >= UTF8String("BA"));
  EXPECT_TRUE(StringViewLite("BA") >= UTF8String("A"));
  EXPECT_TRUE(StringViewLite("BA") >= UTF8String("B"));
  EXPECT_FALSE(StringViewLite("BA") >= UTF8String("C"));

  EXPECT_FALSE(StringViewLite("AA") >= UTF8String("B"));
  EXPECT_TRUE(StringViewLite("BA") >= UTF8String("B"));
  EXPECT_TRUE(StringViewLite("CA") >= UTF8String("B"));
  EXPECT_TRUE(StringViewLite("B") >= UTF8String("AA"));
  EXPECT_FALSE(StringViewLite("B") >= UTF8String("BA"));
  EXPECT_FALSE(StringViewLite("B") >= UTF8String("CA"));
}

TEST(TestString_UTF8String, OperatorGreaterThanOrEqual_StringViewLite_RHS)
{
  EXPECT_GE(UTF8String("B"), StringViewLite("A"));
  EXPECT_GE(UTF8String("AB"), StringViewLite("A"));
  EXPECT_GE(UTF8String("AB"), StringViewLite(""));
  EXPECT_GE(UTF8String("A"), StringViewLite("A"));

  EXPECT_FALSE(UTF8String("A") >= StringViewLite("B"));
  EXPECT_TRUE(UTF8String("B") >= StringViewLite("B"));
  EXPECT_TRUE(UTF8String("C") >= StringViewLite("B"));
  EXPECT_TRUE(UTF8String("B") >= StringViewLite("A"));
  EXPECT_TRUE(UTF8String("B") >= StringViewLite("B"));
  EXPECT_FALSE(UTF8String("B") >= StringViewLite("C"));

  EXPECT_FALSE(UTF8String("A") >= StringViewLite("BA"));
  EXPECT_FALSE(UTF8String("B") >= StringViewLite("BA"));
  EXPECT_TRUE(UTF8String("C") >= StringViewLite("BA"));
  EXPECT_TRUE(UTF8String("BA") >= StringViewLite("A"));
  EXPECT_TRUE(UTF8String("BA") >= StringViewLite("B"));
  EXPECT_FALSE(UTF8String("BA") >= StringViewLite("C"));

  EXPECT_FALSE(UTF8String("AA") >= StringViewLite("B"));
  EXPECT_TRUE(UTF8String("BA") >= StringViewLite("B"));
  EXPECT_TRUE(UTF8String("CA") >= StringViewLite("B"));
  EXPECT_TRUE(UTF8String("B") >= StringViewLite("AA"));
  EXPECT_FALSE(UTF8String("B") >= StringViewLite("BA"));
  EXPECT_FALSE(UTF8String("B") >= StringViewLite("CA"));
}

TEST(TestString_UTF8String, OperatorGreaterThanOrEqual_String_LHS)
{
  EXPECT_GE(std::string("B"), UTF8String("A"));
  EXPECT_GE(std::string("AB"), UTF8String("A"));
  EXPECT_GE(std::string("AB"), UTF8String(""));
  EXPECT_GE(std::string("A"), UTF8String("A"));

  EXPECT_FALSE(std::string("A") >= UTF8String("B"));
  EXPECT_TRUE(std::string("B") >= UTF8String("B"));
  EXPECT_TRUE(std::string("C") >= UTF8String("B"));
  EXPECT_TRUE(std::string("B") >= UTF8String("A"));
  EXPECT_TRUE(std::string("B") >= UTF8String("B"));
  EXPECT_FALSE(std::string("B") >= UTF8String("C"));

  EXPECT_FALSE(std::string("A") >= UTF8String("BA"));
  EXPECT_FALSE(std::string("B") >= UTF8String("BA"));
  EXPECT_TRUE(std::string("C") >= UTF8String("BA"));
  EXPECT_TRUE(std::string("BA") >= UTF8String("A"));
  EXPECT_TRUE(std::string("BA") >= UTF8String("B"));
  EXPECT_FALSE(std::string("BA") >= UTF8String("C"));

  EXPECT_FALSE(std::string("AA") >= UTF8String("B"));
  EXPECT_TRUE(std::string("BA") >= UTF8String("B"));
  EXPECT_TRUE(std::string("CA") >= UTF8String("B"));
  EXPECT_TRUE(std::string("B") >= UTF8String("AA"));
  EXPECT_FALSE(std::string("B") >= UTF8String("BA"));
  EXPECT_FALSE(std::string("B") >= UTF8String("CA"));
}

TEST(TestString_UTF8String, OperatorGreaterThanOrEqual_String_RHS)
{
  EXPECT_GE(UTF8String("B"), std::string("A"));
  EXPECT_GE(UTF8String("AB"), std::string("A"));
  EXPECT_GE(UTF8String("AB"), std::string(""));
  EXPECT_GE(UTF8String("A"), std::string("A"));

  EXPECT_FALSE(UTF8String("A") >= std::string("B"));
  EXPECT_TRUE(UTF8String("B") >= std::string("B"));
  EXPECT_TRUE(UTF8String("C") >= std::string("B"));
  EXPECT_TRUE(UTF8String("B") >= std::string("A"));
  EXPECT_TRUE(UTF8String("B") >= std::string("B"));
  EXPECT_FALSE(UTF8String("B") >= std::string("C"));

  EXPECT_FALSE(UTF8String("A") >= std::string("BA"));
  EXPECT_FALSE(UTF8String("B") >= std::string("BA"));
  EXPECT_TRUE(UTF8String("C") >= std::string("BA"));
  EXPECT_TRUE(UTF8String("BA") >= std::string("A"));
  EXPECT_TRUE(UTF8String("BA") >= std::string("B"));
  EXPECT_FALSE(UTF8String("BA") >= std::string("C"));

  EXPECT_FALSE(UTF8String("AA") >= std::string("B"));
  EXPECT_TRUE(UTF8String("BA") >= std::string("B"));
  EXPECT_TRUE(UTF8String("CA") >= std::string("B"));
  EXPECT_TRUE(UTF8String("B") >= std::string("AA"));
  EXPECT_FALSE(UTF8String("B") >= std::string("BA"));
  EXPECT_FALSE(UTF8String("B") >= std::string("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator ==
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_UTF8String, OperatorEqual)
{
  EXPECT_TRUE(UTF8String("A") == UTF8String("A"));
  EXPECT_TRUE(UTF8String("") == UTF8String(""));
  EXPECT_FALSE(UTF8String("A") == UTF8String(""));
  EXPECT_FALSE(UTF8String("A") == UTF8String("B"));
  EXPECT_FALSE(UTF8String("A") == UTF8String("a"));

  EXPECT_FALSE(UTF8String("A") == UTF8String("B"));
  EXPECT_TRUE(UTF8String("B") == UTF8String("B"));
  EXPECT_FALSE(UTF8String("C") == UTF8String("B"));
  EXPECT_FALSE(UTF8String("B") == UTF8String("A"));
  EXPECT_TRUE(UTF8String("B") == UTF8String("B"));
  EXPECT_FALSE(UTF8String("B") == UTF8String("C"));

  EXPECT_FALSE(UTF8String("A") == UTF8String("BA"));
  EXPECT_FALSE(UTF8String("B") == UTF8String("BA"));
  EXPECT_FALSE(UTF8String("C") == UTF8String("BA"));
  EXPECT_FALSE(UTF8String("BA") == UTF8String("A"));
  EXPECT_FALSE(UTF8String("BA") == UTF8String("B"));
  EXPECT_FALSE(UTF8String("BA") == UTF8String("C"));

  EXPECT_FALSE(UTF8String("AA") == UTF8String("B"));
  EXPECT_FALSE(UTF8String("BA") == UTF8String("B"));
  EXPECT_FALSE(UTF8String("CA") == UTF8String("B"));
  EXPECT_FALSE(UTF8String("B") == UTF8String("AA"));
  EXPECT_FALSE(UTF8String("B") == UTF8String("BA"));
  EXPECT_FALSE(UTF8String("B") == UTF8String("CA"));
}


TEST(TestString_UTF8String, OperatorEqual_CString_LHS)
{
  EXPECT_TRUE("A" == UTF8String("A"));
  EXPECT_TRUE("" == UTF8String(""));
  EXPECT_FALSE("A" == UTF8String(""));
  EXPECT_FALSE("A" == UTF8String("B"));
  EXPECT_FALSE("A" == UTF8String("a"));

  EXPECT_FALSE("A" == UTF8String("B"));
  EXPECT_TRUE("B" == UTF8String("B"));
  EXPECT_FALSE("C" == UTF8String("B"));
  EXPECT_FALSE("B" == UTF8String("A"));
  EXPECT_TRUE("B" == UTF8String("B"));
  EXPECT_FALSE("B" == UTF8String("C"));

  EXPECT_FALSE("A" == UTF8String("BA"));
  EXPECT_FALSE("B" == UTF8String("BA"));
  EXPECT_FALSE("C" == UTF8String("BA"));
  EXPECT_FALSE("BA" == UTF8String("A"));
  EXPECT_FALSE("BA" == UTF8String("B"));
  EXPECT_FALSE("BA" == UTF8String("C"));

  EXPECT_FALSE("AA" == UTF8String("B"));
  EXPECT_FALSE("BA" == UTF8String("B"));
  EXPECT_FALSE("CA" == UTF8String("B"));
  EXPECT_FALSE("B" == UTF8String("AA"));
  EXPECT_FALSE("B" == UTF8String("BA"));
  EXPECT_FALSE("B" == UTF8String("CA"));
}


TEST(TestString_UTF8String, OperatorEqual_CString_RHS)
{
  EXPECT_TRUE(UTF8String("A") == "A");
  EXPECT_TRUE(UTF8String("") == "");
  EXPECT_FALSE(UTF8String("A") == "");
  EXPECT_FALSE(UTF8String("A") == "B");
  EXPECT_FALSE(UTF8String("A") == "a");

  EXPECT_FALSE(UTF8String("A") == "B");
  EXPECT_TRUE(UTF8String("B") == "B");
  EXPECT_FALSE(UTF8String("C") == "B");
  EXPECT_FALSE(UTF8String("B") == "A");
  EXPECT_TRUE(UTF8String("B") == "B");
  EXPECT_FALSE(UTF8String("B") == "C");

  EXPECT_FALSE(UTF8String("A") == "BA");
  EXPECT_FALSE(UTF8String("B") == "BA");
  EXPECT_FALSE(UTF8String("C") == "BA");
  EXPECT_FALSE(UTF8String("BA") == "A");
  EXPECT_FALSE(UTF8String("BA") == "B");
  EXPECT_FALSE(UTF8String("BA") == "C");

  EXPECT_FALSE(UTF8String("AA") == "B");
  EXPECT_FALSE(UTF8String("BA") == "B");
  EXPECT_FALSE(UTF8String("CA") == "B");
  EXPECT_FALSE(UTF8String("B") == "AA");
  EXPECT_FALSE(UTF8String("B") == "BA");
  EXPECT_FALSE(UTF8String("B") == "CA");
}


TEST(TestString_UTF8String, OperatorEqual_StringViewLite_LHS)
{
  EXPECT_TRUE(StringViewLite("A") == UTF8String("A"));
  EXPECT_TRUE(StringViewLite("") == UTF8String(""));
  EXPECT_FALSE(StringViewLite("A") == UTF8String(""));
  EXPECT_FALSE(StringViewLite("A") == UTF8String("B"));
  EXPECT_FALSE(StringViewLite("A") == UTF8String("a"));

  EXPECT_FALSE(StringViewLite("A") == UTF8String("B"));
  EXPECT_TRUE(StringViewLite("B") == UTF8String("B"));
  EXPECT_FALSE(StringViewLite("C") == UTF8String("B"));
  EXPECT_FALSE(StringViewLite("B") == UTF8String("A"));
  EXPECT_TRUE(StringViewLite("B") == UTF8String("B"));
  EXPECT_FALSE(StringViewLite("B") == UTF8String("C"));

  EXPECT_FALSE(StringViewLite("A") == UTF8String("BA"));
  EXPECT_FALSE(StringViewLite("B") == UTF8String("BA"));
  EXPECT_FALSE(StringViewLite("C") == UTF8String("BA"));
  EXPECT_FALSE(StringViewLite("BA") == UTF8String("A"));
  EXPECT_FALSE(StringViewLite("BA") == UTF8String("B"));
  EXPECT_FALSE(StringViewLite("BA") == UTF8String("C"));

  EXPECT_FALSE(StringViewLite("AA") == UTF8String("B"));
  EXPECT_FALSE(StringViewLite("BA") == UTF8String("B"));
  EXPECT_FALSE(StringViewLite("CA") == UTF8String("B"));
  EXPECT_FALSE(StringViewLite("B") == UTF8String("AA"));
  EXPECT_FALSE(StringViewLite("B") == UTF8String("BA"));
  EXPECT_FALSE(StringViewLite("B") == UTF8String("CA"));
}


TEST(TestString_UTF8String, OperatorEqual_StringViewLite_RHS)
{
  EXPECT_TRUE(UTF8String("A") == StringViewLite("A"));
  EXPECT_TRUE(UTF8String("") == StringViewLite(""));
  EXPECT_FALSE(UTF8String("A") == StringViewLite(""));
  EXPECT_FALSE(UTF8String("A") == StringViewLite("B"));
  EXPECT_FALSE(UTF8String("A") == StringViewLite("a"));


  EXPECT_FALSE(UTF8String("A") == StringViewLite("B"));
  EXPECT_TRUE(UTF8String("B") == StringViewLite("B"));
  EXPECT_FALSE(UTF8String("C") == StringViewLite("B"));
  EXPECT_FALSE(UTF8String("B") == StringViewLite("A"));
  EXPECT_TRUE(UTF8String("B") == StringViewLite("B"));
  EXPECT_FALSE(UTF8String("B") == StringViewLite("C"));

  EXPECT_FALSE(UTF8String("A") == StringViewLite("BA"));
  EXPECT_FALSE(UTF8String("B") == StringViewLite("BA"));
  EXPECT_FALSE(UTF8String("C") == StringViewLite("BA"));
  EXPECT_FALSE(UTF8String("BA") == StringViewLite("A"));
  EXPECT_FALSE(UTF8String("BA") == StringViewLite("B"));
  EXPECT_FALSE(UTF8String("BA") == StringViewLite("C"));

  EXPECT_FALSE(UTF8String("AA") == StringViewLite("B"));
  EXPECT_FALSE(UTF8String("BA") == StringViewLite("B"));
  EXPECT_FALSE(UTF8String("CA") == StringViewLite("B"));
  EXPECT_FALSE(UTF8String("B") == StringViewLite("AA"));
  EXPECT_FALSE(UTF8String("B") == StringViewLite("BA"));
  EXPECT_FALSE(UTF8String("B") == StringViewLite("CA"));
}


TEST(TestString_UTF8String, OperatorEqual_String_LHS)
{
  EXPECT_TRUE(std::string("A") == UTF8String("A"));
  EXPECT_TRUE(std::string("") == UTF8String(""));
  EXPECT_FALSE(std::string("A") == UTF8String(""));
  EXPECT_FALSE(std::string("A") == UTF8String("B"));
  EXPECT_FALSE(std::string("A") == UTF8String("a"));

  EXPECT_FALSE(std::string("A") == UTF8String("B"));
  EXPECT_TRUE(std::string("B") == UTF8String("B"));
  EXPECT_FALSE(std::string("C") == UTF8String("B"));
  EXPECT_FALSE(std::string("B") == UTF8String("A"));
  EXPECT_TRUE(std::string("B") == UTF8String("B"));
  EXPECT_FALSE(std::string("B") == UTF8String("C"));

  EXPECT_FALSE(std::string("A") == UTF8String("BA"));
  EXPECT_FALSE(std::string("B") == UTF8String("BA"));
  EXPECT_FALSE(std::string("C") == UTF8String("BA"));
  EXPECT_FALSE(std::string("BA") == UTF8String("A"));
  EXPECT_FALSE(std::string("BA") == UTF8String("B"));
  EXPECT_FALSE(std::string("BA") == UTF8String("C"));

  EXPECT_FALSE(std::string("AA") == UTF8String("B"));
  EXPECT_FALSE(std::string("BA") == UTF8String("B"));
  EXPECT_FALSE(std::string("CA") == UTF8String("B"));
  EXPECT_FALSE(std::string("B") == UTF8String("AA"));
  EXPECT_FALSE(std::string("B") == UTF8String("BA"));
  EXPECT_FALSE(std::string("B") == UTF8String("CA"));
}


TEST(TestString_UTF8String, OperatorEqual_String_RHS)
{
  EXPECT_TRUE(UTF8String("A") == std::string("A"));
  EXPECT_TRUE(UTF8String("") == std::string(""));
  EXPECT_FALSE(UTF8String("A") == std::string(""));
  EXPECT_FALSE(UTF8String("A") == std::string("B"));
  EXPECT_FALSE(UTF8String("A") == std::string("a"));

  EXPECT_FALSE(UTF8String("A") == std::string("B"));
  EXPECT_TRUE(UTF8String("B") == std::string("B"));
  EXPECT_FALSE(UTF8String("C") == std::string("B"));
  EXPECT_FALSE(UTF8String("B") == std::string("A"));
  EXPECT_TRUE(UTF8String("B") == std::string("B"));
  EXPECT_FALSE(UTF8String("B") == std::string("C"));

  EXPECT_FALSE(UTF8String("A") == std::string("BA"));
  EXPECT_FALSE(UTF8String("B") == std::string("BA"));
  EXPECT_FALSE(UTF8String("C") == std::string("BA"));
  EXPECT_FALSE(UTF8String("BA") == std::string("A"));
  EXPECT_FALSE(UTF8String("BA") == std::string("B"));
  EXPECT_FALSE(UTF8String("BA") == std::string("C"));

  EXPECT_FALSE(UTF8String("AA") == std::string("B"));
  EXPECT_FALSE(UTF8String("BA") == std::string("B"));
  EXPECT_FALSE(UTF8String("CA") == std::string("B"));
  EXPECT_FALSE(UTF8String("B") == std::string("AA"));
  EXPECT_FALSE(UTF8String("B") == std::string("BA"));
  EXPECT_FALSE(UTF8String("B") == std::string("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator !=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_UTF8String, OperatorNotEqual)
{
  EXPECT_FALSE(UTF8String("A") != UTF8String("A"));
  EXPECT_FALSE(UTF8String("") != UTF8String(""));
  EXPECT_TRUE(UTF8String("A") != UTF8String(""));
  EXPECT_TRUE(UTF8String("A") != UTF8String("B"));
  EXPECT_TRUE(UTF8String("A") != UTF8String("a"));

  EXPECT_TRUE(UTF8String("A") != UTF8String("B"));
  EXPECT_FALSE(UTF8String("B") != UTF8String("B"));
  EXPECT_TRUE(UTF8String("C") != UTF8String("B"));
  EXPECT_TRUE(UTF8String("B") != UTF8String("A"));
  EXPECT_FALSE(UTF8String("B") != UTF8String("B"));
  EXPECT_TRUE(UTF8String("B") != UTF8String("C"));

  EXPECT_TRUE(UTF8String("A") != UTF8String("BA"));
  EXPECT_TRUE(UTF8String("B") != UTF8String("BA"));
  EXPECT_TRUE(UTF8String("C") != UTF8String("BA"));
  EXPECT_TRUE(UTF8String("BA") != UTF8String("A"));
  EXPECT_TRUE(UTF8String("BA") != UTF8String("B"));
  EXPECT_TRUE(UTF8String("BA") != UTF8String("C"));

  EXPECT_TRUE(UTF8String("AA") != UTF8String("B"));
  EXPECT_TRUE(UTF8String("BA") != UTF8String("B"));
  EXPECT_TRUE(UTF8String("CA") != UTF8String("B"));
  EXPECT_TRUE(UTF8String("B") != UTF8String("AA"));
  EXPECT_TRUE(UTF8String("B") != UTF8String("BA"));
  EXPECT_TRUE(UTF8String("B") != UTF8String("CA"));
}


TEST(TestString_UTF8String, OperatorNotEqual_CString_LHS)
{
  EXPECT_FALSE("A" != UTF8String("A"));
  EXPECT_FALSE("" != UTF8String(""));
  EXPECT_TRUE("A" != UTF8String(""));
  EXPECT_TRUE("A" != UTF8String("B"));
  EXPECT_TRUE("A" != UTF8String("a"));

  EXPECT_TRUE("A" != UTF8String("B"));
  EXPECT_FALSE("B" != UTF8String("B"));
  EXPECT_TRUE("C" != UTF8String("B"));
  EXPECT_TRUE("B" != UTF8String("A"));
  EXPECT_FALSE("B" != UTF8String("B"));
  EXPECT_TRUE("B" != UTF8String("C"));

  EXPECT_TRUE("A" != UTF8String("BA"));
  EXPECT_TRUE("B" != UTF8String("BA"));
  EXPECT_TRUE("C" != UTF8String("BA"));
  EXPECT_TRUE("BA" != UTF8String("A"));
  EXPECT_TRUE("BA" != UTF8String("B"));
  EXPECT_TRUE("BA" != UTF8String("C"));

  EXPECT_TRUE("AA" != UTF8String("B"));
  EXPECT_TRUE("BA" != UTF8String("B"));
  EXPECT_TRUE("CA" != UTF8String("B"));
  EXPECT_TRUE("B" != UTF8String("AA"));
  EXPECT_TRUE("B" != UTF8String("BA"));
  EXPECT_TRUE("B" != UTF8String("CA"));
}


TEST(TestString_UTF8String, OperatorNotEqual_CString_RHS)
{
  EXPECT_FALSE(UTF8String("A") != "A");
  EXPECT_FALSE(UTF8String("") != "");
  EXPECT_TRUE(UTF8String("A") != "");
  EXPECT_TRUE(UTF8String("A") != "B");
  EXPECT_TRUE(UTF8String("A") != "a");

  EXPECT_TRUE(UTF8String("A") != "B");
  EXPECT_FALSE(UTF8String("B") != "B");
  EXPECT_TRUE(UTF8String("C") != "B");
  EXPECT_TRUE(UTF8String("B") != "A");
  EXPECT_FALSE(UTF8String("B") != "B");
  EXPECT_TRUE(UTF8String("B") != "C");

  EXPECT_TRUE(UTF8String("A") != "BA");
  EXPECT_TRUE(UTF8String("B") != "BA");
  EXPECT_TRUE(UTF8String("C") != "BA");
  EXPECT_TRUE(UTF8String("BA") != "A");
  EXPECT_TRUE(UTF8String("BA") != "B");
  EXPECT_TRUE(UTF8String("BA") != "C");

  EXPECT_TRUE(UTF8String("AA") != "B");
  EXPECT_TRUE(UTF8String("BA") != "B");
  EXPECT_TRUE(UTF8String("CA") != "B");
  EXPECT_TRUE(UTF8String("B") != "AA");
  EXPECT_TRUE(UTF8String("B") != "BA");
  EXPECT_TRUE(UTF8String("B") != "CA");
}


TEST(TestString_UTF8String, OperatorNotEqual_StringViewLite_LHS)
{
  EXPECT_FALSE(StringViewLite("A") != UTF8String("A"));
  EXPECT_FALSE(StringViewLite("") != UTF8String(""));
  EXPECT_TRUE(StringViewLite("A") != UTF8String(""));
  EXPECT_TRUE(StringViewLite("A") != UTF8String("B"));
  EXPECT_TRUE(StringViewLite("A") != UTF8String("a"));

  EXPECT_TRUE(StringViewLite("A") != UTF8String("B"));
  EXPECT_FALSE(StringViewLite("B") != UTF8String("B"));
  EXPECT_TRUE(StringViewLite("C") != UTF8String("B"));
  EXPECT_TRUE(StringViewLite("B") != UTF8String("A"));
  EXPECT_FALSE(StringViewLite("B") != UTF8String("B"));
  EXPECT_TRUE(StringViewLite("B") != UTF8String("C"));

  EXPECT_TRUE(StringViewLite("A") != UTF8String("BA"));
  EXPECT_TRUE(StringViewLite("B") != UTF8String("BA"));
  EXPECT_TRUE(StringViewLite("C") != UTF8String("BA"));
  EXPECT_TRUE(StringViewLite("BA") != UTF8String("A"));
  EXPECT_TRUE(StringViewLite("BA") != UTF8String("B"));
  EXPECT_TRUE(StringViewLite("BA") != UTF8String("C"));

  EXPECT_TRUE(StringViewLite("AA") != UTF8String("B"));
  EXPECT_TRUE(StringViewLite("BA") != UTF8String("B"));
  EXPECT_TRUE(StringViewLite("CA") != UTF8String("B"));
  EXPECT_TRUE(StringViewLite("B") != UTF8String("AA"));
  EXPECT_TRUE(StringViewLite("B") != UTF8String("BA"));
  EXPECT_TRUE(StringViewLite("B") != UTF8String("CA"));
}


TEST(TestString_UTF8String, OperatorNotEqual_StringViewLite_RHS)
{
  EXPECT_FALSE(UTF8String("A") != StringViewLite("A"));
  EXPECT_FALSE(UTF8String("") != StringViewLite(""));
  EXPECT_TRUE(UTF8String("A") != StringViewLite(""));
  EXPECT_TRUE(UTF8String("A") != StringViewLite("B"));
  EXPECT_TRUE(UTF8String("A") != StringViewLite("a"));

  EXPECT_TRUE(UTF8String("A") != StringViewLite("B"));
  EXPECT_FALSE(UTF8String("B") != StringViewLite("B"));
  EXPECT_TRUE(UTF8String("C") != StringViewLite("B"));
  EXPECT_TRUE(UTF8String("B") != StringViewLite("A"));
  EXPECT_FALSE(UTF8String("B") != StringViewLite("B"));
  EXPECT_TRUE(UTF8String("B") != StringViewLite("C"));

  EXPECT_TRUE(UTF8String("A") != StringViewLite("BA"));
  EXPECT_TRUE(UTF8String("B") != StringViewLite("BA"));
  EXPECT_TRUE(UTF8String("C") != StringViewLite("BA"));
  EXPECT_TRUE(UTF8String("BA") != StringViewLite("A"));
  EXPECT_TRUE(UTF8String("BA") != StringViewLite("B"));
  EXPECT_TRUE(UTF8String("BA") != StringViewLite("C"));

  EXPECT_TRUE(UTF8String("AA") != StringViewLite("B"));
  EXPECT_TRUE(UTF8String("BA") != StringViewLite("B"));
  EXPECT_TRUE(UTF8String("CA") != StringViewLite("B"));
  EXPECT_TRUE(UTF8String("B") != StringViewLite("AA"));
  EXPECT_TRUE(UTF8String("B") != StringViewLite("BA"));
  EXPECT_TRUE(UTF8String("B") != StringViewLite("CA"));
}


TEST(TestString_UTF8String, OperatorNotEqual_String_LHS)
{
  EXPECT_FALSE(std::string("A") != UTF8String("A"));
  EXPECT_FALSE(std::string("") != UTF8String(""));
  EXPECT_TRUE(std::string("A") != UTF8String(""));
  EXPECT_TRUE(std::string("A") != UTF8String("B"));
  EXPECT_TRUE(std::string("A") != UTF8String("a"));

  EXPECT_TRUE(std::string("A") != UTF8String("B"));
  EXPECT_FALSE(std::string("B") != UTF8String("B"));
  EXPECT_TRUE(std::string("C") != UTF8String("B"));
  EXPECT_TRUE(std::string("B") != UTF8String("A"));
  EXPECT_FALSE(std::string("B") != UTF8String("B"));
  EXPECT_TRUE(std::string("B") != UTF8String("C"));

  EXPECT_TRUE(std::string("A") != UTF8String("BA"));
  EXPECT_TRUE(std::string("B") != UTF8String("BA"));
  EXPECT_TRUE(std::string("C") != UTF8String("BA"));
  EXPECT_TRUE(std::string("BA") != UTF8String("A"));
  EXPECT_TRUE(std::string("BA") != UTF8String("B"));
  EXPECT_TRUE(std::string("BA") != UTF8String("C"));

  EXPECT_TRUE(std::string("AA") != UTF8String("B"));
  EXPECT_TRUE(std::string("BA") != UTF8String("B"));
  EXPECT_TRUE(std::string("CA") != UTF8String("B"));
  EXPECT_TRUE(std::string("B") != UTF8String("AA"));
  EXPECT_TRUE(std::string("B") != UTF8String("BA"));
  EXPECT_TRUE(std::string("B") != UTF8String("CA"));
}


TEST(TestString_UTF8String, OperatorNotEqual_String_RHS)
{
  EXPECT_FALSE(UTF8String("A") != std::string("A"));
  EXPECT_FALSE(UTF8String("") != std::string(""));
  EXPECT_TRUE(UTF8String("A") != std::string(""));
  EXPECT_TRUE(UTF8String("A") != std::string("B"));
  EXPECT_TRUE(UTF8String("A") != std::string("a"));

  EXPECT_TRUE(UTF8String("A") != std::string("B"));
  EXPECT_FALSE(UTF8String("B") != std::string("B"));
  EXPECT_TRUE(UTF8String("C") != std::string("B"));
  EXPECT_TRUE(UTF8String("B") != std::string("A"));
  EXPECT_FALSE(UTF8String("B") != std::string("B"));
  EXPECT_TRUE(UTF8String("B") != std::string("C"));

  EXPECT_TRUE(UTF8String("A") != std::string("BA"));
  EXPECT_TRUE(UTF8String("B") != std::string("BA"));
  EXPECT_TRUE(UTF8String("C") != std::string("BA"));
  EXPECT_TRUE(UTF8String("BA") != std::string("A"));
  EXPECT_TRUE(UTF8String("BA") != std::string("B"));
  EXPECT_TRUE(UTF8String("BA") != std::string("C"));

  EXPECT_TRUE(UTF8String("AA") != std::string("B"));
  EXPECT_TRUE(UTF8String("BA") != std::string("B"));
  EXPECT_TRUE(UTF8String("CA") != std::string("B"));
  EXPECT_TRUE(UTF8String("B") != std::string("AA"));
  EXPECT_TRUE(UTF8String("B") != std::string("BA"));
  EXPECT_TRUE(UTF8String("B") != std::string("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Compare Operators vs null
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_UTF8String, OperatorLessThan_Null)
{
  const char* pszNull = nullptr;
  constexpr StringViewLite strViewNull(nullptr);

  EXPECT_FALSE(UTF8String("A") < pszNull);
  EXPECT_FALSE(UTF8String("A") < strViewNull);
  EXPECT_TRUE(pszNull < UTF8String("A"));
  EXPECT_TRUE(strViewNull < UTF8String("A"));

  // Compare against empty string
  EXPECT_FALSE(UTF8String("") < pszNull);
  EXPECT_FALSE(UTF8String("") < strViewNull);
  EXPECT_FALSE(pszNull < UTF8String(""));
  EXPECT_FALSE(strViewNull < UTF8String(""));

  // Compare against null
  EXPECT_FALSE(pszNull < strViewNull);
  EXPECT_FALSE(strViewNull < pszNull);
  EXPECT_FALSE(strViewNull < strViewNull);
}


TEST(TestString_UTF8String, OperatorLessThanOrEqual_Null)
{
  const char* pszNull = nullptr;
  constexpr StringViewLite strViewNull(nullptr);

  EXPECT_FALSE(UTF8String("A") <= pszNull);
  EXPECT_FALSE(UTF8String("A") <= strViewNull);
  EXPECT_TRUE(pszNull <= UTF8String("A"));
  EXPECT_TRUE(strViewNull <= UTF8String("A"));

  // Compare against empty string
  EXPECT_TRUE(UTF8String("") <= pszNull);
  EXPECT_TRUE(UTF8String("") <= strViewNull);
  EXPECT_TRUE(pszNull <= UTF8String(""));
  EXPECT_TRUE(strViewNull <= UTF8String(""));

  // Compare against null
  EXPECT_TRUE(pszNull <= strViewNull);
  EXPECT_TRUE(strViewNull <= pszNull);
  EXPECT_TRUE(strViewNull <= strViewNull);
}


TEST(TestString_UTF8String, OperatorGreaterThan_Null)
{
  const char* pszNull = nullptr;
  constexpr StringViewLite strViewNull(nullptr);

  EXPECT_TRUE(UTF8String("A") > pszNull);
  EXPECT_TRUE(UTF8String("A") > strViewNull);
  EXPECT_FALSE(pszNull > UTF8String("A"));
  EXPECT_FALSE(strViewNull > UTF8String("A"));

  // Compare against empty string
  EXPECT_FALSE(UTF8String("") > pszNull);
  EXPECT_FALSE(UTF8String("") > strViewNull);
  EXPECT_FALSE(pszNull > UTF8String(""));
  EXPECT_FALSE(strViewNull > UTF8String(""));

  // Compare against null
  EXPECT_FALSE(pszNull > strViewNull);
  EXPECT_FALSE(strViewNull > pszNull);
  EXPECT_FALSE(strViewNull > strViewNull);
}


TEST(TestString_UTF8String, OperatorGreaterThanOrEqual_Null)
{
  const char* pszNull = nullptr;
  constexpr StringViewLite strViewNull(nullptr);
  // UTF8String str8ViewNull(nullptr);

  EXPECT_TRUE(UTF8String("A") >= pszNull);
  EXPECT_TRUE(UTF8String("A") >= strViewNull);
  EXPECT_FALSE(pszNull >= UTF8String("A"));
  EXPECT_FALSE(strViewNull >= UTF8String("A"));

  // Compare against empty string
  EXPECT_TRUE(UTF8String("") >= pszNull);
  EXPECT_TRUE(UTF8String("") >= strViewNull);
  EXPECT_TRUE(pszNull >= UTF8String(""));
  EXPECT_TRUE(strViewNull >= UTF8String(""));

  // Compare against null
  EXPECT_TRUE(pszNull >= strViewNull);
  EXPECT_TRUE(strViewNull >= pszNull);
  EXPECT_TRUE(strViewNull >= strViewNull);
}


TEST(TestString_UTF8String, OperatorEqual_Null)
{
  const char* pszNull = nullptr;
  constexpr StringViewLite strViewNull(nullptr);

  EXPECT_FALSE(UTF8String("A") == pszNull);
  EXPECT_FALSE(UTF8String("A") == strViewNull);
  EXPECT_FALSE(pszNull == UTF8String("A"));
  EXPECT_FALSE(strViewNull == UTF8String("A"));

  // Compare against empty string
  EXPECT_TRUE(pszNull == UTF8String(""));
  EXPECT_TRUE(strViewNull == UTF8String(""));
  EXPECT_TRUE(UTF8String("") == pszNull);
  EXPECT_TRUE(UTF8String("") == strViewNull);

  // Compare against null
  EXPECT_TRUE(pszNull == strViewNull);
  EXPECT_TRUE(strViewNull == pszNull);
  EXPECT_TRUE(strViewNull == strViewNull);
}


TEST(TestString_UTF8String, OperatorNotEqual_Null)
{
  const char* pszNull = nullptr;
  constexpr StringViewLite strViewNull(nullptr);

  EXPECT_TRUE(UTF8String("A") != pszNull);
  EXPECT_TRUE(UTF8String("A") != strViewNull);
  EXPECT_TRUE(pszNull != UTF8String("A"));
  EXPECT_TRUE(strViewNull != UTF8String("A"));

  // Compare against empty string
  EXPECT_FALSE(pszNull != UTF8String(""));
  EXPECT_FALSE(strViewNull != UTF8String(""));
  EXPECT_FALSE(UTF8String("") != pszNull);
  EXPECT_FALSE(UTF8String("") != strViewNull);

  // Compare against null
  EXPECT_FALSE(pszNull != strViewNull);
  EXPECT_FALSE(strViewNull != pszNull);
  EXPECT_FALSE(strViewNull != strViewNull);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator+
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestString_UTF8String, Op_Plus_UTF8String_CString)
{
  //---------------01234567890
  UTF8String str("hello");

  EXPECT_EQ(StringViewLite("hello world"), str + " world");
  EXPECT_EQ(StringViewLite("hello there!"), str + " there!");
  EXPECT_EQ(StringViewLite("hello!!"), str + "!!");
  EXPECT_EQ(StringViewLite("hello and welcome"), str + " and" + " welcome");
}


TEST(TestString_UTF8String, Op_Plus_UTF8String_StringViewLite)
{
  //---------------01234567890
  UTF8String str("hello");

  EXPECT_EQ(StringViewLite("hello world"), str + StringViewLite(" world"));
  EXPECT_EQ(StringViewLite("hello there!"), str + StringViewLite(" there!"));
  EXPECT_EQ(StringViewLite("hello!!"), str + StringViewLite("!!"));
  EXPECT_EQ(StringViewLite("hello and welcome"), str + StringViewLite(" and") + StringViewLite(" welcome"));
}


TEST(TestString_UTF8String, Op_Plus_UTF8String_UTF8String)
{
  //---------------01234567890
  UTF8String str("hello");

  EXPECT_EQ(StringViewLite("hello world"), str + UTF8String(" world"));
  EXPECT_EQ(StringViewLite("hello there!"), str + UTF8String(" there!"));
  EXPECT_EQ(StringViewLite("hello!!"), str + UTF8String("!!"));
  EXPECT_EQ(StringViewLite("hello and welcome"), str + UTF8String(" and") + UTF8String(" welcome"));
}


TEST(TestString_UTF8String, Op_Plus_CString_UTF8String)
{
  //---------------01234567890

  EXPECT_EQ(StringViewLite("hello world"), "hello" + UTF8String(" world"));
  EXPECT_EQ(StringViewLite("hello there!"), "hello" + UTF8String(" there!"));
  EXPECT_EQ(StringViewLite("hello!!"), "hello" + UTF8String("!!"));
}


TEST(TestString_UTF8String, Op_Plus_StringViewLite_UTF8String)
{
  //---------------01234567890
  UTF8String str("hello");

  EXPECT_EQ(StringViewLite("hello world"), StringViewLite("hello") + UTF8String(" world"));
  EXPECT_EQ(StringViewLite("hello there!"), StringViewLite("hello") + UTF8String(" there!"));
  EXPECT_EQ(StringViewLite("hello!!"), StringViewLite("hello") + UTF8String("!!"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator+=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

// TEST(TestString_UTF8String, Op_PlusEquals_InvalidChar)
//{
//  //---------------01234567890
//  UTF8String str("hello world");
//
//  EXPECT_THROW(str += static_cast<char>(0xFF), std::invalid_argument);
//}


// TEST(TestString_UTF8String, Op_PlusEquals_Char)
//{
//  //---------------01234567890
//  UTF8String str("hello world");
//  str += '!';
//
//  EXPECT_EQ(StringViewLite("hello world!"), str);
//
//  str += '!';
//  EXPECT_EQ(StringViewLite("hello world!!"), str);
//}


TEST(TestString_UTF8String, Op_PlusEquals_CString)
{
  //---------------01234567890
  UTF8String str("hello");

  str += " world";

  EXPECT_EQ(StringViewLite("hello world"), str);
}


TEST(TestString_UTF8String, Op_PlusEquals_StringViewLite)
{
  //---------------01234567890
  UTF8String str("hello");

  str += StringViewLite(" world");

  EXPECT_EQ(StringViewLite("hello world"), str);
}


TEST(TestString_UTF8String, Op_PlusEquals_UTF8String)
{
  //---------------01234567890
  UTF8String str("hello");

  str += UTF8String(" world");

  EXPECT_EQ(StringViewLite("hello world"), str);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Move
// ---------------------------------------------------------------------------------------------------------------------------------------------------


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
