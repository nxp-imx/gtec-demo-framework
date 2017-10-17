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

#include <FslSimpleUI/Base/BaseWindowCollection.hpp>
#include <FslSimpleUI/Base/BaseWindow.hpp>
#include <FslSimpleUI/Base/IWindowManager.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <cassert>

namespace Fsl
{
  namespace UI
  {
    namespace
    {
      struct RecordWindowComp
      {
        const std::shared_ptr<BaseWindow> Key;
        explicit RecordWindowComp(const std::shared_ptr<BaseWindow>& key)
          : Key(key)
        {
        }
        inline bool operator()(const BaseWindowCollection::Record& record) const { return record.Window == Key; }
      };
    }

    BaseWindowCollection::BaseWindowCollection()
      : m_pOwner(nullptr)
      , m_windowManager()
      , m_entries()
    {
    }


    BaseWindowCollection::~BaseWindowCollection()
    {

    }


    void BaseWindowCollection::SYS_WinInit(BaseWindow*const pOwner, const std::shared_ptr<IWindowManager>& windowManager)
    {
      // Initialize should not have been called
      assert(m_pOwner == nullptr);
      assert(!m_windowManager);

      if (!pOwner)
        throw std::invalid_argument("pOwner can not be null");
      if (!windowManager)
        throw std::invalid_argument("windowManager can not be null");

      m_pOwner = pOwner;
      m_windowManager = windowManager;
      for (auto itr = m_entries.begin(); itr != m_entries.end(); ++itr)
        m_windowManager->AddChild(m_pOwner, itr->Window);

      m_pOwner->SetLayoutDirty(true);
    }


    void BaseWindowCollection::Clear()
    {
      // Remove the entry if found, else quick exit
      if (m_entries.size() == 0)
        return;

      if (m_windowManager)
      {
        for (auto itr = m_entries.begin(); itr != m_entries.end(); ++itr)
        {
          m_windowManager->ScheduleClose(itr->Window);
        }
        m_pOwner->SetLayoutDirty(true);
      }
      m_entries.clear();
    }



    void BaseWindowCollection::Add(const std::shared_ptr<BaseWindow>& window)
    {
      if (m_windowManager)
      {
        m_windowManager->AddChild(m_pOwner, window);
      }
      else
      {
        // Since we ain't registered with the window manager at this point we need to do basic validation ourself
        if (!window)
          throw std::invalid_argument("window can not be null");

        if (std::find_if(m_entries.begin(), m_entries.end(), RecordWindowComp(window)) != m_entries.end())
          throw UsageErrorException("Can not add a window twice");
      }

      // We rely on the window managers checks for null and duplicated add's
      assert(window);
      m_entries.push_back(Record(window));

      if (m_pOwner != nullptr)
        m_pOwner->SetLayoutDirty(true);
    }


    void BaseWindowCollection::Remove(const std::shared_ptr<BaseWindow>& window)
    {
      // Remove the entry if found, else quick exit
      {
        auto itr = std::find_if(m_entries.begin(), m_entries.end(), RecordWindowComp(window));
        if (itr == m_entries.end())
          return;
        m_entries.erase(itr);
      }

      if (m_windowManager)
      {
        m_windowManager->ScheduleClose(window);
        m_pOwner->SetLayoutDirty(true);
      }
    }
  }
}
