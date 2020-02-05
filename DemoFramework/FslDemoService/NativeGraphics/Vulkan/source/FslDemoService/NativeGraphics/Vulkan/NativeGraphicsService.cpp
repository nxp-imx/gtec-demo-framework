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

#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsService.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsSwapchainInfo.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslDemoApp/Shared/Host/DemoHostFeatureUtil.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Render/Adapter/INativeGraphics.hpp>
#include <FslUtil/Vulkan1_0/Batch/QuadBatchShaders.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/NativeBatch2D.hpp>
#include <FslUtil/Vulkan1_0/NativeTexture2D.hpp>
#include <FslUtil/Vulkan1_0/Util/ConvertUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/ScreenshotUtil.hpp>
#include "NativeGraphicsBasic2D.hpp"

namespace Fsl
{
  namespace Vulkan
  {
    namespace
    {
    }

    NativeGraphicsService::NativeGraphicsService(const ServiceProvider& serviceProvider)
      : ThreadLocalService(serviceProvider)
    {
    }


    NativeGraphicsService::~NativeGraphicsService() = default;


    void NativeGraphicsService::VulkanInit()
    {
      if (m_state != State::Uninitialized)
      {
        throw UsageErrorException("Expected state to be Uninitialized");
      }
      m_state = State::Initialized;
    }

    void NativeGraphicsService::VulkanShutdown() noexcept
    {
      if (m_state < State::Initialized)
      {
        return;
      }
      FSLLOG3_WARNING_IF(m_state != State::Initialized, "A required shutdown method was not called.");
      VulkanDeviceShutdown();

      assert(m_state == State::Initialized);
      PerformGarbageCollection();

      m_state = State::Uninitialized;
      if (!m_quadBatches.empty())
      {
        FSLLOG3_ERROR("QuadBatch objects found during Vulkan shutdown sequence (NativeBatch usage error)");
        m_quadBatches.clear();
      }
    }


    void NativeGraphicsService::VulkanDeviceInit(const VUDevice& device, const VkQueue queue, const uint32_t queueFamilyIndex)
    {
      if (m_state != State::Initialized)
      {
        throw UsageErrorException("Expected state to be Initialized");
      }

      try
      {
        m_state = State::DeviceInitialized;

        m_resources.Device = device.Get();
        m_resources.Queue = queue;
        m_resources.QueueFamilyIndex = queueFamilyIndex;
        m_resources.PhysicalDevice = device.GetPhysicalDevice();
        m_resources.ImageCreator = std::make_shared<VulkanImageCreator>(device, queue, queueFamilyIndex);

        auto basic2D = m_basic2D.lock();
        if (basic2D)
        {
          basic2D->VulkanDeviceInit(device, queue, queueFamilyIndex, m_resources.ImageCreator);
        }

        // Create the resources
        auto itr = m_quadBatches.begin();
        while (itr != m_quadBatches.end())
        {
          auto quadBatch = itr->lock();
          if (quadBatch)
          {
            quadBatch->CreateDeviceResources(device.GetPhysicalDevice(), device.Get());
            ++itr;
          }
          else
          {
            itr = m_quadBatches.erase(itr);
          }
        }
      }
      catch (const std::exception&)
      {
        VulkanDeviceShutdown();
        throw;
      }
    }


    void NativeGraphicsService::VulkanDeviceShutdown() noexcept
    {
      if (m_state < State::DeviceInitialized)
      {
        return;
      }
      VulkanDestroyDependentResources();
      assert(m_state == State::DeviceInitialized);
      m_state = State::Initialized;

      auto basic2D = m_basic2D.lock();
      if (basic2D)
      {
        basic2D->VulkanDeviceShutdown();
      }

      // Destroy resources on any existing quad batches
      auto itr = m_quadBatches.begin();
      while (itr != m_quadBatches.end())
      {
        auto quadBatch = itr->lock();
        if (quadBatch)
        {
          quadBatch->DestroyDeviceResources();
          ++itr;
        }
        else
        {
          itr = m_quadBatches.erase(itr);
        }
      }
      m_resources = {};
      m_swapchainInfo.reset();
    }

