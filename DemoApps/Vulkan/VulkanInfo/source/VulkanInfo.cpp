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

#include "VulkanInfo.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslUtil/Vulkan1_0/Log/FmtAll.hpp>
#include <FslUtil/Vulkan1_0/Util/InstanceUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceUtil.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <FslUtil/Vulkan1_0/VUPhysicalDeviceRecord.hpp>
#include <RapidVulkan/Debug/Strings/VkFormatFeatureFlagBits.hpp>
#include <RapidVulkan/Debug/Strings/VkMemoryHeapFlagBits.hpp>
#include <RapidVulkan/Debug/Strings/VkMemoryPropertyFlagBits.hpp>
#include <RapidVulkan/Debug/Strings/VkPhysicalDeviceType.hpp>
#include <RapidVulkan/Debug/Strings/VkQueueFlagBits.hpp>
#include <fmt/format.h>
// Included last as a workaround
#include <FslUtil/Vulkan1_0/Debug/BitFlags.hpp>

namespace fmt
{
  template <>
  struct formatter<VkMemoryType>
  {
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
      return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const VkMemoryType& value, FormatContext& ctx)
    {
      return format_to(ctx.out(), "{{propertyFlags: {}, heapIndex: {}}}",
                       Fsl::Vulkan::Debug::GetBitflagsString(static_cast<VkMemoryPropertyFlagBits>(value.propertyFlags)), value.heapIndex);
    }
  };


  template <>
  struct formatter<VkMemoryHeap>
  {
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
      return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const VkMemoryHeap& value, FormatContext& ctx)
    {
      return format_to(ctx.out(), "{{size: {}, flags: {}}}", value.size,
                       Fsl::Vulkan::Debug::GetBitflagsString(static_cast<VkMemoryHeapFlagBits>(value.flags)));
    }
  };
}


namespace Fsl
{
  using namespace Vulkan;

