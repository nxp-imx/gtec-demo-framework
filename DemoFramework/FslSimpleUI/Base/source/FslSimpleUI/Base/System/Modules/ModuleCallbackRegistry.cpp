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

#include "ModuleCallbackRegistry.hpp"
#include <FslBase/Exceptions.hpp>
#include <algorithm>
#include <cassert>
#include <utility>
#include "../TreeNode.hpp"
#include "IModuleCallbackReceiver.hpp"

namespace Fsl::UI
{
  namespace
  {
    struct ReceiverComp
    {
      const std::shared_ptr<IModuleCallbackReceiver> Key;
      explicit ReceiverComp(std::shared_ptr<IModuleCallbackReceiver> key)
        : Key(std::move(key))
      {
      }
      inline bool operator()(const std::weak_ptr<IModuleCallbackReceiver>& record) const
      {
        return record.lock() == Key;
      }
    };
  }


  ModuleCallbackRegistry::ModuleCallbackRegistry() = default;


  ModuleCallbackRegistry::~ModuleCallbackRegistry() = default;


  void ModuleCallbackRegistry::ModuleOnTreeNodeAdd(const std::shared_ptr<TreeNode>& node)
  {
    assert(node);
    auto itr = m_receivers.begin();
    while (itr != m_receivers.end())
    {
      std::shared_ptr<IModuleCallbackReceiver> receiver = itr->lock();
      if (receiver)
      {
        receiver->ModuleOnTreeNodeAdd(node);
        ++itr;
      }
      else
      {
        itr = m_receivers.erase(itr);
      }
    }
  }


  void ModuleCallbackRegistry::ModuleOnTreeNodeDispose(const std::shared_ptr<TreeNode>& node)
  {
    assert(node);
    auto itr = m_receivers.begin();
    while (itr != m_receivers.end())
    {
      std::shared_ptr<IModuleCallbackReceiver> receiver = itr->lock();
      if (receiver)
      {
        receiver->ModuleOnTreeNodeDispose(node);
        ++itr;
      }
      else
      {
        itr = m_receivers.erase(itr);
      }
    }
  }


  void ModuleCallbackRegistry::AddCallbackReceiver(const std::weak_ptr<IModuleCallbackReceiver>& module)
  {
    m_receivers.push_back(module);
  }


  void ModuleCallbackRegistry::RemoveCallbackReceiver(const std::weak_ptr<IModuleCallbackReceiver>& module)
  {
    auto entry = module.lock();
    auto itr = std::find_if(m_receivers.begin(), m_receivers.end(), ReceiverComp(entry));
    if (itr == m_receivers.end())
    {
      return;
    }
    m_receivers.erase(itr);
  }
}
