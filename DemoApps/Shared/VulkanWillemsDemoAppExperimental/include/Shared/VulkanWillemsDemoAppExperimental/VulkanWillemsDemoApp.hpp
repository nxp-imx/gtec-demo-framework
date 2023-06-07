#ifndef SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_VULKANWILLEMSDEMOAPP_HPP
#define SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_VULKANWILLEMSDEMOAPP_HPP
/*
 * Vulkan Example base class
 *
 * Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */


// Based on a code by Sascha Willems from https://github.com/SaschaWillems/Vulkan
// Recreated as a DemoFramework freestyle window sample by Freescale (2016)
// This class simulates the functionality found in VulkanExampleBase to make it easier
// to port samples.

#include <FslDemoApp/Vulkan/DemoAppVulkan.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/Camera.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/DepthStencil.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/MeshLoader/MeshBuffer.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/MeshLoader/MeshCreateInfo.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/MeshLoader/VertexLayout.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/MeshLoader/VulkanMeshLoader.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/Swapchain.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/VulkanDevice.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/VulkanTextOverlay.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/VulkanTextureLoader.hpp>
// #include <Shared/VulkanWillemsDemoAppExperimental/Camera.hpp>
#include <RapidVulkan/CommandBuffer.hpp>
#include <RapidVulkan/CommandBuffers.hpp>
#include <RapidVulkan/CommandPool.hpp>
#include <RapidVulkan/Device.hpp>
#include <RapidVulkan/Framebuffer.hpp>
#include <RapidVulkan/PipelineCache.hpp>
#include <RapidVulkan/RenderPass.hpp>
#include <RapidVulkan/Semaphore.hpp>
#include <RapidVulkan/ShaderModule.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include <functional>
#include <string>

namespace Fsl::Willems
{
  class VulkanWillemsDemoApp : public DemoAppVulkan
  {
  public:
    using MeshLoaderAllocFunc = std::function<std::unique_ptr<VulkanMeshLoader>(const std::shared_ptr<IContentManager>&)>;

  protected:
    VulkanDevice m_vulkanDevice;

  private:
    MeshLoaderAllocFunc m_meshLoaderAllocFunc;
    //! Indicates that the view (position, rotation) has changed.
    bool m_viewChanged;

    // Synchronization semaphores
    struct Semaphores
    {
      // Swap chain image presentation
      RapidVulkan::Semaphore PresentComplete;
      // Command buffer submission and execution
      RapidVulkan::Semaphore RenderComplete;
      // Text overlay submission and execution
      RapidVulkan::Semaphore TextOverlayComplete;
    };

    struct LocalMouseButtons
    {
      bool Left = false;
      bool Right = false;
      bool Middle = false;
    };

    Semaphores m_semaphores;

  public:
    void _PostConstruct() override;
    void _Update(const DemoTime& demoTime) override;
    void _Draw(const FrameInfo& frameInfo) override;

  protected:
    void OnKeyEvent(const KeyEvent& event) override;
    void OnMouseButtonEvent(const MouseButtonEvent& event) override;
    void OnMouseMoveEvent(const MouseMoveEvent& event) override;
    void OnMouseWheelEvent(const MouseWheelEvent& event) override;
    // Last frame time, measured using a high performance timer (if available)
    float m_frameTimer = 1.0f;
    // Frame counter to display fps
    uint32_t m_frameCounter = 0;
    uint32_t m_lastFPS = 0;
    // Defines a frame rate independent timer value clamped from -1.0...1.0
    // For use in animations, rotations, etc.
    float m_timer = 0.0f;
    // Multiplier for speeding up (or slowing down) the global timer
    float m_timerSpeed = 0.25f;

    bool m_paused = false;

    VkClearColorValue m_defaultClearColor{};

    //! @brief Pipeline stages used to wait at for graphics queue submissions
    VkPipelineStageFlags m_submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    // Contains command buffers and semaphores to be presented to the queue
    VkSubmitInfo m_submitInfo{};

    bool m_enableVSync;
    // fps timer (one second interval)
    float m_fpsTimer = 0.0f;

    // Stores physical device properties (for e.g. checking device limits)
    VkPhysicalDeviceProperties m_deviceProperties;
    // Stores physical device features (for e.g. checking if a feature is available)
    VkPhysicalDeviceFeatures m_deviceFeatures;

    RapidVulkan::CommandPool m_commandPool;
    RapidVulkan::CommandBuffer m_setupCmdBuffer;
    // SetupSwapchain
    Willems::Swapchain m_swapchain;
    RapidVulkan::CommandBuffers m_drawCmdBuffers;
    // SetupDepthStencil
    Willems::DepthStencil m_depthStencil;
    // SetupRenderPass
    RapidVulkan::RenderPass m_renderPass;
    // CreatePipelineCache
    RapidVulkan::PipelineCache m_pipelineCache;
    // SetupFrameBuffer
    std::vector<RapidVulkan::Framebuffer> m_frameBuffers;

    std::unique_ptr<VulkanTextureLoader> m_textureLoader;

    uint32_t m_currentBufferIndex = 0;

    std::vector<RapidVulkan::ShaderModule> m_shaderModules;

    // Use to adjust mouse rotation speed
    float m_rotationSpeed = 1.0f;
    // Use to adjust mouse zoom speed
    float m_zoomSpeed = 1.0f;

