#ifndef FSLGRAPHICSVULKAN1_0_EXTEND_PHYSICALDEVICEFEATURE_HPP
#define FSLGRAPHICSVULKAN1_0_EXTEND_PHYSICALDEVICEFEATURE_HPP
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
#include <FslGraphicsVulkan1_0/Common.hpp>
#include <vulkan/vulkan.h>

namespace Fsl
{
  namespace Vulkan
  {
    //! @brief Based on VkPhysicalDeviceFeatures
    enum class PhysicalDeviceFeature
    {
      //! This is not a valid feature
      Invalid = 0,
      RobustBufferAccess,
      FullDrawIndexUint32,
      ImageCubeArray,
      IndependentBlend,
      GeometryShader,
      TessellationShader,
      SampleRateShading,
      DualSrcBlend,
      LogicOp,
      MultiDrawIndirect,
      DrawIndirectFirstInstance,
      DepthClamp,
      DepthBiasClamp,
      FillModeNonSolid,
      DepthBounds,
      WideLines,
      LargePoints,
      AlphaToOne,
      MultiViewport,
      SamplerAnisotropy,
      TextureCompressionETC2,
      TextureCompressionASTC_LDR,
      TextureCompressionBC,
      OcclusionQueryPrecise,
      PipelineStatisticsQuery,
      VertexPipelineStoresAndAtomics,
      FragmentStoresAndAtomics,
      ShaderTessellationAndGeometryPointSize,
      ShaderImageGatherExtended,
      ShaderStorageImageExtendedFormats,
      ShaderStorageImageMultisample,
      ShaderStorageImageReadWithoutFormat,
      ShaderStorageImageWriteWithoutFormat,
      ShaderUniformBufferArrayDynamicIndexing,
      ShaderSampledImageArrayDynamicIndexing,
      ShaderStorageBufferArrayDynamicIndexing,
      ShaderStorageImageArrayDynamicIndexing,
      ShaderClipDistance,
      ShaderCullDistance,
      ShaderFloat64,
      ShaderInt64,
      ShaderInt16,
      ShaderResourceResidency,
      ShaderResourceMinLod,
      SparseBinding,
      SparseResidencyBuffer,
      SparseResidencyImage2D,
      SparseResidencyImage3D,
      SparseResidency2Samples,
      SparseResidency4Samples,
      SparseResidency8Samples,
      SparseResidency16Samples,
      SparseResidencyAliased,
      VariableMultisampleRate,
      InheritedQueries,
    };
  }
}

#endif
