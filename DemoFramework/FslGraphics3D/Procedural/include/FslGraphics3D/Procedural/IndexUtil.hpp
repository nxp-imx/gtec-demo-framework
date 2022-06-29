#ifndef FSLGRAPHICS3D_PROCEDURAL_INDEXUTIL_HPP
#define FSLGRAPHICS3D_PROCEDURAL_INDEXUTIL_HPP
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
#include <FslGraphics3D/Procedural/IndexUtil_fwd.hpp>
#include <cassert>
#include <limits>

namespace Fsl::Procedural
{
  // The class is defined in IndexUtil_fwd.hpp and this file contains the implementation of the 'templated' functions of IndexUtil.
  // This allows us to use templates without 'spreading' dependencies to files that don't need it by allowing them to only include the _fwd file

  template <typename TIndex>
  std::size_t IndexUtil::Merge(std::vector<TIndex>& rDstIndices, const std::size_t dstIndex, const std::vector<TIndex>& srcIndices, const TIndex mod,
                               const PrimitiveType primitiveType)
  {
    return Merge(rDstIndices, dstIndex, srcIndices, 0, srcIndices.size(), mod, primitiveType);
  }


  template <typename TIndex>
  std::size_t IndexUtil::Merge(std::vector<TIndex>& rDstIndices, const std::size_t dstIndex, const std::vector<TIndex>& srcIndices,
                               const std::size_t srcStartIndex, const std::size_t srcCount, const TIndex mod, const PrimitiveType primitiveType)
  {
    switch (primitiveType)
    {
    case PrimitiveType::LineList:
    case PrimitiveType::TriangleList:
      return MergeListIndices<TIndex>(rDstIndices, dstIndex, srcIndices, srcStartIndex, srcCount, mod);
    case PrimitiveType::TriangleStrip:
      return MergeTriangleStripIndices<TIndex>(rDstIndices, dstIndex, srcIndices, srcStartIndex, srcCount, mod);
    case PrimitiveType::LineStrip:
      throw NotSupportedException("LineStrip indices can not be merged");
    default:
      throw NotSupportedException("Unknown primitive type");
    }
  }


  template <typename TIndex>
  std::size_t IndexUtil::MergeListIndices(std::vector<TIndex>& rDstIndices, const std::size_t dstIndex, const std::vector<TIndex>& srcIndices,
                                          const TIndex mod)
  {
    return MergeListIndices(rDstIndices, dstIndex, srcIndices, 0, srcIndices.size(), mod);
  }


  template <typename TIndex>
  std::size_t IndexUtil::MergeListIndices(std::vector<TIndex>& rDstIndices, const std::size_t dstIndex, const std::vector<TIndex>& srcIndices,
                                          const std::size_t srcStartIndex, const std::size_t srcCount, const TIndex mod)
  {
    if (/*dstIndex < 0 || srcStartIndex < 0 || srcCount < 0 ||*/ std::size_t(srcStartIndex + srcCount) > srcIndices.size() ||
        std::size_t(dstIndex + srcCount) > rDstIndices.size())
    {
      if (std::size_t(srcStartIndex + srcCount) > srcIndices.size())
      {
        throw std::invalid_argument("(srcStartIndex + srcCount) should be <= srcIndices.size()");
      }

      throw std::invalid_argument("(dstIndex + srcCount) should be <= rDstIndices.size()");
    }

    for (std::size_t i = 0; i < srcCount; ++i)
    {
      assert((srcIndices[srcStartIndex + i] + mod) >= 0);
      assert((srcIndices[srcStartIndex + i] + mod) <= std::numeric_limits<TIndex>::max());
      rDstIndices[dstIndex + i] = srcIndices[srcStartIndex + i] + mod;
    }
    return srcCount;
  }


  template <typename TIndex>
  std::size_t IndexUtil::MergeTriangleStripIndices(std::vector<TIndex>& rDstIndices, const std::size_t dstIndex,
                                                   const std::vector<TIndex>& srcIndices, const TIndex mod)
  {
    return MergeTriangleStripIndices(rDstIndices, dstIndex, srcIndices, 0, srcIndices.size(), mod);
  }


