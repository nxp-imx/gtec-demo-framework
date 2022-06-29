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
#include <FslDemoService/NativeGraphics/No3D/ANativeGraphicsServiceNo3D.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeBeginFrameInfo.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeDependentCreateInfo.hpp>
#include <cassert>
#include <utility>

namespace Fsl
{
  ANativeGraphicsServiceNo3D::ANativeGraphicsServiceNo3D(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)
  {
  }


  ANativeGraphicsServiceNo3D::~ANativeGraphicsServiceNo3D()
  {
    if (m_state != State::Uninitialized)
    {
      FSLLOG3_DEBUG_WARNING("ANativeGraphicsServiceNo3D device still initialized at shutdown");
      DestroyDevice();
    }
  }

  std::shared_ptr<INativeTexture2D> ANativeGraphicsServiceNo3D::CreateTexture2D(const RawTexture& /*texture*/,
                                                                                const Texture2DFilterHint /*filterHint*/,
                                                                                const TextureFlags /*textureFlags*/)
  {
    if (m_state < State::DeviceReady)
    {
      throw UsageErrorException("The internal state is incorrect");
    }
    throw NotSupportedException("CreateTexture2D");
  }


  std::shared_ptr<IDynamicNativeTexture2D> ANativeGraphicsServiceNo3D::CreateDynamicTexture2D(const RawTexture& /*texture*/,
                                                                                              const Texture2DFilterHint /*filterHint*/,
                                                                                              const TextureFlags /*textureFlags*/)
  {
    if (m_state < State::DeviceReady)
    {
      throw UsageErrorException("The internal state is incorrect");
    }
    throw NotSupportedException("CreateDynamicTexture2D");
  }

  std::shared_ptr<INativeBatch2D> ANativeGraphicsServiceNo3D::CreateNativeBatch2D(const PxExtent2D& /*extentPx*/)
  {
    if (m_state < State::DeviceReady)
    {
      throw UsageErrorException("The internal state is incorrect");
    }
    throw NotSupportedException("CreateNativeBatch2D");
  }

  std::shared_ptr<IBasicRenderSystem> ANativeGraphicsServiceNo3D::GetBasicRenderSystem()
  {
    if (m_state < State::DeviceReady)
    {
      throw UsageErrorException("The internal state is incorrect");
    }
    throw NotSupportedException("GetBasicRenderSystem");
  }


  void ANativeGraphicsServiceNo3D::CreateDevice(const NativeGraphicsDeviceCreateInfo& /*createInfo*/)
  {
    if (m_state != State::Uninitialized)
    {
      throw UsageErrorException("Invalid state");
    }
    FSLLOG3_VERBOSE("ANativeGraphicsServiceNo3D::CreateDevice");
    m_state = State::DeviceReady;
  }


  void ANativeGraphicsServiceNo3D::DestroyDevice() noexcept
  {
    if (m_state == State::Uninitialized)
    {
      return;
    }
    FSLLOG3_VERBOSE("ANativeGraphicsServiceNo3D::DestroyDevice");

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
    m_state = State::Uninitialized;
  }


  void ANativeGraphicsServiceNo3D::CreateDependentResources(const BasicNativeDependentCreateInfo& createInfo)
  {
    if (m_state != State::DeviceReady)
    {
      throw UsageErrorException("incorrect state");
    }
    FSLLOG3_VERBOSE("ANativeGraphicsServiceNo3D::CreateDependentResources({})", createInfo.ExtentPx);

    m_state = State::DependentResourcesReady;
  }

  void ANativeGraphicsServiceNo3D::DestroyDependentResources()
  {
    if (m_state != State::DependentResourcesReady)
    {
      FSLLOG3_DEBUG_WARNING("DoShutdownDependentResources: Called from invalid state, call ignored");
      return;
    }
    FSLLOG3_VERBOSE("ANativeGraphicsServiceNo3D::DestroyDependentResources");

    m_state = State::DeviceReady;
  }

  void ANativeGraphicsServiceNo3D::PreUpdate()
  {
  }

  void ANativeGraphicsServiceNo3D::BeginFrame(const BasicNativeBeginFrameInfo& /*frameInfo*/)
  {
  }

  void ANativeGraphicsServiceNo3D::EndFrame()
  {
  }

  void ANativeGraphicsServiceNo3D::OnRenderSystemEvent(const BasicRenderSystemEvent /*theEvent*/)
  {
    FSLLOG3_VERBOSE("ANativeGraphicsServiceNo3D::OnRenderSystemEvent");
  }
}
