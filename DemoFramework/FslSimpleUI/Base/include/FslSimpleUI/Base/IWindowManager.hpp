#ifndef FSLSIMPLEUI_BASE_IWINDOWMANAGER_HPP
#define FSLSIMPLEUI_BASE_IWINDOWMANAGER_HPP
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

#include <FslSimpleUI/Base/IWindowInfo.hpp>
#include <memory>

namespace Fsl
{
  namespace UI
  {

    class BaseWindow;
    class WindowEvent;
    struct WindowFlags;

    class IWindowManager : public IWindowInfo
    {
    public:
      virtual ~IWindowManager() {};

      //! @brief Add the given window as a child of the root window
      //! @warning There can only be one instance of a given window object in the list.
      //! @param window The window to add (!=null). The window can not exist in the tree already. If the window is being added to a parent that is
      //                in the closing, closed or crashed state the supplied window will be marked as closing.
      //! @throws std::invalid_agrument if 'window' is null.
      //! @throws UsageErrorException if 'window' already exist.
      //! @throws UsageErrorException if 'window' is marked as disposed.
      virtual void Add(const std::shared_ptr<BaseWindow>& window) = 0;

      //! @brief Add the given window as a child of the given parent window
      //! @warning There can only be one instance of a given window object in the list.
      //! @param parentWindow The parent window (A null pointer is considered the current context window), the parent must exist in the tree.
      //! @param window The window to add (!=null). The window can not exist in the tree already. If the window is being added to a parent that is
      //                in the closing, closed or crashed state the supplied window will be marked as closing.
      //! @throws std::invalid_agrument if 'parentWindow' is null.
      //! @throws UsageErrorException if the parent isn't part of the contexts window tree.
      //! @throws std::invalid_agrument if 'window' is null.
      //! @throws UsageErrorException if 'window' already exist.
      //! @throws UsageErrorException if 'window' is marked as disposed.
      virtual void AddChild(const BaseWindow*const parentWindow, const std::shared_ptr<BaseWindow>& window) = 0;
      virtual void AddChild(const std::shared_ptr<BaseWindow>& parentWindow, const std::shared_ptr<BaseWindow>& window) = 0;

      //! @brief Check if the given window exist.
      //! @param window">The window to check (null never exist)  </param>
      //! @return true if the window exists (windows that are scheduled for close but not yet closed will also return true).
      virtual bool Exists(const std::shared_ptr<BaseWindow>& window) const = 0;

      //! @brief Check if the window is a member of the tree.
      //! @param tree the tree root window
      //! @param window a window
      //! @return true if the window is part of the tree (note the tree node is considered part of the tree)
      virtual bool IsMemberOfTree(const std::shared_ptr<BaseWindow>& tree, const std::shared_ptr<BaseWindow>& window) const = 0;

      //! @brief Check if the window is a member of the tree.
      //! @param tree the tree root window.
      //! @param window a window.
      //! @param considerTreeRootAMember f false the tree node is not considered part of the tree.
      //! @return true if the window is part of the tree (note the tree node is considered part of the tree).
      virtual bool IsMemberOfTree(const std::shared_ptr<BaseWindow>& tree, const std::shared_ptr<BaseWindow>& window, const bool considerTreeRootAMember) const = 0;

      //! @brief Close the supplied window.
      //! @note Closing a window will also schedule a close of all its children!
      //!        If the window doesn't exist nothing happens, but a warning will logged in debug builds.
      //! @param window the window that should be closed (if null, the current context will be closed).
      //! @throws UsageErrorException if the window isn't part of the contexts window tree.
      virtual void ScheduleClose(const std::shared_ptr<BaseWindow>& window) = 0;

      //! Schedule all child windows of the given window to be closed.
      //! @note If the window doesnt exist a warning will be logged in debug builds.
      //! @param parentWindow The window for which to close all children.
      //! @throws std::invalid_agrument if 'window' is null.
      //! @throws UsageErrorException if the parentWin isn't part of the contexts window tree.
      virtual void ScheduleCloseAllChildren(const std::shared_ptr<BaseWindow>& parentWindow) = 0;

      //! @brief Set window flags
      virtual bool TrySetWindowFlags(const BaseWindow*const pWindow, const WindowFlags& flags) = 0;


      //! @brief Set the event source of this event
      virtual void SYS_SetEventSource(WindowEvent*const pEvent, const IWindowId*const pSource) = 0;
    };
  }
}

#endif
