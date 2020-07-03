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

#include <FslBase/IO/PathView.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/IO/LogPathView.hpp>
#include <FslBase/Log/String/LogStringViewLite.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestIO_PathView = TestFixtureFslBase;
}


TEST(TestIO_PathView, Construct_Default)
{
  IO::PathView path;

  EXPECT_TRUE(path.empty());
}


TEST(TestIO_PathView, Construct_Default_StringViewLite_NoCheck)
{
  StringViewLite strEmpty;
  IO::PathView path(strEmpty, OptimizationCheckFlag::NoCheck);

  EXPECT_TRUE(path.empty());
}


TEST(TestIO_PathView, Construct_Default_Null_NoCheck)
{
  IO::PathView path(nullptr, 0u, OptimizationCheckFlag::NoCheck);
  EXPECT_TRUE(path.empty());
}

TEST(TestIO_PathView, Construct_Default_StringViewLite)
{
  StringViewLite strEmpty;
  IO::PathView path(strEmpty);

  EXPECT_TRUE(path.empty());
}

TEST(TestIO_PathView, Construct_Default_BasicString)
{
  const char* const pszNull = nullptr;
  IO::PathView path(pszNull, 0u);

  EXPECT_TRUE(path.empty());
}


TEST(TestIO_PathView, Construct_Default_CString)
{
  const char* const pszNull = nullptr;
  IO::PathView path(pszNull);

  EXPECT_TRUE(path.empty());
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------

namespace
{
  IO::PathView Construct_BasicString_Backslash()
  {
    StringViewLite str("hello\\world");
    return IO::PathView(str);
  }

  IO::PathView Construct_CString_Backslash()
  {
    const char* const psz = "hello\\world";
    return IO::PathView(psz);
  }
}

TEST(TestIO_PathView, Construct_StringViewLite_Backslash)
{
  EXPECT_THROW(Construct_BasicString_Backslash(), PathFormatErrorException);
}

TEST(TestIO_PathView, Construct_BasicString_Backslash)
{
  const char* const psz = "h\\ello world";
  EXPECT_THROW(IO::PathView(psz, 2u), PathFormatErrorException);
}


TEST(TestIO_PathView, Construct_CString_Backslash)
{
  EXPECT_THROW(Construct_CString_Backslash(), PathFormatErrorException);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestIO_PathView, Construct_CString_NoCheck)
{
  const char* const psz = "hello";
  IO::PathView path(psz, 5u, OptimizationCheckFlag::NoCheck);

  EXPECT_EQ(5u, path.size());
  EXPECT_EQ(psz, path);
}


TEST(TestIO_PathView, Construct_StringViewLite_NoCheck)
{
  StringViewLite str("hello");
  IO::PathView path(str, OptimizationCheckFlag::NoCheck);

  EXPECT_EQ(5u, path.size());
  EXPECT_EQ(str, path);
}


TEST(TestIO_PathView, Construct_Null_Basic_NoCheck)
{
  const char* const psz = "hello";
  IO::PathView path(psz, 2u, OptimizationCheckFlag::NoCheck);

  EXPECT_EQ(2u, path.size());
  EXPECT_EQ("he", path);
}

TEST(TestIO_PathView, Construct_StringViewLite)
{
  StringViewLite str("hello");
  IO::PathView path(str);

  EXPECT_EQ(5u, path.size());
  EXPECT_EQ(str, path);
}

TEST(TestIO_PathView, Construct_BasicString)
{
  const char* const psz = "hello";
  IO::PathView path(psz, 2u);

  EXPECT_EQ(2u, path.size());
  EXPECT_EQ("he", path);
}


TEST(TestIO_PathView, Construct_CString)
{
  const char* const psz = "hello";
  IO::PathView path(psz);

  EXPECT_EQ(5u, path.size());
  EXPECT_EQ(psz, path);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestIO_PathView, SubPath_Full)
{
  //                     01234567890
  IO::PathView pathView("hello world");

  auto pathView2 = pathView.subpath();
  EXPECT_EQ(11u, pathView2.size());
  EXPECT_EQ("hello world", pathView2);
}

TEST(TestIO_PathView, SubPath_PartialEnd)
{
  //                     01234567890
  IO::PathView pathView("hello world");

  auto pathView2 = pathView.subpath(6);
  EXPECT_EQ(5u, pathView2.size());
  EXPECT_EQ("world", pathView2);
}

TEST(TestIO_PathView, SubPath_Partial)
{
  //                     01234567890
  IO::PathView pathView("hello world");

  auto pathView2 = pathView.subpath(6, 0);
  EXPECT_EQ(0u, pathView2.size());
  EXPECT_EQ("", pathView2);

  auto pathView3 = pathView.subpath(6, 3);
  EXPECT_EQ(3u, pathView3.size());
  EXPECT_EQ("wor", pathView3);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator==
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestIO_PathView, OpEqual_PathView_PathView)
{
  //                     01234567890
  IO::PathView pathView("hello world");

  EXPECT_TRUE(pathView == IO::PathView("hello world"));
}


TEST(TestIO_PathView, OpEqual_PathView_StringViewLite)
{
  //                     01234567890
  IO::PathView pathView("hello world");

  EXPECT_TRUE(pathView == StringViewLite("hello world"));
}


TEST(TestIO_PathView, OpEqual_PathView_CString)
{
  //                     01234567890
  IO::PathView pathView("hello world");

  EXPECT_TRUE(pathView == StringViewLite("hello world"));
}

TEST(TestIO_PathView, OpEqual_StringViewLite_PathView)
{
  //                     01234567890
  IO::PathView pathView("hello world");

  EXPECT_TRUE(StringViewLite("hello world") == pathView);
}


TEST(TestIO_PathView, OpEqual_CString_PathView)
{
  //                     01234567890
  IO::PathView pathView("hello world");

  EXPECT_TRUE(StringViewLite("hello world") == pathView);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
