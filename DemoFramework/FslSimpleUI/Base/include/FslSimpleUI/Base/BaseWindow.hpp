#ifndef FSLSIMPLEUI_BASE_BASEWINDOW_HPP
#define FSLSIMPLEUI_BASE_BASEWINDOW_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslBase/ITag.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Dp/DpThicknessF.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslSimpleUI/Base/BaseWindowFlags.hpp>
#include <FslSimpleUI/Base/DefaultValues.hpp>
#include <FslSimpleUI/Base/DpLayoutSize1D.hpp>
#include <FslSimpleUI/Base/DpLayoutSize2D.hpp>
#include <FslSimpleUI/Base/ItemAlignment.hpp>
#include <FslSimpleUI/Base/IWindowId.hpp>
#include <FslSimpleUI/Base/LayoutCache.hpp>
#include <FslSimpleUI/Base/PxAvailableSize.hpp>
#include <FslSimpleUI/Base/System/IEventListener.hpp>
#include <memory>
#include <stdexcept>

namespace Fsl
{
  struct DemoTime;
  struct TransitionTimeSpan;

  namespace UI
  {
    class BaseWindowContext;
    struct PropertyTypeFlags;
    struct ResolutionChangedInfo;
    struct RoutedEvent;
    struct RoutedEventArgs;
    struct UIDrawContext;
    class WindowEvent;
    class WindowEventPool;

