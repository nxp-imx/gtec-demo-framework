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
#include <FslBase/IO/Path.hpp>
#include <FslBase/Log/IO/LogPath.hpp>
#include <FslBase/Log/String/LogStringViewLite.hpp>
#include <FslBase/Log/String/LogUTF8String.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestIO_Path = TestFixtureFslBase;
}


TEST(TestIO_Path, Construct_Default)
{
  IO::Path path;

  EXPECT_TRUE(path.IsEmpty());
}


TEST(TestIO_Path, Construct_CString)
{
  IO::Path path("hello");

  EXPECT_FALSE(path.IsEmpty());
  EXPECT_EQ("hello", path);
}


TEST(TestIO_Path, Construct_CString_nullptr)
{
  const char* const pszNull = nullptr;
  IO::Path path(pszNull);

  EXPECT_TRUE(path.IsEmpty());
  EXPECT_EQ("", path);
}

TEST(TestIO_Path, Construct_CString_BackSlash)
{
  IO::Path path("hello\\");

  EXPECT_FALSE(path.IsEmpty());
  EXPECT_EQ("hello/", path);
}


TEST(TestIO_Path, Construct_StringViewLite)
{
  IO::Path path(StringViewLite("hello"));

  EXPECT_FALSE(path.IsEmpty());
  EXPECT_EQ("hello", path);
}


TEST(TestIO_Path, Construct_PathView)
{
  IO::Path path(IO::PathView("hello"));

  EXPECT_FALSE(path.IsEmpty());
  EXPECT_EQ("hello", path);
}


TEST(TestIO_Path, Construct_PathView_Empty)
{
  IO::PathView qw;
  IO::Path path(qw);

  EXPECT_TRUE(path.IsEmpty());
  EXPECT_EQ("", path);
}


TEST(TestIO_Path, Construct_StringViewLite_Empty)
{
  StringViewLite qw;
  IO::Path path(qw);

  EXPECT_TRUE(path.IsEmpty());
  EXPECT_EQ("", path);
}

TEST(TestIO_Path, Construct_StringViewLite_BackSlash)
{
  IO::Path path(StringViewLite("hello\\"));

  EXPECT_FALSE(path.IsEmpty());
  EXPECT_EQ("hello/", path);
}


TEST(TestIO_Path, Construct_String)
{
  IO::Path path(std::string("hello"));

  EXPECT_FALSE(path.IsEmpty());
  EXPECT_EQ("hello", path);
}

TEST(TestIO_Path, Construct_String_BackSlash)
{
  IO::Path path(std::string("hello\\"));

  EXPECT_FALSE(path.IsEmpty());
  EXPECT_EQ("hello/", path);
}


TEST(TestIO_Path, Construct_UTF8String)
{
  IO::Path path(UTF8String("hello"));

  EXPECT_FALSE(path.IsEmpty());
  EXPECT_EQ("hello", path);
}

TEST(TestIO_Path, Construct_UTF8String_BackSlash)
{
  IO::Path path(UTF8String("hello\\"));

  EXPECT_FALSE(path.IsEmpty());
  EXPECT_EQ("hello/", path);
}

TEST(TestIO_Path, Append_InvalidChar)
{
  //---------------01234567890
  IO::Path str("hello world");

  EXPECT_THROW(str.Append(1, static_cast<char>(0xFF)), std::invalid_argument);
}


TEST(TestIO_Path, Append_Char)
{
  //---------------01234567890
  IO::Path str("hello world");
  str.Append(1, '!');

  EXPECT_EQ(StringViewLite("hello world!"), str);

  str.Append(2, '!');
  EXPECT_EQ(StringViewLite("hello world!!!"), str);

  str.Append(1, '/');
  EXPECT_EQ(StringViewLite("hello world!!!/"), str);
}


TEST(TestIO_Path, Append_Char_BackSlash)
{
  //---------------01234567890
  IO::Path str("hello world");

  str.Append(1, '\\');
  EXPECT_EQ(StringViewLite("hello world/"), str);
}


TEST(TestIO_Path, Append_CString)
{
  //---------------01234567890
  IO::Path str("hello");

  str.Append(" world");

  EXPECT_EQ(StringViewLite("hello world"), str);

  str.Append("/now");

  EXPECT_EQ(StringViewLite("hello world/now"), str);
}


TEST(TestIO_Path, Append_CString_nullptr)
{
  //---------------01234567890
  IO::Path str("hello");

  const char* const pszNull = nullptr;

  str.Append(pszNull);

  EXPECT_EQ(StringViewLite("hello"), str);
}

TEST(TestIO_Path, Append_CString_BackSlash)
{
  //---------------01234567890
  IO::Path str("hello");

  str.Append("\\world/. There\\it/is!");

  EXPECT_EQ(StringViewLite("hello/world/. There/it/is!"), str);
}


TEST(TestIO_Path, Append_StringViewLite)
{
  //---------------01234567890
  IO::Path str("hello");

  str.Append(StringViewLite(" world"));

  EXPECT_EQ(StringViewLite("hello world"), str);
}

TEST(TestIO_Path, Append_StringViewLite_Empty)
{
  //---------------01234567890
  IO::Path str("hello");

  str.Append(StringViewLite());

  EXPECT_EQ(StringViewLite("hello"), str);
}


TEST(TestIO_Path, Append_StringViewLite_Backslash)
{
  //---------------01234567890
  IO::Path str("hello");

  str.Append(StringViewLite(" world\\"));

  EXPECT_EQ(StringViewLite("hello world/"), str);
}

TEST(TestIO_Path, Append_PathView)
{
  //---------------01234567890
  IO::Path str("hello");

  str.Append(IO::PathView(" world"));

  EXPECT_EQ("hello world", str);
}

TEST(TestIO_Path, Append_PathView_Empty)
{
  //---------------01234567890
  IO::Path str("hello");

  str.Append(IO::PathView());

  EXPECT_EQ("hello", str);
}


TEST(TestIO_Path, Append_UTF8String)
{
  //---------------01234567890
  IO::Path str("hello");

  str.Append(UTF8String(" world"));

  EXPECT_EQ(StringViewLite("hello world"), str);
}


TEST(TestIO_Path, Append_UTF8String_BackSlash)
{
  //---------------01234567890
  IO::Path str("hello");

  str.Append(UTF8String(" world\\"));

  EXPECT_EQ(StringViewLite("hello world/"), str);
}


TEST(TestIO_Path, Prepend_Char_InvalidChar)
{
  //---------------01234567890
  IO::Path str("hello world");

  EXPECT_THROW(str.Prepend(1, static_cast<char>(0xFF)), std::invalid_argument);
}


TEST(TestIO_Path, Prepend_Char)
{
  //---------------01234567890
  IO::Path str("hello world");
  str.Prepend(1, '!');

  EXPECT_EQ(StringViewLite("!hello world"), str);

  str.Prepend(2, '!');
  EXPECT_EQ(StringViewLite("!!!hello world"), str);
}

TEST(TestIO_Path, Prepend_Char_BackSlash)
{
  //---------------01234567890
  IO::Path str("hello world");
  str.Prepend(1, '\\');

  EXPECT_EQ(StringViewLite("/hello world"), str);
}


TEST(TestIO_Path, Prepend_CString)
{
  //---------------01234567890
  IO::Path str("world");

  str.Prepend("hello ");

  EXPECT_EQ(StringViewLite("hello world"), str);
}


TEST(TestIO_Path, Prepend_CString_nullptr)
{
  //---------------01234567890
  IO::Path str("world");

  const char* const pszNull = nullptr;

  str.Prepend(pszNull);

  EXPECT_EQ(StringViewLite("world"), str);
}


TEST(TestIO_Path, Prepend_CString_BackSlash)
{
  //---------------01234567890
  IO::Path str("world");

  str.Prepend("hello\\");

  EXPECT_EQ(StringViewLite("hello/world"), str);
}


TEST(TestIO_Path, Prepend_StringViewLite)
{
  //---------------01234567890
  IO::Path str("world");

  str.Prepend(StringViewLite("hello "));

  EXPECT_EQ(StringViewLite("hello world"), str);
}


TEST(TestIO_Path, Prepend_StringViewLite_Empty)
{
  //---------------01234567890
  IO::Path str("world");

  str.Prepend(StringViewLite());

  EXPECT_EQ(StringViewLite("world"), str);
}


TEST(TestIO_Path, Prepend_StringViewLite_BackSlash)
{
  //---------------01234567890
  IO::Path str("world");

  str.Prepend(StringViewLite("hello\\"));

  EXPECT_EQ(StringViewLite("hello/world"), str);
}


TEST(TestIO_Path, Prepend_PathView)
{
  //---------------01234567890
  IO::Path str("world");

  str.Prepend(IO::PathView("hello "));

  EXPECT_EQ("hello world", str);
}


TEST(TestIO_Path, Prepend_PathView_Empty)
{
  //---------------01234567890
  IO::Path str("world");

  str.Prepend(IO::PathView());

  EXPECT_EQ("world", str);
}


TEST(TestIO_Path, Prepend_UTF8String)
{
  //---------------01234567890
  IO::Path str("world");

  str.Prepend(UTF8String("hello "));

  EXPECT_EQ(StringViewLite("hello world"), str);
}


TEST(TestIO_Path, Prepend_UTF8String_BackSlash)
{
  //---------------01234567890
  IO::Path str("world");

  str.Prepend(UTF8String("hello\\"));

  EXPECT_EQ(StringViewLite("hello/world"), str);
}


TEST(TestIO_Path, Prepend_Path)
{
  //---------------01234567890
  IO::Path str("world");

  str.Prepend(IO::Path("hello "));

  EXPECT_EQ(StringViewLite("hello world"), str);
}


TEST(TestIO_Path, Prepend_Path_BackSlash)
{
  //---------------01234567890
  IO::Path str("world");

  str.Prepend(IO::Path("hello\\"));

  EXPECT_EQ(StringViewLite("hello/world"), str);
}


TEST(TestIO_Path, Contains_Char)
{
  IO::Path path("hello");

  EXPECT_TRUE(path.Contains('h'));
  EXPECT_TRUE(path.Contains('e'));
  EXPECT_TRUE(path.Contains('l'));
  EXPECT_TRUE(path.Contains('o'));
  EXPECT_FALSE(path.Contains('H'));
  EXPECT_FALSE(path.Contains('q'));
}


TEST(TestIO_Path, Contains_String)
{
  IO::Path path("hello");

  EXPECT_TRUE(path.Contains(""));
  EXPECT_TRUE(path.Contains("h"));
  EXPECT_TRUE(path.Contains("ell"));
  EXPECT_TRUE(path.Contains("lo"));
  EXPECT_TRUE(path.Contains("hello"));

  EXPECT_FALSE(path.Contains("hello."));
  EXPECT_FALSE(path.Contains("qel"));
  EXPECT_FALSE(path.Contains("los"));
}


TEST(TestIO_Path, Contains_UTF8String)
{
  IO::Path path(UTF8String("hello"));

  EXPECT_TRUE(path.Contains(UTF8String("")));
  EXPECT_TRUE(path.Contains(UTF8String("h")));
  EXPECT_TRUE(path.Contains(UTF8String("ell")));
  EXPECT_TRUE(path.Contains(UTF8String("lo")));
  EXPECT_TRUE(path.Contains(UTF8String("hello")));

  EXPECT_FALSE(path.Contains(UTF8String("hello.")));
  EXPECT_FALSE(path.Contains(UTF8String("qel")));
  EXPECT_FALSE(path.Contains(UTF8String("los")));
}


TEST(TestIO_Path, Contains_Path)
{
  IO::Path path(IO::Path("hello"));

  EXPECT_TRUE(path.Contains(IO::Path("")));
  EXPECT_TRUE(path.Contains(IO::Path("h")));
  EXPECT_TRUE(path.Contains(IO::Path("ell")));
  EXPECT_TRUE(path.Contains(IO::Path("lo")));
  EXPECT_TRUE(path.Contains(IO::Path("hello")));

  EXPECT_FALSE(path.Contains(IO::Path("hello.")));
  EXPECT_FALSE(path.Contains(IO::Path("qel")));
  EXPECT_FALSE(path.Contains(IO::Path("los")));
}


TEST(TestIO_Path, StartsWith_Char)
{
  IO::Path path("hello");

  EXPECT_TRUE(path.StartsWith('h'));
  EXPECT_FALSE(path.StartsWith('e'));
  EXPECT_FALSE(path.StartsWith('l'));
  EXPECT_FALSE(path.StartsWith('o'));
  EXPECT_FALSE(path.StartsWith('H'));
  EXPECT_FALSE(path.StartsWith('q'));
}


TEST(TestIO_Path, StartsWith_CString)
{
  IO::Path path("hello");

  EXPECT_TRUE(path.StartsWith(""));
  EXPECT_TRUE(path.StartsWith("h"));
  EXPECT_TRUE(path.StartsWith("he"));
  EXPECT_TRUE(path.StartsWith("hel"));
  EXPECT_TRUE(path.StartsWith("hell"));
  EXPECT_TRUE(path.StartsWith("hello"));
  EXPECT_FALSE(path.StartsWith("e"));
  EXPECT_FALSE(path.StartsWith("el"));
  EXPECT_FALSE(path.StartsWith("ello"));
  EXPECT_FALSE(path.StartsWith("H"));
}


TEST(TestIO_Path, StartsWith_StringViewLite)
{
  IO::Path path(UTF8String("hello"));

  EXPECT_TRUE(path.StartsWith(StringViewLite("")));
  EXPECT_TRUE(path.StartsWith(StringViewLite("h")));
  EXPECT_TRUE(path.StartsWith(StringViewLite("he")));
  EXPECT_TRUE(path.StartsWith(StringViewLite("hel")));
  EXPECT_TRUE(path.StartsWith(StringViewLite("hell")));
  EXPECT_TRUE(path.StartsWith(StringViewLite("hello")));
  EXPECT_FALSE(path.StartsWith(StringViewLite("e")));
  EXPECT_FALSE(path.StartsWith(StringViewLite("el")));
  EXPECT_FALSE(path.StartsWith(StringViewLite("ello")));
  EXPECT_FALSE(path.StartsWith(StringViewLite("H")));
}


TEST(TestIO_Path, StartsWith_PathView)
{
  IO::Path path(UTF8String("hello"));

  EXPECT_TRUE(path.StartsWith(IO::PathView("")));
  EXPECT_TRUE(path.StartsWith(IO::PathView("h")));
  EXPECT_TRUE(path.StartsWith(IO::PathView("he")));
  EXPECT_TRUE(path.StartsWith(IO::PathView("hel")));
  EXPECT_TRUE(path.StartsWith(IO::PathView("hell")));
  EXPECT_TRUE(path.StartsWith(IO::PathView("hello")));
  EXPECT_FALSE(path.StartsWith(IO::PathView("e")));
  EXPECT_FALSE(path.StartsWith(IO::PathView("el")));
  EXPECT_FALSE(path.StartsWith(IO::PathView("ello")));
  EXPECT_FALSE(path.StartsWith(IO::PathView("H")));
}


TEST(TestIO_Path, StartsWith_UTF8String)
{
  IO::Path path(UTF8String("hello"));

  EXPECT_TRUE(path.StartsWith(UTF8String("")));
  EXPECT_TRUE(path.StartsWith(UTF8String("h")));
  EXPECT_TRUE(path.StartsWith(UTF8String("he")));
  EXPECT_TRUE(path.StartsWith(UTF8String("hel")));
  EXPECT_TRUE(path.StartsWith(UTF8String("hell")));
  EXPECT_TRUE(path.StartsWith(UTF8String("hello")));
  EXPECT_FALSE(path.StartsWith(UTF8String("e")));
  EXPECT_FALSE(path.StartsWith(UTF8String("el")));
  EXPECT_FALSE(path.StartsWith(UTF8String("ello")));
  EXPECT_FALSE(path.StartsWith(UTF8String("H")));
}


TEST(TestIO_Path, StartsWith_Path)
{
  IO::Path path(IO::Path("hello"));

  EXPECT_TRUE(path.StartsWith(IO::Path("")));
  EXPECT_TRUE(path.StartsWith(IO::Path("h")));
  EXPECT_TRUE(path.StartsWith(IO::Path("he")));
  EXPECT_TRUE(path.StartsWith(IO::Path("hel")));
  EXPECT_TRUE(path.StartsWith(IO::Path("hell")));
  EXPECT_TRUE(path.StartsWith(IO::Path("hello")));
  EXPECT_FALSE(path.StartsWith(IO::Path("e")));
  EXPECT_FALSE(path.StartsWith(IO::Path("el")));
  EXPECT_FALSE(path.StartsWith(IO::Path("ello")));
  EXPECT_FALSE(path.StartsWith(IO::Path("H")));
}


TEST(TestIO_Path, EndsWith_Char)
{
  IO::Path path("hello");

  EXPECT_TRUE(path.EndsWith('o'));
  EXPECT_FALSE(path.EndsWith('O'));
  EXPECT_FALSE(path.EndsWith('l'));
  EXPECT_FALSE(path.EndsWith('e'));
  EXPECT_FALSE(path.EndsWith('H'));
  EXPECT_FALSE(path.EndsWith('h'));
}


TEST(TestIO_Path, EndsWith_CString)
{
  IO::Path path("hello");

  EXPECT_TRUE(path.EndsWith(""));
  EXPECT_TRUE(path.EndsWith("o"));
  EXPECT_TRUE(path.EndsWith("lo"));
  EXPECT_TRUE(path.EndsWith("llo"));
  EXPECT_TRUE(path.EndsWith("ello"));
  EXPECT_TRUE(path.EndsWith("hello"));
  EXPECT_FALSE(path.EndsWith("e"));
  EXPECT_FALSE(path.EndsWith("el"));
  EXPECT_FALSE(path.EndsWith("ell"));
  EXPECT_FALSE(path.EndsWith("O"));
}


TEST(TestIO_Path, EndsWith_StringViewLite)
{
  IO::Path path(UTF8String("hello"));

  EXPECT_TRUE(path.EndsWith(StringViewLite("")));
  EXPECT_TRUE(path.EndsWith(StringViewLite("o")));
  EXPECT_TRUE(path.EndsWith(StringViewLite("lo")));
  EXPECT_TRUE(path.EndsWith(StringViewLite("llo")));
  EXPECT_TRUE(path.EndsWith(StringViewLite("ello")));
  EXPECT_TRUE(path.EndsWith(StringViewLite("hello")));
  EXPECT_FALSE(path.EndsWith(StringViewLite("e")));
  EXPECT_FALSE(path.EndsWith(StringViewLite("el")));
  EXPECT_FALSE(path.EndsWith(StringViewLite("ell")));
  EXPECT_FALSE(path.EndsWith(StringViewLite("O")));
}


