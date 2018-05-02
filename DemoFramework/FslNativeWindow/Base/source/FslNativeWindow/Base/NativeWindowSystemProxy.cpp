/****************************************************************************************************************************************************
* Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslNativeWindow/Base/NativeWindowSystemProxy.hpp>

namespace Fsl
{
  NativeWindowSystemProxy::NativeWindowSystemProxy(const std::shared_ptr<INativeWindowSystem>& windowSystem)
    : m_windowSystem(windowSystem)
  {
    if (!windowSystem)
      throw std::invalid_argument("windowSystem can not be null");
  }

  std::shared_ptr<INativeWindow> NativeWindowSystemProxy::CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowAllocationParams*const pPlatformCustomWindowAllocationParams)
  {
    return m_windowSystem->CreateNativeWindow(nativeWindowSetup, pPlatformCustomWindowAllocationParams);
  }


  bool NativeWindowSystemProxy::ProcessMessages(const NativeWindowProcessMessagesArgs& args)
  {
    return m_windowSystem->ProcessMessages(args);
  }

  bool NativeWindowSystemProxy::IsDisplayHDRCompatible(const int32_t displayId) const
  {
    return m_windowSystem->IsDisplayHDRCompatible(displayId);
  }

}
