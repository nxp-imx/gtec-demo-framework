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
#include <FslBase/IO/FileAttributes.hpp>
#include <FslBase/Log/IO/LogPath.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestIO_FileAttributes = TestFixtureFslBase;
}


TEST(TestIO_FileAttributes, Construct_Default)
{
  IO::FileAttributes attributes;

  EXPECT_EQ(0u, attributes.Flags);
  EXPECT_FALSE(attributes.HasFlag(IO::FileAttributes::Directory));
  EXPECT_FALSE(attributes.HasFlag(IO::FileAttributes::File));
}


TEST(TestIO_FileAttributes, Construct_Directory)
{
  IO::FileAttributes attributes(IO::FileAttributes::Directory);

  EXPECT_EQ(uint32_t(IO::FileAttributes::Directory), attributes.Flags);
  EXPECT_TRUE(attributes.HasFlag(IO::FileAttributes::Directory));
  EXPECT_FALSE(attributes.HasFlag(IO::FileAttributes::File));
}


TEST(TestIO_FileAttributes, Construct_File)
{
  IO::FileAttributes attributes(IO::FileAttributes::File);

  EXPECT_EQ(uint32_t(IO::FileAttributes::File), attributes.Flags);
  EXPECT_FALSE(attributes.HasFlag(IO::FileAttributes::Directory));
  EXPECT_TRUE(attributes.HasFlag(IO::FileAttributes::File));
}


TEST(TestIO_FileAttributes, SetFlag_Directory)
{
  IO::FileAttributes attributes;

  attributes.SetFlag(IO::FileAttributes::Directory);
  EXPECT_EQ(uint32_t(IO::FileAttributes::Directory), attributes.Flags);
  EXPECT_TRUE(attributes.HasFlag(IO::FileAttributes::Directory));
  EXPECT_FALSE(attributes.HasFlag(IO::FileAttributes::File));
}


TEST(TestIO_FileAttributes, SetFlag_File)
{
  IO::FileAttributes attributes;

  attributes.SetFlag(IO::FileAttributes::File);

  EXPECT_EQ(uint32_t(IO::FileAttributes::File), attributes.Flags);
  EXPECT_FALSE(attributes.HasFlag(IO::FileAttributes::Directory));
  EXPECT_TRUE(attributes.HasFlag(IO::FileAttributes::File));
}