    void NativeGraphicsService::VulkanCreateDependentResources(const uint32_t commandBufferCount, const VkRenderPass renderPass,
                                                               const uint32_t subpass, const Extent2D& screenResolution)
    {
      if (m_state != State::DeviceInitialized)
      {
        throw UsageErrorException("Expected state to be DeviceInitialized");
      }
      if (renderPass == VK_NULL_HANDLE)
      {
        throw std::invalid_argument("renderPass");
      }

      assert(m_state == State::DeviceInitialized);
      try
      {
        m_state = State::DependentResourcesInitialized;
        m_dependentResources.CommandBufferCount = commandBufferCount;
        m_dependentResources.RenderPass = renderPass;
        m_dependentResources.Subpass = subpass;
        m_dependentResources.ScreenResolution = screenResolution;

        // Destroy resources on any existing quad batches
        auto itr = m_quadBatches.begin();
        while (itr != m_quadBatches.end())
        {
          auto quadBatch = itr->lock();
          if (quadBatch)
          {
            quadBatch->CreateDependentResources(commandBufferCount, renderPass, subpass, screenResolution);
            ++itr;
          }
          else
          {
            itr = m_quadBatches.erase(itr);
          }
        }
      }
      catch (const std::exception&)
      {
        VulkanDestroyDependentResources();
        throw;
      }
    }


    void NativeGraphicsService::VulkanDestroyDependentResources() noexcept
    {
      if (m_state < State::DependentResourcesInitialized)
      {
        return;
      }
      assert(m_state == State::DependentResourcesInitialized);
      m_state = State::DeviceInitialized;
      m_dependentResources = {};

      // Destroy resources on any existing quad batches
      auto itr = m_quadBatches.begin();
      while (itr != m_quadBatches.end())
      {
        auto quadBatch = itr->lock();
        if (quadBatch)
        {
          quadBatch->DestroyDependentResources();
          ++itr;
        }
        else
        {
          itr = m_quadBatches.erase(itr);
        }
      }
      m_swapchainInfo.reset();
    }


    void NativeGraphicsService::VulkanBeginFrame(const VkCommandBuffer commandBuffer, const uint32_t commandBufferIndex)
    {
      if (m_state != State::DependentResourcesInitialized)
      {
        throw UsageErrorException("VulkanBeginFrame");
      }

      auto itr = m_quadBatches.begin();
      while (itr != m_quadBatches.end())
      {
        auto quadBatch = itr->lock();
        if (quadBatch)
        {
          quadBatch->BeginFrame(commandBuffer, commandBufferIndex);
          ++itr;
        }
        else
        {
          itr = m_quadBatches.erase(itr);
        }
      }
    }


    void NativeGraphicsService::VulkanEndFrame()
    {
      if (m_state != State::DependentResourcesInitialized)
      {
        throw UsageErrorException("VulkanEndFrame");
      }

      auto itr = m_quadBatches.begin();
      while (itr != m_quadBatches.end())
      {
        auto quadBatch = itr->lock();
        if (quadBatch)
        {
          quadBatch->EndFrame();
          ++itr;
        }
        else
        {
          itr = m_quadBatches.erase(itr);
        }
      }
    }


    void NativeGraphicsService::SetSwapchainInfoLink(const std::weak_ptr<NativeGraphicsSwapchainInfo>& swapchainInfo)
    {
      m_swapchainInfo = swapchainInfo;
    }


    std::shared_ptr<INativeTexture2D> NativeGraphicsService::CreateTexture2D(const RawBitmap& bitmap, const Texture2DFilterHint filterHint,
                                                                             const TextureFlags& textureFlags)
    {
      if (m_state < State::DeviceInitialized)
      {
        throw UsageErrorException("The internal state is incorrect");
      }
      assert(m_resources.ImageCreator);

      return std::make_shared<NativeTexture2D>(m_resources.ImageCreator, bitmap, filterHint, textureFlags);
    }


