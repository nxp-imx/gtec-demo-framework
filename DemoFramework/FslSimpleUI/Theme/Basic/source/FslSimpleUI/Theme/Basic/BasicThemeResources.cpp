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

#include <FslBase/IO/PathView.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Dp/DpPoint2.hpp>
#include <FslGraphics/Sprite/BasicImageSprite.hpp>
#include <FslGraphics/Sprite/BasicNineSliceSprite.hpp>
#include <FslGraphics/Sprite/ISpriteResourceManager.hpp>
#include <FslGraphics/Sprite/ImageSprite.hpp>
#include <FslGraphics/Sprite/NineSliceSprite.hpp>
#include <FslGraphics/Sprite/OptimizedNineSliceSprite.hpp>
#include <FslSimpleUI/Base/WindowContext.hpp>
#include <FslSimpleUI/Theme/Base/ThemeFactoryCreateInfo.hpp>
#include <FslSimpleUI/Theme/Basic/BasicThemeResources.hpp>

namespace Fsl::UI::Theme
{
  namespace
  {
    namespace TextureName
    {
      constexpr const IO::PathView Fill("Fill");
      constexpr const IO::PathView MiniFill("Fill4x4");
      constexpr const IO::PathView Divider("Control/White/Divider");
      constexpr const IO::PathView Test("Misc/Logo");

      // Dialog
      constexpr const IO::PathView Dialog("Control/White/WindowBackground");

      // Bar
      constexpr const IO::PathView LeftBar("Control/White/BarLeft");
      constexpr const IO::PathView TopBar("Control/White/BarTop");
      constexpr const IO::PathView RightBar("Control/White/BarRight");
      constexpr const IO::PathView BottomBar("Control/White/BarBottom");

      // Button
      constexpr const IO::PathView ButtonHover("Control/White/ButtonH");
      constexpr const IO::PathView ButtonNormal("Control/White/ButtonN");

      constexpr const IO::PathView ButtonOutlineNormal("Control/White/ButtonOutlineN");
      constexpr const IO::PathView ButtonOutlineHover("Control/White/ButtonOutlineH");

      // CheckBox
      constexpr const IO::PathView CheckBoxCursorHover("Control/White/Switch_Hover");
      constexpr const IO::PathView CheckBoxCursor("Control/White/CheckBox_Cursor");
      constexpr const IO::PathView CheckBoxBackground("Control/White/CheckBox_BG");

      // FloatingButton
      constexpr const IO::PathView FloatingButtonPressed("Control/White/FloatingRoundButtonP");
      constexpr const IO::PathView FloatingButtonNormal("Control/White/FloatingRoundButtonN");
      constexpr const IO::PathView FloatingButtonHover("Control/White/FloatingRoundButtonH");
      constexpr const IO::PathView FloatingSmallButtonPressed("Control/White/FloatingSmallRoundButtonP");
      constexpr const IO::PathView FloatingSmallButtonNormal("Control/White/FloatingSmallRoundButtonN");
      constexpr const IO::PathView FloatingSmallButtonHover("Control/White/FloatingSmallRoundButtonH");

      // RadioButton
      constexpr const IO::PathView RadioButtonCursorHover("Control/White/Switch_Hover");
      constexpr const IO::PathView RadioButtonCursor("Control/White/RadioButton_Cursor");
      constexpr const IO::PathView RadioButtonBackground("Control/White/RadioButton_BG");

      // Switch
      constexpr const IO::PathView SwitchCursorHover("Control/White/Switch_Hover");
      constexpr const IO::PathView SwitchCursor("Control/White/Switch_Cursor");
      constexpr const IO::PathView SwitchBackground("Control/White/Switch_BG");

      // Slider
      constexpr const IO::PathView SliderBackground("Control/White/SliderBG");
      constexpr const IO::PathView SliderCursor("Control/White/SliderCursor");
      constexpr const IO::PathView SliderCursorHover("Control/White/SliderCursor_Hover");