  namespace
  {
    std::array<VkFormat, 184> g_allFormats = {
      VK_FORMAT_R4G4_UNORM_PACK8,              // = 1,
      VK_FORMAT_R4G4B4A4_UNORM_PACK16,         // = 2,
      VK_FORMAT_B4G4R4A4_UNORM_PACK16,         // = 3,
      VK_FORMAT_R5G6B5_UNORM_PACK16,           // = 4,
      VK_FORMAT_B5G6R5_UNORM_PACK16,           // = 5,
      VK_FORMAT_R5G5B5A1_UNORM_PACK16,         // = 6,
      VK_FORMAT_B5G5R5A1_UNORM_PACK16,         // = 7,
      VK_FORMAT_A1R5G5B5_UNORM_PACK16,         // = 8,
      VK_FORMAT_R8_UNORM,                      // = 9,
      VK_FORMAT_R8_SNORM,                      // = 10,
      VK_FORMAT_R8_USCALED,                    // = 11,
      VK_FORMAT_R8_SSCALED,                    // = 12,
      VK_FORMAT_R8_UINT,                       // = 13,
      VK_FORMAT_R8_SINT,                       // = 14,
      VK_FORMAT_R8_SRGB,                       // = 15,
      VK_FORMAT_R8G8_UNORM,                    // = 16,
      VK_FORMAT_R8G8_SNORM,                    // = 17,
      VK_FORMAT_R8G8_USCALED,                  // = 18,
      VK_FORMAT_R8G8_SSCALED,                  // = 19,
      VK_FORMAT_R8G8_UINT,                     // = 20,
      VK_FORMAT_R8G8_SINT,                     // = 21,
      VK_FORMAT_R8G8_SRGB,                     // = 22,
      VK_FORMAT_R8G8B8_UNORM,                  // = 23,
      VK_FORMAT_R8G8B8_SNORM,                  // = 24,
      VK_FORMAT_R8G8B8_USCALED,                // = 25,
      VK_FORMAT_R8G8B8_SSCALED,                // = 26,
      VK_FORMAT_R8G8B8_UINT,                   // = 27,
      VK_FORMAT_R8G8B8_SINT,                   // = 28,
      VK_FORMAT_R8G8B8_SRGB,                   // = 29,
      VK_FORMAT_B8G8R8_UNORM,                  // = 30,
      VK_FORMAT_B8G8R8_SNORM,                  // = 31,
      VK_FORMAT_B8G8R8_USCALED,                // = 32,
      VK_FORMAT_B8G8R8_SSCALED,                // = 33,
      VK_FORMAT_B8G8R8_UINT,                   // = 34,
      VK_FORMAT_B8G8R8_SINT,                   // = 35,
      VK_FORMAT_B8G8R8_SRGB,                   // = 36,
      VK_FORMAT_R8G8B8A8_UNORM,                // = 37,
      VK_FORMAT_R8G8B8A8_SNORM,                // = 38,
      VK_FORMAT_R8G8B8A8_USCALED,              // = 39,
      VK_FORMAT_R8G8B8A8_SSCALED,              // = 40,
      VK_FORMAT_R8G8B8A8_UINT,                 // = 41,
      VK_FORMAT_R8G8B8A8_SINT,                 // = 42,
      VK_FORMAT_R8G8B8A8_SRGB,                 // = 43,
      VK_FORMAT_B8G8R8A8_UNORM,                // = 44,
      VK_FORMAT_B8G8R8A8_SNORM,                // = 45,
      VK_FORMAT_B8G8R8A8_USCALED,              // = 46,
      VK_FORMAT_B8G8R8A8_SSCALED,              // = 47,
      VK_FORMAT_B8G8R8A8_UINT,                 // = 48,
      VK_FORMAT_B8G8R8A8_SINT,                 // = 49,
      VK_FORMAT_B8G8R8A8_SRGB,                 // = 50,
      VK_FORMAT_A8B8G8R8_UNORM_PACK32,         // = 51,
      VK_FORMAT_A8B8G8R8_SNORM_PACK32,         // = 52,
      VK_FORMAT_A8B8G8R8_USCALED_PACK32,       // = 53,
      VK_FORMAT_A8B8G8R8_SSCALED_PACK32,       // = 54,
      VK_FORMAT_A8B8G8R8_UINT_PACK32,          // = 55,
      VK_FORMAT_A8B8G8R8_SINT_PACK32,          // = 56,
      VK_FORMAT_A8B8G8R8_SRGB_PACK32,          // = 57,
      VK_FORMAT_A2R10G10B10_UNORM_PACK32,      // = 58,
      VK_FORMAT_A2R10G10B10_SNORM_PACK32,      // = 59,
      VK_FORMAT_A2R10G10B10_USCALED_PACK32,    // = 60,
      VK_FORMAT_A2R10G10B10_SSCALED_PACK32,    // = 61,
      VK_FORMAT_A2R10G10B10_UINT_PACK32,       // = 62,
      VK_FORMAT_A2R10G10B10_SINT_PACK32,       // = 63,
      VK_FORMAT_A2B10G10R10_UNORM_PACK32,      // = 64,
      VK_FORMAT_A2B10G10R10_SNORM_PACK32,      // = 65,
      VK_FORMAT_A2B10G10R10_USCALED_PACK32,    // = 66,
      VK_FORMAT_A2B10G10R10_SSCALED_PACK32,    // = 67,
      VK_FORMAT_A2B10G10R10_UINT_PACK32,       // = 68,
      VK_FORMAT_A2B10G10R10_SINT_PACK32,       // = 69,
      VK_FORMAT_R16_UNORM,                     // = 70,
      VK_FORMAT_R16_SNORM,                     // = 71,
      VK_FORMAT_R16_USCALED,                   // = 72,
      VK_FORMAT_R16_SSCALED,                   // = 73,
      VK_FORMAT_R16_UINT,                      // = 74,
      VK_FORMAT_R16_SINT,                      // = 75,
      VK_FORMAT_R16_SFLOAT,                    // = 76,
      VK_FORMAT_R16G16_UNORM,                  // = 77,
      VK_FORMAT_R16G16_SNORM,                  // = 78,
      VK_FORMAT_R16G16_USCALED,                // = 79,
      VK_FORMAT_R16G16_SSCALED,                // = 80,
      VK_FORMAT_R16G16_UINT,                   // = 81,
      VK_FORMAT_R16G16_SINT,                   // = 82,
      VK_FORMAT_R16G16_SFLOAT,                 // = 83,
      VK_FORMAT_R16G16B16_UNORM,               // = 84,
      VK_FORMAT_R16G16B16_SNORM,               // = 85,
      VK_FORMAT_R16G16B16_USCALED,             // = 86,
      VK_FORMAT_R16G16B16_SSCALED,             // = 87,
      VK_FORMAT_R16G16B16_UINT,                // = 88,
      VK_FORMAT_R16G16B16_SINT,                // = 89,
      VK_FORMAT_R16G16B16_SFLOAT,              // = 90,
      VK_FORMAT_R16G16B16A16_UNORM,            // = 91,
      VK_FORMAT_R16G16B16A16_SNORM,            // = 92,
      VK_FORMAT_R16G16B16A16_USCALED,          // = 93,
      VK_FORMAT_R16G16B16A16_SSCALED,          // = 94,
      VK_FORMAT_R16G16B16A16_UINT,             // = 95,
      VK_FORMAT_R16G16B16A16_SINT,             // = 96,
      VK_FORMAT_R16G16B16A16_SFLOAT,           // = 97,
      VK_FORMAT_R32_UINT,                      // = 98,
      VK_FORMAT_R32_SINT,                      // = 99,
      VK_FORMAT_R32_SFLOAT,                    // = 100,
      VK_FORMAT_R32G32_UINT,                   // = 101,
      VK_FORMAT_R32G32_SINT,                   // = 102,
      VK_FORMAT_R32G32_SFLOAT,                 // = 103,
      VK_FORMAT_R32G32B32_UINT,                // = 104,
      VK_FORMAT_R32G32B32_SINT,                // = 105,
      VK_FORMAT_R32G32B32_SFLOAT,              // = 106,
      VK_FORMAT_R32G32B32A32_UINT,             // = 107,
      VK_FORMAT_R32G32B32A32_SINT,             // = 108,
      VK_FORMAT_R32G32B32A32_SFLOAT,           // = 109,
      VK_FORMAT_R64_UINT,                      // = 110,
      VK_FORMAT_R64_SINT,                      // = 111,
      VK_FORMAT_R64_SFLOAT,                    // = 112,
      VK_FORMAT_R64G64_UINT,                   // = 113,
      VK_FORMAT_R64G64_SINT,                   // = 114,
      VK_FORMAT_R64G64_SFLOAT,                 // = 115,
      VK_FORMAT_R64G64B64_UINT,                // = 116,
      VK_FORMAT_R64G64B64_SINT,                // = 117,
      VK_FORMAT_R64G64B64_SFLOAT,              // = 118,
      VK_FORMAT_R64G64B64A64_UINT,             // = 119,
      VK_FORMAT_R64G64B64A64_SINT,             // = 120,
      VK_FORMAT_R64G64B64A64_SFLOAT,           // = 121,
      VK_FORMAT_B10G11R11_UFLOAT_PACK32,       // = 122,
      VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,        // = 123,
      VK_FORMAT_D16_UNORM,                     // = 124,
      VK_FORMAT_X8_D24_UNORM_PACK32,           // = 125,
      VK_FORMAT_D32_SFLOAT,                    // = 126,
      VK_FORMAT_S8_UINT,                       // = 127,
      VK_FORMAT_D16_UNORM_S8_UINT,             // = 128,
      VK_FORMAT_D24_UNORM_S8_UINT,             // = 129,
      VK_FORMAT_D32_SFLOAT_S8_UINT,            // = 130,
      VK_FORMAT_BC1_RGB_UNORM_BLOCK,           // = 131,
      VK_FORMAT_BC1_RGB_SRGB_BLOCK,            // = 132,
      VK_FORMAT_BC1_RGBA_UNORM_BLOCK,          // = 133,
      VK_FORMAT_BC1_RGBA_SRGB_BLOCK,           // = 134,
      VK_FORMAT_BC2_UNORM_BLOCK,               // = 135,
      VK_FORMAT_BC2_SRGB_BLOCK,                // = 136,
      VK_FORMAT_BC3_UNORM_BLOCK,               // = 137,
      VK_FORMAT_BC3_SRGB_BLOCK,                // = 138,
      VK_FORMAT_BC4_UNORM_BLOCK,               // = 139,
      VK_FORMAT_BC4_SNORM_BLOCK,               // = 140,
      VK_FORMAT_BC5_UNORM_BLOCK,               // = 141,
      VK_FORMAT_BC5_SNORM_BLOCK,               // = 142,
      VK_FORMAT_BC6H_UFLOAT_BLOCK,             // = 143,
      VK_FORMAT_BC6H_SFLOAT_BLOCK,             // = 144,
      VK_FORMAT_BC7_UNORM_BLOCK,               // = 145,
      VK_FORMAT_BC7_SRGB_BLOCK,                // = 146,
      VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,       // = 147,
      VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,        // = 148,
      VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK,     // = 149,
      VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK,      // = 150,
      VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,     // = 151,
      VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,      // = 152,
      VK_FORMAT_EAC_R11_UNORM_BLOCK,           // = 153,
      VK_FORMAT_EAC_R11_SNORM_BLOCK,           // = 154,
      VK_FORMAT_EAC_R11G11_UNORM_BLOCK,        // = 155,
      VK_FORMAT_EAC_R11G11_SNORM_BLOCK,        // = 156,
      VK_FORMAT_ASTC_4x4_UNORM_BLOCK,          // = 157,
      VK_FORMAT_ASTC_4x4_SRGB_BLOCK,           // = 158,
      VK_FORMAT_ASTC_5x4_UNORM_BLOCK,          // = 159,
      VK_FORMAT_ASTC_5x4_SRGB_BLOCK,           // = 160,
      VK_FORMAT_ASTC_5x5_UNORM_BLOCK,          // = 161,
      VK_FORMAT_ASTC_5x5_SRGB_BLOCK,           // = 162,
      VK_FORMAT_ASTC_6x5_UNORM_BLOCK,          // = 163,
      VK_FORMAT_ASTC_6x5_SRGB_BLOCK,           // = 164,
      VK_FORMAT_ASTC_6x6_UNORM_BLOCK,          // = 165,
      VK_FORMAT_ASTC_6x6_SRGB_BLOCK,           // = 166,
      VK_FORMAT_ASTC_8x5_UNORM_BLOCK,          // = 167,
      VK_FORMAT_ASTC_8x5_SRGB_BLOCK,           // = 168,
      VK_FORMAT_ASTC_8x6_UNORM_BLOCK,          // = 169,
      VK_FORMAT_ASTC_8x6_SRGB_BLOCK,           // = 170,
      VK_FORMAT_ASTC_8x8_UNORM_BLOCK,          // = 171,
      VK_FORMAT_ASTC_8x8_SRGB_BLOCK,           // = 172,
      VK_FORMAT_ASTC_10x5_UNORM_BLOCK,         // = 173,
      VK_FORMAT_ASTC_10x5_SRGB_BLOCK,          // = 174,
      VK_FORMAT_ASTC_10x6_UNORM_BLOCK,         // = 175,
      VK_FORMAT_ASTC_10x6_SRGB_BLOCK,          // = 176,
      VK_FORMAT_ASTC_10x8_UNORM_BLOCK,         // = 177,
      VK_FORMAT_ASTC_10x8_SRGB_BLOCK,          // = 178,
      VK_FORMAT_ASTC_10x10_UNORM_BLOCK,        // = 179,
      VK_FORMAT_ASTC_10x10_SRGB_BLOCK,         // = 180,
      VK_FORMAT_ASTC_12x10_UNORM_BLOCK,        // = 181,
      VK_FORMAT_ASTC_12x10_SRGB_BLOCK,         // = 182,
      VK_FORMAT_ASTC_12x12_UNORM_BLOCK,        // = 183,
      VK_FORMAT_ASTC_12x12_SRGB_BLOCK,         // = 184,
    };

