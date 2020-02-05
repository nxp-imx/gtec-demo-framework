/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <Shared/TextureCompression/VerticalScroller.hpp>
#include <FslBase/Math/EqualHelper.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslSimpleUI/Base/LayoutHelper.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <algorithm>

namespace Fsl
{
  namespace UI
  {
    VerticalScroller::VerticalScroller(const std::shared_ptr<BaseWindowContext>& context)
      : ContentControl(context)
    {
      Enable(WindowFlags::UpdateEnabled);
    }


    void VerticalScroller::WinUpdate(const DemoTime& demoTime)
    {
      if (!m_animate)
      {
        return;
      }

      m_animationPosition += m_animationSpeed * demoTime.DeltaTime;
      if (m_animationPosition > MathHelper::RADS360)
      {
        m_animationPosition -= MathHelper::RADS360;
      }
      PropertyUpdated(PropertyType::Layout);
    }


    void VerticalScroller::SetScrollPadding(const ThicknessF& padding)
    {
      m_scrollPadding = padding;
    }


    Vector2 VerticalScroller::ArrangeOverride(const Vector2& finalSize)
    {
      if (m_animate)
      {
        auto contentDesiredSize = GetContentDesiredSize();
        contentDesiredSize.X = finalSize.X;
        contentDesiredSize.Y += m_scrollPadding.SumY();
        auto scollarea = (contentDesiredSize.Y - finalSize.Y);
        float pos = scollarea > 0.0f ? -(((std::cos(m_animationPosition + MathHelper::RADS180) + 1.0f) / 2.0f) * scollarea) : 0.0f;
        ContentControl::CustomArrange(contentDesiredSize, Vector2(0, pos));

        m_animationSpeed = scollarea > 0.001f ? (1.0f / scollarea) * 600.0f : 0.0f;
        m_animationSpeed = std::min(m_animationSpeed, 4.0f);
        return finalSize;
      }

      return ContentControl::ArrangeOverride(finalSize);
    }


    Vector2 VerticalScroller::MeasureOverride(const Vector2& availableSize)
    {
      auto desiredSize = ContentControl::MeasureOverride(Vector2(availableSize.X, LayoutHelper::InfiniteSpace));
      if (desiredSize.Y > availableSize.Y)
      {
        // Enable scrolling
        m_animate = true;
      }
      else
      {
        m_animate = false;
        m_animationPosition = 0.0f;
      }
      // We always only take the available size in y (this is kind of a hack, but it works for this purpose)
      // desiredSize.Y = availableSize.Y;
      return availableSize;
    }
  }
}
