#ifndef FSLSIMPLEUI_BASE_WINDOWCOLLECTION_GENERICWINDOWCOLLECTION_HPP
#define FSLSIMPLEUI_BASE_WINDOWCOLLECTION_GENERICWINDOWCOLLECTION_HPP
/****************************************************************************************************************************************************
* Copyright 2018 NXP
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
*    * Neither the name of the NXP. nor the names of
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
#include <FslSimpleUI/Base/WindowCollection/WindowCollectionBase.hpp>
#include <algorithm>

namespace Fsl
{
  namespace UI
  {
    struct GenericWindowCollectionRecordBase
    {
      std::shared_ptr<BaseWindow> Window;

      GenericWindowCollectionRecordBase(const std::shared_ptr<BaseWindow>& window)
        : Window(window)
      {
      }
    };

    //! @brief
    //! @warning When using this class remember to use WinInit on the owner and call SYS_WinInit
    template <typename TRecord>
    class GenericWindowCollection : public WindowCollectionBase
    {
      std::deque<TRecord> m_entries;
    public:
      using record_type = TRecord;
      using queue_type = std::deque<record_type>;
      using size_type = typename queue_type::size_type;

      // Make this object non-copyable
      GenericWindowCollection(const GenericWindowCollection&) = delete;
      GenericWindowCollection& operator=(const GenericWindowCollection&) = delete;

      GenericWindowCollection()
        : WindowCollectionBase()
        , m_entries()
      {
      }

      //! @brief Should be called during WinInit of the 'owner' class
      void SYS_WinInit(BaseWindow*const pOwner, const std::shared_ptr<IWindowManager>& windowManager)
      {
        DoInit(pOwner, windowManager);

        for (auto itr = m_entries.begin(); itr != m_entries.end(); ++itr)
          DoAdd(itr->Window);

        DoMarkLayoutDirty();
      }

      void Clear()
      {
        // Remove the entry if found, else quick exit
        if (m_entries.empty())
          return;


        if (IsInitialized())
        {
          for (auto itr = m_entries.begin(); itr != m_entries.end(); ++itr)
            DoScheduleClose(itr->Window);
          DoMarkLayoutDirty();
        }
        m_entries.clear();
      }

      void Add(const std::shared_ptr<BaseWindow>& window)
      {
        if (IsInitialized())
        {
          DoAdd(window);
        }
        else
        {
          // Since we ain't registered with the window manager at this point we need to do basic validation ourself
          if (!window)
            throw std::invalid_argument("window can not be null");


          if (std::find_if(m_entries.begin(), m_entries.end(), [window](const TRecord& record) { return record.Window == window; }) != m_entries.end())
            throw UsageErrorException("Can not add a window twice");
        }

        // We rely on the window managers checks for null and duplicated add's
        m_entries.push_back(TRecord(window));

        if (IsInitialized())
          DoMarkLayoutDirty();
      }


      void Remove(const std::shared_ptr<BaseWindow>& window)
      {
        // Remove the entry if found, else quick exit
        auto itr = std::find_if(m_entries.begin(), m_entries.end(), [window](const TRecord& record) { return record.Window == window; });
        if (itr == m_entries.end())
          return;
        m_entries.erase(itr);

        // Schedule a remove from the window manager
        if (IsInitialized())
        {
          DoScheduleClose(window);
          DoMarkLayoutDirty();
        }
      }


      inline bool empty() const
      {
        return m_entries.empty();
      }

      inline typename queue_type::size_type size() const
      {
        return m_entries.size();
      }

      inline typename queue_type::const_iterator begin() const
      {
        return m_entries.begin();
      }

      inline typename queue_type::iterator begin()
      {
        return m_entries.begin();
      }

      inline typename queue_type::const_iterator end() const
      {
        return m_entries.end();
      }

      inline typename queue_type::iterator end()
      {
        return m_entries.end();
      }
    };
  }
}

#endif
