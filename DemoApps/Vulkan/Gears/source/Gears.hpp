#ifndef VULKAN_GEARS_GEARS_HPP
#define VULKAN_GEARS_GEARS_HPP
/*
* Vulkan Example - Animated gears using multiple uniform buffers
*
* See readme.md for details
*
* Copyright (C) 2015 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

// Based on a example called 'Gears' by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)

#include <VulkanWillemsDemoAppExperimental/VulkanWillemsDemoApp.hpp>
#include <FslGraphicsVulkan1_0/DescriptorPool.hpp>
#include <FslGraphicsVulkan1_0/DescriptorSetLayout.hpp>
#include <FslGraphicsVulkan1_0/GraphicsPipeline.hpp>
#include <FslGraphicsVulkan1_0/PipelineLayout.hpp>
#include <memory>
#include "VulkanGear.hpp"

namespace Fsl
{
  class Gears : public Willems::VulkanWillemsDemoApp
  {
    struct Vertices
    {
      VkPipelineVertexInputStateCreateInfo InputState{};
      std::vector<VkVertexInputBindingDescription> BindingDescriptions;
      std::vector<VkVertexInputAttributeDescription> AttributeDescriptions;
    };

    struct Pipelines
    {
      Vulkan::GraphicsPipeline Solid;
    };

    std::vector<std::unique_ptr<VulkanGear> > m_gears;
    Vertices m_vertices;
    Vulkan::DescriptorSetLayout m_descriptorSetLayout;
    Vulkan::PipelineLayout m_pipelineLayout;
    Pipelines m_pipelines;
    Vulkan::DescriptorPool m_descriptorPool;

  public:
    Gears(const DemoAppConfig& config);
    ~Gears();
  protected:
    virtual void Prepare() override;
    virtual void BuildCommandBuffers() override;
    virtual void OnViewChanged() override;
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
  private:
    void PrepareVertices();
    void SetupDescriptorSetLayout();
    void PreparePipelines();
    void SetupDescriptorPool();
    void SetupDescriptorSets();
    void UpdateUniformBuffers();
  };
}

#endif
