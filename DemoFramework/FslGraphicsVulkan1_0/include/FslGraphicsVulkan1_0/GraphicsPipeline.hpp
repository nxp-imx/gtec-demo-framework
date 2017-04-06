#ifndef FSLGRAPHICSVULKAN1_0_GRAPHICSPIPELINE_HPP
#define FSLGRAPHICSVULKAN1_0_GRAPHICSPIPELINE_HPP
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

// Auto-generated Vulkan 1.0 C++11 RAII classes by RAIIGen (https://github.com/Unarmed1000)
// Auto generation template based on RapidVulkan https://github.com/Unarmed1000/RapidVulkan with permission.

#include <FslGraphicsVulkan1_0/ClaimMode.hpp>
#include <FslGraphicsVulkan1_0/Common.hpp>
#include <FslGraphicsVulkan1_0/Util.hpp>
#include <FslBase/Attributes.hpp>
#include <vulkan/vulkan.h>
#include <cassert>

namespace Fsl
{
  namespace Vulkan
  {
    //! This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
    class GraphicsPipeline
    {
      VkDevice m_device;
      VkPipeline m_pipelines;
    public:
      GraphicsPipeline(const GraphicsPipeline&) = delete;
      GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

      //! @brief Move assignment operator
      GraphicsPipeline& operator=(GraphicsPipeline&& other)
      {
        if (this != &other)
        {
          // Free existing resources then transfer the content of other to this one and fill other with default values
          if (IsValid())
            Reset();

          // Claim ownership here
          m_device = other.m_device;
          m_pipelines = other.m_pipelines;

          // Remove the data from other
          other.m_device = VK_NULL_HANDLE;
          other.m_pipelines = VK_NULL_HANDLE;
        }
        return *this;
      }

      //! @brief Move constructor
      //! Transfer ownership from other to this
      GraphicsPipeline(GraphicsPipeline&& other)
        : m_device(other.m_device)
        , m_pipelines(other.m_pipelines)
      {
        // Remove the data from other
        other.m_device = VK_NULL_HANDLE;
        other.m_pipelines = VK_NULL_HANDLE;
      }

      //! @brief Create a 'invalid' instance (use Reset to populate it)
      GraphicsPipeline()
        : m_device(VK_NULL_HANDLE)
        , m_pipelines(VK_NULL_HANDLE)
      {
      }

      //! @brief Assume control of the GraphicsPipeline (this object becomes responsible for releasing it)
      explicit GraphicsPipeline(const ClaimMode claimMode, const VkDevice device, const VkPipeline pipelines)
        : GraphicsPipeline()
      {
        Reset(claimMode, device, pipelines);
      }

      //! @brief Create the requested resource
      //! @note  Function: vkCreateGraphicsPipelines
      GraphicsPipeline(const VkDevice device, const VkPipelineCache pipelineCache, const VkGraphicsPipelineCreateInfo& createInfos)
        : GraphicsPipeline()
      {
        Reset(device, pipelineCache, createInfos);
      }

      //! @brief Create the requested resource
      //! @note  Function: vkCreateGraphicsPipelines
      GraphicsPipeline(const VkDevice device, const VkPipelineCache pipelineCache, const VkPipelineCreateFlags flags, const uint32_t stageCount, VkPipelineShaderStageCreateInfo*const pStages, VkPipelineVertexInputStateCreateInfo*const pVertexInputState, VkPipelineInputAssemblyStateCreateInfo*const pInputAssemblyState, VkPipelineTessellationStateCreateInfo*const pTessellationState, VkPipelineViewportStateCreateInfo*const pViewportState, VkPipelineRasterizationStateCreateInfo*const pRasterizationState, VkPipelineMultisampleStateCreateInfo*const pMultisampleState, VkPipelineDepthStencilStateCreateInfo*const pDepthStencilState, VkPipelineColorBlendStateCreateInfo*const pColorBlendState, VkPipelineDynamicStateCreateInfo*const pDynamicState, const VkPipelineLayout layout, const VkRenderPass renderPass, const uint32_t subpass, const VkPipeline basePipelineHandle, const int32_t basePipelineIndex)
        : GraphicsPipeline()
      {
        Reset(device, pipelineCache, flags, stageCount, pStages, pVertexInputState, pInputAssemblyState, pTessellationState, pViewportState, pRasterizationState, pMultisampleState, pDepthStencilState, pColorBlendState, pDynamicState, layout, renderPass, subpass, basePipelineHandle, basePipelineIndex);
      }

      ~GraphicsPipeline()
      {
        Reset();
      }

      //! @brief returns the managed handle and releases the ownership.
      VkPipeline Release() FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT
      {
        const auto resource = m_pipelines;
        m_device = VK_NULL_HANDLE;
        m_pipelines = VK_NULL_HANDLE;
        return resource;
      }

