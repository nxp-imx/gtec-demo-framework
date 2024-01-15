/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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
#include <FslNativeWindow/Platform/Adapter/IPlatformNativeWindowSystemAdapter.hpp>
#include <FslNativeWindow/Platform/PlatformNativeWindow.hpp>
#include <FslNativeWindow/Platform/PlatformNativeWindowSystem.hpp>

namespace Fsl
{
  // For now this is just a simple proxy
  PlatformNativeWindowSystem::PlatformNativeWindowSystem(const NativeWindowSystemSetup& setup,
                                                         std::unique_ptr<IPlatformNativeWindowSystemAdapter> adapter)
    : m_adapter(std::move(adapter))
  {
    if (!m_adapter)
    {
      throw std::invalid_argument("can not be null");
    }
  }


  PlatformNativeWindowSystem::~PlatformNativeWindowSystem()
  {
    FSLLOG3_WARNING_IF(!m_shutdownWasCalled, "PlatformNativeWindowSystem Shutdown was not called.");
  }


  std::shared_ptr<INativeWindow>
    PlatformNativeWindowSystem::CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup,
                                                   const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
  {
    if (!m_adapter)
    {
      throw ObjectShutdownException("CreateNativeWindow: Object shutdown");
    }
    std::shared_ptr<IPlatformNativeWindowAdapter> nativeWindow =
      m_adapter->CreateNativeWindow(nativeWindowSetup, pPlatformCustomWindowAllocationParams);
    return std::make_shared<PlatformNativeWindow>(nativeWindow);
  }


  bool PlatformNativeWindowSystem::ProcessMessages(const NativeWindowProcessMessagesArgs& args)
  {
    if (!m_adapter)
    {
      FSLLOG3_WARNING("ProcessMessages: Object shutdown");
      return false;
    }

    return m_adapter->ProcessMessages(args);
  }


  bool PlatformNativeWindowSystem::IsDisplayHDRCompatible(const int32_t displayId) const
  {
    if (!m_adapter)
    {
      FSLLOG3_WARNING("ProcessMessages: Object shutdown");
      return false;
    }

    return m_adapter->IsDisplayHDRCompatible(displayId);
  }


  void PlatformNativeWindowSystem::Shutdown()
  {
    if (!m_adapter)
    {
      return;
    }
    FSLLOG3_VERBOSE2("PlatformNameWindowSystem: shutdown");
    m_shutdownWasCalled = true;
    m_adapter.reset();
  }

}
