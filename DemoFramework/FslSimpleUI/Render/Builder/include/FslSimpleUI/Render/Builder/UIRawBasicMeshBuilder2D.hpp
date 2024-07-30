#ifndef FSLSIMPLEUI_RENDER_BUILDER_UIRAWBASICMESHBUILDER2D_HPP
#define FSLSIMPLEUI_RENDER_BUILDER_UIRAWBASICMESHBUILDER2D_HPP
/****************************************************************************************************************************************************
 * Copyright 2021, 2024 NXP
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

#include <FslSimpleUI/Render/Builder/UIVertex.hpp>
#include <FslGraphics2D/Procedural/Builder/InlineRawBasicMeshBuilder2D.hpp>

namespace Fsl::UI
{
  class UIRawBasicMeshBuilder2D : public InlineRawBasicMeshBuilder2D<UIVertex>
  {
  public:
    constexpr UIRawBasicMeshBuilder2D() noexcept = default;
    constexpr UIRawBasicMeshBuilder2D(const UIRawBasicMeshBuilder2D& other) noexcept = default;

    //! @brief overload that allows you to create a InlineRawMeshBuilder2D from pointers and capacities that is noexcept.
    //!        only use this in cases where you are 100% sure that your input is valid
    inline explicit UIRawBasicMeshBuilder2D(vertex_pointer pVertices, size_type vertexCapacity, const vertex_position_value_type zPos,
                                            const vertex_color_type& color, const OptimizationCheckFlag flag) noexcept
      : InlineRawBasicMeshBuilder2D<UIVertex>(pVertices, vertexCapacity, zPos, color, flag)
    {
    }

    explicit constexpr UIRawBasicMeshBuilder2D(vertex_pointer pVertices, size_type vertexCapacity, const vertex_position_value_type zPos,
                                               const vertex_color_type& color) noexcept
      : InlineRawBasicMeshBuilder2D<UIVertex>(pVertices, vertexCapacity, zPos, color)
    {
    }
  };
}


#endif
