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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxExtent2D.hpp>
#include <FslDemoService/NativeGraphics/Base/NativeGraphicsDeviceCreateInfo.hpp>
#include <FslDemoService/NativeGraphics/BasicRender/ANativeGraphicsService.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeBeginFrameInfo.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeDependentCreateInfo.hpp>
#include <FslGraphics3D/BasicRender/Adapter/INativeDevice.hpp>
#include <FslGraphics3D/BasicRender/BasicRenderSystem.hpp>
#include <cassert>
#include <utility>

namespace Fsl
{
  ANativeGraphicsService::ANativeGraphicsService(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)
  {
  }


  ANativeGraphicsService::~ANativeGraphicsService()
  {
    if (m_state != State::Uninitialized)
    {
      FSLLOG3_DEBUG_WARNING("ANativeGraphicsService device still initialized at shutdown");
      DestroyDevice();
    }
  }

  std::shared_ptr<INativeTexture2D> ANativeGraphicsService::CreateTexture2D(const RawTexture& texture, const Texture2DFilterHint filterHint,
                                                                            const TextureFlags textureFlags)
  {
    if (m_state < State::DeviceReady)
    {
      throw UsageErrorException("The internal state is incorrect");
    }
    assert(m_renderSystem);
    return m_renderSystem->CreateTexture2D(texture, filterHint, textureFlags);
  }


  std::shared_ptr<IDynamicNativeTexture2D>
    ANativeGraphicsService::CreateDynamicTexture2D(const RawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags)
  {
    if (m_state < State::DeviceReady)
    {
      throw UsageErrorException("The internal state is incorrect");
    }
    return m_renderSystem->CreateDynamicTexture2D(texture, filterHint, textureFlags);
  }


  std::shared_ptr<IBasicRenderSystem> ANativeGraphicsService::GetBasicRenderSystem()
  {
    if (m_state < State::DeviceReady)
    {
      throw UsageErrorException("The internal state is incorrect");
    }
    assert(m_renderSystem);
    return m_renderSystem;
  }


  void ANativeGraphicsService::CreateDevice(const NativeGraphicsDeviceCreateInfo& createInfo)
  {
    if (m_state != State::Uninitialized)
    {
      throw UsageErrorException("Invalid state");
    }
    FSLLOG3_VERBOSE("ANativeGraphicsService::CreateDevice");

    try
    {
      m_state = State::DeviceReady;

      Graphics3D::BasicRenderSystemCreateInfo renderCreateInfo(createInfo.MaxFramesInFlight, GetNativeDevice());
      m_renderSystem = std::make_shared<Graphics3D::BasicRenderSystem>(renderCreateInfo);
    }
    catch (std::exception& ex)
    {
      FSLLOG3_ERROR("initialization failed with: {}", ex.what());
      DestroyDevice();
      throw;
    }
  }


  void ANativeGraphicsService::DestroyDevice() noexcept
  {
    if (m_state == State::Uninitialized)
    {
      return;
    }
    FSLLOG3_VERBOSE("ANativeGraphicsService::DestroyDevice");

    if (m_state == State::DependentResourcesReady)
    {
      try
      {
        FSLLOG3_WARNING("Dependent resources still allocated on device shutdown, trying to free them");
        DestroyDependentResources();
      }
      catch (std::exception& ex)
      {
        FSLLOG3_ERROR("Exception that occurred during shutdown was ignored: {}", ex.what());
      }
    }
    if (m_renderSystem)
    {
      m_renderSystem->Dispose();
      m_renderSystem.reset();
    }
    m_state = State::Uninitialized;
  }


  void ANativeGraphicsService::CreateDependentResources(const BasicNativeDependentCreateInfo& createInfo)
  {
    if (m_state != State::DeviceReady)
    {
      throw UsageErrorException("incorrect state");
    }
    FSLLOG3_VERBOSE("ANativeGraphicsService::CreateDependentResources({})", createInfo.ExtentPx);

    assert(m_renderSystem);
    m_renderSystem->CreateDependentResources(createInfo);
    m_state = State::DependentResourcesReady;
  }

  void ANativeGraphicsService::DestroyDependentResources()
  {
    if (m_state != State::DependentResourcesReady)
    {
      FSLLOG3_DEBUG_WARNING("DoShutdownDependentResources: Called from invalid state, call ignored");
      return;
    }
    FSLLOG3_VERBOSE("ANativeGraphicsService::DestroyDependentResources");

    assert(m_renderSystem);
    m_renderSystem->DestroyDependentResources();

    m_state = State::DeviceReady;
  }

  void ANativeGraphicsService::PreUpdate()
  {
    if (m_state < State::DeviceReady)
    {
      return;
    }

    assert(m_renderSystem);
    m_renderSystem->PreUpdate();
  }

  void ANativeGraphicsService::BeginFrame(const BasicNativeBeginFrameInfo& frameInfo)
  {
    m_renderSystem->BeginFrame(frameInfo);
  }

  void ANativeGraphicsService::EndFrame()
  {
    m_renderSystem->EndFrame();
  }


  void ANativeGraphicsService::OnRenderSystemEvent(const BasicRenderSystemEvent theEvent)
  {
    FSLLOG3_VERBOSE("ANativeGraphicsService::OnRenderSystemEvent");
    if (m_state >= State::DeviceReady)
    {
      assert(m_renderSystem);
      m_renderSystem->OnRenderSystemEvent(theEvent);
    }
  }
}
