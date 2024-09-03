#ifndef FSLUTIL_VULKAN1_0_LOG_FMTALL_HPP
#define FSLUTIL_VULKAN1_0_LOG_FMTALL_HPP
/****************************************************************************************************************************************************
 * Copyright 2019, 2024 NXP
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

#include <FslUtil/Vulkan1_0/EncodedVulkanVersion.hpp>
#include <RapidVulkan/Debug/Strings/VkColorSpaceKHR.hpp>
#include <RapidVulkan/Debug/Strings/VkFormat.hpp>
#include <fmt/format.h>
#include <vulkan/vulkan.h>

namespace fmt
{
  // https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#fundamentals-versionnum
  // The Vulkan version number is used in several places in the API.In each such use, the API major version number, minor version number,
  // and patch version number are packed into a 32 - bit integer as follows :
  // The major version number is a 10 - bit integer packed into bits 31 - 22.
  // The minor version number is a 10 - bit integer packed into bits 21 - 12.
  // The patch version number is a 12 - bit integer packed into bits 11 - 0.
  template <>
  struct formatter<Fsl::Vulkan::EncodedVulkanVersion>
  {
    template <typename ParseContext>
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr auto parse(ParseContext& ctx)
    {
      return ctx.begin();
    }

    template <typename FormatContext>
    // NOLINTNEXTLINE(readability-identifier-naming)
    auto format(const Fsl::Vulkan::EncodedVulkanVersion& value, FormatContext& ctx)
    {
      uint32_t major = (value.Value >> 22) & ((1 << 10) - 1);
      uint32_t minor = (value.Value >> 12) & ((1 << 10) - 1);
      uint32_t patch = value.Value & ((1 << 12) - 1);
      return fmt::format_to(ctx.out(), "{}.{}.{}", major, minor, patch);
    }
  };

  template <>
  struct formatter<VkExtent2D>
  {
    template <typename ParseContext>
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr auto parse(ParseContext& ctx)
    {
      return ctx.begin();
    }

    template <typename FormatContext>
    // NOLINTNEXTLINE(readability-identifier-naming)
    auto format(const VkExtent2D& value, FormatContext& ctx)
    {
      return fmt::format_to(ctx.out(), "{{width={} height={}}}", value.width, value.height);
    }
  };

  template <>
  struct formatter<VkExtent3D>
  {
    template <typename ParseContext>
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr auto parse(ParseContext& ctx)
    {
      return ctx.begin();
    }

    template <typename FormatContext>
    // NOLINTNEXTLINE(readability-identifier-naming)
    auto format(const VkExtent3D& value, FormatContext& ctx)
    {
      return fmt::format_to(ctx.out(), "{{width={} height={} depth={}}}", value.width, value.height, value.depth);
    }
  };

  template <>
  struct formatter<VkFormat>
  {
    template <typename ParseContext>
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr auto parse(ParseContext& ctx)
    {
      return ctx.begin();
    }

    template <typename FormatContext>
    // NOLINTNEXTLINE(readability-identifier-naming)
    auto format(const VkFormat& value, FormatContext& ctx)
    {
      const auto* psz = RapidVulkan::Debug::TryToString(value);
      if (psz != nullptr)
      {
        return fmt::format_to(ctx.out(), "{}", psz);
      }
      return fmt::format_to(ctx.out(), "0x{:x}", static_cast<uint32_t>(value));
    }
  };

  template <>
  struct formatter<VkColorSpaceKHR>
  {
    template <typename ParseContext>
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr auto parse(ParseContext& ctx)
    {
      return ctx.begin();
    }

    template <typename FormatContext>
    // NOLINTNEXTLINE(readability-identifier-naming)
    auto format(const VkColorSpaceKHR& value, FormatContext& ctx)
    {
      const auto* psz = RapidVulkan::Debug::TryToString(value);
      if (psz != nullptr)
      {
        return fmt::format_to(ctx.out(), "{}", psz);
      }
      return fmt::format_to(ctx.out(), "0x{:x}", static_cast<uint32_t>(value));
    }
  };
}

#endif
