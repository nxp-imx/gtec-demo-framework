#ifndef FSLDEMOAPP_VULKAN_BASIC_DEMOAPPVULKANBASIC_HPP
#define FSLDEMOAPP_VULKAN_BASIC_DEMOAPPVULKANBASIC_HPP
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

#include <FslDemoApp/Vulkan/Basic/BuildResourcesContext.hpp>
#include <FslDemoApp/Vulkan/Basic/DemoAppVulkanSetup.hpp>
#include <FslDemoApp/Vulkan/Basic/DrawContext.hpp>
#include <FslDemoApp/Vulkan/Basic/FrameBufferCreateContext.hpp>
#include <FslDemoApp/Vulkan/Basic/SwapchainInfo.hpp>
#include <FslDemoApp/Vulkan/DemoAppVulkan.hpp>
#include <FslUtil/Vulkan1_0/SurfaceFormatInfo.hpp>
#include <FslUtil/Vulkan1_0/VUImageMemoryView.hpp>
#include <FslUtil/Vulkan1_0/VUSwapchainKHR.hpp>
#include <RapidVulkan/CommandBuffers.hpp>
#include <RapidVulkan/CommandPool.hpp>
#include <RapidVulkan/Fence.hpp>
#include <RapidVulkan/Framebuffer.hpp>
#include <RapidVulkan/ImageView.hpp>
#include <RapidVulkan/RenderPass.hpp>
#include <RapidVulkan/Semaphore.hpp>
#include <vulkan/vulkan.h>
#include <cassert>
#include <memory>
#include <utility>
#include <vector>

namespace Fsl
{
  class DemoAppHostConfigVulkan;
  class DemoAppProfilerOverlay;

  namespace Vulkan
  {
    struct NativeGraphicsSwapchainInfo;
  }

  namespace VulkanBasic
  {
    class DemoAppVulkanBasic : public DemoAppVulkan
    {
    private:
      enum class AppState
      {
        // The app is ready to render
        Ready,
        // We are waiting for the swapchain to be re-creatable.
        // This can occur on windows when the app is minimized and the vkGetPhysicalDeviceSurfaceCapabilitiesKHR call can
        // return a surface with a max extent of zero.
        WaitForSwapchainRecreation
      };

      enum class RecreateSwapchainResult
      {
        Failed,
        NotReady,
        Completed    // Named Complete because we can't use Success as the X11 header might define it :(
      };

      //! We overload the move operator and constructor to ensure we destroy resources in destruction order.
      struct FrameDrawRecord
      {
        RapidVulkan::Semaphore ImageAcquiredSemaphore;
        RapidVulkan::Semaphore ImageReleasedSemaphore;
        RapidVulkan::Fence QueueSubmitFence;
        uint32_t AssignedSwapImageIndex{0};
        bool HasSwapBufferImage{false};

        FrameDrawRecord() noexcept = default;

        FrameDrawRecord(const FrameDrawRecord&) = delete;
        FrameDrawRecord& operator=(const FrameDrawRecord&) = delete;

        FrameDrawRecord(FrameDrawRecord&& other) noexcept
          : ImageAcquiredSemaphore(std::move(other.ImageAcquiredSemaphore))
          , ImageReleasedSemaphore(std::move(other.ImageReleasedSemaphore))
          , QueueSubmitFence(std::move(other.QueueSubmitFence))
          , AssignedSwapImageIndex(other.AssignedSwapImageIndex)
          , HasSwapBufferImage(other.HasSwapBufferImage)
        {
          other.AssignedSwapImageIndex = 0;
          other.HasSwapBufferImage = false;
        }

        FrameDrawRecord& operator=(FrameDrawRecord&& other) noexcept
        {
          if (this != &other)
          {
            Reset();

            ImageAcquiredSemaphore = std::move(other.ImageAcquiredSemaphore);
            ImageReleasedSemaphore = std::move(other.ImageReleasedSemaphore);
            QueueSubmitFence = std::move(other.QueueSubmitFence);
            AssignedSwapImageIndex = other.AssignedSwapImageIndex;
            HasSwapBufferImage = other.HasSwapBufferImage;

            other.AssignedSwapImageIndex = 0;
            other.HasSwapBufferImage = false;
          }
          return *this;
        }

