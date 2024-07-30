#ifndef FSLSIMPLEUI_BASE_CONTROL_IMPL_SLIDERRENDERIMPL_HPP
#define FSLSIMPLEUI_BASE_CONTROL_IMPL_SLIDERRENDERIMPL_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022, 2024 NXP
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

#include <FslBase/Math/Dp/DpPoint2.hpp>
#include <FslBase/Math/Dp/DpSize2D.hpp>
#include <FslBase/Math/Dp/DpThickness.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/Pixel/PxVector2.hpp>
#include <FslSimpleUI/Base/Control/Logic/SliderPixelSpanInfo.hpp>
#include <FslSimpleUI/Base/DefaultValues.hpp>
#include <FslSimpleUI/Base/Layout/LayoutOrientation.hpp>
#include <FslSimpleUI/Base/LayoutDirection.hpp>
#include <FslSimpleUI/Base/Mesh/ContentSpriteMesh.hpp>
#include <FslSimpleUI/Base/Mesh/SizedSpriteMesh.hpp>
#include <FslSimpleUI/Base/Transition/TransitionUIRenderColor.hpp>
#include <FslSimpleUI/Base/UIColor.hpp>
#include <FslSimpleUI/Base/UIColorConverter.hpp>
#include <memory>

namespace Fsl
{
  class SpriteUnitConverter;

  namespace UI
  {
    class DrawCommandBuffer;
    struct PxAvailableSize;
    class WindowMouseOverEvent;

    class SliderRenderImpl
    {
      struct BackgroundGraphicsRecord
      {
        ContentSpriteMesh Sprite;
        UIColor EnabledColor;
        UIColor DisabledColor;
        UIRenderColor EnabledRenderColor;
        UIRenderColor DisabledRenderColor;

        TransitionUIRenderColor CurrentColor;

        explicit BackgroundGraphicsRecord(UIColorConverter colorConverter, const std::shared_ptr<IMeshManager>& meshManager,
                                          const UIColor enabledColor, const UIColor disabledColor, const TimeSpan time, const TransitionType type)
          : Sprite(meshManager)
          , EnabledColor(enabledColor)
          , DisabledColor(disabledColor)
          , EnabledRenderColor(colorConverter.Convert(enabledColor))
          , DisabledRenderColor(colorConverter.Convert(disabledColor))
          , CurrentColor(time, type)
        {
        }
      };

      struct CursorGraphicsRecord
      {
        SizedSpriteMesh Sprite;
        DpPoint2 OriginDp;
        DpSize2D SizeDp;
        UIColor EnabledColor;
        UIColor DisabledColor;
        UIRenderColor EnabledRenderColor;
        UIRenderColor DisabledRenderColor;
        TransitionUIRenderColor CurrentColor;

        explicit CursorGraphicsRecord(UIColorConverter colorConverter, const std::shared_ptr<IMeshManager>& meshManager, const UIColor enabledColor,
                                      const UIColor disabledColor, const TimeSpan time, const TransitionType type)
          : Sprite(meshManager)
          , EnabledColor(enabledColor)
          , DisabledColor(disabledColor)
          , EnabledRenderColor(colorConverter.Convert(enabledColor))
          , DisabledRenderColor(colorConverter.Convert(disabledColor))
          , CurrentColor(time, type)
        {
        }
      };

      struct OverlayGraphicsRecord
      {
        SizedSpriteMesh Sprite;
        UIColor EnabledColor;
        UIRenderColor EnabledRenderColor;
        TransitionUIRenderColor CurrentColor;

        explicit OverlayGraphicsRecord(UIColorConverter colorConverter, const std::shared_ptr<IMeshManager>& meshManager, const UIColor enabledColor,
                                       const TimeSpan time, const TransitionType type)
          : Sprite(meshManager)
          , EnabledColor(enabledColor)
          , EnabledRenderColor(colorConverter.Convert(enabledColor))
          , CurrentColor(time, type)
        {
        }
      };

      struct ArrangeCache
      {
        PxSize2D RenderSizePx;
        LayoutOrientation Orientation{LayoutOrientation::Vertical};
        LayoutDirection Direction{LayoutDirection::NearToFar};
        SliderPixelSpanInfo SpanInfo;

        constexpr ArrangeCache() noexcept = default;
        constexpr ArrangeCache(const PxSize2D renderSizePx, const LayoutOrientation orientation, const LayoutDirection layoutDirection,
                               const SliderPixelSpanInfo& spanInfo) noexcept
          : RenderSizePx(renderSizePx)
          , Orientation(orientation)
          , Direction(layoutDirection)
          , SpanInfo(spanInfo)
        {
        }
      };

      UIColorConverter m_colorConverter;
      BackgroundGraphicsRecord m_background;
      CursorGraphicsRecord m_cursor;
      OverlayGraphicsRecord m_cursorOverlay;

      bool m_isHovering{false};
      bool m_verticalGraphicsRotationEnabled{false};