    class CustomWriter
    {
    public:
      void Print(const char* const psz) noexcept
      {
        Logger::WriteLine(LogType::Info, psz);
      }

      template <typename... Args>
      void Print(const char* const pszFormat, const Args&... args) noexcept
      {
        Logger::WriteLine(LogType::Info, pszFormat, args...);
      }
    };


    void LogPhysicalDeviceLimits(CustomWriter& rWriter, const VkPhysicalDeviceLimits& limits)
    {
      rWriter.Print("- limits.maxImageDimension1D: {}", limits.maxImageDimension1D);
      rWriter.Print("- limits.maxImageDimension2D: {}", limits.maxImageDimension2D);
      rWriter.Print("- limits.maxImageDimension3D: {}", limits.maxImageDimension3D);
      rWriter.Print("- limits.maxImageDimensionCube: {}", limits.maxImageDimensionCube);
      rWriter.Print("- limits.maxImageArrayLayers: {}", limits.maxImageArrayLayers);
      rWriter.Print("- limits.maxTexelBufferElements: {}", limits.maxTexelBufferElements);
      rWriter.Print("- limits.maxUniformBufferRange: {}", limits.maxUniformBufferRange);
      rWriter.Print("- limits.maxStorageBufferRange: {}", limits.maxStorageBufferRange);
      rWriter.Print("- limits.maxPushConstantsSize: {}", limits.maxPushConstantsSize);
      rWriter.Print("- limits.maxMemoryAllocationCount: {}", limits.maxMemoryAllocationCount);
      rWriter.Print("- limits.maxSamplerAllocationCount: {}", limits.maxSamplerAllocationCount);
      rWriter.Print("- limits.bufferImageGranularity: {}", limits.bufferImageGranularity);
      rWriter.Print("- limits.sparseAddressSpaceSize: {}", limits.sparseAddressSpaceSize);
      rWriter.Print("- limits.maxBoundDescriptorSets: {}", limits.maxBoundDescriptorSets);
      rWriter.Print("- limits.maxPerStageDescriptorSamplers: {}", limits.maxPerStageDescriptorSamplers);
      rWriter.Print("- limits.maxPerStageDescriptorUniformBuffers: {}", limits.maxPerStageDescriptorUniformBuffers);
      rWriter.Print("- limits.maxPerStageDescriptorStorageBuffers: {}", limits.maxPerStageDescriptorStorageBuffers);
      rWriter.Print("- limits.maxPerStageDescriptorSampledImages: {}", limits.maxPerStageDescriptorSampledImages);
      rWriter.Print("- limits.maxPerStageDescriptorStorageImages: {}", limits.maxPerStageDescriptorStorageImages);
      rWriter.Print("- limits.maxPerStageDescriptorInputAttachments: {}", limits.maxPerStageDescriptorInputAttachments);
      rWriter.Print("- limits.maxPerStageResources: {}", limits.maxPerStageResources);
      rWriter.Print("- limits.maxDescriptorSetSamplers: {}", limits.maxDescriptorSetSamplers);
      rWriter.Print("- limits.maxDescriptorSetUniformBuffers: {}", limits.maxDescriptorSetUniformBuffers);
      rWriter.Print("- limits.maxDescriptorSetUniformBuffersDynamic: {}", limits.maxDescriptorSetUniformBuffersDynamic);
      rWriter.Print("- limits.maxDescriptorSetStorageBuffers: {}", limits.maxDescriptorSetStorageBuffers);
      rWriter.Print("- limits.maxDescriptorSetStorageBuffersDynamic: {}", limits.maxDescriptorSetStorageBuffersDynamic);
      rWriter.Print("- limits.maxDescriptorSetSampledImages: {}", limits.maxDescriptorSetSampledImages);
      rWriter.Print("- limits.maxDescriptorSetStorageImages: {}", limits.maxDescriptorSetStorageImages);
      rWriter.Print("- limits.maxDescriptorSetInputAttachments: {}", limits.maxDescriptorSetInputAttachments);
      rWriter.Print("- limits.maxVertexInputAttributes: {}", limits.maxVertexInputAttributes);
      rWriter.Print("- limits.maxVertexInputBindings: {}", limits.maxVertexInputBindings);
      rWriter.Print("- limits.maxVertexInputAttributeOffset: {}", limits.maxVertexInputAttributeOffset);
      rWriter.Print("- limits.maxVertexInputBindingStride: {}", limits.maxVertexInputBindingStride);
      rWriter.Print("- limits.maxVertexOutputComponents: {}", limits.maxVertexOutputComponents);
      rWriter.Print("- limits.maxTessellationGenerationLevel: {}", limits.maxTessellationGenerationLevel);
      rWriter.Print("- limits.maxTessellationPatchSize: {}", limits.maxTessellationPatchSize);
      rWriter.Print("- limits.maxTessellationControlPerVertexInputComponents: {}", limits.maxTessellationControlPerVertexInputComponents);
      rWriter.Print("- limits.maxTessellationControlPerVertexOutputComponents: {}", limits.maxTessellationControlPerVertexOutputComponents);
      rWriter.Print("- limits.maxTessellationControlPerPatchOutputComponents: {}", limits.maxTessellationControlPerPatchOutputComponents);
      rWriter.Print("- limits.maxTessellationControlTotalOutputComponents: {}", limits.maxTessellationControlTotalOutputComponents);
      rWriter.Print("- limits.maxTessellationEvaluationInputComponents: {}", limits.maxTessellationEvaluationInputComponents);
      rWriter.Print("- limits.maxTessellationEvaluationOutputComponents: {}", limits.maxTessellationEvaluationOutputComponents);
      rWriter.Print("- limits.maxGeometryShaderInvocations: {}", limits.maxGeometryShaderInvocations);
      rWriter.Print("- limits.maxGeometryInputComponents: {}", limits.maxGeometryInputComponents);
      rWriter.Print("- limits.maxGeometryOutputComponents: {}", limits.maxGeometryOutputComponents);

      rWriter.Print("- limits.maxGeometryOutputVertices: {}", limits.maxGeometryOutputVertices);
      rWriter.Print("- limits.maxGeometryTotalOutputComponents: {}", limits.maxGeometryTotalOutputComponents);
      rWriter.Print("- limits.maxFragmentInputComponents: {}", limits.maxFragmentInputComponents);
      rWriter.Print("- limits.maxFragmentOutputAttachments: {}", limits.maxFragmentOutputAttachments);
      rWriter.Print("- limits.maxFragmentDualSrcAttachments: {}", limits.maxFragmentDualSrcAttachments);
      rWriter.Print("- limits.maxFragmentCombinedOutputResources: {}", limits.maxFragmentCombinedOutputResources);
      rWriter.Print("- limits.maxComputeSharedMemorySize: {}", limits.maxComputeSharedMemorySize);

      for (uint32_t i = 0; i < 3; ++i)
      {
        rWriter.Print("- limits.maxComputeWorkGroupCount[{}]: {}", i, limits.maxComputeWorkGroupCount[i]);
      }

      rWriter.Print("- limits.maxComputeWorkGroupInvocations: {}", limits.maxComputeWorkGroupInvocations);

      for (uint32_t i = 0; i < 3; ++i)
      {
        rWriter.Print("- limits.maxComputeWorkGroupSize[{}]: {}", i, limits.maxComputeWorkGroupSize[i]);
      }

      rWriter.Print("- limits.subPixelPrecisionBits: {}", limits.subPixelPrecisionBits);
      rWriter.Print("- limits.subTexelPrecisionBits: {}", limits.subTexelPrecisionBits);
      rWriter.Print("- limits.mipmapPrecisionBits: {}", limits.mipmapPrecisionBits);
      rWriter.Print("- limits.maxDrawIndexedIndexValue: {}", limits.maxDrawIndexedIndexValue);
      rWriter.Print("- limits.maxDrawIndirectCount: {}", limits.maxDrawIndirectCount);
      rWriter.Print("- limits.maxSamplerLodBias: {}", limits.maxSamplerLodBias);
      rWriter.Print("- limits.maxSamplerAnisotropy: {}", limits.maxSamplerAnisotropy);
      rWriter.Print("- limits.maxViewports: {}", limits.maxViewports);

      for (uint32_t i = 0; i < 2; ++i)
      {
        rWriter.Print("- limits.maxViewportDimensions[{}]: {}", i, limits.maxViewportDimensions[i]);
      }
      for (uint32_t i = 0; i < 2; ++i)
      {
        rWriter.Print("- limits.viewportBoundsRange[{}]: {}", i, limits.viewportBoundsRange[i]);
      }

      rWriter.Print("- limits.viewportSubPixelBits: {}", limits.viewportSubPixelBits);
      rWriter.Print("- limits.minMemoryMapAlignment: {}", limits.minMemoryMapAlignment);
      rWriter.Print("- limits.minTexelBufferOffsetAlignment: {}", limits.minTexelBufferOffsetAlignment);
      rWriter.Print("- limits.minUniformBufferOffsetAlignment: {}", limits.minUniformBufferOffsetAlignment);
      rWriter.Print("- limits.minStorageBufferOffsetAlignment: {}", limits.minStorageBufferOffsetAlignment);
      rWriter.Print("- limits.minTexelOffset: {}", limits.minTexelOffset);
      rWriter.Print("- limits.maxTexelOffset: {}", limits.maxTexelOffset);
      rWriter.Print("- limits.minTexelGatherOffset: {}", limits.minTexelGatherOffset);
      rWriter.Print("- limits.maxTexelGatherOffset: {}", limits.maxTexelGatherOffset);
      rWriter.Print("- limits.minInterpolationOffset: {}", limits.minInterpolationOffset);
      rWriter.Print("- limits.maxInterpolationOffset: {}", limits.maxInterpolationOffset);
      rWriter.Print("- limits.subPixelInterpolationOffsetBits: {}", limits.subPixelInterpolationOffsetBits);
      rWriter.Print("- limits.maxFramebufferWidth: {}", limits.maxFramebufferWidth);
      rWriter.Print("- limits.maxFramebufferHeight: {}", limits.maxFramebufferHeight);
      rWriter.Print("- limits.maxFramebufferLayers: {}", limits.maxFramebufferLayers);
      rWriter.Print("- limits.framebufferColorSampleCounts: {}", limits.framebufferColorSampleCounts);
      rWriter.Print("- limits.framebufferDepthSampleCounts: {}", limits.framebufferDepthSampleCounts);
      rWriter.Print("- limits.framebufferStencilSampleCounts: {}", limits.framebufferStencilSampleCounts);
      rWriter.Print("- limits.framebufferNoAttachmentsSampleCounts: {}", limits.framebufferNoAttachmentsSampleCounts);
      rWriter.Print("- limits.maxColorAttachments: {}", limits.maxColorAttachments);
      rWriter.Print("- limits.sampledImageColorSampleCounts: {}", limits.sampledImageColorSampleCounts);
      rWriter.Print("- limits.sampledImageIntegerSampleCounts: {}", limits.sampledImageIntegerSampleCounts);
      rWriter.Print("- limits.sampledImageDepthSampleCounts: {}", limits.sampledImageDepthSampleCounts);
      rWriter.Print("- limits.sampledImageStencilSampleCounts: {}", limits.sampledImageStencilSampleCounts);
      rWriter.Print("- limits.storageImageSampleCounts: {}", limits.storageImageSampleCounts);
      rWriter.Print("- limits.maxSampleMaskWords: {}", limits.maxSampleMaskWords);
      rWriter.Print("- limits.timestampComputeAndGraphics: {}", limits.timestampComputeAndGraphics);

      rWriter.Print("- limits.timestampPeriod: {}", limits.timestampPeriod);
      rWriter.Print("- limits.maxClipDistances: {}", limits.maxClipDistances);
      rWriter.Print("- limits.maxCullDistances: {}", limits.maxCullDistances);
      rWriter.Print("- limits.maxCombinedClipAndCullDistances: {}", limits.maxCombinedClipAndCullDistances);
      rWriter.Print("- limits.discreteQueuePriorities: {}", limits.discreteQueuePriorities);

      for (uint32_t i = 0; i < 2; ++i)
      {
        rWriter.Print("- limits.pointSizeRange[{}]: {}", i, limits.pointSizeRange[i]);
      }

      for (uint32_t i = 0; i < 2; ++i)
      {
        rWriter.Print("- limits.lineWidthRange[{}]: {}", i, limits.lineWidthRange[i]);
      }

      rWriter.Print("- limits.pointSizeGranularity: {}", limits.pointSizeGranularity);
      rWriter.Print("- limits.lineWidthGranularity: {}", limits.lineWidthGranularity);
      rWriter.Print("- limits.strictLines: {}", limits.strictLines);
      rWriter.Print("- limits.standardSampleLocations: {}", limits.standardSampleLocations);
      rWriter.Print("- limits.optimalBufferCopyOffsetAlignment: {}", limits.optimalBufferCopyOffsetAlignment);
      rWriter.Print("- limits.optimalBufferCopyRowPitchAlignment: {}", limits.optimalBufferCopyRowPitchAlignment);
      rWriter.Print("- limits.nonCoherentAtomSize: {}", limits.nonCoherentAtomSize);
    }

