#ifndef FSLNATIVEWINDOW_VULKAN_IVULKANNATIVEWINDOWSYSTEM_HPP
#define FSLNATIVEWINDOW_VULKAN_IVULKANNATIVEWINDOWSYSTEM_HPP
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

#include <FslNativeWindow/Base/NativeWindowProcessMessagesArgs.hpp>
#include <FslNativeWindow/Base/NativeWindowSetup.hpp>
#include <FslNativeWindow/Base/INativeWindowSystem.hpp>
#include <FslNativeWindow/Platform/PlatformNativeWindowSystemTypes.hpp>
#include <memory>
#include <string>

namespace Fsl
{
  class IVulkanNativeWindow;
  struct NativeVulkanSetup;

  class IVulkanNativeWindowSystem : public virtual INativeWindowSystem
  {
  public:
    ~IVulkanNativeWindowSystem() override = default;

    //! @brief Retrieve the name of the platform KHR surface extension (for example VK_KHR_WIN32_SURFACE_EXTENSION_NAME)
    virtual std::string GetKHRSurfaceExtensionName() const = 0;

    virtual PlatformNativeDisplayType GetDisplayType() const = 0;

    virtual std::shared_ptr<IVulkanNativeWindow> CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup,
                                                                    const NativeVulkanSetup& nativeVulkanSetup) = 0;
  };
}    // namespace Fsl

#endif