      ArrangeCache m_arrangeCache;

    public:
      explicit SliderRenderImpl(const UIColorConverter colorConverter, const std::shared_ptr<IMeshManager>& meshManager);

      bool GetEnableVerticalGraphicsRotation() const
      {
        return m_verticalGraphicsRotationEnabled;
      }

      bool SetEnableVerticalGraphicsRotation(const bool enabled)
      {
        const bool modified = enabled != m_verticalGraphicsRotationEnabled;
        if (modified)
        {
          m_verticalGraphicsRotationEnabled = enabled;
        }
        return modified;
      }

      const std::shared_ptr<ISizedSprite>& GetCursorSprite() const
      {
        return m_cursor.Sprite.GetSprite();
      }

      bool SetCursorSprite(const std::shared_ptr<ISizedSprite>& value);

      UIColor GetCursorColor() const
      {
        return m_cursor.EnabledColor;
      }

      bool SetCursorColor(const UIColor value)
      {
        const bool modified = value != m_cursor.EnabledColor;
        if (modified)
        {
          m_cursor.EnabledColor = value;
          m_cursor.EnabledRenderColor = m_colorConverter.Convert(value);
        }
        return modified;
      }

      UIColor GetCursorDisabledColor() const
      {
        return m_cursor.DisabledColor;
      }

      bool SetCursorDisabledColor(const UIColor value)
      {
        const bool modified = value != m_cursor.DisabledColor;
        if (modified)
        {
          m_cursor.DisabledColor = value;
          m_cursor.DisabledRenderColor = m_colorConverter.Convert(value);
        }
        return modified;
      }

      const DpPoint2& GetCursorOrigin() const
      {
        return m_cursor.OriginDp;
      }

      bool SetCursorOrigin(const DpPoint2& value)
      {
        const bool modified = value != m_cursor.OriginDp;
        if (modified)
        {
          m_cursor.OriginDp = value;
        }
        return modified;
      }

      DpSize2D GetCursorSize() const
      {
        return m_cursor.SizeDp;
      }

      bool SetCursorSize(const DpSize2D value)
      {
        const bool modified = value != m_cursor.SizeDp;
        if (modified)
        {
          m_cursor.SizeDp = value;
        }
        return modified;
      }

      // ------


      const std::shared_ptr<ISizedSprite>& GetCursorOverlaySprite() const
      {
        return m_cursorOverlay.Sprite.GetSprite();
      }

      bool SetCursorOverlaySprite(const std::shared_ptr<ISizedSprite>& value);

      UIColor GetCursorOverlayColor() const
      {
        return m_cursorOverlay.EnabledColor;
      }

      bool SetCursorOverlayColor(const UIColor value)
      {
        const bool modified = value != m_cursorOverlay.EnabledColor;
        if (modified)
        {
          m_cursorOverlay.EnabledColor = value;
          m_cursorOverlay.EnabledRenderColor = m_colorConverter.Convert(value);
        }
        return modified;
      }

      // ------

      const std::shared_ptr<IContentSprite>& GetBackgroundSprite() const
      {
        return m_background.Sprite.GetSprite();
      }

      bool SetBackgroundSprite(const std::shared_ptr<IContentSprite>& value);

      UIColor GetBackgroundColor() const
      {
        return m_background.EnabledColor;
      }

      bool SetBackgroundColor(const UIColor value)
      {
        const bool modified = value != m_background.EnabledColor;
        if (modified)
        {
          m_background.EnabledColor = value;
          m_background.EnabledRenderColor = m_colorConverter.Convert(value);
        }
        return modified;
      }

      UIColor GetBackgroundDisabledColor() const
      {
        return m_background.DisabledColor;
      }

      bool SetBackgroundDisabledColor(const UIColor value)
      {
        const bool modified = value != m_background.DisabledColor;
        if (modified)
        {
          m_background.DisabledColor = value;
          m_background.DisabledRenderColor = m_colorConverter.Convert(value);
        }
        return modified;
      }

      // ------

      void Draw(DrawCommandBuffer& commandBuffer, const PxVector2 dstPositionPxf, const UIRenderColor finalColor, const PxValue cursorPositionPx,
                const bool isDragging, const SpriteUnitConverter& spriteUnitConverter);

      void OnMouseOver(const std::shared_ptr<WindowMouseOverEvent>& theEvent, const bool isEnabled);

      PxSize2D Measure(const PxAvailableSize& availableSizePx);
      SliderPixelSpanInfo Arrange(const PxSize2D finalSizePx, const LayoutOrientation orientation, const LayoutDirection layoutDirection,
                                  const SpriteUnitConverter& spriteUnitConverter);

      void UpdateAnimation(const TimeSpan& timeSpan);

      bool UpdateAnimationState(const bool forceCompleteAnimation, const bool isEnabled, const bool isDragging);
    };
  }
}

#endif
