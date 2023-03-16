#ifndef FSLBASE_MATH_PIXEL_PXVIEWPORT_HPP
#define FSLBASE_MATH_PIXEL_PXVIEWPORT_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxRectangle.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>

namespace Fsl
{
  struct PxViewport
  {
  private:
    int32_t m_x{0};
    int32_t m_y{0};
    int32_t m_width{0};
    int32_t m_height{0};
    float m_minDepth{0};
    float m_maxDepth{0};

  public:
    constexpr PxViewport() noexcept = default;

    explicit constexpr PxViewport(const PxRectangle& bounds) noexcept
      : PxViewport(bounds.X(), bounds.Y(), bounds.Width(), bounds.Height())
    {
    }

    constexpr PxViewport(const PxRectangle& src, const float minDepth, const float maxDepth) noexcept
      : PxViewport(src.Left(), src.Top(), src.Width(), src.Height(), minDepth, maxDepth)
    {
    }

    constexpr PxViewport(const int32_t x, const int32_t y, const int32_t width, const int32_t height, const float minDepth = 0.0f,
                         const float maxDepth = 1.0f) noexcept
      : m_x(x)
      , m_y(y)
      , m_width(width >= 0 ? width : 0)
      , m_height(height >= 0 ? height : 0)
      , m_minDepth(minDepth)
      , m_maxDepth(maxDepth)
    {
    }

    constexpr int32_t X() const noexcept
    {
      return m_x;
    }

    constexpr int32_t Y() const noexcept
    {
      return m_y;
    }

    constexpr int32_t Width() const noexcept
    {
      return m_width;
    }

    constexpr int32_t Height() const noexcept
    {
      return m_height;
    }

    constexpr float MinDepth() const noexcept
    {
      return m_minDepth;
    }

    constexpr float MaxDepth() const noexcept
    {
      return m_maxDepth;
    }

    constexpr int32_t Left() const
    {
      return m_x;
    }

    constexpr int32_t Top() const
    {
      return m_y;
    }

    constexpr int32_t Right() const
    {
      return m_x + m_width;
    }

    constexpr int32_t Bottom() const
    {
      return m_y + m_height;
    }

    constexpr PxPoint2 Location() const
    {
      return {m_x, m_y};
    }

    constexpr PxSize2D GetSize() const
    {
      return {m_width, m_height};
    }

    constexpr float GetAspectRatio() const
    {
      return m_width != 0 && m_height != 0 ? static_cast<float>(m_width) / static_cast<float>(m_height) : 0.0f;
    }

    constexpr bool operator==(const PxViewport& rhs) const noexcept
    {
      return m_x == rhs.m_x && m_y == rhs.m_y && m_width == rhs.m_width && m_height == rhs.m_height && m_minDepth == rhs.m_minDepth &&
             m_maxDepth == rhs.m_maxDepth;
    }

    constexpr bool operator!=(const PxViewport& rhs) const noexcept
    {
      return !(*this == rhs);
    }

    constexpr PxRectangle GetBounds() noexcept
    {
      return {m_x, m_y, m_width, m_height};
    }

    constexpr static PxViewport SetBounds(PxViewport& src, const PxRectangle& rectangle) noexcept
    {
      return {rectangle, src.MinDepth(), src.MaxDepth()};
    }
  };
}

#endif