    class BaseWindow
      : public IWindowId
      , protected IEventListener
    {
      const std::shared_ptr<BaseWindowContext> m_context;
      std::shared_ptr<ITag> m_tag;
      DpLayoutSize2D m_sizeDp;
      DpThicknessF m_marginDp;
      ItemAlignment m_alignmentX;
      ItemAlignment m_alignmentY;
      BaseWindowFlags m_flags;
      LayoutCache m_layoutCache;

    public:
      BaseWindow(const BaseWindow&) = delete;
      BaseWindow& operator=(const BaseWindow&) = delete;

      explicit BaseWindow(const std::shared_ptr<BaseWindowContext>& context);
      ~BaseWindow() override;

      //! @brief Called on windows that registered for it once they become known to the window manager.
      virtual void WinInit()
      {
      }

      //! @brief Called by the engine to inform the window that the resolution has been modified and give it a chance to recache things as needed.
      //! @note this will also mark the layout as dirty
      //! @warning This method is not allowed to throw a exception!!!!!
      virtual void WinResolutionChanged(const ResolutionChangedInfo& info);

      //! @brief Called by the engine to mark the layout of the window as dirty.
      //! @warning This method is not allowed to throw a exception!!!!!
      //! @return true if the window was marked as dirty, false if it was already dirty.
      virtual bool WinMarkLayoutAsDirty();

      virtual WindowFlags WinGetFlags() const
      {
        return WindowFlags(m_flags);    // NOLINT(modernize-return-braced-init-list)
      }

      virtual const PxRectangle& WinGetContentPxRectangle() const
      {
        return m_layoutCache.ContentRectPx;
      }

      virtual void WinHandleEvent(const RoutedEvent& routedEvent);

      //! @note This is only called if enabled.
      virtual void WinUpdate(const DemoTime& demoTime);

      //! @brief Called by the UITree to request a draw operation.
      //! @param drawContext the UIDrawContext.
      //! @note This is only called if enabled.
      virtual void WinDraw(const UIDrawContext& context)
      {
        FSL_PARAM_NOT_USED(context);
      }

      //! @brief Called by the UITree to request a resolve operation.
      //! @note This is only called if enabled (WindowFlags::ResolveEnabled).
      virtual void WinResolve(const DemoTime& demoTime)
      {
        FSL_PARAM_NOT_USED(demoTime);
      }

      void Arrange(const PxRectangle& finalRectPx);

      void Measure(const PxAvailableSize& availableSizePx);

      //! @brief Get the current width
      //! @return the width or a negative value if auto sizing is used
      DpLayoutSize1D GetWidth() const
      {
        return m_sizeDp.LayoutSizeWidth();
      }

      //! @brief Set the current Width
      //! @param value the new width (set it to a negative value to use auto sizing)
      void SetWidth(const DpLayoutSize1D value);

      //! @brief Get the current height
      //! @return the height or a negative value if auto sizing is used
      DpLayoutSize1D GetHeight() const
      {
        return m_sizeDp.LayoutSizeHeight();
      }

      //! @brief Set the current height
      //! @param value the new height (set it to a negative value to use auto sizing)
      void SetHeight(const DpLayoutSize1D value);

      DpThicknessF GetMargin() const
      {
        return m_marginDp;
      }

      void SetMargin(const DpThicknessF& valueDp);

      ItemAlignment GetAlignmentX() const
      {
        return m_alignmentX;
      }

      void SetAlignmentX(const ItemAlignment& value);

      ItemAlignment GetAlignmentY() const
      {
        return m_alignmentY;
      };

      void SetAlignmentY(const ItemAlignment& value);

      //! @brief Get the layout desired size.
      const PxSize2D& DesiredSizePx() const
      {
        return m_layoutCache.DesiredSizePx;
      }

      //! @brief Get the layout render size.
      const PxSize2D& RenderSizePx() const
      {
        return m_layoutCache.RenderSizePx;
      }


      //! @brief Get the layout render size in pixels.
      PxExtent2D RenderExtentPx() const
      {
        return TypeConverter::UncheckedTo<PxExtent2D>(m_layoutCache.RenderSizePx);
      }

      //! @brief Transform a screen point to be relative to this window
      PxPoint2 PointFromScreen(const PxPoint2& screenPoint) const;

      //! @brief Transform a point relative to fromWin window, to be relative to this window
      PxPoint2 PointFrom(const IWindowId* const pFromWin, const PxPoint2& pointPx) const;

      // @brief Transform a point relative to this window, to be relative to the toWin
      PxPoint2 PointTo(const IWindowId* const pToWin, const PxPoint2& pointPx) const;

      //! @brief Get the user tag value
      std::shared_ptr<ITag> GetTag() const
      {
        return m_tag;
      }

      //! @brief Set the user tag value
      //! @note allows you to associate a custom value with this window
      void SetTag(const std::shared_ptr<ITag>& value)
      {
        m_tag = value;
      }

      //! @brief call this to ensure that all animation is force finished (all factories should end their creation with this)
      void FinishAnimation()
      {
        SetAnimationUpdate(UpdateAnimationState(true));
      }

    protected:
      virtual void UpdateAnimation(const TransitionTimeSpan& timeSpan)
      {
        FSL_PARAM_NOT_USED(timeSpan);
      }

      //! @brief Update the basic animation state and return true if a animation is active
      virtual bool UpdateAnimationState(const bool forceCompleteAnimation)
      {
        FSL_PARAM_NOT_USED(forceCompleteAnimation);
        return false;
      }


      void OnClickInputPreview(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent) override
      {
        FSL_PARAM_NOT_USED(args);
        FSL_PARAM_NOT_USED(theEvent);
      }
      void OnClickInput(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent) override
      {
        FSL_PARAM_NOT_USED(args);
        FSL_PARAM_NOT_USED(theEvent);
      }
      void OnMouseOverPreview(const RoutedEventArgs& args, const std::shared_ptr<WindowMouseOverEvent>& theEvent) override
      {
        FSL_PARAM_NOT_USED(args);
        FSL_PARAM_NOT_USED(theEvent);
      }
      void OnMouseOver(const RoutedEventArgs& args, const std::shared_ptr<WindowMouseOverEvent>& theEvent) override
      {
        FSL_PARAM_NOT_USED(args);
        FSL_PARAM_NOT_USED(theEvent);
      }
      void OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent) override
      {
        FSL_PARAM_NOT_USED(args);
        FSL_PARAM_NOT_USED(theEvent);
      }
      void OnContentChanged(const RoutedEventArgs& args, const std::shared_ptr<WindowContentChangedEvent>& theEvent) override
      {
        FSL_PARAM_NOT_USED(args);
        FSL_PARAM_NOT_USED(theEvent);
      }

