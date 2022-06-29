#ifndef FSLGRAPHICS_VERTICES_VERTEXELEMENT_HPP
#define FSLGRAPHICS_VERTICES_VERTEXELEMENT_HPP
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
#include <FslGraphics/Vertices/VertexElementFormat.hpp>
#include <FslGraphics/Vertices/VertexElementUsage.hpp>

namespace Fsl
{
  struct VertexElement
  {
    uint32_t Offset{0};
    VertexElementFormat Format{VertexElementFormat::Single};
    VertexElementUsage Usage{VertexElementUsage::Color};
    uint32_t UsageIndex{0};

    constexpr VertexElement() noexcept = default;

    constexpr VertexElement(const uint32_t offset, const VertexElementFormat& format, const VertexElementUsage usage,
                            const uint32_t usageIndex) noexcept
      : Offset(offset)
      , Format(format)
      , Usage(usage)
      , UsageIndex(usageIndex)
    {
    }

    constexpr bool operator==(const VertexElement& rhs) const noexcept
    {
      return Offset == rhs.Offset && Format == rhs.Format && Usage == rhs.Usage && UsageIndex == rhs.UsageIndex;
    }

    constexpr bool operator!=(const VertexElement& rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };
}

#endif
