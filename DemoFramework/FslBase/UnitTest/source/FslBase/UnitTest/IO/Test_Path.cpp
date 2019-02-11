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

#include <FslBase/IO/Path.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/IO/LogPath.hpp>
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


TEST(TestIO_Path, Construct_String)
{
  IO::Path path("hello");

  EXPECT_FALSE(path.IsEmpty());
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


TEST(TestIO_Path, StartsWith_String)
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


TEST(TestIO_Path, EndsWith_String)
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

  auto res = path.ToUTF8String();

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


TEST(TestIO_Path, Combine)
{
  EXPECT_EQ(IO::Path("world"), IO::Path::Combine("", "world"));
  EXPECT_EQ(IO::Path("/world"), IO::Path::Combine("/", "world"));
  EXPECT_EQ(IO::Path("hello/world"), IO::Path::Combine("hello", "world"));
  // Rooted path2 -> path2
  EXPECT_EQ(IO::Path("/world"), IO::Path::Combine("hello", "/world"));
  // empty path 2 -> path1
  EXPECT_EQ(IO::Path("hello"), IO::Path::Combine("hello", ""));
}


TEST(TestIO_Path, GetDirectoryName)
{
  EXPECT_EQ(IO::Path(""), IO::Path::GetDirectoryName("world"));
  // This could technically give incorrect results when working on unix root dir level, investigate.
  EXPECT_EQ(IO::Path(""), IO::Path::GetDirectoryName("/world"));
  EXPECT_EQ(IO::Path("hello"), IO::Path::GetDirectoryName("hello/world"));
  EXPECT_EQ(IO::Path("q/hello"), IO::Path::GetDirectoryName("q/hello/world"));
  EXPECT_EQ(IO::Path("/q/hello"), IO::Path::GetDirectoryName("/q/hello/world"));
}


TEST(TestIO_Path, GetFileName)
{
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileName("world"));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileName("/world"));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileName("hello/world"));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileName("q/hello/world"));
  EXPECT_EQ(IO::Path("world"), IO::Path::GetFileName("/q/hello/world"));
}


TEST(TestIO_Path, GetExtension)
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
