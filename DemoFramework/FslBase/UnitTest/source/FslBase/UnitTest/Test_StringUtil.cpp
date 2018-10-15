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
  using TestFixtureFslBase_TestStringUtil = TestFixtureFslBase;
}




TEST(TestFixtureFslBase_TestStringUtil, Empty)
{
  std::string src;
  const auto result = StringUtil::Split(src, ' ', false);

  ASSERT_EQ(result.size(), 0u);
}


TEST(TestFixtureFslBase_TestStringUtil, HelloWorld)
{
  std::string src = "hello world";
  const auto result = StringUtil::Split(src, ' ', false);

  ASSERT_EQ(result.size(), 2u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "world");
}


TEST(TestFixtureFslBase_TestStringUtil, HelloWorld2_KeepEmpty)
{
  std::string src = "hello  world";
  const auto result = StringUtil::Split(src, ' ', false);

  ASSERT_EQ(result.size(), 3u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "");
  ASSERT_EQ(result[2], "world");
}



TEST(TestFixtureFslBase_TestStringUtil, HelloWorld2_RemoveEmpty)
{
  std::string src = "hello  world";
  const auto result = StringUtil::Split(src, ' ', true);

  ASSERT_EQ(result.size(), 2u);
  ASSERT_EQ(result[0], "hello");
  ASSERT_EQ(result[1], "world");
}



TEST(TestFixtureFslBase_TestStringUtil, MoreComplex)
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
