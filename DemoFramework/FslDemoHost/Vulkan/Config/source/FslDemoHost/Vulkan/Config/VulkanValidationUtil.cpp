/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslDemoHost/Vulkan/Config/VulkanValidationUtil.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/LogExtent2D.hpp>
#include <FslUtil/Vulkan1_0/Util/ConvertUtil.hpp>
#include <FslUtil/Vulkan1_0/Log/All.hpp>
#include <RapidVulkan/Check.hpp>
#include <sstream>

namespace Fsl
{
  namespace Vulkan
  {
    namespace VulkanValidationUtil
    {
      namespace
      {
        std::string ToString(const Extent2D& extent)
        {
          std::stringstream stream;
          stream << extent;
          return stream.str();
        }

        std::string ToString(const VkExtent2D& extent)
        {
          std::stringstream stream;
          stream << extent;
          return stream.str();
        }
      }

      void CheckWindowAndSurfaceExtent(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, const Extent2D& windowExtent)
      {
        VkSurfaceCapabilitiesKHR surfaceCapabilities{};
        RAPIDVULKAN_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities));

        // Deal with the special undefined surface extent
        // https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html#vkGetPhysicalDeviceSurfaceCapabilitiesKHR
        if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF && surfaceCapabilities.currentExtent.height == 0xFFFFFFFF)
        {
          FSLLOG3_VERBOSE4("Skipping vulkan native window and surface extent comparison due to the surface not being defined.")
        }
        else if (ConvertUtil::Convert(surfaceCapabilities.currentExtent) != windowExtent)
        {
          throw InitFailedException("The Vulkan surface extent did not match the window extent. Screen: " + ToString(windowExtent) +
                                    " vs surface: " + ToString(surfaceCapabilities.currentExtent));
        }
      }
    };
  }
}
