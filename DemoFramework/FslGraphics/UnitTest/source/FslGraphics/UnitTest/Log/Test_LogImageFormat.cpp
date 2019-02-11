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

#include <FslBase/String/ToString.hpp>
#include <FslGraphics/Log/LogImageFormat.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  using TestLog_LogImageFormat = TestFixtureFslGraphics;
}


TEST(TestLog_LogImageFormat, Log)
{
  EXPECT_EQ(std::string("ImageFormat::Undefined (0)"), ToString(ImageFormat::Undefined));
  EXPECT_EQ(std::string("ImageFormat::Bmp (1)"), ToString(ImageFormat::Bmp));
  EXPECT_EQ(std::string("ImageFormat::DDS (2)"), ToString(ImageFormat::DDS));
  EXPECT_EQ(std::string("ImageFormat::Exr (3)"), ToString(ImageFormat::Exr));
  EXPECT_EQ(std::string("ImageFormat::Hdr (4)"), ToString(ImageFormat::Hdr));
  EXPECT_EQ(std::string("ImageFormat::Jpeg (5)"), ToString(ImageFormat::Jpeg));
  EXPECT_EQ(std::string("ImageFormat::KTX (6)"), ToString(ImageFormat::KTX));
  EXPECT_EQ(std::string("ImageFormat::Png (7)"), ToString(ImageFormat::Png));
  EXPECT_EQ(std::string("ImageFormat::Tga (8)"), ToString(ImageFormat::Tga));
}
