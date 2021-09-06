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
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslDemoApp/Shared/Host/DemoHostFeatureUtil.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeBeginFrameInfo.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeDependentCreateInfo.hpp>
#include <FslDemoService/NativeGraphics/Base/NativeGraphicsDeviceCreateInfo.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeBatch2D.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/BasicNativeBeginCustomVulkanFrameInfo.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/BasicNativeDependentCustomVulkanCreateInfo.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsCustomVulkanDeviceCreateInfo.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsDevice.hpp>
#include <FslDemoService/NativeGraphics/Vulkan/NativeGraphicsSwapchainInfo.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Render/Adapter/INativeGraphics.hpp>
#include <FslGraphics3D/BasicRender/BasicRenderSystemCreateInfo.hpp>
#include <FslUtil/Vulkan1_0/Batch/QuadBatchShaders.hpp>
#include <FslUtil/Vulkan1_0/Draft/VulkanImageCreator.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <FslUtil/Vulkan1_0/TypeConverter.hpp>
#include <FslUtil/Vulkan1_0/Util/ScreenshotUtil.hpp>
#include <memory>
#include "NativeGraphicsBasic2D.hpp"

namespace Fsl
{
  namespace Vulkan
  {
    NativeGraphicsService::NativeGraphicsService(const ServiceProvider& serviceProvider)
      : ANativeGraphicsService(serviceProvider)
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
      DestroyDevice();

      assert(m_state == State::Initialized);
      PerformGarbageCollection();

      m_state = State::Uninitialized;
      if (!m_quadBatches.empty())
      {
        FSLLOG3_ERROR("Found {} QuadBatch objects during Vulkan shutdown sequence (NativeBatch usage error)", m_quadBatches.size());
        m_quadBatches.clear();
      }
    }


