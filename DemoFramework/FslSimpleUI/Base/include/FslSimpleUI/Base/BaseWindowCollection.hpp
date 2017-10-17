#ifndef FSLSIMPLEUI_BASE_BASEWINDOWCOLLECTION_HPP
#define FSLSIMPLEUI_BASE_BASEWINDOWCOLLECTION_HPP
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

#include <FslBase/Noncopyable.hpp>
#include <deque>
#include <memory>

namespace Fsl
{
  namespace UI
  {
    class BaseWindow;
    class IWindowManager;


    //! @brief
    //! @warning When using this class remember to use WinInit on the owner and call SYS_WinInit
    class BaseWindowCollection : private Noncopyable
    {
    public:
      struct Record
      {
        std::shared_ptr<BaseWindow> Window;
        //bool IsAdded;
        Record(const std::shared_ptr<BaseWindow>& window)
          : Window(window)
          //, IsAdded(false)
        {
        }
      };
    private:
      BaseWindow* m_pOwner;
      std::shared_ptr<IWindowManager> m_windowManager;
      std::deque<Record> m_entries;
    public:
      BaseWindowCollection();
      ~BaseWindowCollection();

      void Clear();
      void Add(const std::shared_ptr<BaseWindow>& window);
      void Remove(const std::shared_ptr<BaseWindow>& window);


      //! @brief Should be called during WinInit of the 'owner' class
      void SYS_WinInit(BaseWindow*const pOwner, const std::shared_ptr<IWindowManager>& windowManager);
    protected:
      const std::deque<Record>& DirectAccess() const { return m_entries; }
      friend class Layout;
    };
  }
}

#endif