    void LogPhysicalDeviceSparseProperties(CustomWriter& rWriter, const VkPhysicalDeviceSparseProperties& properties)
    {
      rWriter.Print("- sparseProperties.residencyStandard2DBlockShape: {}", properties.residencyStandard2DBlockShape);
      rWriter.Print("- sparseProperties.residencyStandard2DMultisampleBlockShape: {}", properties.residencyStandard2DMultisampleBlockShape);
      rWriter.Print("- sparseProperties.residencyStandard3DBlockShape: {}", properties.residencyStandard3DBlockShape);
      rWriter.Print("- sparseProperties.residencyAlignedMipSize: {}", properties.residencyAlignedMipSize);
      rWriter.Print("- sparseProperties.residencyNonResidentStrict: {}", properties.residencyNonResidentStrict);
    }

    void LogPhysicalProperties(CustomWriter& rWriter, const VkPhysicalDeviceProperties& properties)
    {
      rWriter.Print("Physical device properties:");
      rWriter.Print("- apiVersion: {}", EncodedVulkanVersion(properties.apiVersion));
      rWriter.Print("- driverVersion: {}", EncodedVulkanVersion(properties.driverVersion));
      rWriter.Print("- vendorID: {}", properties.vendorID);
      rWriter.Print("- deviceID: {}", properties.deviceID);
      rWriter.Print("- deviceType: {}", Debug::GetBitflagsString(properties.deviceType));    // VkPhysicalDeviceType
      rWriter.Print("- deviceName: {}", properties.deviceName);
      // uint8_t                             pipelineCacheUUID[VK_UUID_SIZE];
      LogPhysicalDeviceLimits(rWriter, properties.limits);
      LogPhysicalDeviceSparseProperties(rWriter, properties.sparseProperties);
    }


