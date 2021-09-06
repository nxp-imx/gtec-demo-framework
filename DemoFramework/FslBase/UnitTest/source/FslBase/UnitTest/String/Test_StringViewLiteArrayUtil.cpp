/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslBase/String/StringViewLiteArrayUtil.hpp>
#include <FslBase/Log/String/LogStringViewLite.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestString_StringViewLiteArrayUtil = TestFixtureFslBase;
}


TEST(TestString_StringViewLiteArrayUtil, CopyToAndZeroTerminate_Empty)
{
  StringViewLite strView;
  std::array<char, 12> dstArray = {'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'};
  StringViewLiteArrayUtil::CopyToAndZeroTerminate(dstArray, strView);

  EXPECT_EQ(dstArray[0], '\0');
}


TEST(TestString_StringViewLiteArrayUtil, CopyToAndZeroTerminate_Filled)
{
  StringViewLite strView("0123456789");
  std::array<char, 12> dstArray = {'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'};
  StringViewLiteArrayUtil::CopyToAndZeroTerminate(dstArray, strView);

  EXPECT_EQ(dstArray[0], '0');
  EXPECT_EQ(dstArray[1], '1');
  EXPECT_EQ(dstArray[2], '2');
  EXPECT_EQ(dstArray[3], '3');
  EXPECT_EQ(dstArray[4], '4');
  EXPECT_EQ(dstArray[5], '5');
  EXPECT_EQ(dstArray[6], '6');
  EXPECT_EQ(dstArray[7], '7');
  EXPECT_EQ(dstArray[8], '8');
  EXPECT_EQ(dstArray[9], '9');
  EXPECT_EQ(dstArray[10], '\0');
}


TEST(TestString_StringViewLiteArrayUtil, CopyToAndZeroTerminate_SubStr)
{
  StringViewLite strView("0123456789");
  std::array<char, 12> dstArray = {'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'};
  StringViewLiteArrayUtil::CopyToAndZeroTerminate(dstArray, strView.substr(2, 2));

  EXPECT_EQ(dstArray[0], '2');
  EXPECT_EQ(dstArray[1], '3');
  EXPECT_EQ(dstArray[2], '\0');
}


TEST(TestString_StringViewLiteArrayUtil, CopyToAndZeroTerminate_FilledOverflow)
{
  StringViewLite strView("0123456789ABCDEFG");
  std::array<char, 12> dstArray = {'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'};
  StringViewLiteArrayUtil::CopyToAndZeroTerminate(dstArray, strView);

  EXPECT_EQ(dstArray[0], '0');
  EXPECT_EQ(dstArray[1], '1');
  EXPECT_EQ(dstArray[2], '2');
  EXPECT_EQ(dstArray[3], '3');
  EXPECT_EQ(dstArray[4], '4');
  EXPECT_EQ(dstArray[5], '5');
  EXPECT_EQ(dstArray[6], '6');
  EXPECT_EQ(dstArray[7], '7');
  EXPECT_EQ(dstArray[8], '8');
  EXPECT_EQ(dstArray[9], '9');
  EXPECT_EQ(dstArray[10], 'A');
  EXPECT_EQ(dstArray[11], '\0');
}


TEST(TestString_StringViewLiteArrayUtil, ToArray_Empty)
{
  StringViewLite strView;
  std::array<char, 12> dstArray = StringViewLiteArrayUtil::ToArray<12>(strView);

  EXPECT_EQ(dstArray[0], '\0');
}


TEST(TestString_StringViewLiteArrayUtil, ToArray_Filled)
{
  StringViewLite strView("0123456789");
  std::array<char, 12> dstArray = StringViewLiteArrayUtil::ToArray<12>(strView);

  EXPECT_EQ(dstArray[0], '0');
  EXPECT_EQ(dstArray[1], '1');
  EXPECT_EQ(dstArray[2], '2');
  EXPECT_EQ(dstArray[3], '3');
  EXPECT_EQ(dstArray[4], '4');
  EXPECT_EQ(dstArray[5], '5');
  EXPECT_EQ(dstArray[6], '6');
  EXPECT_EQ(dstArray[7], '7');
  EXPECT_EQ(dstArray[8], '8');
  EXPECT_EQ(dstArray[9], '9');
  EXPECT_EQ(dstArray[10], '\0');
}


TEST(TestString_StringViewLiteArrayUtil, ToArray_SubStr)
{
  StringViewLite strView("0123456789");
  std::array<char, 12> dstArray = StringViewLiteArrayUtil::ToArray<12>(strView.substr(2, 2));

  EXPECT_EQ(dstArray[0], '2');
  EXPECT_EQ(dstArray[1], '3');
  EXPECT_EQ(dstArray[2], '\0');
}


TEST(TestString_StringViewLiteArrayUtil, ToArray_FilledOverflow)
{
  StringViewLite strView("0123456789ABCDEFG");
  std::array<char, 12> dstArray = StringViewLiteArrayUtil::ToArray<12>(strView);

  EXPECT_EQ(dstArray[0], '0');
  EXPECT_EQ(dstArray[1], '1');
  EXPECT_EQ(dstArray[2], '2');
  EXPECT_EQ(dstArray[3], '3');
  EXPECT_EQ(dstArray[4], '4');
  EXPECT_EQ(dstArray[5], '5');
  EXPECT_EQ(dstArray[6], '6');
  EXPECT_EQ(dstArray[7], '7');
  EXPECT_EQ(dstArray[8], '8');
  EXPECT_EQ(dstArray[9], '9');
  EXPECT_EQ(dstArray[10], 'A');
  EXPECT_EQ(dstArray[11], '\0');
}
