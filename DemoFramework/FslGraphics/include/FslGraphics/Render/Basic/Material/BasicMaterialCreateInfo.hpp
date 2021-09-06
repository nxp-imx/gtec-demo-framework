#ifndef FSLGRAPHICS_RENDER_BASIC_MATERIAL_BASICMATERIALCREATEINFO_HPP
#define FSLGRAPHICS_RENDER_BASIC_MATERIAL_BASICMATERIALCREATEINFO_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslGraphics/Render/Basic/Material/BasicMaterialInfo.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>

namespace Fsl
{
  struct BasicMaterialCreateInfo
  {
    const BasicMaterialInfo MaterialInfo;
    const VertexDeclarationSpan VertexDeclaration;


    constexpr BasicMaterialCreateInfo(const BlendState blend, const VertexDeclarationSpan& vertexDeclaration) noexcept
      : BasicMaterialCreateInfo(BasicMaterialInfo(blend, BasicCullMode::Back, BasicFrontFace::CounterClockwise, BasicMaterialDepthInfo()),
                                vertexDeclaration)
    {
    }

    constexpr BasicMaterialCreateInfo(const BlendState blend, const BasicCullMode cullMode, const BasicFrontFace cullFrontFace,
                                      const BasicMaterialDepthInfo& depthInfo, const VertexDeclarationSpan& vertexDeclaration) noexcept
      : BasicMaterialCreateInfo(BasicMaterialInfo(blend, cullMode, cullFrontFace, depthInfo), vertexDeclaration)
    {
    }

    constexpr BasicMaterialCreateInfo(const BasicMaterialInfo materialInfo, const VertexDeclarationSpan& vertexDeclaration) noexcept
      : MaterialInfo(materialInfo)
      , VertexDeclaration(vertexDeclaration)
    {
    }

    constexpr bool IsValid() const
    {
      return !VertexDeclaration.Empty();
    }
  };
}

#endif