    void LogPhysicalDeviceFeatures(CustomWriter& rWriter, const VkPhysicalDeviceFeatures& features)
    {
      rWriter.Print("Physical device features:");
      rWriter.Print("- robustBufferAccess: {}", features.robustBufferAccess);
      rWriter.Print("- fullDrawIndexUint32: {}", features.fullDrawIndexUint32);
      rWriter.Print("- imageCubeArray: {}", features.imageCubeArray);
      rWriter.Print("- independentBlend: {}", features.independentBlend);
      rWriter.Print("- geometryShader: {}", features.geometryShader);
      rWriter.Print("- tessellationShader: {}", features.tessellationShader);
      rWriter.Print("- sampleRateShading: {}", features.sampleRateShading);
      rWriter.Print("- dualSrcBlend: {}", features.dualSrcBlend);
      rWriter.Print("- logicOp: {}", features.logicOp);
      rWriter.Print("- multiDrawIndirect: {}", features.multiDrawIndirect);
      rWriter.Print("- drawIndirectFirstInstance: {}", features.drawIndirectFirstInstance);
      rWriter.Print("- depthClamp: {}", features.depthClamp);
      rWriter.Print("- depthBiasClamp: {}", features.depthBiasClamp);
      rWriter.Print("- fillModeNonSolid: {}", features.fillModeNonSolid);
      rWriter.Print("- depthBounds: {}", features.depthBounds);
      rWriter.Print("- wideLines: {}", features.wideLines);
      rWriter.Print("- largePoints: {}", features.largePoints);
      rWriter.Print("- alphaToOne: {}", features.alphaToOne);
      rWriter.Print("- multiViewport: {}", features.multiViewport);
      rWriter.Print("- samplerAnisotropy: {}", features.samplerAnisotropy);
      rWriter.Print("- textureCompressionETC2: {}", features.textureCompressionETC2);
      rWriter.Print("- textureCompressionASTC_LDR: {}", features.textureCompressionASTC_LDR);
      rWriter.Print("- textureCompressionBC: {}", features.textureCompressionBC);
      rWriter.Print("- occlusionQueryPrecise: {}", features.occlusionQueryPrecise);
      rWriter.Print("- pipelineStatisticsQuery: {}", features.pipelineStatisticsQuery);
      rWriter.Print("- vertexPipelineStoresAndAtomics: {}", features.vertexPipelineStoresAndAtomics);
      rWriter.Print("- fragmentStoresAndAtomics: {}", features.fragmentStoresAndAtomics);
      rWriter.Print("- shaderTessellationAndGeometryPointSize: {}", features.shaderTessellationAndGeometryPointSize);
      rWriter.Print("- shaderImageGatherExtended: {}", features.shaderImageGatherExtended);
      rWriter.Print("- shaderStorageImageExtendedFormats: {}", features.shaderStorageImageExtendedFormats);
      rWriter.Print("- shaderStorageImageMultisample: {}", features.shaderStorageImageMultisample);
      rWriter.Print("- shaderStorageImageReadWithoutFormat: {}", features.shaderStorageImageReadWithoutFormat);
      rWriter.Print("- shaderStorageImageWriteWithoutFormat: {}", features.shaderStorageImageWriteWithoutFormat);
      rWriter.Print("- shaderUniformBufferArrayDynamicIndexing: {}", features.shaderUniformBufferArrayDynamicIndexing);
      rWriter.Print("- shaderSampledImageArrayDynamicIndexing: {}", features.shaderSampledImageArrayDynamicIndexing);
      rWriter.Print("- shaderStorageBufferArrayDynamicIndexing: {}", features.shaderStorageBufferArrayDynamicIndexing);
      rWriter.Print("- shaderStorageImageArrayDynamicIndexing: {}", features.shaderStorageImageArrayDynamicIndexing);
      rWriter.Print("- shaderClipDistance: {}", features.shaderClipDistance);
      rWriter.Print("- shaderCullDistance: {}", features.shaderCullDistance);
      rWriter.Print("- shaderFloat64: {}", features.shaderFloat64);
      rWriter.Print("- shaderInt64: {}", features.shaderInt64);
      rWriter.Print("- shaderInt16: {}", features.shaderInt16);
      rWriter.Print("- shaderResourceResidency: {}", features.shaderResourceResidency);
      rWriter.Print("- shaderResourceMinLod: {}", features.shaderResourceMinLod);
      rWriter.Print("- sparseBinding: {}", features.sparseBinding);
      rWriter.Print("- sparseResidencyBuffer: {}", features.sparseResidencyBuffer);
      rWriter.Print("- sparseResidencyImage2D: {}", features.sparseResidencyImage2D);
      rWriter.Print("- sparseResidencyImage3D: {}", features.sparseResidencyImage3D);
      rWriter.Print("- sparseResidency2Samples: {}", features.sparseResidency2Samples);
      rWriter.Print("- sparseResidency4Samples: {}", features.sparseResidency4Samples);
      rWriter.Print("- sparseResidency8Samples: {}", features.sparseResidency8Samples);
      rWriter.Print("- sparseResidency16Samples: {}", features.sparseResidency16Samples);
      rWriter.Print("- sparseResidencyAliased: {}", features.sparseResidencyAliased);
      rWriter.Print("- variableMultisampleRate: {}", features.variableMultisampleRate);
      rWriter.Print("- inheritedQueries: {}", features.inheritedQueries);
    }


