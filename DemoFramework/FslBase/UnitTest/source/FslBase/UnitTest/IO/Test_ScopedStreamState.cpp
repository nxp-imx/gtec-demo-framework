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
#include <FslBase/IO/ScopedStreamState.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBaseContent.hpp>
#include <array>
#include <limits>
#include <sstream>
#include <vector>

using namespace Fsl;

namespace
{
  using TestIO_ScopedStreamState = TestFixtureFslBaseContent;
}


TEST(TestIO_ScopedStreamState, Log)
{
  std::stringstream stream;
  stream << "hello: " << 1337;
  EXPECT_EQ(std::string("hello: 1337"), stream.str());
  stream << " " << std::hex << 42;
  stream << " " << 10;
  stream << std::dec;
  EXPECT_EQ(std::string("hello: 1337 2a a"), stream.str());
  stream << " " << 10;
  EXPECT_EQ(std::string("hello: 1337 2a a 10"), stream.str());
  {
    IO::ScopedStreamState scoped(stream);
    stream << " " << std::hex << 42;
    stream << " " << 10;
  }
  stream << " " << 1337;
  EXPECT_EQ(std::string("hello: 1337 2a a 10 2a a 1337"), stream.str());
}
