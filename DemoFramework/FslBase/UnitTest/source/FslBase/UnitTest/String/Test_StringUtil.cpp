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

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, Contains_Char_Empty_StringViewLite)
{
  StringViewLite src;

  EXPECT_FALSE(StringUtil::Contains(src, ' '));
  EXPECT_FALSE(StringUtil::Contains(src, 'a'));
}


TEST(TestString_StringUtil, Contains_Char_Nullptr_CString)
{
  const char* const pszNull = nullptr;

  EXPECT_FALSE(StringUtil::Contains(pszNull, ' '));
  EXPECT_FALSE(StringUtil::Contains(pszNull, 'a'));
}


TEST(TestString_StringUtil, Contains_Char_Empty_String)
{
  std::string src;

  EXPECT_FALSE(StringUtil::Contains(src, ' '));
  EXPECT_FALSE(StringUtil::Contains(src, 'a'));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, Contains_Char_StringViewLite)
{
  StringViewLite src("hello world");

  EXPECT_TRUE(StringUtil::Contains(src, 'h'));
  EXPECT_TRUE(StringUtil::Contains(src, ' '));
  EXPECT_TRUE(StringUtil::Contains(src, 'd'));

  EXPECT_FALSE(StringUtil::Contains(src, 'a'));
  EXPECT_FALSE(StringUtil::Contains(src, 'H'));
}

TEST(TestString_StringUtil, Contains_Char_CString)
{
  const char* const psz = "hello world";

  EXPECT_TRUE(StringUtil::Contains(psz, 'h'));
  EXPECT_TRUE(StringUtil::Contains(psz, ' '));
  EXPECT_TRUE(StringUtil::Contains(psz, 'd'));

  EXPECT_FALSE(StringUtil::Contains(psz, 'a'));
  EXPECT_FALSE(StringUtil::Contains(psz, 'H'));
}

TEST(TestString_StringUtil, Contains_Char_String)
{
  std::string src("hello world");

  EXPECT_TRUE(StringUtil::Contains(src, 'h'));
  EXPECT_TRUE(StringUtil::Contains(src, ' '));
  EXPECT_TRUE(StringUtil::Contains(src, 'd'));

  EXPECT_FALSE(StringUtil::Contains(src, 'a'));
  EXPECT_FALSE(StringUtil::Contains(src, 'H'));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, Contains_String_Empty)
{
  std::string src;

  EXPECT_TRUE(StringUtil::Contains(src, ""));

  EXPECT_FALSE(StringUtil::Contains(src, "a"));
  EXPECT_FALSE(StringUtil::Contains(src, "abcd"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestString_StringUtil, Contains_String)
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

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, StartsWith_Char_Empty_StringViewLite)
{
  StringViewLite src;

  EXPECT_FALSE(StringUtil::StartsWith(src, ' '));
  EXPECT_FALSE(StringUtil::StartsWith(src, 'a'));
}


TEST(TestString_StringUtil, StartsWith_Char_Empty_CString)
{
  const char* const pszNull = nullptr;

  EXPECT_FALSE(StringUtil::StartsWith(pszNull, ' '));
  EXPECT_FALSE(StringUtil::StartsWith(pszNull, 'a'));
}


TEST(TestString_StringUtil, StartsWith_Char_Empty_String)
{
  std::string src;

  EXPECT_FALSE(StringUtil::StartsWith(src, ' '));
  EXPECT_FALSE(StringUtil::StartsWith(src, 'a'));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, StartsWith_Char_StringViewLite)
{
  StringViewLite src("hello world");

  EXPECT_TRUE(StringUtil::StartsWith(src, 'h'));

  EXPECT_FALSE(StringUtil::StartsWith(src, 'H'));
  EXPECT_FALSE(StringUtil::StartsWith(src, 'e'));
  EXPECT_FALSE(StringUtil::StartsWith(src, 'd'));
}

TEST(TestString_StringUtil, StartsWith_Char_CString)
{
  const char* const psz = "hello world";

  EXPECT_TRUE(StringUtil::StartsWith(psz, 'h'));

  EXPECT_FALSE(StringUtil::StartsWith(psz, 'H'));
  EXPECT_FALSE(StringUtil::StartsWith(psz, 'e'));
  EXPECT_FALSE(StringUtil::StartsWith(psz, 'd'));
}

TEST(TestString_StringUtil, StartsWith_Char_String)
{
  std::string src("hello world");

  EXPECT_TRUE(StringUtil::StartsWith(src, 'h'));

  EXPECT_FALSE(StringUtil::StartsWith(src, 'H'));
  EXPECT_FALSE(StringUtil::StartsWith(src, 'e'));
  EXPECT_FALSE(StringUtil::StartsWith(src, 'd'));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, StartsWith_Empty_StringViewLite)
{
  StringViewLite src;

  EXPECT_TRUE(StringUtil::StartsWith(src, ""));
  EXPECT_FALSE(StringUtil::StartsWith(src, " "));
  EXPECT_FALSE(StringUtil::StartsWith(src, "a"));
}

TEST(TestString_StringUtil, StartsWith_Empty_CString)
{
  const char* const psz = nullptr;

  EXPECT_TRUE(StringUtil::StartsWith(psz, ""));
  EXPECT_FALSE(StringUtil::StartsWith(psz, " "));
  EXPECT_FALSE(StringUtil::StartsWith(psz, "a"));
}

TEST(TestString_StringUtil, StartsWith_Empty_String)
{
  std::string src;

  EXPECT_TRUE(StringUtil::StartsWith(src, ""));
  EXPECT_FALSE(StringUtil::StartsWith(src, " "));
  EXPECT_FALSE(StringUtil::StartsWith(src, "a"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, StartsWith_String_Empty_StringViewLite_StringViewLite)
{
  StringViewLite src;

  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("")));
  EXPECT_FALSE(StringUtil::StartsWith(src, StringViewLite(" ")));
  EXPECT_FALSE(StringUtil::StartsWith(src, StringViewLite("a")));
}

TEST(TestString_StringUtil, StartsWith_String_Empty_StringViewLite_CString)
{
  StringViewLite src;

  EXPECT_TRUE(StringUtil::StartsWith(src, ""));
  EXPECT_FALSE(StringUtil::StartsWith(src, " "));
  EXPECT_FALSE(StringUtil::StartsWith(src, "a"));
}

TEST(TestString_StringUtil, StartsWith_String_Empty_StringViewLite_String)
{
  StringViewLite src;

  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("")));
  EXPECT_FALSE(StringUtil::StartsWith(src, std::string(" ")));
  EXPECT_FALSE(StringUtil::StartsWith(src, std::string("a")));
}