      // Scrollbar
      constexpr const IO::PathView ScrollbarH("Control/White/ScrollbarH");
      constexpr const IO::PathView ScrollbarV("Control/White/ScrollbarV");

      // ToolTip
      constexpr const IO::PathView ToolTip("Control/White/ToolTip");

      // ColorMarker
      constexpr const IO::PathView ColorMarker("Control/White/ColorMarker");

      // Icons - Media
      constexpr const IO::PathView IconMediaFastForward("Icon/Av/ic_fast_forward_white_48dp");
      constexpr const IO::PathView IconMediaFastRewind("Icon/Av/ic_fast_rewind_white_48dp");
      constexpr const IO::PathView IconMediaNext("Icon/Av/ic_skip_next_white_48dp");
      constexpr const IO::PathView IconMediaPause("Icon/Av/ic_pause_white_48dp");
      constexpr const IO::PathView IconMediaPlay("Icon/Av/ic_play_arrow_white_48dp");
      constexpr const IO::PathView IconMediaPrevious("Icon/Av/ic_skip_previous_white_48dp");
      constexpr const IO::PathView IconMediaRecord("Icon/Av/ic_fiber_manual_record_48dp");
      constexpr const IO::PathView IconMediaReplay("Icon/Av/ic_replay_white_48dp");
      constexpr const IO::PathView IconMediaStop("Icon/Av/ic_stop_white_48dp");

      // Icons - Content
      constexpr const IO::PathView IconsContentAdd("Icon/Content/ic_add_white_48dp");
      constexpr const IO::PathView IconsContentRemove("Icon/Content/ic_remove_white_48dp");

      // Icons - Navigation
      constexpr const IO::PathView IconNavigationArrowBack("Icon/Navigation/ic_arrow_back_48dp");
      constexpr const IO::PathView IconNavigationArrowDownward("Icon/Navigation/ic_arrow_downward_48dp");
      constexpr const IO::PathView IconNavigationArrowDropDown("Icon/Navigation/ic_arrow_drop_down_white_48dp");
      constexpr const IO::PathView IconNavigationArrowDropUp("Icon/Navigation/ic_arrow_drop_up_white_48dp");
      constexpr const IO::PathView IconNavigationArrowForward("Icon/Navigation/ic_arrow_forward_48dp");
      constexpr const IO::PathView IconNavigationArrowUpward("Icon/Navigation/ic_arrow_upward_48dp");
      constexpr const IO::PathView IconNavigationCancel("Icon/Navigation/ic_cancel_48dp");
    }

    //! Create a nine-slice of a type that is compatible with the current settings
    std::shared_ptr<INineSliceSprite> CreateNineSlice(ISpriteResourceManager& rResourceManager, const ThemeFactoryCreateInfo& createInfo,
                                                      const IO::PathView& path)
    {
      if (!createInfo.DisableOpaqueMaterials)
      {
        return rResourceManager.CreateOptimizedNineSliceSprite(createInfo.DefaultOpaqueMaterialId, createInfo.DefaultTransparentMaterialId, path);
      }
      return rResourceManager.CreateNineSliceSprite(createInfo.DefaultTransparentMaterialId, path);
    }
  }

