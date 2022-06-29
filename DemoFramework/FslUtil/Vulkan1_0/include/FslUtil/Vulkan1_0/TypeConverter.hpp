#ifndef FSLUTIL_VULKAN1_0_TYPECONVERTER_HPP
#define FSLUTIL_VULKAN1_0_TYPECONVERTER_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022 NXP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslBase/Math/Extent2D.hpp>
#include <FslBase/Math/Extent3D.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxExtent3D.hpp>
#include <FslBase/TypeConverter.hpp>
#include <FslGraphics/Render/Basic/BasicCompareOp.hpp>
#include <FslGraphics/Render/Basic/BasicCullMode.hpp>
#include <FslGraphics/Render/Basic/BasicFrontFace.hpp>
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <vulkan/vulkan.h>

namespace Fsl::TypeConverter
{
  // Extent2D

  template <>
  constexpr inline Extent2D UncheckedTo<Extent2D, VkExtent2D>(const VkExtent2D& value) noexcept
  {
    return {value.width, value.height};
  }

  // Extent3D

  template <>
  constexpr inline Extent3D UncheckedTo<Extent3D, VkExtent3D>(const VkExtent3D& value) noexcept
  {
    return {value.width, value.height, value.depth};
  }

  // PxExtent2D

  template <>
  constexpr inline PxExtent2D UncheckedTo<PxExtent2D, VkExtent2D>(const VkExtent2D& value) noexcept
  {
    return {value.width, value.height};
  }

  // PxExtent3D

  template <>
  constexpr inline PxExtent3D UncheckedTo<PxExtent3D, VkExtent3D>(const VkExtent3D& value) noexcept
  {
    return {value.width, value.height, value.depth};
  }

  // VkExtent2D

  template <>
  constexpr inline VkExtent2D UncheckedTo<VkExtent2D, Extent2D>(const Extent2D& value) noexcept
  {
    return {value.Width, value.Height};
  }

  template <>
  constexpr inline VkExtent2D UncheckedTo<VkExtent2D, PxExtent2D>(const PxExtent2D& value) noexcept
  {
    return {value.Width, value.Height};
  }

  // VkExtent3D

  template <>
  constexpr inline VkExtent3D UncheckedTo<VkExtent3D, Extent3D>(const Extent3D& value) noexcept
  {
    return {value.Width, value.Height, value.Depth};
  }

  template <>
  constexpr inline VkExtent3D UncheckedTo<VkExtent3D, PxExtent3D>(const PxExtent3D& value) noexcept
  {
    return {value.Width, value.Height, value.Depth};
  }

  // VkCullModeFlags

  template <>
  inline VkCullModeFlags ChangeTo<VkCullModeFlags, BasicCullMode>(const BasicCullMode& value)
  {
    switch (value)
    {
    case BasicCullMode::Disabled:
      return VK_CULL_MODE_NONE;
    case BasicCullMode::Back:
      return VK_CULL_MODE_BACK_BIT;
    case BasicCullMode::Front:
      return VK_CULL_MODE_FRONT_BIT;
    case BasicCullMode::FrontAndBack:
      return VK_CULL_MODE_FRONT_AND_BACK;
    default:
      throw NotSupportedException("Unsupported BasicCullMode");
    }
  }

  // VkCullModeFlags

  template <>
  inline VkFrontFace ChangeTo<VkFrontFace, BasicFrontFace>(const BasicFrontFace& value)
  {
    switch (value)
    {
    case BasicFrontFace::CounterClockwise:
      return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    case BasicFrontFace::Clockwise:
      return VK_FRONT_FACE_CLOCKWISE;
    default:
      throw NotSupportedException("Unsupported BasicFrontFace");
    }
  }

  // VkCompareOp

  template <>
  inline VkCompareOp ChangeTo<VkCompareOp, BasicCompareOp>(const BasicCompareOp& value)
  {
    switch (value)
    {
    case BasicCompareOp::Never:
      return VK_COMPARE_OP_NEVER;
    case BasicCompareOp::AlwaysMatch:
      return VK_COMPARE_OP_ALWAYS;
    case BasicCompareOp::Equal:
      return VK_COMPARE_OP_EQUAL;
    case BasicCompareOp::Greater:
      return VK_COMPARE_OP_GREATER;
    case BasicCompareOp::GreaterOrEqual:
      return VK_COMPARE_OP_GREATER_OR_EQUAL;
    case BasicCompareOp::Less:
      return VK_COMPARE_OP_LESS;
    case BasicCompareOp::LessOrEqual:
      return VK_COMPARE_OP_LESS_OR_EQUAL;
    case BasicCompareOp::NotEqual:
      return VK_COMPARE_OP_NOT_EQUAL;
    default:
      throw NotSupportedException("Unsupported BasicCompareOp");
    }
  }
}

#endif
