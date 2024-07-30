#ifndef FSLGRAPHICS2D_PROCEDURAL_BUILDER_INLINERAWMESHBUILDER2D_HPP
#define FSLGRAPHICS2D_PROCEDURAL_BUILDER_INLINERAWMESHBUILDER2D_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2024 NXP
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
#include <FslBase/Span/SpanUtil_Create.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/NativeNineSliceTextureArea.hpp>
#include <FslGraphics/NativeTextureArea.hpp>
#include <FslGraphics/TextureAtlas/AtlasNineSliceFlags.hpp>
#include <FslGraphics2D/Procedural/Builder/Clip2DUtil.hpp>
#include <cassert>
#include <type_traits>

namespace Fsl
{
  //! Low level vertex + index generation. 'It always writes exactly the requested vertices.'
  //! Its intended as a tool for writing more complex mesh builders.
  //! It expects that clipping and invisible due to color based culling' has been done.
  //! It also expects that the user ensures that there is capacity before calling it and that the parameters are valid.
  //! All error checking is done with asserts just to verify the assumptions in debug builds.
  template <typename TVertex, typename TIndex>
  class InlineRawMeshBuilder2D
  {
  public:
    using vertex_element_type = TVertex;
    using vertex_value_type = std::remove_cv_t<TVertex>;
    using vertex_pointer = TVertex*;
    using const_vertex_pointer = const TVertex*;
    using vertex_reference = TVertex&;
    using const_vertex_reference = const TVertex&;

    using index_element_type = TIndex;
    using index_value_type = std::remove_cv_t<TIndex>;
    using index_pointer = index_value_type*;
    using const_index_pointer = const index_value_type*;
    using index_reference = index_value_type&;
    using const_index_reference = const index_value_type&;

    using size_type = uint32_t;
    using difference_type = std::ptrdiff_t;

    using vertex_position_type = Vector3;
    using vertex_position_value_type = Vector3::value_type;
    using vertex_color_type = typename vertex_element_type::color_type;
    using vertex_uv_type = Vector2;

  private:
    vertex_pointer m_pVertexData{nullptr};
    size_type m_vertexCapacity{0u};
    size_type m_vertexCount{0u};

    index_pointer m_pIndexData{nullptr};
    size_type m_indexCapacity{0u};
    size_type m_indexCount{0u};
    index_value_type m_indexVertexOffset{0};

    vertex_position_value_type m_zPos{0};
    vertex_color_type m_color{};

  public:
    inline constexpr InlineRawMeshBuilder2D() noexcept = default;
    inline constexpr InlineRawMeshBuilder2D(const InlineRawMeshBuilder2D& other) noexcept = default;

    //! @brief overload that allows you to create a InlineRawMeshBuilder2D from pointers and capacities that is noexcept.
    //!        only use this in cases where you are 100% sure that your input is valid
    inline explicit InlineRawMeshBuilder2D(vertex_pointer pVertices, size_type vertexCapacity, index_pointer pIndices, size_type indexCapacity,
                                           const index_value_type indexVertexOffset, const vertex_position_value_type zPos,
                                           const vertex_color_type& color, const OptimizationCheckFlag /*unused*/) noexcept
      : m_pVertexData(pVertices)
      , m_vertexCapacity(vertexCapacity)
      , m_pIndexData(pIndices)
      , m_indexCapacity(indexCapacity)
      , m_indexVertexOffset(indexVertexOffset >= 0 ? indexVertexOffset : 0)
      , m_zPos(zPos)
      , m_color(color)
    {
      assert(m_pVertexData != nullptr || vertexCapacity == 0u);
      assert(m_pIndexData != nullptr || indexCapacity == 0u);
      assert(indexVertexOffset >= 0);
    }

    inline explicit constexpr InlineRawMeshBuilder2D(vertex_pointer pVertices, size_type vertexCapacity, index_pointer pIndices,
                                                     size_type indexCapacity, const index_value_type indexVertexOffset,
                                                     const vertex_position_value_type zPos, const vertex_color_type& color) noexcept
      : m_pVertexData(pVertices)
      , m_vertexCapacity(pVertices != nullptr ? vertexCapacity : 0u)
      , m_pIndexData(pIndices)
      , m_indexCapacity(pIndices != nullptr ? indexCapacity : 0u)
      , m_indexVertexOffset(indexVertexOffset >= 0 ? indexVertexOffset : 0)
      , m_zPos(zPos)
      , m_color(color)
    {
      FSLLOG3_DEBUG_INFO_IF(pVertices == nullptr && vertexCapacity != 0, "forcing vertex capacity to zero");
      FSLLOG3_DEBUG_INFO_IF(pIndices == nullptr && indexCapacity != 0, "forcing index capacity to zero");
    }

    inline vertex_color_type GetColor() const noexcept
    {
      return m_color;
    }

    inline void SetColor(const vertex_color_type color) noexcept
    {
      m_color = color;
    }

    //! Direct span access
    inline ReadOnlySpan<TVertex> VerticesAsReadOnlySpan() const
    {
      return SpanUtil::UncheckedCreateReadOnly(m_pVertexData, m_vertexCount);
    }

    //! Direct span access
    inline ReadOnlySpan<index_value_type> IndicesAsReadOnlySpan() const
    {
      return SpanUtil::UncheckedCreateReadOnly(m_pIndexData, m_indexCount);
    }


    inline constexpr float GetZPos() const noexcept
    {
      return m_zPos;
    }

    inline constexpr index_value_type GetIndexVertexOffset() const noexcept
    {
      return m_indexVertexOffset;
    }

    inline constexpr size_type GetVertexCount() const noexcept
    {
      return m_vertexCount;
    }

    inline constexpr size_type GetIndexCount() const noexcept
    {
      return m_indexCount;
    }

    inline constexpr size_type GetRemainingVertexCapacity() const noexcept
    {
      return m_vertexCapacity - m_vertexCount;
    }

    inline constexpr size_type GetRemainingIndexCapacity() const noexcept
    {
      return m_indexCapacity - m_indexCount;
    }

    //! Clear the current mesh builder so it can be used again.
    constexpr void Clear() noexcept
    {
      m_vertexCount = 0;
      m_indexCount = 0;
    }

    constexpr void AddVertex(const float x0, const float y0, const float u0, const float v0) noexcept
    {
      assert((m_vertexCount + 1) <= m_vertexCapacity);
      *(m_pVertexData + m_vertexCount) = TVertex(vertex_position_type(x0, y0, m_zPos), m_color, vertex_uv_type(u0, v0));
      ++m_vertexCount;
    }

