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

#include "UITree.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslDemoApp/Base/DemoTime.hpp>
#include <FslSimpleUI/Base/Event/WindowEvent.hpp>
#include <FslSimpleUI/Base/Event/WindowEventPool.hpp>
#include <cassert>
#include "RootWindow.hpp"
#include "TreeNode.hpp"
#include "Modules/ModuleCallbackRegistry.hpp"
#include "Event/WindowEventQueueEx.hpp"

namespace Fsl
{
  namespace UI
  {

    namespace
    {
      const std::size_t MAX_EVENT_LOOPS = 1024;

      inline void MarkWindowAndParentsAsDirty(const std::shared_ptr<TreeNode>& node)
      {
        std::shared_ptr<TreeNode> currentNode = node;
        while (currentNode && currentNode->WinMarkLayoutAsDirty())
          currentNode = currentNode->GetParent();
      }


      inline void RemoveDictEntry(WindowToNodeMap& rDict, const std::shared_ptr<BaseWindow>& window)
      {
        auto itrNode = rDict.find(window.get());
        if (itrNode != rDict.end())
          rDict.erase(itrNode);
      }


      inline void CommandAddChild(const std::shared_ptr<ModuleCallbackRegistry>& moduleCallbackRegistry, const std::shared_ptr<TreeNode>& parentNode, const std::shared_ptr<TreeNode>& node, FastTreeNodeDeque* pNewWindows, const TreeNodeFlags& filterFlags)
      {
        TreeNode::AddChild(parentNode, node);
        if (pNewWindows != nullptr)
        {
          if ((node->GetFlags().Value & filterFlags.Value) != 0)
            pNewWindows->push_back(node.get());
        }
      }

      inline void CommandScheduleCloseEx(WindowToNodeMap& rDict, const std::shared_ptr<ModuleCallbackRegistry>& moduleCallbackRegistry, const std::shared_ptr<TreeNode>& node, FastTreeNodeDeque* pNewWindows)
      {
        // Depth first close
        auto& nodeChildren = node->m_children;
        for (auto itr = nodeChildren.begin(); itr != nodeChildren.end(); ++itr)
          CommandScheduleCloseEx(rDict, moduleCallbackRegistry, *itr, pNewWindows);
        node->ClearChildren();

        // Check if its in the newWindowsList and remove it if it was
        if (pNewWindows != nullptr)
        {
          auto itrFind = std::find(pNewWindows->begin(), pNewWindows->end(), node.get());
          if (itrFind != pNewWindows->end())
            pNewWindows->erase(itrFind);
        }

        // We dont remove the child node from the parent here since it will break iterators already on the stack
        // instead we allow the caller to handle that detail

        // Give the modules a chance to react
        moduleCallbackRegistry->ModuleOnTreeNodeDispose(node);

        // mark the node as disposed
        node->MarkNodeAsDisposed();

        // remove it from the lookup dict
        RemoveDictEntry(rDict, node->GetWindow());
      }


      inline void CommandScheduleClose(WindowToNodeMap& rDict, const std::shared_ptr<ModuleCallbackRegistry>& moduleCallbackRegistry, const std::shared_ptr<TreeNode>& node, FastTreeNodeDeque* pNewWindows)
      {
        CommandScheduleCloseEx(rDict, moduleCallbackRegistry, node, pNewWindows);

        // Remove the node from the parent
        auto parent = node->GetParent();
        if (parent)
          parent->RemoveChild(node);
      }


      inline void CommandScheduleCloseChildren(WindowToNodeMap& rDict, const std::shared_ptr<ModuleCallbackRegistry>& moduleCallbackRegistry, const std::shared_ptr<TreeNode>& node, FastTreeNodeDeque* pNewWindows)
      {
        auto& nodeChildren = node->m_children;
        for (auto itr = nodeChildren.begin(); itr != nodeChildren.end(); ++itr)
          CommandScheduleCloseEx(rDict, moduleCallbackRegistry, *itr, pNewWindows);
        node->ClearChildren();
      }



