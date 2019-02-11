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
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using Test_Exceptions = TestFixtureFslBase;
}


TEST(Test_Exceptions, UsageErrorException_Construct1)
{
  UsageErrorException ex;

  // Mainly to avoid warning
  EXPECT_THROW(throw ex, UsageErrorException);
}


TEST(Test_Exceptions, UsageErrorException_Construct2)
{
  const std::string message("hello");
  UsageErrorException ex(message);
  EXPECT_EQ(message, ex.what());
}


TEST(Test_Exceptions, UnknownTypeException_Construct1)
{
  const std::string message("hello");
  UnknownTypeException ex(message);

  EXPECT_EQ(message, ex.what());
}


TEST(Test_Exceptions, NotFoundException_Construct1)
{
  const std::string message("hello");
  NotFoundException ex(message);

  EXPECT_EQ(message, ex.what());
}


TEST(Test_Exceptions, DirectoryNotFoundException_Construct1)
{
  const std::string message("hello");
  DirectoryNotFoundException ex(message);

  EXPECT_EQ(message, ex.what());
}


TEST(Test_Exceptions, InitFailedException_Construct1)
{
  const std::string message("hello");
  InitFailedException ex(message);

  EXPECT_EQ(message, ex.what());
}


TEST(Test_Exceptions, NotImplementedException_Construct1)
{
  const std::string message("hello");
  NotImplementedException ex(message);

  EXPECT_EQ(message, ex.what());
}


TEST(Test_Exceptions, NotImplementedException_Construct2)
{
  NotImplementedException ex;

  // Mainly to avoid warning
  EXPECT_THROW(throw ex, NotImplementedException);
}


TEST(Test_Exceptions, NotSupportedException_Construct1)
{
  const std::string message("hello");
  NotSupportedException ex(message);

  EXPECT_EQ(message, ex.what());
}


TEST(Test_Exceptions, NotSupportedException_Construct2)
{
  NotSupportedException ex;

  // Mainly to avoid warning
  EXPECT_THROW(throw ex, NotSupportedException);
}


TEST(Test_Exceptions, IndexOutOfRangeException_Construct1)
{
  const std::string message("hello");
  IndexOutOfRangeException ex(message);

  EXPECT_EQ(message, ex.what());
}


TEST(Test_Exceptions, IndexOutOfRangeException_Construct2)
{
  IndexOutOfRangeException ex;

  // Mainly to avoid warning
  EXPECT_THROW(throw ex, IndexOutOfRangeException);
}


TEST(Test_Exceptions, GraphicsException_Construct1)
{
  const std::string message("hello");
  GraphicsException ex(message);

  EXPECT_EQ(message, ex.what());
}


TEST(Test_Exceptions, IOException_Construct1)
{
  const std::string message("hello");
  IOException ex(message);

  EXPECT_EQ(message, ex.what());
}


TEST(Test_Exceptions, OverflowException_Construct1)
{
  const std::string message("hello");
  OverflowException ex(message);

  EXPECT_EQ(message, ex.what());
}


TEST(Test_Exceptions, FormatException_Construct1)
{
  const std::string message("hello");
  FormatException ex(message);

  EXPECT_EQ(message, ex.what());
}


TEST(Test_Exceptions, InvalidUTF8StringException_Construct1)
{
  const std::string message("hello");
  InvalidUTF8StringException ex(message);

  EXPECT_EQ(message, ex.what());
}


TEST(Test_Exceptions, InvalidFormatException_Construct1)
{
  const std::string message("hello");
  InvalidFormatException ex(message);

  EXPECT_EQ(message, ex.what());
}