    void NativeGraphicsService::CreateDevice(const NativeGraphicsDeviceCreateInfo& createInfo)
    {
      FSLLOG3_VERBOSE("NativeGraphicsService::VulkanDeviceInit");
      if (m_state != State::Initialized)
      {
        throw UsageErrorException("Expected state to be Initialized");
      }
      const auto* pVulkanCreateInfo = dynamic_cast<const NativeGraphicsCustomVulkanDeviceCreateInfo*>(createInfo.pCustomCreateInfo);
      if (pVulkanCreateInfo == nullptr)
      {
        throw UsageErrorException("Missing NativeGraphicsCustomDeviceCreateInfo");
      }

      try
      {
        m_state = State::DeviceInitialized;

        m_resources.MaxFramesInFlight = createInfo.MaxFramesInFlight;
        m_resources.Device = pVulkanCreateInfo->Device.Get();
        m_resources.Queue = pVulkanCreateInfo->Queue;
        m_resources.QueueFamilyIndex = pVulkanCreateInfo->QueueFamilyIndex;
        m_resources.PhysicalDevice = pVulkanCreateInfo->Device.GetPhysicalDevice();
        m_resources.ImageCreator =
          std::make_shared<VulkanImageCreator>(pVulkanCreateInfo->Device, pVulkanCreateInfo->Queue, pVulkanCreateInfo->QueueFamilyIndex);
        m_resources.BufferManager = std::make_shared<VMBufferManager>(pVulkanCreateInfo->Device.GetPhysicalDevice(), pVulkanCreateInfo->Device.Get(),
                                                                      pVulkanCreateInfo->Queue, pVulkanCreateInfo->QueueFamilyIndex);
        m_resources.NativeDevice =
          std::make_shared<NativeGraphicsDevice>(pVulkanCreateInfo->Device, pVulkanCreateInfo->Queue, pVulkanCreateInfo->QueueFamilyIndex,
                                                 m_resources.BufferManager, m_resources.ImageCreator, createInfo.MaxFramesInFlight);

        ANativeGraphicsService::CreateDevice(createInfo);

        auto basic2D = m_basic2D.lock();
        if (basic2D)
        {
          basic2D->VulkanDeviceInit(m_resources.ImageCreator);
        }

        // Create the resources
        auto itr = m_quadBatches.begin();
        while (itr != m_quadBatches.end())
        {
          auto quadBatch = itr->lock();
          if (quadBatch)
          {
            quadBatch->CreateDeviceResources(pVulkanCreateInfo->Device.GetPhysicalDevice(), pVulkanCreateInfo->Device.Get());
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
        DestroyDevice();
        throw;
      }
    }


    void NativeGraphicsService::DestroyDevice() noexcept
    {
      if (m_state < State::DeviceInitialized)
      {
        return;
      }
      FSLLOG3_VERBOSE("NativeGraphicsService::VulkanDeviceShutdown");

      if (m_state >= State::DependentResourcesInitialized)
      {
        DestroyDependentResources();
      }

      assert(m_state == State::DeviceInitialized);
      m_state = State::Initialized;

      ANativeGraphicsService::DestroyDevice();

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
      if (m_resources.NativeDevice)
      {
        // We mark the NativeDevice as disposed to make it easier to find use after all pointers ought to have been freed issues
        FSLLOG3_VERBOSE6("Marking the NativeDevice as disposed");
        m_resources.NativeDevice->Dispose();
      }
      m_resources = {};
      m_swapchainInfo.reset();
    }

    //! @sets the swapchain info, this class will only keep a weak pointer to the information.
    void NativeGraphicsService::CreateDependentResources(const BasicNativeDependentCreateInfo& createInfo)
    {
      FSLLOG3_VERBOSE("NativeGraphicsService::CreateDependentResources()");
      if (m_state != State::DeviceInitialized)
      {
        throw UsageErrorException("Expected state to be DeviceInitialized");
      }

      const auto* pVulkanCreateInfo = dynamic_cast<const BasicNativeDependentCustomVulkanCreateInfo*>(createInfo.pCustomCreateInfo);
      if (pVulkanCreateInfo == nullptr)
      {
        throw UsageErrorException("Missing NativeGraphicsCustomVulkanDependentCreateInfo");
      }


      if (pVulkanCreateInfo->MainRenderPass == VK_NULL_HANDLE)
      {
        throw std::invalid_argument("renderPass");
      }
      // assert(createInfo.FramesInFlightCount <= m_resources.MaxFramesInFlight);

      assert(m_state == State::DeviceInitialized);
      try
      {
        m_state = State::DependentResourcesInitialized;
        m_dependentResources.RenderPass = pVulkanCreateInfo->MainRenderPass;
        m_dependentResources.Subpass = pVulkanCreateInfo->SubpassSystemUI;
        m_dependentResources.ScreenExtentPx = createInfo.ExtentPx;

        // Destroy resources on any existing quad batches
        auto itr = m_quadBatches.begin();
        while (itr != m_quadBatches.end())
        {
          auto quadBatch = itr->lock();
          if (quadBatch)
          {
            quadBatch->CreateDependentResources(m_resources.MaxFramesInFlight, m_dependentResources.RenderPass, m_dependentResources.Subpass,
                                                m_dependentResources.ScreenExtentPx);
            ++itr;
          }
          else
          {
            itr = m_quadBatches.erase(itr);
          }
        }

        ANativeGraphicsService::CreateDependentResources(createInfo);

        m_swapchainInfo = pVulkanCreateInfo->SwapchainInfo;
      }
      catch (const std::exception&)
      {
        DestroyDependentResources();
        throw;
      }
    }

    void NativeGraphicsService::DestroyDependentResources() noexcept
    {
      if (m_state < State::DependentResourcesInitialized)
      {
        return;
      }
      FSLLOG3_VERBOSE("NativeGraphicsService::DestroyDependentResources");

      m_swapchainInfo.reset();

      assert(m_state == State::DependentResourcesInitialized);
      ANativeGraphicsService::DestroyDependentResources();

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


    void NativeGraphicsService::BeginFrame(const BasicNativeBeginFrameInfo& frameInfo)
    {
      if (m_state != State::DependentResourcesInitialized)
      {
        throw UsageErrorException("BeginFrame");
      }
      const auto* pVulkanInfo = dynamic_cast<const BasicNativeBeginCustomVulkanFrameInfo*>(frameInfo.pCustomInfo);
      if (pVulkanInfo == nullptr)
      {
        throw UsageErrorException("Missing NativeGraphicsCustomVulkanDependentCreateInfo");
      }


      auto itr = m_quadBatches.begin();
      while (itr != m_quadBatches.end())
      {
        auto quadBatch = itr->lock();
        if (quadBatch)
        {
          quadBatch->BeginFrame(pVulkanInfo->CommandBuffer, frameInfo.FrameIndex);
          ++itr;
        }
        else
        {
          itr = m_quadBatches.erase(itr);
        }
      }
      ANativeGraphicsService::BeginFrame(frameInfo);
    }


    void NativeGraphicsService::EndFrame()
    {
      if (m_state != State::DependentResourcesInitialized)
      {
        throw UsageErrorException("EndFrame");
      }

      ANativeGraphicsService::EndFrame();

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


    bool NativeGraphicsService::IsSupported(const DemoHostFeature& activeAPI) const
    {
      return activeAPI.Name == DemoHostFeatureName::Vulkan;
    }


    void NativeGraphicsService::Capture(Bitmap& rBitmap, const Rectangle& /*srcRectangle*/)
    {
      auto swapchainInfo = m_swapchainInfo.lock();
      if (!swapchainInfo || m_resources.Device == VK_NULL_HANDLE)
      {
        FSLLOG3_WARNING("Not ready, capture failed");
        rBitmap.Reset();
        return;
      }

      rBitmap = ScreenshotUtil::TryCaptureScreenshot(
        m_resources.PhysicalDevice.Device, m_resources.Device, m_resources.Queue, m_resources.QueueFamilyIndex, swapchainInfo->FrameImage,
        swapchainInfo->ImageFormat, swapchainInfo->ImageUsageFlags, TypeConverter::UncheckedTo<VkExtent2D>(m_dependentResources.ScreenExtentPx));
    }


    std::shared_ptr<INativeGraphicsBasic2D> NativeGraphicsService::CreateBasic2D(const PxExtent2D& currentExtent)
    {
      if (!m_basic2D.expired())
      {
        throw UsageErrorException("Only one NativeGraphicsBasic2D instance allowed at a time");
      }

      auto quadBatch = CreateQuadBatch();
      auto basic2D = std::make_shared<NativeGraphicsBasic2D>(quadBatch, currentExtent);

      UpdateState(*basic2D);
      m_basic2D = basic2D;
      return basic2D;
    }


    std::shared_ptr<INativeBatch2D> NativeGraphicsService::CreateNativeBatch2D(const PxExtent2D& currentExtent)
    {
      PerformGarbageCollection();

      auto basicRenderSystem = GetBasicRenderSystem();
      auto quadBatch = CreateQuadBatch();
      return std::make_shared<NativeBatch2D>(basicRenderSystem, m_resources.NativeDevice, quadBatch, currentExtent);
    }

    std::shared_ptr<Graphics3D::INativeDevice> NativeGraphicsService::GetNativeDevice()
    {
      return m_resources.NativeDevice;
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
      const auto contentVertShader = QuadBatchShaders::GetVertexShader();
      const auto contentFragShader = QuadBatchShaders::GetFragmentShader();
      const auto contentSdfFragShader = QuadBatchShaders::GetSdfFragmentShader();
      std::vector<uint8_t> vertShaderBinary(contentVertShader.data(), contentVertShader.data() + contentVertShader.size());
      std::vector<uint8_t> fragShaderBinary(contentFragShader.data(), contentFragShader.data() + contentFragShader.size());
      std::vector<uint8_t> sdfFragShaderBinary(contentSdfFragShader.data(), contentSdfFragShader.data() + contentSdfFragShader.size());

      auto quadBatch = std::make_shared<QuadBatch>(vertShaderBinary, fragShaderBinary, sdfFragShaderBinary, GenericBatch2D_DEFAULT_CAPACITY);

      if (m_state >= State::DeviceInitialized)
      {
        quadBatch->CreateDeviceResources(m_resources.PhysicalDevice, m_resources.Device);
      }
      if (m_state >= State::DependentResourcesInitialized)
      {
        quadBatch->CreateDependentResources(m_resources.MaxFramesInFlight, m_dependentResources.RenderPass, m_dependentResources.Subpass,
                                            m_dependentResources.ScreenExtentPx);
      }

      m_quadBatches.push_back(quadBatch);
      return quadBatch;
    }

    void NativeGraphicsService::UpdateState(NativeGraphicsBasic2D& basic2D)
    {
      switch (m_state)
      {
      case State::Uninitialized:
        break;
      case State::Initialized:
        break;
      case State::DeviceInitialized:
      case State::DependentResourcesInitialized:
        basic2D.VulkanDeviceInit(m_resources.ImageCreator);
        break;
      default:
        throw NotSupportedException("Unknown state");
      }
    }

  }
}
