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
#include <FslUtil/Vulkan1_0/Util/InstanceUtil.hpp>

namespace Fsl
{
  using namespace Vulkan;

  namespace
  {

    struct DecodeVulkanVersion
    {
      uint32_t Value;

      DecodeVulkanVersion(const uint32_t value)
      {
        Value = value;
      }
    };


    // https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#fundamentals-versionnum
    // The Vulkan version number is used in several places in the API.In each such use, the API major version number, minor version number,
    // and patch version number are packed into a 32 - bit integer as follows :
    // The major version number is a 10 - bit integer packed into bits 31 - 22.
    // The minor version number is a 10 - bit integer packed into bits 21 - 12.
    // The patch version number is a 12 - bit integer packed into bits 11 - 0.
    inline std::ostream &operator <<(std::ostream &o, const DecodeVulkanVersion& value)
    {
      uint32_t major = (value.Value >> 22) & ((1 << 10) - 1);
      uint32_t minor = (value.Value >> 12) & ((1 << 10) - 1);
      uint32_t patch = value.Value & ((1 << 12) - 1);
      return o << major << '.' << minor  << '.' << patch;
    }

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

      for(uint32_t i=0; i<3; ++i)
        FSLLOG("- limits.maxComputeWorkGroupCount[" << i<< "]: " << limits.maxComputeWorkGroupCount[i]);

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
      FSLLOG("- apiVersion: " << DecodeVulkanVersion(properties.apiVersion));
      FSLLOG("- driverVersion: " << DecodeVulkanVersion(properties.driverVersion));
      FSLLOG("- vendorID: " << properties.vendorID);
      FSLLOG("- deviceID: " << properties.deviceID);
      FSLLOG("- deviceType: " << properties.deviceType); // VkPhysicalDeviceType
      FSLLOG("- deviceName: " << properties.deviceName);
      //uint8_t                             pipelineCacheUUID[VK_UUID_SIZE];
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


    inline std::ostream &operator <<(std::ostream &o, const VkMemoryType& value)
    {
      return o << "{ propertyFlags: " << value.propertyFlags << ", heapIndex: " << value.heapIndex << " }";
    }

    inline std::ostream &operator <<(std::ostream &o, const VkMemoryHeap& value)
    {
      return o << "{ size: " << value.size << ", flags: " << value.flags << " }";
    }


    void LogPhysicalDeviceMemoryProperties(const VkPhysicalDeviceMemoryProperties& properties)
    {
      FSLLOG("Physical device memory properties:");
      FSLLOG("- memoryTypeCount: " << properties.memoryTypeCount);
      for(uint32_t i=0; i<properties.memoryTypeCount; ++i)
        FSLLOG("- memoryTypes[" << i << "]: " << properties.memoryTypes[i]);

      FSLLOG("- memoryHeapCount: " << properties.memoryHeapCount);
      for (uint32_t i = 0; i < properties.memoryHeapCount; ++i)
        FSLLOG("- memoryHeaps[" << i << "]: " << properties.memoryHeaps[i]);
    }

    void LogPhysicalDevice(const PhysicalDeviceRecord& physicalDevice)
    {
      LogPhysicalProperties(physicalDevice.GetPhysicalDeviceProperties());
      LogPhysicalDeviceFeatures(physicalDevice.GetPhysicalDeviceFeatures());
      LogPhysicalDeviceMemoryProperties(physicalDevice.GetPhysicalDeviceMemoryProperties());
    }

  }


  VulkanInfo::VulkanInfo(const DemoAppConfig& config)
    : DemoAppConsole(config)
  {
  }


  VulkanInfo::~VulkanInfo()
  {
  }


  void VulkanInfo::Run()
  {
    auto instance = InstanceUtil::CreateInstance("VulkanInfo", VK_MAKE_VERSION(1, 0, 0), VK_MAKE_VERSION(1, 0, 0), 0, 0, nullptr, 0, nullptr);

    const auto physicalDevices = InstanceUtil::EnumeratePhysicalDevices(instance.Get());
    FSLLOG("Physical device count: " << physicalDevices.size());
    for( std::size_t i=0; i<physicalDevices.size(); ++i)
    {
      FSLLOG("*** Physical device #" << i << " ***");
      LogPhysicalDevice(PhysicalDeviceRecord(physicalDevices[i]));


    }


    //Vulkan
  }
}