TEST(TestIO_Path, EndsWith_PathView)
{
  IO::Path path(UTF8String("hello"));

  EXPECT_TRUE(path.EndsWith(IO::PathView("")));
  EXPECT_TRUE(path.EndsWith(IO::PathView("o")));
  EXPECT_TRUE(path.EndsWith(IO::PathView("lo")));
  EXPECT_TRUE(path.EndsWith(IO::PathView("llo")));
  EXPECT_TRUE(path.EndsWith(IO::PathView("ello")));
  EXPECT_TRUE(path.EndsWith(IO::PathView("hello")));
  EXPECT_FALSE(path.EndsWith(IO::PathView("e")));
  EXPECT_FALSE(path.EndsWith(IO::PathView("el")));
  EXPECT_FALSE(path.EndsWith(IO::PathView("ell")));
  EXPECT_FALSE(path.EndsWith(IO::PathView("O")));
}

TEST(TestIO_Path, EndsWith_UTF8String)
{
  IO::Path path(UTF8String("hello"));

  EXPECT_TRUE(path.EndsWith(UTF8String("")));
  EXPECT_TRUE(path.EndsWith(UTF8String("o")));
  EXPECT_TRUE(path.EndsWith(UTF8String("lo")));
  EXPECT_TRUE(path.EndsWith(UTF8String("llo")));
  EXPECT_TRUE(path.EndsWith(UTF8String("ello")));
  EXPECT_TRUE(path.EndsWith(UTF8String("hello")));
  EXPECT_FALSE(path.EndsWith(UTF8String("e")));
  EXPECT_FALSE(path.EndsWith(UTF8String("el")));
  EXPECT_FALSE(path.EndsWith(UTF8String("ell")));
  EXPECT_FALSE(path.EndsWith(UTF8String("O")));
}


TEST(TestIO_Path, EndsWith_Path)
{
  IO::Path path(IO::Path("hello"));

  EXPECT_TRUE(path.EndsWith(IO::Path("")));
  EXPECT_TRUE(path.EndsWith(IO::Path("o")));
  EXPECT_TRUE(path.EndsWith(IO::Path("lo")));
  EXPECT_TRUE(path.EndsWith(IO::Path("llo")));
  EXPECT_TRUE(path.EndsWith(IO::Path("ello")));
  EXPECT_TRUE(path.EndsWith(IO::Path("hello")));
  EXPECT_FALSE(path.EndsWith(IO::Path("e")));
  EXPECT_FALSE(path.EndsWith(IO::Path("el")));
  EXPECT_FALSE(path.EndsWith(IO::Path("ell")));
  EXPECT_FALSE(path.EndsWith(IO::Path("O")));
}


TEST(TestIO_Path, IndexOf_Char)
{
  //-------------01234
  IO::Path path("hello");

  EXPECT_EQ(0, path.IndexOf('h'));
  EXPECT_EQ(1, path.IndexOf('e'));
  EXPECT_EQ(2, path.IndexOf('l'));
  EXPECT_EQ(4, path.IndexOf('o'));

  EXPECT_EQ(1, path.IndexOf('e', 1));
  EXPECT_EQ(2, path.IndexOf('l', 2));
  EXPECT_EQ(3, path.IndexOf('l', 3));

  EXPECT_GE(0, path.IndexOf('h', 1));
  EXPECT_GE(0, path.IndexOf('o', 5));
}


TEST(TestIO_Path, ToUTF8String)
{
  std::string str("hello");
  IO::Path path(str);

  const auto& res = path.ToUTF8String();

  EXPECT_EQ(str, res);
}


TEST(TestIO_Path, ToAsciiString)
{
  std::string str("hello");
  IO::Path path(str);

  auto res = path.ToAsciiString();

  EXPECT_EQ(str, res);
}


TEST(TestIO_Path, IsPathRooted)
{
  EXPECT_FALSE(IO::Path::IsPathRooted("hello"));
  // IMPROVEMENT: need some rooted paths here
}


TEST(TestIO_Path, Combine_CString_CString)
{
  EXPECT_EQ("world", IO::Path::Combine("", "world"));
  EXPECT_EQ("/world", IO::Path::Combine("/", "world"));
  EXPECT_EQ("hello/world", IO::Path::Combine("hello", "world"));
  // Rooted path2 -> path2
  EXPECT_EQ("/world", IO::Path::Combine("hello", "/world"));
  // empty path 2 -> path1
  EXPECT_EQ("hello", IO::Path::Combine("hello", ""));
}


TEST(TestIO_Path, Combine_CString_StringViewLite)
{
  EXPECT_EQ("world", IO::Path::Combine("", StringViewLite("world")));
  EXPECT_EQ("/world", IO::Path::Combine("/", StringViewLite("world")));
  EXPECT_EQ("hello/world", IO::Path::Combine("hello", StringViewLite("world")));
  // Rooted path2 -> path2
  EXPECT_EQ("/world", IO::Path::Combine("hello", StringViewLite("/world")));
  // empty path 2 -> path1
  EXPECT_EQ("hello", IO::Path::Combine("hello", StringViewLite("")));
}


TEST(TestIO_Path, Combine_CString_PathView)
{
  EXPECT_EQ("world", IO::Path::Combine("", IO::PathView("world")));
  EXPECT_EQ("/world", IO::Path::Combine("/", IO::PathView("world")));
  EXPECT_EQ("hello/world", IO::Path::Combine("hello", IO::PathView("world")));
  // Rooted path2 -> path2
  EXPECT_EQ("/world", IO::Path::Combine("hello", IO::PathView("/world")));
  // empty path 2 -> path1
  EXPECT_EQ("hello", IO::Path::Combine("hello", IO::PathView("")));
}


TEST(TestIO_Path, Combine_CString_Path)
{
  EXPECT_EQ("world", IO::Path::Combine("", IO::Path("world")));
  EXPECT_EQ("/world", IO::Path::Combine("/", IO::Path("world")));
  EXPECT_EQ("hello/world", IO::Path::Combine("hello", IO::Path("world")));
  // Rooted path2 -> path2
  EXPECT_EQ("/world", IO::Path::Combine("hello", IO::Path("/world")));
  // empty path 2 -> path1
  EXPECT_EQ("hello", IO::Path::Combine("hello", IO::Path("")));
}


TEST(TestIO_Path, Combine_StringViewLite_CString)
{
  EXPECT_EQ("world", IO::Path::Combine(StringViewLite(""), "world"));
  EXPECT_EQ("/world", IO::Path::Combine(StringViewLite("/"), "world"));
  EXPECT_EQ("hello/world", IO::Path::Combine(StringViewLite("hello"), "world"));
  // Rooted path2 -> path2
  EXPECT_EQ("/world", IO::Path::Combine(StringViewLite("hello"), "/world"));
  // empty path 2 -> path1
  EXPECT_EQ("hello", IO::Path::Combine(StringViewLite("hello"), ""));
}


TEST(TestIO_Path, Combine_StringViewLite_StringViewLite)
{
  EXPECT_EQ("world", IO::Path::Combine(StringViewLite(""), StringViewLite("world")));
  EXPECT_EQ("/world", IO::Path::Combine(StringViewLite("/"), StringViewLite("world")));
  EXPECT_EQ("hello/world", IO::Path::Combine(StringViewLite("hello"), StringViewLite("world")));
  // Rooted path2 -> path2
  EXPECT_EQ("/world", IO::Path::Combine(StringViewLite("hello"), StringViewLite("/world")));
  // empty path 2 -> path1
  EXPECT_EQ("hello", IO::Path::Combine(StringViewLite("hello"), StringViewLite("")));
}


TEST(TestIO_Path, Combine_StringViewLite_PathView)
{
  EXPECT_EQ("world", IO::Path::Combine(StringViewLite(""), IO::PathView("world")));
  EXPECT_EQ("/world", IO::Path::Combine(StringViewLite("/"), IO::PathView("world")));
  EXPECT_EQ("hello/world", IO::Path::Combine(StringViewLite("hello"), IO::PathView("world")));
  // Rooted path2 -> path2
  EXPECT_EQ("/world", IO::Path::Combine(StringViewLite("hello"), IO::PathView("/world")));
  // empty path 2 -> path1
  EXPECT_EQ("hello", IO::Path::Combine(StringViewLite("hello"), IO::PathView("")));
}


TEST(TestIO_Path, Combine_StringViewLite_Path)
{
  EXPECT_EQ("world", IO::Path::Combine(StringViewLite(""), IO::Path("world")));
  EXPECT_EQ("/world", IO::Path::Combine(StringViewLite("/"), IO::Path("world")));
  EXPECT_EQ("hello/world", IO::Path::Combine(StringViewLite("hello"), IO::Path("world")));
  // Rooted path2 -> path2
  EXPECT_EQ("/world", IO::Path::Combine(StringViewLite("hello"), IO::Path("/world")));
  // empty path 2 -> path1
  EXPECT_EQ("hello", IO::Path::Combine(StringViewLite("hello"), IO::Path("")));
}

TEST(TestIO_Path, Combine_PathView_CString)
{
  EXPECT_EQ("world", IO::Path::Combine(IO::PathView(""), "world"));
  EXPECT_EQ("/world", IO::Path::Combine(IO::PathView("/"), "world"));
  EXPECT_EQ("hello/world", IO::Path::Combine(IO::PathView("hello"), "world"));
  // Rooted path2 -> path2
  EXPECT_EQ("/world", IO::Path::Combine(IO::PathView("hello"), "/world"));
  // empty path 2 -> path1
  EXPECT_EQ("hello", IO::Path::Combine(IO::PathView("hello"), ""));
}


TEST(TestIO_Path, Combine_PathView_StringViewLite)
{
  EXPECT_EQ("world", IO::Path::Combine(IO::PathView(""), StringViewLite("world")));
  EXPECT_EQ("/world", IO::Path::Combine(IO::PathView("/"), StringViewLite("world")));
  EXPECT_EQ("hello/world", IO::Path::Combine(IO::PathView("hello"), StringViewLite("world")));
  // Rooted path2 -> path2
  EXPECT_EQ("/world", IO::Path::Combine(IO::PathView("hello"), StringViewLite("/world")));
  // empty path 2 -> path1
  EXPECT_EQ("hello", IO::Path::Combine(IO::PathView("hello"), StringViewLite("")));
}


TEST(TestIO_Path, Combine_PathView_PathView)
{
  EXPECT_EQ("world", IO::Path::Combine(IO::PathView(""), IO::PathView("world")));
  EXPECT_EQ("/world", IO::Path::Combine(IO::PathView("/"), IO::PathView("world")));
  EXPECT_EQ("hello/world", IO::Path::Combine(IO::PathView("hello"), IO::PathView("world")));
  // Rooted path2 -> path2
  EXPECT_EQ("/world", IO::Path::Combine(IO::PathView("hello"), IO::PathView("/world")));
  // empty path 2 -> path1
  EXPECT_EQ("hello", IO::Path::Combine(IO::PathView("hello"), IO::PathView("")));
}


TEST(TestIO_Path, Combine_PathView_Path)
{
  EXPECT_EQ("world", IO::Path::Combine(IO::PathView(""), IO::Path("world")));
  EXPECT_EQ("/world", IO::Path::Combine(IO::PathView("/"), IO::Path("world")));
  EXPECT_EQ("hello/world", IO::Path::Combine(IO::PathView("hello"), IO::Path("world")));
  // Rooted path2 -> path2
  EXPECT_EQ("/world", IO::Path::Combine(IO::PathView("hello"), IO::Path("/world")));
  // empty path 2 -> path1
  EXPECT_EQ("hello", IO::Path::Combine(IO::PathView("hello"), IO::Path("")));
}

TEST(TestIO_Path, Combine_Path_CString)
{
  EXPECT_EQ("world", IO::Path::Combine(IO::Path(""), "world"));
  EXPECT_EQ("/world", IO::Path::Combine(IO::Path("/"), "world"));
  EXPECT_EQ("hello/world", IO::Path::Combine(IO::Path("hello"), "world"));
  // Rooted path2 -> path2
  EXPECT_EQ("/world", IO::Path::Combine(IO::Path("hello"), "/world"));
  // empty path 2 -> path1
  EXPECT_EQ("hello", IO::Path::Combine(IO::Path("hello"), ""));
}


TEST(TestIO_Path, Combine_Path_StringViewLite)
{
  EXPECT_EQ("world", IO::Path::Combine(IO::Path(""), StringViewLite("world")));
  EXPECT_EQ("/world", IO::Path::Combine(IO::Path("/"), StringViewLite("world")));
  EXPECT_EQ("hello/world", IO::Path::Combine(IO::Path("hello"), StringViewLite("world")));
  // Rooted path2 -> path2
  EXPECT_EQ("/world", IO::Path::Combine(IO::Path("hello"), StringViewLite("/world")));
  // empty path 2 -> path1
  EXPECT_EQ("hello", IO::Path::Combine(IO::Path("hello"), StringViewLite("")));
}


TEST(TestIO_Path, Combine_Path_PathView)
{
  EXPECT_EQ("world", IO::Path::Combine(IO::Path(""), IO::PathView("world")));
  EXPECT_EQ("/world", IO::Path::Combine(IO::Path("/"), IO::PathView("world")));
  EXPECT_EQ("hello/world", IO::Path::Combine(IO::Path("hello"), IO::PathView("world")));
  // Rooted path2 -> path2
  EXPECT_EQ("/world", IO::Path::Combine(IO::Path("hello"), IO::PathView("/world")));
  // empty path 2 -> path1
  EXPECT_EQ("hello", IO::Path::Combine(IO::Path("hello"), IO::PathView("")));
}


TEST(TestIO_Path, Combine_Path_Path)
{
  EXPECT_EQ("world", IO::Path::Combine(IO::Path(""), IO::Path("world")));
  EXPECT_EQ("/world", IO::Path::Combine(IO::Path("/"), IO::Path("world")));
  EXPECT_EQ("hello/world", IO::Path::Combine(IO::Path("hello"), IO::Path("world")));
  // Rooted path2 -> path2
  EXPECT_EQ("/world", IO::Path::Combine(IO::Path("hello"), IO::Path("/world")));
  // empty path 2 -> path1
  EXPECT_EQ("hello", IO::Path::Combine(IO::Path("hello"), IO::Path("")));
}


TEST(TestIO_Path, GetDirectoryName_CString)
{
  EXPECT_EQ(IO::Path(""), IO::Path::GetDirectoryName("world"));
  // This could technically give incorrect results when working on unix root dir level, investigate.
  EXPECT_EQ(IO::Path(""), IO::Path::GetDirectoryName("/world"));
  EXPECT_EQ(IO::Path("hello"), IO::Path::GetDirectoryName("hello/world"));
  EXPECT_EQ(IO::Path("q/hello"), IO::Path::GetDirectoryName("q/hello/world"));
  EXPECT_EQ(IO::Path("/q/hello"), IO::Path::GetDirectoryName("/q/hello/world"));
}

TEST(TestIO_Path, GetDirectoryName_PathView)
{
  EXPECT_EQ(IO::Path(""), IO::Path::GetDirectoryName(IO::PathView("world")));
  // This could technically give incorrect results when working on unix root dir level, investigate.
  EXPECT_EQ(IO::Path(""), IO::Path::GetDirectoryName(IO::PathView("/world")));
  EXPECT_EQ(IO::Path("hello"), IO::Path::GetDirectoryName(IO::PathView("hello/world")));
  EXPECT_EQ(IO::Path("q/hello"), IO::Path::GetDirectoryName(IO::PathView("q/hello/world")));
  EXPECT_EQ(IO::Path("/q/hello"), IO::Path::GetDirectoryName(IO::PathView("/q/hello/world")));
}

TEST(TestIO_Path, GetDirectoryName_Path)
{
  EXPECT_EQ(IO::Path(""), IO::Path::GetDirectoryName(IO::Path("world")));
  // This could technically give incorrect results when working on unix root dir level, investigate.
  EXPECT_EQ(IO::Path(""), IO::Path::GetDirectoryName(IO::Path("/world")));
  EXPECT_EQ(IO::Path("hello"), IO::Path::GetDirectoryName(IO::Path("hello/world")));
  EXPECT_EQ(IO::Path("q/hello"), IO::Path::GetDirectoryName(IO::Path("q/hello/world")));
  EXPECT_EQ(IO::Path("/q/hello"), IO::Path::GetDirectoryName(IO::Path("/q/hello/world")));
}


TEST(TestIO_Path, GetFileName_CString)
{
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileName("world"));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileName("/world"));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileName("hello/world"));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileName("q/hello/world"));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileName("/q/hello/world"));
}

TEST(TestIO_Path, GetFileName_PathView)
{
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileName(IO::PathView("world")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileName(IO::PathView("/world")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileName(IO::PathView("hello/world")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileName(IO::PathView("q/hello/world")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileName(IO::PathView("/q/hello/world")));
}

TEST(TestIO_Path, GetFileName_Path)
{
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileName(IO::Path("world")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileName(IO::Path("/world")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileName(IO::Path("hello/world")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileName(IO::Path("q/hello/world")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileName(IO::Path("/q/hello/world")));
}


TEST(TestIO_Path, GetFileNameWithoutExtension_CString)
{
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension("world"));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension("/world"));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension("hello/world"));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension("q/hello/world"));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension("/q/hello/world"));

  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension("world.txt"));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension("/world.txt"));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension("hello/world.txt"));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension("q/hello/world.txt"));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension("/q/hello/world.txt"));

  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension("world."));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension("/world."));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension("hello/world."));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension("q/hello/world."));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension("/q/hello/world."));

  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension("world.t"));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension("/world.t"));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension("hello/world.t"));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension("q/hello/world.t"));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension("/q/hello/world.t"));
}


TEST(TestIO_Path, GetFileNameWithoutExtension_PathView)
{
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::PathView("world")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::PathView("/world")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::PathView("hello/world")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::PathView("q/hello/world")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::PathView("/q/hello/world")));

  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::PathView("world.txt")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::PathView("/world.txt")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::PathView("hello/world.txt")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::PathView("q/hello/world.txt")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::PathView("/q/hello/world.txt")));

  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::PathView("world.")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::PathView("/world.")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::PathView("hello/world.")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::PathView("q/hello/world.")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::PathView("/q/hello/world.")));

  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::PathView("world.t")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::PathView("/world.t")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::PathView("hello/world.t")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::PathView("q/hello/world.t")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::PathView("/q/hello/world.t")));
}


