#ifndef FSLSIMPLEUI_BASE_SYSTEM_UITREE_HPP
#define FSLSIMPLEUI_BASE_SYSTEM_UITREE_HPP
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

#include <FslBase/Math/Rect.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <deque>
#include <map>
#include <memory>
#include "ITreeContextInfo.hpp"
#include "ITreeNodeLocator.hpp"
#include "ITreeNodeClickInputTargetLocator.hpp"
#include "Event/IEventHandler.hpp"
#include "Event/WindowEventQueueRecord.hpp"
#include "Event/EventRoute.hpp"
#include "TreeNodeFlags.hpp"

namespace Fsl
{
  struct DemoTime;
  struct Point2;

  namespace UI
  {
    class IEventListener;
    class ModuleCallbackRegistry;
    class RootWindow;
    class TreeNode;
    class WindowEventPool;
    class WindowEventQueueEx;

    typedef std::map<const IWindowId*, std::shared_ptr<TreeNode> > WindowToNodeMap;
    typedef std::deque<TreeNode*> FastTreeNodeDeque;

    struct UITreeDrawRecord
    {
      UIDrawContext DrawContext;
      TreeNode* pNode;

      UITreeDrawRecord()
        : DrawContext()
        , pNode(nullptr)
      {
      }

      UITreeDrawRecord(const UIDrawContext& drawContext, TreeNode* pTheNode)
        : DrawContext(drawContext)
        , pNode(pTheNode)
      {
      }
    };

    struct UITreeClickInputTargetRecord
    {
      Rect VisibleRect;
      std::shared_ptr<TreeNode> Node;

      UITreeClickInputTargetRecord()
        : VisibleRect()
        , Node()
      {
      }

      UITreeClickInputTargetRecord(const Rect& visibleRect, const std::shared_ptr<TreeNode>& node)
        : VisibleRect(visibleRect)
        , Node(node)
      {
      }
    };

    typedef std::deque<UITreeDrawRecord> UITreeDrawDeque;
    typedef std::deque<UITreeClickInputTargetRecord> UITreeClickInputTargetDeque;


    //! @note This tree is designed with the assumption that windows will NOT be reused.
    class UITree
      : public IWindowManager
      , public ITreeContextInfo
      , public ITreeNodeLocator
      , public ITreeNodeClickInputTargetLocator
      , public IEventHandler
    {
      enum class Context
      {
        System,
        Internal
      };

      const std::shared_ptr<ModuleCallbackRegistry> m_moduleCallbackRegistry;
      const std::shared_ptr<WindowEventPool> m_eventPool;
      //! the command queue that ensure we are able to modify the tree at all times without causing conflicts
      const std::shared_ptr<WindowEventQueueEx> m_eventQueue;
      std::unique_ptr<std::deque<WindowEventQueueRecord> > m_eventRecordQueue;
      EventRoute m_eventRoute;

      //! quick window -> node lookup
      WindowToNodeMap m_dict;

      //! the root window
      std::shared_ptr<RootWindow> m_rootWindow;
      //! the root node
      std::shared_ptr<TreeNode> m_root;
      Rect m_rootRect;

      bool m_updateCacheDirty;
      bool m_drawCacheDirty;
      bool m_clickInputCacheDirty;
      bool m_layoutIsDirty;

      FastTreeNodeDeque m_dequeUpdate;
      UITreeDrawDeque m_dequeDraw;
      UITreeClickInputTargetDeque m_dequeClickInputTarget;

      FastTreeNodeDeque m_nodeScratchpad;
      mutable Context m_context;
    public:
      UITree(const std::shared_ptr<ModuleCallbackRegistry>& moduleCallbackRegistry, const std::shared_ptr<WindowEventPool>& eventPool, const std::shared_ptr<WindowEventQueueEx>& eventQueue, const Vector2& size);
      ~UITree();

      void Resized(const Vector2& size);
      void Update(const DemoTime& demoTime);
      void Draw();

      std::shared_ptr<TreeNode> GetRootNode() const { return m_root; }

      // From IWindowInfo
      virtual Vector2 PointToScreen(const IWindowId*const pWindow, const Vector2& point) const override;
      virtual Vector2 PointFromScreen(const IWindowId*const pWindow, const Vector2& point) const override;

      // From IWindowManager
      virtual void Add(const std::shared_ptr<BaseWindow>& window) override;
      virtual void AddChild(const BaseWindow*const parentWindow, const std::shared_ptr<BaseWindow>& window) override;
      virtual void AddChild(const std::shared_ptr<BaseWindow>& parentWindow, const std::shared_ptr<BaseWindow>& window) override;
      virtual bool Exists(const std::shared_ptr<BaseWindow>& window) const override;
      virtual bool IsMemberOfTree(const std::shared_ptr<BaseWindow>& tree, const std::shared_ptr<BaseWindow>& window) const override;
      virtual bool IsMemberOfTree(const std::shared_ptr<BaseWindow>& tree, const std::shared_ptr<BaseWindow>& window, const bool considerTreeRootAMember) const override;
      virtual void ScheduleClose(const std::shared_ptr<BaseWindow>& window) override;
      virtual void ScheduleCloseAllChildren(const std::shared_ptr<BaseWindow>& parentWindow) override;
      virtual bool TrySetWindowFlags(const BaseWindow*const pWindow, const WindowFlags& flags) override;
      virtual void SYS_SetEventSource(WindowEvent*const pEvent, const IWindowId*const pSource) override;

      // From ITreeContextInfo
      virtual bool IsInSystemContext() const override;

      // From ITreeNodeLocator
      virtual std::shared_ptr<TreeNode> Get(const IWindowId*const pWindowId) const override;
      virtual std::shared_ptr<TreeNode> Get(const std::shared_ptr<IWindowId>& windowId) const override;

      // From ITreeNodeClickInputTargetLocator
      virtual std::shared_ptr<TreeNode> TryGet(const Vector2& hitPosition) const override;

      // From IEventHandler
      virtual void HandleEvent(const std::shared_ptr<TreeNode>& target, const RoutedEvent& routedEvent) override;

      //! @brief Register a event listener
      void RegisterEventListener(const std::weak_ptr<IEventListener>& eventListener);
      //! @brief Unregister a event listener
      void UnregisterEventListener(const std::weak_ptr<IEventListener>& eventListener);
    private:
      inline void PerformLayout();
      inline void RebuildDeques();
      void RebuildDeques(const std::shared_ptr<TreeNode>& node, const Rect& parentRect);
      inline void ProcessEventsPreUpdate();
      inline void ProcessEventsPostUpdate(const DemoTime& demoTime);

      //! @param pNewWindows if not null this should be filled with any new nodes that are spawned by processing
      void ProcessEvents(FastTreeNodeDeque* pNewWindows, const TreeNodeFlags& filterFlags = TreeNodeFlags());
      void SendEvent(const WindowEventQueueRecord& eventRecord);

      class ScopedContextChange
      {
        const UITree* m_pThis;
        Context m_oldContext;
      public:
        inline ScopedContextChange(const UITree* pThis, const Context newContext)
          : m_pThis(pThis)
          , m_oldContext(pThis->m_context)
        {
          m_pThis->m_context = newContext;
        }

        inline ~ScopedContextChange()
        {
          m_pThis->m_context = m_oldContext;
        }
      };
    };
  }
}

#endif
