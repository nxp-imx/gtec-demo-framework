#ifndef FSLSIMPLEUI_BASE_LAYOUT_WRAPLAYOUT_HPP
#define FSLSIMPLEUI_BASE_LAYOUT_WRAPLAYOUT_HPP
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

#include <FslBase/Math/Dp/DpPoint2F.hpp>
#include <FslSimpleUI/Base/Layout/ComplexLayout.hpp>
#include <FslSimpleUI/Base/Layout/LayoutOrientation.hpp>

namespace Fsl::UI
{
  struct WrapLayoutWindowRecord : GenericWindowCollectionRecordBase
  {
    PxPoint2 PositionPx;

    explicit WrapLayoutWindowRecord(const std::shared_ptr<BaseWindow>& window)
      : GenericWindowCollectionRecordBase(window)
    {
    }
  };

  class WrapLayout : public ComplexLayout<WrapLayoutWindowRecord>
  {
    LayoutOrientation m_orientation;
    DpPoint2F m_spacingDp;

  public:
    explicit WrapLayout(const std::shared_ptr<BaseWindowContext>& context);

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

    static PxSize2D MeasureHorizontalStackLayout(const collection_type::queue_type::iterator& itrBegin,
                                                 const collection_type::queue_type::iterator& itrEnd, const PxValue spacingXPx,
                                                 const PxAvailableSize& availableSizePx);
    static PxSize2D MeasureVerticalStackLayout(const collection_type::queue_type::iterator& itrBegin,
                                               const collection_type::queue_type::iterator& itrEnd, const PxValue spacingYPx,
                                               const PxAvailableSize& availableSizePx);
    static PxSize2D MeasureHorizontalWrapLayout(const collection_type::queue_type::iterator& itrBegin,
                                                const collection_type::queue_type::iterator& itrEnd, const PxPoint2& spacingPx,
                                                const PxAvailableSize& availableSizePx);
    static PxSize2D MeasureVerticalWrapLayout(const collection_type::queue_type::iterator& itrBegin,
                                              const collection_type::queue_type::iterator& itrEnd, const PxPoint2& spacingPx,
                                              const PxAvailableSize& availableSizePx);
  };
}

#endif
