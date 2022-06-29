#ifndef FSLSIMPLEUI_BASE_LAYOUT_UNIFORMWRAPLAYOUT_HPP
#define FSLSIMPLEUI_BASE_LAYOUT_UNIFORMWRAPLAYOUT_HPP
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

#include <FslBase/Math/Dp/DpPoint2F.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxValue.hpp>
#include <FslSimpleUI/Base/Layout/LayoutOrientation.hpp>
#include <FslSimpleUI/Base/Layout/SimpleLayout.hpp>

namespace Fsl::UI
{
  class UniformWrapLayout : public SimpleLayout
  {
    struct MeasureResult
    {
      PxSize2D MeasuredSizePx;
      PxSize2D ElementUniformSizePx;

      MeasureResult() = default;
      MeasureResult(const PxSize2D& measuredSizePx, const PxSize2D& elementUniformSizePx)
        : MeasuredSizePx(measuredSizePx)
        , ElementUniformSizePx(elementUniformSizePx)
      {
      }
    };

    LayoutOrientation m_orientation;
    DpPoint2F m_spacingDp;
    PxSize2D m_elementUniformSizePx;

  public:
    explicit UniformWrapLayout(const std::shared_ptr<BaseWindowContext>& context);

    LayoutOrientation GetLayoutOrientation() const
    {
      return m_orientation;
    }

    void SetOrientation(const LayoutOrientation& value);

    DpPoint2F GetSpacing() const
    {
      return m_spacingDp;
    }

    bool SetSpacing(const DpPoint2F valueDp);

  protected:
    PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) override;
    PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) override;

    static MeasureResult MeasureHorizontalStackLayout(const collection_type::queue_type::const_iterator& itrBegin,
                                                      const collection_type::queue_type::const_iterator& itrEnd, const PxValue spacingXPx,
                                                      const PxAvailableSize& availableSizePx);
    static MeasureResult MeasureVerticalStackLayout(const collection_type::queue_type::const_iterator& itrBegin,
                                                    const collection_type::queue_type::const_iterator& itrEnd, const PxValue spacingYPx,
                                                    const PxAvailableSize& availableSizePx);
    static MeasureResult MeasureHorizontalWrapLayout(const collection_type::queue_type::const_iterator& itrBegin,
                                                     const collection_type::queue_type::const_iterator& itrEnd, const PxPoint2 spacingPx,
                                                     const PxAvailableSize& availableSizePx);
    static MeasureResult MeasureVerticalWrapLayout(const collection_type::queue_type::const_iterator& itrBegin,
                                                   const collection_type::queue_type::const_iterator& itrEnd, const PxPoint2 spacingPx,
                                                   const PxAvailableSize& availableSizePx);
  };
}

#endif
