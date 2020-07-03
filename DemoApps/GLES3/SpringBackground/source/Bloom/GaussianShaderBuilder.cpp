/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include "GaussianShaderBuilder.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/String/StringUtil.hpp>
#include <array>
#include <cassert>
#include <iomanip>
#include <sstream>

namespace Fsl
{
  // To generate Pascal's triangle, write it out as a sequence of rows.
  // Each number is the sum of two other numbers: the number directly above, and the number above and to the left. It's like this:
  //  1. 001
  //  2. 001 001
  //  3. 001 002 001
  //  4. 001 003 003 001
  //  5. 001 004 006 004 001
  //  6. 001 005 010 010 005 001
  //  7. 001 006 015 020 015 006 001
  //  8. 001 007 021 035 035 021 007 001
  //  9. 001 008 028 056 070 056 028 008 001
  //
  // To determine the weights of an n x n kernel, select the row that has n numbers in it, then convolve it.
  // This basically means do a vector multiply with its own transpose.


  namespace
  {
    constexpr std::size_t CalcLinearKernelLength(const std::size_t x)
    {
      return ((((x)-1) / 2) + 1);
    }

    struct LinearData
    {
      float Weight{0};
      float Offset{0};

      LinearData() = default;

      LinearData(const float weight, const float offset)
        : Weight(weight)
        , Offset(offset)
      {
      }
    };

    void BuildLinearKernel(LinearData* pLinearKernel, const std::size_t linearKernelLength, const float* const pActualKernel,
                           const std::size_t actualKernelLength)
    {
      assert(pLinearKernel != nullptr);
      assert(pActualKernel != nullptr);
      assert(linearKernelLength == CalcLinearKernelLength(actualKernelLength));
      assert((actualKernelLength & 1) != 0);
      assert(((actualKernelLength - 1) % 2) == 0);

      // Weight:       1         4
      //          |----A----|----B----|  -> 5
      // Offset:     1/5 A     4/5 B  = 0,8 B + (pixel offset)
      //               A------X-B

      const std::size_t srcMiddleIndex = (actualKernelLength - 1) / 2;
      const std::size_t dstMiddleIndex = (linearKernelLength - 1) / 2;
      std::size_t srcIndex = srcMiddleIndex + 1;
      for (std::size_t i = dstMiddleIndex + 1; i < linearKernelLength; ++i)
      {
        const auto value = pActualKernel[srcIndex] + pActualKernel[srcIndex + 1];
        const float offsetAT = pActualKernel[srcIndex] * float(srcIndex - srcMiddleIndex);
        const float offsetBT = pActualKernel[srcIndex + 1] * float(srcIndex + 1 - srcMiddleIndex);
        const auto offset = (offsetAT + offsetBT) / value;

        pLinearKernel[i].Weight = value;
        pLinearKernel[i].Offset = offset;
        assert((int32_t(dstMiddleIndex) - int32_t(i - dstMiddleIndex)) >= 0);
        pLinearKernel[(dstMiddleIndex) - (i - dstMiddleIndex)].Weight = value;
        pLinearKernel[(dstMiddleIndex) - (i - dstMiddleIndex)].Offset = offset;
        srcIndex += 2;
      }
      pLinearKernel[dstMiddleIndex].Weight = pActualKernel[srcMiddleIndex];
    }


    void NormalizeKernel(LinearData* pKernel, const std::size_t kernelLength, const float weight)
    {
      assert(pKernel != nullptr);

      float sum = 0;
      for (std::size_t i = 0; i < kernelLength; ++i)
      {
        sum += pKernel[i].Weight;
      }

      if (weight != 1.0f)
      {
        sum *= weight;
      }

      for (std::size_t i = 0; i < kernelLength; ++i)
      {
        pKernel[i].Weight /= sum;
      }
    }

    void StringReplaceWithValue(std::string& rStr, const char* const oldText, const float value)
    {
      std::stringstream str;
      str << std::setprecision(13) << std::fixed;
      str << value;
      StringUtil::Replace(rStr, oldText, str.str());
    }
  }

  std::string GaussianShaderBuilder::Build5x5(const std::string& strTemplate, const float kernelWeightMod)
  {
    constexpr const std::array<float, 5> actualKernel = {1, 4, 6, 4, 1};

    std::array<LinearData, CalcLinearKernelLength(actualKernel.size())> linearKernel{};

    BuildLinearKernel(linearKernel.data(), linearKernel.size(), actualKernel.data(), actualKernel.size());

    // Normalize kernel coefficients
    NormalizeKernel(linearKernel.data(), linearKernel.size(), kernelWeightMod);


    std::string res = strTemplate;
    StringReplaceWithValue(res, "##REPLACE0_WEIGHT##", linearKernel[0].Weight);
    StringReplaceWithValue(res, "##REPLACE0_OFFSET##", linearKernel[0].Offset);
    StringReplaceWithValue(res, "##REPLACE1_WEIGHT##", linearKernel[1].Weight);

    // FSLLOG3_INFO(res);
    return res;
  }


  std::string GaussianShaderBuilder::Build9x9(const std::string& strTemplate, const float kernelWeightMod)
  {
    // yes we could take advantage of the fact that the kernel is mirrored around the middle but for now its not important and
    // the code is more 'clear' when we just ignore it
    constexpr const std::array<float, 9> actualKernel = {1, 8, 28, 56, 70, 56, 28, 8, 1};
    // const std::array<float, 9>  = { 0, 5, 0, 5, 5, 5, 0, 5, 0 };
    // const std::array<float, 9>  = { 5, 0, 5, 0, 5, 0, 5, 0, 5 };
    // const std::array<float, 9>  = { 5, 5, 5, 5, 5, 5, 5, 5, 5 };

    std::array<LinearData, CalcLinearKernelLength(actualKernel.size())> linearKernel{};

    BuildLinearKernel(linearKernel.data(), linearKernel.size(), actualKernel.data(), actualKernel.size());

    // Normalize kernel coefficients
    NormalizeKernel(linearKernel.data(), linearKernel.size(), kernelWeightMod);


    std::string res = strTemplate;
    StringReplaceWithValue(res, "##REPLACE0_WEIGHT##", linearKernel[0].Weight);
    StringReplaceWithValue(res, "##REPLACE0_OFFSET##", linearKernel[0].Offset);
    StringReplaceWithValue(res, "##REPLACE1_WEIGHT##", linearKernel[1].Weight);
    StringReplaceWithValue(res, "##REPLACE1_OFFSET##", linearKernel[1].Offset);
    StringReplaceWithValue(res, "##REPLACE2_WEIGHT##", linearKernel[2].Weight);
    return res;
  }
}