    void LogPhysicalDeviceMemoryProperties(CustomWriter& rWriter, const VkPhysicalDeviceMemoryProperties& properties)
    {
      rWriter.Print("Physical device memory properties:");
      rWriter.Print("- memoryTypeCount: {}", properties.memoryTypeCount);
      for (uint32_t i = 0; i < properties.memoryTypeCount; ++i)
      {
        rWriter.Print("- memoryTypes[{}]: {}", i, properties.memoryTypes[i]);
      }

      rWriter.Print("- memoryHeapCount: {}", properties.memoryHeapCount);
      for (uint32_t i = 0; i < properties.memoryHeapCount; ++i)
      {
        rWriter.Print("- memoryHeaps[{}]: {}", i, properties.memoryHeaps[i]);
      }
    }

    void LogPhysicalDeviceQueueFamilyProperties(CustomWriter& rWriter, const std::vector<VkQueueFamilyProperties>& queueFamilyProperties)
    {
      rWriter.Print("Physical device queue family properties:");
      for (std::size_t i = 0; i < queueFamilyProperties.size(); ++i)
      {
        rWriter.Print("- Queue Famility #{}", i);
        rWriter.Print("  - queueFlags: {}", Debug::GetBitflagsString(static_cast<VkQueueFlagBits>(queueFamilyProperties[i].queueFlags)));
        rWriter.Print("  - queueCount: {}", queueFamilyProperties[i].queueCount);
        rWriter.Print("  - timestampValidBits: {}", queueFamilyProperties[i].timestampValidBits);
        rWriter.Print("  - minImageTransferGranularity: {}", queueFamilyProperties[i].minImageTransferGranularity);
      }
    }