      inline bool IsWindowMemberOfTree(const TreeNode& node, const std::shared_ptr<BaseWindow>& window)
      {
        auto& nodeChildren = node.m_children;
        for (auto itr = nodeChildren.begin(); itr != nodeChildren.end(); ++itr)
        {
          assert(*itr);
          if ((*itr)->GetWindow() == window)
            return true;
          if (IsWindowMemberOfTree(*(itr->get()), window))
            return true;
        }
        return false;
      }


      inline bool IsWindowMemberOfTree(const TreeNode& node, const std::shared_ptr<BaseWindow>& window, const bool considerTreeRootAMember)
      {
        if (considerTreeRootAMember && node.GetWindow() == window)
          return true;
        return IsWindowMemberOfTree(node, window);
      }
    }


    UITree::UITree(const std::shared_ptr<ModuleCallbackRegistry>& moduleCallbackRegistry, const std::shared_ptr<WindowEventPool>& eventPool, const std::shared_ptr<WindowEventQueueEx>& eventQueue, const Vector2& size)
      : m_moduleCallbackRegistry(moduleCallbackRegistry)
      , m_eventPool(eventPool)
      , m_eventQueue(eventQueue)
      , m_eventRecordQueue(new std::deque<WindowEventQueueRecord>())
      , m_dict()
      , m_rootWindow(std::make_shared<RootWindow>(size))
      , m_root(std::make_shared<TreeNode>(m_rootWindow))
      , m_rootRect(Rect(0, 0, size.X, size.Y))
      , m_updateCacheDirty(true)
      , m_drawCacheDirty(true)
      , m_clickInputCacheDirty(true)
      , m_layoutIsDirty(true)
      , m_dequeUpdate()
      , m_dequeDraw()
      , m_nodeScratchpad()
      , m_context(Context::System)
    {
      if (!m_eventPool)
        throw std::invalid_argument("eventPool can not be null");
      if (!m_eventQueue)
        throw std::invalid_argument("eventQueue can not be null");

      m_dict.emplace(m_root->GetWindow().get(), m_root);
    }


    UITree::~UITree()
    {
    }


    void UITree::Resized(const Vector2& size)
    {
      ScopedContextChange scopedContextChange(this, Context::Internal);

      m_rootRect = Rect(0, 0, size.X, size.Y);
      m_rootWindow->SetScreenResolution(size);

      m_layoutIsDirty = true;
    }


    void UITree::Update(const DemoTime& demoTime)
    {
      ScopedContextChange scopedContextChange(this, Context::Internal);

      ProcessEventsPreUpdate();

      { // Update all the existing windows
        for (auto itr = m_dequeUpdate.begin(); itr != m_dequeUpdate.end(); ++itr)
        {
          (*itr)->Update(demoTime);
        }
      }

      ProcessEventsPostUpdate(demoTime);

      PerformLayout();
    }


    void UITree::Draw()
    {
      ScopedContextChange scopedContextChange(this, Context::Internal);

      for (auto itr = m_dequeDraw.begin(); itr != m_dequeDraw.end(); ++itr)
      {
        itr->pNode->Draw(itr->DrawContext);
      }
    }


    Vector2 UITree::PointToScreen(const IWindowId*const pWindow, const Vector2& point) const
    {
      const IWindowId*const pActualWindow = (pWindow != nullptr ? pWindow : m_rootWindow.get());

      // Check if we know the window
      const auto itr = m_dict.find(pActualWindow);
      if (itr == m_dict.end())
      {
        FSLLOG_WARNING("PointFromScreen unknown window");
        return Vector2();
      }
      return (itr->second->CalcScreenTopLeftCorner() + point);
    }


    Vector2 UITree::PointFromScreen(const IWindowId*const pWindow, const Vector2& point) const
    {
      const IWindowId*const pActualWindow = (pWindow != nullptr ? pWindow : m_rootWindow.get());

      // Check if we know the window
      const auto itr = m_dict.find(pActualWindow);
      if (itr == m_dict.end())
      {
        FSLLOG_WARNING("PointFromScreen unknown window");
        return Vector2();
      }

      return (point - itr->second->CalcScreenTopLeftCorner());
    }


