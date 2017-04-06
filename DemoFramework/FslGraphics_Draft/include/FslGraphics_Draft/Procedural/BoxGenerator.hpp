#ifndef FSLGRAPHICS_DRAFT_PROCEDURAL_BOXGENERATOR_HPP
#define FSLGRAPHICS_DRAFT_PROCEDURAL_BOXGENERATOR_HPP
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
#include <FslBase/Math/BoxF.hpp>
#include <FslGraphics/NativeTextureArea.hpp>
#include <FslGraphics/WindingOrder.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslGraphics_Draft/Procedural/BasicMesh.hpp>

namespace Fsl {
namespace Procedural
{
  class BoxGenerator
  {
  public:

    //! @brief Creates a box centered at dstCenter of the given width and height.
    //! @param dstCenter the center of the cube (origin).
    //! @param dstWidth the width of the cube.
    //! @param dstHeight the height of the cube.
    //! @param dstDepth the depth of the cube
    //! @param pTextureAreas a pointer to at least 6 texture areas (!= null).
    //! @param numTextureAreas the number of texture areas provided (must be >= 6 and only six will be used, front, right, back, left, top, bottom).
    //! @param windingOrder the winding order to utilize.
    //! @note  ASCII example below
    //!          x----x
    //!         /    /| < dstHeight
    //!        x----x |
    //!        |    | x
    //!        |    |/  < dstDepth
    //!        x----x
    //!          ^ dstWidth
    static BasicMesh GenerateList(const Vector3& dstCenter, const float dstWidth, const float dstHeight, const float dstDepth, const NativeTextureArea*const pTextureAreas, const int32_t numTextureAreas, const WindingOrder::Enum windingOrder);
    static BasicMesh GenerateStrip(const Vector3& dstCenter, const float dstWidth, const float dstHeight, const float dstDepth, const NativeTextureArea*const pTextureAreas, const int32_t numTextureAreas, const WindingOrder::Enum windingOrder);
  };

}}

#endif
