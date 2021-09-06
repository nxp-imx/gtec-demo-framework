#ifndef FSLSIMPLEUI_THEME_BASIC_BASICTHEMERESOURCES_HPP
#define FSLSIMPLEUI_THEME_BASIC_BASICTHEMERESOURCES_HPP
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

#include <FslBase/Math/Dp/DpPoint.hpp>
#include <FslBase/Math/Dp/DpSize.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/Sprite/IContentSprite.hpp>
#include <FslGraphics/Sprite/ImageSprite.hpp>
#include <FslGraphics/Sprite/NineSliceSprite.hpp>
#include <FslSimpleUI/Theme/Base/IThemeResources.hpp>
#include <memory>
#include <utility>

namespace Fsl
{
  class BasicImageSprite;
  class BasicNineSliceSprite;
  class ISpriteResourceManager;
  class ImageSprite;
  class NineSliceSprite;
  struct SpriteMaterialId;
  class SpriteFont;

  namespace UI
  {
    namespace Theme
    {
      struct ThemeFactoryCreateInfo;

      class BasicThemeResources final : public IThemeResources
      {
      public:
        struct BasicSpriteRecord
        {
          const std::shared_ptr<BasicImageSprite> Opaque;
          const std::shared_ptr<BasicImageSprite> Transparent;

          BasicSpriteRecord(std::shared_ptr<BasicImageSprite> opaque, std::shared_ptr<BasicImageSprite> transparent)
            : Opaque(std::move(opaque))
            , Transparent(std::move(transparent))
          {
          }
        };

        const std::shared_ptr<SpriteFont> DefaultFont;
        const std::shared_ptr<SpriteFont> DefaultHeaderFont;

        const BasicSpriteRecord FillSprite;
        const BasicSpriteRecord MiniFillSprite;
        const BasicSpriteRecord TestSprite;

        const std::shared_ptr<ImageSprite> OpaqueFillSprite;

        const std::shared_ptr<INineSliceSprite> DividerNineSliceSprite;

        const std::shared_ptr<INineSliceSprite> LeftBarNineSliceSprite;
        const std::shared_ptr<INineSliceSprite> TopBarNineSliceSprite;
        const std::shared_ptr<INineSliceSprite> RightBarNineSliceSprite;
        const std::shared_ptr<INineSliceSprite> BottomBarNineSliceSprite;

        // Dialog
        const std::shared_ptr<INineSliceSprite> DialogNineSliceSprite;
        const std::shared_ptr<INineSliceSprite> SmallDialogNineSliceSprite;

        // button
        const std::shared_ptr<INineSliceSprite> ContainedButtonNormalSprite;
        const std::shared_ptr<INineSliceSprite> ContainedButtonHoverSprite;
        const std::shared_ptr<INineSliceSprite> OutlineButtonNormalSprite;
        const std::shared_ptr<INineSliceSprite> OutlineButtonHoverSprite;

        // CheckBox
        const std::shared_ptr<ImageSprite> TexCheckBoxHover;
        const std::shared_ptr<ImageSprite> TexCheckBoxChecked;
        const std::shared_ptr<ImageSprite> TexCheckBoxUnchecked;

        // floating button
        const std::shared_ptr<ImageSprite> FloatingButtonPressedSprite;
        const std::shared_ptr<ImageSprite> FloatingButtonNormalSprite;
        const std::shared_ptr<ImageSprite> FloatingButtonHoverSprite;
        const std::shared_ptr<ImageSprite> FloatingSmallButtonPressedSprite;
        const std::shared_ptr<ImageSprite> FloatingSmallButtonNormalSprite;
        const std::shared_ptr<ImageSprite> FloatingSmallButtonHoverSprite;