    //! @note all WindowManager modifications to tree need to be queued into the command queue
    void UITree::Add(const std::shared_ptr<BaseWindow>& window)
    {
      // ScopedContextChange scopedContextChange(this, Context::Internal);  --> We rely on AddChild to change the context
      AddChild(m_root->GetWindow(), window);
    }


    //! @note all WindowManager modifications to tree need to be queued into the command queue
    void UITree::AddChild(const BaseWindow*const parentWindow, const std::shared_ptr<BaseWindow>& window)
    {
      FSLLOG_WARNING_IF(m_context == Context::InternalLayout, "Children should not be added during layout");

      ScopedContextChange scopedContextChange(this, Context::Internal);

      if (parentWindow == nullptr || !window)
        throw std::invalid_argument("windows can not be null");

      // Locate the parent node
      const auto itrParent = m_dict.find(parentWindow);
      if (itrParent == m_dict.end())
        throw std::invalid_argument("the parent window is not part of this tree");

      if (m_dict.find(window.get()) != m_dict.end())
        throw std::invalid_argument("the window is already part of this tree");

      const auto flags = window->WinGetFlags();


      // We add the element to the lookup dict right away
      auto node = std::make_shared<TreeNode>(itrParent->second, window);
      try
      {
        m_dict.emplace(window.get(), node);
        m_eventQueue->Push(WindowEventQueueRecord(WindowEventQueueRecordType::AddChild, itrParent->second, node));

        if (flags.IsEnabled(WindowFlags::WinInit))
          window->WinInit();
      }
      catch (const std::exception&)
      {
        // Remove the new node from the dict on error
        RemoveDictEntry(m_dict, window);
        throw;
      }
    }


    //! @note all WindowManager modifications to tree need to be queued into the command queue
    void UITree::AddChild(const std::shared_ptr<BaseWindow>& parentWindow, const std::shared_ptr<BaseWindow>& window)
    {
      // ScopedContextChange scopedContextChange(this, Context::Internal);  --> We rely on AddChild to change the context
      AddChild(parentWindow.get(), window);
    }

    bool UITree::Exists(const BaseWindow*const pWindow) const
    {
      if (pWindow == nullptr)
      {
        FSLLOG_DEBUG_WARNING("A null window will always return false");
        return false;
      }
      return (m_dict.find(pWindow) != m_dict.end());
    }

    bool UITree::Exists(const std::shared_ptr<BaseWindow>& window) const
    {
      // ScopedContextChange scopedContextChange(this, Context::Internal);  --> Nothing here does callbacks, so no need for a context change
      return (m_dict.find(window.get()) != m_dict.end());
    }


    bool UITree::IsMemberOfTree(const std::shared_ptr<BaseWindow>& tree, const std::shared_ptr<BaseWindow>& window) const
    {
      // ScopedContextChange scopedContextChange(this, Context::Internal); --> We rely on IsMemberOfTree to change the context
      return IsMemberOfTree(tree, window, true);
    }


    bool UITree::IsMemberOfTree(const std::shared_ptr<BaseWindow>& tree, const std::shared_ptr<BaseWindow>& window, const bool considerTreeRootAMember) const
    {
      ScopedContextChange scopedContextChange(this, Context::Internal);

      if (!tree || !window)
      {
        FSLLOG_WARNING("one of the windows is null, this will always return false");
        return false;
      }

      // Locate the parent node
      const auto itrParent = m_dict.find(tree.get());
      if (itrParent == m_dict.end())
      {
        FSLLOG_WARNING("tree window is not a member of the UITree");
        return false;
      }
      assert(itrParent->second);
      return IsWindowMemberOfTree(*(itrParent->second.get()), window, considerTreeRootAMember);
    }