    constexpr void AddIndex(const index_value_type vertexIndex) noexcept
    {
      assert((m_indexCount + 1) <= m_indexCapacity);
      *(m_pIndexData + m_indexCount) = m_indexVertexOffset + vertexIndex;
      ++m_indexCount;
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
    inline constexpr void AddRect(const float x0, const float y0, const float x1, const float y1, const NativeTextureArea& textureCoords) noexcept
    {
      assert((m_vertexCount + 4) <= m_vertexCapacity);
      assert((m_indexCount + 6) <= m_indexCapacity);

      // 0123
      // ABCD
      {
        vertex_pointer pDst = m_pVertexData + m_vertexCount;
        // pDst[0] = TVertex(x0, y0, m_zPos, m_color, textureCoords.X0, textureCoords.Y0);
        pDst[0].Position = vertex_position_type(x0, y0, m_zPos);
        pDst[0].Color = m_color;
        pDst[0].TextureCoordinate = vertex_uv_type(textureCoords.X0, textureCoords.Y0);
        // pDst[1] = TVertex(x0, y1, m_zPos, m_color, textureCoords.X0, textureCoords.Y1);
        pDst[1].Position = vertex_position_type(x0, y1, m_zPos);
        pDst[1].Color = m_color;
        pDst[1].TextureCoordinate = vertex_uv_type(textureCoords.X0, textureCoords.Y1);

        // pDst[2] = TVertex(x1, y0, m_zPos, m_color, textureCoords.X1, textureCoords.Y0);
        pDst[2].Position = vertex_position_type(x1, y0, m_zPos);
        pDst[2].Color = m_color;
        pDst[2].TextureCoordinate = vertex_uv_type(textureCoords.X1, textureCoords.Y0);

        // pDst[3] = TVertex(x1, y1, m_zPos, m_color, textureCoords.X1, textureCoords.Y1);
        pDst[3].Position = vertex_position_type(x1, y1, m_zPos);
        pDst[3].Color = m_color;
        pDst[3].TextureCoordinate = vertex_uv_type(textureCoords.X1, textureCoords.Y1);
      }
      {
        index_pointer pDst = m_pIndexData + m_indexCount;
        const index_value_type indexOffset = m_indexVertexOffset + m_vertexCount;
        pDst[0] = indexOffset + 1;    // B
        pDst[1] = indexOffset + 0;    // A
        pDst[2] = indexOffset + 3;    // D

        pDst[3] = indexOffset + 3;    // D
        pDst[4] = indexOffset + 0;    // A
        pDst[5] = indexOffset + 2;    // C
      }

      m_vertexCount += 4;
      m_indexCount += 3 * 2;
    }

    //! @brief  Add a simple quad mesh
    //! @note   Mesh example:
    //!         X X
    //!         0 1
    //!
    //!         A-C Y0
    //!         |\|
    //!         B-D Y1
    inline constexpr void AddRect(const PxValueF x0, const PxValueF y0, const PxValueF x1, const PxValueF y1,
                                  const NativeTextureArea& textureCoords) noexcept
    {
      assert((m_vertexCount + 4) <= m_vertexCapacity);
      assert((m_indexCount + 6) <= m_indexCapacity);

      // 0123
      // ABCD
      {
        vertex_pointer pDst = m_pVertexData + m_vertexCount;
        // pDst[0] = TVertex(x0, y0, m_zPos, m_color, textureCoords.X0, textureCoords.Y0);
        pDst[0].Position = vertex_position_type(x0.Value, y0.Value, m_zPos);
        pDst[0].Color = m_color;
        pDst[0].TextureCoordinate = vertex_uv_type(textureCoords.X0, textureCoords.Y0);
        // pDst[1] = TVertex(x0, y1, m_zPos, m_color, textureCoords.X0, textureCoords.Y1);
        pDst[1].Position = vertex_position_type(x0.Value, y1.Value, m_zPos);
        pDst[1].Color = m_color;
        pDst[1].TextureCoordinate = vertex_uv_type(textureCoords.X0, textureCoords.Y1);

        // pDst[2] = TVertex(x1, y0, m_zPos, m_color, textureCoords.X1, textureCoords.Y0);
        pDst[2].Position = vertex_position_type(x1.Value, y0.Value, m_zPos);
        pDst[2].Color = m_color;
        pDst[2].TextureCoordinate = vertex_uv_type(textureCoords.X1, textureCoords.Y0);

        // pDst[3] = TVertex(x1, y1, m_zPos, m_color, textureCoords.X1, textureCoords.Y1);
        pDst[3].Position = vertex_position_type(x1.Value, y1.Value, m_zPos);
        pDst[3].Color = m_color;
        pDst[3].TextureCoordinate = vertex_uv_type(textureCoords.X1, textureCoords.Y1);
      }
      {
        index_pointer pDst = m_pIndexData + m_indexCount;
        const index_value_type indexOffset = m_indexVertexOffset + m_vertexCount;
        pDst[0] = indexOffset + 1;    // B
        pDst[1] = indexOffset + 0;    // A
        pDst[2] = indexOffset + 3;    // D

        pDst[3] = indexOffset + 3;    // D
        pDst[4] = indexOffset + 0;    // A
        pDst[5] = indexOffset + 2;    // C
      }

      m_vertexCount += 4;
      m_indexCount += 3 * 2;
    }


    //! @brief  Add a nine slice
    //! @note   Mesh example:
    //!         X X X X
    //!         0 1 2 3
    //!
    //!         A-C-E-G Y0
    //!         | | | |
    //!         B-D-F-H Y1
    //!         | | | |
    //!         I-K-M-O Y2
    //!         | | | |
    //!         J-L-N-P Y3
    inline constexpr void AddNineSlice(const float x0, const float y0, const float x1, const float y1, const float x2, const float y2, const float x3,
                                       const float y3, const NativeNineSliceTextureArea& texCoordNineSlice) noexcept
    {
      assert((m_vertexCount + 16) <= m_vertexCapacity);
      assert((m_indexCount + 54) <= m_indexCapacity);

      {
        vertex_pointer pDst = m_pVertexData + m_vertexCount;

        // 0123
        // ABCD
        // pDst[0] = TVertex(vertex_position_type(x0, y0, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y0));
        pDst[0].Position = vertex_position_type(x0, y0, m_zPos);
        pDst[0].Color = m_color;
        pDst[0].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y0);
        // pDst[1] = TVertex(vertex_position_type(x0, y1, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y1));
        pDst[1].Position = vertex_position_type(x0, y1, m_zPos);
        pDst[1].Color = m_color;
        pDst[1].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y1);
        // pDst[2] = TVertex(vertex_position_type(x1, y0, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y0));
        pDst[2].Position = vertex_position_type(x1, y0, m_zPos);
        pDst[2].Color = m_color;
        pDst[2].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y0);
        // pDst[3] = TVertex(vertex_position_type(x1, y1, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y1));
        pDst[3].Position = vertex_position_type(x1, y1, m_zPos);
        pDst[3].Color = m_color;
        pDst[3].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y1);

