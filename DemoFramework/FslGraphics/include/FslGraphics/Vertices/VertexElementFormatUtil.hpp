#ifndef FSLGRAPHICS_VERTICES_VERTEXELEMENTFORMATUTIL_HPP
#define FSLGRAPHICS_VERTICES_VERTEXELEMENTFORMATUTIL_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/Vertices/VertexElementFormat.hpp>

namespace Fsl
{
  class VertexElementFormatUtil
  {
  public:
    constexpr static int32_t TryGetBytesPerElement(const VertexElementFormat format)
    {
      switch (format)
      {
      case VertexElementFormat::Single:
        return sizeof(float) * 1;
      case VertexElementFormat::Vector2:
        return sizeof(float) * 2;
      case VertexElementFormat::Vector3:
        return sizeof(float) * 3;
      case VertexElementFormat::Vector4:
        return sizeof(float) * 4;
      case VertexElementFormat::Matrix4x4:
        return sizeof(float) * 4 * 4;
      case VertexElementFormat::X8_UNORM:
      case VertexElementFormat::X8_UINT:
        return sizeof(uint8_t) * 1;
      case VertexElementFormat::X8Y8_UNORM:
      case VertexElementFormat::X8Y8_UINT:
        return sizeof(uint8_t) * 2;
      case VertexElementFormat::X8Y8Z8_UNORM:
      case VertexElementFormat::X8Y8Z8_UINT:
        return sizeof(uint8_t) * 3;
      case VertexElementFormat::X8Y8Z8W8_UNORM:
      case VertexElementFormat::X8Y8Z8W8_UINT:
        return sizeof(uint8_t) * 4;
      case VertexElementFormat::X16Y16Z16W16_UNORM:
      case VertexElementFormat::X16Y16Z16W16_UINT:
        return sizeof(uint16_t) * 4;
      default:
        return -1;
      }
    }

    constexpr static int32_t TryGetElementCount(const VertexElementFormat format)
    {
      switch (format)
      {
      case VertexElementFormat::Single:
      case VertexElementFormat::X8_UNORM:
      case VertexElementFormat::X8_UINT:
        return 1;
      case VertexElementFormat::Vector2:
      case VertexElementFormat::X8Y8_UNORM:
      case VertexElementFormat::X8Y8_UINT:
        return 2;
      case VertexElementFormat::Vector3:
      case VertexElementFormat::X8Y8Z8_UNORM:
      case VertexElementFormat::X8Y8Z8_UINT:
        return 3;
      case VertexElementFormat::Vector4:
      case VertexElementFormat::X8Y8Z8W8_UNORM:
      case VertexElementFormat::X8Y8Z8W8_UINT:
      case VertexElementFormat::X16Y16Z16W16_UNORM:
      case VertexElementFormat::X16Y16Z16W16_UINT:
        return 4;
      case VertexElementFormat::Matrix4x4:
        return 4 * 4;
      default:
        return -1;
      }
    }

    constexpr static uint32_t GetBytesPerElement(const VertexElementFormat format)
    {
      return ProcessResult(TryGetBytesPerElement(format));
    }

    constexpr static uint32_t GetElementCount(const VertexElementFormat format)
    {
      return ProcessResult(TryGetElementCount(format));
    }

  private:
    constexpr inline static uint32_t ProcessResult(const int32_t res)
    {
      return res >= 0 ? static_cast<uint32_t>(res) : throw NotSupportedException("Unknown VertexElementFormat");
    }
  };
}

#endif
