/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics3D/Procedural/IndexUtilBase.hpp>
#include <algorithm>
#include <cassert>
#include <limits>

namespace Fsl::Procedural
{
  std::size_t IndexUtilBase::CalcAppendMinimumCapacity(const std::size_t currentIndexCount, const std::size_t srcIndexCount,
                                                       const PrimitiveType primitiveType)
  {
    switch (primitiveType)
    {
    case PrimitiveType::LineStrip:
      // If the linestrips can be merged (then it would require the dstIndex[end] == srcIndex[0] to be equal)
      return currentIndexCount > 0 ? std::max(currentIndexCount + srcIndexCount - 1, static_cast<std::size_t>(0)) : srcIndexCount;
    case PrimitiveType::LineList:
    case PrimitiveType::TriangleList:
      return currentIndexCount + srcIndexCount;
    case PrimitiveType::TriangleStrip:
      if (currentIndexCount != 0)
      {
        return (currentIndexCount & 1) == 0 ? currentIndexCount + 4 + srcIndexCount : currentIndexCount + 5 + srcIndexCount;
      }
      else
      {
        return srcIndexCount;
      }
    default:
      throw NotImplementedException("Unknown primitive type");
    }
  }


  std::size_t IndexUtilBase::CalcAppendInstancesRequiredCapacity(const std::size_t currentIndexCount, const std::size_t srcIndexCount,
                                                                 const std::size_t instanceCount, const PrimitiveType primitiveType)
  {
    if (srcIndexCount <= 2)
    {
      throw std::invalid_argument("we expect at least two indices");
    }

    switch (primitiveType)
    {
    case PrimitiveType::LineStrip:
      return (currentIndexCount > 0 ? currentIndexCount + srcIndexCount - 1 : srcIndexCount);
    case PrimitiveType::LineList:
    case PrimitiveType::TriangleList:
      return currentIndexCount + (srcIndexCount * instanceCount);
    case PrimitiveType::TriangleStrip:
      {
        const std::size_t mod = ((srcIndexCount & 1) == 0 ? 4 : 5);
        std::size_t startCapacity = 0;
        if (currentIndexCount == 0)
        {
          startCapacity = 0;
        }
        else
        {
          startCapacity = currentIndexCount + ((currentIndexCount & 1) == 0 ? 4 : 5);
        }
        return startCapacity + (instanceCount * srcIndexCount) + (mod * (instanceCount - 1));
      }
    default:
      throw NotImplementedException();
    }
  }


  std::size_t IndexUtilBase::CalcAppendMaxInstancesThatFit(const std::size_t currentIndexCount, const std::size_t srcIndexCount,
                                                           const std::size_t indexCapacity, const PrimitiveType primitiveType)
  {
    if (srcIndexCount <= 2)
    {
      throw std::invalid_argument("we expect at least two indices");
    }

    if (indexCapacity < srcIndexCount)
    {
      return 0;
    }

    switch (primitiveType)
    {
    case PrimitiveType::LineList:
    case PrimitiveType::TriangleList:
      return (indexCapacity - currentIndexCount) / srcIndexCount;
    case PrimitiveType::TriangleStrip:
      {
        // capacity = (instanceCount * srcIndexCount) + (4 * (instances-1))
        // capacity = -4 + (instanceCount * (4+srcIndexCount))
        // instanceCount = (capacity + 4) / (4+srcIndexCount)
        const uint64_t capacity = (indexCapacity - (currentIndexCount > 0 ? currentIndexCount + ((currentIndexCount & 1) == 0 ? 4 : 5) : 0));
        const uint64_t res = ((srcIndexCount & 1) == 0 ? (capacity + 4) / (4 + srcIndexCount) : (capacity + 5) / (5 + srcIndexCount));
        assert(res <= std::numeric_limits<std::size_t>::max());
        return static_cast<std::size_t>(res);
      }
    case PrimitiveType::LineStrip:
      // If the linestrips can be merged (then it would require the dstIndex[end] == srcIndex[0] to be equal)
      return (srcIndexCount > 1 ? (indexCapacity - currentIndexCount) / (srcIndexCount - 1) : 0);
    default:
      throw NotImplementedException();
    }
  }
}
