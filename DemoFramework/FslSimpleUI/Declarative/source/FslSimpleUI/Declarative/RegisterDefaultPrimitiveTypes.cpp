/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include "RegisterDefaultPrimitiveTypes.hpp"
#include <FslBase/Math/ConstrainedValue.hpp>
#include <FslBase/Math/Dp/DpSize1DF.hpp>
#include <FslBase/Math/Dp/DpSize2DF.hpp>
#include <FslBase/Math/Dp/DpThicknessF.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslBase/Time/TimeSpan.hpp>
#include <FslBase/Transition/TransitionType.hpp>
#include <FslSimpleUI/Base/Control/ScrollModeFlags.hpp>
#include <FslSimpleUI/Base/DpLayoutSize1D.hpp>
#include <FslSimpleUI/Base/ItemAlignment.hpp>
#include <FslSimpleUI/Base/ItemScalePolicy.hpp>
#include <FslSimpleUI/Base/Layout/LayoutOrientation.hpp>
#include <FslSimpleUI/Base/LayoutDirection.hpp>
#include <FslSimpleUI/Base/UIColor.hpp>
#include <FslSimpleUI/Declarative/PrimitiveTypeData.hpp>
#include <FslSimpleUI/Declarative/PrimitiveTypeRegistry.hpp>
#include <FslSimpleUI/Theme/Base/BarType.hpp>
#include <FslSimpleUI/Theme/Base/FloatingButtonType.hpp>
#include <FslSimpleUI/Theme/Base/WindowType.hpp>

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOCAL_EXTRACT(tYPEnAME) PrimitiveTypeInfo(std::string(#tYPEnAME), typeid(tYPEnAME))
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOCAL_ENUMVALUE(eNUMmEMBERnAME) ThemePropertyValueRecord(PropertyValue(#eNUMmEMBERnAME), static_cast<uint32_t>(eNUMmEMBERnAME))

namespace Fsl::UI::Declarative::Internal
{
  void RegisterDefaultPrimitiveTypes(PrimitiveTypeRegistry& rRegistry)
  {
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(bool)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(int8_t)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(uint8_t)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(int16_t)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(uint16_t)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(int32_t)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(uint32_t)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(int64_t)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(uint64_t)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(float)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(double)));

    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::ConstrainedValue<int8_t>)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::ConstrainedValue<uint8_t>)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::ConstrainedValue<int16_t>)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::ConstrainedValue<uint16_t>)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::ConstrainedValue<int32_t>)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::ConstrainedValue<uint32_t>)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::ConstrainedValue<int64_t>)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::ConstrainedValue<uint64_t>)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::ConstrainedValue<float>)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::ConstrainedValue<double>)));

    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::DpSize1DF)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::DpSize2DF)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::DpThicknessF)));

    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::StringViewLite)));

    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::TimeSpan)));

    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::UI::DpLayoutSize1D)));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::UI::UIColor)));

    // Enums
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::TransitionType),
                                         {LOCAL_ENUMVALUE(TransitionType::Linear),         LOCAL_ENUMVALUE(TransitionType::EaseInSine),
                                          LOCAL_ENUMVALUE(TransitionType::EaseOutSine),    LOCAL_ENUMVALUE(TransitionType::EaseInOutSine),
                                          LOCAL_ENUMVALUE(TransitionType::EaseInQuad),     LOCAL_ENUMVALUE(TransitionType::EaseOutQuad),
                                          LOCAL_ENUMVALUE(TransitionType::EaseInOutQuad),  LOCAL_ENUMVALUE(TransitionType::EaseInCubic),
                                          LOCAL_ENUMVALUE(TransitionType::EaseOutCubic),   LOCAL_ENUMVALUE(TransitionType::EaseInOutCubic),
                                          LOCAL_ENUMVALUE(TransitionType::EaseInQuart),    LOCAL_ENUMVALUE(TransitionType::EaseOutQuart),
                                          LOCAL_ENUMVALUE(TransitionType::EaseInOutQuart), LOCAL_ENUMVALUE(TransitionType::EaseInQuint),
                                          LOCAL_ENUMVALUE(TransitionType::EaseOutQuint),   LOCAL_ENUMVALUE(TransitionType::EaseInOutQuint),
                                          LOCAL_ENUMVALUE(TransitionType::EaseInExpo),     LOCAL_ENUMVALUE(TransitionType::EaseOutExpo),
                                          LOCAL_ENUMVALUE(TransitionType::EaseInOutExpo),  LOCAL_ENUMVALUE(TransitionType::EaseInCirc),
                                          LOCAL_ENUMVALUE(TransitionType::EaseOutCirc),    LOCAL_ENUMVALUE(TransitionType::EaseInOutCirc),
                                          LOCAL_ENUMVALUE(TransitionType::EaseInBack),     LOCAL_ENUMVALUE(TransitionType::EaseOutBack),
                                          LOCAL_ENUMVALUE(TransitionType::EaseInOutBack),  LOCAL_ENUMVALUE(TransitionType::EaseInElastic),
                                          LOCAL_ENUMVALUE(TransitionType::EaseOutElastic), LOCAL_ENUMVALUE(TransitionType::EaseInOutElastic),
                                          LOCAL_ENUMVALUE(TransitionType::EaseInBounce),   LOCAL_ENUMVALUE(TransitionType::EaseOutBounce),
                                          LOCAL_ENUMVALUE(TransitionType::EaseInOutBounce)}));

    rRegistry.Register(
      PrimitiveTypeData(LOCAL_EXTRACT(Fsl::UI::ItemAlignment), {LOCAL_ENUMVALUE(ItemAlignment::Near), LOCAL_ENUMVALUE(ItemAlignment::Center),
                                                                LOCAL_ENUMVALUE(ItemAlignment::Far), LOCAL_ENUMVALUE(ItemAlignment::Stretch)}));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::UI::ItemScalePolicy),
                                         {LOCAL_ENUMVALUE(ItemScalePolicy::NoScaling), LOCAL_ENUMVALUE(ItemScalePolicy::Fit),
                                          LOCAL_ENUMVALUE(ItemScalePolicy::FitKeepAR), LOCAL_ENUMVALUE(ItemScalePolicy::Downscale),
                                          LOCAL_ENUMVALUE(ItemScalePolicy::DownscaleKeepAR)}));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::UI::LayoutDirection),
                                         {LOCAL_ENUMVALUE(LayoutDirection::NearToFar), LOCAL_ENUMVALUE(LayoutDirection::FarToNear)}));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::UI::LayoutOrientation),
                                         {LOCAL_ENUMVALUE(LayoutOrientation::Vertical), LOCAL_ENUMVALUE(LayoutOrientation::Horizontal)}));
    rRegistry.Register(PrimitiveTypeData(
      LOCAL_EXTRACT(Fsl::UI::ScrollModeFlags),
      {LOCAL_ENUMVALUE(ScrollModeFlags::TranslateX), LOCAL_ENUMVALUE(ScrollModeFlags::TranslateY), LOCAL_ENUMVALUE(ScrollModeFlags::Translate)}));

    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::UI::Theme::BarType),
                                         {LOCAL_ENUMVALUE(Theme::BarType::Normal), LOCAL_ENUMVALUE(Theme::BarType::Transparent)}));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::UI::Theme::FloatingButtonType), {LOCAL_ENUMVALUE(Theme::FloatingButtonType::Back)}));
    rRegistry.Register(PrimitiveTypeData(LOCAL_EXTRACT(Fsl::UI::Theme::WindowType),
                                         {LOCAL_ENUMVALUE(Theme::WindowType::Normal), LOCAL_ENUMVALUE(Theme::WindowType::Transparent),
                                          LOCAL_ENUMVALUE(Theme::WindowType::DialogNormal), LOCAL_ENUMVALUE(Theme::WindowType::DialogTransparent)}));
  }
}
