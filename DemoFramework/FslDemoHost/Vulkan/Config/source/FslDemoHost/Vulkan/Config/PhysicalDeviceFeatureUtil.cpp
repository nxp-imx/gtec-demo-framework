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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <FslDemoHost/Vulkan/Config/PhysicalDeviceFeatureUtil.hpp>
#include <FslBase/Exceptions.hpp>
#include <cstddef>
#include <type_traits>

namespace Fsl
{
  namespace Vulkan
  {
    namespace
    {
      //! @brief Based on VkPhysicalDeviceFeatures
      bool TryGetOffset(const PhysicalDeviceFeature featureName, std::size_t& rOffset)
      {
        switch (featureName)
        {
        case PhysicalDeviceFeature::RobustBufferAccess:
          rOffset = offsetof(VkPhysicalDeviceFeatures, robustBufferAccess);
          return true;
        case PhysicalDeviceFeature::FullDrawIndexUint32:
          rOffset = offsetof(VkPhysicalDeviceFeatures, fullDrawIndexUint32);
          return true;
        case PhysicalDeviceFeature::ImageCubeArray:
          rOffset = offsetof(VkPhysicalDeviceFeatures, imageCubeArray);
          return true;
        case PhysicalDeviceFeature::IndependentBlend:
          rOffset = offsetof(VkPhysicalDeviceFeatures, independentBlend);
          return true;
        case PhysicalDeviceFeature::GeometryShader:
          rOffset = offsetof(VkPhysicalDeviceFeatures, geometryShader);
          return true;
        case PhysicalDeviceFeature::TessellationShader:
          rOffset = offsetof(VkPhysicalDeviceFeatures, tessellationShader);
          return true;
        case PhysicalDeviceFeature::SampleRateShading:
          rOffset = offsetof(VkPhysicalDeviceFeatures, sampleRateShading);
          return true;
        case PhysicalDeviceFeature::DualSrcBlend:
          rOffset = offsetof(VkPhysicalDeviceFeatures, dualSrcBlend);
          return true;
        case PhysicalDeviceFeature::LogicOp:
          rOffset = offsetof(VkPhysicalDeviceFeatures, logicOp);
          return true;
        case PhysicalDeviceFeature::MultiDrawIndirect:
          rOffset = offsetof(VkPhysicalDeviceFeatures, multiDrawIndirect);
          return true;
        case PhysicalDeviceFeature::DrawIndirectFirstInstance:
          rOffset = offsetof(VkPhysicalDeviceFeatures, drawIndirectFirstInstance);
          return true;
        case PhysicalDeviceFeature::DepthClamp:
          rOffset = offsetof(VkPhysicalDeviceFeatures, depthClamp);
          return true;
        case PhysicalDeviceFeature::DepthBiasClamp:
          rOffset = offsetof(VkPhysicalDeviceFeatures, depthBiasClamp);
          return true;
        case PhysicalDeviceFeature::FillModeNonSolid:
          rOffset = offsetof(VkPhysicalDeviceFeatures, fillModeNonSolid);
          return true;
        case PhysicalDeviceFeature::DepthBounds:
          rOffset = offsetof(VkPhysicalDeviceFeatures, depthBounds);
          return true;
        case PhysicalDeviceFeature::WideLines:
          rOffset = offsetof(VkPhysicalDeviceFeatures, wideLines);
          return true;
        case PhysicalDeviceFeature::LargePoints:
          rOffset = offsetof(VkPhysicalDeviceFeatures, largePoints);
          return true;
        case PhysicalDeviceFeature::AlphaToOne:
          rOffset = offsetof(VkPhysicalDeviceFeatures, alphaToOne);
          return true;
        case PhysicalDeviceFeature::MultiViewport:
          rOffset = offsetof(VkPhysicalDeviceFeatures, multiViewport);
          return true;
        case PhysicalDeviceFeature::SamplerAnisotropy:
          rOffset = offsetof(VkPhysicalDeviceFeatures, samplerAnisotropy);
          return true;
        case PhysicalDeviceFeature::TextureCompressionETC2:
          rOffset = offsetof(VkPhysicalDeviceFeatures, textureCompressionETC2);
          return true;
        case PhysicalDeviceFeature::TextureCompressionASTC_LDR:
          rOffset = offsetof(VkPhysicalDeviceFeatures, textureCompressionASTC_LDR);
          return true;
        case PhysicalDeviceFeature::TextureCompressionBC:
          rOffset = offsetof(VkPhysicalDeviceFeatures, textureCompressionBC);
          return true;
        case PhysicalDeviceFeature::OcclusionQueryPrecise:
          rOffset = offsetof(VkPhysicalDeviceFeatures, occlusionQueryPrecise);
          return true;
        case PhysicalDeviceFeature::PipelineStatisticsQuery:
          rOffset = offsetof(VkPhysicalDeviceFeatures, pipelineStatisticsQuery);
          return true;
        case PhysicalDeviceFeature::VertexPipelineStoresAndAtomics:
          rOffset = offsetof(VkPhysicalDeviceFeatures, vertexPipelineStoresAndAtomics);
          return true;
        case PhysicalDeviceFeature::FragmentStoresAndAtomics:
          rOffset = offsetof(VkPhysicalDeviceFeatures, fragmentStoresAndAtomics);
          return true;
        case PhysicalDeviceFeature::ShaderTessellationAndGeometryPointSize:
          rOffset = offsetof(VkPhysicalDeviceFeatures, shaderTessellationAndGeometryPointSize);
          return true;
        case PhysicalDeviceFeature::ShaderImageGatherExtended:
          rOffset = offsetof(VkPhysicalDeviceFeatures, shaderImageGatherExtended);
          return true;
        case PhysicalDeviceFeature::ShaderStorageImageExtendedFormats:
          rOffset = offsetof(VkPhysicalDeviceFeatures, shaderStorageImageExtendedFormats);
          return true;
        case PhysicalDeviceFeature::ShaderStorageImageMultisample:
          rOffset = offsetof(VkPhysicalDeviceFeatures, shaderStorageImageMultisample);
          return true;
        case PhysicalDeviceFeature::ShaderStorageImageReadWithoutFormat:
          rOffset = offsetof(VkPhysicalDeviceFeatures, shaderStorageImageReadWithoutFormat);
          return true;
        case PhysicalDeviceFeature::ShaderStorageImageWriteWithoutFormat:
          rOffset = offsetof(VkPhysicalDeviceFeatures, shaderStorageImageWriteWithoutFormat);
          return true;
        case PhysicalDeviceFeature::ShaderUniformBufferArrayDynamicIndexing:
          rOffset = offsetof(VkPhysicalDeviceFeatures, shaderUniformBufferArrayDynamicIndexing);
          return true;
        case PhysicalDeviceFeature::ShaderSampledImageArrayDynamicIndexing:
          rOffset = offsetof(VkPhysicalDeviceFeatures, shaderSampledImageArrayDynamicIndexing);
          return true;
        case PhysicalDeviceFeature::ShaderStorageBufferArrayDynamicIndexing:
          rOffset = offsetof(VkPhysicalDeviceFeatures, shaderStorageBufferArrayDynamicIndexing);
          return true;
        case PhysicalDeviceFeature::ShaderStorageImageArrayDynamicIndexing:
          rOffset = offsetof(VkPhysicalDeviceFeatures, shaderStorageImageArrayDynamicIndexing);
          return true;
        case PhysicalDeviceFeature::ShaderClipDistance:
          rOffset = offsetof(VkPhysicalDeviceFeatures, shaderClipDistance);
          return true;
        case PhysicalDeviceFeature::ShaderCullDistance:
          rOffset = offsetof(VkPhysicalDeviceFeatures, shaderCullDistance);
          return true;
        case PhysicalDeviceFeature::ShaderFloat64:
          rOffset = offsetof(VkPhysicalDeviceFeatures, shaderFloat64);
          return true;
        case PhysicalDeviceFeature::ShaderInt64:
          rOffset = offsetof(VkPhysicalDeviceFeatures, shaderInt64);
          return true;
        case PhysicalDeviceFeature::ShaderInt16:
          rOffset = offsetof(VkPhysicalDeviceFeatures, shaderInt16);
          return true;
        case PhysicalDeviceFeature::ShaderResourceResidency:
          rOffset = offsetof(VkPhysicalDeviceFeatures, shaderResourceResidency);
          return true;
        case PhysicalDeviceFeature::ShaderResourceMinLod:
          rOffset = offsetof(VkPhysicalDeviceFeatures, shaderResourceMinLod);
          return true;
        case PhysicalDeviceFeature::SparseBinding:
          rOffset = offsetof(VkPhysicalDeviceFeatures, sparseBinding);
          return true;
        case PhysicalDeviceFeature::SparseResidencyBuffer:
          rOffset = offsetof(VkPhysicalDeviceFeatures, sparseResidencyBuffer);
          return true;
        case PhysicalDeviceFeature::SparseResidencyImage2D:
          rOffset = offsetof(VkPhysicalDeviceFeatures, sparseResidencyImage2D);
          return true;
        case PhysicalDeviceFeature::SparseResidencyImage3D:
          rOffset = offsetof(VkPhysicalDeviceFeatures, sparseResidencyImage3D);
          return true;
        case PhysicalDeviceFeature::SparseResidency2Samples:
          rOffset = offsetof(VkPhysicalDeviceFeatures, sparseResidency2Samples);
          return true;
        case PhysicalDeviceFeature::SparseResidency4Samples:
          rOffset = offsetof(VkPhysicalDeviceFeatures, sparseResidency4Samples);
          return true;
        case PhysicalDeviceFeature::SparseResidency8Samples:
          rOffset = offsetof(VkPhysicalDeviceFeatures, sparseResidency8Samples);
          return true;
        case PhysicalDeviceFeature::SparseResidency16Samples:
          rOffset = offsetof(VkPhysicalDeviceFeatures, sparseResidency16Samples);
          return true;
        case PhysicalDeviceFeature::SparseResidencyAliased:
          rOffset = offsetof(VkPhysicalDeviceFeatures, sparseResidencyAliased);
          return true;
        case PhysicalDeviceFeature::VariableMultisampleRate:
          rOffset = offsetof(VkPhysicalDeviceFeatures, variableMultisampleRate);
          return true;
        case PhysicalDeviceFeature::InheritedQueries:
          rOffset = offsetof(VkPhysicalDeviceFeatures, inheritedQueries);
          return true;
        case PhysicalDeviceFeature::Invalid:
        default:
          rOffset = 0;
          return false;
        }
      }
    }