    void LogAllFormats(CustomWriter& rWriter, const VUPhysicalDeviceRecord& physicalDevice)
    {
      rWriter.Print("All phsycial device format properties:");
      for (std::size_t i = 0; i < g_allFormats.size(); ++i)
      {
        const auto format = g_allFormats[i];
        rWriter.Print("- Format #{} ({})", i, format);

        VkFormatProperties formatProperties = PhysicalDeviceUtil::GetPhysicalDeviceFormatProperties(physicalDevice.Device, format);

        rWriter.Print("  - linearTilingFeatures: {}",
                      Debug::GetBitflagsString(static_cast<VkFormatFeatureFlagBits>(formatProperties.linearTilingFeatures)));
        rWriter.Print("  - optimalTilingFeatures: {}",
                      Debug::GetBitflagsString(static_cast<VkFormatFeatureFlagBits>(formatProperties.optimalTilingFeatures)));
        rWriter.Print("  - bufferFeatures: {}", Debug::GetBitflagsString(static_cast<VkFormatFeatureFlagBits>(formatProperties.bufferFeatures)));
      }
    }

    void LogPhysicalDevice(CustomWriter& rWriter, const VUPhysicalDeviceRecord& physicalDevice)
    {
      LogPhysicalProperties(rWriter, physicalDevice.Properties);
      LogPhysicalDeviceFeatures(rWriter, physicalDevice.Features);
      LogPhysicalDeviceMemoryProperties(rWriter, physicalDevice.MemoryProperties);

      LogPhysicalDeviceQueueFamilyProperties(rWriter, PhysicalDeviceUtil::GetPhysicalDeviceQueueFamilyProperties(physicalDevice.Device));

      LogAllFormats(rWriter, physicalDevice);
    }

