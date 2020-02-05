#ifndef FSLDEMOAPP_VULKAN_BASIC_DEMOAPPVULKANSETUP_HPP
#define FSLDEMOAPP_VULKAN_BASIC_DEMOAPPVULKANSETUP_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Extent2D.hpp>
#include <FslDemoApp/Vulkan/Basic/DepthBufferMode.hpp>
#include <FslDemoApp/Vulkan/Basic/ResizeStrategy.hpp>
#include <vulkan/vulkan.h>

namespace Fsl
{
  namespace VulkanBasic
  {
    struct DemoAppVulkanSetup
    {
      //! If true a depth buffer will be created
      DepthBufferMode DepthBuffer{DepthBufferMode::Disabled};
      //! The minimum extent of the depth buffer (can be used to ensure the depth buffer is large enough to be reused for offscreen surfaces)
      Extent2D DepthBufferMinimumExtent;

      //! This only selects a strategy, but the DemoAppEnvironment will still have to be set to support resize
      //! - CustomDemoAppConfig customConfig;
      //! - customConfig.RestartOnResize = false;
      ResizeStrategy ActiveResizeStrategy{ResizeStrategy::RebuildResources};

      //! The max frames in flight (if zero the default setting will be used)
      uint32_t MaxFramesInFlight{0};

      //! The subpass the system UI should be rendered on.
      uint32_t SubpassSystemUI{0};

      //! The desired present mode
      VkPresentModeKHR DesiredSwapchainPresentMode{VK_PRESENT_MODE_FIFO_KHR};

      //! Additional image usage flags will be merged with VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
      //! The flags will only be set if the surface supports it (if it don't a warning will be logged and the unsupported flag will be ignored)
      VkImageUsageFlags DesiredSwapchainImageUsageFlags{0};
    };
  }
}

#endif
