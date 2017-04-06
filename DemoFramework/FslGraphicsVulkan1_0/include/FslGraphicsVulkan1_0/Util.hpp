#ifndef FSLGRAPHICSVULKAN1_0_UTIL_HPP
#define FSLGRAPHICSVULKAN1_0_UTIL_HPP
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

// Auto-generated Vulkan 1.0 C++11 RAII classes by RAIIGen (https://github.com/Unarmed1000)
// Auto generation template based on RapidVulkan https://github.com/Unarmed1000/RapidVulkan with permission.

#include <FslGraphicsVulkan1_0/Exceptions.hpp>
#include <cassert>
#include <string>
#include <vulkan/vulkan.h>

namespace Fsl
{
  namespace Vulkan
  {
    class Util
    {
    public:
      static inline void Check(const VkResult result)
      {
        if (result != VK_SUCCESS)
          throw VulkanErrorException(ToNiceMessage("Unknown", result), result);
      }

      static inline void Check(const VkResult result, const char*const pszMessage)
      {
        if (result != VK_SUCCESS)
        {
          assert(pszMessage != nullptr);
          throw VulkanErrorException(ToNiceMessage(pszMessage, result), result);
        }
      }

      static inline void Check(const VkResult result, const std::string& message)
      {
        if (result != VK_SUCCESS)
        {
          throw VulkanErrorException(ToNiceMessage(message, result), result);
        }
      }

      static inline void Check(const VkResult result, const char*const pszMessage, const char*const pszFileName, const int lineNumber)
      {
        if (result != VK_SUCCESS)
        {
          assert(pszMessage != nullptr);
          assert(pszFileName != nullptr);
          throw VulkanErrorException(ToNiceMessage(pszMessage, result, pszFileName, lineNumber), result, pszFileName, lineNumber);
        }
      }

      static inline void Check(const VkResult result, const char*const pszMessage, const std::string& fileName, const int lineNumber)
      {
        if (result != VK_SUCCESS)
        {
          assert(pszMessage != nullptr);
          throw VulkanErrorException(ToNiceMessage(pszMessage, result, fileName, lineNumber), result, fileName, lineNumber);
        }
      }


      static inline void Check(const VkResult result, const std::string& message, const char*const pszFileName, const int lineNumber)
      {
        if (result != VK_SUCCESS)
        {
          assert(pszFileName != nullptr);
          throw VulkanErrorException(ToNiceMessage(message, result, pszFileName, lineNumber), result, pszFileName, lineNumber);
        }
      }

      static inline void Check(const VkResult result, const std::string& message, const std::string& fileName, const int lineNumber)
      {
        if (result != VK_SUCCESS)
        {
          throw VulkanErrorException(ToNiceMessage(message, result, fileName, lineNumber), result, fileName, lineNumber);
        }
      }
    private:
      static std::string ToNiceMessage(const std::string& message, const VkResult errorCode);
      static std::string ToNiceMessage(const std::string& message, const VkResult errorCode, const std::string& fileName, const int lineNumber);
    };
  }
}

#endif
