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

#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxRectangle.hpp>
#include <FslBase/Math/Rect.hpp>
#include <FslSimpleUI/Base/UIStats.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslSimpleUI/Base/UIDrawContext.hpp>
#include <deque>
#include <map>
#include <memory>
#include <utility>
#include <vector>
#include "ITreeContextInfo.hpp"
#include "ITreeNodeLocator.hpp"
#include "ITreeNodeClickInputTargetLocater.hpp"
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

    struct UITreeInputTargetRecord
    {
      PxRectangle VisibleRectPx;
      std::shared_ptr<TreeNode> Node;

      UITreeInputTargetRecord() = default;

      UITreeInputTargetRecord(const PxRectangle& visibleRectPx, std::shared_ptr<TreeNode> node)
        : VisibleRectPx(visibleRectPx)
        , Node(std::move(node))
      {
      }
    };

    using UITreeDrawVector = std::vector<UITreeDrawRecord>;


    //! @note This tree is designed with the assumption that windows will NOT be reused.
    class UITree final
      : public IWindowManager
      , public ITreeContextInfo
      , public ITreeNodeLocator
      , public ITreeNodeClickInputTargetLocater
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
      PxRectangle m_rootRectPx;

      bool m_updateCacheDirty;
      bool m_resolveCacheDirty;
      bool m_drawCacheDirty;
      bool m_clickInputCacheDirty;
      bool m_layoutIsDirty;


      FastTreeNodeVector m_vectorUpdate;
      FastTreeNodeVector m_vectorResolve;
      UITreeDrawVector m_vectorDraw;
      std::vector<UITreeInputTargetRecord> m_vectorClickInputTarget;
      std::vector<UITreeInputTargetRecord> m_vectorMouseOverTarget;

      FastTreeNodeVector m_nodeScratchpad;
      FastTreeNodeVector m_nodeScratchpadPostResolve;
      mutable Context m_context;

      UIStats m_stats;

    public:
      UITree(std::shared_ptr<ModuleCallbackRegistry> moduleCallbackRegistry, std::shared_ptr<WindowEventPool> eventPool,
             std::shared_ptr<WindowEventQueueEx> eventQueue);
      ~UITree() final;

      void Init(const std::shared_ptr<RootWindow>& rootWindow);
      void Shutdown();
      void ScheduleCloseAll();

      //! Check if the tree is considered idle
      //! NOTE: Basically checks that there are now pending work scheduled and that no child element expects updates
      bool IsIdle() const;

      UIStats GetStats() const
      {
        return m_stats;
      }

      void ProcessEvents();
      void Resized(const PxExtent2D& extentPx, const uint32_t densityDpi);
      void Update(const DemoTime& demoTime);
      void Draw();


      std::size_t GetNodeCount() const;

      std::shared_ptr<TreeNode> GetRootNode() const
      {
        return m_root;
      }

      // From IWindowInfo
      PxPoint2 PointToScreen(const IWindowId* const pWindow, const PxPoint2& point) const final;
      PxPoint2 PointFromScreen(const IWindowId* const pWindow, const PxPoint2& point) const final;

      // From IWindowManager
      void Add(const std::shared_ptr<BaseWindow>& window) final;
      void AddChild(const BaseWindow* const parentWindow, const std::shared_ptr<BaseWindow>& window) final;
      void AddChild(const std::shared_ptr<BaseWindow>& parentWindow, const std::shared_ptr<BaseWindow>& window) final;
      bool Exists(const BaseWindow* const pWindow) const final;
      bool Exists(const std::shared_ptr<BaseWindow>& window) const final;
      bool IsMemberOfTree(const std::shared_ptr<BaseWindow>& tree, const std::shared_ptr<BaseWindow>& window) const final;
      bool IsMemberOfTree(const std::shared_ptr<BaseWindow>& tree, const std::shared_ptr<BaseWindow>& window,
                          const bool considerTreeRootAMember) const final;
      void ScheduleClose(const std::shared_ptr<BaseWindow>& window) final;
      void ScheduleCloseAllChildren(const std::shared_ptr<BaseWindow>& parentWindow) final;
      bool TrySetWindowFlags(const BaseWindow* const pWindow, const WindowFlags& flags, const bool enable) final;
      void SYS_SetEventSource(WindowEvent* const pEvent, const IWindowId* const pSource) final;

      // From ITreeContextInfo
      bool IsInSystemContext() const final;

      // From ITreeNodeLocator
      std::shared_ptr<TreeNode> Get(const IWindowId* const pWindowId) const final;
      std::shared_ptr<TreeNode> Get(const std::shared_ptr<IWindowId>& windowId) const final;
      std::shared_ptr<TreeNode> TryGet(const IWindowId* const pWindowId) const final;
      std::shared_ptr<TreeNode> TryGet(const std::shared_ptr<IWindowId>& windowId) const final;

      // From ITreeNodeClickInputTargetLocater
      std::shared_ptr<TreeNode> TryGetMouseOverWindow(const PxPoint2& hitPositionPx) const final;
      std::shared_ptr<TreeNode> TryGetClickInputWindow(const PxPoint2& hitPositionPx) const final;

      // From IEventHandler
      void HandleEvent(const std::shared_ptr<TreeNode>& target, const RoutedEvent& routedEvent) final;

      //! @brief Register a event listener
      void RegisterEventListener(const std::weak_ptr<IEventListener>& eventListener);
      //! @brief Unregister a event listener
      void UnregisterEventListener(const std::weak_ptr<IEventListener>& eventListener);

    private:
      inline void PerformLayout();
      inline void RebuildDeques();
      void RebuildDeques(const std::shared_ptr<TreeNode>& node, const PxRectangle& parentRectPx);
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