    std::shared_ptr<INativeTexture2D> NativeGraphicsService::CreateTexture2D(const RawTexture& texture, const Texture2DFilterHint filterHint,
                                                                             const TextureFlags& textureFlags)
    {
      if (m_state < State::DeviceInitialized)
      {
        throw UsageErrorException("The internal state is incorrect");
      }
      assert(m_resources.ImageCreator);

      return std::make_shared<NativeTexture2D>(m_resources.ImageCreator, texture, filterHint, textureFlags);
    }


    bool NativeGraphicsService::IsSupported(const DemoHostFeature& activeAPI) const
    {
      return true;
    }


    void NativeGraphicsService::Capture(Bitmap& rBitmap, const Rectangle& srcRectangle)
    {
      auto swapchainInfo = m_swapchainInfo.lock();
      if (!swapchainInfo || m_resources.Device == VK_NULL_HANDLE)
      {
        FSLLOG3_WARNING("Not ready, capture failed");
        rBitmap.Reset();
        return;
      }

      rBitmap = ScreenshotUtil::TryCaptureScreenshot(m_resources.PhysicalDevice.Device, m_resources.Device, m_resources.Queue,
                                                     m_resources.QueueFamilyIndex, swapchainInfo->FrameImage, swapchainInfo->ImageFormat,
                                                     swapchainInfo->ImageUsageFlags, ConvertUtil::Convert(m_dependentResources.ScreenResolution));
    }


    std::shared_ptr<INativeGraphicsBasic2D> NativeGraphicsService::CreateBasic2D(const Point2& currentResolution)
    {
      if (!m_basic2D.expired())
      {
        throw UsageErrorException("Only one NativeGraphicsBasic2D instance allowed at a time");
      }

      auto quadBatch = CreateQuadBatch();
      auto basic2D = std::shared_ptr<NativeGraphicsBasic2D>(new NativeGraphicsBasic2D(quadBatch, currentResolution));
      m_basic2D = basic2D;
      return basic2D;
    }


    std::shared_ptr<INativeBatch2D> NativeGraphicsService::CreateNativeBatch2D(const Point2& currentResolution)
    {
      PerformGarbageCollection();

      auto quadBatch = CreateQuadBatch();
      return std::make_shared<NativeBatch2D>(quadBatch, currentResolution);
    }


    void NativeGraphicsService::PerformGarbageCollection() noexcept
    {
      auto itr = m_quadBatches.begin();
      while (itr != m_quadBatches.end())
      {
        if (!itr->expired())
        {
          ++itr;
        }
        else
        {
          itr = m_quadBatches.erase(itr);
        }
      }
    }

    std::shared_ptr<QuadBatch> NativeGraphicsService::CreateQuadBatch()
    {
      const auto contentFragShader = QuadBatchShaders::GetFragmentShader();
      const auto contentVertShader = QuadBatchShaders::GetVertexShader();
      std::vector<uint8_t> fragShaderBinary(contentFragShader.Data, contentFragShader.Data + contentFragShader.Length);
      std::vector<uint8_t> vertShaderBinary(contentVertShader.Data, contentVertShader.Data + contentVertShader.Length);

      auto quadBatch = std::make_shared<QuadBatch>(vertShaderBinary, fragShaderBinary, GenericBatch2D_DEFAULT_CAPACITY);

      if (m_state >= State::DeviceInitialized)
      {
        quadBatch->CreateDeviceResources(m_resources.PhysicalDevice, m_resources.Device);
      }
      if (m_state >= State::DependentResourcesInitialized)
      {
        quadBatch->CreateDependentResources(m_dependentResources.CommandBufferCount, m_dependentResources.RenderPass, m_dependentResources.Subpass,
                                            m_dependentResources.ScreenResolution);
      }

      m_quadBatches.push_back(quadBatch);
      return quadBatch;
    }
  }
}
