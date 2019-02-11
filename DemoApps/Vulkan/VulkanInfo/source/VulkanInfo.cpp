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
#include <FslBase/Log/Log.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Log/All.hpp>
#include <FslUtil/Vulkan1_0/Util/InstanceUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/PhysicalDeviceUtil.hpp>
#include <FslUtil/Vulkan1_0/VUPhysicalDeviceRecord.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <RapidVulkan/Debug/Strings/VkFormatFeatureFlagBits.hpp>
#include <RapidVulkan/Debug/Strings/VkMemoryHeapFlagBits.hpp>
#include <RapidVulkan/Debug/Strings/VkMemoryPropertyFlagBits.hpp>
#include <RapidVulkan/Debug/Strings/VkPhysicalDeviceType.hpp>
#include <RapidVulkan/Debug/Strings/VkQueueFlagBits.hpp>
#include <ios>
// Included last as a workaround
#include <FslUtil/Vulkan1_0/Debug/BitFlags.hpp>

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


    void LogPhysicalDeviceLimits(const VkPhysicalDeviceLimits& limits)
    {
      FSLLOG("- limits.maxImageDimension1D: " << limits.maxImageDimension1D);
      FSLLOG("- limits.maxImageDimension2D: " << limits.maxImageDimension2D);
      FSLLOG("- limits.maxImageDimension3D: " << limits.maxImageDimension3D);
      FSLLOG("- limits.maxImageDimensionCube: " << limits.maxImageDimensionCube);
      FSLLOG("- limits.maxImageArrayLayers: " << limits.maxImageArrayLayers);
      FSLLOG("- limits.maxTexelBufferElements: " << limits.maxTexelBufferElements);
      FSLLOG("- limits.maxUniformBufferRange: " << limits.maxUniformBufferRange);
      FSLLOG("- limits.maxStorageBufferRange: " << limits.maxStorageBufferRange);
      FSLLOG("- limits.maxPushConstantsSize: " << limits.maxPushConstantsSize);
      FSLLOG("- limits.maxMemoryAllocationCount: " << limits.maxMemoryAllocationCount);
      FSLLOG("- limits.maxSamplerAllocationCount: " << limits.maxSamplerAllocationCount);
      FSLLOG("- limits.bufferImageGranularity: " << limits.bufferImageGranularity);
      FSLLOG("- limits.sparseAddressSpaceSize: " << limits.sparseAddressSpaceSize);
      FSLLOG("- limits.maxBoundDescriptorSets: " << limits.maxBoundDescriptorSets);
      FSLLOG("- limits.maxPerStageDescriptorSamplers: " << limits.maxPerStageDescriptorSamplers);
      FSLLOG("- limits.maxPerStageDescriptorUniformBuffers: " << limits.maxPerStageDescriptorUniformBuffers);
      FSLLOG("- limits.maxPerStageDescriptorStorageBuffers: " << limits.maxPerStageDescriptorStorageBuffers);
      FSLLOG("- limits.maxPerStageDescriptorSampledImages: " << limits.maxPerStageDescriptorSampledImages);
      FSLLOG("- limits.maxPerStageDescriptorStorageImages: " << limits.maxPerStageDescriptorStorageImages);
      FSLLOG("- limits.maxPerStageDescriptorInputAttachments: " << limits.maxPerStageDescriptorInputAttachments);
      FSLLOG("- limits.maxPerStageResources: " << limits.maxPerStageResources);
      FSLLOG("- limits.maxDescriptorSetSamplers: " << limits.maxDescriptorSetSamplers);
      FSLLOG("- limits.maxDescriptorSetUniformBuffers: " << limits.maxDescriptorSetUniformBuffers);
      FSLLOG("- limits.maxDescriptorSetUniformBuffersDynamic: " << limits.maxDescriptorSetUniformBuffersDynamic);
      FSLLOG("- limits.maxDescriptorSetStorageBuffers: " << limits.maxDescriptorSetStorageBuffers);
      FSLLOG("- limits.maxDescriptorSetStorageBuffersDynamic: " << limits.maxDescriptorSetStorageBuffersDynamic);
      FSLLOG("- limits.maxDescriptorSetSampledImages: " << limits.maxDescriptorSetSampledImages);
      FSLLOG("- limits.maxDescriptorSetStorageImages: " << limits.maxDescriptorSetStorageImages);
      FSLLOG("- limits.maxDescriptorSetInputAttachments: " << limits.maxDescriptorSetInputAttachments);
      FSLLOG("- limits.maxVertexInputAttributes: " << limits.maxVertexInputAttributes);
      FSLLOG("- limits.maxVertexInputBindings: " << limits.maxVertexInputBindings);
      FSLLOG("- limits.maxVertexInputAttributeOffset: " << limits.maxVertexInputAttributeOffset);
      FSLLOG("- limits.maxVertexInputBindingStride: " << limits.maxVertexInputBindingStride);
      FSLLOG("- limits.maxVertexOutputComponents: " << limits.maxVertexOutputComponents);
      FSLLOG("- limits.maxTessellationGenerationLevel: " << limits.maxTessellationGenerationLevel);
      FSLLOG("- limits.maxTessellationPatchSize: " << limits.maxTessellationPatchSize);
      FSLLOG("- limits.maxTessellationControlPerVertexInputComponents: " << limits.maxTessellationControlPerVertexInputComponents);
      FSLLOG("- limits.maxTessellationControlPerVertexOutputComponents: " << limits.maxTessellationControlPerVertexOutputComponents);
      FSLLOG("- limits.maxTessellationControlPerPatchOutputComponents: " << limits.maxTessellationControlPerPatchOutputComponents);
      FSLLOG("- limits.maxTessellationControlTotalOutputComponents: " << limits.maxTessellationControlTotalOutputComponents);
      FSLLOG("- limits.maxTessellationEvaluationInputComponents: " << limits.maxTessellationEvaluationInputComponents);
      FSLLOG("- limits.maxTessellationEvaluationOutputComponents: " << limits.maxTessellationEvaluationOutputComponents);
      FSLLOG("- limits.maxGeometryShaderInvocations: " << limits.maxGeometryShaderInvocations);
      FSLLOG("- limits.maxGeometryInputComponents: " << limits.maxGeometryInputComponents);
      FSLLOG("- limits.maxGeometryOutputComponents: " << limits.maxGeometryOutputComponents);

      FSLLOG("- limits.maxGeometryOutputVertices: " << limits.maxGeometryOutputVertices);
      FSLLOG("- limits.maxGeometryTotalOutputComponents: " << limits.maxGeometryTotalOutputComponents);
      FSLLOG("- limits.maxFragmentInputComponents: " << limits.maxFragmentInputComponents);
      FSLLOG("- limits.maxFragmentOutputAttachments: " << limits.maxFragmentOutputAttachments);
      FSLLOG("- limits.maxFragmentDualSrcAttachments: " << limits.maxFragmentDualSrcAttachments);
      FSLLOG("- limits.maxFragmentCombinedOutputResources: " << limits.maxFragmentCombinedOutputResources);
      FSLLOG("- limits.maxComputeSharedMemorySize: " << limits.maxComputeSharedMemorySize);

      for (uint32_t i = 0; i < 3; ++i)
        FSLLOG("- limits.maxComputeWorkGroupCount[" << i << "]: " << limits.maxComputeWorkGroupCount[i]);

      FSLLOG("- limits.maxComputeWorkGroupInvocations: " << limits.maxComputeWorkGroupInvocations);

      for (uint32_t i = 0; i < 3; ++i)
        FSLLOG("- limits.maxComputeWorkGroupSize[" << i << "]: " << limits.maxComputeWorkGroupSize[i]);

      FSLLOG("- limits.subPixelPrecisionBits: " << limits.subPixelPrecisionBits);
      FSLLOG("- limits.subTexelPrecisionBits: " << limits.subTexelPrecisionBits);
      FSLLOG("- limits.mipmapPrecisionBits: " << limits.mipmapPrecisionBits);
      FSLLOG("- limits.maxDrawIndexedIndexValue: " << limits.maxDrawIndexedIndexValue);
      FSLLOG("- limits.maxDrawIndirectCount: " << limits.maxDrawIndirectCount);
      FSLLOG("- limits.maxSamplerLodBias: " << limits.maxSamplerLodBias);
      FSLLOG("- limits.maxSamplerAnisotropy: " << limits.maxSamplerAnisotropy);
      FSLLOG("- limits.maxViewports: " << limits.maxViewports);

      for (uint32_t i = 0; i < 2; ++i)
        FSLLOG("- limits.maxViewportDimensions[" << i << "]: " << limits.maxViewportDimensions[i]);
      for (uint32_t i = 0; i < 2; ++i)
        FSLLOG("- limits.viewportBoundsRange[" << i << "]: " << limits.viewportBoundsRange[i]);

      FSLLOG("- limits.viewportSubPixelBits: " << limits.viewportSubPixelBits);
      FSLLOG("- limits.minMemoryMapAlignment: " << limits.minMemoryMapAlignment);
      FSLLOG("- limits.minTexelBufferOffsetAlignment: " << limits.minTexelBufferOffsetAlignment);
      FSLLOG("- limits.minUniformBufferOffsetAlignment: " << limits.minUniformBufferOffsetAlignment);
      FSLLOG("- limits.minStorageBufferOffsetAlignment: " << limits.minStorageBufferOffsetAlignment);
      FSLLOG("- limits.minTexelOffset: " << limits.minTexelOffset);
      FSLLOG("- limits.maxTexelOffset: " << limits.maxTexelOffset);
      FSLLOG("- limits.minTexelGatherOffset: " << limits.minTexelGatherOffset);
      FSLLOG("- limits.maxTexelGatherOffset: " << limits.maxTexelGatherOffset);
      FSLLOG("- limits.minInterpolationOffset: " << limits.minInterpolationOffset);
      FSLLOG("- limits.maxInterpolationOffset: " << limits.maxInterpolationOffset);
      FSLLOG("- limits.subPixelInterpolationOffsetBits: " << limits.subPixelInterpolationOffsetBits);
      FSLLOG("- limits.maxFramebufferWidth: " << limits.maxFramebufferWidth);
      FSLLOG("- limits.maxFramebufferHeight: " << limits.maxFramebufferHeight);
      FSLLOG("- limits.maxFramebufferLayers: " << limits.maxFramebufferLayers);
      FSLLOG("- limits.framebufferColorSampleCounts: " << limits.framebufferColorSampleCounts);
      FSLLOG("- limits.framebufferDepthSampleCounts: " << limits.framebufferDepthSampleCounts);
      FSLLOG("- limits.framebufferStencilSampleCounts: " << limits.framebufferStencilSampleCounts);
      FSLLOG("- limits.framebufferNoAttachmentsSampleCounts: " << limits.framebufferNoAttachmentsSampleCounts);
      FSLLOG("- limits.maxColorAttachments: " << limits.maxColorAttachments);
      FSLLOG("- limits.sampledImageColorSampleCounts: " << limits.sampledImageColorSampleCounts);
      FSLLOG("- limits.sampledImageIntegerSampleCounts: " << limits.sampledImageIntegerSampleCounts);
      FSLLOG("- limits.sampledImageDepthSampleCounts: " << limits.sampledImageDepthSampleCounts);
      FSLLOG("- limits.sampledImageStencilSampleCounts: " << limits.sampledImageStencilSampleCounts);
      FSLLOG("- limits.storageImageSampleCounts: " << limits.storageImageSampleCounts);
      FSLLOG("- limits.maxSampleMaskWords: " << limits.maxSampleMaskWords);
      FSLLOG("- limits.timestampComputeAndGraphics: " << limits.timestampComputeAndGraphics);

      FSLLOG("- limits.timestampPeriod: " << limits.timestampPeriod);
      FSLLOG("- limits.maxClipDistances: " << limits.maxClipDistances);
      FSLLOG("- limits.maxCullDistances: " << limits.maxCullDistances);
      FSLLOG("- limits.maxCombinedClipAndCullDistances: " << limits.maxCombinedClipAndCullDistances);
      FSLLOG("- limits.discreteQueuePriorities: " << limits.discreteQueuePriorities);

      for (uint32_t i = 0; i < 2; ++i)
        FSLLOG("- limits.pointSizeRange[" << i << "]: " << limits.pointSizeRange[i]);

      for (uint32_t i = 0; i < 2; ++i)
        FSLLOG("- limits.lineWidthRange[" << i << "]: " << limits.lineWidthRange[i]);

      FSLLOG("- limits.pointSizeGranularity: " << limits.pointSizeGranularity);
      FSLLOG("- limits.lineWidthGranularity: " << limits.lineWidthGranularity);
      FSLLOG("- limits.strictLines: " << limits.strictLines);
      FSLLOG("- limits.standardSampleLocations: " << limits.standardSampleLocations);
      FSLLOG("- limits.optimalBufferCopyOffsetAlignment: " << limits.optimalBufferCopyOffsetAlignment);
      FSLLOG("- limits.optimalBufferCopyRowPitchAlignment: " << limits.optimalBufferCopyRowPitchAlignment);
      FSLLOG("- limits.nonCoherentAtomSize: " << limits.nonCoherentAtomSize);
    }

    void LogPhysicalDeviceSparseProperties(const VkPhysicalDeviceSparseProperties& properties)
    {
      FSLLOG("- sparseProperties.residencyStandard2DBlockShape: " << properties.residencyStandard2DBlockShape);
      FSLLOG("- sparseProperties.residencyStandard2DMultisampleBlockShape: " << properties.residencyStandard2DMultisampleBlockShape);
      FSLLOG("- sparseProperties.residencyStandard3DBlockShape: " << properties.residencyStandard3DBlockShape);
      FSLLOG("- sparseProperties.residencyAlignedMipSize: " << properties.residencyAlignedMipSize);
      FSLLOG("- sparseProperties.residencyNonResidentStrict: " << properties.residencyNonResidentStrict);
    }

    void LogPhysicalProperties(const VkPhysicalDeviceProperties& properties)
    {
      FSLLOG("Physical device properties:");
      FSLLOG("- apiVersion: " << EncodedVulkanVersion(properties.apiVersion));
      FSLLOG("- driverVersion: " << EncodedVulkanVersion(properties.driverVersion));
      FSLLOG("- vendorID: " << properties.vendorID);
      FSLLOG("- deviceID: " << properties.deviceID);
      FSLLOG("- deviceType: " << Debug::GetBitflagsString(properties.deviceType));    // VkPhysicalDeviceType
      FSLLOG("- deviceName: " << properties.deviceName);
      // uint8_t                             pipelineCacheUUID[VK_UUID_SIZE];
      LogPhysicalDeviceLimits(properties.limits);
      LogPhysicalDeviceSparseProperties(properties.sparseProperties);
    }


    void LogPhysicalDeviceFeatures(const VkPhysicalDeviceFeatures& features)
    {
      FSLLOG("Physical device features:");
      FSLLOG("- robustBufferAccess: " << features.robustBufferAccess);
      FSLLOG("- fullDrawIndexUint32: " << features.fullDrawIndexUint32);
      FSLLOG("- imageCubeArray: " << features.imageCubeArray);
      FSLLOG("- independentBlend: " << features.independentBlend);
      FSLLOG("- geometryShader: " << features.geometryShader);
      FSLLOG("- tessellationShader: " << features.tessellationShader);
      FSLLOG("- sampleRateShading: " << features.sampleRateShading);
      FSLLOG("- dualSrcBlend: " << features.dualSrcBlend);
      FSLLOG("- logicOp: " << features.logicOp);
      FSLLOG("- multiDrawIndirect: " << features.multiDrawIndirect);
      FSLLOG("- drawIndirectFirstInstance: " << features.drawIndirectFirstInstance);
      FSLLOG("- depthClamp: " << features.depthClamp);
      FSLLOG("- depthBiasClamp: " << features.depthBiasClamp);
      FSLLOG("- fillModeNonSolid: " << features.fillModeNonSolid);
      FSLLOG("- depthBounds: " << features.depthBounds);
      FSLLOG("- wideLines: " << features.wideLines);
      FSLLOG("- largePoints: " << features.largePoints);
      FSLLOG("- alphaToOne: " << features.alphaToOne);
      FSLLOG("- multiViewport: " << features.multiViewport);
      FSLLOG("- samplerAnisotropy: " << features.samplerAnisotropy);
      FSLLOG("- textureCompressionETC2: " << features.textureCompressionETC2);
      FSLLOG("- textureCompressionASTC_LDR: " << features.textureCompressionASTC_LDR);
      FSLLOG("- textureCompressionBC: " << features.textureCompressionBC);
      FSLLOG("- occlusionQueryPrecise: " << features.occlusionQueryPrecise);
      FSLLOG("- pipelineStatisticsQuery: " << features.pipelineStatisticsQuery);
      FSLLOG("- vertexPipelineStoresAndAtomics: " << features.vertexPipelineStoresAndAtomics);
      FSLLOG("- fragmentStoresAndAtomics: " << features.fragmentStoresAndAtomics);
      FSLLOG("- shaderTessellationAndGeometryPointSize: " << features.shaderTessellationAndGeometryPointSize);
      FSLLOG("- shaderImageGatherExtended: " << features.shaderImageGatherExtended);
      FSLLOG("- shaderStorageImageExtendedFormats: " << features.shaderStorageImageExtendedFormats);
      FSLLOG("- shaderStorageImageMultisample: " << features.shaderStorageImageMultisample);
      FSLLOG("- shaderStorageImageReadWithoutFormat: " << features.shaderStorageImageReadWithoutFormat);
      FSLLOG("- shaderStorageImageWriteWithoutFormat: " << features.shaderStorageImageWriteWithoutFormat);
      FSLLOG("- shaderUniformBufferArrayDynamicIndexing: " << features.shaderUniformBufferArrayDynamicIndexing);
      FSLLOG("- shaderSampledImageArrayDynamicIndexing: " << features.shaderSampledImageArrayDynamicIndexing);
      FSLLOG("- shaderStorageBufferArrayDynamicIndexing: " << features.shaderStorageBufferArrayDynamicIndexing);
      FSLLOG("- shaderStorageImageArrayDynamicIndexing: " << features.shaderStorageImageArrayDynamicIndexing);
      FSLLOG("- shaderClipDistance: " << features.shaderClipDistance);
      FSLLOG("- shaderCullDistance: " << features.shaderCullDistance);
      FSLLOG("- shaderFloat64: " << features.shaderFloat64);
      FSLLOG("- shaderInt64: " << features.shaderInt64);
      FSLLOG("- shaderInt16: " << features.shaderInt16);
      FSLLOG("- shaderResourceResidency: " << features.shaderResourceResidency);
      FSLLOG("- shaderResourceMinLod: " << features.shaderResourceMinLod);
      FSLLOG("- sparseBinding: " << features.sparseBinding);
      FSLLOG("- sparseResidencyBuffer: " << features.sparseResidencyBuffer);
      FSLLOG("- sparseResidencyImage2D: " << features.sparseResidencyImage2D);
      FSLLOG("- sparseResidencyImage3D: " << features.sparseResidencyImage3D);
      FSLLOG("- sparseResidency2Samples: " << features.sparseResidency2Samples);
      FSLLOG("- sparseResidency4Samples: " << features.sparseResidency4Samples);
      FSLLOG("- sparseResidency8Samples: " << features.sparseResidency8Samples);
      FSLLOG("- sparseResidency16Samples: " << features.sparseResidency16Samples);
      FSLLOG("- sparseResidencyAliased: " << features.sparseResidencyAliased);
      FSLLOG("- variableMultisampleRate: " << features.variableMultisampleRate);
      FSLLOG("- inheritedQueries: " << features.inheritedQueries);
    }

    inline std::ostream& operator<<(std::ostream& o, const VkMemoryType& value)
    {
      return o << "{ propertyFlags: " << Debug::GetBitflagsString(static_cast<VkMemoryPropertyFlagBits>(value.propertyFlags))
               << ", heapIndex: " << value.heapIndex << " }";
    }

    inline std::ostream& operator<<(std::ostream& o, const VkMemoryHeap& value)
    {
      return o << "{ size: " << value.size << ", flags: " << Debug::GetBitflagsString(static_cast<VkMemoryHeapFlagBits>(value.flags)) << " }";
    }


    void LogPhysicalDeviceMemoryProperties(const VkPhysicalDeviceMemoryProperties& properties)
    {
      FSLLOG("Physical device memory properties:");
      FSLLOG("- memoryTypeCount: " << properties.memoryTypeCount);
      for (uint32_t i = 0; i < properties.memoryTypeCount; ++i)
      {
        FSLLOG("- memoryTypes[" << i << "]: " << properties.memoryTypes[i]);
      }

      FSLLOG("- memoryHeapCount: " << properties.memoryHeapCount);
      for (uint32_t i = 0; i < properties.memoryHeapCount; ++i)
      {
        FSLLOG("- memoryHeaps[" << i << "]: " << properties.memoryHeaps[i]);
      }
    }

    void LogPhysicalDeviceQueueFamilyProperties(const std::vector<VkQueueFamilyProperties>& queueFamilyProperties)
    {
      FSLLOG("Physical device queue family properties:");
      for (std::size_t i = 0; i < queueFamilyProperties.size(); ++i)
      {
        FSLLOG("- Queue Famility #" << i);
        FSLLOG("  - queueFlags: " << Debug::GetBitflagsString(static_cast<VkQueueFlagBits>(queueFamilyProperties[i].queueFlags)));
        FSLLOG("  - queueCount: " << queueFamilyProperties[i].queueCount);
        FSLLOG("  - timestampValidBits: " << queueFamilyProperties[i].timestampValidBits);
        FSLLOG("  - minImageTransferGranularity: " << queueFamilyProperties[i].minImageTransferGranularity);
      }
    }

    void LogAllFormats(const VUPhysicalDeviceRecord& physicalDevice)
    {
      FSLLOG("All phsycial device format properties:");
      for (std::size_t i = 0; i < g_allFormats.size(); ++i)
      {
        const auto format = g_allFormats[i];
        FSLLOG("- Format #" << i << " (" << format << ")");

        VkFormatProperties formatProperties = PhysicalDeviceUtil::GetPhysicalDeviceFormatProperties(physicalDevice.Device, format);

        FSLLOG("  - linearTilingFeatures: " << Debug::GetBitflagsString(static_cast<VkFormatFeatureFlagBits>(formatProperties.linearTilingFeatures)));
        FSLLOG(
          "  - optimalTilingFeatures: " << Debug::GetBitflagsString(static_cast<VkFormatFeatureFlagBits>(formatProperties.optimalTilingFeatures)));
        FSLLOG("  - bufferFeatures: " << Debug::GetBitflagsString(static_cast<VkFormatFeatureFlagBits>(formatProperties.bufferFeatures)));
      }
    }

    void LogPhysicalDevice(const VUPhysicalDeviceRecord& physicalDevice)
    {
      LogPhysicalProperties(physicalDevice.Properties);
      LogPhysicalDeviceFeatures(physicalDevice.Features);
      LogPhysicalDeviceMemoryProperties(physicalDevice.MemoryProperties);

      LogPhysicalDeviceQueueFamilyProperties(PhysicalDeviceUtil::GetPhysicalDeviceQueueFamilyProperties(physicalDevice.Device));

      LogAllFormats(physicalDevice);
    }

    void LogInstanceLayerProperties(const std::vector<VkLayerProperties>& layerProperties)
    {
      FSLLOG("Instance layer properties: " << layerProperties.size());
      for (std::size_t i = 0; i < layerProperties.size(); ++i)
      {
        FSLLOG("- layer #" << i);
        FSLLOG("  - name: " << layerProperties[i].layerName);
        FSLLOG("  - specVersion: " << EncodedVulkanVersion(layerProperties[i].specVersion));
        FSLLOG("  - implementationVersion: " << layerProperties[i].implementationVersion);
        FSLLOG("  - description: " << layerProperties[i].description);
        auto extensionProperties = InstanceUtil::EnumerateInstanceExtensionProperties(layerProperties[i].layerName);
        if (!extensionProperties.empty())
        {
          FSLLOG("  - Extension properties:");
          for (std::size_t j = 0; j < extensionProperties.size(); ++j)
          {
            FSLLOG("    - property #" << j);
            FSLLOG("    - name: " << extensionProperties[j].extensionName);
            FSLLOG("    - specVersion: " << extensionProperties[j].specVersion);
          }
        }
      }
    }

    void LogVulkanCoreExtensions()
    {
      auto extensionProperties = InstanceUtil::EnumerateInstanceExtensionProperties(nullptr);
      FSLLOG("Core extensions: " << extensionProperties.size());
      for (std::size_t j = 0; j < extensionProperties.size(); ++j)
      {
        FSLLOG("- Extension #" << j);
        FSLLOG("  - name: " << extensionProperties[j].extensionName);
        FSLLOG("  - specVersion: " << extensionProperties[j].specVersion);
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
    auto instance = InstanceUtil::CreateInstance("VulkanInfo", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0, 0, 0, nullptr, 0, nullptr);

    auto instanceLayerProperties = InstanceUtil::EnumerateInstanceLayerProperties();
    LogInstanceLayerProperties(instanceLayerProperties);
    LogVulkanCoreExtensions();

    const auto physicalDevices = InstanceUtil::EnumeratePhysicalDevices(instance.Get());
    FSLLOG("Physical device count: " << physicalDevices.size());
    for (std::size_t i = 0; i < physicalDevices.size(); ++i)
    {
      FSLLOG("*** Physical device #" << i << " ***");
      LogPhysicalDevice(VUPhysicalDeviceRecord(physicalDevices[i]));
    }

    // for (std::size_t i = 0; i < physicalDevices.size(); ++i)
    //{
    //  VkDeviceCreateInfo deviceCreateInfo{};
    //  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    //  VUDevice device(physicalDevices[i], deviceCreateInfo);
    //}
  }
}