      //! @brief Destroys any owned resources and resets the object to its default state.
      void Reset()
      {
        if (! IsValid())
          return;

        assert(m_device != VK_NULL_HANDLE);
        assert(m_pipelines != VK_NULL_HANDLE);

        vkDestroyPipeline(m_device, m_pipelines, nullptr);
        m_device = VK_NULL_HANDLE;
        m_pipelines = VK_NULL_HANDLE;
      }

      //! @brief Destroys any owned resources and assume control of the GraphicsPipeline (this object becomes responsible for releasing it)
      void Reset(const ClaimMode claimMode, const VkDevice device, const VkPipeline pipelines)
      {
        if (IsValid())
          Reset();


        m_device = device;
        m_pipelines = pipelines;
      }

      //! @brief Destroys any owned resources and then creates the requested one
      //! @note  Function: vkCreateGraphicsPipelines
      void Reset(const VkDevice device, const VkPipelineCache pipelineCache, const VkGraphicsPipelineCreateInfo& createInfos)
      {
#ifndef FSLGRAPHICSVULKAN_DISABLE_PARAM_VALIDATION
        if (device == VK_NULL_HANDLE)
          throw std::invalid_argument("device can not be VK_NULL_HANDLE");
#else
        assert(device != VK_NULL_HANDLE);
        assert( == 1);
#endif

        // Free any currently allocated resource
        if (IsValid())
          Reset();

        // Since we want to ensure that the resource is left untouched on error we use a local variable as a intermediary
        VkPipeline pipelines;
        Util::Check(vkCreateGraphicsPipelines(device, pipelineCache, 1, &createInfos, nullptr, &pipelines), "vkCreateGraphicsPipelines", __FILE__, __LINE__);

        // Everything is ready, so assign the members
        m_device = device;
        m_pipelines = pipelines;
      }

      //! @brief Destroys any owned resources and then creates the requested one
      //! @note  Function: vkCreateGraphicsPipelines
      void Reset(const VkDevice device, const VkPipelineCache pipelineCache, const VkPipelineCreateFlags flags, const uint32_t stageCount, VkPipelineShaderStageCreateInfo*const pStages, VkPipelineVertexInputStateCreateInfo*const pVertexInputState, VkPipelineInputAssemblyStateCreateInfo*const pInputAssemblyState, VkPipelineTessellationStateCreateInfo*const pTessellationState, VkPipelineViewportStateCreateInfo*const pViewportState, VkPipelineRasterizationStateCreateInfo*const pRasterizationState, VkPipelineMultisampleStateCreateInfo*const pMultisampleState, VkPipelineDepthStencilStateCreateInfo*const pDepthStencilState, VkPipelineColorBlendStateCreateInfo*const pColorBlendState, VkPipelineDynamicStateCreateInfo*const pDynamicState, const VkPipelineLayout layout, const VkRenderPass renderPass, const uint32_t subpass, const VkPipeline basePipelineHandle, const int32_t basePipelineIndex)
      {
        VkGraphicsPipelineCreateInfo createInfos{};
        createInfos.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        createInfos.pNext = nullptr;
        createInfos.flags = flags;
        createInfos.stageCount = stageCount;
        createInfos.pStages = pStages;
        createInfos.pVertexInputState = pVertexInputState;
        createInfos.pInputAssemblyState = pInputAssemblyState;
        createInfos.pTessellationState = pTessellationState;
        createInfos.pViewportState = pViewportState;
        createInfos.pRasterizationState = pRasterizationState;
        createInfos.pMultisampleState = pMultisampleState;
        createInfos.pDepthStencilState = pDepthStencilState;
        createInfos.pColorBlendState = pColorBlendState;
        createInfos.pDynamicState = pDynamicState;
        createInfos.layout = layout;
        createInfos.renderPass = renderPass;
        createInfos.subpass = subpass;
        createInfos.basePipelineHandle = basePipelineHandle;
        createInfos.basePipelineIndex = basePipelineIndex;

        Reset(device, pipelineCache, createInfos);
      }

      //! @brief Get the associated 'Device'
      VkDevice GetDevice() const
      {
        return m_device;
      }

      //! @brief Get the associated resource handle
      VkPipeline Get() const
      {
        return m_pipelines;
      }

      //! @brief Get a pointer to the associated resource handle
      const VkPipeline* GetPointer() const
      {
        return &m_pipelines;
      }

      //! @brief Check if this object contains a valid resource
      inline bool IsValid() const
      {
        return m_pipelines != VK_NULL_HANDLE;
      }
    };

  }
}

#endif