TEST(TestString_StringUtil, StartsWith_String_Empty_CString_StringViewLite)
{
  const char* const pszNull = nullptr;

  EXPECT_TRUE(StringUtil::StartsWith(pszNull, StringViewLite("")));
  EXPECT_FALSE(StringUtil::StartsWith(pszNull, StringViewLite(" ")));
  EXPECT_FALSE(StringUtil::StartsWith(pszNull, StringViewLite("a")));
}


TEST(TestString_StringUtil, StartsWith_String_Empty_CString_CString)
{
  const char* const pszNull = nullptr;

  EXPECT_TRUE(StringUtil::StartsWith(pszNull, ""));
  EXPECT_FALSE(StringUtil::StartsWith(pszNull, " "));
  EXPECT_FALSE(StringUtil::StartsWith(pszNull, "a"));
}

TEST(TestString_StringUtil, StartsWith_String_Empty_CString_String)
{
  const char* const pszNull = nullptr;

  EXPECT_TRUE(StringUtil::StartsWith(pszNull, std::string("")));
  EXPECT_FALSE(StringUtil::StartsWith(pszNull, std::string(" ")));
  EXPECT_FALSE(StringUtil::StartsWith(pszNull, std::string("a")));
}


TEST(TestString_StringUtil, StartsWith_String_Empty_String_StringViewLite)
{
  std::string src;

  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("")));
  EXPECT_FALSE(StringUtil::StartsWith(src, StringViewLite(" ")));
  EXPECT_FALSE(StringUtil::StartsWith(src, StringViewLite("a")));
}

TEST(TestString_StringUtil, StartsWith_String_Empty_String_CString)
{
  std::string src;

  EXPECT_TRUE(StringUtil::StartsWith(src, ""));
  EXPECT_FALSE(StringUtil::StartsWith(src, " "));
  EXPECT_FALSE(StringUtil::StartsWith(src, "a"));
}

TEST(TestString_StringUtil, StartsWith_String_Empty_String_String)
{
  std::string src;

  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("")));
  EXPECT_FALSE(StringUtil::StartsWith(src, std::string(" ")));
  EXPECT_FALSE(StringUtil::StartsWith(src, std::string("a")));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, StartsWith_StringViewLite_StringViewLite)
{
  StringViewLite src("hello world");

  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("h")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("he")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("hel")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("hell")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("hello")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("hello ")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("hello w")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("hello wo")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("hello wor")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("hello worl")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("hello world")));

  EXPECT_FALSE(StringUtil::StartsWith(src, StringViewLite("hello world!")));
  EXPECT_FALSE(StringUtil::StartsWith(src, StringViewLite("e")));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, StartsWith_StringViewLite_CString)
{
  StringViewLite src("hello world");

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

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, StartsWith_StringViewLite_String)
{
  StringViewLite src("hello world");

  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("h")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("he")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("hel")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("hell")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("hello")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("hello ")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("hello w")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("hello wo")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("hello wor")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("hello worl")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("hello world")));

  EXPECT_FALSE(StringUtil::StartsWith(src, std::string("hello world!")));
  EXPECT_FALSE(StringUtil::StartsWith(src, std::string("e")));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, StartsWith_CString_StringViewLite)
{
  const char* const pszSrc = "hello world";

  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, StringViewLite("")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, StringViewLite("h")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, StringViewLite("he")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, StringViewLite("hel")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, StringViewLite("hell")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, StringViewLite("hello")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, StringViewLite("hello ")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, StringViewLite("hello w")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, StringViewLite("hello wo")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, StringViewLite("hello wor")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, StringViewLite("hello worl")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, StringViewLite("hello world")));

  EXPECT_FALSE(StringUtil::StartsWith(pszSrc, StringViewLite("hello world!")));
  EXPECT_FALSE(StringUtil::StartsWith(pszSrc, StringViewLite("e")));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, StartsWith_CString_CString)
{
  const char* const pszSrc = "hello world";

  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, ""));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, "h"));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, "he"));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, "hel"));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, "hell"));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, "hello"));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, "hello "));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, "hello w"));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, "hello wo"));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, "hello wor"));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, "hello worl"));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, "hello world"));

  EXPECT_FALSE(StringUtil::StartsWith(pszSrc, "hello world!"));
  EXPECT_FALSE(StringUtil::StartsWith(pszSrc, "e"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, StartsWith_CString_String)
{
  const char* const pszSrc = "hello world";

  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, std::string("")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, std::string("h")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, std::string("he")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, std::string("hel")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, std::string("hell")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, std::string("hello")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, std::string("hello ")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, std::string("hello w")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, std::string("hello wo")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, std::string("hello wor")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, std::string("hello worl")));
  EXPECT_TRUE(StringUtil::StartsWith(pszSrc, std::string("hello world")));

  EXPECT_FALSE(StringUtil::StartsWith(pszSrc, std::string("hello world!")));
  EXPECT_FALSE(StringUtil::StartsWith(pszSrc, std::string("e")));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, StartsWith_String_StringViewLite)
{
  std::string src("hello world");

  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("h")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("he")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("hel")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("hell")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("hello")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("hello ")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("hello w")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("hello wo")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("hello wor")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("hello worl")));
  EXPECT_TRUE(StringUtil::StartsWith(src, StringViewLite("hello world")));

  EXPECT_FALSE(StringUtil::StartsWith(src, StringViewLite("hello world!")));
  EXPECT_FALSE(StringUtil::StartsWith(src, StringViewLite("e")));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, StartsWith_String_CString)
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

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, StartsWith_String_String)
{
  std::string src("hello world");

  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("h")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("he")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("hel")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("hell")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("hello")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("hello ")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("hello w")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("hello wo")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("hello wor")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("hello worl")));
  EXPECT_TRUE(StringUtil::StartsWith(src, std::string("hello world")));

  EXPECT_FALSE(StringUtil::StartsWith(src, std::string("hello world!")));
  EXPECT_FALSE(StringUtil::StartsWith(src, std::string("e")));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, EndsWith_Char_Empty_StringViewLite)
{
  StringViewLite src;

  EXPECT_FALSE(StringUtil::EndsWith(src, ' '));
  EXPECT_FALSE(StringUtil::EndsWith(src, 'a'));
}

TEST(TestString_StringUtil, EndsWith_Char_Empty_CString)
{
  const char* const pszNull = nullptr;

  EXPECT_FALSE(StringUtil::EndsWith(pszNull, ' '));
  EXPECT_FALSE(StringUtil::EndsWith(pszNull, 'a'));
}

