#ifndef FSLGRAPHICS_UNITTEST_HELPER_TESTCUSTOMCHECKS_HPP
#define FSLGRAPHICS_UNITTEST_HELPER_TESTCUSTOMCHECKS_HPP
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

#include <FslBase/Log/Math/Pixel/FmtPxExtent3D.hpp>
#include <FslBase/Log/Math/Pixel/LogPxExtent3D.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphics/UnitTest/Helper/Log/FmtPixelUInt8.hpp>
#include <FslGraphics/UnitTest/Helper/Log/LogPixelUInt8.hpp>
#include <FslUnitTest/FormatDebugInfo.hpp>
#include <FslUnitTest/TestCustomChecks.hpp>

namespace Fsl
{
  inline ::testing::AssertionResult IsBitmap(const Texture& tex)
  {
    // const auto currentValue = tex.GetExtent();
    if (tex.GetBlobCount() == 1 && tex.GetLevels() == 1 && tex.GetFaces() == 1 && tex.GetLayers() == 1)
    {
      return ::testing::AssertionSuccess();
    }

    std::string strError = fmt::format("Texture is not a bitmap");

    FormatDebugInfo<std::size_t>(strError, "GetBlobCount() was: {0} expected: {1}", tex.GetBlobCount(), 1);
    FormatDebugInfo<uint32_t>(strError, "GetLevels() was: {0} expected: {1}", tex.GetLevels(), 1);
    FormatDebugInfo<uint32_t>(strError, "GetFaces() was: {0} expected: {1}", tex.GetFaces(), 1);
    FormatDebugInfo<uint32_t>(strError, "GetLayers() was: {0} expected: {1}", tex.GetLayers(), 1);

    return ::testing::AssertionFailure() << strError;
  }


  inline ::testing::AssertionResult ExtentEquals(const Texture& tex, const PxExtent3D& expectedValue)
  {
    const auto currentValue = tex.GetExtent();
    if (currentValue == expectedValue)
    {
      return ::testing::AssertionSuccess();
    }

    std::string strError = fmt::format("Extent {0} did not match the expected value of {1}", currentValue, expectedValue);

    return ::testing::AssertionFailure() << strError;
  }


  inline ::testing::AssertionResult ExtentEquals(const Texture& tex, const uint32_t width, const uint32_t height, const uint32_t depth)
  {
    return ExtentEquals(tex, PxExtent3D(width, height, depth));
  }
}

#endif
