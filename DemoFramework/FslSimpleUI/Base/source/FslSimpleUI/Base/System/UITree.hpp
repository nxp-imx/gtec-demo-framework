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
#include <vector>
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

    using WindowToNodeMap = std::map<const IWindowId*, std::shared_ptr<TreeNode>>;
    using FastTreeNodeVector = std::vector<TreeNode*>;

    struct UITreeDrawRecord
    {
      UIDrawContext DrawContext;
      TreeNode* pNode{nullptr};

      UITreeDrawRecord() = default;

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

      UITreeClickInputTargetRecord() = default;

      UITreeClickInputTargetRecord(const Rect& visibleRect, const std::shared_ptr<TreeNode>& node)
        : VisibleRect(visibleRect)
        , Node(node)
      {
      }
    };

    using UITreeDrawVector = std::vector<UITreeDrawRecord>;
    using UITreeClickInputTargetVector = std::vector<UITreeClickInputTargetRecord>;


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
        Internal,
        InternalLayout
      };
      enum class State
      {
        WaitForInit,
        Ready,
        Shutdown,
      };

      State m_state = State::WaitForInit;

      const std::shared_ptr<ModuleCallbackRegistry> m_moduleCallbackRegistry;
      const std::shared_ptr<WindowEventPool> m_eventPool;
      //! the command queue that ensure we are able to modify the tree at all times without causing conflicts
      const std::shared_ptr<WindowEventQueueEx> m_eventQueue;
      std::unique_ptr<std::deque<WindowEventQueueRecord>> m_eventRecordQueue;
      EventRoute m_eventRoute;

      //! quick window -> node lookup
      WindowToNodeMap m_dict;

      //! the root window
      std::shared_ptr<RootWindow> m_rootWindow;
      //! the root node
      std::shared_ptr<TreeNode> m_root;
      Rect m_rootRect;

      bool m_updateCacheDirty;
      bool m_resolveCacheDirty;
      bool m_drawCacheDirty;
      bool m_clickInputCacheDirty;
      bool m_layoutIsDirty;


      FastTreeNodeVector m_vectorUpdate;
      FastTreeNodeVector m_vectorResolve;
      UITreeDrawVector m_vectorDraw;
      UITreeClickInputTargetVector m_vectorClickInputTarget;

      FastTreeNodeVector m_nodeScratchpad;
      FastTreeNodeVector m_nodeScratchpadPostResolve;
      mutable Context m_context;

    public:
      UITree(const std::shared_ptr<ModuleCallbackRegistry>& moduleCallbackRegistry, const std::shared_ptr<WindowEventPool>& eventPool,
             const std::shared_ptr<WindowEventQueueEx>& eventQueue);
      ~UITree() override;

      void Init(const std::shared_ptr<RootWindow>& rootWindow);
      void Shutdown();
      void ScheduleCloseAll();

      void ProcessEvents();
      void Resized(const Vector2& size);
      void Update(const DemoTime& demoTime);
      void Draw();

      std::size_t GetNodeCount() const;

      std::shared_ptr<TreeNode> GetRootNode() const
      {
        return m_root;
      }

      // From IWindowInfo
      Vector2 PointToScreen(const IWindowId* const pWindow, const Vector2& point) const override;
      Vector2 PointFromScreen(const IWindowId* const pWindow, const Vector2& point) const override;

      // From IWindowManager
      void Add(const std::shared_ptr<BaseWindow>& window) override;
      void AddChild(const BaseWindow* const parentWindow, const std::shared_ptr<BaseWindow>& window) override;
      void AddChild(const std::shared_ptr<BaseWindow>& parentWindow, const std::shared_ptr<BaseWindow>& window) override;
      bool Exists(const BaseWindow* const pWindow) const override;
      bool Exists(const std::shared_ptr<BaseWindow>& window) const override;
      bool IsMemberOfTree(const std::shared_ptr<BaseWindow>& tree, const std::shared_ptr<BaseWindow>& window) const override;
      bool IsMemberOfTree(const std::shared_ptr<BaseWindow>& tree, const std::shared_ptr<BaseWindow>& window,
                          const bool considerTreeRootAMember) const override;
      void ScheduleClose(const std::shared_ptr<BaseWindow>& window) override;
      void ScheduleCloseAllChildren(const std::shared_ptr<BaseWindow>& parentWindow) override;
      bool TrySetWindowFlags(const BaseWindow* const pWindow, const WindowFlags& flags) override;
      void SYS_SetEventSource(WindowEvent* const pEvent, const IWindowId* const pSource) override;

      // From ITreeContextInfo
      bool IsInSystemContext() const override;

      // From ITreeNodeLocator
      std::shared_ptr<TreeNode> Get(const IWindowId* const pWindowId) const override;
      std::shared_ptr<TreeNode> Get(const std::shared_ptr<IWindowId>& windowId) const override;
      std::shared_ptr<TreeNode> TryGet(const IWindowId* const pWindowId) const override;
      std::shared_ptr<TreeNode> TryGet(const std::shared_ptr<IWindowId>& windowId) const override;

      // From ITreeNodeClickInputTargetLocator
      std::shared_ptr<TreeNode> TryGet(const Vector2& hitPosition) const override;

      // From IEventHandler
      void HandleEvent(const std::shared_ptr<TreeNode>& target, const RoutedEvent& routedEvent) override;

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
      inline void ProcessEventsPostResolve(const DemoTime& demoTime);

      //! @param pNewWindows if not null this should be filled with any new nodes that are spawned by processing
      //! @param filterFlags any node that has any of the supplied flags will be added to pNewWindows
      void ProcessEvents(FastTreeNodeVector* pNewWindows, const TreeNodeFlags& filterFlags = TreeNodeFlags());
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
