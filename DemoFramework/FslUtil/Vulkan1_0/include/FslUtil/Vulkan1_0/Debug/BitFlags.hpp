#ifndef FSLUTIL_VULKAN1_0_DEBUG_BITFLAGS_HPP
#define FSLUTIL_VULKAN1_0_DEBUG_BITFLAGS_HPP
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

#include <RapidVulkan/Debug/Strings/VkQueueFlagBits.hpp>
#include <ios>
#include <sstream>
#include <string>

namespace Fsl
{
  namespace Vulkan
  {
    namespace Debug
    {
      template <typename TVulkanType>
      std::string GetBitflagsString(const TVulkanType flags)
      {
        std::stringstream stream;
        std::size_t foundCount = 0;

        if (flags > 0)
        {
          stream << "0x" << std::hex << flags << " (";
          uint32_t bitFlags = flags;
          for (uint32_t i = 0; i < 32; ++i)
          {
            if ((bitFlags & 0x80000000u) != 0)
            {
              auto flag = static_cast<TVulkanType>(0x80000000u >> i);
              if (foundCount > 0)
              {
                stream << "|";
              }
              auto pszFlag = RapidVulkan::Debug::TryToString(flag);
              if (pszFlag != nullptr)
              {
                stream << pszFlag;
              }
              else
              {
                stream << "0x" << std::hex << flag;
              }

              ++foundCount;
            }
            bitFlags <<= 1;
          }
          stream << ")";
        }
        else
        {
          stream << "0";
        }
        return stream.str();
      }
    }
  }
}

#endif
