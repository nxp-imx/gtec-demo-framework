/*
 * Vulkan Example - Basic indexed triangle rendering
 *
 * Note:
 * This is a "pedal to the metal" example to show off how to get Vulkan up an displaying something
 * Contrary to the other examples, this one won't make use of helper functions or initializers
 * Except in a few cases (swap chain setup e.g.)
 *
 * Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

// Based on a example called 'Triangle' by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)

#include "Triangle.hpp"
#include <FslBase/Log/Log.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/ConvertUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/MemoryTypeUtil.hpp>
#include <RapidVulkan/Check.hpp>
#include <array>
#include <cstring>

using namespace RapidVulkan;

namespace Fsl
{
  using namespace Vulkan;
  using namespace Vulkan::ConvertUtil;

  namespace
  {
    // Set to "true" to use staging buffers for uploading vertex and index data to device local memory
    // See "prepareVertices" for details on what's staging and on why to use it
    const bool g_configUseStaging = true;

    // Default fence timeout in nanoseconds
    const auto DEFAULT_FENCE_TIMEOUT = 100000000000;

    const auto VERTEX_BUFFER_BIND_ID = 0;


    // Get a new command buffer from the command pool
    // If begin is true, the command buffer is also started so we can start adding commands
    CommandBuffer DoGetCommandBuffer(const VkDevice device, const CommandPool& cmdPool, const bool begin)
    {
      VkCommandBufferAllocateInfo cmdBufAllocateInfo{};
      cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      cmdBufAllocateInfo.commandPool = cmdPool.Get();
      cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      cmdBufAllocateInfo.commandBufferCount = 1;

      CommandBuffer cmdBuffer(device, cmdBufAllocateInfo);

      // If requested, also start the new command buffer
      if (begin)
      {
        VkCommandBufferBeginInfo cmdBufInfo{};
        cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBufInfo.pNext = nullptr;

        cmdBuffer.Begin(cmdBufInfo);
      }
      return cmdBuffer;
    }


    // End the command buffer and submit it to the queue
    // Uses a fence to ensure command buffer has finished executing before deleting it
    void DoFlushCommandBuffer(CommandBuffer& rCommandBuffer, VUDeviceQueueRecord& rQueue)
    {
      assert(rCommandBuffer.IsValid());

      rCommandBuffer.End();

      // Create fence to ensure that the command buffer has finished executing
      VkFenceCreateInfo fenceCreateInfo{};
      fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
      fenceCreateInfo.flags = 0;
      Fence fence(rCommandBuffer.GetDevice(), fenceCreateInfo);

      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = rCommandBuffer.GetPointer();

      // Submit to the queue
      rQueue.Submit(1, &submitInfo, fence.Get());

      // Wait for the fence to signal that command buffer has finished executing
      fence.WaitForFence(DEFAULT_FENCE_TIMEOUT);

      rCommandBuffer.Reset();
    }
  }


  Triangle::Triangle(const DemoAppConfig& config)
    : VulkanWillemsDemoApp(config)
    , m_descriptorSet(VK_NULL_HANDLE)
  {
    m_zoom = -2.5f;
  }


  Triangle::~Triangle()
  {
    // Wait for everything to be idle before we try to free it
    SafeWaitForDeviceIdle();
  }


  void Triangle::Prepare()
  {
    VulkanWillemsDemoApp::Prepare();

    PrepareSynchronizationPrimitives();
    PrepareVertices(g_configUseStaging);
    PrepareUniformBuffers();
    SetupDescriptorSetLayout();
    PreparePipelines();
    SetupDescriptorPool();
    SetupDescriptorSet();
    BuildCommandBuffers();
  }


  // Create the depth (and stencil) buffer attachments used by our framebuffers
  // Note: Override of virtual function in the base class and called from within VulkanExampleBase::Prepare
  void Triangle::SetupDepthStencil(const VkFormat depthFormat)
  {
    using namespace MemoryTypeUtil;

    // NOTE: this code is almost identical to the method we are overriding.

    const auto screenExtent = GetScreenExtent();

    // Create an optimal image used as the depth stencil attachment
    VkImageCreateInfo image{};
    image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image.imageType = VK_IMAGE_TYPE_2D;
    image.format = depthFormat;
    // Use example's height and width
    image.extent = {screenExtent.Width, screenExtent.Height, 1};
    image.mipLevels = 1;
    image.arrayLayers = 1;
    image.samples = VK_SAMPLE_COUNT_1_BIT;
    image.tiling = VK_IMAGE_TILING_OPTIMAL;
    image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    image.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    m_depthStencil.Image.Reset(m_device.Get(), image);

    const VkMemoryRequirements memReqs = m_depthStencil.Image.GetImageMemoryRequirements();
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = m_physicalDevice.MemoryProperties;
    const auto memoryTypeIndex = GetMemoryTypeIndex(physicalDeviceMemoryProperties, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // Allocate memory for the image (device local) and bind it to our image
    VkMemoryAllocateInfo memAlloc{};
    memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = memoryTypeIndex;

    m_depthStencil.Mem.Reset(m_device.Get(), memAlloc);

    RAPIDVULKAN_CHECK(vkBindImageMemory(m_device.Get(), m_depthStencil.Image.Get(), m_depthStencil.Mem.Get(), 0));

    // Create a view for the depth stencil image
    // Images aren't directly accessed in Vulkan, but rather through views described by a subresource range
    // This allows for multiple views of one image with differing ranges (e.g. for different layers)
    VkImageViewCreateInfo depthStencilView{};
    depthStencilView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
    depthStencilView.format = depthFormat;
    depthStencilView.subresourceRange = {};
    depthStencilView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    depthStencilView.subresourceRange.baseMipLevel = 0;
    depthStencilView.subresourceRange.levelCount = 1;
    depthStencilView.subresourceRange.baseArrayLayer = 0;
    depthStencilView.subresourceRange.layerCount = 1;
    depthStencilView.image = m_depthStencil.Image.Get();

    m_depthStencil.View.Reset(m_device.Get(), depthStencilView);
  }


  // Render pass setup
  // Render passes are a new concept in Vulkan. They describe the attachments used during rendering and may contain multiple subpasses with attachment
  // dependencies This allows the driver to know up-front what the rendering will look like and is a good opportunity to optimize especially on
  // tile-based renderers (with multiple subpasses) Using sub pass dependencies also adds implicit layout transitions for the attachment used, so we
  // don't need to add explicit image memory barriers to transform them Note: Override of virtual function in the base class and called from within
  // VulkanExampleBase::prepare
  void Triangle::SetupRenderPass(const VkFormat colorFormat, const VkFormat depthFormat)
  {
    // This example will use a single render pass with one subpass

    // Descriptors for the attachments used by this renderPass
    std::array<VkAttachmentDescription, 2> attachments{};

    // Color attachment
    attachments[0].format = colorFormat;
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;                      // We don't use multi sampling in this example
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;                 // Clear this attachment at the start of the render pass
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;               // Keep it's contents after the render pass is finished (for displaying it)
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;      // We don't use stencil, so don't care for load
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;    // Same for store
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;            // Layout at render pass start. Initial doesn't matter, so we use undefined
    attachments[0].finalLayout =
      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;    // Layout to which the attachment is transitioned when the render pass is finished
    // As we want to present the color buffer to the swapchain, we transition to PRESENT_KHR
    // Depth attachment
    attachments[1].format = depthFormat;
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;    // Clear depth at start of first subpass
    attachments[1].storeOp =
      VK_ATTACHMENT_STORE_OP_DONT_CARE;    // We don't need depth after render pass has finished (DONT_CARE may result in better performance)
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;      // No stencil
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;    // No Stencil
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;            // Layout at render pass start. Initial doesn't matter, so we use undefined
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;    // Transition to depth/stencil attachment

    // Setup attachment references
    VkAttachmentReference colorReference{};
    colorReference.attachment = 0;                                       // Attachment 0 is color
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;    // Attachment layout used as color during the subpass

    VkAttachmentReference depthReference{};
    depthReference.attachment = 1;                                               // Attachment 1 is color
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;    // Attachment used as depth/stemcil used during the subpass

    // Setup a single subpass reference
    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;                     // Subpass uses one color attachment
    subpassDescription.pColorAttachments = &colorReference;          // Reference to the color attachment in slot 0
    subpassDescription.pDepthStencilAttachment = &depthReference;    // Reference to the depth attachment in slot 1
    subpassDescription.inputAttachmentCount = 0;                     // Input attachments can be used to sample from contents of a previous subpass
    subpassDescription.pInputAttachments = nullptr;                  // (Input attachments not used by this example)
    subpassDescription.preserveAttachmentCount = 0;       // Preserved attachments can be used to loop (and preserve) attachments through subpasses
    subpassDescription.pPreserveAttachments = nullptr;    // (Preserve attachments not used by this example)
    subpassDescription.pResolveAttachments =
      nullptr;    // Resolve attachments are resolved at the end of a sub pass and can be used for e.g. multi sampling

    // Setup subpass dependencies
    // These will add the implicit attachment layout transitions specified by the attachment descriptions
    // The actual usage layout is preserved through the layout specified in the attachment reference
    // Each subpass dependency will introduce a memory and execution dependency between the source and dest subpass described by
    // srcStageMask, dstStageMask, srcAccessMask, dstAccessMask (and dependencyFlags is set)
    // Note: VK_SUBPASS_EXTERNAL is a special constant that refers to all commands executed outside of the actual renderPass)
    std::array<VkSubpassDependency, 2> dependencies{};

    // First dependency at the start of the renderPass
    // Does the transition from final to initial layout
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;    // Producer of the dependency
    dependencies[0].dstSubpass = 0;                      // Consumer is our single subpass that will wait for the execution depdendency
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    // Second dependency at the end the renderPass
    // Does the transition from the initial to the final layout
    dependencies[1].srcSubpass = 0;                      // Producer of the dependency is our single subpass
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;    // Consumer are all commands outside of the renderPass
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    // Create the actual renderPass
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());     // Number of attachments used by this render pass
    renderPassInfo.pAttachments = attachments.data();                               // Descriptions of the attachments used by the render pass
    renderPassInfo.subpassCount = 1;                                                // We only use one subpass in this example
    renderPassInfo.pSubpasses = &subpassDescription;                                // Description of that subpass
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());    // Number of subpass dependencies
    renderPassInfo.pDependencies = dependencies.data();                             // Subpass dependencies used by the render pass

    m_renderPass.Reset(m_device.Get(), renderPassInfo);
  }

  // Create a frame buffer for each swap chain image
  // Note: Override of virtual function in the base class and called from within VulkanExampleBase::prepare
  void Triangle::SetupFrameBuffer()
  {
    const auto screenExtent = GetScreenExtent();

    // Create a frame buffer for every image in the swapchain
    m_frameBuffers.clear();
    m_frameBuffers.resize(m_swapchain.GetImageCount());
    for (std::size_t i = 0; i < m_frameBuffers.size(); ++i)
    {
      std::array<VkImageView, 2> attachments{};
      attachments[0] = m_swapchain[i].View.Get();    // Color attachment is the view of the swapchain image
      attachments[1] = m_depthStencil.View.Get();    // Depth/Stencil attachment is the same for all frame buffers

      VkFramebufferCreateInfo frameBufferCreateInfo{};
      frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      // All frame buffers use the same renderPass setup
      frameBufferCreateInfo.renderPass = m_renderPass.Get();
      frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
      frameBufferCreateInfo.pAttachments = attachments.data();
      frameBufferCreateInfo.width = screenExtent.Width;
      frameBufferCreateInfo.height = screenExtent.Height;
      frameBufferCreateInfo.layers = 1;
      // Create the framebuffer
      m_frameBuffers[i].Reset(m_device.Get(), frameBufferCreateInfo);
    }
  }

  void Triangle::Update(const DemoTime& demoTime)
  {
  }


  void Triangle::Draw(const DemoTime& demoTime)
  {
    // Get next image in the swap chain (back/front buffer)
    VkResult result = m_swapchain.TryAcquireNextImage(m_presentCompleteSemaphore.Get(), m_currentBufferIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
      // TODO: support 'soft restart'
      FSLLOG("Restarting app due to VK_ERROR_OUT_OF_DATE_KHR");
      GetDemoAppControl()->RequestAppRestart();
      return;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
      throw std::runtime_error("Could not acquire next image.");
    }

    // Use a fence to wait until the command buffer has finished execution before using it again
    m_waitFences[m_currentBufferIndex].WaitForFence(UINT64_MAX);
    m_waitFences[m_currentBufferIndex].ResetFence();

    // Pipeline stage at which the queue submission will wait (via pWaitSemaphores)
    VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    // The submit info structure specifies a command buffer queue submission batch
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pWaitDstStageMask = &waitStageMask;    // Pointer to the list of pipeline stages that the semaphore waits will occur at
    submitInfo.pWaitSemaphores =
      m_presentCompleteSemaphore.GetPointer();    // Semaphore(s) to wait upon before the submitted command buffer starts executing
    submitInfo.waitSemaphoreCount = 1;            // One wait semaphore
    submitInfo.pSignalSemaphores = m_renderCompleteSemaphore.GetPointer();    // Semaphore(s) to be signaled when command buffers have completed
    submitInfo.signalSemaphoreCount = 1;                                      // One signal semaphore
    submitInfo.pCommandBuffers = m_drawCmdBuffers.GetPointer(m_currentBufferIndex);    // Command buffers(s) to execute in this batch (submission)
    submitInfo.commandBufferCount = 1;                                                 // One command buffer

    // Submit to the graphics queue passing a wait fence
    m_deviceQueue.Submit(1, &submitInfo, m_waitFences[m_currentBufferIndex].Get());

    // Present the current buffer to the swap chain
    // Pass the semaphore signaled by the command buffer submission from the submit info as the wait semaphore for swap chain presentation
    // This ensures that the image is not presented to the windowing system until all commands have been submitted
    result = m_swapchain.TryQueuePresent(m_deviceQueue.Queue, m_currentBufferIndex, m_renderCompleteSemaphore.Get());
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
      // TODO: support 'soft restart'
      FSLLOG("Restarting app due to VK_ERROR_OUT_OF_DATE_KHR");
      GetDemoAppControl()->RequestAppRestart();
      return;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
      throw std::runtime_error("Could not present queue");
    }
  }


  // Create the Vulkan synchronization primitives used in this example
  void Triangle::PrepareSynchronizationPrimitives()
  {
    // Semaphores (Used for correct command ordering)
    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = nullptr;

    // Semaphore used to ensures that image presentation is complete before starting to submit again
    m_presentCompleteSemaphore.Reset(m_device.Get(), semaphoreCreateInfo);

    // Semaphore used to ensures that all commands submitted have been finished before submitting the image to the queue
    m_renderCompleteSemaphore.Reset(m_device.Get(), semaphoreCreateInfo);

    // Fences (Used to check draw command buffer completion)
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    // Create in signaled state so we don't wait on first render of each command buffer
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    m_waitFences.resize(m_drawCmdBuffers.Size());
    for (auto& rFence : m_waitFences)
    {
      rFence.Reset(m_device.Get(), fenceCreateInfo);
    }
  }


  // Prepare vertex and index buffers for an indexed triangle
  // Also uploads them to device local memory using staging and initializes vertex input and attribute binding to match the vertex shader
  void Triangle::PrepareVertices(const bool useStagingBuffers)
  {
    using namespace MemoryTypeUtil;
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = m_physicalDevice.MemoryProperties;

    // A note on memory management in Vulkan in general:
    //  This is a very complex topic and while it's fine for an example application to do small individual memory allocations that is not
    //  what should be done a real-world application, where you should allocate large chunks of memory at once instead.

    struct Vertex
    {
      float position[3];
      float color[3];
    };

    // Setup vertices
    std::vector<Vertex> vertexBuffer = {
      {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}}, {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, {{0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}};
    uint32_t vertexBufferSize = static_cast<uint32_t>(vertexBuffer.size()) * sizeof(Vertex);

    // Setup indices
    std::vector<uint32_t> indexBuffer = {0, 1, 2};
    m_indices.Count = static_cast<uint32_t>(indexBuffer.size());
    uint32_t indexBufferSize = m_indices.Count * sizeof(uint32_t);

    void* data;
    if (useStagingBuffers)
    {
      // Static data like vertex and index buffer should be stored on the device memory
      // for optimal (and fastest) access by the GPU
      //
      // To achieve this we use so-called "staging buffers" :
      // - Create a buffer that's visible to the host (and can be mapped)
      // - Copy the data to this buffer
      // - Create another buffer that's local on the device (VRAM) with the same size
      // - Copy the data from the host to the device using a command buffer
      // - Delete the host visible (staging) buffer
      // - Use the device local buffers for rendering

      struct StagingBuffer
      {
        RapidVulkan::Memory Memory;
        RapidVulkan::Buffer Buffer;
      };

      struct StagingBuffers
      {
        StagingBuffer Vertices;
        StagingBuffer Indices;
      };
      StagingBuffers stagingBuffers;

      // Vertex buffer
      VkBufferCreateInfo vertexBufferInfo{};
      vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      vertexBufferInfo.size = vertexBufferSize;
      // Buffer is used as the copy source
      vertexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
      // Create a host-visible buffer to copy the vertex data to (staging buffer)

      stagingBuffers.Vertices.Buffer.Reset(m_device.Get(), vertexBufferInfo);

      VkMemoryRequirements memReqs = stagingBuffers.Vertices.Buffer.GetBufferMemoryRequirements();

      VkMemoryAllocateInfo memAlloc{};
      memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      memAlloc.allocationSize = memReqs.size;
      // Request a host visible memory type that can be used to copy our data do
      // Also request it to be coherent, so that writes are visible to the GPU right after unmapping the buffer
      memAlloc.memoryTypeIndex = GetMemoryTypeIndex(physicalDeviceMemoryProperties, memReqs.memoryTypeBits,
                                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

      stagingBuffers.Vertices.Memory.Reset(m_device.Get(), memAlloc);

      // Map and copy
      RAPIDVULKAN_CHECK(vkMapMemory(m_device.Get(), stagingBuffers.Vertices.Memory.Get(), 0, memAlloc.allocationSize, 0, &data));
      {
        std::memcpy(data, vertexBuffer.data(), vertexBufferSize);
      }
      vkUnmapMemory(m_device.Get(), stagingBuffers.Vertices.Memory.Get());
      RAPIDVULKAN_CHECK(vkBindBufferMemory(m_device.Get(), stagingBuffers.Vertices.Buffer.Get(), stagingBuffers.Vertices.Memory.Get(), 0));

      // Create a device local buffer to which the (host local) vertex data will be copied and which will be used for rendering
      vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      m_vertices.Buffer.Reset(m_device.Get(), vertexBufferInfo);
      vkGetBufferMemoryRequirements(m_device.Get(), m_vertices.Buffer.Get(), &memReqs);
      memAlloc.allocationSize = memReqs.size;
      memAlloc.memoryTypeIndex = GetMemoryTypeIndex(physicalDeviceMemoryProperties, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

      m_vertices.Memory.Reset(m_device.Get(), memAlloc);
      RAPIDVULKAN_CHECK(vkBindBufferMemory(m_device.Get(), m_vertices.Buffer.Get(), m_vertices.Memory.Get(), 0));

      // Index buffer
      VkBufferCreateInfo indexbufferInfo{};
      indexbufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      indexbufferInfo.size = indexBufferSize;
      indexbufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
      // Copy index data to a buffer visible to the host (staging buffer)
      stagingBuffers.Indices.Buffer.Reset(m_device.Get(), indexbufferInfo);
      memReqs = stagingBuffers.Indices.Buffer.GetBufferMemoryRequirements();
      memAlloc.allocationSize = memReqs.size;
      memAlloc.memoryTypeIndex = GetMemoryTypeIndex(physicalDeviceMemoryProperties, memReqs.memoryTypeBits,
                                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

      stagingBuffers.Indices.Memory.Reset(m_device.Get(), memAlloc);

      RAPIDVULKAN_CHECK(vkMapMemory(m_device.Get(), stagingBuffers.Indices.Memory.Get(), 0, indexBufferSize, 0, &data));
      {
        std::memcpy(data, indexBuffer.data(), indexBufferSize);
      }
      vkUnmapMemory(m_device.Get(), stagingBuffers.Indices.Memory.Get());
      RAPIDVULKAN_CHECK(vkBindBufferMemory(m_device.Get(), stagingBuffers.Indices.Buffer.Get(), stagingBuffers.Indices.Memory.Get(), 0));

      // Create destination buffer with device only visibility
      indexbufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      m_indices.Buffer.Reset(m_device.Get(), indexbufferInfo);
      memReqs = m_indices.Buffer.GetBufferMemoryRequirements();
      memAlloc.allocationSize = memReqs.size;
      memAlloc.memoryTypeIndex = GetMemoryTypeIndex(physicalDeviceMemoryProperties, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
      m_indices.Memory.Reset(m_device.Get(), memAlloc);
      RAPIDVULKAN_CHECK(vkBindBufferMemory(m_device.Get(), m_indices.Buffer.Get(), m_indices.Memory.Get(), 0));

      // VkCommandBufferBeginInfo cmdBufferBeginInfo{};
      // cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      // cmdBufferBeginInfo.pNext = nullptr;

      // Buffer copies have to be submitted to a queue, so we need a command buffer for them
      // Note: Some devices offer a dedicated transfer queue (with only the transfer bit set) that may be faster when doing lots of copies
      CommandBuffer copyCmd = DoGetCommandBuffer(m_device.Get(), m_commandPool, true);

      // Put buffer region copies into command buffer
      VkBufferCopy copyRegion{};

      // Vertex buffer
      copyRegion.size = vertexBufferSize;
      vkCmdCopyBuffer(copyCmd.Get(), stagingBuffers.Vertices.Buffer.Get(), m_vertices.Buffer.Get(), 1, &copyRegion);
      // Index buffer
      copyRegion.size = indexBufferSize;
      vkCmdCopyBuffer(copyCmd.Get(), stagingBuffers.Indices.Buffer.Get(), m_indices.Buffer.Get(), 1, &copyRegion);

      // Flushing the command buffer will also submit it to the queue and uses a fence to ensure that all commands have been executed before returning
      DoFlushCommandBuffer(copyCmd, m_deviceQueue);

      // Destroy staging buffers (by having the go out of scope)
      // Note: Staging buffer must not be deleted before the copies have been submitted and executed
    }
    else
    {
      throw NotImplementedException("PrepareVertices");
      // Don't use staging
      // Create host-visible buffers only and use these for rendering. This is not advised and will usually result in lower rendering performance

      //  // Vertex buffer
      //  VkBufferCreateInfo vertexBufferInfo = {};
      //  vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      //  vertexBufferInfo.size = vertexBufferSize;
      //  vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

      //  // Copy vertex data to a buffer visible to the host
      //  RAPIDVULKAN_CHECK(vkCreateBuffer(device, &vertexBufferInfo, nullptr, &vertices.buffer));
      //  vkGetBufferMemoryRequirements(device, vertices.buffer, &memReqs);
      //  memAlloc.allocationSize = memReqs.size;
      //  memAlloc.memoryTypeIndex = getMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
      //  RAPIDVULKAN_CHECK(vkAllocateMemory(device, &memAlloc, nullptr, &vertices.memory));
      //  RAPIDVULKAN_CHECK(vkMapMemory(device, vertices.memory, 0, memAlloc.allocationSize, 0, &data));
      //  memcpy(data, vertexBuffer.data(), vertexBufferSize);
      //  vkUnmapMemory(device, vertices.memory);
      //  RAPIDVULKAN_CHECK(vkBindBufferMemory(device, vertices.buffer, vertices.memory, 0));

      //  // Index buffer
      //  VkBufferCreateInfo indexbufferInfo = {};
      //  indexbufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      //  indexbufferInfo.size = indexBufferSize;
      //  indexbufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

      //  // Copy index data to a buffer visible to the host
      //  RAPIDVULKAN_CHECK(vkCreateBuffer(device, &indexbufferInfo, nullptr, &indices.buffer));
      //  vkGetBufferMemoryRequirements(device, indices.buffer, &memReqs);
      //  memAlloc.allocationSize = memReqs.size;
      //  memAlloc.memoryTypeIndex = getMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
      //  RAPIDVULKAN_CHECK(vkAllocateMemory(device, &memAlloc, nullptr, &indices.memory));
      //  RAPIDVULKAN_CHECK(vkMapMemory(device, indices.memory, 0, indexBufferSize, 0, &data));
      //  memcpy(data, indexBuffer.data(), indexBufferSize);
      //  vkUnmapMemory(device, indices.memory);
      //  RAPIDVULKAN_CHECK(vkBindBufferMemory(device, indices.buffer, indices.memory, 0));
    }

    // Vertex input binding
    m_vertices.InputBinding.binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.InputBinding.stride = sizeof(Vertex);
    m_vertices.InputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // Input attribute binding describe shader attribute locations and memory layouts
    // These match the following shader layout (see triangle.vert):
    //  layout (location = 0) in vec3 inPos;
    //  layout (location = 1) in vec3 inColor;
    m_vertices.InputAttributes.resize(2);
    // Attribute location 0: Position
    m_vertices.InputAttributes[0].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.InputAttributes[0].location = 0;
    m_vertices.InputAttributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    m_vertices.InputAttributes[0].offset = offsetof(Vertex, position);
    // Attribute location 1: Color
    m_vertices.InputAttributes[1].binding = VERTEX_BUFFER_BIND_ID;
    m_vertices.InputAttributes[1].location = 1;
    m_vertices.InputAttributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    m_vertices.InputAttributes[1].offset = offsetof(Vertex, color);

    // Assign to the vertex input state used for pipeline creation
    m_vertices.InputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    m_vertices.InputState.pNext = nullptr;
    m_vertices.InputState.flags = 0;
    m_vertices.InputState.vertexBindingDescriptionCount = 1;
    m_vertices.InputState.pVertexBindingDescriptions = &m_vertices.InputBinding;
    m_vertices.InputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_vertices.InputAttributes.size());
    m_vertices.InputState.pVertexAttributeDescriptions = m_vertices.InputAttributes.data();
  }


  void Triangle::PrepareUniformBuffers()
  {
    using namespace MemoryTypeUtil;
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = m_physicalDevice.MemoryProperties;

    // Prepare and initialize a uniform buffer block containing shader uniforms
    // Single uniforms like in OpenGL are no longer present in Vulkan. All Shader uniforms are passed via uniform buffer blocks
    VkMemoryRequirements memReqs;

    // Vertex shader uniform buffer block
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.allocationSize = 0;
    allocInfo.memoryTypeIndex = 0;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(m_uboVS);
    // This buffer will be used as a uniform buffer
    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    // Create a new buffer
    m_uniformDataVS.Buffer.Reset(m_device.Get(), bufferInfo);
    // Get memory requirements including size, alignment and memory type
    memReqs = m_uniformDataVS.Buffer.GetBufferMemoryRequirements();
    allocInfo.allocationSize = memReqs.size;
    // Get the memory type index that supports host visible memory access
    // Most implementations offer multiple memory types and selecting the correct one to allocate memory from is crucial
    // We also want the buffer to be host coherent so we don't have to flush (or sync after every update.
    // Note: This may affect performance so you might not want to do this in a real world application that updates buffers on a regular base
    allocInfo.memoryTypeIndex = GetMemoryTypeIndex(physicalDeviceMemoryProperties, memReqs.memoryTypeBits,
                                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    // Allocate memory for the uniform buffer
    m_uniformDataVS.Memory.Reset(m_device.Get(), allocInfo);
    // Bind memory to buffer
    RAPIDVULKAN_CHECK(vkBindBufferMemory(m_device.Get(), m_uniformDataVS.Buffer.Get(), m_uniformDataVS.Memory.Get(), 0));

    // Store information in the uniform's descriptor that is used by the descriptor set
    m_uniformDataVS.Descriptor.buffer = m_uniformDataVS.Buffer.Get();
    m_uniformDataVS.Descriptor.offset = 0;
    m_uniformDataVS.Descriptor.range = sizeof(m_uboVS);

    UpdateUniformBuffers();
  }


  void Triangle::UpdateUniformBuffers()
  {
    auto screenRes = GetScreenResolution();

    // Update matrices
    m_uboVS.ProjectionMatrix = glm::perspective(glm::radians(60.0f), static_cast<float>(screenRes.X) / static_cast<float>(screenRes.Y), 0.1f, 256.0f);

    m_uboVS.ViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, m_zoom));

    m_uboVS.ModelMatrix = glm::mat4(1.0f);
    m_uboVS.ModelMatrix = glm::rotate(m_uboVS.ModelMatrix, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    m_uboVS.ModelMatrix = glm::rotate(m_uboVS.ModelMatrix, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    m_uboVS.ModelMatrix = glm::rotate(m_uboVS.ModelMatrix, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // Map uniform buffer and update it
    uint8_t* pData;
    RAPIDVULKAN_CHECK(vkMapMemory(m_device.Get(), m_uniformDataVS.Memory.Get(), 0, sizeof(m_uboVS), 0, reinterpret_cast<void**>(&pData)));
    {
      std::memcpy(pData, &m_uboVS, sizeof(m_uboVS));
      // Unmap after data has been copied
      // Note: Since we requested a host coherent memory type for the uniform buffer, the write is instantly visible to the GPU
    }
    vkUnmapMemory(m_device.Get(), m_uniformDataVS.Memory.Get());
  }


  void Triangle::SetupDescriptorSetLayout()
  {
    // Setup layout of descriptors used in this example
    // Basically connects the different shader stages to descriptors for binding uniform buffers, image samplers, etc.
    // So every shader binding should map to one descriptor set layout binding

    // Binding 0: Uniform buffer (Vertex shader)
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding.descriptorCount = 1;
    layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo descriptorLayout{};
    descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayout.pNext = nullptr;
    descriptorLayout.bindingCount = 1;
    descriptorLayout.pBindings = &layoutBinding;

    m_descriptorSetLayout.Reset(m_device.Get(), descriptorLayout);

    // Create the pipeline layout that is used to generate the rendering pipelines that are based on this descriptor set layout
    // In a more complex scenario you would have different pipeline layouts for different descriptor set layouts that could be reused
    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo{};
    pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pPipelineLayoutCreateInfo.pNext = nullptr;
    pPipelineLayoutCreateInfo.setLayoutCount = 1;
    pPipelineLayoutCreateInfo.pSetLayouts = m_descriptorSetLayout.GetPointer();

    m_pipelineLayout.Reset(m_device.Get(), pPipelineLayoutCreateInfo);
  }


  void Triangle::PreparePipelines()
  {
    // Create the graphics pipeline used in this example
    // Vulkan uses the concept of rendering pipelines to encapsulate fixed states, replacing OpenGL's complex state machine
    // A pipeline is then stored and hashed on the GPU making pipeline changes very fast
    // Note: There are still a few dynamic states that are not directly part of the pipeline (but the info that they are used is)

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    // The layout used for this pipeline (can be shared among multiple pipelines using the same layout)
    pipelineCreateInfo.layout = m_pipelineLayout.Get();
    // Renderpass this pipeline is attached to
    pipelineCreateInfo.renderPass = m_renderPass.Get();

    // Construct the different states making up the pipeline

    // Input assembly state describes how primitives are assembled
    // This pipeline will assemble vertex data as a triangle lists (though we only use one triangle)
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    // Rasterization state
    VkPipelineRasterizationStateCreateInfo rasterizationState{};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationState.cullMode = VK_CULL_MODE_NONE;
    rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationState.depthClampEnable = VK_FALSE;
    rasterizationState.rasterizerDiscardEnable = VK_FALSE;
    rasterizationState.depthBiasEnable = VK_FALSE;
    rasterizationState.lineWidth = 1.0f;

    // Color blend state describes how blend factors are calculated (if used)
    // We need one blend attachment state per color attachment (even if blending is not used
    VkPipelineColorBlendAttachmentState blendAttachmentState[1]{};
    blendAttachmentState[0].colorWriteMask = 0xf;
    blendAttachmentState[0].blendEnable = VK_FALSE;
    VkPipelineColorBlendStateCreateInfo colorBlendState{};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = blendAttachmentState;

    // Viewport state sets the number of viewports and scissor used in this pipeline
    // Note: This is actually overridden by the dynamic states (see below)
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    // Enable dynamic states
    // Most states are baked into the pipeline, but there are still a few dynamic states that can be changed within a command buffer
    // To be able to change these we need do specify which dynamic states will be changed using this pipeline. Their actual states are set later on in
    // the command buffer. For this example we will set the viewport and scissor using dynamic states
    std::vector<VkDynamicState> dynamicStateEnables;
    dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
    dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pDynamicStates = dynamicStateEnables.data();
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

    // Depth and stencil state containing depth and stencil compare and test operations
    // We only use depth tests and want depth tests and writes to be enabled and compare with less or equal
    VkPipelineDepthStencilStateCreateInfo depthStencilState{};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.depthTestEnable = VK_TRUE;
    depthStencilState.depthWriteEnable = VK_TRUE;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilState.depthBoundsTestEnable = VK_FALSE;
    depthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
    depthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
    depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depthStencilState.stencilTestEnable = VK_FALSE;
    depthStencilState.front = depthStencilState.back;

    // Multi sampling state
    // This example does not make use fo multi sampling (for anti-aliasing), the state must still be set and passed to the pipeline
    VkPipelineMultisampleStateCreateInfo multisampleState{};
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleState.pSampleMask = nullptr;

    // Load shaders
    // Vulkan loads it's shaders from an immediate binary representation called SPIR-V
    // Shaders are compiled offline from e.g. GLSL using the reference glslang compiler
    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};
    shaderStages[0] = LoadShader("triangle.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = LoadShader("triangle.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    // Assign the pipeline states to the pipeline creation info structure
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();
    pipelineCreateInfo.pVertexInputState = &m_vertices.InputState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.renderPass = m_renderPass.Get();
    pipelineCreateInfo.pDynamicState = &dynamicState;

    // Create rendering pipeline using the specified states
    m_pipeline.Reset(m_device.Get(), m_pipelineCache.Get(), pipelineCreateInfo);
  }


  void Triangle::SetupDescriptorPool()
  {
    // We need to tell the API the number of max. requested descriptors per type
    VkDescriptorPoolSize typeCounts[1]{};
    // This example only uses one descriptor type (uniform buffer) and only requests one descriptor of this type
    typeCounts[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    typeCounts[0].descriptorCount = 1;
    // For additional types you need to add new entries in the type count list
    // E.g. for two combined image samplers :
    // typeCounts[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    // typeCounts[1].descriptorCount = 2;

    // Create the global descriptor pool
    // All descriptors used in this example are allocated from this pool
    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.pNext = nullptr;
    descriptorPoolInfo.poolSizeCount = 1;
    descriptorPoolInfo.pPoolSizes = typeCounts;
    // Set the max. number of descriptor sets that can be requested from this pool (requesting beyond this limit will result in an error)
    descriptorPoolInfo.maxSets = 1;

    m_descriptorPool.Reset(m_device.Get(), descriptorPoolInfo);
  }


  void Triangle::SetupDescriptorSet()
  {
    // Allocate a new descriptor set from the global descriptor pool
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool.Get();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = m_descriptorSetLayout.GetPointer();

    RAPIDVULKAN_CHECK(vkAllocateDescriptorSets(m_device.Get(), &allocInfo, &m_descriptorSet));

    // Update the descriptor set determining the shader binding points
    // For every binding point used in a shader there needs to be one
    // descriptor set matching that binding point

    VkWriteDescriptorSet writeDescriptorSet{};

    // Binding 0 : Uniform buffer
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = m_descriptorSet;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSet.pBufferInfo = &m_uniformDataVS.Descriptor;
    // Binds this uniform buffer to binding point 0
    writeDescriptorSet.dstBinding = 0;

    vkUpdateDescriptorSets(m_device.Get(), 1, &writeDescriptorSet, 0, nullptr);
  }


  // Build separate command buffers for every framebuffer image
  // Unlike in OpenGL all rendering commands are recorded once into command buffers that are then resubmitted to the queue
  // This allows to generate work upfront and from multiple threads, one of the biggest advantages of Vulkan
  void Triangle::BuildCommandBuffers()
  {
    const auto screenExtent = Convert(GetScreenExtent());

    VkCommandBufferBeginInfo cmdBufInfo{};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufInfo.pNext = nullptr;

    // Set clear values for all framebuffer attachments with loadOp set to clear
    // We use two attachments (color and depth) that are cleared at the start of the subpass and as such we need to set clear values for both
    VkClearValue clearValues[2];
    clearValues[0].color = {{0.0f, 0.0f, 0.2f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = m_renderPass.Get();
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent = screenExtent;
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;

    for (std::size_t i = 0; i < m_drawCmdBuffers.Size(); ++i)
    {
      // Set target frame buffer
      renderPassBeginInfo.framebuffer = m_frameBuffers[i].Get();

      RAPIDVULKAN_CHECK(vkBeginCommandBuffer(m_drawCmdBuffers[i], &cmdBufInfo));

      // Start the first sub pass specified in our default render pass setup by the base class
      // This will clear the color and depth attachment
      vkCmdBeginRenderPass(m_drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

      // Update dynamic viewport state
      VkViewport viewport{};
      viewport.width = static_cast<float>(screenExtent.width);
      viewport.height = static_cast<float>(screenExtent.height);
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      vkCmdSetViewport(m_drawCmdBuffers[i], 0, 1, &viewport);

      // Update dynamic scissor state
      VkRect2D scissor{};
      scissor.offset.x = 0;
      scissor.offset.y = 0;
      scissor.extent = screenExtent;
      vkCmdSetScissor(m_drawCmdBuffers[i], 0, 1, &scissor);

      // Bind descriptor sets describing shader binding points
      vkCmdBindDescriptorSets(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout.Get(), 0, 1, &m_descriptorSet, 0, nullptr);

      // Bind the rendering pipeline
      // The pipeline (state object) contains all states of the rendering pipeline, binding it will set all the states specified at pipeline creation
      // time
      vkCmdBindPipeline(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.Get());

      // Bind triangle vertex buffer (contains position and colors)
      VkDeviceSize offsets[1] = {0};
      vkCmdBindVertexBuffers(m_drawCmdBuffers[i], VERTEX_BUFFER_BIND_ID, 1, m_vertices.Buffer.GetPointer(), offsets);

      // Bind triangle index buffer
      vkCmdBindIndexBuffer(m_drawCmdBuffers[i], m_indices.Buffer.Get(), 0, VK_INDEX_TYPE_UINT32);

      // Draw indexed triangle
      vkCmdDrawIndexed(m_drawCmdBuffers[i], m_indices.Count, 1, 0, 0, 1);

      DrawUI(m_drawCmdBuffers[i]);

      vkCmdEndRenderPass(m_drawCmdBuffers[i]);

      // Ending the render pass will add an implicit barrier transitioning the frame buffer color attachment to
      // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR for presenting it to the windowing system

      RAPIDVULKAN_CHECK(vkEndCommandBuffer(m_drawCmdBuffers[i]));
    }
  }
}