        void Reset() noexcept
        {
          // Reset in destruction order
          HasSwapBufferImage = false;
          AssignedSwapImageIndex = 0;
          QueueSubmitFence.Reset();
          ImageReleasedSemaphore.Reset();
          ImageAcquiredSemaphore.Reset();
        }
      };

      //! We overload the move operator and constructor to ensure we destroy resources in destruction order.
      struct SwapchainRecord
      {
        RapidVulkan::ImageView SwapchainImageView;
        RapidVulkan::Framebuffer Framebuffer;
        //! This is the frame index it was assigned to
        uint32_t AssignedFrameIndex{0};
        //! This is true if this swapchain record has been assigned to a frame index
        bool HasAssignedFrame{false};

        SwapchainRecord() noexcept = default;

        SwapchainRecord(const SwapchainRecord&) = delete;
        SwapchainRecord& operator=(const SwapchainRecord&) = delete;

        SwapchainRecord(SwapchainRecord&& other) noexcept
          : SwapchainImageView(std::move(other.SwapchainImageView))
          , Framebuffer(std::move(other.Framebuffer))
          , AssignedFrameIndex(other.AssignedFrameIndex)
          , HasAssignedFrame(other.HasAssignedFrame)
        {
          other.AssignedFrameIndex = 0;
          other.HasAssignedFrame = false;
        }

        SwapchainRecord& operator=(SwapchainRecord&& other) noexcept
        {
          if (this != &other)
          {
            Reset();

            SwapchainImageView = std::move(other.SwapchainImageView);
            Framebuffer = std::move(other.Framebuffer);
            AssignedFrameIndex = other.AssignedFrameIndex;
            HasAssignedFrame = other.HasAssignedFrame;

            other.AssignedFrameIndex = 0;
            other.HasAssignedFrame = false;
          }
          return *this;
        }

        void Reset() noexcept
        {
          // Reset in destruction order
          HasAssignedFrame = false;
          AssignedFrameIndex = 0;
          Framebuffer.Reset();
          SwapchainImageView.Reset();
        }
      };

      struct Resources
      {
        RapidVulkan::CommandPool MainCommandPool;
        std::vector<RapidVulkan::Semaphore> m_recycledSemaphores;
        std::vector<FrameDrawRecord> Frames;

        Resources() noexcept = default;
        Resources(const Resources&) = delete;
        Resources& operator=(const Resources&) = delete;
        Resources(Resources&& other) noexcept = delete;
        Resources& operator=(Resources&& other) noexcept = delete;

        void Reset() noexcept
        {
          // Reset in destruction order
          Frames.clear();
          m_recycledSemaphores.clear();
          MainCommandPool.Reset();
        }

        RapidVulkan::Semaphore AcquireSemaphore(VkDevice device)
        {
          if (!m_recycledSemaphores.empty())
          {
            auto tmp = std::move(m_recycledSemaphores.back());
            m_recycledSemaphores.pop_back();
            return tmp;
          }
          return {device, 0};
        }

        void ReleaseSemaphore(RapidVulkan::Semaphore&& value)
        {
          m_recycledSemaphores.push_back(std::move(value));
          // If this fires its likely we have a error
          assert(m_recycledSemaphores.size() < 1024);
        }
      };

      struct FrameRecord
      {
        bool IsValid{false};
        uint32_t FrameIndex{0};

        FrameRecord() noexcept = default;

        explicit FrameRecord(const uint32_t frameIndex) noexcept
          : IsValid(true)
          , FrameIndex(frameIndex)
        {
        }
      };

      struct DependentResources
      {
        bool Valid = false;
        uint32_t FramesInFlightCount{0};
        //! A optional Depth image, only enabled if requested
        //! We only need one RenderAttachment as command buffers on the same queue are executed in order
        Vulkan::VUImageMemoryView DepthImage;
        RapidVulkan::CommandBuffers CmdBuffers;
        std::vector<SwapchainRecord> SwapchainRecords;
        std::shared_ptr<Vulkan::NativeGraphicsSwapchainInfo> NGScreenshotLink;

        DependentResources() = default;
        DependentResources(const DependentResources&) = delete;
        DependentResources& operator=(const DependentResources&) = delete;
        DependentResources(DependentResources&& other) noexcept = delete;
        DependentResources& operator=(DependentResources&& other) noexcept = delete;