  template <typename TIndex>
  std::size_t IndexUtil::MergeTriangleStripIndices(std::vector<TIndex>& rDstIndices, const std::size_t dstIndex,
                                                   const std::vector<TIndex>& srcIndices, const std::size_t srcStartIndex, const std::size_t srcCount,
                                                   const TIndex mod)
  {
    if (/*dstIndex < 0 || srcStartIndex < 0 || srcCount < 0 ||*/ std::size_t(srcStartIndex + srcCount) > srcIndices.size())
    {
      throw std::invalid_argument("One of the arguments is invalid");
    }

    std::size_t written = 0;
    if ((dstIndex & 1) == 0)
    {
      // Generate a counter clockwise quad strip mesh
      //              | a   | b   | *c  | *d  | *e  | *f  | g   | h   |
      //              |-----|-----|-----|-----|-----|-----|-----|-----|
      // 0-1    4-5   | 0-1 |   1 | 1   |   4 |   4 | 4   | 4-5 |   5 |
      // |/| -> |/|   | |/  |  /| | |   |  /  |  /  | |   | |/  |  /| |
      // 2-3    6-7   | 2   | 2-3 | 3   | 3   | 3   | 6   | 6   | 6-7 |
      //              |     |     |     |     |     |     |     |     |
      //
      // * = a degenerated triangle
      //
      // Triangle indices (CCW)
      // | Instance #1 | a: 0,2,1 ccw | b: 2,1,3 cw |              |             |
      // | Degenerated | c: 1,3,3 ccw | d: 3,3,4 cw | e: 3,4,4 ccw | f: 4,4,6 cw |  (always four)
      // | Instance #2 | g: 4,6,5 ccw | h: 6,5,7 cw |              |             |

      // Generate a clockwise quad strip mesh
      //              | a   | b   | *c  | *d  | *e  | *f  | g   | h   |
      //              |-----|-----|-----|-----|-----|-----|-----|-----|
      // 0-1    4-5   | 0   | 0-1 | 1   | 1   | 1   | 4   | 4   | 4-5 |
      // |/| -> |/|   | |\  |  \| | |   |  \  |  \  | |   | |\  |  \| |
      // 2-3    6-7   | 2-3 |   3 | 3   |   6 |   6 | 6   | 6-7 |   7 |
      //              |     |     |     |     |     |     |     |     |
      //
      // * = a degenerated triangle
      //
      // Triangle indices (CW)
      // | Instance #1 | a: 2,0,3 ccw | b: 0,3,1 cw |              |             |
      // | Degenerated | c: 3,1,1 ccw | d: 1,1,6 cw | e: 1,6,6 ccw | f: 6,6,4 cw |  (always four)
      // | Instance #2 | g: 6,4,7 ccw | h: 4,7,5 cw |              |             |

      const std::size_t minDstCapacity = dstIndex + srcCount + (dstIndex > 0 ? 4 : 0);
      if (minDstCapacity > rDstIndices.size())
      {
        throw std::invalid_argument("dst capacity too small");
      }

      if (dstIndex > 0)
      {
        rDstIndices[dstIndex] = rDstIndices[dstIndex - 1];
        rDstIndices[dstIndex + 1] = mod + srcIndices[srcStartIndex];
        rDstIndices[dstIndex + 2] = mod + srcIndices[srcStartIndex];
        rDstIndices[dstIndex + 3] = mod + srcIndices[srcStartIndex + 1];
        written += 4;
      }
    }
    else
    {
      // Generate a counter clockwise quad strip mesh
      //              | a   | *b  | *c  | *d  | *e  | *f  | h   | i   |
      //              |-----|-----|-----|-----|-----|-----|-----|-----|
      // 0-1    4-5   | 0-1 |   1 | 1-4 | 1-4 | 4   | 4   | 4-5 |   5 |
      // |/  -> |/|   | |/  |  /  |     |     |     | |   | |/  |  /| |
      // 2      6-7   | 2   | 2   |     |     |     | 6   | 6   | 6-7 |
      //              |     |     |     |     |     |     |     |     |
      //
      // * = a degenerated triangle
      //
      // Triangle indices (CCW)
      // | Instance #1 | a: 0,2,1 ccw |              |              |              |             |
      // | Degenerated | b: 2,1,1  cw | c: 1,1,4 ccw | d: 1,4,4 cw  | e: 4,4,4 ccw | f: 4,4,6 cw |  (always five)
      // | Instance #2 | g: 4,6,5 ccw | h: 6,5,7  cw |              |              |             |

      // Generate a clockwise quad strip mesh
      //              | a   | *b  | *c  | *d  | *e  | *f  | g   | h   |
      //              |-----|-----|-----|-----|-----|-----|-----|-----|
      // 0-1    4-5   | 0   | 0-1 | 1   | 1   | 1   | 4   | 4   | 4-5 |
      // |/  -> |/|   | |\  |  \| | |   |  \  |  \  | |   | |\  |  \| |
      // 2      6-7   | 2-3 |   3 | 3   |   6 |   6 | 6   | 6-7 |   7 |
      //              |     |     |     |     |     |     |     |     |
      //
      // * = a degenerated triangle
      //
      // Triangle indices (CW)
      // | Instance #1 | a: 2,0,3 ccw |              |             |              |             |
      // | Degenerated | b: 0,3,3  cw | c: 3,3,6 ccw | d: 3,6,6 cw | e: 6,6,6 ccw | f: 6,6,4 cw |  (always five)
      // | Instance #2 | g: 6,4,7 ccw | h: 4,7,5 cw  |             |              |             |

      const std::size_t minDstCapacity = dstIndex + srcCount + (dstIndex > 0 ? 5 : 0);
      if (minDstCapacity > rDstIndices.size())
      {
        throw std::invalid_argument("dst capacity too small");
      }

      rDstIndices[dstIndex] = rDstIndices[dstIndex - 1];
      rDstIndices[dstIndex + 1] = mod + srcIndices[srcStartIndex];
      rDstIndices[dstIndex + 2] = mod + srcIndices[srcStartIndex];
      rDstIndices[dstIndex + 3] = mod + srcIndices[srcStartIndex];
      rDstIndices[dstIndex + 4] = mod + srcIndices[srcStartIndex + 1];
      written += 5;
    }
    written += MergeListIndices(rDstIndices, dstIndex + written, srcIndices, srcStartIndex, srcCount, mod);
    return written;
  }
}

#endif