TEST(TestString_StringUtil, EndsWith_Char_Empty_String)
{
  std::string src;

  EXPECT_FALSE(StringUtil::EndsWith(src, ' '));
  EXPECT_FALSE(StringUtil::EndsWith(src, 'a'));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, EndsWith_Char_StringViewLite)
{
  StringViewLite src("hello world");

  EXPECT_TRUE(StringUtil::EndsWith(src, 'd'));

  EXPECT_FALSE(StringUtil::EndsWith(src, 'D'));
  EXPECT_FALSE(StringUtil::EndsWith(src, 'l'));
  EXPECT_FALSE(StringUtil::EndsWith(src, 'h'));
}

TEST(TestString_StringUtil, EndsWith_Char_CString)
{
  const char* const pszSrc = "hello world";

  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, 'd'));

  EXPECT_FALSE(StringUtil::EndsWith(pszSrc, 'D'));
  EXPECT_FALSE(StringUtil::EndsWith(pszSrc, 'l'));
  EXPECT_FALSE(StringUtil::EndsWith(pszSrc, 'h'));
}

TEST(TestString_StringUtil, EndsWith_Char_String)
{
  std::string src("hello world");

  EXPECT_TRUE(StringUtil::EndsWith(src, 'd'));

  EXPECT_FALSE(StringUtil::EndsWith(src, 'D'));
  EXPECT_FALSE(StringUtil::EndsWith(src, 'l'));
  EXPECT_FALSE(StringUtil::EndsWith(src, 'h'));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, EndsWith_String_Empty_StringViewLite_StringViewLite)
{
  StringViewLite src;

  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("")));
  EXPECT_FALSE(StringUtil::EndsWith(src, StringViewLite(" ")));
  EXPECT_FALSE(StringUtil::EndsWith(src, StringViewLite("a")));
}


TEST(TestString_StringUtil, EndsWith_String_Empty_StringViewLite_CString)
{
  StringViewLite src;

  EXPECT_TRUE(StringUtil::EndsWith(src, ""));
  EXPECT_FALSE(StringUtil::EndsWith(src, " "));
  EXPECT_FALSE(StringUtil::EndsWith(src, "a"));
}

TEST(TestString_StringUtil, EndsWith_String_Empty_StringViewLite_String)
{
  const char* const pszNull = nullptr;

  EXPECT_TRUE(StringUtil::EndsWith(pszNull, std::string("")));
  EXPECT_FALSE(StringUtil::EndsWith(pszNull, std::string(" ")));
  EXPECT_FALSE(StringUtil::EndsWith(pszNull, std::string("a")));
}

TEST(TestString_StringUtil, EndsWith_CString_Empty_String_StringViewLite)
{
  const char* const pszNull = nullptr;

  EXPECT_TRUE(StringUtil::EndsWith(pszNull, StringViewLite("")));
  EXPECT_FALSE(StringUtil::EndsWith(pszNull, StringViewLite(" ")));
  EXPECT_FALSE(StringUtil::EndsWith(pszNull, StringViewLite("a")));
}


TEST(TestString_StringUtil, EndsWith_CString_Empty_String_CString)
{
  const char* const pszNull = nullptr;

  EXPECT_TRUE(StringUtil::EndsWith(pszNull, ""));
  EXPECT_FALSE(StringUtil::EndsWith(pszNull, " "));
  EXPECT_FALSE(StringUtil::EndsWith(pszNull, "a"));
}

TEST(TestString_StringUtil, EndsWith_CString_Empty_String_String)
{
  std::string src;

  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("")));
  EXPECT_FALSE(StringUtil::EndsWith(src, std::string(" ")));
  EXPECT_FALSE(StringUtil::EndsWith(src, std::string("a")));
}

TEST(TestString_StringUtil, EndsWith_String_Empty_String_StringViewLite)
{
  std::string src;

  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("")));
  EXPECT_FALSE(StringUtil::EndsWith(src, StringViewLite(" ")));
  EXPECT_FALSE(StringUtil::EndsWith(src, StringViewLite("a")));
}


TEST(TestString_StringUtil, EndsWith_String_Empty_String_CString)
{
  std::string src;

  EXPECT_TRUE(StringUtil::EndsWith(src, ""));
  EXPECT_FALSE(StringUtil::EndsWith(src, " "));
  EXPECT_FALSE(StringUtil::EndsWith(src, "a"));
}

TEST(TestString_StringUtil, EndsWith_String_Empty_String_String)
{
  std::string src;

  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("")));
  EXPECT_FALSE(StringUtil::EndsWith(src, std::string(" ")));
  EXPECT_FALSE(StringUtil::EndsWith(src, std::string("a")));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, EndsWith_String_StringViewLite_StringViewLite)
{
  StringViewLite src("hello world");

  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("d")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("ld")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("rld")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("orld")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite(" world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("o world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("lo world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("llo world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("ello world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("hello world")));

  EXPECT_FALSE(StringUtil::EndsWith(src, StringViewLite("!hello world")));
  EXPECT_FALSE(StringUtil::EndsWith(src, StringViewLite("l")));
}

TEST(TestString_StringUtil, EndsWith_String_StringViewLite_CString)
{
  StringViewLite src("hello world");

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

TEST(TestString_StringUtil, EndsWith_String_StringViewLite_String)
{
  StringViewLite src("hello world");

  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("d")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("ld")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("rld")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("orld")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string(" world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("o world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("lo world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("llo world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("ello world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("hello world")));

  EXPECT_FALSE(StringUtil::EndsWith(src, std::string("!hello world")));
  EXPECT_FALSE(StringUtil::EndsWith(src, std::string("l")));
}

TEST(TestString_StringUtil, EndsWith_String_CString_StringViewLite)
{
  const char* const pszSrc = "hello world";

  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, StringViewLite("")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, StringViewLite("d")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, StringViewLite("ld")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, StringViewLite("rld")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, StringViewLite("orld")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, StringViewLite("world")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, StringViewLite(" world")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, StringViewLite("o world")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, StringViewLite("lo world")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, StringViewLite("llo world")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, StringViewLite("ello world")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, StringViewLite("hello world")));

  EXPECT_FALSE(StringUtil::EndsWith(pszSrc, StringViewLite("!hello world")));
  EXPECT_FALSE(StringUtil::EndsWith(pszSrc, StringViewLite("l")));
}

TEST(TestString_StringUtil, EndsWith_String_CString_CString)
{
  const char* const pszSrc = "hello world";

  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, ""));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, "d"));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, "ld"));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, "rld"));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, "orld"));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, "world"));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, " world"));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, "o world"));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, "lo world"));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, "llo world"));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, "ello world"));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, "hello world"));

  EXPECT_FALSE(StringUtil::EndsWith(pszSrc, "!hello world"));
  EXPECT_FALSE(StringUtil::EndsWith(pszSrc, "l"));
}

