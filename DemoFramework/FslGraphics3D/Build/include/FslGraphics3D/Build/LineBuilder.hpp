#ifndef FSLGRAPHICS3D_BUILD_LINEBUILDER_HPP
#define FSLGRAPHICS3D_BUILD_LINEBUILDER_HPP
/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/BoundingSphere.hpp>
#include <FslBase/Math/Rect.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/Vertices/VertexPositionColor.hpp>
#include <FslGraphics/Vertices/VertexSpan.hpp>
#include <algorithm>
#include <array>
#include <cassert>
#include <limits>
#include <vector>

namespace Fsl
{
  struct BoundingBox;
  class BoundingFrustum;
  struct BoxF;
  struct Ray;
  struct Rect;
  struct Rectangle;
  struct Rectangle3D;
  struct Rectangle2D;

  namespace Graphics3D
  {
    //! @brief A simple vertex builder for colored lines.
    //! @note  A easy way to create colored lines with lots of useful functionality for debugging.
    //!        ok performance for 'dynamic' per frame lines (for a simple plug and play solution).
    //!        Also works fine for static line vertex generation.
    class LineBuilder
    {
      static constexpr std::size_t MIN_VERTEX_CAPACITY = 512;
      static constexpr float RAY_LENGTH = 200000.0f;

      std::vector<VertexPositionColor> m_vertices;
      //! The number of vertices currently in use
      uint32_t m_entries{0};

      std::array<Vector3, 8> m_cornersScratchpad;

    public:
      using vertex_type = VertexPositionColor;
      static constexpr const uint32_t VERTICES_PER_LINE = 2;
      static constexpr const uint32_t INITIAL_LINE_CAPACITY = VERTICES_PER_LINE * 8192;
      static constexpr const uint32_t GROW_BY = VERTICES_PER_LINE * 2048;
      static constexpr const uint32_t DEFAULT_SPHERE_STEPS = 24;

      LineBuilder()
        : LineBuilder(INITIAL_LINE_CAPACITY)
      {
      }

      explicit LineBuilder(const uint32_t initialLineCapacity)
        : m_vertices(std::max(static_cast<std::size_t>(initialLineCapacity) * static_cast<std::size_t>(VERTICES_PER_LINE), MIN_VERTEX_CAPACITY))
      {
      }

      inline uint32_t VertexCapacity() const
      {
        // Validate the cast
        assert(m_vertices.size() <= std::numeric_limits<uint32_t>::max());
        return static_cast<uint32_t>(m_vertices.size());
      }

      inline uint32_t VertexCount() const
      {
        return m_entries;
      }

      inline VertexSpan<vertex_type> GetVertexSpan() const
      {
        return {m_vertices.data(), m_entries};
      }

      inline bool IsEmpty() const
      {
        return m_entries == 0u;
      }

      inline void Clear()
      {
        m_entries = 0u;
      }

      inline uint32_t LineCount() const
      {
        assert((m_entries % VERTICES_PER_LINE) == 0u);
        return m_entries / VERTICES_PER_LINE;
      }

      // Color color

      //! @brief Add a line of the given color
      // inline void Add(const Vector3& from, const Vector3& to, const Vector4& color)
      //{
      //  Add(from, to, Color(color));
      //}

      ////! @brief Add a line of the given color and transform it by the supplied matrix
      // inline void Add(const Vector3& from, const Vector3& to, const Vector4& color, const Matrix& matrix)
      //{
      //  Add(from, to, Color(color), matrix);
      //}

      ////! @brief Add a line of the given color
      // inline void Add(const Vector3& from, const Vector3& to, const Vector4& colorFrom, const Color& colorTo)
      //{
      //  Add(from, to, Color(colorFrom), Color(colorTo));
      //}

      ////! @brief Add a line of the given color and transform it by the supplied matrix
      // inline void Add(const Vector3& from, const Vector3& to, const Vector4& colorFrom, const Vector4& colorTo, const Matrix& matrix)
      //{
      //  Add(from, to, Color(colorFrom), Color(colorTo), matrix);
      //}

      // inline void Add(const BoundingBox& value, const Vector4& color)
      //{
      //  Add(value, Color(color));
      //}