    VkBool32 PhysicalDeviceFeatureUtil::Get(const VkPhysicalDeviceFeatures& features, const PhysicalDeviceFeature featureName)
    {
      std::size_t offset;
      if (!TryGetOffset(featureName, offset))
      {
        throw std::invalid_argument("Unknown Vulkan PhysicalDeviceFeature");
      }

      // Nasty lookup that breaks without warning if the type changes from VkBool32 in VkPhysicalDeviceFeatures (this is the reason we have the static
      // asserts at the bottom of this file)
      const auto pMember = reinterpret_cast<const VkBool32*>((reinterpret_cast<const uint8_t*>(&features) + offset));
      return *pMember;
    }


    void PhysicalDeviceFeatureUtil::Set(VkPhysicalDeviceFeatures& rFeatures, const PhysicalDeviceFeature featureName, const VkBool32 value)
    {
      std::size_t offset;
      if (!TryGetOffset(featureName, offset))
      {
        throw std::invalid_argument("Unknown Vulkan PhysicalDeviceFeature");
      }

      // Nasty lookup that breaks without warning if the type changes from VkBool32 in VkPhysicalDeviceFeatures (this is the reason we have the static
      // asserts at the bottom of this file)
      auto pMember = reinterpret_cast<VkBool32*>((reinterpret_cast<uint8_t*>(&rFeatures) + offset));
      *pMember = value;
    }