    void LogInstanceLayerProperties(CustomWriter& rWriter, const std::vector<VkLayerProperties>& layerProperties)
    {
      rWriter.Print("Instance layer properties: {}", layerProperties.size());
      for (std::size_t i = 0; i < layerProperties.size(); ++i)
      {
        rWriter.Print("- layer #{}", i);
        rWriter.Print("  - name: {}", layerProperties[i].layerName);
        rWriter.Print("  - specVersion: {}", EncodedVulkanVersion(layerProperties[i].specVersion));
        rWriter.Print("  - implementationVersion: {}", layerProperties[i].implementationVersion);
        rWriter.Print("  - description: {}", layerProperties[i].description);
        auto extensionProperties = InstanceUtil::EnumerateInstanceExtensionProperties(layerProperties[i].layerName);
        if (!extensionProperties.empty())
        {
          rWriter.Print("  - Extension properties:");
          for (std::size_t j = 0; j < extensionProperties.size(); ++j)
          {
            rWriter.Print("    - property #{}", j);
            rWriter.Print("    - name: {}", extensionProperties[j].extensionName);
            rWriter.Print("    - specVersion: {}", extensionProperties[j].specVersion);
          }
        }
      }
    }

    void LogVulkanCoreExtensions(CustomWriter& rWriter)
    {
      auto extensionProperties = InstanceUtil::EnumerateInstanceExtensionProperties(nullptr);
      rWriter.Print("Core extensions: {}", extensionProperties.size());
      for (std::size_t j = 0; j < extensionProperties.size(); ++j)
      {
        rWriter.Print("- Extension #{}", j);
        rWriter.Print("  - name: {}", extensionProperties[j].extensionName);
        rWriter.Print("  - specVersion: {}", extensionProperties[j].specVersion);
      }
    }
  }


  VulkanInfo::VulkanInfo(const DemoAppConfig& config)
    : DemoAppConsoleMinimal(config)
  {
  }


  VulkanInfo::~VulkanInfo() = default;


  void VulkanInfo::Run()
  {
    CustomWriter writer;

    auto instance = InstanceUtil::CreateInstance("VulkanInfo", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0, 0, 0, nullptr, 0, nullptr);

    auto instanceLayerProperties = InstanceUtil::EnumerateInstanceLayerProperties();
    LogInstanceLayerProperties(writer, instanceLayerProperties);
    LogVulkanCoreExtensions(writer);

    const auto physicalDevices = InstanceUtil::EnumeratePhysicalDevices(instance.Get());
    writer.Print("Physical device count: {}", physicalDevices.size());
    for (std::size_t i = 0; i < physicalDevices.size(); ++i)
    {
      writer.Print("*** Physical device #{} ***", i);
      LogPhysicalDevice(writer, VUPhysicalDeviceRecord(physicalDevices[i]));
    }

    // for (std::size_t i = 0; i < physicalDevices.size(); ++i)
    //{
    //  VkDeviceCreateInfo deviceCreateInfo{};
    //  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    //  VUDevice device(physicalDevices[i], deviceCreateInfo);
    //}
  }
}
