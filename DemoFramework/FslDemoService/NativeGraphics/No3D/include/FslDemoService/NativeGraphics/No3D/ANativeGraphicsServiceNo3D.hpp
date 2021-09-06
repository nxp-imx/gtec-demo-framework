#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_NO3D_ANATIVEGRAPHICSSERVICENO3D_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_NO3D_ANATIVEGRAPHICSSERVICENO3D_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslDemoService/NativeGraphics/Base/INativeGraphicsService.hpp>
#include <FslDemoService/NativeGraphics/Base/INativeGraphicsServiceControl.hpp>
#include <FslGraphics/Render/Basic/BasicRenderSystemEvent.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalService.hpp>
#include <memory>

namespace Fsl
{
  class ANativeGraphicsServiceNo3D
    : public ThreadLocalService
    , public INativeGraphicsService
    , public INativeGraphicsServiceControl
  {
    enum class State
    {
      Uninitialized = 0,
      DeviceReady = 1,
      DependentResourcesReady = 2
    };

    State m_state{State::Uninitialized};

  public:
    explicit ANativeGraphicsServiceNo3D(const ServiceProvider& serviceProvider);
    ~ANativeGraphicsServiceNo3D() override;

    // INativeGraphicsService
    std::shared_ptr<INativeTexture2D> CreateTexture2D(const RawTexture& texture, const Texture2DFilterHint filterHint,
                                                      const TextureFlags textureFlags) final;
    std::shared_ptr<IDynamicNativeTexture2D> CreateDynamicTexture2D(const RawTexture& texture, const Texture2DFilterHint filterHint,
                                                                    const TextureFlags textureFlags) final;
    std::shared_ptr<INativeBatch2D> CreateNativeBatch2D(const PxExtent2D& extentPx) final;
    std::shared_ptr<IBasicRenderSystem> GetBasicRenderSystem() final;


    // INativeGraphicsServiceControl
    void CreateDevice(const NativeGraphicsDeviceCreateInfo& createInfo) override;
    void DestroyDevice() noexcept override;
    void CreateDependentResources(const BasicNativeDependentCreateInfo& createInfo) override;
    void DestroyDependentResources() override;
    void PreUpdate() override;
    void BeginFrame(const BasicNativeBeginFrameInfo& frameInfo) override;
    void EndFrame() override;
    void OnRenderSystemEvent(const BasicRenderSystemEvent theEvent) override;
  };
}
#endif
