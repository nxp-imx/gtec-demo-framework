#ifndef FSLSIMPLEUI_SYSTEM_EVENT_WINDOWEVENTQUEUERECORD_HPP
#define FSLSIMPLEUI_SYSTEM_EVENT_WINDOWEVENTQUEUERECORD_HPP
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

#include <memory>
#include "WindowEventQueueRecordType.hpp"

namespace Fsl
{
  namespace UI
  {
    class TreeNode;
    class WindowEvent;

    struct WindowEventQueueRecord
    {
      WindowEventQueueRecordType Type;
      std::shared_ptr<TreeNode> Node1;
      std::shared_ptr<TreeNode> Node2;
      std::shared_ptr<WindowEvent> Event;


      WindowEventQueueRecord(const WindowEventQueueRecordType type, const std::shared_ptr<TreeNode>& node)
        : Type(type)
        , Node1(node)
        , Node2()
        , Event()
      {
      }


      WindowEventQueueRecord(const WindowEventQueueRecordType type, const std::shared_ptr<TreeNode>& node1, const std::shared_ptr<TreeNode>& node2)
        : Type(type)
        , Node1(node1)
        , Node2(node2)
        , Event()
      {
      }


      WindowEventQueueRecord(const WindowEventQueueRecordType type, const std::shared_ptr<TreeNode>& source, const std::shared_ptr<TreeNode>& target, const std::shared_ptr<WindowEvent>& theEvent)
        : Type(type)
        , Node1(source)
        , Node2(target)
        , Event(theEvent)
      {
      }
    };
  }
}

#endif
