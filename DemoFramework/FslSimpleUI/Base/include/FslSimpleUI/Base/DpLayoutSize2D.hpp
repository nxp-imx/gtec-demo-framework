#ifndef FSLSIMPLEUI_BASE_DPLAYOUTSIZE2D_HPP
#define FSLSIMPLEUI_BASE_DPLAYOUTSIZE2D_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslSimpleUI/Base/DpLayoutSize1D.hpp>

namespace Fsl
{
  namespace UI
  {
    struct DpLayoutSize2D
    {
      using value_type = float;

    private:
      value_type m_width{-1.0f};
      value_type m_height{-1.0f};

    public:
      constexpr DpLayoutSize2D() = default;

      constexpr explicit DpLayoutSize2D(const value_type width, const value_type height)
        : m_width(width >= 0.0f ? width : -1.0f)
        , m_height(height >= 0.0f ? height : -1.0f)
      {
      }

      constexpr bool HasWidthValue() const
      {
        return m_width >= 0.0f;
      }

      constexpr bool HasHeightValue() const
      {
        return m_height >= 0.0f;
      }

      constexpr float Width() const
      {
        return (m_width >= 0.0f ? m_width : 0.0f);
      }


      constexpr float Height() const
      {
        return (m_height >= 0.0f ? m_height : 0.0f);
      }

      constexpr DpLayoutSize1D LayoutSizeWidth() const
      {
        return DpLayoutSize1D(m_width);
      }

      constexpr DpLayoutSize1D LayoutSizeHeight() const
      {
        return DpLayoutSize1D(m_height);
      }

      constexpr void SetWidth(const DpLayoutSize1D& value)
      {
        m_width = value.RawValue();
      }

      constexpr void SetHeight(const DpLayoutSize1D& value)
      {
        m_height = value.RawValue();
      }
    };
  }

}

#endif
