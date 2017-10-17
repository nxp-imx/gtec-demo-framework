#ifndef RAPIDVULKAN_GRAPHICSPIPELINES_HPP
#define RAPIDVULKAN_GRAPHICSPIPELINES_HPP
//***************************************************************************************************************************************************
//* BSD 3-Clause License
//*
//* Copyright (c) 2016, Rene Thrane
//* All rights reserved.
//*
//* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//*
//* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
//*    documentation and/or other materials provided with the distribution.
//* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this
//*    software without specific prior written permission.
//*
//* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//***************************************************************************************************************************************************

// Auto-generated Vulkan 1.0 C++11 RAII classes by RAIIGen (https://github.com/Unarmed1000/RAIIGen)

#include <RapidVulkan/ClaimMode.hpp>
#include <RapidVulkan/CheckError.hpp>
#include <RapidVulkan/System/Macro.hpp>
#include <vulkan/vulkan.h>
#include <cassert>
#include <utility>
#include <vector>

namespace RapidVulkan
{
  //! This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
  class GraphicsPipelines
  {
    VkDevice m_device;
    std::vector<VkPipeline> m_pipelines;
  public:
    GraphicsPipelines(const GraphicsPipelines&) = delete;
    GraphicsPipelines& operator=(const GraphicsPipelines&) = delete;