        // RadioButton
        const std::shared_ptr<ImageSprite> RadioButtonHoverSprite;
        const std::shared_ptr<ImageSprite> RadioButtonCheckedSprite;
        const std::shared_ptr<ImageSprite> RadioButtonUncheckedSprite;
        // Switch
        const std::shared_ptr<ImageSprite> SwitchHoverSprite;
        const std::shared_ptr<ImageSprite> SwitchCheckedSprite;
        const std::shared_ptr<ImageSprite> SwitchUncheckedSprite;
        // slider
        const std::shared_ptr<ImageSprite> SliderCursorSprite;
        const std::shared_ptr<ImageSprite> SliderCursorHoverSprite;
        const std::shared_ptr<INineSliceSprite> SliderBGSprite;
        // tooltip
        const std::shared_ptr<NineSliceSprite> ToolTipNineSliceSprite;
        // ColorMarker
        const std::shared_ptr<INineSliceSprite> ColorMarkerNineSliceSprite;

        // Icons - Media
        const std::shared_ptr<ImageSprite> IconMediaFastForward;
        const std::shared_ptr<ImageSprite> IconMediaFastRewind;
        const std::shared_ptr<ImageSprite> IconMediaNext;
        const std::shared_ptr<ImageSprite> IconMediaPause;
        const std::shared_ptr<ImageSprite> IconMediaPlay;
        const std::shared_ptr<ImageSprite> IconMediaPrevious;
        const std::shared_ptr<ImageSprite> IconMediaRecord;
        const std::shared_ptr<ImageSprite> IconMediaReplay;
        const std::shared_ptr<ImageSprite> IconMediaStop;

        // Icons - Navigation
        const std::shared_ptr<ImageSprite> IconNavigationArrowBack;
        const std::shared_ptr<ImageSprite> IconNavigationArrowDownward;
        const std::shared_ptr<ImageSprite> IconNavigationArrowDropDown;
        const std::shared_ptr<ImageSprite> IconNavigationArrowDropUp;
        const std::shared_ptr<ImageSprite> IconNavigationArrowForward;
        const std::shared_ptr<ImageSprite> IconNavigationArrowUpward;
        const std::shared_ptr<ImageSprite> IconNavigationCancel;


        // Consts
        const DpSize SliderCursorSizeDp;
        const DpPoint SliderCursorOriginDp;


        BasicThemeResources(ISpriteResourceManager& rResourceManager, const ThemeFactoryCreateInfo& createInfo);
        ~BasicThemeResources() override;

        // IThemeResources
        const std::shared_ptr<INineSliceSprite>& GetBackground(const WindowType windowType) const override
        {
          switch (windowType)
          {
          case WindowType::DialogNormal:
          case WindowType::DialogTransparent:
            return DialogNineSliceSprite;
          default:
            return SmallDialogNineSliceSprite;
          }
        }


        const std::shared_ptr<BasicImageSprite>& GetBasicFillSprite(const bool opaque) const final
        {
          return opaque ? FillSprite.Opaque : FillSprite.Transparent;
        }

        const std::shared_ptr<BasicImageSprite>& GetBasicMiniFillSprite(const bool opaque) const final
        {
          return opaque ? MiniFillSprite.Opaque : MiniFillSprite.Transparent;
        }

        const std::shared_ptr<BasicImageSprite>& GetBasicTestSprite(const bool opaque = true) const final
        {
          return opaque ? TestSprite.Opaque : TestSprite.Transparent;
        }


        const std::shared_ptr<ImageSprite>& GetFillSprite() const final
        {
          return OpaqueFillSprite;
        }

        const std::shared_ptr<SpriteFont>& GetDefaultSpriteFont() const final
        {
          return DefaultFont;
        }

        const std::shared_ptr<NineSliceSprite>& GetToolTipNineSliceSprite() const final
        {
          return ToolTipNineSliceSprite;
        }

        const std::shared_ptr<INineSliceSprite>& GetDialogNineSliceSprite() const final
        {
          return DialogNineSliceSprite;
        }

        const std::shared_ptr<INineSliceSprite>& GetDividerNineSliceSprite() const final
        {
          return DividerNineSliceSprite;
        }

        const std::shared_ptr<INineSliceSprite>& GetColorMarkerNineSliceSprite() const final
        {
          return ColorMarkerNineSliceSprite;
        }

        const std::shared_ptr<ImageSprite>& GetIconSprite(const IconType iconType) const final;
      };
    }
  }
}

#endif
