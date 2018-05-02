#ifndef FSLDEMOHOST_BASE_IDEMOHOST_HPP
#define FSLDEMOHOST_BASE_IDEMOHOST_HPP
/****************************************************************************************************************************************************
* Copyright (c) 2014 Freescale Semiconductor, Inc.
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
#include <FslBase/Math/Point2.hpp>
#include <FslDemoApp/Shared/Host/DemoHostFeature.hpp>

namespace Fsl
{
  class IDemoHost : public Noncopyable
  {
  public:
    virtual ~IDemoHost() {};


    //! @brief Called just after the demo host have been constructed to allow it to do a bit of post processing on a valid host object
    virtual void OnConstructed() = 0;
    //! @brief Called when the demo host is activated (the currently active host)
    virtual void OnActivate() = 0;
    //! @brief Called when the demo host is deactivated
    virtual void OnDeactivate() = 0;
    //! @brief Called when the demo host is suspended
    virtual void OnSuspend() = 0;
    //! @brief Called when the demo host is resumed
    virtual void OnResume() = 0;

    //! @brief Get the currently active API
    virtual DemoHostFeature GetActiveAPI() const = 0;
    //! @brief Get the current screen resolution
    virtual Point2 GetScreenResolution() const = 0;
    //! @brief Called to swap the main buffer.
    //! @return true on success, false if a error occurred.
    virtual bool SwapBuffers() = 0;

    //! @brief Called each frame to allow the demo host to do a bit of processing
    //! @param allowBlock if true the method is allowed to block for a extended period
    //         (for example it could wait for the next native message to arrive).
    virtual bool ProcessNativeMessages(const bool allowBlock) = 0;
  };
}

#endif
