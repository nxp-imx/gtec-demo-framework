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
#include <FslBase/Math/Dp/DpThicknessF.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslDataBinding/Base/Object/DependencyObject.hpp>
#include <FslDataBinding/Base/Property/TypedDependencyProperty.hpp>
#include <FslGraphics/ColorsU16.hpp>
#include <FslSimpleUI/Base/BaseWindowFlags.hpp>
#include <FslSimpleUI/Base/DefaultValues.hpp>
#include <FslSimpleUI/Base/DpLayoutSize1D.hpp>
#include <FslSimpleUI/Base/DpLayoutSize2D.hpp>
#include <FslSimpleUI/Base/IWindowId.hpp>
#include <FslSimpleUI/Base/ItemAlignment.hpp>
#include <FslSimpleUI/Base/ItemVisibility.hpp>
#include <FslSimpleUI/Base/LayoutCache.hpp>
#include <FslSimpleUI/Base/Property/DependencyPropertyUIColor.hpp>
#include <FslSimpleUI/Base/PxAvailableSize.hpp>
#include <FslSimpleUI/Base/System/IEventListener.hpp>
#include <FslSimpleUI/Base/UIColor.hpp>
#include <FslSimpleUI/Base/UIColors.hpp>
#include <memory>

namespace Fsl
{
  struct TimeSpan;

  namespace UI
  {
    class BaseWindowContext;
    struct PropertyTypeFlags;
    struct ResolutionChangedInfo;
    struct RoutedEvent;
    struct UIDrawContext;
    class WindowEvent;
    class WindowEventPool;

