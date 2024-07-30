#ifndef FSLSIMPLEUI_BASE_CONTROL_EXPERIMENTAL_HISTOGRAM_HPP
#define FSLSIMPLEUI_BASE_CONTROL_EXPERIMENTAL_HISTOGRAM_HPP
#ifdef FSL_DISABLED
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

#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Render/NineSliceAtlasTexture2D.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Base/Control/Util/Draw9SliceUtil.hpp>
#include <FslSimpleUI/Base/DefaultValues.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <algorithm>
#include <cassert>
#include <vector>

namespace Fsl
{
  namespace UI
  {
    class WindowContext;

    //! Experimental histogram rendering.
    //! The next revision of this should probably just take a list of bars and their height instead!
    template <typename T>
    class Histogram : public BaseWindow
    {
      using base_type = BaseWindow;

      struct BarGraphics
      {
        NineSliceAtlasTexture2D NineSliceTexture;
        UIColor PrimaryColor{DefaultColor::Palette::Primary};
      };

      struct BarRecord
      {
        uint32_t Entries{};
      };


    public:
      using value_type = T;

    private:
      struct HistogramInfo
      {
        value_type Min{};
        value_type Max{};
        uint32_t MaxEntriesInOneBar{};

        HistogramInfo() = default;
        HistogramInfo(const value_type min, const value_type max, const uint32_t maxEntriesInOneBar)
          : Min(min)
          , Max(std::max(max, min))
          , MaxEntriesInOneBar(maxEntriesInOneBar)
        {
        }
      };

      std::shared_ptr<WindowContext> m_windowContext;

      BarGraphics m_bar;
      std::vector<value_type> m_values;
      std::vector<BarRecord> m_bars;
      HistogramInfo m_info;

    public:
      explicit Histogram(const std::shared_ptr<WindowContext>& context)
        : BaseWindow(context)
        , m_windowContext(context)
        , m_bars(10)
      {
        // We need to be draw enabled
        Enable(WindowFlags::DrawEnabled);
      }

      const NineSliceAtlasTexture2D& GetBarTexture() const
      {
        return m_bar.Texture;
      }

      void SetBarTexture(const NineSliceAtlasTexture2D& value)
      {
        if (value != m_bar.NineSliceTexture)
        {
          m_bar.NineSliceTexture = value;
          PropertyUpdated(PropertyType::Content);
        }
      }

      UIColor GetBarColor() const
      {
        return m_bar.PrimaryColor;
      }

      void SetBarColor(const UIColor value)
      {
        if (value != m_bar.PrimaryColor)
        {
          m_bar.PrimaryColor = value;
          PropertyUpdated(PropertyType::Other);
        }
      }

      const std::vector<value_type>& GetData() const
      {
        return m_values;
      }

      bool SetData(const std::vector<value_type>& values, const uint16_t barCount)
      {
        const uint16_t cappedBarCount = std::max(barCount, uint16_t(1u));
        if (cappedBarCount != m_bars.size() || values != m_values)
        {
          m_values = values;

          {    // now build the histogram for the data
            const auto range = std::minmax_element(m_values.begin(), m_values.end());
            BuildHistogram(*range.first, *range.second, cappedBarCount);
          }
          return true;
        }
        return false;
      }

      bool SetData(const std::vector<value_type>& values, const value_type minValue, const value_type maxValue, const uint16_t barCount)
      {
        const uint16_t cappedBarCount = std::max(barCount, uint16_t(1u));
        if (cappedBarCount != m_bars.size() || values != m_values)
        {
          m_values = values;
          // now build the histogram for the data
          BuildHistogram(minValue, maxValue, cappedBarCount);
          return true;
        }
        return false;
      }


      void WinDraw(const UIDrawContext& context) final
      {
        const auto renderSizePx = TypeConverter::UncheckedTo<PxPoint2>(RenderExtentPx());

        if (renderSizePx.Y < 1)
        {
          return;
        }
        const int32_t barWidthPx = renderSizePx.X / int32_t(m_bars.size());
        const int32_t barBottomPx = renderSizePx.Y - 1;
        const int32_t finalBarWidthPx = barWidthPx * int32_t(m_bars.size());

        if (m_bar.NineSliceTexture.IsValid() && m_info.MaxEntriesInOneBar > 0u)
        {
          int32_t xPosPx = renderSizePx.X >= finalBarWidthPx ? renderSizePx.X - finalBarWidthPx : 0;
          PxAreaRectangleF cursorDstRectPxf(context.TargetRect.Left(), context.TargetRect.Top(), float(barWidthPx), context.TargetRect.Height());
          for (uint32_t i = 0; i < m_bars.size(); ++i)
          {
            assert(m_info.MaxEntriesInOneBar > 0u);
            int32_t ySizePx = MathHelper::Clamp(
              int32_t(std::round((float(renderSizePx.Y) / float(m_info.MaxEntriesInOneBar)) * float(m_bars[i].Entries))), 0, renderSizePx.Y);

            cursorDstRectPxf.MoveLeft(context.TargetRect.Left() + float(xPosPx));
            cursorDstRectPxf.SetTopBottom(context.TargetRect.Top() + float(barBottomPx - ySizePx), context.TargetRect.Top() + float(barBottomPx));

            Draw9SliceUtil::WinDraw(m_windowContext->Batch2D, cursorDstRectPxf, m_bar.NineSliceTexture.Texture, m_bar.NineSliceTexture.NSlice,
                                    ThicknessF(), m_bar.PrimaryColor);

            xPosPx += barWidthPx;
          }
        }
      }

    protected:
      // PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) final
      //{

      //}

      PxSize2D MeasureOverride(const PxAvailableSize& /*availableSizePx*/) final
      {
        auto minBarExtent = m_bar.NineSliceTexture.Texture.GetExtent();
        auto width = m_bars.size() * minBarExtent.Width;
        return {UncheckedNumericCast<int32_t>(width), UncheckedNumericCast<int32_t>(minBarExtent.Height)};
      }

    private:
      void BuildHistogram(const value_type min, const value_type max, const uint16_t cappedBarCount)
      {
        m_info = HistogramInfo(min, max, 0u);
        if (cappedBarCount != m_bars.size())
        {
          m_bars.resize(cappedBarCount);
        }

        // Clear the counters
        std::fill(m_bars.begin(), m_bars.end(), BarRecord());
        if (m_bars.size() > 0u)
        {
          // Then put each entry into its bucket
          const value_type interval = (m_info.Max - m_info.Min) / value_type(m_bars.size() - 1);
          for (const auto& entry : m_values)
          {
            auto bucket = static_cast<value_type>(std::round((entry - m_info.Min) / interval));
            const uint32_t bucketIndex = MathHelper::Clamp(uint32_t(bucket), 0u, uint32_t(m_bars.size()));
            ++m_bars[bucketIndex].Entries;
            if (m_bars[bucketIndex].Entries > m_info.MaxEntriesInOneBar)
            {
              m_info.MaxEntriesInOneBar = m_bars[bucketIndex].Entries;
            }
          }
        }
      }
    };
  }
}

#endif
#endif
