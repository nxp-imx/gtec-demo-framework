#ifndef FSLSIMPLEUI_BASE_UNITTEST_CONTROL_FSLSIMPLEUI_BASE_UNITTEST_UTIMAGECONTROL_HPP
#define FSLSIMPLEUI_BASE_UNITTEST_CONTROL_FSLSIMPLEUI_BASE_UNITTEST_UTIMAGECONTROL_HPP
/****************************************************************************************************************************************************
 * Copyright 2019, 2022 NXP
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

#include <FslSimpleUI/Base/ItemScalePolicy.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/UIScaleUtil.hpp>
#include <FslSimpleUI/Base/UnitTest/Control/UTControl.hpp>

namespace Fsl::UI
{
  class UTImageControl : public UTControl
  {
    PxSize2D m_testImageSizePx;
    ItemScalePolicy m_scalePolicy{ItemScalePolicy::NoScaling};

  public:
    explicit UTImageControl(const std::shared_ptr<BaseWindowContext>& context)
      : UTControl(context)
    {
    }

    UTImageControl(const std::shared_ptr<BaseWindowContext>& context, const PxSize2D& imageSizePx)
      : UTControl(context)
      , m_testImageSizePx(imageSizePx)
    {
    }


    PxSize2D GetImageSize() const
    {
      return m_testImageSizePx;
    }


    void SetImageSize(const PxSize2D& sizePx)
    {
      if (sizePx != m_testImageSizePx)
      {
        m_testImageSizePx = sizePx;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    ItemScalePolicy GetScalePolicy() const
    {
      return m_scalePolicy;
    }


    void SetScalePolicy(const ItemScalePolicy value)
    {
      if (value != m_scalePolicy)
      {
        m_scalePolicy = value;
        PropertyUpdated(PropertyType::ScalePolicy);
      }
    }


    PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) override
    {
      UTControl::MeasureOverride(availableSizePx);
      return m_testImageSizePx;
    }


    PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) override
    {
      UTControl::ArrangeOverride(finalSizePx);
      PxSize2D calcedSizePx;
      if (!UIScaleUtil::TryCalcSize(calcedSizePx, finalSizePx, m_testImageSizePx, m_scalePolicy))
      {
        return finalSizePx;
      }
      return calcedSizePx;
    }
  };
}

#endif
