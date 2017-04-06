#ifndef FSLSIMPLEUI_EVENT_WINDOWCONTENTCHANGEDEVENT_HPP
#define FSLSIMPLEUI_EVENT_WINDOWCONTENTCHANGEDEVENT_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/ITag.hpp>
#include <FslSimpleUI/Event/WindowEvent.hpp>

namespace Fsl
{
  namespace UI
  {
    class WindowContentChangedEvent : public WindowEvent
    {
      uint32_t m_contentId;
      int32_t m_param1;
      int32_t m_param2;
    public:
      WindowContentChangedEvent();

      uint32_t GetContentId() const;
      int32_t GetParam1() const;
      int32_t GetParam2() const;
    protected:

      void SYS_Construct(const uint32_t contentId, const int32_t param1, const int32_t param2)
      {
        WindowEvent::SYS_DoConstruct();
        m_contentId = contentId;
        m_param1 = param1;
        m_param2 = param2;
      }

      virtual void SYS_Destruct()
      {
        m_contentId = 0;
        m_param1 = 0;
        m_param2 = 0;
        WindowEvent::SYS_Destruct();
      }

      friend class WindowEventPool;
    };
  }
}

#endif