      // inline void Add(const BoundingBox& value, const Vector4& color, const Matrix& matrix)
      //{
      //  Add(value, Color(color), matrix);
      //}

      // inline void Add(const BoundingFrustum& value, const Vector4& color)
      //{
      //  Add(value, Color(color));
      //}
      // inline void Add(const BoundingFrustum& value, const Vector4& color, const Matrix& matrix)
      //{
      //  Add(value, Color(color), matrix);
      //}

      // inline void Add(const BoundingSphere& value, const Vector4& color, const uint32_t steps = DEFAULT_SPHERE_STEPS)
      //{
      //  Add(value, Color(color), steps);
      //}

      // inline void Add(const BoundingSphere& value, const Vector4& color, const Matrix& matrix, const uint32_t steps = DEFAULT_SPHERE_STEPS)
      //{
      //  Add(value, Color(color), matrix, steps);
      //}

      // inline void Add(const BoxF& value, const Vector4& color)
      //{
      //  Add(value, Color(color));
      //}

      // inline void Add(const BoxF& value, const Vector4& color, const Matrix& matrix)
      //{
      //  Add(value, Color(color), matrix);
      //}

      // void Add(const Ray& value, const Vector4& color)
      //{
      //  Add(value, Color(color));
      //}

      // void Add(const Ray& value, const Vector4& color, const Matrix& matrix)
      //{
      //  Add(value, Color(color), matrix);
      //}

      // void Add(const Rect& value, const Vector4& color)
      //{
      //  Add(value, Color(color));
      //}

      // void Add(const Rect& value, const Vector4& color, const Matrix& matrix)
      //{
      //  Add(value, Color(color), matrix);
      //}

      // void Add(const Rectangle& value, const Vector4& color)
      //{
      //  Add(value, Color(color));
      //}

      // void Add(const Rectangle& value, const Vector4& color, const Matrix& matrix)
      //{
      //  Add(value, Color(color), matrix);
      //}

      // void Add(const Rectangle2D& value, const Vector4& color)
      //{
      //  Add(value, Color(color));
      //}

      // void Add(const Rectangle2D& value, const Vector4& color, const Matrix& matrix)
      //{
      //  Add(value, Color(color), matrix);
      //}

      // void Add(const Rectangle3D& value, const Vector4& color)
      //{
      //  Add(value, Color(color));
      //}

      // void Add(const Rectangle3D& value, const Vector4& color, const Matrix& matrix)
      //{
      //  Add(value, Color(color), matrix);
      //}

      // void AddGridXY(const Rect& rect, const float posZ, const uint32_t steps, const Vector4& color)
      //{
      //  AddGridXY(rect, posZ, steps, Color(color));
      //}

      // void AddGridXY(const Rect& rect, const float posZ, const uint32_t steps, const Vector4& color, const Matrix& matrix)
      //{
      //  AddGridXY(rect, posZ, steps, Color(color), matrix);
      //}

      // void AddGridXZ(const Rect& rect, const float posY, const uint32_t steps, const Vector4& color)
      //{
      //  AddGridXZ(rect, posY, steps, Color(color));
      //}

      // void AddGridXZ(const Rect& rect, const float posY, const uint32_t steps, const Vector4& color, const Matrix& matrix)
      //{
      //  AddGridXZ(rect, posY, steps, Color(color), matrix);
      //}

      // VECTOR4 colors

      //! @brief Add a line of the given color
      void Add(const Vector3& from, const Vector3& to, const Color& color)
      {
        EnsureCapacityFor(2u);
        auto* pDst = m_vertices.data() + m_entries;

        pDst[0].Position = from;
        pDst[0].Color = color;
        pDst[1].Position = to;
        pDst[1].Color = color;

        m_entries += 2u;
        assert(m_entries <= m_vertices.size());
      }


      //! @brief Add a line of the given color and transform it by the supplied matrix
      void Add(const Vector3& from, const Vector3& to, const Color& color, const Matrix& matrix)
      {
        EnsureCapacityFor(2u);
        auto* pDst = m_vertices.data() + m_entries;

        Vector3::Transform(from, matrix, pDst[0].Position);
        pDst[0].Color = color;
        Vector3::Transform(to, matrix, pDst[1].Position);
        pDst[1].Color = color;

        m_entries += 2u;
        assert(m_entries <= m_vertices.size());
      }