TEST(TestIO_Path, GetFileNameWithoutExtension_Path)
{
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::Path("world")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::Path("/world")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::Path("hello/world")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::Path("q/hello/world")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::Path("/q/hello/world")));

  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::Path("world.txt")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::Path("/world.txt")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::Path("hello/world.txt")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::Path("q/hello/world.txt")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::Path("/q/hello/world.txt")));

  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::Path("world.")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::Path("/world.")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::Path("hello/world.")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::Path("q/hello/world.")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::Path("/q/hello/world.")));

  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::Path("world.t")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::Path("/world.t")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::Path("hello/world.t")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::Path("q/hello/world.t")));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileNameWithoutExtension(IO::Path("/q/hello/world.t")));
}


TEST(TestIO_Path, GetFileNameWithoutExtensionView_PathView)
{
  EXPECT_EQ(IO::PathView("world"), IO::Path::GetFileNameWithoutExtensionView(IO::PathView("world")));
  EXPECT_EQ(IO::PathView("world"), IO::Path::GetFileNameWithoutExtensionView(IO::PathView("/world")));
  EXPECT_EQ(IO::PathView("world"), IO::Path::GetFileNameWithoutExtensionView(IO::PathView("hello/world")));
  EXPECT_EQ(IO::PathView("world"), IO::Path::GetFileNameWithoutExtensionView(IO::PathView("q/hello/world")));
  EXPECT_EQ(IO::PathView("world"), IO::Path::GetFileNameWithoutExtensionView(IO::PathView("/q/hello/world")));

  EXPECT_EQ(IO::PathView("world"), IO::Path::GetFileNameWithoutExtensionView(IO::PathView("world.txt")));
  EXPECT_EQ(IO::PathView("world"), IO::Path::GetFileNameWithoutExtensionView(IO::PathView("/world.txt")));
  EXPECT_EQ(IO::PathView("world"), IO::Path::GetFileNameWithoutExtensionView(IO::PathView("hello/world.txt")));
  EXPECT_EQ(IO::PathView("world"), IO::Path::GetFileNameWithoutExtensionView(IO::PathView("q/hello/world.txt")));
  EXPECT_EQ(IO::PathView("world"), IO::Path::GetFileNameWithoutExtensionView(IO::PathView("/q/hello/world.txt")));

  EXPECT_EQ(IO::PathView("world"), IO::Path::GetFileNameWithoutExtensionView(IO::PathView("world.")));
  EXPECT_EQ(IO::PathView("world"), IO::Path::GetFileNameWithoutExtensionView(IO::PathView("/world.")));
  EXPECT_EQ(IO::PathView("world"), IO::Path::GetFileNameWithoutExtensionView(IO::PathView("hello/world.")));
  EXPECT_EQ(IO::PathView("world"), IO::Path::GetFileNameWithoutExtensionView(IO::PathView("q/hello/world.")));
  EXPECT_EQ(IO::PathView("world"), IO::Path::GetFileNameWithoutExtensionView(IO::PathView("/q/hello/world.")));

  EXPECT_EQ(IO::PathView("world"), IO::Path::GetFileNameWithoutExtensionView(IO::PathView("world.t")));
  EXPECT_EQ(IO::PathView("world"), IO::Path::GetFileNameWithoutExtensionView(IO::PathView("/world.t")));
  EXPECT_EQ(IO::PathView("world"), IO::Path::GetFileNameWithoutExtensionView(IO::PathView("hello/world.t")));
  EXPECT_EQ(IO::PathView("world"), IO::Path::GetFileNameWithoutExtensionView(IO::PathView("q/hello/world.t")));
  EXPECT_EQ(IO::PathView("world"), IO::Path::GetFileNameWithoutExtensionView(IO::PathView("/q/hello/world.t")));
}


TEST(TestIO_Path, GetExtension_CString)
{
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension("world"));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension("/world"));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension("hello/world"));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension("q/hello/world"));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension("/q/hello/world"));

  EXPECT_EQ(IO::Path(".t"), IO::Path::GetExtension("world.t"));
  EXPECT_EQ(IO::Path(".txt"), IO::Path::GetExtension("/world.txt"));
  EXPECT_EQ(IO::Path(".txt"), IO::Path::GetExtension("hello/world.txt"));
  EXPECT_EQ(IO::Path(".txt"), IO::Path::GetExtension("q/hello/world.txt"));
  EXPECT_EQ(IO::Path(".txt"), IO::Path::GetExtension("/q/hello/world.txt"));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension("/q/hello/"));

  EXPECT_EQ(IO::Path("."), IO::Path::GetExtension("/e."));
  EXPECT_EQ(IO::Path("."), IO::Path::GetExtension("/."));

  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(".e/"));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(".e/e"));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension("./e"));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension("/.e/"));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension("/.e/e"));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension("/./e"));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension("e./e/"));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension("e./e/e"));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension("./e/e"));
}


TEST(TestIO_Path, GetExtension_PathView)
{
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::PathView("world")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::PathView("/world")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::PathView("hello/world")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::PathView("q/hello/world")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::PathView("/q/hello/world")));

  EXPECT_EQ(IO::Path(".t"), IO::Path::GetExtension(IO::PathView("world.t")));
  EXPECT_EQ(IO::Path(".txt"), IO::Path::GetExtension(IO::PathView("/world.txt")));
  EXPECT_EQ(IO::Path(".txt"), IO::Path::GetExtension(IO::PathView("hello/world.txt")));
  EXPECT_EQ(IO::Path(".txt"), IO::Path::GetExtension(IO::PathView("q/hello/world.txt")));
  EXPECT_EQ(IO::Path(".txt"), IO::Path::GetExtension(IO::PathView("/q/hello/world.txt")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::PathView("/q/hello/")));

  EXPECT_EQ(IO::Path("."), IO::Path::GetExtension(IO::PathView("/e.")));
  EXPECT_EQ(IO::Path("."), IO::Path::GetExtension(IO::PathView("/.")));

  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::PathView(".e/")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::PathView(".e/e")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::PathView("./e")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::PathView("/.e/")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::PathView("/.e/e")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::PathView("/./e")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::PathView("e./e/")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::PathView("e./e/e")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::PathView("./e/e")));
}


TEST(TestIO_Path, GetExtension_Path)
{
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::Path("world")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::Path("/world")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::Path("hello/world")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::Path("q/hello/world")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::Path("/q/hello/world")));

  EXPECT_EQ(IO::Path(".t"), IO::Path::GetExtension(IO::Path("world.t")));
  EXPECT_EQ(IO::Path(".txt"), IO::Path::GetExtension(IO::Path("/world.txt")));
  EXPECT_EQ(IO::Path(".txt"), IO::Path::GetExtension(IO::Path("hello/world.txt")));
  EXPECT_EQ(IO::Path(".txt"), IO::Path::GetExtension(IO::Path("q/hello/world.txt")));
  EXPECT_EQ(IO::Path(".txt"), IO::Path::GetExtension(IO::Path("/q/hello/world.txt")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::Path("/q/hello/")));

  EXPECT_EQ(IO::Path("."), IO::Path::GetExtension(IO::Path("/e.")));
  EXPECT_EQ(IO::Path("."), IO::Path::GetExtension(IO::Path("/.")));

  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::Path(".e/")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::Path(".e/e")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::Path("./e")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::Path("/.e/")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::Path("/.e/e")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::Path("/./e")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::Path("e./e/")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::Path("e./e/e")));
  EXPECT_EQ(IO::Path(""), IO::Path::GetExtension(IO::Path("./e/e")));
}


TEST(TestIO_Path, Equal)
{
  IO::Path path1("hello");
  IO::Path path2("hello");

  EXPECT_EQ(path1, path2);
}


TEST(TestIO_Path, NotEqual)
{
  IO::Path path1("hello");
  IO::Path path2("world");

  EXPECT_NE(path1, path2);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator=
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestIO_Path, OpAssign_CString)
{
  //--------------0123456789
  IO::Path src("old school");
  //---------0123456789

  const char* const pszNew = "the quick brown fox";
  src = pszNew;
  EXPECT_EQ(pszNew, src);
}

TEST(TestIO_Path, OpAssign_StringViewLite)
{
  //--------------0123456789
  IO::Path src("old school");
  //---------0123456789

  constexpr StringViewLite strNew("the quick brown fox");
  src = strNew;
  EXPECT_EQ(strNew, src);
}

TEST(TestIO_Path, OpAssign_PathView)
{
  //--------------0123456789
  IO::Path src("old school");
  //---------0123456789

  constexpr IO::PathView strNew("the quick brown fox");
  src = strNew;
  EXPECT_EQ(strNew, src);
}

TEST(TestIO_Path, OpAssign_UTF8String)
{
  //--------------0123456789
  IO::Path src("old school");
  //---------0123456789

  UTF8String strNew("the quick brown fox");

  src = strNew;
  EXPECT_EQ(strNew, src);
}

TEST(TestIO_Path, OpAssign_Path)
{
  //--------------0123456789
  IO::Path src("old school");
  //---------0123456789

  IO::Path strNew("the quick brown fox");

  src = strNew;
  EXPECT_EQ(strNew, src);
}


TEST(TestIO_Path, OpAssign_CString_Null)
{
  //--------------0123456789
  IO::Path src("old school");
  //---------0123456789

  constexpr const char* const pszNull = nullptr;
  src = pszNull;

  EXPECT_EQ("", src);
}


TEST(TestIO_Path, OpAssign_StringViewLite_Empty)
{
  //--------------0123456789
  IO::Path src("old school");
  //---------0123456789

  constexpr StringViewLite strNew;
  src = strNew;
  EXPECT_EQ(strNew, src);
}


TEST(TestIO_Path, OpAssign_PathView_Empty)
{
  //--------------0123456789
  IO::Path src("old school");
  //---------0123456789

  constexpr IO::PathView strNew;
  src = strNew;
  EXPECT_EQ(strNew, src);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestIO_Path, OperatorLessThan)
{
  EXPECT_LT(IO::Path("A"), IO::Path("B"));
  EXPECT_LT(IO::Path("A"), IO::Path("AB"));
  EXPECT_LT(IO::Path(""), IO::Path("AB"));

  EXPECT_TRUE(IO::Path("A") < IO::Path("B"));
  EXPECT_FALSE(IO::Path("B") < IO::Path("B"));
  EXPECT_FALSE(IO::Path("C") < IO::Path("B"));
  EXPECT_FALSE(IO::Path("B") < IO::Path("A"));
  EXPECT_FALSE(IO::Path("B") < IO::Path("B"));
  EXPECT_TRUE(IO::Path("B") < IO::Path("C"));

  EXPECT_TRUE(IO::Path("A") < IO::Path("BA"));
  EXPECT_TRUE(IO::Path("B") < IO::Path("BA"));
  EXPECT_FALSE(IO::Path("C") < IO::Path("BA"));
  EXPECT_FALSE(IO::Path("BA") < IO::Path("A"));
  EXPECT_FALSE(IO::Path("BA") < IO::Path("B"));
  EXPECT_TRUE(IO::Path("BA") < IO::Path("C"));

  EXPECT_TRUE(IO::Path("AA") < IO::Path("B"));
  EXPECT_FALSE(IO::Path("BA") < IO::Path("B"));
  EXPECT_FALSE(IO::Path("CA") < IO::Path("B"));
  EXPECT_FALSE(IO::Path("B") < IO::Path("AA"));
  EXPECT_TRUE(IO::Path("B") < IO::Path("BA"));
  EXPECT_TRUE(IO::Path("B") < IO::Path("CA"));
}

TEST(TestIO_Path, OperatorLessThan_CString_LHS)
{
  EXPECT_LT("A", IO::Path("B"));
  EXPECT_LT("A", IO::Path("AB"));
  EXPECT_LT("", IO::Path("AB"));

  EXPECT_TRUE("A" < IO::Path("B"));
  EXPECT_FALSE("B" < IO::Path("B"));
  EXPECT_FALSE("C" < IO::Path("B"));
  EXPECT_FALSE("B" < IO::Path("A"));
  EXPECT_FALSE("B" < IO::Path("B"));
  EXPECT_TRUE("B" < IO::Path("C"));

  EXPECT_TRUE("A" < IO::Path("BA"));
  EXPECT_TRUE("B" < IO::Path("BA"));
  EXPECT_FALSE("C" < IO::Path("BA"));
  EXPECT_FALSE("BA" < IO::Path("A"));
  EXPECT_FALSE("BA" < IO::Path("B"));
  EXPECT_TRUE("BA" < IO::Path("C"));

  EXPECT_TRUE("AA" < IO::Path("B"));
  EXPECT_FALSE("BA" < IO::Path("B"));
  EXPECT_FALSE("CA" < IO::Path("B"));
  EXPECT_FALSE("B" < IO::Path("AA"));
  EXPECT_TRUE("B" < IO::Path("BA"));
  EXPECT_TRUE("B" < IO::Path("CA"));
}

TEST(TestIO_Path, OperatorLessThan_CString_RHS)
{
  EXPECT_LT(IO::Path("A"), "B");
  EXPECT_LT(IO::Path("A"), "AB");
  EXPECT_LT(IO::Path(""), "AB");

  EXPECT_TRUE(IO::Path("A") < "B");
  EXPECT_FALSE(IO::Path("B") < "B");
  EXPECT_FALSE(IO::Path("C") < "B");
  EXPECT_FALSE(IO::Path("B") < "A");
  EXPECT_FALSE(IO::Path("B") < "B");
  EXPECT_TRUE(IO::Path("B") < "C");

  EXPECT_TRUE(IO::Path("A") < "BA");
  EXPECT_TRUE(IO::Path("B") < "BA");
  EXPECT_FALSE(IO::Path("C") < "BA");
  EXPECT_FALSE(IO::Path("BA") < "A");
  EXPECT_FALSE(IO::Path("BA") < "B");
  EXPECT_TRUE(IO::Path("BA") < "C");

  EXPECT_TRUE(IO::Path("AA") < "B");
  EXPECT_FALSE(IO::Path("BA") < "B");
  EXPECT_FALSE(IO::Path("CA") < "B");
  EXPECT_FALSE(IO::Path("B") < "AA");
  EXPECT_TRUE(IO::Path("B") < "BA");
  EXPECT_TRUE(IO::Path("B") < "CA");
}

TEST(TestIO_Path, OperatorLessThan_StringViewLite_LHS)
{
  EXPECT_LT(StringViewLite("A"), IO::Path("B"));
  EXPECT_LT(StringViewLite("A"), IO::Path("AB"));
  EXPECT_LT(StringViewLite(""), IO::Path("AB"));

  EXPECT_TRUE(StringViewLite("A") < IO::Path("B"));
  EXPECT_FALSE(StringViewLite("B") < IO::Path("B"));
  EXPECT_FALSE(StringViewLite("C") < IO::Path("B"));
  EXPECT_FALSE(StringViewLite("B") < IO::Path("A"));
  EXPECT_FALSE(StringViewLite("B") < IO::Path("B"));
  EXPECT_TRUE(StringViewLite("B") < IO::Path("C"));

  EXPECT_TRUE(StringViewLite("A") < IO::Path("BA"));
  EXPECT_TRUE(StringViewLite("B") < IO::Path("BA"));
  EXPECT_FALSE(StringViewLite("C") < IO::Path("BA"));
  EXPECT_FALSE(StringViewLite("BA") < IO::Path("A"));
  EXPECT_FALSE(StringViewLite("BA") < IO::Path("B"));
  EXPECT_TRUE(StringViewLite("BA") < IO::Path("C"));

  EXPECT_TRUE(StringViewLite("AA") < IO::Path("B"));
  EXPECT_FALSE(StringViewLite("BA") < IO::Path("B"));
  EXPECT_FALSE(StringViewLite("CA") < IO::Path("B"));
  EXPECT_FALSE(StringViewLite("B") < IO::Path("AA"));
  EXPECT_TRUE(StringViewLite("B") < IO::Path("BA"));
  EXPECT_TRUE(StringViewLite("B") < IO::Path("CA"));
}

TEST(TestIO_Path, OperatorLessThan_StringViewLite_RHS)
{
  EXPECT_LT(IO::Path("A"), StringViewLite("B"));
  EXPECT_LT(IO::Path("A"), StringViewLite("AB"));
  EXPECT_LT(IO::Path(""), StringViewLite("AB"));

  EXPECT_TRUE(IO::Path("A") < StringViewLite("B"));
  EXPECT_FALSE(IO::Path("B") < StringViewLite("B"));
  EXPECT_FALSE(IO::Path("C") < StringViewLite("B"));
  EXPECT_FALSE(IO::Path("B") < StringViewLite("A"));
  EXPECT_FALSE(IO::Path("B") < StringViewLite("B"));
  EXPECT_TRUE(IO::Path("B") < StringViewLite("C"));

  EXPECT_TRUE(IO::Path("A") < StringViewLite("BA"));
  EXPECT_TRUE(IO::Path("B") < StringViewLite("BA"));
  EXPECT_FALSE(IO::Path("C") < StringViewLite("BA"));
  EXPECT_FALSE(IO::Path("BA") < StringViewLite("A"));
  EXPECT_FALSE(IO::Path("BA") < StringViewLite("B"));
  EXPECT_TRUE(IO::Path("BA") < StringViewLite("C"));

  EXPECT_TRUE(IO::Path("AA") < StringViewLite("B"));
  EXPECT_FALSE(IO::Path("BA") < StringViewLite("B"));
  EXPECT_FALSE(IO::Path("CA") < StringViewLite("B"));
  EXPECT_FALSE(IO::Path("B") < StringViewLite("AA"));
  EXPECT_TRUE(IO::Path("B") < StringViewLite("BA"));
  EXPECT_TRUE(IO::Path("B") < StringViewLite("CA"));
}

TEST(TestIO_Path, OperatorLessThan_String_LHS)
{
  EXPECT_LT(std::string("A"), IO::Path("B"));
  EXPECT_LT(std::string("A"), IO::Path("AB"));
  EXPECT_LT(std::string(""), IO::Path("AB"));

  EXPECT_TRUE(std::string("A") < IO::Path("B"));
  EXPECT_FALSE(std::string("B") < IO::Path("B"));
  EXPECT_FALSE(std::string("C") < IO::Path("B"));
  EXPECT_FALSE(std::string("B") < IO::Path("A"));
  EXPECT_FALSE(std::string("B") < IO::Path("B"));
  EXPECT_TRUE(std::string("B") < IO::Path("C"));

  EXPECT_TRUE(std::string("A") < IO::Path("BA"));
  EXPECT_TRUE(std::string("B") < IO::Path("BA"));
  EXPECT_FALSE(std::string("C") < IO::Path("BA"));
  EXPECT_FALSE(std::string("BA") < IO::Path("A"));
  EXPECT_FALSE(std::string("BA") < IO::Path("B"));
  EXPECT_TRUE(std::string("BA") < IO::Path("C"));

  EXPECT_TRUE(std::string("AA") < IO::Path("B"));
  EXPECT_FALSE(std::string("BA") < IO::Path("B"));
  EXPECT_FALSE(std::string("CA") < IO::Path("B"));
  EXPECT_FALSE(std::string("B") < IO::Path("AA"));
  EXPECT_TRUE(std::string("B") < IO::Path("BA"));
  EXPECT_TRUE(std::string("B") < IO::Path("CA"));
}