    //! @brief Move assignment operator
    GraphicsPipelines& operator=(GraphicsPipelines&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_device = other.m_device;
        m_pipelines = std::move(other.m_pipelines);

        // Remove the data from other
        other.m_device = VK_NULL_HANDLE;
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    GraphicsPipelines(GraphicsPipelines&& other)
      : m_device(other.m_device)
      , m_pipelines(std::move(other.m_pipelines))
    {
      // Remove the data from other
      other.m_device = VK_NULL_HANDLE;
    }

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    GraphicsPipelines()
      : m_device(VK_NULL_HANDLE)
      , m_pipelines()
    {
    }

    //! @brief Assume control of the GraphicsPipelines (this object becomes responsible for releasing it)
    //explicit GraphicsPipelines(const ClaimMode claimMode, const VkDevice device, const VkPipeline pipelines)
    //  : GraphicsPipelines()
    //{
    //  Reset(claimMode, device, pipelines);
    //}

    //! @brief Create the requested resource
    //! @note  Function: vkCreateGraphicsPipelines
    GraphicsPipelines(const VkDevice device, const VkPipelineCache pipelineCache, const uint32_t createInfoCount, const VkGraphicsPipelineCreateInfo& createInfos)
      : GraphicsPipelines()
    {
      Reset(device, pipelineCache, createInfoCount, createInfos);
    }

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Create the requested resource
    //! @note  Function: vkCreateGraphicsPipelines
    GraphicsPipelines(const VkDevice device, const VkPipelineCache pipelineCache, const uint32_t createInfoCount, const VkPipelineCreateFlags flags, const uint32_t stageCount, VkPipelineShaderStageCreateInfo*const pStages, VkPipelineVertexInputStateCreateInfo*const pVertexInputState, VkPipelineInputAssemblyStateCreateInfo*const pInputAssemblyState, VkPipelineTessellationStateCreateInfo*const pTessellationState, VkPipelineViewportStateCreateInfo*const pViewportState, VkPipelineRasterizationStateCreateInfo*const pRasterizationState, VkPipelineMultisampleStateCreateInfo*const pMultisampleState, VkPipelineDepthStencilStateCreateInfo*const pDepthStencilState, VkPipelineColorBlendStateCreateInfo*const pColorBlendState, VkPipelineDynamicStateCreateInfo*const pDynamicState, const VkPipelineLayout layout, const VkRenderPass renderPass, const uint32_t subpass, const VkPipeline basePipelineHandle, const int32_t basePipelineIndex)
      : GraphicsPipelines()
    {
      Reset(device, pipelineCache, createInfoCount, flags, stageCount, pStages, pVertexInputState, pInputAssemblyState, pTessellationState, pViewportState, pRasterizationState, pMultisampleState, pDepthStencilState, pColorBlendState, pDynamicState, layout, renderPass, subpass, basePipelineHandle, basePipelineIndex);
    }
#endif

    ~GraphicsPipelines()
    {
      Reset();
    }

    //! @brief returns the managed handle and releases the ownership.
    std::vector<VkPipeline> Release() RAPIDVULKAN_FUNC_POSTFIX_WARN_UNUSED_RESULT
    {
      auto resource = std::move(m_pipelines); 
      m_device = VK_NULL_HANDLE;
      m_pipelines.clear();
      return resource;
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset()
    {
      if (! IsValid())
        return;

      assert(m_device != VK_NULL_HANDLE);
      assert(m_pipelines.size() > 0);

      for(std::size_t i=0; i<m_pipelines.size(); ++i)
      {
        vkDestroyPipeline(m_device, m_pipelines[i], nullptr);
      }
      m_device = VK_NULL_HANDLE;
      m_pipelines.clear();
    }

/*    
    //! @brief Destroys any owned resources and assume control of the GraphicsPipelines (this object becomes responsible for releasing it)
    void Reset(const ClaimMode claimMode, const VkDevice device, const VkPipeline pipelines)
    {
      if (IsValid())
        Reset();


      m_device = device;
      m_pipelines = std::move(pipelines);
    }
*/
    
    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateGraphicsPipelines
    void Reset(const VkDevice device, const VkPipelineCache pipelineCache, const uint32_t createInfoCount, const VkGraphicsPipelineCreateInfo& createInfos)
    {
#ifndef RAPIDVULKAN_DISABLE_PARAM_VALIDATION
      if (device == VK_NULL_HANDLE)
        throw std::invalid_argument("device can not be VK_NULL_HANDLE");
#else
      assert(device != VK_NULL_HANDLE);
#endif

      // Free any currently allocated resource
      if (IsValid())
        Reset();

      // Since we want to ensure that the resource is left untouched on error we use a local variable as a intermediary
      std::vector<VkPipeline> pipelines();
      CheckError(vkCreateGraphicsPipelines(device, pipelineCache, createInfoCount, &createInfos, nullptr, pipelines.data()), "vkCreateGraphicsPipelines", __FILE__, __LINE__);

      // Everything is ready, so assign the members
      m_device = device;
      m_pipelines = std::move(pipelines);
    }

#ifndef RAPIDVULKAN_DISABLE_UNROLLED_STRUCT_METHODS
    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vkCreateGraphicsPipelines
    void Reset(const VkDevice device, const VkPipelineCache pipelineCache, const uint32_t createInfoCount, const VkPipelineCreateFlags flags, const uint32_t stageCount, VkPipelineShaderStageCreateInfo*const pStages, VkPipelineVertexInputStateCreateInfo*const pVertexInputState, VkPipelineInputAssemblyStateCreateInfo*const pInputAssemblyState, VkPipelineTessellationStateCreateInfo*const pTessellationState, VkPipelineViewportStateCreateInfo*const pViewportState, VkPipelineRasterizationStateCreateInfo*const pRasterizationState, VkPipelineMultisampleStateCreateInfo*const pMultisampleState, VkPipelineDepthStencilStateCreateInfo*const pDepthStencilState, VkPipelineColorBlendStateCreateInfo*const pColorBlendState, VkPipelineDynamicStateCreateInfo*const pDynamicState, const VkPipelineLayout layout, const VkRenderPass renderPass, const uint32_t subpass, const VkPipeline basePipelineHandle, const int32_t basePipelineIndex)
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

      Reset(device, pipelineCache, createInfoCount, createInfos);
    }
#endif

    //! @brief Get the associated 'Device'
    VkDevice GetDevice() const
    {
      return m_device;
    }

    //! @brief Get size of the vector
    std::size_t Size() const
    {
      return m_pipelines.size();
    }


    //! @brief Get direct access to the vector content
    const VkPipeline* Data() const
    {
      return m_pipelines.data();
    }


    //! @brief Get the associated resource handles
    const std::vector<VkPipeline>& Get() const
    {
      return m_pipelines;
    }


    VkPipeline Get(const std::size_t arrayIndex) const
    {
      assert(arrayIndex < m_pipelines.size());
      return m_pipelines[arrayIndex];
    }


    //! @brief Access the resource at a given index
    VkPipeline operator[] (const std::size_t arrayIndex) const
    {
      assert(arrayIndex < m_pipelines.size());
      return m_pipelines[arrayIndex];
    }

    //! @brief get a pointer to the resource at the given index
    const VkPipeline* GetPointer(const std::size_t arrayIndex) const
    {
      assert(arrayIndex < m_pipelines.size());
      return &m_pipelines[arrayIndex];
    }

    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_pipelines.size() > 0;
    }
  };
}

#endif