        // 4567
        // EFGH
        // pDst[4] = TVertex(vertex_position_type(x2, y0, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y0));
        pDst[4].Position = vertex_position_type(x2, y0, m_zPos);
        pDst[4].Color = m_color;
        pDst[4].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y0);
        // pDst[5] = TVertex(vertex_position_type(x2, y1, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y1));
        pDst[5].Position = vertex_position_type(x2, y1, m_zPos);
        pDst[5].Color = m_color;
        pDst[5].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y1);
        // pDst[6] = TVertex(vertex_position_type(x3, y0, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y0));
        pDst[6].Position = vertex_position_type(x3, y0, m_zPos);
        pDst[6].Color = m_color;
        pDst[6].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y0);
        // pDst[7] = TVertex(vertex_position_type(x3, y1, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y1));
        pDst[7].Position = vertex_position_type(x3, y1, m_zPos);
        pDst[7].Color = m_color;
        pDst[7].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y1);
        //   11
        // 8901
        // IJKL
        // pDst[8] = TVertex(vertex_position_type(x0, y2, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y2));
        pDst[8].Position = vertex_position_type(x0, y2, m_zPos);
        pDst[8].Color = m_color;
        pDst[8].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y2);
        // pDst[9] = TVertex(vertex_position_type(x0, y3, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y3));
        pDst[9].Position = vertex_position_type(x0, y3, m_zPos);
        pDst[9].Color = m_color;
        pDst[9].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y3);
        // pDst[10] = TVertex(vertex_position_type(x1, y2, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y2));
        pDst[10].Position = vertex_position_type(x1, y2, m_zPos);
        pDst[10].Color = m_color;
        pDst[10].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y2);
        // pDst[11] = TVertex(vertex_position_type(x1, y3, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y3));
        pDst[11].Position = vertex_position_type(x1, y3, m_zPos);
        pDst[11].Color = m_color;
        pDst[11].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y3);
        // 1111
        // 2345
        // MNOP
        // pDst[12] = TVertex(vertex_position_type(x2, y2, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y2));
        pDst[12].Position = vertex_position_type(x2, y2, m_zPos);
        pDst[12].Color = m_color;
        pDst[12].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y2);
        // pDst[13] = TVertex(vertex_position_type(x2, y3, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y3));
        pDst[13].Position = vertex_position_type(x2, y3, m_zPos);
        pDst[13].Color = m_color;
        pDst[13].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y3);
        // pDst[14] = TVertex(vertex_position_type(x3, y2, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y2));
        pDst[14].Position = vertex_position_type(x3, y2, m_zPos);
        pDst[14].Color = m_color;
        pDst[14].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y2);
        // pDst[15] = TVertex(vertex_position_type(x3, y3, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y3));
        pDst[15].Position = vertex_position_type(x3, y3, m_zPos);
        pDst[15].Color = m_color;
        pDst[15].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y3);
      }
      {
        index_pointer pDst = m_pIndexData + m_indexCount;
        const index_value_type indexOffset = m_indexVertexOffset + m_vertexCount;

        //                     1 1 1 1 1 1
        // 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
        // A B C D E F G H I J K L M N O P

        // ABCD
        pDst[0] = indexOffset + 1;    // B
        pDst[1] = indexOffset + 0;    // A
        pDst[2] = indexOffset + 3;    // D

        pDst[3] = indexOffset + 3;    // D
        pDst[4] = indexOffset + 0;    // A
        pDst[5] = indexOffset + 2;    // C

        // CDEF
        pDst[6] = indexOffset + 3;    // D
        pDst[7] = indexOffset + 2;    // C
        pDst[8] = indexOffset + 5;    // F

        pDst[9] = indexOffset + 5;     // F
        pDst[10] = indexOffset + 2;    // C
        pDst[11] = indexOffset + 4;    // E

        // EFGH
        pDst[12] = indexOffset + 5;    // F
        pDst[13] = indexOffset + 4;    // E
        pDst[14] = indexOffset + 7;    // H

        pDst[15] = indexOffset + 7;    // H
        pDst[16] = indexOffset + 4;    // E
        pDst[17] = indexOffset + 6;    // G

        // BDIK
        pDst[18] = indexOffset + 8;     // I
        pDst[19] = indexOffset + 1;     // B
        pDst[20] = indexOffset + 10;    // K

        pDst[21] = indexOffset + 10;    // K
        pDst[22] = indexOffset + 1;     // B
        pDst[23] = indexOffset + 3;     // D

        // DFKM
        pDst[24] = indexOffset + 10;    // K
        pDst[25] = indexOffset + 3;     // D
        pDst[26] = indexOffset + 12;    // M

        pDst[27] = indexOffset + 12;    // M
        pDst[28] = indexOffset + 3;     // D
        pDst[29] = indexOffset + 5;     // F

        // FHMO
        pDst[30] = indexOffset + 12;    // M
        pDst[31] = indexOffset + 5;     // F
        pDst[32] = indexOffset + 14;    // O

        pDst[33] = indexOffset + 14;    // O
        pDst[34] = indexOffset + 5;     // F
        pDst[35] = indexOffset + 7;     // H

        // IKJL
        pDst[36] = indexOffset + 9;     // J
        pDst[37] = indexOffset + 8;     // I
        pDst[38] = indexOffset + 11;    // L

        pDst[39] = indexOffset + 11;    // L
        pDst[40] = indexOffset + 8;     // I
        pDst[41] = indexOffset + 10;    // K

        // KMLN
        pDst[42] = indexOffset + 11;    // L
        pDst[43] = indexOffset + 10;    // K
        pDst[44] = indexOffset + 13;    // N

        pDst[45] = indexOffset + 13;    // N
        pDst[46] = indexOffset + 10;    // K
        pDst[47] = indexOffset + 12;    // M

        // MONP
        pDst[48] = indexOffset + 13;    // N
        pDst[49] = indexOffset + 12;    // M
        pDst[50] = indexOffset + 15;    // P

        pDst[51] = indexOffset + 15;    // P
        pDst[52] = indexOffset + 12;    // M
        pDst[53] = indexOffset + 14;    // O
      }

      m_vertexCount += 8 * 2;
      m_indexCount += (3 * 6) * 3;
    }


    //! @brief  Add a nine slice
    //! @note   Mesh example:
    //!         X X X X
    //!         0 1 2 3
    //!
    //!         A-C-E-G Y0
    //!         | | | |
    //!         B-D-F-H Y1
    //!         | | | |
    //!         I-K-M-O Y2
    //!         | | | |
    //!         J-L-N-P Y3
    inline constexpr void AddNineSlice(const Vector2 pos0, const Vector2 pos1, const Vector2 pos2, const Vector2 pos3,
                                       const NativeNineSliceTextureArea& texCoordNineSlice) noexcept
    {
      assert((m_vertexCount + 16) <= m_vertexCapacity);
      assert((m_indexCount + 54) <= m_indexCapacity);

      {
        vertex_pointer pDst = m_pVertexData + m_vertexCount;

        // 0123
        // ABCD
        // pDst[0] = TVertex(vertex_position_type(pos0.X, pos0.Y, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X0,
        // texCoordNineSlice.Y0));
        pDst[0].Position = vertex_position_type(pos0.X, pos0.Y, m_zPos);
        pDst[0].Color = m_color;
        pDst[0].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y0);
        // pDst[1] = TVertex(vertex_position_type(pos0.X, pos1.Y, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X0,
        // texCoordNineSlice.Y1));
        pDst[1].Position = vertex_position_type(pos0.X, pos1.Y, m_zPos);
        pDst[1].Color = m_color;
        pDst[1].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y1);
        // pDst[2] = TVertex(vertex_position_type(x1, pos0.Y, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y0));
        pDst[2].Position = vertex_position_type(pos1.X, pos0.Y, m_zPos);
        pDst[2].Color = m_color;
        pDst[2].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y0);
        // pDst[3] = TVertex(vertex_position_type(pos1.X, pos1.Y, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X1,
        // texCoordNineSlice.Y1));
        pDst[3].Position = vertex_position_type(pos1.X, pos1.Y, m_zPos);
        pDst[3].Color = m_color;
        pDst[3].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y1);

        // 4567
        // EFGH
        // pDst[4] = TVertex(vertex_position_type(pos2.X, pos0.Y, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X2,
        // texCoordNineSlice.Y0));
        pDst[4].Position = vertex_position_type(pos2.X, pos0.Y, m_zPos);
        pDst[4].Color = m_color;
        pDst[4].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y0);
        // pDst[5] = TVertex(vertex_position_type(pos2.X, pos1.Y, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X2,
        // texCoordNineSlice.Y1));
        pDst[5].Position = vertex_position_type(pos2.X, pos1.Y, m_zPos);
        pDst[5].Color = m_color;
        pDst[5].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y1);
        // pDst[6] = TVertex(vertex_position_type(pos3.X, pos0.Y, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X3,
        // texCoordNineSlice.Y0));
        pDst[6].Position = vertex_position_type(pos3.X, pos0.Y, m_zPos);
        pDst[6].Color = m_color;
        pDst[6].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y0);
        // pDst[7] = TVertex(vertex_position_type(pos3.X, pos1.Y, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X3,
        // texCoordNineSlice.Y1));
        pDst[7].Position = vertex_position_type(pos3.X, pos1.Y, m_zPos);
        pDst[7].Color = m_color;
        pDst[7].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y1);
        //   11
        // 8901
        // IJKL
        // pDst[8] = TVertex(vertex_position_type(pos0.X, pos2.Y, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X0,
        // texCoordNineSlice.Y2));
        pDst[8].Position = vertex_position_type(pos0.X, pos2.Y, m_zPos);
        pDst[8].Color = m_color;
        pDst[8].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y2);
        // pDst[9] = TVertex(vertex_position_type(pos0.X, pos3.Y, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X0,
        // texCoordNineSlice.Y3));
        pDst[9].Position = vertex_position_type(pos0.X, pos3.Y, m_zPos);
        pDst[9].Color = m_color;
        pDst[9].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y3);
        // pDst[10] = TVertex(vertex_position_type(pos1.X, pos2.Y, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X1,
        // texCoordNineSlice.Y2));
        pDst[10].Position = vertex_position_type(pos1.X, pos2.Y, m_zPos);
        pDst[10].Color = m_color;
        pDst[10].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y2);
        // pDst[11] = TVertex(vertex_position_type(pos1.X, pos3.Y, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X1,
        // texCoordNineSlice.Y3));
        pDst[11].Position = vertex_position_type(pos1.X, pos3.Y, m_zPos);
        pDst[11].Color = m_color;
        pDst[11].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y3);
        // 1111
        // 2345
        // MNOP
        // pDst[12] = TVertex(vertex_position_type(pos2.X, pos2.Y, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X2,
        // texCoordNineSlice.Y2));
        pDst[12].Position = vertex_position_type(pos2.X, pos2.Y, m_zPos);
        pDst[12].Color = m_color;
        pDst[12].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y2);
        // pDst[13] = TVertex(vertex_position_type(pos2.X, pos3.Y, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X2,
        // texCoordNineSlice.Y3));
        pDst[13].Position = vertex_position_type(pos2.X, pos3.Y, m_zPos);
        pDst[13].Color = m_color;
        pDst[13].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y3);
        // pDst[14] = TVertex(vertex_position_type(pos3.X, pos2.Y, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X3,
        // texCoordNineSlice.Y2));
        pDst[14].Position = vertex_position_type(pos3.X, pos2.Y, m_zPos);
        pDst[14].Color = m_color;
        pDst[14].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y2);
        // pDst[15] = TVertex(vertex_position_type(pos3.X, pos3.Y, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X3,
        // texCoordNineSlice.Y3));
        pDst[15].Position = vertex_position_type(pos3.X, pos3.Y, m_zPos);
        pDst[15].Color = m_color;
        pDst[15].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y3);
      }
      {
        index_pointer pDst = m_pIndexData + m_indexCount;
        const index_value_type indexOffset = m_indexVertexOffset + m_vertexCount;

        //                     1 1 1 1 1 1
        // 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
        // A B C D E F G H I J K L M N O P

        // ABCD
        pDst[0] = indexOffset + 1;    // B
        pDst[1] = indexOffset + 0;    // A
        pDst[2] = indexOffset + 3;    // D

        pDst[3] = indexOffset + 3;    // D
        pDst[4] = indexOffset + 0;    // A
        pDst[5] = indexOffset + 2;    // C

        // CDEF
        pDst[6] = indexOffset + 3;    // D
        pDst[7] = indexOffset + 2;    // C
        pDst[8] = indexOffset + 5;    // F

        pDst[9] = indexOffset + 5;     // F
        pDst[10] = indexOffset + 2;    // C
        pDst[11] = indexOffset + 4;    // E

        // EFGH
        pDst[12] = indexOffset + 5;    // F
        pDst[13] = indexOffset + 4;    // E
        pDst[14] = indexOffset + 7;    // H

        pDst[15] = indexOffset + 7;    // H
        pDst[16] = indexOffset + 4;    // E
        pDst[17] = indexOffset + 6;    // G

        // BDIK
        pDst[18] = indexOffset + 8;     // I
        pDst[19] = indexOffset + 1;     // B
        pDst[20] = indexOffset + 10;    // K

        pDst[21] = indexOffset + 10;    // K
        pDst[22] = indexOffset + 1;     // B
        pDst[23] = indexOffset + 3;     // D

        // DFKM
        pDst[24] = indexOffset + 10;    // K
        pDst[25] = indexOffset + 3;     // D
        pDst[26] = indexOffset + 12;    // M

        pDst[27] = indexOffset + 12;    // M
        pDst[28] = indexOffset + 3;     // D
        pDst[29] = indexOffset + 5;     // F

        // FHMO
        pDst[30] = indexOffset + 12;    // M
        pDst[31] = indexOffset + 5;     // F
        pDst[32] = indexOffset + 14;    // O

        pDst[33] = indexOffset + 14;    // O
        pDst[34] = indexOffset + 5;     // F
        pDst[35] = indexOffset + 7;     // H

        // IKJL
        pDst[36] = indexOffset + 9;     // J
        pDst[37] = indexOffset + 8;     // I
        pDst[38] = indexOffset + 11;    // L

        pDst[39] = indexOffset + 11;    // L
        pDst[40] = indexOffset + 8;     // I
        pDst[41] = indexOffset + 10;    // K

        // KMLN
        pDst[42] = indexOffset + 11;    // L
        pDst[43] = indexOffset + 10;    // K
        pDst[44] = indexOffset + 13;    // N

        pDst[45] = indexOffset + 13;    // N
        pDst[46] = indexOffset + 10;    // K
        pDst[47] = indexOffset + 12;    // M

        // MONP
        pDst[48] = indexOffset + 13;    // N
        pDst[49] = indexOffset + 12;    // M
        pDst[50] = indexOffset + 15;    // P

        pDst[51] = indexOffset + 15;    // P
        pDst[52] = indexOffset + 12;    // M
        pDst[53] = indexOffset + 14;    // O
      }

      m_vertexCount += 8 * 2;
      m_indexCount += (3 * 6) * 3;
    }

    //! @brief  Add a nine slice
    //! @note   Mesh example:
    //!         X X X X
    //!         0 1 2 3
    //!
    //!         A-C-E-G Y0
    //!         | | | |
    //!         B-D-F-H Y1
    //!         | | | |
    //!         I-K-M-O Y2
    //!         | | | |
    //!         J-L-N-P Y3
    constexpr void AddNineSlice(const float x0, const float y0, const float x1, const float y1, const float x2, const float y2, const float x3,
                                const float y3, const NativeNineSliceTextureArea& texCoordNineSlice, const AtlasNineSliceFlags flags,
                                const bool drawOpaque) noexcept
    {
      assert((m_vertexCount + 16) <= m_vertexCapacity);
      assert((m_indexCount + 54) <= m_indexCapacity);

      {
        vertex_pointer pDst = m_pVertexData + m_vertexCount;

        // 0123
        // ABCD
        // pDst[0] = TVertex(vertex_position_type(x0, y0, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y0));
        pDst[0].Position = vertex_position_type(x0, y0, m_zPos);
        pDst[0].Color = m_color;
        pDst[0].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y0);
        // pDst[1] = TVertex(vertex_position_type(x0, y1, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y1));
        pDst[1].Position = vertex_position_type(x0, y1, m_zPos);
        pDst[1].Color = m_color;
        pDst[1].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y1);
        // pDst[2] = TVertex(vertex_position_type(x1, y0, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y0));
        pDst[2].Position = vertex_position_type(x1, y0, m_zPos);
        pDst[2].Color = m_color;
        pDst[2].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y0);
        // pDst[3] = TVertex(vertex_position_type(x1, y1, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y1));
        pDst[3].Position = vertex_position_type(x1, y1, m_zPos);
        pDst[3].Color = m_color;
        pDst[3].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y1);
        // 4567
        // EFGH
        // pDst[4] = TVertex(vertex_position_type(x2, y0, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y0));
        pDst[4].Position = vertex_position_type(x2, y0, m_zPos);
        pDst[4].Color = m_color;
        pDst[4].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y0);
        // pDst[5] = TVertex(vertex_position_type(x2, y1, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y1));
        pDst[5].Position = vertex_position_type(x2, y1, m_zPos);
        pDst[5].Color = m_color;
        pDst[5].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y1);
        // pDst[6] = TVertex(vertex_position_type(x3, y0, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y0));
        pDst[6].Position = vertex_position_type(x3, y0, m_zPos);
        pDst[6].Color = m_color;
        pDst[6].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y0);
        // pDst[7] = TVertex(vertex_position_type(x3, y1, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y1));
        pDst[7].Position = vertex_position_type(x3, y1, m_zPos);
        pDst[7].Color = m_color;
        pDst[7].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y1);
        //   11
        // 8901
        // IJKL
        // pDst[8] = TVertex(vertex_position_type(x0, y2, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y2));
        pDst[8].Position = vertex_position_type(x0, y2, m_zPos);
        pDst[8].Color = m_color;
        pDst[8].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y2);
        // pDst[9] = TVertex(vertex_position_type(x0, y3, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y3));
        pDst[9].Position = vertex_position_type(x0, y3, m_zPos);
        pDst[9].Color = m_color;
        pDst[9].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y3);
        // pDst[10] = TVertex(vertex_position_type(x1, y2, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y2));
        pDst[10].Position = vertex_position_type(x1, y2, m_zPos);
        pDst[10].Color = m_color;
        pDst[10].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y2);
        // pDst[11] = TVertex(vertex_position_type(x1, y3, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y3));
        pDst[11].Position = vertex_position_type(x1, y3, m_zPos);
        pDst[11].Color = m_color;
        pDst[11].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y3);
        // 1111
        // 2345
        // MNOP
        // pDst[12] = TVertex(vertex_position_type(x2, y2, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y2));
        pDst[12].Position = vertex_position_type(x2, y2, m_zPos);
        pDst[12].Color = m_color;
        pDst[12].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y2);
        // pDst[13] = TVertex(vertex_position_type(x2, y3, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y3));
        pDst[13].Position = vertex_position_type(x2, y3, m_zPos);
        pDst[13].Color = m_color;
        pDst[13].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y3);
        // pDst[14] = TVertex(vertex_position_type(x3, y2, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y2));
        pDst[14].Position = vertex_position_type(x3, y2, m_zPos);
        pDst[14].Color = m_color;
        pDst[14].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y2);
        // pDst[15] = TVertex(vertex_position_type(x3, y3, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y3));
        pDst[15].Position = vertex_position_type(x3, y3, m_zPos);
        pDst[15].Color = m_color;
        pDst[15].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y3);
      }
      uint32_t indexCount = 0;
      {
        index_pointer pDst = m_pIndexData + m_indexCount;
        const index_value_type indexOffset = m_indexVertexOffset + m_vertexCount;

        //                     1 1 1 1 1 1
        // 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
        // A B C D E F G H I J K L M N O P

        if (AtlasNineSliceFlagsUtil::IsEnabled(flags, AtlasNineSliceFlags::Slice0Transparent) != drawOpaque)
        {
          // ABCD
          pDst[0] = indexOffset + 1;    // B
          pDst[1] = indexOffset + 0;    // A
          pDst[2] = indexOffset + 3;    // D

          pDst[3] = indexOffset + 3;    // D
          pDst[4] = indexOffset + 0;    // A
          pDst[5] = indexOffset + 2;    // C
          pDst += 6;
        }


        if (AtlasNineSliceFlagsUtil::IsEnabled(flags, AtlasNineSliceFlags::Slice1Transparent) != drawOpaque)
        {
          // CDEF
          pDst[0] = indexOffset + 3;    // D
          pDst[1] = indexOffset + 2;    // C
          pDst[2] = indexOffset + 5;    // F

          pDst[3] = indexOffset + 5;    // F
          pDst[4] = indexOffset + 2;    // C
          pDst[5] = indexOffset + 4;    // E
          pDst += 6;
        }

        if (AtlasNineSliceFlagsUtil::IsEnabled(flags, AtlasNineSliceFlags::Slice2Transparent) != drawOpaque)
        {
          // EFGH
          pDst[0] = indexOffset + 5;    // F
          pDst[1] = indexOffset + 4;    // E
          pDst[2] = indexOffset + 7;    // H

          pDst[3] = indexOffset + 7;    // H
          pDst[4] = indexOffset + 4;    // E
          pDst[5] = indexOffset + 6;    // G
          pDst += 6;
        }

        if (AtlasNineSliceFlagsUtil::IsEnabled(flags, AtlasNineSliceFlags::Slice3Transparent) != drawOpaque)
        {
          // BDIK
          pDst[0] = indexOffset + 8;     // I
          pDst[1] = indexOffset + 1;     // B
          pDst[2] = indexOffset + 10;    // K

          pDst[3] = indexOffset + 10;    // K
          pDst[4] = indexOffset + 1;     // B
          pDst[5] = indexOffset + 3;     // D
          pDst += 6;
        }

        if (AtlasNineSliceFlagsUtil::IsEnabled(flags, AtlasNineSliceFlags::Slice4Transparent) != drawOpaque)
        {
          // DFKM
          pDst[0] = indexOffset + 10;    // K
          pDst[1] = indexOffset + 3;     // D
          pDst[2] = indexOffset + 12;    // M

          pDst[3] = indexOffset + 12;    // M
          pDst[4] = indexOffset + 3;     // D
          pDst[5] = indexOffset + 5;     // F
          pDst += 6;
        }

        if (AtlasNineSliceFlagsUtil::IsEnabled(flags, AtlasNineSliceFlags::Slice5Transparent) != drawOpaque)
        {
          // FHMO
          pDst[0] = indexOffset + 12;    // M
          pDst[1] = indexOffset + 5;     // F
          pDst[2] = indexOffset + 14;    // O

          pDst[3] = indexOffset + 14;    // O
          pDst[4] = indexOffset + 5;     // F
          pDst[5] = indexOffset + 7;     // H
          pDst += 6;
        }

        if (AtlasNineSliceFlagsUtil::IsEnabled(flags, AtlasNineSliceFlags::Slice6Transparent) != drawOpaque)
        {
          // IKJL
          pDst[0] = indexOffset + 9;     // J
          pDst[1] = indexOffset + 8;     // I
          pDst[2] = indexOffset + 11;    // L

          pDst[3] = indexOffset + 11;    // L
          pDst[4] = indexOffset + 8;     // I
          pDst[5] = indexOffset + 10;    // K
          pDst += 6;
        }

        if (AtlasNineSliceFlagsUtil::IsEnabled(flags, AtlasNineSliceFlags::Slice7Transparent) != drawOpaque)
        {
          // KMLN
          pDst[0] = indexOffset + 11;    // L
          pDst[1] = indexOffset + 10;    // K
          pDst[2] = indexOffset + 13;    // N

          pDst[3] = indexOffset + 13;    // N
          pDst[4] = indexOffset + 10;    // K
          pDst[5] = indexOffset + 12;    // M
          pDst += 6;
        }

        if (AtlasNineSliceFlagsUtil::IsEnabled(flags, AtlasNineSliceFlags::Slice8Transparent) != drawOpaque)
        {
          // MONP
          pDst[0] = indexOffset + 13;    // N
          pDst[1] = indexOffset + 12;    // M
          pDst[2] = indexOffset + 15;    // P

          pDst[3] = indexOffset + 15;    // P
          pDst[4] = indexOffset + 12;    // M
          pDst[5] = indexOffset + 14;    // O
          pDst += 6;
        }
        indexCount = UncheckedNumericCast<uint32_t>(pDst - (m_pIndexData + m_indexCount));
      }

      m_vertexCount += 8 * 2;
      m_indexCount += indexCount;
    }


    //! @brief  Add a nine slice
    //! @note   Mesh example:
    //!         X X X X
    //!         0 1 2 3
    //!
    //!         A-C-E-G Y0
    //!         | | | |
    //!         B-D-F-H Y1
    //!         | | | |
    //!         I-K-M-O Y2
    //!         | | | |
    //!         J-L-N-P Y3
    constexpr void AddNineSliceUVRotated90CW(const float x0, const float y0, const float x1, const float y1, const float x2, const float y2,
                                             const float x3, const float y3, const NativeNineSliceTextureArea& texCoordNineSlice) noexcept
    {
      assert((m_vertexCount + 16) <= m_vertexCapacity);
      assert((m_indexCount + 54) <= m_indexCapacity);

      {
        vertex_pointer pDst = m_pVertexData + m_vertexCount;
        //  u0 u1
        // v 0--1    None     = 0=u0,v0 1=u1,v0 2=u0,v1 3=u2,v1
        // 0 |  |    Rot90CW  = 0=u0,v1 1=u0,v0 2=u1,v1 3=u2,v0
        // v |  |    Rot180CW = 0=u1,v1 1=u0,v1 2=u1,v0 3=u0,v0
        // 1 2--3    Rot270CW = 0=u1,v0 1=u1,v1 2=u0,v0 3=u0,v1

        // u0v3, u0v2, u0v1, u0v0,
        // u1v3, u1v2, u1v1, u1v0,
        // u2v3, u2v2, u2v1, u2v0,
        // u3v3, u3v2, u3v1, u3v0,

        // 0123
        // ABCD
        // pDst[0] = TVertex(vertex_position_type(x0, y0, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y3));
        pDst[0].Position = vertex_position_type(x0, y0, m_zPos);
        pDst[0].Color = m_color;
        pDst[0].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y3);
        // pDst[1] = TVertex(vertex_position_type(x0, y1, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y3));
        pDst[1].Position = vertex_position_type(x0, y1, m_zPos);
        pDst[1].Color = m_color;
        pDst[1].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y3);
        // pDst[2] = TVertex(vertex_position_type(x1, y0, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y2));
        pDst[2].Position = vertex_position_type(x1, y0, m_zPos);
        pDst[2].Color = m_color;
        pDst[2].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y2);
        // pDst[3] = TVertex(vertex_position_type(x1, y1, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y2));
        pDst[3].Position = vertex_position_type(x1, y1, m_zPos);
        pDst[3].Color = m_color;
        pDst[3].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y2);
        // 4567
        // EFGH
        // pDst[4] = TVertex(vertex_position_type(x2, y0, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y1));
        pDst[4].Position = vertex_position_type(x2, y0, m_zPos);
        pDst[4].Color = m_color;
        pDst[4].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y1);
        // pDst[5] = TVertex(vertex_position_type(x2, y1, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y1));
        pDst[5].Position = vertex_position_type(x2, y1, m_zPos);
        pDst[5].Color = m_color;
        pDst[5].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y1);
        // pDst[6] = TVertex(vertex_position_type(x3, y0, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y0));
        pDst[6].Position = vertex_position_type(x3, y0, m_zPos);
        pDst[6].Color = m_color;
        pDst[6].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y0);
        // pDst[7] = TVertex(vertex_position_type(x3, y1, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y0));
        pDst[7].Position = vertex_position_type(x3, y1, m_zPos);
        pDst[7].Color = m_color;
        pDst[7].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y0);
        //   11
        // 8901
        // IJKL
        // pDst[8] = TVertex(vertex_position_type(x0, y2, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y3));
        pDst[8].Position = vertex_position_type(x0, y2, m_zPos);
        pDst[8].Color = m_color;
        pDst[8].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y3);
        // pDst[9] = TVertex(vertex_position_type(x0, y3, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y3));
        pDst[9].Position = vertex_position_type(x0, y3, m_zPos);
        pDst[9].Color = m_color;
        pDst[9].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y3);
        // pDst[10] = TVertex(vertex_position_type(x1, y2, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y2));
        pDst[10].Position = vertex_position_type(x1, y2, m_zPos);
        pDst[10].Color = m_color;
        pDst[10].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y2);
        // pDst[11] = TVertex(vertex_position_type(x1, y3, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y2));
        pDst[11].Position = vertex_position_type(x1, y3, m_zPos);
        pDst[11].Color = m_color;
        pDst[11].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y2);
        // 1111
        // 2345
        // MNOP
        // pDst[12] = TVertex(vertex_position_type(x2, y2, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y1));
        pDst[12].Position = vertex_position_type(x2, y2, m_zPos);
        pDst[12].Color = m_color;
        pDst[12].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y1);
        // pDst[13] = TVertex(vertex_position_type(x2, y3, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y1));
        pDst[13].Position = vertex_position_type(x2, y3, m_zPos);
        pDst[13].Color = m_color;
        pDst[13].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y1);
        // pDst[14] = TVertex(vertex_position_type(x3, y2, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y0));
        pDst[14].Position = vertex_position_type(x3, y2, m_zPos);
        pDst[14].Color = m_color;
        pDst[14].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y0);
        // pDst[15] = TVertex(vertex_position_type(x3, y3, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y0));
        pDst[15].Position = vertex_position_type(x3, y3, m_zPos);
        pDst[15].Color = m_color;
        pDst[15].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y0);
      }

      {
        index_pointer pDst = m_pIndexData + m_indexCount;
        const index_value_type indexOffset = m_indexVertexOffset + m_vertexCount;

        //                     1 1 1 1 1 1
        // 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
        // A B C D E F G H I J K L M N O P

        // ABCD
        pDst[0] = indexOffset + 1;    // B
        pDst[1] = indexOffset + 0;    // A
        pDst[2] = indexOffset + 3;    // D

        pDst[3] = indexOffset + 3;    // D
        pDst[4] = indexOffset + 0;    // A
        pDst[5] = indexOffset + 2;    // C

        // CDEF
        pDst[6] = indexOffset + 3;    // D
        pDst[7] = indexOffset + 2;    // C
        pDst[8] = indexOffset + 5;    // F

        pDst[9] = indexOffset + 5;     // F
        pDst[10] = indexOffset + 2;    // C
        pDst[11] = indexOffset + 4;    // E

        // EFGH
        pDst[12] = indexOffset + 5;    // F
        pDst[13] = indexOffset + 4;    // E
        pDst[14] = indexOffset + 7;    // H

        pDst[15] = indexOffset + 7;    // H
        pDst[16] = indexOffset + 4;    // E
        pDst[17] = indexOffset + 6;    // G

        // BDIK
        pDst[18] = indexOffset + 8;     // I
        pDst[19] = indexOffset + 1;     // B
        pDst[20] = indexOffset + 10;    // K

        pDst[21] = indexOffset + 10;    // K
        pDst[22] = indexOffset + 1;     // B
        pDst[23] = indexOffset + 3;     // D

        // DFKM
        pDst[24] = indexOffset + 10;    // K
        pDst[25] = indexOffset + 3;     // D
        pDst[26] = indexOffset + 12;    // M

        pDst[27] = indexOffset + 12;    // M
        pDst[28] = indexOffset + 3;     // D
        pDst[29] = indexOffset + 5;     // F

        // FHMO
        pDst[30] = indexOffset + 12;    // M
        pDst[31] = indexOffset + 5;     // F
        pDst[32] = indexOffset + 14;    // O

        pDst[33] = indexOffset + 14;    // O
        pDst[34] = indexOffset + 5;     // F
        pDst[35] = indexOffset + 7;     // H

        // IKJL
        pDst[36] = indexOffset + 9;     // J
        pDst[37] = indexOffset + 8;     // I
        pDst[38] = indexOffset + 11;    // L

        pDst[39] = indexOffset + 11;    // L
        pDst[40] = indexOffset + 8;     // I
        pDst[41] = indexOffset + 10;    // K

        // KMLN
        pDst[42] = indexOffset + 11;    // L
        pDst[43] = indexOffset + 10;    // K
        pDst[44] = indexOffset + 13;    // N

        pDst[45] = indexOffset + 13;    // N
        pDst[46] = indexOffset + 10;    // K
        pDst[47] = indexOffset + 12;    // M

        // MONP
        pDst[48] = indexOffset + 13;    // N
        pDst[49] = indexOffset + 12;    // M
        pDst[50] = indexOffset + 15;    // P

        pDst[51] = indexOffset + 15;    // P
        pDst[52] = indexOffset + 12;    // M
        pDst[53] = indexOffset + 14;    // O
      }

      m_vertexCount += 8 * 2;
      m_indexCount += (3 * 6) * 3;
    }

    //! @brief  Add a nine slice
    //! @note   Mesh example:
    //!         X X X X
    //!         0 1 2 3
    //!
    //!         A-C-E-G Y0
    //!         | | | |
    //!         B-D-F-H Y1
    //!         | | | |
    //!         I-K-M-O Y2
    //!         | | | |
    //!         J-L-N-P Y3
    constexpr void AddNineSliceUVRotated90CW(const float x0, const float y0, const float x1, const float y1, const float x2, const float y2,
                                             const float x3, const float y3, const NativeNineSliceTextureArea& texCoordNineSlice,
                                             const AtlasNineSliceFlags flags, const bool drawOpaque) noexcept
    {
      assert((m_vertexCount + 16) <= m_vertexCapacity);
      assert((m_indexCount + 54) <= m_indexCapacity);

      {
        vertex_pointer pDst = m_pVertexData + m_vertexCount;
        //  u0 u1
        // v 0--1    None     = 0=u0,v0 1=u1,v0 2=u0,v1 3=u2,v1
        // 0 |  |    Rot90CW  = 0=u0,v1 1=u0,v0 2=u1,v1 3=u2,v0
        // v |  |    Rot180CW = 0=u1,v1 1=u0,v1 2=u1,v0 3=u0,v0
        // 1 2--3    Rot270CW = 0=u1,v0 1=u1,v1 2=u0,v0 3=u0,v1

        // u0v3, u0v2, u0v1, u0v0,
        // u1v3, u1v2, u1v1, u1v0,
        // u2v3, u2v2, u2v1, u2v0,
        // u3v3, u3v2, u3v1, u3v0,

        // 0123
        // ABCD
        // pDst[0] = TVertex(vertex_position_type(x0, y0, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y3));
        pDst[0].Position = vertex_position_type(x0, y0, m_zPos);
        pDst[0].Color = m_color;
        pDst[0].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y3);
        // pDst[1] = TVertex(vertex_position_type(x0, y1, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y3));
        pDst[1].Position = vertex_position_type(x0, y1, m_zPos);
        pDst[1].Color = m_color;
        pDst[1].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y3);
        // pDst[2] = TVertex(vertex_position_type(x1, y0, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y2));
        pDst[2].Position = vertex_position_type(x1, y0, m_zPos);
        pDst[2].Color = m_color;
        pDst[2].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y2);
        // pDst[3] = TVertex(vertex_position_type(x1, y1, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y2));
        pDst[3].Position = vertex_position_type(x1, y1, m_zPos);
        pDst[3].Color = m_color;
        pDst[3].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y2);
        // 4567
        // EFGH
        // pDst[4] = TVertex(vertex_position_type(x2, y0, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y1));
        pDst[4].Position = vertex_position_type(x2, y0, m_zPos);
        pDst[4].Color = m_color;
        pDst[4].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y1);
        // pDst[5] = TVertex(vertex_position_type(x2, y1, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y1));
        pDst[5].Position = vertex_position_type(x2, y1, m_zPos);
        pDst[5].Color = m_color;
        pDst[5].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y1);
        // pDst[6] = TVertex(vertex_position_type(x3, y0, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y0));
        pDst[6].Position = vertex_position_type(x3, y0, m_zPos);
        pDst[6].Color = m_color;
        pDst[6].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X0, texCoordNineSlice.Y0);
        // pDst[7] = TVertex(vertex_position_type(x3, y1, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y0));
        pDst[7].Position = vertex_position_type(x3, y1, m_zPos);
        pDst[7].Color = m_color;
        pDst[7].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X1, texCoordNineSlice.Y0);
        //   11
        // 8901
        // IJKL
        // pDst[8] = TVertex(vertex_position_type(x0, y2, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y3));
        pDst[8].Position = vertex_position_type(x0, y2, m_zPos);
        pDst[8].Color = m_color;
        pDst[8].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y3);
        // pDst[9] = TVertex(vertex_position_type(x0, y3, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y3));
        pDst[9].Position = vertex_position_type(x0, y3, m_zPos);
        pDst[9].Color = m_color;
        pDst[9].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y3);
        // pDst[10] = TVertex(vertex_position_type(x1, y2, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y2));
        pDst[10].Position = vertex_position_type(x1, y2, m_zPos);
        pDst[10].Color = m_color;
        pDst[10].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y2);
        // pDst[11] = TVertex(vertex_position_type(x1, y3, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y2));
        pDst[11].Position = vertex_position_type(x1, y3, m_zPos);
        pDst[11].Color = m_color;
        pDst[11].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y2);
        // 1111
        // 2345
        // MNOP
        // pDst[12] = TVertex(vertex_position_type(x2, y2, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y1));
        pDst[12].Position = vertex_position_type(x2, y2, m_zPos);
        pDst[12].Color = m_color;
        pDst[12].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y1);
        // pDst[13] = TVertex(vertex_position_type(x2, y3, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y1));
        pDst[13].Position = vertex_position_type(x2, y3, m_zPos);
        pDst[13].Color = m_color;
        pDst[13].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y1);
        // pDst[14] = TVertex(vertex_position_type(x3, y2, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y0));
        pDst[14].Position = vertex_position_type(x3, y2, m_zPos);
        pDst[14].Color = m_color;
        pDst[14].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X2, texCoordNineSlice.Y0);
        // pDst[15] = TVertex(vertex_position_type(x3, y3, m_zPos), m_color, vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y0));
        pDst[15].Position = vertex_position_type(x3, y3, m_zPos);
        pDst[15].Color = m_color;
        pDst[15].TextureCoordinate = vertex_uv_type(texCoordNineSlice.X3, texCoordNineSlice.Y0);
      }
      uint32_t indexCount = 0;
      {
        index_pointer pDst = m_pIndexData + m_indexCount;
        const index_value_type indexOffset = m_indexVertexOffset + m_vertexCount;

        //                     1 1 1 1 1 1
        // 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
        // A B C D E F G H I J K L M N O P

        if (AtlasNineSliceFlagsUtil::IsEnabled(flags, AtlasNineSliceFlags::Slice0Transparent) != drawOpaque)
        {
          // ABCD
          pDst[0] = indexOffset + 1;    // B
          pDst[1] = indexOffset + 0;    // A
          pDst[2] = indexOffset + 3;    // D

          pDst[3] = indexOffset + 3;    // D
          pDst[4] = indexOffset + 0;    // A
          pDst[5] = indexOffset + 2;    // C
          pDst += 6;
        }


        if (AtlasNineSliceFlagsUtil::IsEnabled(flags, AtlasNineSliceFlags::Slice1Transparent) != drawOpaque)
        {
          // CDEF
          pDst[0] = indexOffset + 3;    // D
          pDst[1] = indexOffset + 2;    // C
          pDst[2] = indexOffset + 5;    // F

          pDst[3] = indexOffset + 5;    // F
          pDst[4] = indexOffset + 2;    // C
          pDst[5] = indexOffset + 4;    // E
          pDst += 6;
        }

        if (AtlasNineSliceFlagsUtil::IsEnabled(flags, AtlasNineSliceFlags::Slice2Transparent) != drawOpaque)
        {
          // EFGH
          pDst[0] = indexOffset + 5;    // F
          pDst[1] = indexOffset + 4;    // E
          pDst[2] = indexOffset + 7;    // H

          pDst[3] = indexOffset + 7;    // H
          pDst[4] = indexOffset + 4;    // E
          pDst[5] = indexOffset + 6;    // G
          pDst += 6;
        }

        if (AtlasNineSliceFlagsUtil::IsEnabled(flags, AtlasNineSliceFlags::Slice3Transparent) != drawOpaque)
        {
          // BDIK
          pDst[0] = indexOffset + 8;     // I
          pDst[1] = indexOffset + 1;     // B
          pDst[2] = indexOffset + 10;    // K

          pDst[3] = indexOffset + 10;    // K
          pDst[4] = indexOffset + 1;     // B
          pDst[5] = indexOffset + 3;     // D
          pDst += 6;
        }

        if (AtlasNineSliceFlagsUtil::IsEnabled(flags, AtlasNineSliceFlags::Slice4Transparent) != drawOpaque)
        {
          // DFKM
          pDst[0] = indexOffset + 10;    // K
          pDst[1] = indexOffset + 3;     // D
          pDst[2] = indexOffset + 12;    // M

          pDst[3] = indexOffset + 12;    // M
          pDst[4] = indexOffset + 3;     // D
          pDst[5] = indexOffset + 5;     // F
          pDst += 6;
        }

        if (AtlasNineSliceFlagsUtil::IsEnabled(flags, AtlasNineSliceFlags::Slice5Transparent) != drawOpaque)
        {
          // FHMO
          pDst[0] = indexOffset + 12;    // M
          pDst[1] = indexOffset + 5;     // F
          pDst[2] = indexOffset + 14;    // O

          pDst[3] = indexOffset + 14;    // O
          pDst[4] = indexOffset + 5;     // F
          pDst[5] = indexOffset + 7;     // H
          pDst += 6;
        }

        if (AtlasNineSliceFlagsUtil::IsEnabled(flags, AtlasNineSliceFlags::Slice6Transparent) != drawOpaque)
        {
          // IKJL
          pDst[0] = indexOffset + 9;     // J
          pDst[1] = indexOffset + 8;     // I
          pDst[2] = indexOffset + 11;    // L

          pDst[3] = indexOffset + 11;    // L
          pDst[4] = indexOffset + 8;     // I
          pDst[5] = indexOffset + 10;    // K
          pDst += 6;
        }

        if (AtlasNineSliceFlagsUtil::IsEnabled(flags, AtlasNineSliceFlags::Slice7Transparent) != drawOpaque)
        {
          // KMLN
          pDst[0] = indexOffset + 11;    // L
          pDst[1] = indexOffset + 10;    // K
          pDst[2] = indexOffset + 13;    // N

          pDst[3] = indexOffset + 13;    // N
          pDst[4] = indexOffset + 10;    // K
          pDst[5] = indexOffset + 12;    // M
          pDst += 6;
        }

        if (AtlasNineSliceFlagsUtil::IsEnabled(flags, AtlasNineSliceFlags::Slice8Transparent) != drawOpaque)
        {
          // MONP
          pDst[0] = indexOffset + 13;    // N
          pDst[1] = indexOffset + 12;    // M
          pDst[2] = indexOffset + 15;    // P

          pDst[3] = indexOffset + 15;    // P
          pDst[4] = indexOffset + 12;    // M
          pDst[5] = indexOffset + 14;    // O
          pDst += 6;
        }
        indexCount = UncheckedNumericCast<uint32_t>(pDst - (m_pIndexData + m_indexCount));
      }

      m_vertexCount += 8 * 2;
      m_indexCount += indexCount;
    }


    //! @brief  Add a nine slice
    //! @note   Mesh example:
    //!         X X X X
    //!         0 1 2 3
    //!
    //!         A-C-E-G Y0
    //!         | | | |
    //!         B-D-F-H Y1
    //!         | | | |
    //!         I-K-M-O Y2
    //!         | | | |
    //!         J-L-N-P Y3
    constexpr void AddNineSlice(const float x0, const float y0, const float x1, const float y1, const float x2, const float y2, const float x3,
                                const float y3, const NativeNineSliceTextureArea& texCoordNineSlice,
                                const PxAreaRectangleF& clippedTargetRectPxf) noexcept
    {
      assert((m_vertexCount + 16) <= m_vertexCapacity);
      assert((m_indexCount + 54) <= m_indexCapacity);

      PxVector2 clipXPxf(clippedTargetRectPxf.Left(), clippedTargetRectPxf.Right());
      PxVector2 clipYPxf(clippedTargetRectPxf.Top(), clippedTargetRectPxf.Bottom());

      std::array<PxVector2, 4> coordsX = {PxVector2::Create(x0, texCoordNineSlice.X0), PxVector2::Create(x1, texCoordNineSlice.X1),
                                          PxVector2::Create(x2, texCoordNineSlice.X2), PxVector2::Create(x3, texCoordNineSlice.X3)};
      std::array<PxVector2, 4> coordsY = {PxVector2::Create(y0, texCoordNineSlice.Y0), PxVector2::Create(y1, texCoordNineSlice.Y1),
                                          PxVector2::Create(y2, texCoordNineSlice.Y2), PxVector2::Create(y3, texCoordNineSlice.Y3)};

      ReadOnlySpan<PxVector2> spanX = Clip2DUtil::Clip(coordsX, clipXPxf);
      ReadOnlySpan<PxVector2> spanY = Clip2DUtil::Clip(coordsY, clipYPxf);
      if (spanX.size() >= 2 && spanY.size() >= 2)
      {
        const auto countY = static_cast<uint32_t>(spanY.size());
        const auto countX = static_cast<uint32_t>(spanX.size());

        {    // Generate the vertices
          vertex_pointer pDst = m_pVertexData + m_vertexCount;
          for (uint32_t indexY = 0; indexY < countY; ++indexY)
          {
            for (uint32_t indexX = 0; indexX < countX; ++indexX)
            {
              //*pDst = TVertex(vertex_position_type(spanX[indexX].X, spanY[indexY].X, m_zPos), m_color,
              //                          vertex_uv_type(spanX[indexX].Y, spanY[indexY].Y));
              pDst->Position = vertex_position_type(spanX[indexX].X.Value, spanY[indexY].X.Value, m_zPos);
              pDst->Color = m_color;
              pDst->TextureCoordinate = vertex_uv_type(spanX[indexX].Y.Value, spanY[indexY].Y.Value);
              ++pDst;
            }
          }
        }
        {    // Generate the indices
             //!  A-B-C-D
             //!  |\|\|\|
             //!  E-F-G-H
          index_pointer pDst = m_pIndexData + m_indexCount;
          index_value_type indexOffset = m_indexVertexOffset + m_vertexCount;
          const uint32_t endY = countY - 1;
          const uint32_t endX = countX - 1;
          for (uint32_t indexY = 0; indexY < endY; ++indexY)
          {
            for (uint32_t indexX = 0; indexX < endX; ++indexX)
            {
              // ABCD
              pDst[0] = indexOffset + countX;        // E
              pDst[1] = indexOffset + 0;             // A
              pDst[2] = indexOffset + countX + 1;    // F

              pDst[3] = indexOffset + countX + 1;    // F
              pDst[4] = indexOffset + 0;             // A
              pDst[5] = indexOffset + 1;             // B

              pDst += 6;
              ++indexOffset;
            }
            ++indexOffset;
          }
        }

        m_vertexCount += countX * countY;
        m_indexCount += 6 * (countX - 1) * (countY - 1);
      }
    }
  };

}


#endif