TEST(TestIO_Path, OperatorLessThan_String_RHS)
{
  EXPECT_LT(IO::Path("A"), std::string("B"));
  EXPECT_LT(IO::Path("A"), std::string("AB"));
  EXPECT_LT(IO::Path(""), std::string("AB"));

  EXPECT_TRUE(IO::Path("A") < std::string("B"));
  EXPECT_FALSE(IO::Path("B") < std::string("B"));
  EXPECT_FALSE(IO::Path("C") < std::string("B"));
  EXPECT_FALSE(IO::Path("B") < std::string("A"));
  EXPECT_FALSE(IO::Path("B") < std::string("B"));
  EXPECT_TRUE(IO::Path("B") < std::string("C"));

  EXPECT_TRUE(IO::Path("A") < std::string("BA"));
  EXPECT_TRUE(IO::Path("B") < std::string("BA"));
  EXPECT_FALSE(IO::Path("C") < std::string("BA"));
  EXPECT_FALSE(IO::Path("BA") < std::string("A"));
  EXPECT_FALSE(IO::Path("BA") < std::string("B"));
  EXPECT_TRUE(IO::Path("BA") < std::string("C"));

  EXPECT_TRUE(IO::Path("AA") < std::string("B"));
  EXPECT_FALSE(IO::Path("BA") < std::string("B"));
  EXPECT_FALSE(IO::Path("CA") < std::string("B"));
  EXPECT_FALSE(IO::Path("B") < std::string("AA"));
  EXPECT_TRUE(IO::Path("B") < std::string("BA"));
  EXPECT_TRUE(IO::Path("B") < std::string("CA"));
}


TEST(TestIO_Path, OperatorLessThan_UTF8String_LHS)
{
  EXPECT_LT(UTF8String("A"), IO::Path("B"));
  EXPECT_LT(UTF8String("A"), IO::Path("AB"));
  EXPECT_LT(UTF8String(""), IO::Path("AB"));

  EXPECT_TRUE(UTF8String("A") < IO::Path("B"));
  EXPECT_FALSE(UTF8String("B") < IO::Path("B"));
  EXPECT_FALSE(UTF8String("C") < IO::Path("B"));
  EXPECT_FALSE(UTF8String("B") < IO::Path("A"));
  EXPECT_FALSE(UTF8String("B") < IO::Path("B"));
  EXPECT_TRUE(UTF8String("B") < IO::Path("C"));

  EXPECT_TRUE(UTF8String("A") < IO::Path("BA"));
  EXPECT_TRUE(UTF8String("B") < IO::Path("BA"));
  EXPECT_FALSE(UTF8String("C") < IO::Path("BA"));
  EXPECT_FALSE(UTF8String("BA") < IO::Path("A"));
  EXPECT_FALSE(UTF8String("BA") < IO::Path("B"));
  EXPECT_TRUE(UTF8String("BA") < IO::Path("C"));

  EXPECT_TRUE(UTF8String("AA") < IO::Path("B"));
  EXPECT_FALSE(UTF8String("BA") < IO::Path("B"));
  EXPECT_FALSE(UTF8String("CA") < IO::Path("B"));
  EXPECT_FALSE(UTF8String("B") < IO::Path("AA"));
  EXPECT_TRUE(UTF8String("B") < IO::Path("BA"));
  EXPECT_TRUE(UTF8String("B") < IO::Path("CA"));
}