    class BaseWindow
      : public DataBinding::DependencyObject
      , public IWindowId
      , protected IEventListener
    {
      using base_type = DataBinding::DependencyObject;

      // NOLINTNEXTLINE(readability-identifier-naming)
      const std::shared_ptr<BaseWindowContext> m_context;
      std::shared_ptr<ITag> m_tag;
      int32_t m_tagValue{0};

      DataBinding::TypedDependencyProperty<DpLayoutSize1D> m_propertyWidthDp;
      DataBinding::TypedDependencyProperty<DpLayoutSize1D> m_propertyHeightDp;
      DataBinding::TypedDependencyProperty<DpSize1DF> m_propertyMinWidthDpf;
      DataBinding::TypedDependencyProperty<DpSize1DF> m_propertyMinHeightDpf;
      DataBinding::TypedDependencyProperty<DpLayoutSize1D> m_propertyMaxWidthDpf;
      DataBinding::TypedDependencyProperty<DpLayoutSize1D> m_propertyMaxHeightDpf;
      DataBinding::TypedDependencyProperty<DpThicknessF> m_propertyMarginDpf;
      DataBinding::TypedDependencyProperty<ItemAlignment> m_propertyAlignmentX{ItemAlignment::Near};
      DataBinding::TypedDependencyProperty<ItemAlignment> m_propertyAlignmentY{ItemAlignment::Near};
      DependencyPropertyUIColor m_propertyBaseColor;

      BaseWindowFlags m_flags;
      LayoutCache m_layoutCache;

      UIRenderColor m_parentBaseColor{ColorsU16::White()};
      UIRenderColor m_cachedBaseColor{ColorsU16::White()};

    public:
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyWidthDp;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyHeightDp;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyMinWidthDp;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyMinHeightDp;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyMaxWidthDp;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyMaxHeightDp;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyMarginDpf;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyAlignmentX;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyAlignmentY;
      // NOLINTNEXTLINE(readability-identifier-naming)
      static DataBinding::DependencyPropertyDefinition PropertyBaseColor;

      BaseWindow(const BaseWindow&) = delete;
      BaseWindow& operator=(const BaseWindow&) = delete;

      explicit BaseWindow(const std::shared_ptr<BaseWindowContext>& context, const WindowFlags initialFlags = {});
      ~BaseWindow() override;


      //! @brief Called on windows that registered for it once they become known to the window manager.
      virtual void WinInit()
      {
      }

      //! @brief Called on all windows just before the window is disposed from the window manager
      //! @note  If the window is not registered in a window manager this will not get called!
      void WinShutdown()
      {
        OnShutdown();
      }

      //! @brief Called by the engine to inform the window that the resolution has been modified and give it a chance to recache things as needed.
      //! @note this will also mark the layout as dirty
      //! @warning This method is not allowed to throw a exception!!!!!
      virtual void WinResolutionChanged(const ResolutionChangedInfo& info);

      //! @brief Called by the engine to mark the layout of the window as dirty.
      //! @warning This method is not allowed to throw a exception!!!!!
      //! @return true if the window was marked as dirty, false if it was already dirty.
      virtual bool WinMarkLayoutAsDirty();

      WindowFlags WinGetFlags() const noexcept
      {
        return WindowFlags(m_flags);    // NOLINT(modernize-return-braced-init-list,google-readability-casting)
      }

      const PxRectangle& WinGetContentRectanglePx() const noexcept
      {
        return m_layoutCache.ContentRectPx;
      }

      virtual void WinHandleEvent(const RoutedEvent& routedEvent);

      //! @note This is only called if enabled.
      virtual void WinUpdate(const TimeSpan& timespan);


      //! @brief Called by the UITree to request a draw operation.
      //! @param drawContext the UIDrawContext.
      //! @note This is only called if enabled.
      virtual void WinDraw(const UIDrawContext& context)
      {
        FSL_PARAM_NOT_USED(context);
        SetContentRenderingDirty(false);
      }

      //! @brief Called by the UITree to request a resolve operation.
      //! @note This is only called if enabled (WindowFlags::ResolveEnabled).
      virtual void WinResolve(const TimeSpan& timespan)
      {
        FSL_PARAM_NOT_USED(timespan);
      }

      void Arrange(const PxRectangle& finalRectPx);

      void Measure(const PxAvailableSize& availableSizePx);

      //! @brief Called by the UITree to allow mesh resizing before the actual draw sequence starts.
      //! @param drawContext the UIDrawContext.
      //! @note This is only called if enabled.
      virtual void WinPostLayout()
      {
      }

      //! @brief Get the current width
      //! @return the width or a negative value if auto sizing is used
      DpLayoutSize1D GetWidth() const noexcept
      {
        return m_propertyWidthDp.Get();
      }

      //! @brief Set the current Width
      //! @param value the new width (set it to a negative value to use auto sizing)
      bool SetWidth(const DpLayoutSize1D value);

      //! @brief Get the current height
      //! @return the height or a negative value if auto sizing is used
      DpLayoutSize1D GetHeight() const noexcept
      {
        return m_propertyHeightDp.Get();
      }

      //! @brief Set the current height
      //! @param value the new height (set it to a negative value to use auto sizing)
      bool SetHeight(const DpLayoutSize1D value);


      //! @brief Get the current minimum width
      //! @return the minimum width (minWidth will be >= 0, not NaN and not positive infinity)
      DpSize1DF GetMinWidth() const noexcept
      {
        return m_propertyMinWidthDpf.Get();
      }

      //! @brief Set the current minimum Width
      //! @param value the new min width (>= 0, not nan, not positive infinity)
      bool SetMinWidth(const DpSize1DF value);

      //! @brief Get the current minimum height
      //! @return the minimum height (minHeight will be >= 0, not NaN and not positive infinity)
      DpSize1DF GetMinHeight() const noexcept
      {
        return m_propertyMinHeightDpf.Get();
      }

      //! @brief Set the current minimum height
      //! @param value the new min height (>= 0, not nan, not positive infinity)
      bool SetMinHeight(const DpSize1DF value);


      //! @brief Get the current max width (Defaults to positive infinity)
      //! @return the maximum width (maxWidth will be >= 0 and <= positive infinity, not NaN)
      DpLayoutSize1D GetMaxWidth() const noexcept
      {
        return m_propertyMaxWidthDpf.Get();
      }

      //! @brief Set the current max Width.  Default is positive infinity to ensure no limits exist.
      bool SetMaxWidth(const DpLayoutSize1D value);

      //! @brief Get the current max height (Defaults to positive infinity)
      //! @return the maximum height (maxHeight will be >= 0 and <= positive infinity, not NaN)
      DpLayoutSize1D GetMaxHeight() const noexcept
      {
        return m_propertyMaxHeightDpf.Get();
      }

      //! @brief Set the current max height.  Default is positive infinity to ensure no limits exist.
      bool SetMaxHeight(const DpLayoutSize1D value);


      DpThicknessF GetMargin() const noexcept
      {
        return m_propertyMarginDpf.Get();
      }

      bool SetMargin(const DpThicknessF value);

      ItemAlignment GetAlignmentX() const noexcept
      {
        return m_propertyAlignmentX.Get();
      }

      bool SetAlignmentX(const ItemAlignment value);

      ItemAlignment GetAlignmentY() const noexcept
      {
        return m_propertyAlignmentY.Get();
      };

      bool SetAlignmentY(const ItemAlignment value);


      UIColor GetBaseColor() const noexcept
      {
        return m_propertyBaseColor.Get();
      }

      bool SetBaseColor(const UIColor value);


      ItemVisibility GetVisibility() const noexcept
      {
        return m_flags.GetVisibility();
      }

      void SetVisibility(const ItemVisibility value);

      //! @brief Get the layout desired size.
      const PxSize2D& DesiredSizePx() const noexcept
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

      //! @brief Transform a point relative to this window to screen
      PxPoint2 PointToScreen(const PxPoint2& windowPointPx) const;

      //! @brief Transform a point relative to fromWin window, to be relative to this window
      PxPoint2 PointFrom(const IWindowId* const pFromWin, const PxPoint2& pointPx) const;

      // @brief Transform a point relative to this window, to be relative to the toWin
      PxPoint2 PointTo(const IWindowId* const pToWin, const PxPoint2& pointPx) const;


      //! @brief Get the user tag value
      int32_t GetTagValue() const noexcept
      {
        return m_tagValue;
      }

      //! @brief Set the user tag value
      //! @note allows you to associate a custom value with this window
      void SetTagValue(const int32_t tagValue) noexcept
      {
        m_tagValue = tagValue;
      }

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

      //! @brief This is only intended to be called internally from components reacting to the base color being changed to propagate it to the
      //! children.
      // NOLINTNEXTLINE(readability-identifier-naming)
      void SYS_SetParentBaseColor(const UIRenderColor color);


      bool IsBusy() const noexcept
      {
        return m_flags.IsEnabled(WindowFlags::UpdateEnabled);
      }

    protected:
      virtual void OnShutdown()
      {
      }

      UIRenderColor GetFinalBaseColor() const noexcept
      {
        return m_cachedBaseColor;
      }


      UIRenderColor GetParentBaseColor() const noexcept
      {
        return m_parentBaseColor;
      }


      virtual void UpdateAnimation(const TimeSpan& timeSpan)
      {
        FSL_PARAM_NOT_USED(timeSpan);
      }

      //! @brief Update the basic animation state and return true if a animation is active
      virtual bool UpdateAnimationState(const bool forceCompleteAnimation)
      {
        FSL_PARAM_NOT_USED(forceCompleteAnimation);
        return false;
      }


      void OnClickInputPreview(const std::shared_ptr<WindowInputClickEvent>& theEvent) override
      {
        FSL_PARAM_NOT_USED(theEvent);
      }
      void OnClickInput(const std::shared_ptr<WindowInputClickEvent>& theEvent) override
      {
        FSL_PARAM_NOT_USED(theEvent);
      }
      void OnMouseOverPreview(const std::shared_ptr<WindowMouseOverEvent>& theEvent) override
      {
        FSL_PARAM_NOT_USED(theEvent);
      }
      void OnMouseOver(const std::shared_ptr<WindowMouseOverEvent>& theEvent) override
      {
        FSL_PARAM_NOT_USED(theEvent);
      }
      void OnSelect(const std::shared_ptr<WindowSelectEvent>& theEvent) override
      {
        FSL_PARAM_NOT_USED(theEvent);
      }
      void OnContentChanged(const std::shared_ptr<WindowContentChangedEvent>& theEvent) override
      {
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

      void Enable(const WindowFlags flags);
      void Disable(const WindowFlags flags);
      void Set(const WindowFlags flags, bool enabled);

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

      //! @brief Control the content dirty flag
      void SetContentRenderingDirty(const bool isDirty);

      void CheckAnimationState()
      {
        SetAnimationUpdate(UpdateAnimationState(false));
      }

    private:
      //! @brief Toggle update calls on / off due to animation state
      void SetAnimationUpdate(const bool isAnimating);

    public:
      //! @brief Do not call this, intended for internal UI framework use only
      // NOLINTNEXTLINE(readability-identifier-naming)
      void SYS_MarkLayoutDirty()
      {
        SetLayoutDirty(true);
      }

    protected:
      DataBinding::DataBindingInstanceHandle TryGetPropertyHandleNow(const DataBinding::DependencyPropertyDefinition& sourceDef) override;
      DataBinding::PropertySetBindingResult TrySetBindingNow(const DataBinding::DependencyPropertyDefinition& targetDef,
                                                             const DataBinding::Binding& binding) override;
      void ExtractAllProperties(DataBinding::DependencyPropertyDefinitionVector& rProperties) override;
    };
  }
}

#endif
