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

#include <FslBase/Log/Log3Core.hpp>
#include <FslGraphics/PrimitiveTypeUtil.hpp>
#include <cassert>

namespace Fsl
{
  int32_t PrimitiveTypeUtil::CalcPrimitiveCount(const int32_t indexCount, const PrimitiveType primitiveType)
  {
    switch (primitiveType)
    {
    case PrimitiveType::LineList:
      return indexCount > 0 ? indexCount / 2 : 0;
    case PrimitiveType::LineStrip:
      return indexCount > 0 ? (indexCount - 1) : 0;
    case PrimitiveType::TriangleList:
      return indexCount > 0 ? indexCount / 3 : 0;
    case PrimitiveType::TriangleStrip:
      return indexCount > 2 ? (indexCount - 2) : 0;
    default:
      FSLLOG3_DEBUG_WARNING("Unsupported primitive type");
      return 0;
    }
  }

  uint32_t PrimitiveTypeUtil::CalcPrimitiveCount(const uint32_t indexCount, const PrimitiveType primitiveType)
  {
    switch (primitiveType)
    {
    case PrimitiveType::LineList:
      return indexCount / 2u;
    case PrimitiveType::LineStrip:
      return indexCount > 1u ? (indexCount - 1u) : 0;
    case PrimitiveType::TriangleList:
      return indexCount / 3u;
    case PrimitiveType::TriangleStrip:
      return indexCount > 2u ? (indexCount - 2u) : 0;
    default:
      FSLLOG3_DEBUG_WARNING("Unsupported primitive type");
      return 0u;
    }
  }
}
