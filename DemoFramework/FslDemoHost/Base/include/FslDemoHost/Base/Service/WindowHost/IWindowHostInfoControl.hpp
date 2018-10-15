#ifndef FSLDEMOHOST_BASE_SERVICE_WINDOWHOST_IWINDOWHOSTINFOCONTROL_HPP
#define FSLDEMOHOST_BASE_SERVICE_WINDOWHOST_IWINDOWHOSTINFOCONTROL_HPP
/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <memory>

namespace Fsl
{
  class INativeWindow;
  class INativeWindowSystem;

  class IWindowHostInfoControl
  {
  public:
    virtual ~IWindowHostInfoControl() = default;

    //! @brief It's not strictly necessary to call this as we use weak pointers,
    //! but calling it ensures that nobody can query the system after its been cleared
    virtual void ClearWindowSystem() = 0;
    virtual void SetWindowSystem(const std::weak_ptr<INativeWindowSystem>& windowSystem) = 0;
    virtual void AddWindow(const std::weak_ptr<INativeWindow>& window) = 0;

    //! @brief It's not strictly necessary to call this as we use weak pointers,
    //!        but calling it ensures that nobody can query the window after its been removed.
    //!        However people with a active pointer can ofcause still use it.
    virtual void RemoveWindow(const std::weak_ptr<INativeWindow>& window) = 0;
  };
}

#endif