  BasicThemeResources::BasicThemeResources(ISpriteResourceManager& rResourceManager, const ThemeFactoryCreateInfo& createInfo)
    : DefaultFont(createInfo.DefaultFont)
    , DefaultHeaderFont(createInfo.DefaultHeaderFont)
    , FillSprite(rResourceManager.CreateBasicImageSprite(createInfo.DefaultOpaqueMaterialId, TextureName::Fill),
                 rResourceManager.CreateBasicImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::Fill))
    , MiniFillSprite(rResourceManager.CreateBasicImageSprite(createInfo.DefaultOpaqueMaterialId, TextureName::MiniFill),
                     rResourceManager.CreateBasicImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::MiniFill))
    , LineListSprite(rResourceManager.CreateBasicImageSprite(createInfo.DefaultLineListOpaqueMaterialId, TextureName::Fill),
                     rResourceManager.CreateBasicImageSprite(createInfo.DefaultLineListTransparentMaterialId, TextureName::Fill))
    , TestSprite(rResourceManager.CreateBasicImageSprite(createInfo.DefaultOpaqueMaterialId, TextureName::Test),
                 rResourceManager.CreateBasicImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::Test))
    , OpaqueFillSprite(rResourceManager.CreateImageSprite(createInfo.DefaultOpaqueMaterialId, TextureName::Fill))
    , TransparentFillSprite(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::Fill))
    , DividerNineSliceSprite(rResourceManager.CreateNineSliceSprite(createInfo.DefaultTransparentMaterialId, TextureName::Divider))
    // bar
    , LeftBarNineSliceSprite(CreateNineSlice(rResourceManager, createInfo, TextureName::LeftBar))
    , TopBarNineSliceSprite(CreateNineSlice(rResourceManager, createInfo, TextureName::TopBar))
    , RightBarNineSliceSprite(CreateNineSlice(rResourceManager, createInfo, TextureName::RightBar))
    , BottomBarNineSliceSprite(CreateNineSlice(rResourceManager, createInfo, TextureName::BottomBar))
    // Dialog
    , DialogNineSliceSprite(CreateNineSlice(rResourceManager, createInfo, TextureName::Dialog))
    , SmallDialogNineSliceSprite(CreateNineSlice(rResourceManager, createInfo, TextureName::ToolTip))
    // button
    , ContainedButtonNormalSprite(CreateNineSlice(rResourceManager, createInfo, TextureName::ButtonNormal))
    , ContainedButtonHoverSprite(CreateNineSlice(rResourceManager, createInfo, TextureName::ButtonHover))
    , OutlineButtonNormalSprite(CreateNineSlice(rResourceManager, createInfo, TextureName::ButtonOutlineNormal))
    , OutlineButtonHoverSprite(CreateNineSlice(rResourceManager, createInfo, TextureName::ButtonOutlineHover))
    // checkbox
    , TexCheckBoxHover(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::CheckBoxCursorHover))
    , TexCheckBoxChecked(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::CheckBoxCursor))
    , TexCheckBoxUnchecked(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::CheckBoxBackground))
    // Floating button
    , FloatingButtonPressedSprite(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::FloatingButtonPressed))
    , FloatingButtonNormalSprite(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::FloatingButtonNormal))
    , FloatingButtonHoverSprite(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::FloatingButtonHover))
    // Small button
    , FloatingSmallButtonPressedSprite(
        rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::FloatingSmallButtonPressed))
    , FloatingSmallButtonNormalSprite(
        rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::FloatingSmallButtonNormal))
    , FloatingSmallButtonHoverSprite(
        rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::FloatingSmallButtonHover))
    // radio button
    , RadioButtonHoverSprite(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::RadioButtonCursorHover))
    , RadioButtonCheckedSprite(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::RadioButtonCursor))
    , RadioButtonUncheckedSprite(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::RadioButtonBackground))
    // switch
    , SwitchHoverSprite(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::SwitchCursorHover))
    , SwitchCheckedSprite(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::SwitchCursor))
    , SwitchUncheckedSprite(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::SwitchBackground))
    // slider
    , SliderCursorSprite(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::SliderCursor))
    , SliderCursorHoverSprite(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::SliderCursorHover))
    , SliderBGSprite(CreateNineSlice(rResourceManager, createInfo, TextureName::SliderBackground))
    // scrollbar
    , TexScrollbarH(rResourceManager.CreateOptimizedNineSliceSprite(createInfo.DefaultOpaqueMaterialId, createInfo.DefaultTransparentMaterialId,
                                                                    TextureName::ScrollbarH))
    , TexScrollbarV(rResourceManager.CreateOptimizedNineSliceSprite(createInfo.DefaultOpaqueMaterialId, createInfo.DefaultTransparentMaterialId,
                                                                    TextureName::ScrollbarV))
    // ToolTip
    , ToolTipNineSliceSprite(rResourceManager.CreateNineSliceSprite(createInfo.DefaultTransparentMaterialId, TextureName::ToolTip))
    // ColorMarker
    , ColorMarkerNineSliceSprite(rResourceManager.CreateNineSliceSprite(createInfo.DefaultTransparentMaterialId, TextureName::ColorMarker))
    // Icons - Media
    , IconMediaFastForward(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::IconMediaFastForward))
    , IconMediaFastRewind(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::IconMediaFastRewind))
    , IconMediaNext(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::IconMediaNext))
    , IconMediaPause(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::IconMediaPause))
    , IconMediaPlay(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::IconMediaPlay))
    , IconMediaPrevious(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::IconMediaPrevious))
    , IconMediaRecord(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::IconMediaRecord))
    , IconMediaReplay(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::IconMediaReplay))
    , IconMediaStop(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::IconMediaStop))
    // Icons - Content
    , IconContentAdd(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::IconsContentAdd))
    , IconContentRemove(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::IconsContentRemove))
    // Icons - Navigation
    , IconNavigationArrowBack(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::IconNavigationArrowBack))
    , IconNavigationArrowDownward(
        rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::IconNavigationArrowDownward))
    , IconNavigationArrowDropDown(
        rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::IconNavigationArrowDropDown))
    , IconNavigationArrowDropUp(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::IconNavigationArrowDropUp))
    , IconNavigationArrowForward(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::IconNavigationArrowForward))
    , IconNavigationArrowUpward(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::IconNavigationArrowUpward))
    , IconNavigationCancel(rResourceManager.CreateImageSprite(createInfo.DefaultTransparentMaterialId, TextureName::IconNavigationCancel))
    // Consts
    , SliderCursorSizeDp(DpSize2D::Create(12, 12))
    , SliderCursorOriginDp(DpPoint2::Create(24, 24))
  {
  }

  BasicThemeResources::~BasicThemeResources() = default;


  const std::shared_ptr<ImageSprite>& BasicThemeResources::GetIconSprite(const IconType iconType) const
  {
    switch (iconType)
    {
    case IconType::Fill:
      return TransparentFillSprite;
    case IconType::MediaFastForward:
      return IconMediaFastForward;
    case IconType::MediaFastRewind:
      return IconMediaFastRewind;
    case IconType::MediaNext:
      return IconMediaNext;
    case IconType::MediaPause:
      return IconMediaPause;
    case IconType::MediaPlay:
      return IconMediaPlay;
    case IconType::MediaPrevious:
      return IconMediaPrevious;
    case IconType::MediaRecord:
      return IconMediaRecord;
    case IconType::MediaReplay:
      return IconMediaReplay;
    case IconType::MediaStop:
      return IconMediaStop;
    case IconType::ContentAdd:
      return IconContentAdd;
    case IconType::ContentRemove:
      return IconContentRemove;
    case IconType::NavigationArrowBack:
      return IconNavigationArrowBack;
    case IconType::NavigationArrowDownward:
      return IconNavigationArrowDownward;
    case IconType::NavigationArrowDropDown:
      return IconNavigationArrowDropDown;
    case IconType::NavigationArrowDropUp:
      return IconNavigationArrowDropUp;
    case IconType::NavigationArrowForward:
      return IconNavigationArrowForward;
    case IconType::NavigationArrowUpward:
      return IconNavigationArrowUpward;
    case IconType::NavigationCancel:
      return IconNavigationCancel;
    default:
      throw NotSupportedException("IconType is not supported");
    }
  }
}