      void Add(const Vector3& from, const Vector3& to, const Color& colorFrom, const Color& colorTo)
      {
        EnsureCapacityFor(2u);
        auto* pDst = m_vertices.data() + m_entries;

        pDst[0].Position = from;
        pDst[0].Color = colorFrom;
        pDst[1].Position = to;
        pDst[1].Color = colorTo;

        m_entries += 2u;
        assert(m_entries <= m_vertices.size());
      }

      void Add(const Vector3& from, const Vector3& to, const Color& colorFrom, const Color& colorTo, const Matrix& matrix)
      {
        EnsureCapacityFor(2u);
        auto* pDst = m_vertices.data() + m_entries;

        Vector3::Transform(from, matrix, pDst[0].Position);
        pDst[0].Color = colorFrom;
        Vector3::Transform(to, matrix, pDst[1].Position);
        pDst[1].Color = colorTo;

        m_entries += 2u;
        assert(m_entries <= m_vertices.size());
      }

      void Add(const BoundingBox& boundingBox, const Color& color);
      void Add(const BoundingBox& boundingBox, const Color& color, const Matrix& matrix);
      void Add(const BoundingFrustum& boundingFrustum, const Color& color);
      void Add(const BoundingFrustum& boundingFrustum, const Color& color, const Matrix& matrix);
      void Add(const BoundingSphere& boundingSphere, const Color& color, const uint32_t steps = DEFAULT_SPHERE_STEPS)
      {
        AddSphere(boundingSphere.Center, boundingSphere.Radius, color, color, color, steps);
      }
      void Add(const BoundingSphere& boundingSphere, const Color& color, const Matrix& matrix, const uint32_t steps = DEFAULT_SPHERE_STEPS)
      {
        AddSphere(boundingSphere.Center, boundingSphere.Radius, color, color, color, matrix, steps);
      }
      void Add(const BoxF& value, const Color& color);
      void Add(const BoxF& value, const Color& color, const Matrix& matrix);

      void Add(const Ray& value, const Color& color);
      void Add(const Ray& value, const Color& color, const Matrix& matrix);
      void Add(const Rect& value, const Color& color);
      void Add(const Rect& value, const Color& color, const Matrix& matrix);
      void Add(const Rectangle& value, const Color& color);
      void Add(const Rectangle& value, const Color& color, const Matrix& matrix);
      void Add(const Rectangle2D& value, const Color& color);
      void Add(const Rectangle2D& value, const Color& color, const Matrix& matrix);
      void Add(const Rectangle3D& value, const Color& color);
      void Add(const Rectangle3D& value, const Color& color, const Matrix& matrix);

      //! @note  x will be red, y will be green, z will be blue
      inline void AddAxisSphere(const BoundingSphere& sphere)
      {
        AddAxisSphere(sphere.Center, sphere.Radius);
      }

      //! @note  x will be red, y will be green, z will be blue
      inline void AddAxisSphere(const BoundingSphere& sphere, const Matrix& matrix)
      {
        AddAxisSphere(sphere.Center, sphere.Radius, matrix);
      }

      //! @note  x will be red, y will be green, z will be blue
      inline void AddAxisSphere(const Vector3& center, const float radius, const uint32_t steps = DEFAULT_SPHERE_STEPS)
      {
        AddSphere(center, radius, Color::Red(), Color::Green(), Color::Blue(), steps);
      }

      //! @note  x will be red, y will be green, z will be blue
      inline void AddAxisSphere(const Vector3& center, const float radius, const Matrix& matrix, const uint32_t steps = DEFAULT_SPHERE_STEPS)
      {
        AddSphere(center, radius, Color::Red(), Color::Green(), Color::Blue(), matrix, steps);
      }

      //! @brief Add a coordinate system
      //! @note  x will be red, y will be green, z will be blue
      void AddAxis(const Vector3& position, const float axisLength);

      //! @brief Add a coordinate system
      //! @note  x will be red, y will be green, z will be blue
      void AddAxis(const Vector3& position, const float axisLength, const Matrix& matrix);

      void AddGridXY(const Rectangle& rect, const float posZ, const uint32_t steps, const Color& color)
      {
        AddGridXY(ToRect(rect), posZ, steps, color);
      }

