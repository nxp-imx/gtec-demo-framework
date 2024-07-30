#ifndef FSLSIMPLEUI_BASE_SYSTEM_TREENODE_HPP
#define FSLSIMPLEUI_BASE_SYSTEM_TREENODE_HPP
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

#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <cassert>
#include <deque>
#include <memory>
#include <utility>
#include "TreeNodeFlags.hpp"

namespace Fsl::UI
{
  class DrawCommandBuffer;
  struct RoutedEvent;
  struct UIDrawContext;

  class TreeNode
  {
    std::weak_ptr<TreeNode> m_parent;
    std::shared_ptr<BaseWindow> m_window;
    TreeNodeFlags m_flags;

  public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    std::deque<std::shared_ptr<TreeNode>> m_children;

    explicit TreeNode(const std::shared_ptr<BaseWindow>& window)
      : m_window(window)
      , m_flags(ExtractWindowFlags(window))
    {
    }


    TreeNode(std::weak_ptr<TreeNode> parent, const std::shared_ptr<BaseWindow>& window)
      : m_parent(std::move(parent))
      , m_window(window)
      , m_flags(ExtractWindowFlags(window))
    {
    }


    //! @brief A static method to so we dont need to use enable_shared_from_this
    static void AddChild(const std::shared_ptr<TreeNode>& parentNode, const std::shared_ptr<TreeNode>& childNode);

    inline std::shared_ptr<TreeNode> GetParent() const
    {
      return m_parent.lock();
    }

    //! @brief Get the associated window
    inline const std::shared_ptr<BaseWindow>& GetWindow() const noexcept
    {
      return m_window;
    }

    inline BaseWindow* GetWindowPointer() noexcept
    {
      return m_window.get();
    }

    inline const BaseWindow* GetWindowPointer() const noexcept
    {
      return m_window.get();
    }

    inline TreeNodeFlags GetFlags() const noexcept
    {
      return m_flags;
    }

    //! @brief Check if this node is considered to be running
    inline bool IsConsideredRunning() const noexcept
    {
      return m_flags.IsRunning();
    }

    //! @brief Check if this node is considered to be enabled
    inline bool IsEnabled() const noexcept
    {
      return m_flags.IsRunning();
    }

    //! @brief Check if this node has been marked as disposed
    inline bool IsDisposed() const noexcept
    {
      return m_flags.IsDisposed();
    }

    //! @brief Mark this node as disposed (expects that all children have been marked as disposed)
    void MarkNodeAsDisposed();

    //! @brief Remove a child (expects the the child has been marked as disposed)
    void RemoveChild(const std::shared_ptr<TreeNode>& childNode);
    void ClearChildren()
    {
      m_children.clear();
    }


    inline void EnableFlags(const TreeNodeFlags flags) noexcept
    {
      m_flags.Value |= flags.Value;
    }


    inline void DisableFlags(const TreeNodeFlags flags) noexcept
    {
      m_flags.Value &= ~flags.Value;
    }


    inline void SetVisibility(const ItemVisibility visibility) noexcept
    {
      m_flags.SetVisibility(visibility);
    }

    inline void Update(const TimeSpan& timespan)
    {
      assert(m_flags.IsFlagged(TreeNodeFlags::UpdateEnabled));
      assert(m_flags.IsRunning());
      m_window->WinUpdate(timespan);
    }

    inline void Resolve(const TimeSpan& timespan)
    {
      assert(m_flags.IsFlagged(TreeNodeFlags::ResolveEnabled));
      assert(m_flags.IsRunning());
      m_window->WinResolve(timespan);
    }

    inline void PostLayout()
    {
      assert(m_flags.IsFlagged(TreeNodeFlags::PostLayoutEnabled));
      assert(m_flags.IsRunning());
      m_window->WinPostLayout();
    }

    inline void Draw(const UIDrawContext& context)
    {
      assert(m_flags.IsFlagged(TreeNodeFlags::DrawEnabled));
      assert(m_flags.IsRunning());
      m_window->WinDraw(context);
    }

    inline bool WinMarkLayoutAsDirty()
    {
      assert(m_flags.IsRunning());
      return m_window->WinMarkLayoutAsDirty();
    }

    inline const PxRectangle& WinGetContentRectanglePx() const
    {
      assert(m_flags.IsRunning());
      return m_window->WinGetContentRectanglePx();
    }

    inline void WinHandleEvent(const RoutedEvent& routedEvent)
    {
      assert(m_flags.IsRunning());
      m_window->WinHandleEvent(routedEvent);
    }

    inline PxPoint2 CalcScreenTopLeftCornerPx() const
    {
      std::shared_ptr<TreeNode> parent = m_parent.lock();
      return (parent ? (parent->CalcScreenTopLeftCornerPx() + WinGetContentRectanglePx().TopLeft()) : WinGetContentRectanglePx().TopLeft());
    }

    void OnResolutionChanged(const ResolutionChangedInfo& info)
    {
      if (m_flags.IsRunning())
      {
        m_window->WinResolutionChanged(info);
        for (auto& rChild : m_children)
        {
          rChild->OnResolutionChanged(info);
        }
      }
    }

  private:
    static TreeNodeFlags ExtractWindowFlags(const std::shared_ptr<BaseWindow>& window) noexcept
    {
      return TreeNodeFlags(window->WinGetFlags());
    }
  };
}

#endif