    //! @note all WindowManager modifications to tree need to be queued into the command queue
    void UITree::ScheduleClose(const std::shared_ptr<BaseWindow>& window)
    {
      FSLLOG_WARNING_IF(m_context == Context::InternalLayout, "Windows should not be closed during layout");

      ScopedContextChange scopedContextChange(this, Context::Internal);

      if (!window)
        throw std::invalid_argument("window can not be null");
      if (window == m_root->GetWindow())
        throw UsageErrorException("The root window can not be deleted");

      // Locate the node
      const auto itrNode = m_dict.find(window.get());
      if (itrNode == m_dict.end())
      {
        FSLLOG_WARNING("the window is not part of the tree, request ignored.");
        return;
      }

      m_eventQueue->Push(WindowEventQueueRecord(WindowEventQueueRecordType::ScheduleClose, itrNode->second));
    }


    //! @note all WindowManager modifications to tree need to be queued into the command queue
    void UITree::ScheduleCloseAllChildren(const std::shared_ptr<BaseWindow>& parentWindow)
    {
      FSLLOG_WARNING_IF(m_context == Context::InternalLayout, "Windows should not be closed during layout");

      ScopedContextChange scopedContextChange(this, Context::Internal);

      if (!parentWindow)
        throw std::invalid_argument("window can not be null");

      // Locate the node
      const auto itrNode = m_dict.find(parentWindow.get());
      if (itrNode == m_dict.end())
      {
        FSLLOG_WARNING("the window is not part of the tree, request ignored.");
        return;
      }

      m_eventQueue->Push(WindowEventQueueRecord(WindowEventQueueRecordType::ScheduleCloseAllChildren, itrNode->second));
    }


    bool UITree::TrySetWindowFlags(const BaseWindow*const pWindow, const WindowFlags& flags)
    {
      FSLLOG_WARNING_IF(m_context == Context::InternalLayout, "Windows flags should not be touched during layout");

      ScopedContextChange scopedContextChange(this, Context::Internal);

      if (pWindow == nullptr)
        return false;

      const auto itrNode = m_dict.find(pWindow);
      if (itrNode == m_dict.end())
        return false;

      if (flags.IsEnabled(WindowFlags::LayoutDirty))
      {
        MarkWindowAndParentsAsDirty(itrNode->second);
        m_layoutIsDirty = true;
      }
      if (flags.IsEnabled(WindowFlags::UpdateEnabled))
      {
        itrNode->second->EnableFlags(TreeNodeFlags::UpdateEnabled);
        m_updateCacheDirty = true;
      }
      if (flags.IsEnabled(WindowFlags::DrawEnabled))
      {
        itrNode->second->EnableFlags(TreeNodeFlags::DrawEnabled);
        m_drawCacheDirty = true;
      }
      if (flags.IsEnabled(WindowFlags::ClickInput))
      {
        itrNode->second->EnableFlags(TreeNodeFlags::ClickInput);
        m_clickInputCacheDirty = true;
      }
      return true;
    }


    void UITree::SYS_SetEventSource(WindowEvent*const pEvent, const IWindowId*const pSource)
    {
      const auto itrNode = m_dict.find(pSource);
      if (itrNode == m_dict.end())
        throw std::invalid_argument("The supplied source is not a window known by the window manager");

      pEvent->SYS_SetSource(itrNode->second->GetWindow());
    }


    bool UITree::IsInSystemContext() const
    {
      return m_context == Context::System;
    }


    std::shared_ptr<TreeNode> UITree::TryGet(const IWindowId*const pWindowId) const
    {
      // ScopedContextChange scopedContextChange(this, Context::Internal);  --> Nothing here does callbacks, so no need for a context change
      auto itr = m_dict.find(pWindowId);
      if (itr == m_dict.end())
        return std::shared_ptr<TreeNode>();
      return itr->second;
    }


    std::shared_ptr<TreeNode> UITree::TryGet(const std::shared_ptr<IWindowId>& windowId) const
    {
      if (! windowId)
      {
        FSLLOG_DEBUG_WARNING("Tried to acquire a null window");
        return std::shared_ptr<TreeNode>();
      }
      return Get(windowId.get());
    }