      bool IsReadyToSendEvents();

      //! @brief Send a event
      void SendEvent(const std::shared_ptr<WindowEvent>& event);
      // bool TrySendEvent(const std::shared_ptr<WindowEvent>& event);

      const std::shared_ptr<BaseWindowContext>& GetContext() const
      {
        return m_context;
      }

      const std::shared_ptr<WindowEventPool>& GetEventPool() const;

      inline const LayoutCache& LayoutInfo() const
      {
        return m_layoutCache;
      }

      //! @brief Check if the layout is dirty
      inline bool IsLayoutDirty() const
      {
        return m_flags.IsEnabled(BaseWindowFlags::LayoutDirty);
      };

      //! @brief Check if update is enabled
      inline bool IsUpdateEnabled() const
      {
        return m_flags.IsEnabled(BaseWindowFlags::UpdateEnabled);
      };

      void Enable(const WindowFlags& flags);
      void Disable(const WindowFlags& flags);

      inline void MarkLayoutArrangeBegin()
      {
        m_flags.Enable(BaseWindowFlags::InLayoutArrange);
      }

      inline void MarkLayoutArrangeEnd()
      {
        m_flags.Disable(BaseWindowFlags::InLayoutArrange);
      }

      inline void MarkLayoutMeasureBegin()
      {
        m_flags.Enable(BaseWindowFlags::InLayoutMeasure);
      }

      inline void MarkLayoutMeasureEnd()
      {
        m_flags.Disable(BaseWindowFlags::InLayoutMeasure);
      }

      //! @brief position child windows and determine their size according to the given size.
      //! @return the actual size used.
      //! @note finalSize the area that was allocated for the window and which should be used to arrange itself and its children.
      //! @note If you wish to customize the arrange pass of the layout process you should override this method.
      //!       A overridden method is responsible for calling Arrange on each visible child window and pass the final desiredSize and
      //!       position to each child window.
      virtual PxSize2D ArrangeOverride(const PxSize2D& finalSizePx)
      {
        return finalSizePx;
      }

      //! @brief Measure this window and its children.
      //! @param availableSize The available size that this window can give to child windows. Infinity can be
      //!        specified as a value to indicate that the window will size to whatever content is available.
      //! @return The size that the window needs during layout based on child window measurements.
      //! @note Allows you to implement custom layout sizing behavior for the window.
      //!       The general strategy is
      //!       1. Iterate over the child windows that are part of the layout and call Measure.
      //!       2. Immediately get the desired size of the child.
      //!       3. Compute the total desired size of this window based on the child element requirements.
      //!          During this process the child windows can return a larger DesiredSize than the initial AvailableSpace to
      //!          indicate that it needs more space. Which can be handled by introducing a scrollable region, resizing the parent control, adding
      //!          some form of stacked order or another appropriate solution for measuring and arranging.
      virtual PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx)
      {
        FSL_PARAM_NOT_USED(availableSizePx);
        return {};
      }

      //! @brief Call this when a property has been updated, to allow the controls to react on it.
      void PropertyUpdated(const PropertyTypeFlags& flags);

      //! @brief Override this to get property update notifications
      virtual void OnPropertiesUpdated(const PropertyTypeFlags& flags)
      {
        FSL_PARAM_NOT_USED(flags);
      }

      //! @brief Control the layout dirty flag
      void SetLayoutDirty(const bool isDirty);

    private:
      //! @brief Toggle update calls on / off due to animation state
      void SetAnimationUpdate(const bool isAnimating);

    public:
      //! @brief Do not call this, intended for internal UI framework use only
      void SYS_MarkLayoutDirty()
      {
        SetLayoutDirty(true);
      }
    };
  }
}

#endif