TEST(TestString_StringUtil, EndsWith_String_CString_String)
{
  const char* const pszSrc = "hello world";

  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, std::string("")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, std::string("d")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, std::string("ld")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, std::string("rld")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, std::string("orld")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, std::string("world")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, std::string(" world")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, std::string("o world")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, std::string("lo world")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, std::string("llo world")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, std::string("ello world")));
  EXPECT_TRUE(StringUtil::EndsWith(pszSrc, std::string("hello world")));

  EXPECT_FALSE(StringUtil::EndsWith(pszSrc, std::string("!hello world")));
  EXPECT_FALSE(StringUtil::EndsWith(pszSrc, std::string("l")));
}

TEST(TestString_StringUtil, EndsWith_String_String_StringViewLite)
{
  std::string src("hello world");

  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("d")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("ld")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("rld")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("orld")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite(" world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("o world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("lo world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("llo world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("ello world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, StringViewLite("hello world")));

  EXPECT_FALSE(StringUtil::EndsWith(src, StringViewLite("!hello world")));
  EXPECT_FALSE(StringUtil::EndsWith(src, StringViewLite("l")));
}

TEST(TestString_StringUtil, EndsWith_String_String_CString)
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

TEST(TestString_StringUtil, EndsWith_String_String_String)
{
  std::string src("hello world");

  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("d")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("ld")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("rld")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("orld")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string(" world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("o world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("lo world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("llo world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("ello world")));
  EXPECT_TRUE(StringUtil::EndsWith(src, std::string("hello world")));

  EXPECT_FALSE(StringUtil::EndsWith(src, std::string("!hello world")));
  EXPECT_FALSE(StringUtil::EndsWith(src, std::string("l")));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, IndexOfChar_NegativeStartIndex_StringViewLite)
{
  StringViewLite src("old school");

  EXPECT_LT(StringUtil::IndexOf(src, 'o', -1), 0);
  EXPECT_LT(StringUtil::IndexOf(src, 'o', -2), 0);
  EXPECT_LT(StringUtil::IndexOf(src, 'o', -3), 0);
  EXPECT_LT(StringUtil::IndexOf(src, 'o', std::numeric_limits<int32_t>::min()), 0);
}

TEST(TestString_StringUtil, IndexOfChar_NegativeStartIndex_CString)
{
  const char* const pszSrc = "old school";

  EXPECT_LT(StringUtil::IndexOf(pszSrc, 'o', -1), 0);
  EXPECT_LT(StringUtil::IndexOf(pszSrc, 'o', -2), 0);
  EXPECT_LT(StringUtil::IndexOf(pszSrc, 'o', -3), 0);
  EXPECT_LT(StringUtil::IndexOf(pszSrc, 'o', std::numeric_limits<int32_t>::min()), 0);
}

TEST(TestString_StringUtil, IndexOfChar_NegativeStartIndex_String)
{
  std::string src("old school");

  EXPECT_LT(StringUtil::IndexOf(src, 'o', -1), 0);
  EXPECT_LT(StringUtil::IndexOf(src, 'o', -2), 0);
  EXPECT_LT(StringUtil::IndexOf(src, 'o', -3), 0);
  EXPECT_LT(StringUtil::IndexOf(src, 'o', std::numeric_limits<int32_t>::min()), 0);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, IndexOfChar_StartIndexTooLarge_StringViewLite)
{
  //                  0123456789
  StringViewLite src("old school");

  EXPECT_LT(StringUtil::IndexOf(src, 'l', 10), 0);
  EXPECT_LT(StringUtil::IndexOf(src, 'l', 11), 0);
  EXPECT_LT(StringUtil::IndexOf(src, 'l', 12), 0);
  EXPECT_LT(StringUtil::IndexOf(src, 'l', 13), 0);

  EXPECT_EQ(StringUtil::IndexOf(src, 'l', 9), 9);
}

TEST(TestString_StringUtil, IndexOfChar_StartIndexTooLarge_CString)
{
  //                          0123456789
  const char* const pszSrc = "old school";

  EXPECT_LT(StringUtil::IndexOf(pszSrc, 'l', 10), 0);
  EXPECT_LT(StringUtil::IndexOf(pszSrc, 'l', 11), 0);
  EXPECT_LT(StringUtil::IndexOf(pszSrc, 'l', 12), 0);
  EXPECT_LT(StringUtil::IndexOf(pszSrc, 'l', 13), 0);

  EXPECT_EQ(StringUtil::IndexOf(pszSrc, 'l', 9), 9);
}

TEST(TestString_StringUtil, IndexOfChar_StartIndexTooLarge_String)
{
  //               0123456789
  std::string src("old school");

  EXPECT_LT(StringUtil::IndexOf(src, 'l', 10), 0);
  EXPECT_LT(StringUtil::IndexOf(src, 'l', 11), 0);
  EXPECT_LT(StringUtil::IndexOf(src, 'l', 12), 0);
  EXPECT_LT(StringUtil::IndexOf(src, 'l', 13), 0);

  EXPECT_EQ(StringUtil::IndexOf(src, 'l', 9), 9);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, IndexOfChar_Empty_StringViewLite)
{
  StringViewLite src;

  EXPECT_LT(StringUtil::IndexOf(src, 'o', -1), 0);
  EXPECT_LT(StringUtil::IndexOf(src, 'o', -2), 0);
  EXPECT_LT(StringUtil::IndexOf(src, 'o', -3), 0);
  EXPECT_LT(StringUtil::IndexOf(src, 'o', -4), 0);
}

TEST(TestString_StringUtil, IndexOfChar_Empty_CString)
{
  const char* const pszSrc = nullptr;

  EXPECT_LT(StringUtil::IndexOf(pszSrc, 'o', -1), 0);
  EXPECT_LT(StringUtil::IndexOf(pszSrc, 'o', -2), 0);
  EXPECT_LT(StringUtil::IndexOf(pszSrc, 'o', -3), 0);
  EXPECT_LT(StringUtil::IndexOf(pszSrc, 'o', -4), 0);
}

TEST(TestString_StringUtil, IndexOfChar_Empty_String)
{
  std::string src;

  EXPECT_LT(StringUtil::IndexOf(src, 'o', -1), 0);
  EXPECT_LT(StringUtil::IndexOf(src, 'o', -2), 0);
  EXPECT_LT(StringUtil::IndexOf(src, 'o', -3), 0);
  EXPECT_LT(StringUtil::IndexOf(src, 'o', -4), 0);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, IndexOfChar_StringViewLite)
{
  //---------------0123456789
  StringViewLite src("old school");

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
  EXPECT_EQ(9, StringUtil::IndexOf(src, 'l', 9));

  EXPECT_GE(0, StringUtil::IndexOf(src, 'O'));
  EXPECT_GE(0, StringUtil::IndexOf(src, 'a'));
  EXPECT_GE(0, StringUtil::IndexOf(src, 'D'));

  EXPECT_GE(0, StringUtil::IndexOf(src, 'o', 9));
  EXPECT_GE(0, StringUtil::IndexOf(src, 'o', 10));
  EXPECT_GE(0, StringUtil::IndexOf(src, 'o', 11));
  EXPECT_GE(0, StringUtil::IndexOf(src, 'o', 12));
}


TEST(TestString_StringUtil, IndexOfChar_CString)
{
  //---------------0123456789
  const char* const pszSrc = "old school";

  EXPECT_EQ(0, StringUtil::IndexOf(pszSrc, 'o'));
  EXPECT_EQ(1, StringUtil::IndexOf(pszSrc, 'l'));
  EXPECT_EQ(2, StringUtil::IndexOf(pszSrc, 'd'));
  EXPECT_EQ(3, StringUtil::IndexOf(pszSrc, ' '));
  EXPECT_EQ(4, StringUtil::IndexOf(pszSrc, 's'));
  EXPECT_EQ(5, StringUtil::IndexOf(pszSrc, 'c'));
  EXPECT_EQ(6, StringUtil::IndexOf(pszSrc, 'h'));

  EXPECT_EQ(7, StringUtil::IndexOf(pszSrc, 'o', 1));
  EXPECT_EQ(7, StringUtil::IndexOf(pszSrc, 'o', 7));
  EXPECT_EQ(8, StringUtil::IndexOf(pszSrc, 'o', 8));
  EXPECT_EQ(9, StringUtil::IndexOf(pszSrc, 'l', 2));
  EXPECT_EQ(9, StringUtil::IndexOf(pszSrc, 'l', 9));

  EXPECT_GE(0, StringUtil::IndexOf(pszSrc, 'O'));
  EXPECT_GE(0, StringUtil::IndexOf(pszSrc, 'a'));
  EXPECT_GE(0, StringUtil::IndexOf(pszSrc, 'D'));

  EXPECT_GE(0, StringUtil::IndexOf(pszSrc, 'o', 9));
  EXPECT_GE(0, StringUtil::IndexOf(pszSrc, 'o', 10));
  EXPECT_GE(0, StringUtil::IndexOf(pszSrc, 'o', 11));
  EXPECT_GE(0, StringUtil::IndexOf(pszSrc, 'o', 12));
}

TEST(TestString_StringUtil, IndexOfChar_String)
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
  EXPECT_EQ(9, StringUtil::IndexOf(src, 'l', 9));

  EXPECT_GE(0, StringUtil::IndexOf(src, 'O'));
  EXPECT_GE(0, StringUtil::IndexOf(src, 'a'));
  EXPECT_GE(0, StringUtil::IndexOf(src, 'D'));

  EXPECT_GE(0, StringUtil::IndexOf(src, 'o', 9));
  EXPECT_GE(0, StringUtil::IndexOf(src, 'o', 10));
  EXPECT_GE(0, StringUtil::IndexOf(src, 'o', 11));
  EXPECT_GE(0, StringUtil::IndexOf(src, 'o', 12));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, LastIndexOfChar_Empty_StringViewLite)
{
  StringViewLite src;

  EXPECT_LT(StringUtil::LastIndexOf(src, 'a'), 0);
  EXPECT_LT(StringUtil::LastIndexOf(src, 'D'), 0);
}

TEST(TestString_StringUtil, LastIndexOfChar_Empty_CString)
{
  const char* const pszSrc = nullptr;

  EXPECT_LT(StringUtil::LastIndexOf(pszSrc, 'a'), 0);
  EXPECT_LT(StringUtil::LastIndexOf(pszSrc, 'D'), 0);
}


TEST(TestString_StringUtil, LastIndexOfChar_Empty_String)
{
  std::string src;

  EXPECT_LT(StringUtil::LastIndexOf(src, 'a'), 0);
  EXPECT_LT(StringUtil::LastIndexOf(src, 'D'), 0);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, LastIndexOfChar_StringViewLite)
{
  //------------------0123456789
  StringViewLite src("Old school");

  EXPECT_EQ(9, StringUtil::LastIndexOf(src, 'l'));
  EXPECT_EQ(8, StringUtil::LastIndexOf(src, 'o'));
  EXPECT_EQ(6, StringUtil::LastIndexOf(src, 'h'));
  EXPECT_EQ(5, StringUtil::LastIndexOf(src, 'c'));
  EXPECT_EQ(4, StringUtil::LastIndexOf(src, 's'));
  EXPECT_EQ(3, StringUtil::LastIndexOf(src, ' '));
  EXPECT_EQ(2, StringUtil::LastIndexOf(src, 'd'));
  EXPECT_EQ(0, StringUtil::LastIndexOf(src, 'O'));

  EXPECT_GE(0, StringUtil::LastIndexOf(src, 'a'));
  EXPECT_GE(0, StringUtil::LastIndexOf(src, 'D'));
}

TEST(TestString_StringUtil, LastIndexOfChar_CString)
{
  //-------------------------0123456789
  const char* const pszSrc = "Old school";

  EXPECT_EQ(9, StringUtil::LastIndexOf(pszSrc, 'l'));
  EXPECT_EQ(8, StringUtil::LastIndexOf(pszSrc, 'o'));
  EXPECT_EQ(6, StringUtil::LastIndexOf(pszSrc, 'h'));
  EXPECT_EQ(5, StringUtil::LastIndexOf(pszSrc, 'c'));
  EXPECT_EQ(4, StringUtil::LastIndexOf(pszSrc, 's'));
  EXPECT_EQ(3, StringUtil::LastIndexOf(pszSrc, ' '));
  EXPECT_EQ(2, StringUtil::LastIndexOf(pszSrc, 'd'));
  EXPECT_EQ(0, StringUtil::LastIndexOf(pszSrc, 'O'));

  EXPECT_GE(0, StringUtil::LastIndexOf(pszSrc, 'a'));
  EXPECT_GE(0, StringUtil::LastIndexOf(pszSrc, 'D'));
}


TEST(TestString_StringUtil, LastIndexOfChar_String)
{
  //--------------0123456789
  std::string src("Old school");

  EXPECT_EQ(9, StringUtil::LastIndexOf(src, 'l'));
  EXPECT_EQ(8, StringUtil::LastIndexOf(src, 'o'));
  EXPECT_EQ(6, StringUtil::LastIndexOf(src, 'h'));
  EXPECT_EQ(5, StringUtil::LastIndexOf(src, 'c'));
  EXPECT_EQ(4, StringUtil::LastIndexOf(src, 's'));
  EXPECT_EQ(3, StringUtil::LastIndexOf(src, ' '));
  EXPECT_EQ(2, StringUtil::LastIndexOf(src, 'd'));
  EXPECT_EQ(0, StringUtil::LastIndexOf(src, 'O'));

  EXPECT_GE(0, StringUtil::LastIndexOf(src, 'O'));
  EXPECT_GE(0, StringUtil::LastIndexOf(src, 'a'));
  EXPECT_GE(0, StringUtil::LastIndexOf(src, 'D'));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, LastIndexOfChar_FromIndex_StartIndexNegative_StringViewLite)
{
  StringViewLite src("old school");

  EXPECT_LT(StringUtil::LastIndexOf(src, 'l', -1), 0);
  EXPECT_LT(StringUtil::LastIndexOf(src, 'o', -2), 0);
  EXPECT_LT(StringUtil::LastIndexOf(src, 'o', -3), 0);
  EXPECT_LT(StringUtil::LastIndexOf(src, 'o', std::numeric_limits<int32_t>::min()), 0);
}

TEST(TestString_StringUtil, LastIndexOfChar_FromIndex_StartIndexNegative_CString)
{
  const char* const pszSrc = "old school";

  EXPECT_LT(StringUtil::LastIndexOf(pszSrc, 'l', -1), 0);
  EXPECT_LT(StringUtil::LastIndexOf(pszSrc, 'o', -2), 0);
  EXPECT_LT(StringUtil::LastIndexOf(pszSrc, 'o', -3), 0);
  EXPECT_LT(StringUtil::LastIndexOf(pszSrc, 'o', std::numeric_limits<int32_t>::min()), 0);
}

TEST(TestString_StringUtil, LastIndexOfChar_FromIndex_StartIndexNegative_String)
{
  std::string src("old school");

  EXPECT_LT(StringUtil::LastIndexOf(src, 'l', -1), 0);
  EXPECT_LT(StringUtil::LastIndexOf(src, 'o', -2), 0);
  EXPECT_LT(StringUtil::LastIndexOf(src, 'o', -3), 0);
  EXPECT_LT(StringUtil::LastIndexOf(src, 'o', std::numeric_limits<int32_t>::min()), 0);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, LastIndexOfChar_FromIndex_StringViewLite)
{
  //------------------0123456789
  StringViewLite src("old school");

  EXPECT_EQ(9, StringUtil::LastIndexOf(src, 'l', 9));
  EXPECT_EQ(1, StringUtil::LastIndexOf(src, 'l', 8));
  EXPECT_EQ(1, StringUtil::LastIndexOf(src, 'l', 7));
  EXPECT_EQ(1, StringUtil::LastIndexOf(src, 'l', 6));
  EXPECT_EQ(1, StringUtil::LastIndexOf(src, 'l', 5));
  EXPECT_EQ(1, StringUtil::LastIndexOf(src, 'l', 4));
  EXPECT_EQ(1, StringUtil::LastIndexOf(src, 'l', 3));
  EXPECT_EQ(1, StringUtil::LastIndexOf(src, 'l', 2));
  EXPECT_EQ(StringUtil::LastIndexOf(src, 'l', 1), 1);
  EXPECT_LT(StringUtil::LastIndexOf(src, 'l', 0), 0);

  EXPECT_EQ(8, StringUtil::LastIndexOf(src, 'o', 9));
  EXPECT_EQ(8, StringUtil::LastIndexOf(src, 'o', 8));
  EXPECT_EQ(7, StringUtil::LastIndexOf(src, 'o', 7));
  EXPECT_EQ(0, StringUtil::LastIndexOf(src, 'o', 6));
  EXPECT_EQ(0, StringUtil::LastIndexOf(src, 'o', 5));
  EXPECT_EQ(0, StringUtil::LastIndexOf(src, 'o', 4));
  EXPECT_EQ(0, StringUtil::LastIndexOf(src, 'o', 3));
  EXPECT_EQ(0, StringUtil::LastIndexOf(src, 'o', 2));
  EXPECT_EQ(0, StringUtil::LastIndexOf(src, 'o', 1));
  EXPECT_EQ(0, StringUtil::LastIndexOf(src, 'o', 0));

  EXPECT_EQ(6, StringUtil::LastIndexOf(src, 'h', 9));
  EXPECT_EQ(6, StringUtil::LastIndexOf(src, 'h', 8));
  EXPECT_EQ(6, StringUtil::LastIndexOf(src, 'h', 7));
  EXPECT_EQ(6, StringUtil::LastIndexOf(src, 'h', 6));
  EXPECT_LT(StringUtil::LastIndexOf(src, 'h', 5), 0);
  EXPECT_LT(StringUtil::LastIndexOf(src, 'h', 4), 0);
  EXPECT_LT(StringUtil::LastIndexOf(src, 'h', 3), 0);
  EXPECT_LT(StringUtil::LastIndexOf(src, 'h', 2), 0);
  EXPECT_LT(StringUtil::LastIndexOf(src, 'h', 1), 0);
  EXPECT_LT(StringUtil::LastIndexOf(src, 'h', 0), 0);
}


TEST(TestString_StringUtil, LastIndexOfChar_FromIndex_CString)
{
  //---------------0123456789
  const char* const pszSrc = "old school";

  EXPECT_EQ(9, StringUtil::LastIndexOf(pszSrc, 'l', 9));
  EXPECT_EQ(1, StringUtil::LastIndexOf(pszSrc, 'l', 8));
  EXPECT_EQ(1, StringUtil::LastIndexOf(pszSrc, 'l', 7));
  EXPECT_EQ(1, StringUtil::LastIndexOf(pszSrc, 'l', 6));
  EXPECT_EQ(1, StringUtil::LastIndexOf(pszSrc, 'l', 5));
  EXPECT_EQ(1, StringUtil::LastIndexOf(pszSrc, 'l', 4));
  EXPECT_EQ(1, StringUtil::LastIndexOf(pszSrc, 'l', 3));
  EXPECT_EQ(1, StringUtil::LastIndexOf(pszSrc, 'l', 2));
  EXPECT_EQ(StringUtil::LastIndexOf(pszSrc, 'l', 1), 1);
  EXPECT_LT(StringUtil::LastIndexOf(pszSrc, 'l', 0), 0);

  EXPECT_EQ(8, StringUtil::LastIndexOf(pszSrc, 'o', 9));
  EXPECT_EQ(8, StringUtil::LastIndexOf(pszSrc, 'o', 8));
  EXPECT_EQ(7, StringUtil::LastIndexOf(pszSrc, 'o', 7));
  EXPECT_EQ(0, StringUtil::LastIndexOf(pszSrc, 'o', 6));
  EXPECT_EQ(0, StringUtil::LastIndexOf(pszSrc, 'o', 5));
  EXPECT_EQ(0, StringUtil::LastIndexOf(pszSrc, 'o', 4));
  EXPECT_EQ(0, StringUtil::LastIndexOf(pszSrc, 'o', 3));
  EXPECT_EQ(0, StringUtil::LastIndexOf(pszSrc, 'o', 2));
  EXPECT_EQ(0, StringUtil::LastIndexOf(pszSrc, 'o', 1));
  EXPECT_EQ(0, StringUtil::LastIndexOf(pszSrc, 'o', 0));

  EXPECT_EQ(6, StringUtil::LastIndexOf(pszSrc, 'h', 9));
  EXPECT_EQ(6, StringUtil::LastIndexOf(pszSrc, 'h', 8));
  EXPECT_EQ(6, StringUtil::LastIndexOf(pszSrc, 'h', 7));
  EXPECT_EQ(6, StringUtil::LastIndexOf(pszSrc, 'h', 6));
  EXPECT_LT(StringUtil::LastIndexOf(pszSrc, 'h', 5), 0);
  EXPECT_LT(StringUtil::LastIndexOf(pszSrc, 'h', 4), 0);
  EXPECT_LT(StringUtil::LastIndexOf(pszSrc, 'h', 3), 0);
  EXPECT_LT(StringUtil::LastIndexOf(pszSrc, 'h', 2), 0);
  EXPECT_LT(StringUtil::LastIndexOf(pszSrc, 'h', 1), 0);
  EXPECT_LT(StringUtil::LastIndexOf(pszSrc, 'h', 0), 0);
}

TEST(TestString_StringUtil, LastIndexOfChar_FromIndex_String)
{
  //---------------0123456789
  std::string src("old school");

  EXPECT_EQ(9, StringUtil::LastIndexOf(src, 'l', 9));
  EXPECT_EQ(1, StringUtil::LastIndexOf(src, 'l', 8));
  EXPECT_EQ(1, StringUtil::LastIndexOf(src, 'l', 7));
  EXPECT_EQ(1, StringUtil::LastIndexOf(src, 'l', 6));
  EXPECT_EQ(1, StringUtil::LastIndexOf(src, 'l', 5));
  EXPECT_EQ(1, StringUtil::LastIndexOf(src, 'l', 4));
  EXPECT_EQ(1, StringUtil::LastIndexOf(src, 'l', 3));
  EXPECT_EQ(1, StringUtil::LastIndexOf(src, 'l', 2));
  EXPECT_EQ(StringUtil::LastIndexOf(src, 'l', 1), 1);
  EXPECT_LT(StringUtil::LastIndexOf(src, 'l', 0), 0);

  EXPECT_EQ(8, StringUtil::LastIndexOf(src, 'o', 9));
  EXPECT_EQ(8, StringUtil::LastIndexOf(src, 'o', 8));
  EXPECT_EQ(7, StringUtil::LastIndexOf(src, 'o', 7));
  EXPECT_EQ(0, StringUtil::LastIndexOf(src, 'o', 6));
  EXPECT_EQ(0, StringUtil::LastIndexOf(src, 'o', 5));
  EXPECT_EQ(0, StringUtil::LastIndexOf(src, 'o', 4));
  EXPECT_EQ(0, StringUtil::LastIndexOf(src, 'o', 3));
  EXPECT_EQ(0, StringUtil::LastIndexOf(src, 'o', 2));
  EXPECT_EQ(0, StringUtil::LastIndexOf(src, 'o', 1));
  EXPECT_EQ(0, StringUtil::LastIndexOf(src, 'o', 0));

  EXPECT_EQ(6, StringUtil::LastIndexOf(src, 'h', 9));
  EXPECT_EQ(6, StringUtil::LastIndexOf(src, 'h', 8));
  EXPECT_EQ(6, StringUtil::LastIndexOf(src, 'h', 7));
  EXPECT_EQ(6, StringUtil::LastIndexOf(src, 'h', 6));
  EXPECT_LT(StringUtil::LastIndexOf(src, 'h', 5), 0);
  EXPECT_LT(StringUtil::LastIndexOf(src, 'h', 4), 0);
  EXPECT_LT(StringUtil::LastIndexOf(src, 'h', 3), 0);
  EXPECT_LT(StringUtil::LastIndexOf(src, 'h', 2), 0);
  EXPECT_LT(StringUtil::LastIndexOf(src, 'h', 1), 0);
  EXPECT_LT(StringUtil::LastIndexOf(src, 'h', 0), 0);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, Split_Empty_StringViewLite)
{
  StringViewLite src;
  const auto result = StringUtil::Split(src, ' ', false);

  ASSERT_EQ(result.size(), 0u);
}

TEST(TestString_StringUtil, Split_Empty_CString)
{
  const char* const pszNull = nullptr;
  const auto result = StringUtil::Split(pszNull, ' ', false);

  ASSERT_EQ(result.size(), 0u);
}

TEST(TestString_StringUtil, Split_Empty_String)
{
  std::string src;
  const auto result = StringUtil::Split(src, ' ', false);

  ASSERT_EQ(result.size(), 0u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, Split_HelloWorld_StringViewLite)
{
  StringViewLite src("hello world");
  const auto result = StringUtil::Split(src, ' ', false);

  ASSERT_EQ(result.size(), 2u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "world");
}

TEST(TestString_StringUtil, Split_HelloWorld_CString)
{
  const char* const pszSrc = "hello world";
  const auto result = StringUtil::Split(pszSrc, ' ', false);

  ASSERT_EQ(result.size(), 2u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "world");
}

TEST(TestString_StringUtil, Split_HelloWorld_String)
{
  std::string src("hello world");
  const auto result = StringUtil::Split(src, ' ', false);

  ASSERT_EQ(result.size(), 2u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "world");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, Split_HelloWorld2_KeepEmpty_StringViewLite)
{
  StringViewLite src("hello  world");
  const auto result = StringUtil::Split(src, ' ', false);

  ASSERT_EQ(result.size(), 3u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "");
  ASSERT_EQ(result[2], "world");
}

TEST(TestString_StringUtil, Split_HelloWorld2_KeepEmpty_CString)
{
  const char* const pszSrc = "hello  world";
  const auto result = StringUtil::Split(pszSrc, ' ', false);

  ASSERT_EQ(result.size(), 3u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "");
  ASSERT_EQ(result[2], "world");
}

TEST(TestString_StringUtil, Split_HelloWorld2_KeepEmpty_String)
{
  std::string src("hello  world");
  const auto result = StringUtil::Split(src, ' ', false);

  ASSERT_EQ(result.size(), 3u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "");
  ASSERT_EQ(result[2], "world");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, Split_HelloWorld2_RemoveEmpty_StringViewLite)
{
  StringViewLite src("hello  world");
  const auto result = StringUtil::Split(src, ' ', true);

  ASSERT_EQ(result.size(), 2u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "world");
}

TEST(TestString_StringUtil, Split_HelloWorld2_RemoveEmpty_CString)
{
  const char* const pszSrc = "hello  world";
  const auto result = StringUtil::Split(pszSrc, ' ', true);

  ASSERT_EQ(result.size(), 2u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "world");
}

TEST(TestString_StringUtil, Split_HelloWorld2_RemoveEmpty_String)
{
  std::string src("hello  world");
  const auto result = StringUtil::Split(src, ' ', true);

  ASSERT_EQ(result.size(), 2u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "world");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, Split_MoreComplex_StringViewLite)
{
  //                  0     1      2    3  4 5    6           7
  StringViewLite src("hello world, this is a more complicated example.");
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

TEST(TestString_StringUtil, Split_MoreComplex_CString)
{
  //                         0     1      2    3  4 5    6           7
  const char* const pszSrc = "hello world, this is a more complicated example.";
  const auto result = StringUtil::Split(pszSrc, ' ', false);

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

TEST(TestString_StringUtil, Split_MoreComplex_String)
{
  //               0     1      2    3  4 5    6           7
  std::string src("hello world, this is a more complicated example.");
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

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, StringSplit_Empty_StringViewLite)
{
  StringViewLite src;
  const auto result = StringUtil::StringSplit(src, ' ', false);

  ASSERT_EQ(result.size(), 0u);
}

TEST(TestString_StringUtil, StringSplit_Empty_CString)
{
  const char* const pszNull = nullptr;
  const auto result = StringUtil::StringSplit(pszNull, ' ', false);

  ASSERT_EQ(result.size(), 0u);
}

TEST(TestString_StringUtil, StringSplit_Empty_String)
{
  std::string src;
  const auto result = StringUtil::StringSplit(src, ' ', false);

  ASSERT_EQ(result.size(), 0u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, StringSplit_HelloWorld_StringViewLite)
{
  StringViewLite src("hello world");
  const auto result = StringUtil::StringSplit(src, ' ', false);

  ASSERT_EQ(result.size(), 2u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "world");
}

TEST(TestString_StringUtil, StringSplit_HelloWorld_CString)
{
  const char* const pszSrc = "hello world";
  const auto result = StringUtil::StringSplit(pszSrc, ' ', false);

  ASSERT_EQ(result.size(), 2u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "world");
}

TEST(TestString_StringUtil, StringSplit_HelloWorld_String)
{
  std::string src("hello world");
  const auto result = StringUtil::StringSplit(src, ' ', false);

  ASSERT_EQ(result.size(), 2u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "world");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, StringSplit_HelloWorld2_KeepEmpty_StringViewLite)
{
  StringViewLite src("hello  world");
  const auto result = StringUtil::StringSplit(src, ' ', false);

  ASSERT_EQ(result.size(), 3u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "");
  ASSERT_EQ(result[2], "world");
}

TEST(TestString_StringUtil, StringSplit_HelloWorld2_KeepEmpty_CString)
{
  const char* const pszSrc = "hello  world";
  const auto result = StringUtil::StringSplit(pszSrc, ' ', false);

  ASSERT_EQ(result.size(), 3u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "");
  ASSERT_EQ(result[2], "world");
}

TEST(TestString_StringUtil, StringSplit_HelloWorld2_KeepEmpty_String)
{
  std::string src("hello  world");
  const auto result = StringUtil::StringSplit(src, ' ', false);

  ASSERT_EQ(result.size(), 3u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "");
  ASSERT_EQ(result[2], "world");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, StringSplit_HelloWorld2_RemoveEmpty_StringViewLite)
{
  StringViewLite src("hello  world");
  const auto result = StringUtil::StringSplit(src, ' ', true);

  ASSERT_EQ(result.size(), 2u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "world");
}

TEST(TestString_StringUtil, StringSplit_HelloWorld2_RemoveEmpty_CString)
{
  const char* const pszSrc = "hello  world";
  const auto result = StringUtil::StringSplit(pszSrc, ' ', true);

  ASSERT_EQ(result.size(), 2u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "world");
}

TEST(TestString_StringUtil, StringSplit_HelloWorld2_RemoveEmpty_String)
{
  std::string src("hello  world");
  const auto result = StringUtil::StringSplit(src, ' ', true);

  ASSERT_EQ(result.size(), 2u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "world");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, StringSplit_MoreComplex_StringViewLite)
{
  //                  0     1      2    3  4 5    6           7
  StringViewLite src("hello world, this is a more complicated example.");
  const auto result = StringUtil::StringSplit(src, ' ', false);

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

TEST(TestString_StringUtil, StringSplit_MoreComplex_CString)
{
  //                         0     1      2    3  4 5    6           7
  const char* const pszSrc = "hello world, this is a more complicated example.";
  const auto result = StringUtil::StringSplit(pszSrc, ' ', false);

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

TEST(TestString_StringUtil, StringSplit_MoreComplex_String)
{
  //               0     1      2    3  4 5    6           7
  std::string src("hello world, this is a more complicated example.");
  const auto result = StringUtil::StringSplit(src, ' ', false);

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

// ---------------------------------------------------------------------------------------------------------------------------------------------------

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
