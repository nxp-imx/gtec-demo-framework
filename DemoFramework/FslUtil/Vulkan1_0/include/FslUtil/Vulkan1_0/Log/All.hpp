#ifndef FSLUTIL_VULKAN1_0_LOG_ALL_HPP
#define FSLUTIL_VULKAN1_0_LOG_ALL_HPP
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

#include <FslBase/IO/ScopedStreamState.hpp>
#include <FslUtil/Vulkan1_0/EncodedVulkanVersion.hpp>
#include <RapidVulkan/Debug/Strings/VkFormat.hpp>
#include <vulkan/vulkan.h>
#include <ios>
#include <ostream>

namespace Fsl
{
  namespace Vulkan
  {
    // https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#fundamentals-versionnum
    // The Vulkan version number is used in several places in the API.In each such use, the API major version number, minor version number,
    // and patch version number are packed into a 32 - bit integer as follows :
    // The major version number is a 10 - bit integer packed into bits 31 - 22.
    // The minor version number is a 10 - bit integer packed into bits 21 - 12.
    // The patch version number is a 12 - bit integer packed into bits 11 - 0.
    inline std::ostream& operator<<(std::ostream& o, const Fsl::Vulkan::EncodedVulkanVersion& value)
    {
      uint32_t major = (value.Value >> 22) & ((1 << 10) - 1);
      uint32_t minor = (value.Value >> 12) & ((1 << 10) - 1);
      uint32_t patch = value.Value & ((1 << 12) - 1);
      return o << major << '.' << minor << '.' << patch;
    }
  }
}


inline std::ostream& operator<<(std::ostream& o, const VkExtent2D& value)
{
  return o << "width=" << value.width << ", height=" << value.height;
}

inline std::ostream& operator<<(std::ostream& o, const VkExtent3D& value)
{
  return o << "width=" << value.width << ", height=" << value.height << ", depth=" << value.depth;
}

inline std::ostream& operator<<(std::ostream& o, const VkFormat& value)
{
  const auto* psz = RapidVulkan::Debug::TryToString(value);
  if (psz != nullptr)
  {
    return o << psz;
  }
  Fsl::IO::ScopedStreamState scopedState(o);
  return o << "0x" << std::hex << static_cast<uint32_t>(value);
}

#endif