      void AddGridXY(const Rectangle& rect, const float posZ, const uint32_t steps, const Color& color, const Matrix& matrix)
      {
        AddGridXY(ToRect(rect), posZ, steps, color, matrix);
      }

      void AddGridXY(const Rectangle& rect, const float posZ, const uint32_t stepsX, const uint32_t stepsY, const Color& color)
      {
        AddGridXY(ToRect(rect), posZ, stepsX, stepsY, color);
      }

      void AddGridXY(const Rectangle& rect, const float posZ, const uint32_t stepsX, const uint32_t stepsY, const Color& color, const Matrix& matrix)
      {
        AddGridXY(ToRect(rect), posZ, stepsX, stepsY, color, matrix);
      }


      void AddGridXY(const Rect& rect, const float posZ, const uint32_t steps, const Color& color)
      {
        return AddGridXY(rect, posZ, steps, steps, color);
      }

      void AddGridXY(const Rect& rect, const float posZ, const uint32_t steps, const Color& color, const Matrix& matrix)
      {
        return AddGridXY(rect, posZ, steps, steps, color, matrix);
      }

      void AddGridXY(const Rect& rect, const float posZ, const uint32_t stepsX, const uint32_t stepsY, const Color& color);
      void AddGridXY(const Rect& rect, const float posZ, const uint32_t stepsX, const uint32_t stepsY, const Color& color, const Matrix& matrix);

      void AddGridXZ(const Rect& rect, const float posY, const uint32_t steps, const Color& color);
      void AddGridXZ(const Rect& rect, const float posY, const uint32_t steps, const Color& color, const Matrix& matrix);

      void AddLines(const vertex_type* const pVertices, const std::size_t vertexCount);
      void AddLines(const vertex_type* const pVertices, const std::size_t vertexCount, const Matrix& matrix);


      template <std::size_t TSize>
      void AddLines(const std::array<vertex_type, TSize>& vertices)
      {
        AddLines(vertices.data(), vertices.size());
      }

      void AddLines(const std::vector<vertex_type>& vertices)
      {
        AddLines(vertices.data(), vertices.size());
      }

      template <std::size_t TSize>
      void AddLines(const std::array<vertex_type, TSize>& vertices, const Matrix& matrix)
      {
        AddLines(vertices.data(), vertices.size(), matrix);
      }

      void AddLines(const std::vector<vertex_type>& vertices, const Matrix& matrix)
      {
        AddLines(vertices.data(), vertices.size(), matrix);
      }

      void AddSphere(const Vector3& center, const float radius, const Color& colYZ, const Color& colXZ, const Color& colXY,
                     const uint32_t steps = DEFAULT_SPHERE_STEPS);
      void AddSphere(const Vector3& center, const float radius, const Color& colYZ, const Color& colXZ, const Color& colXY, const Matrix& matrix,
                     const uint32_t steps = DEFAULT_SPHERE_STEPS);

    private:
      constexpr Rect ToRect(const Rectangle& rect) noexcept
      {
        return Rect::FromLeftTopRightBottom(static_cast<float>(rect.Left()) + 0.5f, static_cast<float>(rect.Top()) + 0.5f,
                                            static_cast<float>(rect.Right()) + 0.5f, static_cast<float>(rect.Bottom()) + 0.5f);
      }

      void EnsureCapacityFor(const std::size_t vertexCount)
      {
        const auto capacityLeft = (m_vertices.size() - m_entries);
        if (vertexCount > capacityLeft)
        {
          const auto missingCapacity = (vertexCount - capacityLeft);
          const auto numBucketsFull = missingCapacity / GROW_BY;
          const auto numLeft = missingCapacity % GROW_BY;
          auto newCapacity = m_vertices.size() + ((numBucketsFull + (numLeft > 0 ? 1 : 0)) * GROW_BY);
          if (newCapacity > std::numeric_limits<uint32_t>::max())
          {
            // ok we don't want to exceed a uint32_t
            newCapacity = std::numeric_limits<uint32_t>::max();
            assert(m_entries <= newCapacity);
            if (missingCapacity > (newCapacity - m_entries))
            {
              throw NotSupportedException("Capacity reached");
            }
          }
          m_vertices.resize(newCapacity);
        }
      }
    };
  }
}

#endif
