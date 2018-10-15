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

#include <FslUtil/Vulkan1_0/SafeType/SwapchainCreateInfoKHRCopy.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <utility>

namespace Fsl
{
  namespace Vulkan
  {
    //! @brief Move assignment operator
    SwapchainCreateInfoKHRCopy& SwapchainCreateInfoKHRCopy::operator=(SwapchainCreateInfoKHRCopy&& other) noexcept
    {
      if (this != &other)
      {
        // Claim ownership here
        m_value = other.m_value;
        m_queueFamilyIndices = std::move(other.m_queueFamilyIndices);

        // Remove the data from other
        other.m_value = VkSwapchainCreateInfoKHR{};
        PatchPointers();
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    SwapchainCreateInfoKHRCopy::SwapchainCreateInfoKHRCopy(SwapchainCreateInfoKHRCopy&& other) noexcept
      : m_value(other.m_value)
      , m_queueFamilyIndices(std::move(other.m_queueFamilyIndices))
    {
      // Remove the data from other
      other.m_value = VkSwapchainCreateInfoKHR{};
      PatchPointers();
    }


    SwapchainCreateInfoKHRCopy::SwapchainCreateInfoKHRCopy()
      : m_value{}
    {
    }


    SwapchainCreateInfoKHRCopy::SwapchainCreateInfoKHRCopy(const VkSwapchainCreateInfoKHR& value)
      : m_value(value)
      , m_queueFamilyIndices(value.queueFamilyIndexCount)
    {
      for (std::size_t i = 0; i < m_queueFamilyIndices.size(); ++i)
      {
        m_queueFamilyIndices[i] = value.pQueueFamilyIndices[i];
      }

      // Now use the safe copied values instead
      PatchPointers();
      m_value.pNext = nullptr;
      FSLLOG_DEBUG_WARNING_IF(value.pNext != nullptr, "SwapchainCreateInfoKHRCopy always stores a nullptr for pNext");
    }


    void SwapchainCreateInfoKHRCopy::Reset()
    {
      *this = std::move(SwapchainCreateInfoKHRCopy());
    }


    void SwapchainCreateInfoKHRCopy::Reset(const VkSwapchainCreateInfoKHR& value)
    {
      *this = std::move(SwapchainCreateInfoKHRCopy(value));
    }


    void SwapchainCreateInfoKHRCopy::Reset(const VkSwapchainCreateInfoKHR& value, const VkSwapchainKHR updateOldSwapchain)
    {
      *this = std::move(SwapchainCreateInfoKHRCopy(value));
      m_value.oldSwapchain = updateOldSwapchain;
      m_value.pNext = nullptr;
      FSLLOG_DEBUG_WARNING_IF(value.pNext != nullptr, "SwapchainCreateInfoKHRCopy always stores a nullptr for pNext");
    }


    void SwapchainCreateInfoKHRCopy::PatchPointers() noexcept
    {
      m_value.pQueueFamilyIndices = m_value.pQueueFamilyIndices != nullptr ? m_queueFamilyIndices.data() : nullptr;
    }
  }
}