TEST(TestIO_Path, OperatorLessThan_UTF8String_RHS)
{
  EXPECT_LT(IO::Path("A"), UTF8String("B"));
  EXPECT_LT(IO::Path("A"), UTF8String("AB"));
  EXPECT_LT(IO::Path(""), UTF8String("AB"));

  EXPECT_TRUE(IO::Path("A") < UTF8String("B"));
  EXPECT_FALSE(IO::Path("B") < UTF8String("B"));
  EXPECT_FALSE(IO::Path("C") < UTF8String("B"));
  EXPECT_FALSE(IO::Path("B") < UTF8String("A"));
  EXPECT_FALSE(IO::Path("B") < UTF8String("B"));
  EXPECT_TRUE(IO::Path("B") < UTF8String("C"));

  EXPECT_TRUE(IO::Path("A") < UTF8String("BA"));
  EXPECT_TRUE(IO::Path("B") < UTF8String("BA"));
  EXPECT_FALSE(IO::Path("C") < UTF8String("BA"));
  EXPECT_FALSE(IO::Path("BA") < UTF8String("A"));
  EXPECT_FALSE(IO::Path("BA") < UTF8String("B"));
  EXPECT_TRUE(IO::Path("BA") < UTF8String("C"));

  EXPECT_TRUE(IO::Path("AA") < UTF8String("B"));
  EXPECT_FALSE(IO::Path("BA") < UTF8String("B"));
  EXPECT_FALSE(IO::Path("CA") < UTF8String("B"));
  EXPECT_FALSE(IO::Path("B") < UTF8String("AA"));
  EXPECT_TRUE(IO::Path("B") < UTF8String("BA"));
  EXPECT_TRUE(IO::Path("B") < UTF8String("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestIO_Path, OperatorLessThanOrEqual)
{
  EXPECT_LE(IO::Path("A"), IO::Path("B"));
  EXPECT_LE(IO::Path("A"), IO::Path("AB"));
  EXPECT_LE(IO::Path(""), IO::Path("AB"));
  EXPECT_LE(IO::Path("A"), IO::Path("A"));

  EXPECT_TRUE(IO::Path("A") <= IO::Path("B"));
  EXPECT_TRUE(IO::Path("B") <= IO::Path("B"));
  EXPECT_FALSE(IO::Path("C") <= IO::Path("B"));
  EXPECT_FALSE(IO::Path("B") <= IO::Path("A"));
  EXPECT_TRUE(IO::Path("B") <= IO::Path("B"));
  EXPECT_TRUE(IO::Path("B") <= IO::Path("C"));

  EXPECT_TRUE(IO::Path("A") <= IO::Path("BA"));
  EXPECT_TRUE(IO::Path("B") <= IO::Path("BA"));
  EXPECT_FALSE(IO::Path("C") <= IO::Path("BA"));
  EXPECT_FALSE(IO::Path("BA") <= IO::Path("A"));
  EXPECT_FALSE(IO::Path("BA") <= IO::Path("B"));
  EXPECT_TRUE(IO::Path("BA") <= IO::Path("C"));

  EXPECT_TRUE(IO::Path("AA") <= IO::Path("B"));
  EXPECT_FALSE(IO::Path("BA") <= IO::Path("B"));
  EXPECT_FALSE(IO::Path("CA") <= IO::Path("B"));
  EXPECT_FALSE(IO::Path("B") <= IO::Path("AA"));
  EXPECT_TRUE(IO::Path("B") <= IO::Path("BA"));
  EXPECT_TRUE(IO::Path("B") <= IO::Path("CA"));
}

TEST(TestIO_Path, OperatorLessThanOrEqual_CString_LHS)
{
  EXPECT_LE("A", IO::Path("B"));
  EXPECT_LE("A", IO::Path("AB"));
  EXPECT_LE("", IO::Path("AB"));
  EXPECT_LE("A", IO::Path("A"));

  EXPECT_TRUE("A" <= IO::Path("B"));
  EXPECT_TRUE("B" <= IO::Path("B"));
  EXPECT_FALSE("C" <= IO::Path("B"));
  EXPECT_FALSE("B" <= IO::Path("A"));
  EXPECT_TRUE("B" <= IO::Path("B"));
  EXPECT_TRUE("B" <= IO::Path("C"));

  EXPECT_TRUE("A" <= IO::Path("BA"));
  EXPECT_TRUE("B" <= IO::Path("BA"));
  EXPECT_FALSE("C" <= IO::Path("BA"));
  EXPECT_FALSE("BA" <= IO::Path("A"));
  EXPECT_FALSE("BA" <= IO::Path("B"));
  EXPECT_TRUE("BA" <= IO::Path("C"));

  EXPECT_TRUE("AA" <= IO::Path("B"));
  EXPECT_FALSE("BA" <= IO::Path("B"));
  EXPECT_FALSE("CA" <= IO::Path("B"));
  EXPECT_FALSE("B" <= IO::Path("AA"));
  EXPECT_TRUE("B" <= IO::Path("BA"));
  EXPECT_TRUE("B" <= IO::Path("CA"));
}

TEST(TestIO_Path, OperatorLessThanOrEqual_CString_RHS)
{
  EXPECT_LE(IO::Path("A"), "B");
  EXPECT_LE(IO::Path("A"), "AB");
  EXPECT_LE(IO::Path(""), "AB");
  EXPECT_LE(IO::Path("A"), "A");

  EXPECT_TRUE(IO::Path("A") <= "B");
  EXPECT_TRUE(IO::Path("B") <= "B");
  EXPECT_FALSE(IO::Path("C") <= "B");
  EXPECT_FALSE(IO::Path("B") <= "A");
  EXPECT_TRUE(IO::Path("B") <= "B");
  EXPECT_TRUE(IO::Path("B") <= "C");

  EXPECT_TRUE(IO::Path("A") <= "BA");
  EXPECT_TRUE(IO::Path("B") <= "BA");
  EXPECT_FALSE(IO::Path("C") <= "BA");
  EXPECT_FALSE(IO::Path("BA") <= "A");
  EXPECT_FALSE(IO::Path("BA") <= "B");
  EXPECT_TRUE(IO::Path("BA") <= "C");

  EXPECT_TRUE(IO::Path("AA") <= "B");
  EXPECT_FALSE(IO::Path("BA") <= "B");
  EXPECT_FALSE(IO::Path("CA") <= "B");
  EXPECT_FALSE(IO::Path("B") <= "AA");
  EXPECT_TRUE(IO::Path("B") <= "BA");
  EXPECT_TRUE(IO::Path("B") <= "CA");
}

TEST(TestIO_Path, OperatorLessThanOrEqual_StringViewLite_LHS)
{
  EXPECT_LE(StringViewLite("A"), IO::Path("B"));
  EXPECT_LE(StringViewLite("A"), IO::Path("AB"));
  EXPECT_LE(StringViewLite(""), IO::Path("AB"));
  EXPECT_LE(StringViewLite("A"), IO::Path("A"));

  EXPECT_TRUE(StringViewLite("A") <= IO::Path("B"));
  EXPECT_TRUE(StringViewLite("B") <= IO::Path("B"));
  EXPECT_FALSE(StringViewLite("C") <= IO::Path("B"));
  EXPECT_FALSE(StringViewLite("B") <= IO::Path("A"));
  EXPECT_TRUE(StringViewLite("B") <= IO::Path("B"));
  EXPECT_TRUE(StringViewLite("B") <= IO::Path("C"));

  EXPECT_TRUE(StringViewLite("A") <= IO::Path("BA"));
  EXPECT_TRUE(StringViewLite("B") <= IO::Path("BA"));
  EXPECT_FALSE(StringViewLite("C") <= IO::Path("BA"));
  EXPECT_FALSE(StringViewLite("BA") <= IO::Path("A"));
  EXPECT_FALSE(StringViewLite("BA") <= IO::Path("B"));
  EXPECT_TRUE(StringViewLite("BA") <= IO::Path("C"));

  EXPECT_TRUE(StringViewLite("AA") <= IO::Path("B"));
  EXPECT_FALSE(StringViewLite("BA") <= IO::Path("B"));
  EXPECT_FALSE(StringViewLite("CA") <= IO::Path("B"));
  EXPECT_FALSE(StringViewLite("B") <= IO::Path("AA"));
  EXPECT_TRUE(StringViewLite("B") <= IO::Path("BA"));
  EXPECT_TRUE(StringViewLite("B") <= IO::Path("CA"));
}

TEST(TestIO_Path, OperatorLessThanOrEqual_StringViewLite_RHS)
{
  EXPECT_LE(IO::Path("A"), StringViewLite("B"));
  EXPECT_LE(IO::Path("A"), StringViewLite("AB"));
  EXPECT_LE(IO::Path(""), StringViewLite("AB"));
  EXPECT_LE(IO::Path("A"), StringViewLite("A"));

  EXPECT_TRUE(IO::Path("A") <= StringViewLite("B"));
  EXPECT_TRUE(IO::Path("B") <= StringViewLite("B"));
  EXPECT_FALSE(IO::Path("C") <= StringViewLite("B"));
  EXPECT_FALSE(IO::Path("B") <= StringViewLite("A"));
  EXPECT_TRUE(IO::Path("B") <= StringViewLite("B"));
  EXPECT_TRUE(IO::Path("B") <= StringViewLite("C"));

  EXPECT_TRUE(IO::Path("A") <= StringViewLite("BA"));
  EXPECT_TRUE(IO::Path("B") <= StringViewLite("BA"));
  EXPECT_FALSE(IO::Path("C") <= StringViewLite("BA"));
  EXPECT_FALSE(IO::Path("BA") <= StringViewLite("A"));
  EXPECT_FALSE(IO::Path("BA") <= StringViewLite("B"));
  EXPECT_TRUE(IO::Path("BA") <= StringViewLite("C"));

  EXPECT_TRUE(IO::Path("AA") <= StringViewLite("B"));
  EXPECT_FALSE(IO::Path("BA") <= StringViewLite("B"));
  EXPECT_FALSE(IO::Path("CA") <= StringViewLite("B"));
  EXPECT_FALSE(IO::Path("B") <= StringViewLite("AA"));
  EXPECT_TRUE(IO::Path("B") <= StringViewLite("BA"));
  EXPECT_TRUE(IO::Path("B") <= StringViewLite("CA"));
}

TEST(TestIO_Path, OperatorLessThanOrEqual_String_LHS)
{
  EXPECT_LE(std::string("A"), IO::Path("B"));
  EXPECT_LE(std::string("A"), IO::Path("AB"));
  EXPECT_LE(std::string(""), IO::Path("AB"));
  EXPECT_LE(std::string("A"), IO::Path("A"));

  EXPECT_TRUE(std::string("A") <= IO::Path("B"));
  EXPECT_TRUE(std::string("B") <= IO::Path("B"));
  EXPECT_FALSE(std::string("C") <= IO::Path("B"));
  EXPECT_FALSE(std::string("B") <= IO::Path("A"));
  EXPECT_TRUE(std::string("B") <= IO::Path("B"));
  EXPECT_TRUE(std::string("B") <= IO::Path("C"));

  EXPECT_TRUE(std::string("A") <= IO::Path("BA"));
  EXPECT_TRUE(std::string("B") <= IO::Path("BA"));
  EXPECT_FALSE(std::string("C") <= IO::Path("BA"));
  EXPECT_FALSE(std::string("BA") <= IO::Path("A"));
  EXPECT_FALSE(std::string("BA") <= IO::Path("B"));
  EXPECT_TRUE(std::string("BA") <= IO::Path("C"));

  EXPECT_TRUE(std::string("AA") <= IO::Path("B"));
  EXPECT_FALSE(std::string("BA") <= IO::Path("B"));
  EXPECT_FALSE(std::string("CA") <= IO::Path("B"));
  EXPECT_FALSE(std::string("B") <= IO::Path("AA"));
  EXPECT_TRUE(std::string("B") <= IO::Path("BA"));
  EXPECT_TRUE(std::string("B") <= IO::Path("CA"));
}

TEST(TestIO_Path, OperatorLessThanOrEqual_String_RHS)
{
  EXPECT_LE(IO::Path("A"), std::string("B"));
  EXPECT_LE(IO::Path("A"), std::string("AB"));
  EXPECT_LE(IO::Path(""), std::string("AB"));
  EXPECT_LE(IO::Path("A"), std::string("A"));

  EXPECT_TRUE(IO::Path("A") <= std::string("B"));
  EXPECT_TRUE(IO::Path("B") <= std::string("B"));
  EXPECT_FALSE(IO::Path("C") <= std::string("B"));
  EXPECT_FALSE(IO::Path("B") <= std::string("A"));
  EXPECT_TRUE(IO::Path("B") <= std::string("B"));
  EXPECT_TRUE(IO::Path("B") <= std::string("C"));

  EXPECT_TRUE(IO::Path("A") <= std::string("BA"));
  EXPECT_TRUE(IO::Path("B") <= std::string("BA"));
  EXPECT_FALSE(IO::Path("C") <= std::string("BA"));
  EXPECT_FALSE(IO::Path("BA") <= std::string("A"));
  EXPECT_FALSE(IO::Path("BA") <= std::string("B"));
  EXPECT_TRUE(IO::Path("BA") <= std::string("C"));

  EXPECT_TRUE(IO::Path("AA") <= std::string("B"));
  EXPECT_FALSE(IO::Path("BA") <= std::string("B"));
  EXPECT_FALSE(IO::Path("CA") <= std::string("B"));
  EXPECT_FALSE(IO::Path("B") <= std::string("AA"));
  EXPECT_TRUE(IO::Path("B") <= std::string("BA"));
  EXPECT_TRUE(IO::Path("B") <= std::string("CA"));
}

TEST(TestIO_Path, OperatorLessThanOrEqual_UTF8String_LHS)
{
  EXPECT_LE(UTF8String("A"), IO::Path("B"));
  EXPECT_LE(UTF8String("A"), IO::Path("AB"));
  EXPECT_LE(UTF8String(""), IO::Path("AB"));
  EXPECT_LE(UTF8String("A"), IO::Path("A"));

  EXPECT_TRUE(UTF8String("A") <= IO::Path("B"));
  EXPECT_TRUE(UTF8String("B") <= IO::Path("B"));
  EXPECT_FALSE(UTF8String("C") <= IO::Path("B"));
  EXPECT_FALSE(UTF8String("B") <= IO::Path("A"));
  EXPECT_TRUE(UTF8String("B") <= IO::Path("B"));
  EXPECT_TRUE(UTF8String("B") <= IO::Path("C"));

  EXPECT_TRUE(UTF8String("A") <= IO::Path("BA"));
  EXPECT_TRUE(UTF8String("B") <= IO::Path("BA"));
  EXPECT_FALSE(UTF8String("C") <= IO::Path("BA"));
  EXPECT_FALSE(UTF8String("BA") <= IO::Path("A"));
  EXPECT_FALSE(UTF8String("BA") <= IO::Path("B"));
  EXPECT_TRUE(UTF8String("BA") <= IO::Path("C"));

  EXPECT_TRUE(UTF8String("AA") <= IO::Path("B"));
  EXPECT_FALSE(UTF8String("BA") <= IO::Path("B"));
  EXPECT_FALSE(UTF8String("CA") <= IO::Path("B"));
  EXPECT_FALSE(UTF8String("B") <= IO::Path("AA"));
  EXPECT_TRUE(UTF8String("B") <= IO::Path("BA"));
  EXPECT_TRUE(UTF8String("B") <= IO::Path("CA"));
}

TEST(TestIO_Path, OperatorLessThanOrEqual_UTF8String_RHS)
{
  EXPECT_LE(IO::Path("A"), UTF8String("B"));
  EXPECT_LE(IO::Path("A"), UTF8String("AB"));
  EXPECT_LE(IO::Path(""), UTF8String("AB"));
  EXPECT_LE(IO::Path("A"), UTF8String("A"));

  EXPECT_TRUE(IO::Path("A") <= UTF8String("B"));
  EXPECT_TRUE(IO::Path("B") <= UTF8String("B"));
  EXPECT_FALSE(IO::Path("C") <= UTF8String("B"));
  EXPECT_FALSE(IO::Path("B") <= UTF8String("A"));
  EXPECT_TRUE(IO::Path("B") <= UTF8String("B"));
  EXPECT_TRUE(IO::Path("B") <= UTF8String("C"));

  EXPECT_TRUE(IO::Path("A") <= UTF8String("BA"));
  EXPECT_TRUE(IO::Path("B") <= UTF8String("BA"));
  EXPECT_FALSE(IO::Path("C") <= UTF8String("BA"));
  EXPECT_FALSE(IO::Path("BA") <= UTF8String("A"));
  EXPECT_FALSE(IO::Path("BA") <= UTF8String("B"));
  EXPECT_TRUE(IO::Path("BA") <= UTF8String("C"));

  EXPECT_TRUE(IO::Path("AA") <= UTF8String("B"));
  EXPECT_FALSE(IO::Path("BA") <= UTF8String("B"));
  EXPECT_FALSE(IO::Path("CA") <= UTF8String("B"));
  EXPECT_FALSE(IO::Path("B") <= UTF8String("AA"));
  EXPECT_TRUE(IO::Path("B") <= UTF8String("BA"));
  EXPECT_TRUE(IO::Path("B") <= UTF8String("CA"));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestIO_Path, OperatorGreaterThan)
{
  EXPECT_GT(IO::Path("B"), IO::Path("A"));
  EXPECT_GT(IO::Path("AB"), IO::Path("A"));
  EXPECT_GT(IO::Path("AB"), IO::Path(""));

  EXPECT_FALSE(IO::Path("A") > IO::Path("B"));
  EXPECT_FALSE(IO::Path("B") > IO::Path("B"));
  EXPECT_TRUE(IO::Path("C") > IO::Path("B"));
  EXPECT_TRUE(IO::Path("B") > IO::Path("A"));
  EXPECT_FALSE(IO::Path("B") > IO::Path("B"));
  EXPECT_FALSE(IO::Path("B") > IO::Path("C"));

  EXPECT_FALSE(IO::Path("A") > IO::Path("BA"));
  EXPECT_FALSE(IO::Path("B") > IO::Path("BA"));
  EXPECT_TRUE(IO::Path("C") > IO::Path("BA"));
  EXPECT_TRUE(IO::Path("BA") > IO::Path("A"));
  EXPECT_TRUE(IO::Path("BA") > IO::Path("B"));
  EXPECT_FALSE(IO::Path("BA") > IO::Path("C"));

  EXPECT_FALSE(IO::Path("AA") > IO::Path("B"));
  EXPECT_TRUE(IO::Path("BA") > IO::Path("B"));
  EXPECT_TRUE(IO::Path("CA") > IO::Path("B"));
  EXPECT_TRUE(IO::Path("B") > IO::Path("AA"));
  EXPECT_FALSE(IO::Path("B") > IO::Path("BA"));
  EXPECT_FALSE(IO::Path("B") > IO::Path("CA"));
}

TEST(TestIO_Path, OperatorGreaterThan_CString_LHS)
{
  EXPECT_GT("B", IO::Path("A"));
  EXPECT_GT("AB", IO::Path("A"));
  EXPECT_GT("AB", IO::Path(""));

  EXPECT_FALSE("A" > IO::Path("B"));
  EXPECT_FALSE("B" > IO::Path("B"));
  EXPECT_TRUE("C" > IO::Path("B"));
  EXPECT_TRUE("B" > IO::Path("A"));
  EXPECT_FALSE("B" > IO::Path("B"));
  EXPECT_FALSE("B" > IO::Path("C"));

  EXPECT_FALSE("A" > IO::Path("BA"));
  EXPECT_FALSE("B" > IO::Path("BA"));
  EXPECT_TRUE("C" > IO::Path("BA"));
  EXPECT_TRUE("BA" > IO::Path("A"));
  EXPECT_TRUE("BA" > IO::Path("B"));
  EXPECT_FALSE("BA" > IO::Path("C"));

  EXPECT_FALSE("AA" > IO::Path("B"));
  EXPECT_TRUE("BA" > IO::Path("B"));
  EXPECT_TRUE("CA" > IO::Path("B"));
  EXPECT_TRUE("B" > IO::Path("AA"));
  EXPECT_FALSE("B" > IO::Path("BA"));
  EXPECT_FALSE("B" > IO::Path("CA"));
}

TEST(TestIO_Path, OperatorGreaterThan_CString_RHS)
{
  EXPECT_GT(IO::Path("B"), "A");
  EXPECT_GT(IO::Path("AB"), "A");
  EXPECT_GT(IO::Path("AB"), "");

  EXPECT_FALSE(IO::Path("A") > "B");
  EXPECT_FALSE(IO::Path("B") > "B");
  EXPECT_TRUE(IO::Path("B") > "A");
  EXPECT_TRUE(IO::Path("C") > "B");
  EXPECT_FALSE(IO::Path("B") > "B");
  EXPECT_FALSE(IO::Path("B") > "C");

  EXPECT_FALSE(IO::Path("A") > "BA");
  EXPECT_FALSE(IO::Path("B") > "BA");
  EXPECT_TRUE(IO::Path("C") > "BA");
  EXPECT_TRUE(IO::Path("BA") > "A");
  EXPECT_TRUE(IO::Path("BA") > "B");
  EXPECT_FALSE(IO::Path("BA") > "C");

  EXPECT_FALSE(IO::Path("AA") > "B");
  EXPECT_TRUE(IO::Path("BA") > "B");
  EXPECT_TRUE(IO::Path("CA") > "B");
  EXPECT_TRUE(IO::Path("B") > "AA");
  EXPECT_FALSE(IO::Path("B") > "BA");
  EXPECT_FALSE(IO::Path("B") > "CA");
}

TEST(TestIO_Path, OperatorGreaterThan_StringViewLite_LHS)
{
  EXPECT_GT(StringViewLite("B"), IO::Path("A"));
  EXPECT_GT(StringViewLite("AB"), IO::Path("A"));
  EXPECT_GT(StringViewLite("AB"), IO::Path(""));

  EXPECT_FALSE(StringViewLite("A") > IO::Path("B"));
  EXPECT_FALSE(StringViewLite("B") > IO::Path("B"));
  EXPECT_TRUE(StringViewLite("C") > IO::Path("B"));
  EXPECT_TRUE(StringViewLite("B") > IO::Path("A"));
  EXPECT_FALSE(StringViewLite("B") > IO::Path("B"));
  EXPECT_FALSE(StringViewLite("B") > IO::Path("C"));

  EXPECT_FALSE(StringViewLite("A") > IO::Path("BA"));
  EXPECT_FALSE(StringViewLite("B") > IO::Path("BA"));
  EXPECT_TRUE(StringViewLite("C") > IO::Path("BA"));
  EXPECT_TRUE(StringViewLite("BA") > IO::Path("A"));
  EXPECT_TRUE(StringViewLite("BA") > IO::Path("B"));
  EXPECT_FALSE(StringViewLite("BA") > IO::Path("C"));

  EXPECT_FALSE(StringViewLite("AA") > IO::Path("B"));
  EXPECT_TRUE(StringViewLite("BA") > IO::Path("B"));
  EXPECT_TRUE(StringViewLite("CA") > IO::Path("B"));
  EXPECT_TRUE(StringViewLite("B") > IO::Path("AA"));
  EXPECT_FALSE(StringViewLite("B") > IO::Path("BA"));
  EXPECT_FALSE(StringViewLite("B") > IO::Path("CA"));
}

TEST(TestIO_Path, OperatorGreaterThan_StringViewLite_RHS)
{
  EXPECT_GT(IO::Path("B"), StringViewLite("A"));
  EXPECT_GT(IO::Path("AB"), StringViewLite("A"));
  EXPECT_GT(IO::Path("AB"), StringViewLite(""));

  EXPECT_FALSE(IO::Path("A") > StringViewLite("B"));
  EXPECT_FALSE(IO::Path("B") > StringViewLite("B"));
  EXPECT_TRUE(IO::Path("C") > StringViewLite("B"));
  EXPECT_TRUE(IO::Path("B") > StringViewLite("A"));
  EXPECT_FALSE(IO::Path("B") > StringViewLite("B"));
  EXPECT_FALSE(IO::Path("B") > StringViewLite("C"));

  EXPECT_FALSE(IO::Path("A") > StringViewLite("BA"));
  EXPECT_FALSE(IO::Path("B") > StringViewLite("BA"));
  EXPECT_TRUE(IO::Path("C") > StringViewLite("BA"));
  EXPECT_TRUE(IO::Path("BA") > StringViewLite("A"));
  EXPECT_TRUE(IO::Path("BA") > StringViewLite("B"));
  EXPECT_FALSE(IO::Path("BA") > StringViewLite("C"));

  EXPECT_FALSE(IO::Path("AA") > StringViewLite("B"));
  EXPECT_TRUE(IO::Path("BA") > StringViewLite("B"));
  EXPECT_TRUE(IO::Path("CA") > StringViewLite("B"));
  EXPECT_TRUE(IO::Path("B") > StringViewLite("AA"));
  EXPECT_FALSE(IO::Path("B") > StringViewLite("BA"));
  EXPECT_FALSE(IO::Path("B") > StringViewLite("CA"));
}

TEST(TestIO_Path, OperatorGreaterThan_String_LHS)
{
  EXPECT_GT(std::string("B"), IO::Path("A"));
  EXPECT_GT(std::string("AB"), IO::Path("A"));
  EXPECT_GT(std::string("AB"), IO::Path(""));

  EXPECT_FALSE(std::string("A") > IO::Path("B"));
  EXPECT_FALSE(std::string("B") > IO::Path("B"));
  EXPECT_TRUE(std::string("C") > IO::Path("B"));
  EXPECT_TRUE(std::string("B") > IO::Path("A"));
  EXPECT_FALSE(std::string("B") > IO::Path("B"));
  EXPECT_FALSE(std::string("B") > IO::Path("C"));

  EXPECT_FALSE(std::string("A") > IO::Path("BA"));
  EXPECT_FALSE(std::string("B") > IO::Path("BA"));
  EXPECT_TRUE(std::string("C") > IO::Path("BA"));
  EXPECT_TRUE(std::string("BA") > IO::Path("A"));
  EXPECT_TRUE(std::string("BA") > IO::Path("B"));
  EXPECT_FALSE(std::string("BA") > IO::Path("C"));

  EXPECT_FALSE(std::string("AA") > IO::Path("B"));
  EXPECT_TRUE(std::string("BA") > IO::Path("B"));
  EXPECT_TRUE(std::string("CA") > IO::Path("B"));
  EXPECT_TRUE(std::string("B") > IO::Path("AA"));
  EXPECT_FALSE(std::string("B") > IO::Path("BA"));
  EXPECT_FALSE(std::string("B") > IO::Path("CA"));
}

TEST(TestIO_Path, OperatorGreaterThan_String_RHS)
{
  EXPECT_GT(IO::Path("B"), std::string("A"));
  EXPECT_GT(IO::Path("AB"), std::string("A"));
  EXPECT_GT(IO::Path("AB"), std::string(""));

  EXPECT_FALSE(IO::Path("A") > std::string("B"));
  EXPECT_FALSE(IO::Path("B") > std::string("B"));
  EXPECT_TRUE(IO::Path("C") > std::string("B"));
  EXPECT_TRUE(IO::Path("B") > std::string("A"));
  EXPECT_FALSE(IO::Path("B") > std::string("B"));
  EXPECT_FALSE(IO::Path("B") > std::string("C"));

  EXPECT_FALSE(IO::Path("A") > std::string("BA"));
  EXPECT_FALSE(IO::Path("B") > std::string("BA"));
  EXPECT_TRUE(IO::Path("C") > std::string("BA"));
  EXPECT_TRUE(IO::Path("BA") > std::string("A"));
  EXPECT_TRUE(IO::Path("BA") > std::string("B"));
  EXPECT_FALSE(IO::Path("BA") > std::string("C"));

  EXPECT_FALSE(IO::Path("AA") > std::string("B"));
  EXPECT_TRUE(IO::Path("BA") > std::string("B"));
  EXPECT_TRUE(IO::Path("CA") > std::string("B"));
  EXPECT_TRUE(IO::Path("B") > std::string("AA"));
  EXPECT_FALSE(IO::Path("B") > std::string("BA"));
  EXPECT_FALSE(IO::Path("B") > std::string("CA"));
}

TEST(TestIO_Path, OperatorGreaterThan_UTF8String_LHS)
{
  EXPECT_GT(UTF8String("B"), IO::Path("A"));
  EXPECT_GT(UTF8String("AB"), IO::Path("A"));
  EXPECT_GT(UTF8String("AB"), IO::Path(""));

  EXPECT_FALSE(UTF8String("A") > IO::Path("B"));
  EXPECT_FALSE(UTF8String("B") > IO::Path("B"));
  EXPECT_TRUE(UTF8String("C") > IO::Path("B"));
  EXPECT_TRUE(UTF8String("B") > IO::Path("A"));
  EXPECT_FALSE(UTF8String("B") > IO::Path("B"));
  EXPECT_FALSE(UTF8String("B") > IO::Path("C"));

  EXPECT_FALSE(UTF8String("A") > IO::Path("BA"));
  EXPECT_FALSE(UTF8String("B") > IO::Path("BA"));
  EXPECT_TRUE(UTF8String("C") > IO::Path("BA"));
  EXPECT_TRUE(UTF8String("BA") > IO::Path("A"));
  EXPECT_TRUE(UTF8String("BA") > IO::Path("B"));
  EXPECT_FALSE(UTF8String("BA") > IO::Path("C"));

  EXPECT_FALSE(UTF8String("AA") > IO::Path("B"));
  EXPECT_TRUE(UTF8String("BA") > IO::Path("B"));
  EXPECT_TRUE(UTF8String("CA") > IO::Path("B"));
  EXPECT_TRUE(UTF8String("B") > IO::Path("AA"));
  EXPECT_FALSE(UTF8String("B") > IO::Path("BA"));
  EXPECT_FALSE(UTF8String("B") > IO::Path("CA"));
}

TEST(TestIO_Path, OperatorGreaterThan_UTF8String_RHS)
{
  EXPECT_GT(IO::Path("B"), UTF8String("A"));
  EXPECT_GT(IO::Path("AB"), UTF8String("A"));
  EXPECT_GT(IO::Path("AB"), UTF8String(""));

  EXPECT_FALSE(IO::Path("A") > UTF8String("B"));
  EXPECT_FALSE(IO::Path("B") > UTF8String("B"));
  EXPECT_TRUE(IO::Path("C") > UTF8String("B"));
  EXPECT_TRUE(IO::Path("B") > UTF8String("A"));
  EXPECT_FALSE(IO::Path("B") > UTF8String("B"));
  EXPECT_FALSE(IO::Path("B") > UTF8String("C"));

  EXPECT_FALSE(IO::Path("A") > UTF8String("BA"));
  EXPECT_FALSE(IO::Path("B") > UTF8String("BA"));
  EXPECT_TRUE(IO::Path("C") > UTF8String("BA"));
  EXPECT_TRUE(IO::Path("BA") > UTF8String("A"));
  EXPECT_TRUE(IO::Path("BA") > UTF8String("B"));
  EXPECT_FALSE(IO::Path("BA") > UTF8String("C"));

  EXPECT_FALSE(IO::Path("AA") > UTF8String("B"));
  EXPECT_TRUE(IO::Path("BA") > UTF8String("B"));
  EXPECT_TRUE(IO::Path("CA") > UTF8String("B"));
  EXPECT_TRUE(IO::Path("B") > UTF8String("AA"));
  EXPECT_FALSE(IO::Path("B") > UTF8String("BA"));
  EXPECT_FALSE(IO::Path("B") > UTF8String("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestIO_Path, OperatorGreaterThanOrEqual)
{
  EXPECT_GE(IO::Path("B"), IO::Path("A"));
  EXPECT_GE(IO::Path("AB"), IO::Path("A"));
  EXPECT_GE(IO::Path("AB"), IO::Path(""));
  EXPECT_GE(IO::Path("A"), IO::Path("A"));

  EXPECT_FALSE(IO::Path("A") >= IO::Path("B"));
  EXPECT_TRUE(IO::Path("B") >= IO::Path("B"));
  EXPECT_TRUE(IO::Path("C") >= IO::Path("B"));
  EXPECT_TRUE(IO::Path("B") >= IO::Path("A"));
  EXPECT_TRUE(IO::Path("B") >= IO::Path("B"));
  EXPECT_FALSE(IO::Path("B") >= IO::Path("C"));

  EXPECT_FALSE(IO::Path("A") >= IO::Path("BA"));
  EXPECT_FALSE(IO::Path("B") >= IO::Path("BA"));
  EXPECT_TRUE(IO::Path("C") >= IO::Path("BA"));
  EXPECT_TRUE(IO::Path("BA") >= IO::Path("A"));
  EXPECT_TRUE(IO::Path("BA") >= IO::Path("B"));
  EXPECT_FALSE(IO::Path("AB") >= IO::Path("C"));

  EXPECT_FALSE(IO::Path("AA") >= IO::Path("B"));
  EXPECT_TRUE(IO::Path("BA") >= IO::Path("B"));
  EXPECT_TRUE(IO::Path("CA") >= IO::Path("B"));
  EXPECT_TRUE(IO::Path("B") >= IO::Path("AA"));
  EXPECT_FALSE(IO::Path("B") >= IO::Path("BA"));
  EXPECT_FALSE(IO::Path("B") >= IO::Path("CA"));
}

TEST(TestIO_Path, OperatorGreaterThanOrEqual_CString_LHS)
{
  EXPECT_GE("B", IO::Path("A"));
  EXPECT_GE("AB", IO::Path("A"));
  EXPECT_GE("AB", IO::Path(""));
  EXPECT_GE("A", IO::Path("A"));

  EXPECT_FALSE("A" >= IO::Path("B"));
  EXPECT_TRUE("B" >= IO::Path("B"));
  EXPECT_TRUE("C" >= IO::Path("B"));
  EXPECT_TRUE("B" >= IO::Path("A"));
  EXPECT_TRUE("B" >= IO::Path("B"));
  EXPECT_FALSE("B" >= IO::Path("C"));

  EXPECT_FALSE("A" >= IO::Path("BA"));
  EXPECT_FALSE("B" >= IO::Path("BA"));
  EXPECT_TRUE("C" >= IO::Path("BA"));
  EXPECT_TRUE("BA" >= IO::Path("A"));
  EXPECT_TRUE("BA" >= IO::Path("B"));
  EXPECT_FALSE("BA" >= IO::Path("C"));

  EXPECT_FALSE("AA" >= IO::Path("B"));
  EXPECT_TRUE("BA" >= IO::Path("B"));
  EXPECT_TRUE("CA" >= IO::Path("B"));
  EXPECT_TRUE("B" >= IO::Path("AA"));
  EXPECT_FALSE("B" >= IO::Path("BA"));
  EXPECT_FALSE("B" >= IO::Path("CA"));
}

TEST(TestIO_Path, OperatorGreaterThanOrEqual_CString_RHS)
{
  EXPECT_GE(IO::Path("B"), "A");
  EXPECT_GE(IO::Path("AB"), "A");
  EXPECT_GE(IO::Path("AB"), "");
  EXPECT_GE(IO::Path("A"), "A");

  EXPECT_FALSE(IO::Path("A") >= "B");
  EXPECT_TRUE(IO::Path("B") >= "B");
  EXPECT_TRUE(IO::Path("C") >= "B");
  EXPECT_TRUE(IO::Path("B") >= "A");
  EXPECT_TRUE(IO::Path("B") >= "B");
  EXPECT_FALSE(IO::Path("B") >= "C");

  EXPECT_FALSE(IO::Path("A") >= "BA");
  EXPECT_FALSE(IO::Path("B") >= "BA");
  EXPECT_TRUE(IO::Path("C") >= "BA");
  EXPECT_TRUE(IO::Path("BA") >= "A");
  EXPECT_TRUE(IO::Path("BA") >= "B");
  EXPECT_FALSE(IO::Path("BA") >= "C");

  EXPECT_FALSE(IO::Path("AA") >= "B");
  EXPECT_TRUE(IO::Path("BA") >= "B");
  EXPECT_TRUE(IO::Path("CA") >= "B");
  EXPECT_TRUE(IO::Path("B") >= "AA");
  EXPECT_FALSE(IO::Path("B") >= "BA");
  EXPECT_FALSE(IO::Path("B") >= "CA");
}

TEST(TestIO_Path, OperatorGreaterThanOrEqual_StringViewLite_LHS)
{
  EXPECT_GE(StringViewLite("B"), IO::Path("A"));
  EXPECT_GE(StringViewLite("AB"), IO::Path("A"));
  EXPECT_GE(StringViewLite("AB"), IO::Path(""));
  EXPECT_GE(StringViewLite("A"), IO::Path("A"));

  EXPECT_FALSE(StringViewLite("A") >= IO::Path("B"));
  EXPECT_TRUE(StringViewLite("B") >= IO::Path("B"));
  EXPECT_TRUE(StringViewLite("C") >= IO::Path("B"));
  EXPECT_TRUE(StringViewLite("B") >= IO::Path("A"));
  EXPECT_TRUE(StringViewLite("B") >= IO::Path("B"));
  EXPECT_FALSE(StringViewLite("B") >= IO::Path("C"));

  EXPECT_FALSE(StringViewLite("A") >= IO::Path("BA"));
  EXPECT_FALSE(StringViewLite("B") >= IO::Path("BA"));
  EXPECT_TRUE(StringViewLite("C") >= IO::Path("BA"));
  EXPECT_TRUE(StringViewLite("BA") >= IO::Path("A"));
  EXPECT_TRUE(StringViewLite("BA") >= IO::Path("B"));
  EXPECT_FALSE(StringViewLite("BA") >= IO::Path("C"));

  EXPECT_FALSE(StringViewLite("AA") >= IO::Path("B"));
  EXPECT_TRUE(StringViewLite("BA") >= IO::Path("B"));
  EXPECT_TRUE(StringViewLite("CA") >= IO::Path("B"));
  EXPECT_TRUE(StringViewLite("B") >= IO::Path("AA"));
  EXPECT_FALSE(StringViewLite("B") >= IO::Path("BA"));
  EXPECT_FALSE(StringViewLite("B") >= IO::Path("CA"));
}

TEST(TestIO_Path, OperatorGreaterThanOrEqual_StringViewLite_RHS)
{
  EXPECT_GE(IO::Path("B"), StringViewLite("A"));
  EXPECT_GE(IO::Path("AB"), StringViewLite("A"));
  EXPECT_GE(IO::Path("AB"), StringViewLite(""));
  EXPECT_GE(IO::Path("A"), StringViewLite("A"));

  EXPECT_FALSE(IO::Path("A") >= StringViewLite("B"));
  EXPECT_TRUE(IO::Path("B") >= StringViewLite("B"));
  EXPECT_TRUE(IO::Path("C") >= StringViewLite("B"));
  EXPECT_TRUE(IO::Path("B") >= StringViewLite("A"));
  EXPECT_TRUE(IO::Path("B") >= StringViewLite("B"));
  EXPECT_FALSE(IO::Path("B") >= StringViewLite("C"));

  EXPECT_FALSE(IO::Path("A") >= StringViewLite("BA"));
  EXPECT_FALSE(IO::Path("B") >= StringViewLite("BA"));
  EXPECT_TRUE(IO::Path("C") >= StringViewLite("BA"));
  EXPECT_TRUE(IO::Path("BA") >= StringViewLite("A"));
  EXPECT_TRUE(IO::Path("BA") >= StringViewLite("B"));
  EXPECT_FALSE(IO::Path("BA") >= StringViewLite("C"));

  EXPECT_FALSE(IO::Path("AA") >= StringViewLite("B"));
  EXPECT_TRUE(IO::Path("BA") >= StringViewLite("B"));
  EXPECT_TRUE(IO::Path("CA") >= StringViewLite("B"));
  EXPECT_TRUE(IO::Path("B") >= StringViewLite("AA"));
  EXPECT_FALSE(IO::Path("B") >= StringViewLite("BA"));
  EXPECT_FALSE(IO::Path("B") >= StringViewLite("CA"));
}

TEST(TestIO_Path, OperatorGreaterThanOrEqual_String_LHS)
{
  EXPECT_GE(std::string("B"), IO::Path("A"));
  EXPECT_GE(std::string("AB"), IO::Path("A"));
  EXPECT_GE(std::string("AB"), IO::Path(""));
  EXPECT_GE(std::string("A"), IO::Path("A"));

  EXPECT_FALSE(std::string("A") >= IO::Path("B"));
  EXPECT_TRUE(std::string("B") >= IO::Path("B"));
  EXPECT_TRUE(std::string("C") >= IO::Path("B"));
  EXPECT_TRUE(std::string("B") >= IO::Path("A"));
  EXPECT_TRUE(std::string("B") >= IO::Path("B"));
  EXPECT_FALSE(std::string("B") >= IO::Path("C"));

  EXPECT_FALSE(std::string("A") >= IO::Path("BA"));
  EXPECT_FALSE(std::string("B") >= IO::Path("BA"));
  EXPECT_TRUE(std::string("C") >= IO::Path("BA"));
  EXPECT_TRUE(std::string("BA") >= IO::Path("A"));
  EXPECT_TRUE(std::string("BA") >= IO::Path("B"));
  EXPECT_FALSE(std::string("BA") >= IO::Path("C"));

  EXPECT_FALSE(std::string("AA") >= IO::Path("B"));
  EXPECT_TRUE(std::string("BA") >= IO::Path("B"));
  EXPECT_TRUE(std::string("CA") >= IO::Path("B"));
  EXPECT_TRUE(std::string("B") >= IO::Path("AA"));
  EXPECT_FALSE(std::string("B") >= IO::Path("BA"));
  EXPECT_FALSE(std::string("B") >= IO::Path("CA"));
}

TEST(TestIO_Path, OperatorGreaterThanOrEqual_String_RHS)
{
  EXPECT_GE(IO::Path("B"), std::string("A"));
  EXPECT_GE(IO::Path("AB"), std::string("A"));
  EXPECT_GE(IO::Path("AB"), std::string(""));
  EXPECT_GE(IO::Path("A"), std::string("A"));

  EXPECT_FALSE(IO::Path("A") >= std::string("B"));
  EXPECT_TRUE(IO::Path("B") >= std::string("B"));
  EXPECT_TRUE(IO::Path("C") >= std::string("B"));
  EXPECT_TRUE(IO::Path("B") >= std::string("A"));
  EXPECT_TRUE(IO::Path("B") >= std::string("B"));
  EXPECT_FALSE(IO::Path("B") >= std::string("C"));

  EXPECT_FALSE(IO::Path("A") >= std::string("BA"));
  EXPECT_FALSE(IO::Path("B") >= std::string("BA"));
  EXPECT_TRUE(IO::Path("C") >= std::string("BA"));
  EXPECT_TRUE(IO::Path("BA") >= std::string("A"));
  EXPECT_TRUE(IO::Path("BA") >= std::string("B"));
  EXPECT_FALSE(IO::Path("BA") >= std::string("C"));

  EXPECT_FALSE(IO::Path("AA") >= std::string("B"));
  EXPECT_TRUE(IO::Path("BA") >= std::string("B"));
  EXPECT_TRUE(IO::Path("CA") >= std::string("B"));
  EXPECT_TRUE(IO::Path("B") >= std::string("AA"));
  EXPECT_FALSE(IO::Path("B") >= std::string("BA"));
  EXPECT_FALSE(IO::Path("B") >= std::string("CA"));
}

TEST(TestIO_Path, OperatorGreaterThanOrEqual_UTF8String_LHS)
{
  EXPECT_GE(UTF8String("B"), IO::Path("A"));
  EXPECT_GE(UTF8String("AB"), IO::Path("A"));
  EXPECT_GE(UTF8String("AB"), IO::Path(""));
  EXPECT_GE(UTF8String("A"), IO::Path("A"));

  EXPECT_FALSE(UTF8String("A") >= IO::Path("B"));
  EXPECT_TRUE(UTF8String("B") >= IO::Path("B"));
  EXPECT_TRUE(UTF8String("C") >= IO::Path("B"));
  EXPECT_TRUE(UTF8String("B") >= IO::Path("A"));
  EXPECT_TRUE(UTF8String("B") >= IO::Path("B"));
  EXPECT_FALSE(UTF8String("B") >= IO::Path("C"));

  EXPECT_FALSE(UTF8String("A") >= IO::Path("BA"));
  EXPECT_FALSE(UTF8String("B") >= IO::Path("BA"));
  EXPECT_TRUE(UTF8String("C") >= IO::Path("BA"));
  EXPECT_TRUE(UTF8String("BA") >= IO::Path("A"));
  EXPECT_TRUE(UTF8String("BA") >= IO::Path("B"));
  EXPECT_FALSE(UTF8String("BA") >= IO::Path("C"));

  EXPECT_FALSE(UTF8String("AA") >= IO::Path("B"));
  EXPECT_TRUE(UTF8String("BA") >= IO::Path("B"));
  EXPECT_TRUE(UTF8String("CA") >= IO::Path("B"));
  EXPECT_TRUE(UTF8String("B") >= IO::Path("AA"));
  EXPECT_FALSE(UTF8String("B") >= IO::Path("BA"));
  EXPECT_FALSE(UTF8String("B") >= IO::Path("CA"));
}

TEST(TestIO_Path, OperatorGreaterThanOrEqual_UTF8String_RHS)
{
  EXPECT_GE(IO::Path("B"), UTF8String("A"));
  EXPECT_GE(IO::Path("AB"), UTF8String("A"));
  EXPECT_GE(IO::Path("AB"), UTF8String(""));
  EXPECT_GE(IO::Path("A"), UTF8String("A"));

  EXPECT_FALSE(IO::Path("A") >= UTF8String("B"));
  EXPECT_TRUE(IO::Path("B") >= UTF8String("B"));
  EXPECT_TRUE(IO::Path("C") >= UTF8String("B"));
  EXPECT_TRUE(IO::Path("B") >= UTF8String("A"));
  EXPECT_TRUE(IO::Path("B") >= UTF8String("B"));
  EXPECT_FALSE(IO::Path("B") >= UTF8String("C"));

  EXPECT_FALSE(IO::Path("A") >= UTF8String("BA"));
  EXPECT_FALSE(IO::Path("B") >= UTF8String("BA"));
  EXPECT_TRUE(IO::Path("C") >= UTF8String("BA"));
  EXPECT_TRUE(IO::Path("BA") >= UTF8String("A"));
  EXPECT_TRUE(IO::Path("BA") >= UTF8String("B"));
  EXPECT_FALSE(IO::Path("BA") >= UTF8String("C"));

  EXPECT_FALSE(IO::Path("AA") >= UTF8String("B"));
  EXPECT_TRUE(IO::Path("BA") >= UTF8String("B"));
  EXPECT_TRUE(IO::Path("CA") >= UTF8String("B"));
  EXPECT_TRUE(IO::Path("B") >= UTF8String("AA"));
  EXPECT_FALSE(IO::Path("B") >= UTF8String("BA"));
  EXPECT_FALSE(IO::Path("B") >= UTF8String("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator ==
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestIO_Path, OperatorEqual)
{
  EXPECT_TRUE(IO::Path("A") == IO::Path("A"));
  EXPECT_TRUE(IO::Path("") == IO::Path(""));
  EXPECT_FALSE(IO::Path("A") == IO::Path(""));
  EXPECT_FALSE(IO::Path("A") == IO::Path("B"));
  EXPECT_FALSE(IO::Path("A") == IO::Path("a"));

  EXPECT_FALSE(IO::Path("A") == IO::Path("B"));
  EXPECT_TRUE(IO::Path("B") == IO::Path("B"));
  EXPECT_FALSE(IO::Path("C") == IO::Path("B"));
  EXPECT_FALSE(IO::Path("B") == IO::Path("A"));
  EXPECT_TRUE(IO::Path("B") == IO::Path("B"));
  EXPECT_FALSE(IO::Path("B") == IO::Path("C"));

  EXPECT_FALSE(IO::Path("A") == IO::Path("BA"));
  EXPECT_FALSE(IO::Path("B") == IO::Path("BA"));
  EXPECT_FALSE(IO::Path("C") == IO::Path("BA"));
  EXPECT_FALSE(IO::Path("BA") == IO::Path("A"));
  EXPECT_FALSE(IO::Path("BA") == IO::Path("B"));
  EXPECT_FALSE(IO::Path("BA") == IO::Path("C"));

  EXPECT_FALSE(IO::Path("AA") == IO::Path("B"));
  EXPECT_FALSE(IO::Path("BA") == IO::Path("B"));
  EXPECT_FALSE(IO::Path("CA") == IO::Path("B"));
  EXPECT_FALSE(IO::Path("B") == IO::Path("AA"));
  EXPECT_FALSE(IO::Path("B") == IO::Path("BA"));
  EXPECT_FALSE(IO::Path("B") == IO::Path("CA"));
}


TEST(TestIO_Path, OperatorEqual_CString_LHS)
{
  EXPECT_TRUE("A" == IO::Path("A"));
  EXPECT_TRUE("" == IO::Path(""));
  EXPECT_FALSE("A" == IO::Path(""));
  EXPECT_FALSE("A" == IO::Path("B"));
  EXPECT_FALSE("A" == IO::Path("a"));

  EXPECT_FALSE("A" == IO::Path("B"));
  EXPECT_TRUE("B" == IO::Path("B"));
  EXPECT_FALSE("C" == IO::Path("B"));
  EXPECT_FALSE("B" == IO::Path("A"));
  EXPECT_TRUE("B" == IO::Path("B"));
  EXPECT_FALSE("B" == IO::Path("C"));

  EXPECT_FALSE("A" == IO::Path("BA"));
  EXPECT_FALSE("B" == IO::Path("BA"));
  EXPECT_FALSE("C" == IO::Path("BA"));
  EXPECT_FALSE("BA" == IO::Path("A"));
  EXPECT_FALSE("BA" == IO::Path("B"));
  EXPECT_FALSE("BA" == IO::Path("C"));

  EXPECT_FALSE("AA" == IO::Path("B"));
  EXPECT_FALSE("BA" == IO::Path("B"));
  EXPECT_FALSE("CA" == IO::Path("B"));
  EXPECT_FALSE("B" == IO::Path("AA"));
  EXPECT_FALSE("B" == IO::Path("BA"));
  EXPECT_FALSE("B" == IO::Path("CA"));
}


TEST(TestIO_Path, OperatorEqual_CString_RHS)
{
  EXPECT_TRUE(IO::Path("A") == "A");
  EXPECT_TRUE(IO::Path("") == "");
  EXPECT_FALSE(IO::Path("A") == "");
  EXPECT_FALSE(IO::Path("A") == "B");
  EXPECT_FALSE(IO::Path("A") == "a");

  EXPECT_FALSE(IO::Path("A") == "B");
  EXPECT_TRUE(IO::Path("B") == "B");
  EXPECT_FALSE(IO::Path("C") == "B");
  EXPECT_FALSE(IO::Path("B") == "A");
  EXPECT_TRUE(IO::Path("B") == "B");
  EXPECT_FALSE(IO::Path("B") == "C");

  EXPECT_FALSE(IO::Path("A") == "BA");
  EXPECT_FALSE(IO::Path("B") == "BA");
  EXPECT_FALSE(IO::Path("C") == "BA");
  EXPECT_FALSE(IO::Path("BA") == "A");
  EXPECT_FALSE(IO::Path("BA") == "B");
  EXPECT_FALSE(IO::Path("BA") == "C");

  EXPECT_FALSE(IO::Path("AA") == "B");
  EXPECT_FALSE(IO::Path("BA") == "B");
  EXPECT_FALSE(IO::Path("CA") == "B");
  EXPECT_FALSE(IO::Path("B") == "AA");
  EXPECT_FALSE(IO::Path("B") == "BA");
  EXPECT_FALSE(IO::Path("B") == "CA");
}

TEST(TestIO_Path, OperatorEqual_StringViewLite_LHS)
{
  EXPECT_TRUE(StringViewLite("A") == IO::Path("A"));
  EXPECT_TRUE(StringViewLite("") == IO::Path(""));
  EXPECT_FALSE(StringViewLite("A") == IO::Path(""));
  EXPECT_FALSE(StringViewLite("A") == IO::Path("B"));
  EXPECT_FALSE(StringViewLite("A") == IO::Path("a"));

  EXPECT_FALSE(StringViewLite("A") == IO::Path("B"));
  EXPECT_TRUE(StringViewLite("B") == IO::Path("B"));
  EXPECT_FALSE(StringViewLite("C") == IO::Path("B"));
  EXPECT_FALSE(StringViewLite("B") == IO::Path("A"));
  EXPECT_TRUE(StringViewLite("B") == IO::Path("B"));
  EXPECT_FALSE(StringViewLite("B") == IO::Path("C"));

  EXPECT_FALSE(StringViewLite("A") == IO::Path("BA"));
  EXPECT_FALSE(StringViewLite("B") == IO::Path("BA"));
  EXPECT_FALSE(StringViewLite("C") == IO::Path("BA"));
  EXPECT_FALSE(StringViewLite("BA") == IO::Path("A"));
  EXPECT_FALSE(StringViewLite("BA") == IO::Path("B"));
  EXPECT_FALSE(StringViewLite("BA") == IO::Path("C"));

  EXPECT_FALSE(StringViewLite("AA") == IO::Path("B"));
  EXPECT_FALSE(StringViewLite("BA") == IO::Path("B"));
  EXPECT_FALSE(StringViewLite("CA") == IO::Path("B"));
  EXPECT_FALSE(StringViewLite("B") == IO::Path("AA"));
  EXPECT_FALSE(StringViewLite("B") == IO::Path("BA"));
  EXPECT_FALSE(StringViewLite("B") == IO::Path("CA"));
}


TEST(TestIO_Path, OperatorEqual_StringViewLite_RHS)
{
  EXPECT_TRUE(IO::Path("A") == StringViewLite("A"));
  EXPECT_TRUE(IO::Path("") == StringViewLite(""));
  EXPECT_FALSE(IO::Path("A") == StringViewLite(""));
  EXPECT_FALSE(IO::Path("A") == StringViewLite("B"));
  EXPECT_FALSE(IO::Path("A") == StringViewLite("a"));

  EXPECT_FALSE(IO::Path("A") == StringViewLite("B"));
  EXPECT_TRUE(IO::Path("B") == StringViewLite("B"));
  EXPECT_FALSE(IO::Path("C") == StringViewLite("B"));
  EXPECT_FALSE(IO::Path("B") == StringViewLite("A"));
  EXPECT_TRUE(IO::Path("B") == StringViewLite("B"));
  EXPECT_FALSE(IO::Path("B") == StringViewLite("C"));

  EXPECT_FALSE(IO::Path("A") == StringViewLite("BA"));
  EXPECT_FALSE(IO::Path("B") == StringViewLite("BA"));
  EXPECT_FALSE(IO::Path("C") == StringViewLite("BA"));
  EXPECT_FALSE(IO::Path("BA") == StringViewLite("A"));
  EXPECT_FALSE(IO::Path("BA") == StringViewLite("B"));
  EXPECT_FALSE(IO::Path("BA") == StringViewLite("C"));

  EXPECT_FALSE(IO::Path("AA") == StringViewLite("B"));
  EXPECT_FALSE(IO::Path("BA") == StringViewLite("B"));
  EXPECT_FALSE(IO::Path("CA") == StringViewLite("B"));
  EXPECT_FALSE(IO::Path("B") == StringViewLite("AA"));
  EXPECT_FALSE(IO::Path("B") == StringViewLite("BA"));
  EXPECT_FALSE(IO::Path("B") == StringViewLite("CA"));
}

TEST(TestIO_Path, OperatorEqual_String_LHS)
{
  EXPECT_TRUE(std::string("A") == IO::Path("A"));
  EXPECT_TRUE(std::string("") == IO::Path(""));
  EXPECT_FALSE(std::string("A") == IO::Path(""));
  EXPECT_FALSE(std::string("A") == IO::Path("B"));
  EXPECT_FALSE(std::string("A") == IO::Path("a"));

  EXPECT_FALSE(std::string("A") == IO::Path("B"));
  EXPECT_TRUE(std::string("B") == IO::Path("B"));
  EXPECT_FALSE(std::string("C") == IO::Path("B"));
  EXPECT_FALSE(std::string("B") == IO::Path("A"));
  EXPECT_TRUE(std::string("B") == IO::Path("B"));
  EXPECT_FALSE(std::string("B") == IO::Path("C"));

  EXPECT_FALSE(std::string("A") == IO::Path("BA"));
  EXPECT_FALSE(std::string("B") == IO::Path("BA"));
  EXPECT_FALSE(std::string("C") == IO::Path("BA"));
  EXPECT_FALSE(std::string("BA") == IO::Path("A"));
  EXPECT_FALSE(std::string("BA") == IO::Path("B"));
  EXPECT_FALSE(std::string("BA") == IO::Path("C"));

  EXPECT_FALSE(std::string("AA") == IO::Path("B"));
  EXPECT_FALSE(std::string("BA") == IO::Path("B"));
  EXPECT_FALSE(std::string("CA") == IO::Path("B"));
  EXPECT_FALSE(std::string("B") == IO::Path("AA"));
  EXPECT_FALSE(std::string("B") == IO::Path("BA"));
  EXPECT_FALSE(std::string("B") == IO::Path("CA"));
}


TEST(TestIO_Path, OperatorEqual_String_RHS)
{
  EXPECT_TRUE(IO::Path("A") == std::string("A"));
  EXPECT_TRUE(IO::Path("") == std::string(""));
  EXPECT_FALSE(IO::Path("A") == std::string(""));
  EXPECT_FALSE(IO::Path("A") == std::string("B"));
  EXPECT_FALSE(IO::Path("A") == std::string("a"));

  EXPECT_FALSE(IO::Path("A") == std::string("B"));
  EXPECT_TRUE(IO::Path("B") == std::string("B"));
  EXPECT_FALSE(IO::Path("C") == std::string("B"));
  EXPECT_FALSE(IO::Path("B") == std::string("A"));
  EXPECT_TRUE(IO::Path("B") == std::string("B"));
  EXPECT_FALSE(IO::Path("B") == std::string("C"));

  EXPECT_FALSE(IO::Path("A") == std::string("BA"));
  EXPECT_FALSE(IO::Path("B") == std::string("BA"));
  EXPECT_FALSE(IO::Path("C") == std::string("BA"));
  EXPECT_FALSE(IO::Path("BA") == std::string("A"));
  EXPECT_FALSE(IO::Path("BA") == std::string("B"));
  EXPECT_FALSE(IO::Path("BA") == std::string("C"));

  EXPECT_FALSE(IO::Path("AA") == std::string("B"));
  EXPECT_FALSE(IO::Path("BA") == std::string("B"));
  EXPECT_FALSE(IO::Path("CA") == std::string("B"));
  EXPECT_FALSE(IO::Path("B") == std::string("AA"));
  EXPECT_FALSE(IO::Path("B") == std::string("BA"));
  EXPECT_FALSE(IO::Path("B") == std::string("CA"));
}


TEST(TestIO_Path, OperatorEqual_UTF8String_LHS)
{
  EXPECT_TRUE(UTF8String("A") == IO::Path("A"));
  EXPECT_TRUE(UTF8String("") == IO::Path(""));
  EXPECT_FALSE(UTF8String("A") == IO::Path(""));
  EXPECT_FALSE(UTF8String("A") == IO::Path("B"));
  EXPECT_FALSE(UTF8String("A") == IO::Path("a"));

  EXPECT_FALSE(UTF8String("A") == IO::Path("B"));
  EXPECT_TRUE(UTF8String("B") == IO::Path("B"));
  EXPECT_FALSE(UTF8String("C") == IO::Path("B"));
  EXPECT_FALSE(UTF8String("B") == IO::Path("A"));
  EXPECT_TRUE(UTF8String("B") == IO::Path("B"));
  EXPECT_FALSE(UTF8String("B") == IO::Path("C"));

  EXPECT_FALSE(UTF8String("A") == IO::Path("BA"));
  EXPECT_FALSE(UTF8String("B") == IO::Path("BA"));
  EXPECT_FALSE(UTF8String("C") == IO::Path("BA"));
  EXPECT_FALSE(UTF8String("BA") == IO::Path("A"));
  EXPECT_FALSE(UTF8String("BA") == IO::Path("B"));
  EXPECT_FALSE(UTF8String("BA") == IO::Path("C"));

  EXPECT_FALSE(UTF8String("AA") == IO::Path("B"));
  EXPECT_FALSE(UTF8String("BA") == IO::Path("B"));
  EXPECT_FALSE(UTF8String("CA") == IO::Path("B"));
  EXPECT_FALSE(UTF8String("B") == IO::Path("AA"));
  EXPECT_FALSE(UTF8String("B") == IO::Path("BA"));
  EXPECT_FALSE(UTF8String("B") == IO::Path("CA"));
}


TEST(TestIO_Path, OperatorEqual_UTF8String_RHS)
{
  EXPECT_TRUE(IO::Path("A") == UTF8String("A"));
  EXPECT_TRUE(IO::Path("") == UTF8String(""));
  EXPECT_FALSE(IO::Path("A") == UTF8String(""));
  EXPECT_FALSE(IO::Path("A") == UTF8String("B"));
  EXPECT_FALSE(IO::Path("A") == UTF8String("a"));

  EXPECT_FALSE(IO::Path("A") == UTF8String("B"));
  EXPECT_TRUE(IO::Path("B") == UTF8String("B"));
  EXPECT_FALSE(IO::Path("C") == UTF8String("B"));
  EXPECT_FALSE(IO::Path("B") == UTF8String("A"));
  EXPECT_TRUE(IO::Path("B") == UTF8String("B"));
  EXPECT_FALSE(IO::Path("B") == UTF8String("C"));

  EXPECT_FALSE(IO::Path("A") == UTF8String("BA"));
  EXPECT_FALSE(IO::Path("B") == UTF8String("BA"));
  EXPECT_FALSE(IO::Path("C") == UTF8String("BA"));
  EXPECT_FALSE(IO::Path("BA") == UTF8String("A"));
  EXPECT_FALSE(IO::Path("BA") == UTF8String("B"));
  EXPECT_FALSE(IO::Path("BA") == UTF8String("C"));

  EXPECT_FALSE(IO::Path("AA") == UTF8String("B"));
  EXPECT_FALSE(IO::Path("BA") == UTF8String("B"));
  EXPECT_FALSE(IO::Path("CA") == UTF8String("B"));
  EXPECT_FALSE(IO::Path("B") == UTF8String("AA"));
  EXPECT_FALSE(IO::Path("B") == UTF8String("BA"));
  EXPECT_FALSE(IO::Path("B") == UTF8String("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator !=
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestIO_Path, OperatorNotEqual)
{
  EXPECT_FALSE(IO::Path("A") != IO::Path("A"));
  EXPECT_FALSE(IO::Path("") != IO::Path(""));
  EXPECT_TRUE(IO::Path("A") != IO::Path(""));
  EXPECT_TRUE(IO::Path("A") != IO::Path("B"));
  EXPECT_TRUE(IO::Path("A") != IO::Path("a"));

  EXPECT_TRUE(IO::Path("A") != IO::Path("B"));
  EXPECT_FALSE(IO::Path("B") != IO::Path("B"));
  EXPECT_TRUE(IO::Path("C") != IO::Path("B"));
  EXPECT_TRUE(IO::Path("B") != IO::Path("A"));
  EXPECT_FALSE(IO::Path("B") != IO::Path("B"));
  EXPECT_TRUE(IO::Path("B") != IO::Path("C"));

  EXPECT_TRUE(IO::Path("A") != IO::Path("BA"));
  EXPECT_TRUE(IO::Path("B") != IO::Path("BA"));
  EXPECT_TRUE(IO::Path("C") != IO::Path("BA"));
  EXPECT_TRUE(IO::Path("BA") != IO::Path("A"));
  EXPECT_TRUE(IO::Path("BA") != IO::Path("B"));
  EXPECT_TRUE(IO::Path("BA") != IO::Path("C"));

  EXPECT_TRUE(IO::Path("AA") != IO::Path("B"));
  EXPECT_TRUE(IO::Path("BA") != IO::Path("B"));
  EXPECT_TRUE(IO::Path("CA") != IO::Path("B"));
  EXPECT_TRUE(IO::Path("B") != IO::Path("AA"));
  EXPECT_TRUE(IO::Path("B") != IO::Path("BA"));
  EXPECT_TRUE(IO::Path("B") != IO::Path("CA"));
}


TEST(TestIO_Path, OperatorNotEqual_CString_LHS)
{
  EXPECT_FALSE("A" != IO::Path("A"));
  EXPECT_FALSE("" != IO::Path(""));
  EXPECT_TRUE("A" != IO::Path(""));
  EXPECT_TRUE("A" != IO::Path("B"));
  EXPECT_TRUE("A" != IO::Path("a"));

  EXPECT_TRUE("A" != IO::Path("B"));
  EXPECT_FALSE("B" != IO::Path("B"));
  EXPECT_TRUE("C" != IO::Path("B"));
  EXPECT_TRUE("B" != IO::Path("A"));
  EXPECT_FALSE("B" != IO::Path("B"));
  EXPECT_TRUE("B" != IO::Path("C"));

  EXPECT_TRUE("A" != IO::Path("BA"));
  EXPECT_TRUE("B" != IO::Path("BA"));
  EXPECT_TRUE("C" != IO::Path("BA"));
  EXPECT_TRUE("BA" != IO::Path("A"));
  EXPECT_TRUE("BA" != IO::Path("B"));
  EXPECT_TRUE("BA" != IO::Path("C"));

  EXPECT_TRUE("AA" != IO::Path("B"));
  EXPECT_TRUE("BA" != IO::Path("B"));
  EXPECT_TRUE("CA" != IO::Path("B"));
  EXPECT_TRUE("B" != IO::Path("AA"));
  EXPECT_TRUE("B" != IO::Path("BA"));
  EXPECT_TRUE("B" != IO::Path("CA"));
}


TEST(TestIO_Path, OperatorNotEqual_CString_RHS)
{
  EXPECT_FALSE(IO::Path("A") != "A");
  EXPECT_FALSE(IO::Path("") != "");
  EXPECT_TRUE(IO::Path("A") != "");
  EXPECT_TRUE(IO::Path("A") != "B");
  EXPECT_TRUE(IO::Path("A") != "a");

  EXPECT_TRUE(IO::Path("A") != "B");
  EXPECT_FALSE(IO::Path("B") != "B");
  EXPECT_TRUE(IO::Path("C") != "B");
  EXPECT_TRUE(IO::Path("B") != "A");
  EXPECT_FALSE(IO::Path("B") != "B");
  EXPECT_TRUE(IO::Path("B") != "C");

  EXPECT_TRUE(IO::Path("A") != "BA");
  EXPECT_TRUE(IO::Path("B") != "BA");
  EXPECT_TRUE(IO::Path("C") != "BA");
  EXPECT_TRUE(IO::Path("BA") != "A");
  EXPECT_TRUE(IO::Path("BA") != "B");
  EXPECT_TRUE(IO::Path("BA") != "C");

  EXPECT_TRUE(IO::Path("AA") != "B");
  EXPECT_TRUE(IO::Path("BA") != "B");
  EXPECT_TRUE(IO::Path("CA") != "B");
  EXPECT_TRUE(IO::Path("B") != "AA");
  EXPECT_TRUE(IO::Path("B") != "BA");
  EXPECT_TRUE(IO::Path("B") != "CA");
}


TEST(TestIO_Path, OperatorNotEqual_StringViewLite_LHS)
{
  EXPECT_FALSE(StringViewLite("A") != IO::Path("A"));
  EXPECT_FALSE(StringViewLite("") != IO::Path(""));
  EXPECT_TRUE(StringViewLite("A") != IO::Path(""));
  EXPECT_TRUE(StringViewLite("A") != IO::Path("B"));
  EXPECT_TRUE(StringViewLite("A") != IO::Path("a"));

  EXPECT_TRUE(StringViewLite("A") != IO::Path("B"));
  EXPECT_FALSE(StringViewLite("B") != IO::Path("B"));
  EXPECT_TRUE(StringViewLite("C") != IO::Path("B"));
  EXPECT_TRUE(StringViewLite("B") != IO::Path("A"));
  EXPECT_FALSE(StringViewLite("B") != IO::Path("B"));
  EXPECT_TRUE(StringViewLite("B") != IO::Path("C"));

  EXPECT_TRUE(StringViewLite("A") != IO::Path("BA"));
  EXPECT_TRUE(StringViewLite("B") != IO::Path("BA"));
  EXPECT_TRUE(StringViewLite("C") != IO::Path("BA"));
  EXPECT_TRUE(StringViewLite("BA") != IO::Path("A"));
  EXPECT_TRUE(StringViewLite("BA") != IO::Path("B"));
  EXPECT_TRUE(StringViewLite("BA") != IO::Path("C"));

  EXPECT_TRUE(StringViewLite("AA") != IO::Path("B"));
  EXPECT_TRUE(StringViewLite("BA") != IO::Path("B"));
  EXPECT_TRUE(StringViewLite("CA") != IO::Path("B"));
  EXPECT_TRUE(StringViewLite("B") != IO::Path("AA"));
  EXPECT_TRUE(StringViewLite("B") != IO::Path("BA"));
  EXPECT_TRUE(StringViewLite("B") != IO::Path("CA"));
}


TEST(TestIO_Path, OperatorNotEqual_StringViewLite_RHS)
{
  EXPECT_FALSE(IO::Path("A") != StringViewLite("A"));
  EXPECT_FALSE(IO::Path("") != StringViewLite(""));
  EXPECT_TRUE(IO::Path("A") != StringViewLite(""));
  EXPECT_TRUE(IO::Path("A") != StringViewLite("B"));
  EXPECT_TRUE(IO::Path("A") != StringViewLite("a"));

  EXPECT_TRUE(IO::Path("A") != StringViewLite("B"));
  EXPECT_FALSE(IO::Path("B") != StringViewLite("B"));
  EXPECT_TRUE(IO::Path("C") != StringViewLite("B"));
  EXPECT_TRUE(IO::Path("B") != StringViewLite("A"));
  EXPECT_FALSE(IO::Path("B") != StringViewLite("B"));
  EXPECT_TRUE(IO::Path("B") != StringViewLite("C"));

  EXPECT_TRUE(IO::Path("A") != StringViewLite("BA"));
  EXPECT_TRUE(IO::Path("B") != StringViewLite("BA"));
  EXPECT_TRUE(IO::Path("C") != StringViewLite("BA"));
  EXPECT_TRUE(IO::Path("BA") != StringViewLite("A"));
  EXPECT_TRUE(IO::Path("BA") != StringViewLite("B"));
  EXPECT_TRUE(IO::Path("BA") != StringViewLite("C"));

  EXPECT_TRUE(IO::Path("AA") != StringViewLite("B"));
  EXPECT_TRUE(IO::Path("BA") != StringViewLite("B"));
  EXPECT_TRUE(IO::Path("CA") != StringViewLite("B"));
  EXPECT_TRUE(IO::Path("B") != StringViewLite("AA"));
  EXPECT_TRUE(IO::Path("B") != StringViewLite("BA"));
  EXPECT_TRUE(IO::Path("B") != StringViewLite("CA"));
}


TEST(TestIO_Path, OperatorNotEqual_String_LHS)
{
  EXPECT_FALSE(std::string("A") != IO::Path("A"));
  EXPECT_FALSE(std::string("") != IO::Path(""));
  EXPECT_TRUE(std::string("A") != IO::Path(""));
  EXPECT_TRUE(std::string("A") != IO::Path("B"));
  EXPECT_TRUE(std::string("A") != IO::Path("a"));

  EXPECT_TRUE(std::string("A") != IO::Path("B"));
  EXPECT_FALSE(std::string("B") != IO::Path("B"));
  EXPECT_TRUE(std::string("C") != IO::Path("B"));
  EXPECT_TRUE(std::string("B") != IO::Path("A"));
  EXPECT_FALSE(std::string("B") != IO::Path("B"));
  EXPECT_TRUE(std::string("B") != IO::Path("C"));

  EXPECT_TRUE(std::string("A") != IO::Path("BA"));
  EXPECT_TRUE(std::string("B") != IO::Path("BA"));
  EXPECT_TRUE(std::string("C") != IO::Path("BA"));
  EXPECT_TRUE(std::string("BA") != IO::Path("A"));
  EXPECT_TRUE(std::string("BA") != IO::Path("B"));
  EXPECT_TRUE(std::string("BA") != IO::Path("C"));

  EXPECT_TRUE(std::string("AA") != IO::Path("B"));
  EXPECT_TRUE(std::string("BA") != IO::Path("B"));
  EXPECT_TRUE(std::string("CA") != IO::Path("B"));
  EXPECT_TRUE(std::string("B") != IO::Path("AA"));
  EXPECT_TRUE(std::string("B") != IO::Path("BA"));
  EXPECT_TRUE(std::string("B") != IO::Path("CA"));
}


TEST(TestIO_Path, OperatorNotEqual_String_RHS)
{
  EXPECT_FALSE(IO::Path("A") != std::string("A"));
  EXPECT_FALSE(IO::Path("") != std::string(""));
  EXPECT_TRUE(IO::Path("A") != std::string(""));
  EXPECT_TRUE(IO::Path("A") != std::string("B"));
  EXPECT_TRUE(IO::Path("A") != std::string("a"));

  EXPECT_TRUE(IO::Path("A") != std::string("B"));
  EXPECT_FALSE(IO::Path("B") != std::string("B"));
  EXPECT_TRUE(IO::Path("C") != std::string("B"));
  EXPECT_TRUE(IO::Path("B") != std::string("A"));
  EXPECT_FALSE(IO::Path("B") != std::string("B"));
  EXPECT_TRUE(IO::Path("B") != std::string("C"));

  EXPECT_TRUE(IO::Path("A") != std::string("BA"));
  EXPECT_TRUE(IO::Path("B") != std::string("BA"));
  EXPECT_TRUE(IO::Path("C") != std::string("BA"));
  EXPECT_TRUE(IO::Path("BA") != std::string("A"));
  EXPECT_TRUE(IO::Path("BA") != std::string("B"));
  EXPECT_TRUE(IO::Path("BA") != std::string("C"));

  EXPECT_TRUE(IO::Path("AA") != std::string("B"));
  EXPECT_TRUE(IO::Path("BA") != std::string("B"));
  EXPECT_TRUE(IO::Path("CA") != std::string("B"));
  EXPECT_TRUE(IO::Path("B") != std::string("AA"));
  EXPECT_TRUE(IO::Path("B") != std::string("BA"));
  EXPECT_TRUE(IO::Path("B") != std::string("CA"));
}


TEST(TestIO_Path, OperatorNotEqual_UTF8String_LHS)
{
  EXPECT_FALSE(UTF8String("A") != IO::Path("A"));
  EXPECT_FALSE(UTF8String("") != IO::Path(""));
  EXPECT_TRUE(UTF8String("A") != IO::Path(""));
  EXPECT_TRUE(UTF8String("A") != IO::Path("B"));
  EXPECT_TRUE(UTF8String("A") != IO::Path("a"));

  EXPECT_TRUE(UTF8String("A") != IO::Path("B"));
  EXPECT_FALSE(UTF8String("B") != IO::Path("B"));
  EXPECT_TRUE(UTF8String("C") != IO::Path("B"));
  EXPECT_TRUE(UTF8String("B") != IO::Path("A"));
  EXPECT_FALSE(UTF8String("B") != IO::Path("B"));
  EXPECT_TRUE(UTF8String("B") != IO::Path("C"));

  EXPECT_TRUE(UTF8String("A") != IO::Path("BA"));
  EXPECT_TRUE(UTF8String("B") != IO::Path("BA"));
  EXPECT_TRUE(UTF8String("C") != IO::Path("BA"));
  EXPECT_TRUE(UTF8String("BA") != IO::Path("A"));
  EXPECT_TRUE(UTF8String("BA") != IO::Path("B"));
  EXPECT_TRUE(UTF8String("BA") != IO::Path("C"));

  EXPECT_TRUE(UTF8String("AA") != IO::Path("B"));
  EXPECT_TRUE(UTF8String("BA") != IO::Path("B"));
  EXPECT_TRUE(UTF8String("CA") != IO::Path("B"));
  EXPECT_TRUE(UTF8String("B") != IO::Path("AA"));
  EXPECT_TRUE(UTF8String("B") != IO::Path("BA"));
  EXPECT_TRUE(UTF8String("B") != IO::Path("CA"));
}


TEST(TestIO_Path, OperatorNotEqual_UTF8String_RHS)
{
  EXPECT_FALSE(IO::Path("A") != UTF8String("A"));
  EXPECT_FALSE(IO::Path("") != UTF8String(""));
  EXPECT_TRUE(IO::Path("A") != UTF8String(""));
  EXPECT_TRUE(IO::Path("A") != UTF8String("B"));
  EXPECT_TRUE(IO::Path("A") != UTF8String("a"));

  EXPECT_TRUE(IO::Path("A") != UTF8String("B"));
  EXPECT_FALSE(IO::Path("B") != UTF8String("B"));
  EXPECT_TRUE(IO::Path("C") != UTF8String("B"));
  EXPECT_TRUE(IO::Path("B") != UTF8String("A"));
  EXPECT_FALSE(IO::Path("B") != UTF8String("B"));
  EXPECT_TRUE(IO::Path("B") != UTF8String("C"));

  EXPECT_TRUE(IO::Path("A") != UTF8String("BA"));
  EXPECT_TRUE(IO::Path("B") != UTF8String("BA"));
  EXPECT_TRUE(IO::Path("C") != UTF8String("BA"));
  EXPECT_TRUE(IO::Path("BA") != UTF8String("A"));
  EXPECT_TRUE(IO::Path("BA") != UTF8String("B"));
  EXPECT_TRUE(IO::Path("BA") != UTF8String("C"));

  EXPECT_TRUE(IO::Path("AA") != UTF8String("B"));
  EXPECT_TRUE(IO::Path("BA") != UTF8String("B"));
  EXPECT_TRUE(IO::Path("CA") != UTF8String("B"));
  EXPECT_TRUE(IO::Path("B") != UTF8String("AA"));
  EXPECT_TRUE(IO::Path("B") != UTF8String("BA"));
  EXPECT_TRUE(IO::Path("B") != UTF8String("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Compare Operators vs null
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestIO_Path, OperatorLessThan_Null)
{
  const char* pszNull = nullptr;
  constexpr StringViewLite strViewNull(nullptr);

  EXPECT_FALSE(IO::Path("A") < pszNull);
  EXPECT_FALSE(IO::Path("A") < strViewNull);
  EXPECT_TRUE(pszNull < IO::Path("A"));
  EXPECT_TRUE(strViewNull < IO::Path("A"));

  // Compare against empty string
  EXPECT_FALSE(IO::Path("") < pszNull);
  EXPECT_FALSE(IO::Path("") < strViewNull);
  EXPECT_FALSE(pszNull < IO::Path(""));
  EXPECT_FALSE(strViewNull < IO::Path(""));

  // Compare against null
  EXPECT_FALSE(pszNull < strViewNull);
  EXPECT_FALSE(strViewNull < pszNull);
  EXPECT_FALSE(strViewNull < strViewNull);
}


TEST(TestIO_Path, OperatorLessThanOrEqual_Null)
{
  const char* pszNull = nullptr;
  constexpr StringViewLite strViewNull(nullptr);

  EXPECT_FALSE(IO::Path("A") <= pszNull);
  EXPECT_FALSE(IO::Path("A") <= strViewNull);
  EXPECT_TRUE(pszNull <= IO::Path("A"));
  EXPECT_TRUE(strViewNull <= IO::Path("A"));

  // Compare against empty string
  EXPECT_TRUE(IO::Path("") <= pszNull);
  EXPECT_TRUE(IO::Path("") <= strViewNull);
  EXPECT_TRUE(pszNull <= IO::Path(""));
  EXPECT_TRUE(strViewNull <= IO::Path(""));

  // Compare against null
  EXPECT_TRUE(pszNull <= strViewNull);
  EXPECT_TRUE(strViewNull <= pszNull);
  EXPECT_TRUE(strViewNull <= strViewNull);
}


TEST(TestIO_Path, OperatorGreaterThan_Null)
{
  const char* pszNull = nullptr;
  constexpr StringViewLite strViewNull(nullptr);

  EXPECT_TRUE(IO::Path("A") > pszNull);
  EXPECT_TRUE(IO::Path("A") > strViewNull);
  EXPECT_FALSE(pszNull > IO::Path("A"));
  EXPECT_FALSE(strViewNull > IO::Path("A"));

  // Compare against empty string
  EXPECT_FALSE(IO::Path("") > pszNull);
  EXPECT_FALSE(IO::Path("") > strViewNull);
  EXPECT_FALSE(pszNull > IO::Path(""));
  EXPECT_FALSE(strViewNull > IO::Path(""));

  // Compare against null
  EXPECT_FALSE(pszNull > strViewNull);
  EXPECT_FALSE(strViewNull > pszNull);
  EXPECT_FALSE(strViewNull > strViewNull);
}


TEST(TestIO_Path, OperatorGreaterThanOrEqual_Null)
{
  const char* pszNull = nullptr;
  constexpr StringViewLite strViewNull(nullptr);
  // IO::Path str8ViewNull(nullptr);

  EXPECT_TRUE(IO::Path("A") >= pszNull);
  EXPECT_TRUE(IO::Path("A") >= strViewNull);
  EXPECT_FALSE(pszNull >= IO::Path("A"));
  EXPECT_FALSE(strViewNull >= IO::Path("A"));

  // Compare against empty string
  EXPECT_TRUE(IO::Path("") >= pszNull);
  EXPECT_TRUE(IO::Path("") >= strViewNull);
  EXPECT_TRUE(pszNull >= IO::Path(""));
  EXPECT_TRUE(strViewNull >= IO::Path(""));

  // Compare against null
  EXPECT_TRUE(pszNull >= strViewNull);
  EXPECT_TRUE(strViewNull >= pszNull);
  EXPECT_TRUE(strViewNull >= strViewNull);
}


TEST(TestIO_Path, OperatorEqual_Null)
{
  const char* pszNull = nullptr;
  constexpr StringViewLite strViewNull(nullptr);

  EXPECT_FALSE(IO::Path("A") == pszNull);
  EXPECT_FALSE(IO::Path("A") == strViewNull);
  EXPECT_FALSE(pszNull == IO::Path("A"));
  EXPECT_FALSE(strViewNull == IO::Path("A"));

  // Compare against empty string
  EXPECT_TRUE(pszNull == IO::Path(""));
  EXPECT_TRUE(strViewNull == IO::Path(""));
  EXPECT_TRUE(IO::Path("") == pszNull);
  EXPECT_TRUE(IO::Path("") == strViewNull);

  // Compare against null
  EXPECT_TRUE(pszNull == strViewNull);
  EXPECT_TRUE(strViewNull == pszNull);
  EXPECT_TRUE(strViewNull == strViewNull);
}


TEST(TestIO_Path, OperatorNotEqual_Null)
{
  const char* pszNull = nullptr;
  constexpr StringViewLite strViewNull(nullptr);

  EXPECT_TRUE(IO::Path("A") != pszNull);
  EXPECT_TRUE(IO::Path("A") != strViewNull);
  EXPECT_TRUE(pszNull != IO::Path("A"));
  EXPECT_TRUE(strViewNull != IO::Path("A"));

  // Compare against empty string
  EXPECT_FALSE(pszNull != IO::Path(""));
  EXPECT_FALSE(strViewNull != IO::Path(""));
  EXPECT_FALSE(IO::Path("") != pszNull);
  EXPECT_FALSE(IO::Path("") != strViewNull);

  // Compare against null
  EXPECT_FALSE(pszNull != strViewNull);
  EXPECT_FALSE(strViewNull != pszNull);
  EXPECT_FALSE(strViewNull != strViewNull);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator+
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestIO_Path, Op_Plus_Path_CString)
{
  //---------------01234567890
  IO::Path str("hello");

  EXPECT_EQ(StringViewLite("hello world"), str + " world");
  EXPECT_EQ(StringViewLite("hello there!"), str + " there!");
  EXPECT_EQ(StringViewLite("hello!!"), str + "!!");
  EXPECT_EQ(StringViewLite("hello and welcome"), str + " and" + " welcome");
}


TEST(TestIO_Path, Op_Plus_Path_StringViewLite)
{
  //---------------01234567890
  IO::Path str("hello");

  EXPECT_EQ(StringViewLite("hello world"), str + StringViewLite(" world"));
  EXPECT_EQ(StringViewLite("hello there!"), str + StringViewLite(" there!"));
  EXPECT_EQ(StringViewLite("hello!!"), str + StringViewLite("!!"));
  EXPECT_EQ(StringViewLite("hello and welcome"), str + StringViewLite(" and") + StringViewLite(" welcome"));
}

// TEST(TestIO_Path, Op_Plus_Path_UTF8String)
//{
//  //---------------01234567890
//  IO::Path str("hello");
//
//  EXPECT_EQ(StringViewLite("hello world"), str + UTF8String(" world"));
//  EXPECT_EQ(StringViewLite("hello there!"), str + UTF8String(" there!"));
//  EXPECT_EQ(StringViewLite("hello!!"), str + UTF8String("!!"));
//  EXPECT_EQ(StringViewLite("hello and welcome"), str + UTF8String(" and") + UTF8String(" welcome"));
//}


TEST(TestIO_Path, Op_Plus_Path_Path)
{
  //---------------01234567890
  IO::Path str("hello");

  EXPECT_EQ(StringViewLite("hello world"), str + IO::Path(" world"));
  EXPECT_EQ(StringViewLite("hello there!"), str + IO::Path(" there!"));
  EXPECT_EQ(StringViewLite("hello!!"), str + IO::Path("!!"));
  EXPECT_EQ(StringViewLite("hello and welcome"), str + IO::Path(" and") + IO::Path(" welcome"));
}


TEST(TestIO_Path, Op_Plus_CString_Path)
{
  //---------------01234567890

  EXPECT_EQ(StringViewLite("hello world"), "hello" + IO::Path(" world"));
  EXPECT_EQ(StringViewLite("hello there!"), "hello" + IO::Path(" there!"));
  EXPECT_EQ(StringViewLite("hello!!"), "hello" + IO::Path("!!"));
}


TEST(TestIO_Path, Op_Plus_StringViewLite_Path)
{
  //---------------01234567890
  IO::Path str("hello");

  EXPECT_EQ(StringViewLite("hello world"), StringViewLite("hello") + IO::Path(" world"));
  EXPECT_EQ(StringViewLite("hello there!"), StringViewLite("hello") + IO::Path(" there!"));
  EXPECT_EQ(StringViewLite("hello!!"), StringViewLite("hello") + IO::Path("!!"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator+=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

// TEST(TestIO_Path, Op_PlusEquals_InvalidChar)
//{
//  //---------------01234567890
//  IO::Path str("hello world");
//
//  EXPECT_THROW(str += static_cast<char>(0xFF), std::invalid_argument);
//}


// TEST(TestIO_Path, Op_PlusEquals_Char)
//{
//  //---------------01234567890
//  IO::Path str("hello world");
//  str += '!';
//
//  EXPECT_EQ(StringViewLite("hello world!"), str);
//
//  str += '!';
//  EXPECT_EQ(StringViewLite("hello world!!"), str);
//}


TEST(TestIO_Path, Op_PlusEquals_CString)
{
  //---------------01234567890
  IO::Path str("hello");

  str += " world";

  EXPECT_EQ(StringViewLite("hello world"), str);
}


TEST(TestIO_Path, Op_PlusEquals_StringViewLite)
{
  //---------------01234567890
  IO::Path str("hello");

  str += StringViewLite(" world");

  EXPECT_EQ(StringViewLite("hello world"), str);
}


TEST(TestIO_Path, Op_PlusEquals_UTF8String)
{
  //---------------01234567890
  IO::Path str("hello");

  str += UTF8String(" world");

  EXPECT_EQ(StringViewLite("hello world"), str);
}

TEST(TestIO_Path, Op_PlusEquals_Path)
{
  //---------------01234567890
  IO::Path str("hello");

  str += IO::Path(" world");

  EXPECT_EQ(StringViewLite("hello world"), str);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Move
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestIO_Path, MoveAssignment)
{
  IO::Path path1("hello");
  IO::Path path2("hey");

  path2 = std::move(path1);
  // Move does not have to change the content of str1 so this check could fail
  // EXPECT_EQ(str1, "");
  EXPECT_EQ(path2, "hello");
}


TEST(TestIO_Path, MoveConstruct)
{
  IO::Path path1("hello");
  IO::Path path2(std::move(path1));

  // Move does not have to change the content of str1 so this check could fail
  // EXPECT_EQ(str1, "");
  EXPECT_EQ(path2, "hello");
}