    Camera m_camera;

    float m_zoom = 0;
    glm::vec3 m_rotation{};
    glm::vec3 m_cameraPos{};
    glm::vec2 m_mousePos{};

    std::string m_title = "Vulkan Example";
    std::string m_name = "vulkanExample";

    bool m_enableTextOverlay;
    std::unique_ptr<VulkanTextOverlay> m_textOverlay;

    LocalMouseButtons m_mouseButtons;
    bool m_commandBuffersDirty = false;

    explicit VulkanWillemsDemoApp(const DemoAppConfig& demoAppConfig, MeshLoaderAllocFunc meshLoaderAllocFunc = nullptr);
    ~VulkanWillemsDemoApp() override;

    virtual void Prepare();
    //! @brief Called whenever the view changes
    virtual void OnViewChanged()
    {
    }

    // Creates a new (graphics) command pool object storing command buffers
    void CreateCommandPool();
    // Create command buffer for setup commands
    void CreateSetupCommandBuffer();
    // Create swap chain images
    void SetupSwapchain();
    // Create command buffers for drawing commands
    void CreateCommandBuffers();
    // Setup default depth and stencil views
    virtual void SetupDepthStencil(const VkFormat depthFormat);
    // Setup a default render pass
    // Can be overriden in derived class to setup a custom render pass (e.g. for MSAA)
    virtual void SetupRenderPass(const VkFormat colorFormat, const VkFormat depthFormat);
    // Create a cache pool for rendering pipelines
    void CreatePipelineCache();
    // Create framebuffers for all requested swap chain images
    // Can be overriden in derived class to setup a custom framebuffer (e.g. for MSAA)
    virtual void SetupFrameBuffer();
    // Finalize setup command bufferm submit it to the queue and remove it
    void FlushSetupCommandBuffer();

    // Pure virtual function to be overriden by the derived class
    // Called in case of an event where e.g. the framebuffer has to be rebuild and thus
    // all command buffers that may reference this
    virtual void BuildCommandBuffers() = 0;


    //// Connect and prepare the swap chain
    // void InitSwapchain();

    // Check if command buffers are valid (!= VK_NULL_HANDLE)
    bool CheckCommandBuffers();
    // Destroy all command buffers and set their handles to VK_NULL_HANDLE
    // May be necessary during runtime if options are toggled
    void DestroyCommandBuffers();

    //! Command buffer creation
    //! Creates and returns a new command buffer
    RapidVulkan::CommandBuffer CreateCommandBuffer(const VkCommandBufferLevel level, const bool begin);
    //! End the command buffer, submit it to the queue and free (if requested)
    //! Note : Waits for the queue to become idle
    void FlushCommandBuffer(RapidVulkan::CommandBuffer& rCommandBuffer, const VkQueue queue, const bool free);

    //! Create a buffer, fill it with data (if != nullptr) and bind buffer memory
    void CreateBuffer(RapidVulkan::Buffer& rBuffer, RapidVulkan::Memory& rMemory, const VkBufferUsageFlags usageFlags,
                      const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize size, const void* pData);

    //! This version always uses HOST_VISIBLE memory
    void CreateBuffer(RapidVulkan::Buffer& rBuffer, RapidVulkan::Memory& rMemory, const VkBufferUsageFlags usage, const VkDeviceSize size,
                      const void* const pData);

    // Overload that assigns buffer info to descriptor
    void CreateBuffer(RapidVulkan::Buffer& rBuffer, RapidVulkan::Memory& rMemory, VkDescriptorBufferInfo& rDescriptor, const VkBufferUsageFlags usage,
                      const VkDeviceSize size, const void* const pData);

    // Overload to pass memory property flags
    void CreateBuffer(RapidVulkan::Buffer& rBuffer, RapidVulkan::Memory& rMemory, VkDescriptorBufferInfo& rDescriptor, const VkBufferUsageFlags usage,
                      const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize size, const void* const pData);

    void UpdateTextOverlay();

    // Called when the text overlay is updating
    // Can be overriden in derived class to add custom text to the overlay
    virtual void GetOverlayText(VulkanTextOverlay& rTextOverlay);

    // Prepare the frame for workload submission
    // - Acquires the next image from the swap chain
    // - Sets the default wait and signal semaphores
    //! @return true if the frame was prepared, false if a app restart was requested
    //! @note if a serious error occurred this actually throws!
    bool TryPrepareFrame();

    // Submit the frames' workload
    // - Submits the text overlay (if enabled)
    void SubmitFrame();

    VkPipelineShaderStageCreateInfo LoadShader(const std::string& fileName, const VkShaderStageFlagBits stage);
    RapidVulkan::ShaderModule LoadShader(const std::string& fileName, const VkDevice device, const VkShaderStageFlagBits stage);

    // Load a mesh (using ASSIMP) and create vulkan vertex and index buffers with given vertex layout
    MeshLoader::MeshBuffer LoadMesh(const std::string& filename, const std::vector<MeshLoader::VertexLayout>& vertexLayout, const float scale);
    MeshLoader::MeshBuffer LoadMesh(const std::string& filename, const std::vector<MeshLoader::VertexLayout>& vertexLayout,
                                    const MeshLoader::MeshCreateInfo* const pMeshCreateInfo);

    void DrawUI(const VkCommandBuffer commandBuffer);
  };
}

#endif
