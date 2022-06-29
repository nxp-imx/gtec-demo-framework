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

#include <FslBase/Exceptions.hpp>
#include <FslSimpleUI/Base/Event/WindowEventSender.hpp>
#include <cassert>
#include <utility>
#include "../System/Event/WindowEventQueue.hpp"
#include "../System/ITreeNodeLocator.hpp"

namespace Fsl::UI
{
  WindowEventSender::WindowEventSender(std::shared_ptr<WindowEventQueue> windowEventQueue, std::shared_ptr<WindowEventPool> eventPool,
                                       std::shared_ptr<ITreeNodeLocator> treeNodeLocator)
    : m_windowEventQueue(std::move(windowEventQueue))
    , m_treeNodeLocator(std::move(treeNodeLocator))
    , EventPool(std::move(eventPool))
  {
  }


  WindowEventSender::~WindowEventSender() = default;


  void WindowEventSender::SendEvent(const std::shared_ptr<WindowEvent>& theEvent, const IWindowId* const pSource)
  {
    if (!theEvent)
    {
      throw std::invalid_argument("theEvent can not be null");
    }
    if (pSource == nullptr)
    {
      throw std::invalid_argument("pSource can not be null");
    }

    m_windowEventQueue->Push(theEvent, m_treeNodeLocator->Get(pSource));
  }


  void WindowEventSender::SendEvent(const std::shared_ptr<WindowEvent>& theEvent, const std::shared_ptr<IWindowId>& source)
  {
    if (!theEvent)
    {
      throw std::invalid_argument("theEvent can not be null");
    }
    if (!source)
    {
      throw std::invalid_argument("source can not be null");
    }

    m_windowEventQueue->Push(theEvent, m_treeNodeLocator->Get(source));
  }


  // bool WindowEventSender::TrySendEvent(const std::shared_ptr<WindowEvent>& theEvent, const IWindowId*const pSource)
  //{
  //  if (!theEvent)
  //    return false;
  //  if (pSource == nullptr)
  //    return false;

  //  auto node = m_treeNodeLocator->TryGet(pSource);
  //  if (!node)
  //    return false;

  //  return m_windowEventQueue->TryPush(theEvent, node);
  //}


  // bool WindowEventSender::TrySendEvent(const std::shared_ptr<WindowEvent>& theEvent, const std::shared_ptr<IWindowId>& source)
  //{
  //  if (!theEvent)
  //    return false;
  //  if (!source)
  //    return false;

  //  auto node = m_treeNodeLocator->TryGet(source);
  //  if (!node)
  //    return false;

  //  return m_windowEventQueue->TryPush(theEvent, node);
  //}
}