    std::shared_ptr<TreeNode> UITree::Get(const IWindowId*const pWindowId) const
    {
      // ScopedContextChange scopedContextChange(this, Context::Internal);  --> Nothing here does callbacks, so no need for a context change
      auto node = TryGet(pWindowId);
      if (! node)
        throw NotFoundException("pWindowId is not part of the tree");
      return node;
    }


    std::shared_ptr<TreeNode> UITree::Get(const std::shared_ptr<IWindowId>& windowId) const
    {
      if( ! windowId )
        throw std::invalid_argument("windowId can not be null");

      return Get(windowId.get());
    }


    std::shared_ptr<TreeNode> UITree::TryGet(const Vector2& hitPosition) const
    {
      auto itr = m_dequeClickInputTarget.rbegin();
      const auto itrEnd = m_dequeClickInputTarget.rend();
      while (itr != itrEnd)
      {
        if (itr->VisibleRect.Contains(hitPosition))
          return itr->Node;
        ++itr;
      }
      return std::shared_ptr<TreeNode>();
    }


    void UITree::HandleEvent(const std::shared_ptr<TreeNode>& target, const RoutedEvent& routedEvent)
    {
      if (m_context != Context::System)
        throw UsageErrorException("Method called from invalid context");

      // Move to the system level context
      ScopedContextChange scopedContextChange(this, Context::Internal);

      if (!target)
        throw std::invalid_argument("target can not be null");

      const auto itrNode = m_dict.find(target->GetWindow().get());
      if (itrNode == m_dict.end())
        throw UsageErrorException("target is not a member of the tree");

      target->WinHandleEvent(routedEvent);
    }




    void UITree::RegisterEventListener(const std::weak_ptr<IEventListener>& eventListener)
    {
      m_rootWindow->RegisterEventListener(eventListener);
    }


    void UITree::UnregisterEventListener(const std::weak_ptr<IEventListener>& eventListener)
    {
      m_rootWindow->UnregisterEventListener(eventListener);
    }


    void UITree::PerformLayout()
    {
      ScopedContextChange scopedContextChange(this, Context::InternalLayout);

      if (m_layoutIsDirty)
      {
        m_layoutIsDirty = false;
        m_drawCacheDirty = true;
        m_clickInputCacheDirty = true;

        m_rootWindow->Measure(m_rootRect.GetSize());
        m_rootWindow->Arrange(m_rootRect);

        auto& rRootChildren = m_root->m_children;
        for (auto itr = rRootChildren.begin(); itr != rRootChildren.end(); ++itr)
        {
          (*itr)->GetWindow()->Measure(m_rootRect.GetSize());
          (*itr)->GetWindow()->Arrange(m_rootRect);
        }

        RebuildDeques();
      }
    }


    // TODO: we need to use a more complex algorithm for rebuilding
    void UITree::RebuildDeques()
    {
      if (!m_updateCacheDirty && !m_drawCacheDirty && !m_clickInputCacheDirty)
        return;

      m_updateCacheDirty = false;
      m_drawCacheDirty = false;
      m_clickInputCacheDirty = false;

      // Lots of optimization possibilities here
      m_dequeUpdate.clear();
      m_dequeDraw.clear();
      m_dequeClickInputTarget.clear();
      RebuildDeques(m_root, m_rootRect);
    }


    void UITree::RebuildDeques(const std::shared_ptr<TreeNode>& node, const Rect& parentRect)
    {
      Rect currentRect = node->WinGetContentRect();
      currentRect.SetX(currentRect.X() + parentRect.X());
      currentRect.SetY(currentRect.Y() + parentRect.Y());

      const auto flags = node->GetFlags();
      if (flags.IsFlagged(TreeNodeFlags::UpdateEnabled))
        m_dequeUpdate.push_back(node.get());
      if (flags.IsFlagged(TreeNodeFlags::DrawEnabled))
        m_dequeDraw.push_back(UITreeDrawRecord(UIDrawContext(currentRect), node.get()));

      // FIX: once we add clipping support we need to take that into account when storing the click input target rect
      if (flags.IsFlagged(TreeNodeFlags::ClickInput))
        m_dequeClickInputTarget.push_back(UITreeClickInputTargetRecord(currentRect, node));

      auto& nodeChildren = node->m_children;
      for (auto itr = nodeChildren.begin(); itr != nodeChildren.end(); ++itr)
      {
        RebuildDeques(*itr, currentRect);
      }
    }