        void Reset() noexcept
        {
          // Reset in destruction order

          NGScreenshotLink.reset();
          SwapchainRecords.clear();
          CmdBuffers.Reset();
          DepthImage.Reset();
          FramesInFlightCount = 0;
          Valid = false;
        }
      };

      std::shared_ptr<DemoAppHostConfigVulkan> m_demoHostConfig;
      Vulkan::SurfaceFormatInfo m_surfaceFormatInfo;

      const DemoAppVulkanSetup m_appSetup;
      Resources m_resources;
      Vulkan::VUSwapchainKHR m_swapchain;
      DependentResources m_dependentResources;
      FrameRecord m_frameRecord;

      AppState m_currentAppState = AppState::Ready;
      std::unique_ptr<DemoAppProfilerOverlay> m_demoAppProfilerOverlay;
      PxExtent2D m_cachedExtentPx;

    protected:
      explicit DemoAppVulkanBasic(const DemoAppConfig& demoAppConfig, const DemoAppVulkanSetup& demoAppVulkanSetup = {});
      ~DemoAppVulkanBasic() override;
      void OnConstructed() override;
      void OnDestroy() override;

      AppDrawResult TryPrepareDraw(const FrameInfo& frameInfo) override;

    public:
      void _BeginDraw(const FrameInfo& frameInfo) override;
      void _EndDraw(const FrameInfo& frameInfo) override;

    protected:
      void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) override;
      void Draw(const FrameInfo& frameInfo) final;
      AppDrawResult TrySwapBuffers(const FrameInfo& frameInfo) override;

      void AddSystemUI(VkCommandBuffer hCmdBuffer, const uint32_t frameIndex);

      void BuildResources();
      void FreeResources();

      //! Check if the dependent resources are currently allocated
      bool IsResourcesAllocated() const
      {
        return m_dependentResources.Valid;
      }

      VkCommandPool GetCommandPool() const
      {
        return m_resources.MainCommandPool.Get();
      }

      bool TryRebuildResources();

      //! @brief Called after the swapchain has been created
      //! @return Must return the main RenderPass.
      virtual VkRenderPass OnBuildResources(const BuildResourcesContext& context) = 0;

      //! @brief Called after the swapchain ImageView and FrameBuffers has been deleted
      virtual void OnFreeResources()
      {
      }

      virtual void VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const DrawContext& drawContext) = 0;

      //! @brief get the swapchain image count
      uint32_t GetSwapchainImageCount() const
      {
        if (!m_swapchain.IsValid() || m_currentAppState != AppState::Ready)
        {
          return 0;
        }
        return m_swapchain.GetImageCount();
      }

      bool TryGetSwapchainInfo(SwapchainInfo& rSwapchainInfo) const;

      //! @brief Call this to create a very basic render pass. This will only be useful for the most basic of applications and
      //         to quickly get something on the screen to get started.
      RapidVulkan::RenderPass CreateBasicRenderPass();

      virtual RapidVulkan::Framebuffer CreateFramebuffer(const FrameBufferCreateContext& frameBufferCreateContext);

      //! @brief Call this to crate a 'basic' depth image view.
      //         The created depth image view will be in the VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL layout.
      static Vulkan::VUImageMemoryView CreateBasicDepthImageView(const Vulkan::VUDevice& device, const VkExtent2D& depthImageExtent,
                                                                 const VkCommandPool commandPool);

      const Vulkan::SurfaceFormatInfo& GetSurfaceFormatInfo() const
      {
        return m_surfaceFormatInfo;
      }

    private:
      static std::vector<FrameDrawRecord> CreateFrameSyncObjects(const VkDevice device, const uint32_t maxFramesInFlight);
      void BuildSwapchainImageView(SwapchainRecord& rSwapchainRecord, const uint32_t swapBufferIndex);

      RecreateSwapchainResult TryRecreateSwapchain();

      AppDrawResult TryDoPrepareDraw(const FrameInfo& frameInfo);
      AppDrawResult TryDoSwapBuffers(const FrameInfo& frameInfo);
      void SetAppState(AppDrawResult result);
    };
  }
}

#endif
