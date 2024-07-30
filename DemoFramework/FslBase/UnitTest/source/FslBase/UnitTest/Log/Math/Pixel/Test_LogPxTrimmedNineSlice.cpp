/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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
#include <FslBase/Log/Math/Pixel/LogPxTrimmedNineSlice.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <sstream>
#include <vector>

using namespace Fsl;

namespace
{
  using TestLog_MathPixel_LogPxTrimmedNineSlice = TestFixtureFslBase;
}


TEST(TestLog_MathPixel_LogPxTrimmedNineSlice, Log)
{
  constexpr PxTrimmedNineSlice Value(PxSize2D::Create(1, 2), PxThicknessF::Create(3.0f, 4.0f, 5.0f, 6.0f),
                                     PxThicknessF::Create(7.0f, 8.0f, 9.0f, 10.0f), PxThickness::Create(11, 12, 13, 14));

  std::stringstream stream;
  stream << Value;
  EXPECT_EQ(std::string("{SizePx={Width=1 Height=2} TrimMarginPxf={Left=3 Top=4 Right=5 Bottom=6} TrimmedNineSlicePxf={Left=7 Top=8 Right=9 "
                        "Bottom=10} ContentMarginPx={Left=11 Top=12 Right=13 Bottom=14}}"),
            stream.str());
}
