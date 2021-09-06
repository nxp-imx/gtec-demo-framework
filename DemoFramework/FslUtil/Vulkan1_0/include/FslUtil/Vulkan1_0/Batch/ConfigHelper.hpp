#ifndef FSLUTIL_VULKAN1_0_BATCH_CONFIGHELPER_HPP
#define FSLUTIL_VULKAN1_0_BATCH_CONFIGHELPER_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Span/Span.hpp>
#include <FslGraphics/Render/BlendState.hpp>
#include <FslGraphics/Render/Basic/BasicCullMode.hpp>
#include <FslGraphics/Render/Basic/BasicFrontFace.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>
#include <RapidVulkan/GraphicsPipeline.hpp>
#include <RapidVulkan/PipelineLayout.hpp>
#include <vulkan/vulkan.h>

namespace Fsl
{
  namespace Vulkan
  {
    namespace ConfigHelper
    {
      void ExtractVertexAttributes(Span<VkVertexInputAttributeDescription> dstAttributes, const VertexDeclarationSpan& vertexDecl);

      RapidVulkan::PipelineLayout CreatePipelineLayout(const VkDevice device, const VkDescriptorSetLayout& descriptorSetLayoutUniform,
                                                       const VkDescriptorSetLayout& descriptorSetLayoutTexture);

      RapidVulkan::PipelineLayout CreatePipelineLayoutWithPushConstant(const VkDevice device, const VkDescriptorSetLayout& descriptorSetLayoutUniform,
                                                                       const VkDescriptorSetLayout& descriptorSetLayoutTexture,
                                                                       const uint32_t cbPushConstants);

      VkPipelineColorBlendAttachmentState CreatePipelineColorBlendAttachmentState(const BlendState blendState);


      RapidVulkan::GraphicsPipeline CreateGraphicsPipeline(const VkDevice device, const VkShaderModule vertexShader,
                                                           const VkShaderModule fragmentShader, const VkPipelineLayout pipelineLayout,
                                                           const VkPipelineCache pipelineCache, const VkRenderPass renderPass, const uint32_t subpass,
                                                           const PxExtent2D& screenExtentPx, const BlendState blendState,
                                                           const BasicCullMode cullMode, const BasicFrontFace frontFace,
                                                           const VertexDeclarationSpan& vertexDeclaration);
    }
  }
}

#endif