    const char* PhysicalDeviceFeatureUtil::ToString(const PhysicalDeviceFeature featureName)
    {
      switch (featureName)
      {
      case PhysicalDeviceFeature::RobustBufferAccess:
        return "RobustBufferAccess";
      case PhysicalDeviceFeature::FullDrawIndexUint32:
        return "FullDrawIndexUint32";
      case PhysicalDeviceFeature::ImageCubeArray:
        return "ImageCubeArray";
      case PhysicalDeviceFeature::IndependentBlend:
        return "IndependentBlend";
      case PhysicalDeviceFeature::GeometryShader:
        return "GeometryShader";
      case PhysicalDeviceFeature::TessellationShader:
        return "TessellationShader";
      case PhysicalDeviceFeature::SampleRateShading:
        return "SampleRateShading";
      case PhysicalDeviceFeature::DualSrcBlend:
        return "DualSrcBlend";
      case PhysicalDeviceFeature::LogicOp:
        return "LogicOp";
      case PhysicalDeviceFeature::MultiDrawIndirect:
        return "MultiDrawIndirect";
      case PhysicalDeviceFeature::DrawIndirectFirstInstance:
        return "DrawIndirectFirstInstance";
      case PhysicalDeviceFeature::DepthClamp:
        return "DepthClamp";
      case PhysicalDeviceFeature::DepthBiasClamp:
        return "DepthBiasClamp";
      case PhysicalDeviceFeature::FillModeNonSolid:
        return "FillModeNonSolid";
      case PhysicalDeviceFeature::DepthBounds:
        return "DepthBounds";
      case PhysicalDeviceFeature::WideLines:
        return "WideLines";
      case PhysicalDeviceFeature::LargePoints:
        return "LargePoints";
      case PhysicalDeviceFeature::AlphaToOne:
        return "AlphaToOne";
      case PhysicalDeviceFeature::MultiViewport:
        return "MultiViewport";
      case PhysicalDeviceFeature::SamplerAnisotropy:
        return "SamplerAnisotropy";
      case PhysicalDeviceFeature::TextureCompressionETC2:
        return "TextureCompressionETC2";
      case PhysicalDeviceFeature::TextureCompressionASTC_LDR:
        return "TextureCompressionASTC_LDR";
      case PhysicalDeviceFeature::TextureCompressionBC:
        return "TextureCompressionBC";
      case PhysicalDeviceFeature::OcclusionQueryPrecise:
        return "OcclusionQueryPrecise";
      case PhysicalDeviceFeature::PipelineStatisticsQuery:
        return "PipelineStatisticsQuery";
      case PhysicalDeviceFeature::VertexPipelineStoresAndAtomics:
        return "VertexPipelineStoresAndAtomics";
      case PhysicalDeviceFeature::FragmentStoresAndAtomics:
        return "FragmentStoresAndAtomics";
      case PhysicalDeviceFeature::ShaderTessellationAndGeometryPointSize:
        return "ShaderTessellationAndGeometryPointSize";
      case PhysicalDeviceFeature::ShaderImageGatherExtended:
        return "ShaderImageGatherExtended";
      case PhysicalDeviceFeature::ShaderStorageImageExtendedFormats:
        return "ShaderStorageImageExtendedFormats";
      case PhysicalDeviceFeature::ShaderStorageImageMultisample:
        return "ShaderStorageImageMultisample";
      case PhysicalDeviceFeature::ShaderStorageImageReadWithoutFormat:
        return "ShaderStorageImageReadWithoutFormat";
      case PhysicalDeviceFeature::ShaderStorageImageWriteWithoutFormat:
        return "ShaderStorageImageWriteWithoutFormat";
      case PhysicalDeviceFeature::ShaderUniformBufferArrayDynamicIndexing:
        return "ShaderUniformBufferArrayDynamicIndexing";
      case PhysicalDeviceFeature::ShaderSampledImageArrayDynamicIndexing:
        return "ShaderSampledImageArrayDynamicIndexing";
      case PhysicalDeviceFeature::ShaderStorageBufferArrayDynamicIndexing:
        return "ShaderStorageBufferArrayDynamicIndexing";
      case PhysicalDeviceFeature::ShaderStorageImageArrayDynamicIndexing:
        return "ShaderStorageImageArrayDynamicIndexing";
      case PhysicalDeviceFeature::ShaderClipDistance:
        return "ShaderClipDistance";
      case PhysicalDeviceFeature::ShaderCullDistance:
        return "ShaderCullDistance";
      case PhysicalDeviceFeature::ShaderFloat64:
        return "ShaderFloat64";
      case PhysicalDeviceFeature::ShaderInt64:
        return "ShaderInt64";
      case PhysicalDeviceFeature::ShaderInt16:
        return "ShaderInt16";
      case PhysicalDeviceFeature::ShaderResourceResidency:
        return "ShaderResourceResidency";
      case PhysicalDeviceFeature::ShaderResourceMinLod:
        return "ShaderResourceMinLod";
      case PhysicalDeviceFeature::SparseBinding:
        return "SparseBinding";
      case PhysicalDeviceFeature::SparseResidencyBuffer:
        return "SparseResidencyBuffer";
      case PhysicalDeviceFeature::SparseResidencyImage2D:
        return "SparseResidencyImage2D";
      case PhysicalDeviceFeature::SparseResidencyImage3D:
        return "SparseResidencyImage3D";
      case PhysicalDeviceFeature::SparseResidency2Samples:
        return "SparseResidency2Samples";
      case PhysicalDeviceFeature::SparseResidency4Samples:
        return "SparseResidency4Samples";
      case PhysicalDeviceFeature::SparseResidency8Samples:
        return "SparseResidency8Samples";
      case PhysicalDeviceFeature::SparseResidency16Samples:
        return "SparseResidency16Samples";
      case PhysicalDeviceFeature::SparseResidencyAliased:
        return "SparseResidencyAliased";
      case PhysicalDeviceFeature::VariableMultisampleRate:
        return "VariableMultisampleRate";
      case PhysicalDeviceFeature::InheritedQueries:
        return "InheritedQueries";
      case PhysicalDeviceFeature::Invalid:
      default:
        return "Unknown";
      }
    }


    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::robustBufferAccess)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::fullDrawIndexUint32)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::imageCubeArray)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::independentBlend)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::geometryShader)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::tessellationShader)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::sampleRateShading)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::dualSrcBlend)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::logicOp)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::multiDrawIndirect)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::drawIndirectFirstInstance)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::depthBiasClamp)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::fillModeNonSolid)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::depthBounds)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::wideLines)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::largePoints)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::alphaToOne)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::multiViewport)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::samplerAnisotropy)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::textureCompressionETC2)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::textureCompressionASTC_LDR)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::textureCompressionBC)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::occlusionQueryPrecise)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::pipelineStatisticsQuery)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::vertexPipelineStoresAndAtomics)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::fragmentStoresAndAtomics)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::shaderTessellationAndGeometryPointSize)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::shaderImageGatherExtended)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::shaderStorageImageExtendedFormats)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::shaderStorageImageMultisample)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::shaderStorageImageReadWithoutFormat)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::shaderStorageImageWriteWithoutFormat)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::shaderUniformBufferArrayDynamicIndexing)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::shaderSampledImageArrayDynamicIndexing)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::shaderStorageBufferArrayDynamicIndexing)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::shaderStorageImageArrayDynamicIndexing)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::shaderClipDistance)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::shaderCullDistance)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::shaderFloat64)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::shaderInt64)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::shaderInt16)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::shaderResourceResidency)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::shaderResourceMinLod)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::sparseBinding)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::sparseResidencyBuffer)>::value, "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::sparseResidencyImage2D)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::sparseResidencyImage3D)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::sparseResidency2Samples)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::sparseResidency4Samples)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::sparseResidency8Samples)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::sparseResidency16Samples)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::sparseResidencyAliased)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::variableMultisampleRate)>::value,
                  "struct member not of the expected type");
    static_assert(std::is_same<VkBool32, decltype(VkPhysicalDeviceFeatures::inheritedQueries)>::value, "struct member not of the expected type");
  }
}
