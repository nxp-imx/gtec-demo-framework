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
#include <FslBase/Noncopyable.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Rect.hpp>
#include <FslBase/Math/ThicknessF.hpp>
#include <FslSimpleUI/Base/System/IEventListener.hpp>
#include <FslSimpleUI/Base/ItemAlignment.hpp>
#include <FslSimpleUI/Base/IWindowId.hpp>
#include <FslSimpleUI/Base/LayoutCache.hpp>
#include <FslSimpleUI/Base/BaseWindowFlags.hpp>
#include <memory>

namespace Fsl
{
  struct DemoTime;

  namespace UI
  {
    struct PropertyTypeFlags;
    struct RoutedEvent;
    struct RoutedEventArgs;
    struct UIDrawContext;
    class WindowContext;
    class WindowEvent;
    class WindowEventPool;
    class WindowInputClickEvent;
    class WindowSelectEvent;

    class BaseWindow
      : public IWindowId
      , protected IEventListener
      , private Noncopyable
    {
      const std::shared_ptr<WindowContext> m_context;
      std::shared_ptr<ITag> m_tag;
      Vector2 m_size;
      ThicknessF m_margin;
      ItemAlignment m_alignmentX;
      ItemAlignment m_alignmentY;
      BaseWindowFlags m_flags;
      LayoutCache m_layoutCache;
    public:
      BaseWindow(const std::shared_ptr<WindowContext>& context);
      virtual ~BaseWindow();

      //! @brief Called on windows that registered for it once they become known to the window manager.
      virtual void WinInit() {}
      //! @brief Called by the engine to mark the layout of the window as dirty.
      //! @warning This method is not allowed to throw a exception!!!!!
      //! @return true if the window was marked as dirty, false if it was already dirty.
      virtual bool WinMarkLayoutAsDirty();
      virtual WindowFlags WinGetFlags() const { return m_flags; }
      virtual Rect WinGetContentRect() const { return m_layoutCache.ContentRect; }
      virtual void WinHandleEvent(const RoutedEvent& routedEvent);

      virtual void WinUpdate(const DemoTime& demoTime) {};
      //! @brief Called by the UITree to request a draw operation.
      //! @param drawContext the UIDrawContext.
      virtual void WinDraw(const UIDrawContext& context) {};

      void Arrange(const Rect& finalRect);
      void Measure(const Vector2& availableSize);

      //! @brief Get the current width
      //! @return the width or a negative value if auto sizing is used
      float GetWidth() const { return m_size.X; }

      //! @brief Set the current Width
      //! @param value the new width (set it to a negative value to use auto sizing)
      void SetWidth(const float value);

      //! @brief Set the current Width
      //! @param value the new width (set it to a negative value to use auto sizing)
      void SetWidth(const int32_t value);

      //! @brief Get the current height
      //! @return the height or a negative value if auto sizing is used
      float GetHeight() const { return m_size.Y; }

      //! @brief Set the current height
      //! @param value the new height (set it to a negative value to use auto sizing)
      void SetHeight(const float value);

      //! @brief Set the current height
      //! @param value the new height (set it to a negative value to use auto sizing)
      void SetHeight(const int32_t value);

      ThicknessF GetMargin() const { return m_margin; }
      void SetMargin(const ThicknessF& value);

      ItemAlignment GetAlignmentX() const { return m_alignmentX; }
      void SetAlignmentX(const ItemAlignment& value);
      ItemAlignment GetAlignmentY() const { return m_alignmentY; };
      void SetAlignmentY(const ItemAlignment& value);

      //! @brief Get the layout desired size.
      const Vector2& DesiredSize() const { return m_layoutCache.DesiredSize; }
      //! @brief Get the layout render size.
      const Vector2& RenderSize() const { return m_layoutCache.RenderSize; }

      //! @brief Transform a screen point to be relative to this window
      Vector2 PointFromScreen(const Vector2& screenPoint) const;

      //! @brief Transform a point relative to fromWin window, to be relative to this window
      Vector2 PointFrom(const IWindowId*const pFromWin, const Vector2& point) const;

      // @brief Transform a point relative to this window, to be relative to the toWin
      Vector2 PointTo(const IWindowId*const pToWin, const Vector2& point) const;

      //! @brief Get the user tag value
      std::shared_ptr<ITag> GetTag() const { return m_tag; }

      //! @brief Set the user tag value
      //! @note allows you to associate a custom value with this window
      void SetTag(const std::shared_ptr<ITag>& value) { m_tag = value; }

    protected:
      virtual void OnClickInputPreview(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent) override { }
      virtual void OnClickInput(const RoutedEventArgs& args, const std::shared_ptr<WindowInputClickEvent>& theEvent) override { }
      virtual void OnSelect(const RoutedEventArgs& args, const std::shared_ptr<WindowSelectEvent>& theEvent) override { }
      virtual void OnContentChanged(const RoutedEventArgs& args, const std::shared_ptr<WindowContentChangedEvent>& theEvent) override { }

      //! @brief Send a event
      void SendEvent(const std::shared_ptr<WindowEvent>& event);

      const std::shared_ptr<WindowContext>& GetContext() const
      {
        return m_context;
      }

      const std::shared_ptr<WindowEventPool>& GetEventPool() const;

      inline const LayoutCache& LayoutInfo() const
      {
        return m_layoutCache;
      }

      //! @brief Check if the layout is dirty
      inline bool IsLayoutDirty() const { return m_flags.IsEnabled(BaseWindowFlags::LayoutDirty); };

      void Enable(const WindowFlags& flags);

      inline void MarkLayoutArrangeBegin() { m_flags.Enable(BaseWindowFlags::InLayoutArrange); }
      inline void MarkLayoutArrangeEnd() { m_flags.Disable(BaseWindowFlags::InLayoutArrange); }
      inline void MarkLayoutMeasureBegin() { m_flags.Enable(BaseWindowFlags::InLayoutMeasure); }
      inline void MarkLayoutMeasureEnd() { m_flags.Disable(BaseWindowFlags::InLayoutMeasure); }

      //! @brief position child windows and determine their size according to the given size.
      //! @return the actual size used.
      //! @note finalSize the area that was allocated for the window and which should be used to arrange itself and its children.
      //! @note If you wish to customize the arrange pass of the layout process you should override this method.
      //!       A overridden method is responsible for calling Arrange on each visible child window and pass the final desiredSize and
      //!       position to each child window.
      virtual Vector2 ArrangeOverride(const Vector2& finalSize) { return finalSize; }

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
      //!          indicate that it needs more space. Which can
      //!       During this process, child elements might return a larger DesiredSize size than the initial availableSize to
      //!       indicate that the child element wants more space. This might be handled in your own implementation by
      //!       introducing a scrollable region, by resizing the parent control, by establishing some manner of stacked order,
      //!       or any number of solutions for measuring or arranging content.
      virtual Vector2 MeasureOverride(const Vector2& availableSize) { return Vector2::Zero(); }

      //! @brief Call this when a property has been updated, to allow the controls to react on it.
      void PropertyUpdated(const PropertyTypeFlags& flags);

      //! @brief Override this to get property update notifications
      virtual void OnPropertiesUpdated(const PropertyTypeFlags& flags) {};

      //! @brief Control the layout dirty flag
      void SetLayoutDirty(const bool isDirty);

      friend class BaseWindowCollection;
    };
  }
}

#endif
