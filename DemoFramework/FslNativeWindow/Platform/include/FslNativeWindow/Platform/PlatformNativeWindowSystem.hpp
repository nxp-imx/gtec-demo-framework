#ifndef FSLNATIVEWINDOW_PLATFORM_PLATFORMNATIVEWINDOWSYSTEM_HPP
#define FSLNATIVEWINDOW_PLATFORM_PLATFORMNATIVEWINDOWSYSTEM_HPP
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

#include <FslNativeWindow/Base/INativeWindowSystem.hpp>
#include <FslNativeWindow/Base/NativeWindowSystemSetup.hpp>
#include <memory>

namespace Fsl
{
  class IPlatformNativeWindowSystemAdapter;
  class NativeWindowSystemSetup;

  class PlatformNativeWindowSystem : public virtual INativeWindowSystem
  {
    std::unique_ptr<IPlatformNativeWindowSystemAdapter> m_adapter;
    bool m_shutdownWasCalled{false};

  public:
    explicit PlatformNativeWindowSystem(const NativeWindowSystemSetup& setup, std::unique_ptr<IPlatformNativeWindowSystemAdapter> adapter);
    ~PlatformNativeWindowSystem() override;

    // INativeWindowSystem
    std::shared_ptr<INativeWindow>
      CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup,
                         const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams) override;

    bool ProcessMessages(const NativeWindowProcessMessagesArgs& args) override;

    //! @brief Check if the display is considered HDR compatible
    bool IsDisplayHDRCompatible(const int32_t displayId) const override;

    void Shutdown() override;


  protected:
    IPlatformNativeWindowSystemAdapter* TryGetAdapter() const
    {
      return m_adapter.get();
    }
  };
}

#endif
