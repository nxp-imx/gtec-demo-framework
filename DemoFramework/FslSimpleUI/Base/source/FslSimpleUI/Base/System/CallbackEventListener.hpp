#ifndef FSLSIMPLEUI_BASE_SYSTEM_CALLBACKEVENTLISTENER_HPP
#define FSLSIMPLEUI_BASE_SYSTEM_CALLBACKEVENTLISTENER_HPP
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

#include <FslSimpleUI/Base/System/IEventListener.hpp>

namespace Fsl::UI
{
  //! @brief This should only be used in combination with the CallbackEventListener scope
  class CallbackEventListener final : public IEventListener
  {
    IEventListener* m_callback;

  public:
    ~CallbackEventListener() override
    {
      m_callback = nullptr;
    }

    void OnClickInputPreview(const std::shared_ptr<WindowInputClickEvent>& theEvent) final
    {
      if (m_callback != nullptr)
      {
        m_callback->OnClickInputPreview(theEvent);
      }
    }


    void OnClickInput(const std::shared_ptr<WindowInputClickEvent>& theEvent) final
    {
      if (m_callback != nullptr)
      {
        m_callback->OnClickInput(theEvent);
      }
    }

    void OnMouseOverPreview(const std::shared_ptr<WindowMouseOverEvent>& theEvent) final
    {
      if (m_callback != nullptr)
      {
        m_callback->OnMouseOverPreview(theEvent);
      }
    }

    void OnMouseOver(const std::shared_ptr<WindowMouseOverEvent>& theEvent) final
    {
      if (m_callback != nullptr)
      {
        m_callback->OnMouseOver(theEvent);
      }
    }


    void OnSelect(const std::shared_ptr<WindowSelectEvent>& theEvent) final
    {
      if (m_callback != nullptr)
      {
        m_callback->OnSelect(theEvent);
      }
    }


    void OnContentChanged(const std::shared_ptr<WindowContentChangedEvent>& theEvent) final
    {
      if (m_callback != nullptr)
      {
        m_callback->OnContentChanged(theEvent);
      }
    }


  private:
    explicit CallbackEventListener(IEventListener* const pForwardTo)
      : m_callback(pForwardTo)
    {
    }


    void Dispose()
    {
      m_callback = nullptr;
    }

    friend class CallbackEventListenerScope;
  };
}

#endif