    void UITree::ProcessEventsPreUpdate()
    {
      // Initial event processing
      ProcessEvents(nullptr);

      // Rebuild the deque's if necessary
      RebuildDeques();
    }


    void UITree::ProcessEventsPostUpdate(const DemoTime& demoTime)
    {
      // process all new events and all new commands (updating newly spawned windows on demand)
      m_nodeScratchpad.clear();
      while (!m_eventQueue->IsEmpty())
      {
        ProcessEvents(&m_nodeScratchpad, TreeNodeFlags::UpdateEnabled);
        // Ensure that all newly allocated windows gets their expected update
        for (auto itr = m_nodeScratchpad.begin(); itr != m_nodeScratchpad.end(); ++itr)
        {
          (*itr)->Update(demoTime);
        }
        m_nodeScratchpad.clear();
      }

      // Rebuild the deque's if necessary
      RebuildDeques();
    }



    void UITree::ProcessEvents(FastTreeNodeDeque* pNewWindows, const TreeNodeFlags& filterFlags)
    {
      ScopedContextChange scopedContextChange(this, Context::System);

      assert(m_eventQueue);
      assert(m_eventRecordQueue);
      assert(m_eventRecordQueue->size() <= 0);

      // Swap the active queue with a empty one
      m_eventQueue->Swap(m_eventRecordQueue);
      // We use two queues to ensure that all events generated by this processing is inserted into their own queue (the new active queue)
      // This can prevent a number of problems and luckily also provides a cleaner design

      // Loop until we reach a empty queue or the max loop count
      std::size_t loopCount = 0;
      while (m_eventRecordQueue->size() > 0 || loopCount > MAX_EVENT_LOOPS)
      {
        while (m_eventRecordQueue->size() > 0)
        {
          const WindowEventQueueRecord entry = m_eventRecordQueue->front();
          m_eventRecordQueue->pop_front();

          switch (entry.Type)
          {
          case WindowEventQueueRecordType::Event:
            SendEvent(entry);
            break;
          case WindowEventQueueRecordType::AddChild:
            m_updateCacheDirty = true;
            m_drawCacheDirty = true;
            m_clickInputCacheDirty = true;
            CommandAddChild(m_moduleCallbackRegistry, entry.Node1, entry.Node2, pNewWindows, filterFlags);
            break;
          case WindowEventQueueRecordType::ScheduleClose:
            m_updateCacheDirty = true;
            m_drawCacheDirty = true;
            m_clickInputCacheDirty = true;
            CommandScheduleClose(m_dict, m_moduleCallbackRegistry, entry.Node1, pNewWindows);
            break;
          case WindowEventQueueRecordType::ScheduleCloseAllChildren:
            m_updateCacheDirty = true;
            m_drawCacheDirty = true;
            m_clickInputCacheDirty = true;
            CommandScheduleCloseChildren(m_dict, m_moduleCallbackRegistry, entry.Node1, pNewWindows);
            break;
          }
        }
        m_eventQueue->Swap(m_eventRecordQueue);
        ++loopCount;
      }

      FSLLOG_WARNING_IF(loopCount > MAX_EVENT_LOOPS, "MaxLoop counter hit during event processing, please check your event sending.");
    }


    void UITree::SendEvent(const WindowEventQueueRecord& eventRecord)
    {
      assert(eventRecord.Type == WindowEventQueueRecordType::Event);
      assert(eventRecord.Event);

      const EventDescription eventDesc = eventRecord.Event->GetDescription();
      EventRoute::StackScopedInit scopedInit(m_eventRoute, eventDesc.RequiredFlags);
      m_eventRoute.SetTarget(eventRecord.Node1, eventDesc.RoutingStrategy);
      m_eventRoute.Send(this, eventRecord.Event);
      m_eventPool->Release(eventRecord.Event);
    }
  }
}
