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

#include <FslBase/IO/PathViewHelper.hpp>
#include <FslBase/Log/IO/LogPath.hpp>
#include <FslBase/Log/IO/LogPathView.hpp>
#include <FslBase/Log/String/LogStringViewLite.hpp>
#include <FslBase/Log/String/LogUTF8String.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestIO_PathViewHelper = TestFixtureFslBase;
}


TEST(TestIO_PathViewHelper, ToString_Empty)
{
  IO::PathView pathViewer;
  auto str = IO::PathViewHelper::ToString(pathViewer);

  EXPECT_EQ(0u, str.size());
  EXPECT_EQ("", str);
}


TEST(TestIO_PathViewHelper, ToString)
{
  IO::PathView pathViewer("hello");
  auto str = IO::PathViewHelper::ToString(pathViewer);

  EXPECT_EQ(5u, str.size());
  EXPECT_EQ(pathViewer, str);
}
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator==
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestIO_PathView, OpEqual_PathView_String)
{
  //                     01234567890
  IO::PathView pathView("hello world");

  EXPECT_TRUE(pathView == std::string("hello world"));
}


TEST(TestIO_PathView, OpEqual_PathView_UTF8String)
{
  //                     01234567890
  IO::PathView pathView("hello world");

  EXPECT_TRUE(pathView == UTF8String("hello world"));
}

TEST(TestIO_PathView, OpEqual_PathView_Path)
{
  //                     01234567890
  IO::PathView pathView("hello world");

  EXPECT_TRUE(pathView == IO::Path("hello world"));
}


TEST(TestIO_PathView, OpEqual_String_PathView)
{
  //                     01234567890
  IO::PathView pathView("hello world");

  EXPECT_TRUE(std::string("hello world") == pathView);
}


TEST(TestIO_PathView, OpEqual_UTF8String_PathView)
{
  //                     01234567890
  IO::PathView pathView("hello world");

  EXPECT_TRUE(UTF8String("hello world") == pathView);
}

TEST(TestIO_PathView, OpEqual_Path_PathView)
{
  //                     01234567890
  IO::PathView pathView("hello world");

  EXPECT_TRUE(IO::Path("hello world") == pathView);
}
