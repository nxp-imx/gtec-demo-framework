#ifndef FSLGRAPHICS2D_PROCEDURAL_BUILDER_INLINERAWBASICMESHBUILDER2D_HPP
#define FSLGRAPHICS2D_PROCEDURAL_BUILDER_INLINERAWBASICMESHBUILDER2D_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslBase/Math/Pixel/PxAreaRectangleF.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/NativeNineSliceTextureArea.hpp>
#include <FslGraphics/NativeTextureArea.hpp>
#include <FslGraphics2D/Procedural/Builder/Clip2DUtil.hpp>
#include <cassert>
#include <type_traits>

namespace Fsl
{
  //! Low level vertex generation. 'It always writes exactly the requested vertices.'
  //! Its intended as a tool for writing more complex mesh builders.
  //! It expects that clipping and invisible due to color based culling' has been done.
  //! It also expects that the user ensures that there is capacity before calling it and that the parameters are valid.
  //! All error checking is done with asserts just to verify the assumptions in debug builds.
  template <typename TVertex>
  class InlineRawBasicMeshBuilder2D
  {
  public:
    using vertex_element_type = TVertex;
    using vertex_value_type = std::remove_cv_t<TVertex>;
    using vertex_pointer = TVertex*;
    using const_vertex_pointer = const TVertex*;
    using vertex_reference = TVertex&;
    using const_vertex_reference = const TVertex&;

    using size_type = uint32_t;
    using difference_type = std::ptrdiff_t;

    using vertex_position_type = Vector3;
    using vertex_position_value_type = Vector3::value_type;
    using vertex_color_type = Color;
    using vertex_uv_type = Vector2;

  private:
    vertex_pointer m_pVertexData{nullptr};
    size_type m_vertexCapacity{0u};
    size_type m_vertexCount{0u};

    vertex_position_value_type m_zPos{0};
    vertex_color_type m_color{};

  public:
    constexpr InlineRawBasicMeshBuilder2D() noexcept = default;
    constexpr InlineRawBasicMeshBuilder2D(const InlineRawBasicMeshBuilder2D& other) noexcept = default;

    //! @brief overload that allows you to create a InlineRawBasicMeshBuilder2D from pointers and capacities that is noexcept.
    //!        only use this in cases where you are 100% sure that your input is valid
    explicit InlineRawBasicMeshBuilder2D(vertex_pointer pVertices, size_type vertexCapacity, const vertex_position_value_type zPos,
                                         const vertex_color_type& color, const OptimizationCheckFlag /*unused*/) noexcept
      : m_pVertexData(pVertices)
      , m_vertexCapacity(vertexCapacity)
      , m_zPos(zPos)
      , m_color(color)
    {
      assert(m_pVertexData != nullptr || vertexCapacity == 0u);
    }

    explicit constexpr InlineRawBasicMeshBuilder2D(vertex_pointer pVertices, size_type vertexCapacity, const vertex_position_value_type zPos,
                                                   const vertex_color_type& color) noexcept
      : m_pVertexData(pVertices)
      , m_vertexCapacity(pVertices != nullptr ? vertexCapacity : 0u)
      , m_zPos(zPos)
      , m_color(color)
    {
      FSLLOG3_DEBUG_INFO_IF(pVertices == nullptr && vertexCapacity != 0, "forcing vertex capacity to zero");
    }

    Color GetColor() const noexcept
    {
      return m_color;
    }

    void SetColor(const Color color) noexcept
    {
      m_color = color;
    }

    //! Direct span access
    ReadOnlySpan<TVertex> VerticesAsReadOnlySpan() const
    {
      return ReadOnlySpan<TVertex>(m_pVertexData, m_vertexCount, OptimizationCheckFlag::NoCheck);
    }

    constexpr float GetZPos() const noexcept
    {
      return m_zPos;
    }

    constexpr size_type GetVertexCount() const noexcept
    {
      return m_vertexCount;
    }

    constexpr size_type GetRemainingVertexCapacity() const noexcept
    {
      return m_vertexCapacity - m_vertexCount;
    }

    //! Clear the current mesh builder so it can be used again.
    constexpr void Clear() noexcept
    {
      m_vertexCount = 0;
    }

    constexpr void AddVertex(const float x0, const float y0, const float u0, const float v0) noexcept
    {
      assert((m_vertexCount + 1) <= m_vertexCapacity);
      *(m_pVertexData + m_vertexCount) = TVertex(vertex_position_type(x0, y0, m_zPos), m_color, vertex_uv_type(u0, v0));
      ++m_vertexCount;
    }

    inline constexpr void AddRect(const PxAreaRectangleF& areaRectPxf, const NativeTextureArea& textureCoords) noexcept
    {
      AddRect(areaRectPxf.Left(), areaRectPxf.Top(), areaRectPxf.Right(), areaRectPxf.Bottom(), textureCoords);
    }


    //! @brief  Add a simple quad mesh
    //! @note   Mesh example:
    //!         X X
    //!         0 1
    //!
    //!         A-C Y0
    //!         |\|
    //!         B-D Y1
    constexpr void AddRect(const float x0, const float y0, const float x1, const float y1, const NativeTextureArea& textureCoords) noexcept
    {
      assert((m_vertexCount + 6) <= m_vertexCapacity);

      // 0123
      // ABCD
      {
        vertex_pointer pDst = m_pVertexData + m_vertexCount;
        pDst[0] = TVertex(vertex_position_type(x0, y1, m_zPos), m_color, vertex_uv_type(textureCoords.X0, textureCoords.Y1));    // B
        pDst[1] = TVertex(vertex_position_type(x0, y0, m_zPos), m_color, vertex_uv_type(textureCoords.X0, textureCoords.Y0));    // A
        pDst[2] = TVertex(vertex_position_type(x1, y1, m_zPos), m_color, vertex_uv_type(textureCoords.X1, textureCoords.Y1));    // D

        pDst[3] = TVertex(vertex_position_type(x1, y1, m_zPos), m_color, vertex_uv_type(textureCoords.X1, textureCoords.Y1));    // D
        pDst[4] = TVertex(vertex_position_type(x0, y0, m_zPos), m_color, vertex_uv_type(textureCoords.X0, textureCoords.Y0));    // A
        pDst[5] = TVertex(vertex_position_type(x1, y0, m_zPos), m_color, vertex_uv_type(textureCoords.X1, textureCoords.Y0));    // C
      }
      m_vertexCount += 6;
    }
  };

}


#endif
