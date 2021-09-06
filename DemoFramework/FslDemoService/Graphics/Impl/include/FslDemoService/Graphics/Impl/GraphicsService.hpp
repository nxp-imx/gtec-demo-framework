#ifndef FSLDEMOSERVICE_GRAPHICS_IMPL_GRAPHICSSERVICE_HPP
#define FSLDEMOSERVICE_GRAPHICS_IMPL_GRAPHICSSERVICE_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <FslDemoService/Graphics/Control/IGraphicsServiceControl.hpp>
#include <FslDemoService/Graphics/Control/IGraphicsServiceHost.hpp>
#include <FslDemoService/Graphics/IGraphicsService.hpp>
#include <FslDemoService/Profiler/IProfilerService.hpp>
#include <FslDemoService/Profiler/ScopedProfilerCustomCounterHandle.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalService.hpp>
#include <memory>

namespace Fsl
{
  class GraphicsServiceOptionParser;
  class INativeGraphicsService;
  class INativeGraphicsServiceControl;
  class INativeGraphicsBasic2D;

  class GraphicsService final
    : public ThreadLocalService
    , public IGraphicsService
    , public IGraphicsServiceControl
    , public IGraphicsServiceHost
  {
    struct ApiResources
    {
      bool IsValid{false};
      std::shared_ptr<INativeGraphicsService> NativeService;
      std::shared_ptr<INativeGraphicsServiceControl> NativeServiceControl;
      std::shared_ptr<INativeGraphicsBasic2D> NativBasic2D;
      std::shared_ptr<IBasic2D> NativeBasic2D;
    };

    struct DeviceResources
    {
      bool IsValid{false};
      std::shared_ptr<INativeBatch2D> NativeBatch2D;

      DeviceResources() = default;
      explicit DeviceResources(const bool isValid)
        : IsValid(isValid)
      {
      }
    };

    struct DeviceDependentResources
    {
      bool IsValid{false};

      DeviceDependentResources() = default;
      explicit DeviceDependentResources(const bool isValid)
        : IsValid(isValid)
      {
      }
    };

    using NativeGraphicsServiceDeque = std::deque<std::shared_ptr<INativeGraphicsService>>;

    std::shared_ptr<IProfilerService> m_profilerService;
    ScopedProfilerCustomCounterHandle m_hProfilerBatchDrawCalls;
    ScopedProfilerCustomCounterHandle m_hProfilerBatchVertices;

    NativeGraphicsServiceDeque m_nativeGraphicsServices;

    DemoWindowMetrics m_windowMetrics;

    ApiResources m_apiResources;
    DeviceResources m_resources;
    DeviceDependentResources m_dependentResources;
    bool m_preallocateBasic2D{true};

  public:
    explicit GraphicsService(const ServiceProvider& serviceProvider, const std::shared_ptr<GraphicsServiceOptionParser>& optionParser);
    ~GraphicsService() final;

    void Update() final;

    // From IGraphicsService
    void Capture(Bitmap& rBitmap, const PixelFormat desiredPixelFormat) final;
    void Capture(Bitmap& rBitmap, const PixelFormat desiredPixelFormat, const Rectangle& srcRectangle) final;
    std::shared_ptr<IBasic2D> GetBasic2D() final;
    std::shared_ptr<INativeBatch2D> GetNativeBatch2D() final;
    std::shared_ptr<INativeGraphics> GetNativeGraphics() final;
    std::shared_ptr<IBasicRenderSystem> GetBasicRenderSystem() final;

    // From IGraphicsServiceControl
    void SetWindowMetrics(const DemoWindowMetrics& windowMetrics) final;
    void ClearActiveApi() final;
    void SetActiveApi(const DemoHostFeature& activeAPI) final;
    void PreUpdate() override;

    // From IGraphicsServiceHost
    void CreateDevice(const GraphicsDeviceCreateInfo& createInfo) final;
    void DestroyDevice() final;
    void CreateDependentResources(const GraphicsDependentCreateInfo& createInfo) final;
    void DestroyDependentResources() final;
    void BeginFrame(const GraphicsBeginFrameInfo& frameInfo) override;
    void EndFrame() override;
    void OnRenderSystemEvent(const BasicRenderSystemEvent theEvent) override;
  };
}

#endif
