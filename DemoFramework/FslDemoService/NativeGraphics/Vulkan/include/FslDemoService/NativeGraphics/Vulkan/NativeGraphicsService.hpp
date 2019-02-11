#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_VULKAN_NATIVEGRAPHICSSERVICE_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_VULKAN_NATIVEGRAPHICSSERVICE_HPP
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

#include <FslBase/Math/Extent2D.hpp>
#include <FslDemoService/NativeGraphics/Base/INativeGraphicsService.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalService.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <FslUtil/Vulkan1_0/VUPhysicalDeviceRecord.hpp>
#include <memory>
#include <list>

namespace Fsl
{
  namespace Vulkan
  {
    class NativeGraphicsBasic2D;
    class QuadBatch;
    class VulkanImageCreator;

    class NativeGraphicsService
      : public ThreadLocalService
      , public INativeGraphicsService
    {
      enum class State
      {
        Uninitialized = 0x00,
        Initialized = 0x01,
        DeviceInitialized = 0x02,
        DependentResourcesInitialized = 0x03,
      };
      struct Resources
      {
        VkDevice Device = VK_NULL_HANDLE;
        VUPhysicalDeviceRecord PhysicalDevice;
        std::shared_ptr<VulkanImageCreator> ImageCreator;
      };
      struct DependentResources
      {
        uint32_t CommandBufferCount = 0;
        VkRenderPass RenderPass = VK_NULL_HANDLE;
        uint32_t Subpass = 0;
        Extent2D ScreenResolution;
      };

      State m_state = State::Uninitialized;
      std::weak_ptr<NativeGraphicsBasic2D> m_basic2D;
      std::list<std::weak_ptr<QuadBatch>> m_quadBatches;

      Resources m_resources;
      DependentResources m_dependentResources;

    public:
      NativeGraphicsService(const ServiceProvider& serviceProvider);
      ~NativeGraphicsService() override;

      void VulkanInit();
      void VulkanShutdown() noexcept;
      void VulkanDeviceInit(const VUDevice& device, const VkQueue queue, const uint32_t queueFamilyIndex);
      void VulkanDeviceShutdown() noexcept;

      void VulkanCreateDependentResources(const uint32_t commandBufferCount, const VkRenderPass renderPass, const uint32_t subpass,
                                          const Extent2D& screenResolution);
      void VulkanDestroyDependentResources() noexcept;

      void VulkanBeginFrame(const VkCommandBuffer commandBuffer, const uint32_t commandBufferIndex);
      void VulkanEndFrame();

      // From INativeGraphics
      std::shared_ptr<INativeTexture2D> CreateTexture2D(const RawBitmap& bitmap, const Texture2DFilterHint filterHint,
                                                        const TextureFlags& textureFlags) override;
      std::shared_ptr<INativeTexture2D> CreateTexture2D(const RawTexture& texture, const Texture2DFilterHint filterHint,
                                                        const TextureFlags& textureFlags) override;
      // From INativeGraphicsService
      bool IsSupported(const DemoHostFeature& activeAPI) const override;
      void Capture(Bitmap& rBitmap, const Rectangle& srcRectangle) override;
      std::shared_ptr<INativeGraphicsBasic2D> CreateBasic2D(const Point2& currentResolution) override;
      std::shared_ptr<INativeBatch2D> CreateNativeBatch2D(const Point2& currentResolution) override;

    private:
      void PerformGarbageCollection() noexcept;
      std::shared_ptr<QuadBatch> CreateQuadBatch();
    };
  }
}

#endif
