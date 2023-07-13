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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoService/Graphics/Control/GraphicsBeginFrameInfo.hpp>
#include <FslDemoService/Graphics/Control/GraphicsDependentCreateInfo.hpp>
#include <FslDemoService/Graphics/Control/GraphicsDeviceCreateInfo.hpp>
#include <FslDemoService/Graphics/Impl/Basic2D.hpp>
#include <FslDemoService/Graphics/Impl/GraphicsService.hpp>
#include <FslDemoService/Graphics/Impl/GraphicsServiceOptionParser.hpp>
#include <FslDemoService/NativeGraphics/Base/INativeGraphicsBasic2D.hpp>
#include <FslDemoService/NativeGraphics/Base/INativeGraphicsService.hpp>
#include <FslDemoService/NativeGraphics/Base/INativeGraphicsServiceControl.hpp>
#include <FslDemoService/NativeGraphics/Base/NativeGraphicsDeviceCreateInfo.hpp>
#include <FslDemoService/Profiler/DefaultProfilerColors.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslGraphics/Render/Adapter/INativeGraphics.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeBeginFrameInfo.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeDependentCreateInfo.hpp>
#include <algorithm>
#include <cassert>
#include <limits>

namespace Fsl
{
  GraphicsService::GraphicsService(const ServiceProvider& serviceProvider, const std::shared_ptr<GraphicsServiceOptionParser>& optionParser)
    : ThreadLocalService(serviceProvider)
  {
    if (optionParser->ProfileEnabled())
    {
      m_profilerService = serviceProvider.TryGet<IProfilerService>();
      FSLLOG3_INFO_IF(!m_profilerService, "Failed to acquire the profiler service, request ignored.");
    }
    if (m_profilerService)
    {
      m_hProfilerBatchDrawCalls.Reset(m_profilerService,
                                      m_profilerService->CreateCustomCounter("batches", 0, 200, DefaultProfilerColors::BatchDrawCalls));
      m_hProfilerBatchVertices.Reset(m_profilerService,
                                     m_profilerService->CreateCustomCounter("vertices", 0, 9000, DefaultProfilerColors::BatchVertices));
    }

    // Acquire all providers of the INativeGraphicsService interface
    serviceProvider.Get(m_nativeGraphicsServices);
  }


  GraphicsService::~GraphicsService()
  {
    if (m_resources.IsValid)
    {
      FSLLOG3_ERROR("Device is still valid, trying to destroy it.");
      DestroyDevice();
    }
    if (m_apiResources.IsValid)
    {
      FSLLOG3_ERROR("API still selected, trying to clear it");
      ClearActiveApi();
    }
  }


  void GraphicsService::Update()
  {
    ThreadLocalService::Update();
    if (m_profilerService)
    {
      int32_t batchDrawCount = 0;
      int32_t batchVertices = 0;
      if (m_resources.NativeBatch2D)
      {
        Batch2DStats stats = m_resources.NativeBatch2D->GetStats();
        batchDrawCount = UncheckedNumericCast<int32_t>(std::min(stats.Native.DrawCalls, 0x7FFFFFFFu));
        batchVertices = UncheckedNumericCast<int32_t>(std::min(stats.Native.Vertices, 0x7FFFFFFFu));
      }

      m_profilerService->Set(m_hProfilerBatchDrawCalls, batchDrawCount);
      m_profilerService->Set(m_hProfilerBatchVertices, batchVertices);
    }
  }


  void GraphicsService::Capture(Bitmap& rBitmap, const PixelFormat desiredPixelFormat)
  {
    if (!m_apiResources.NativeService)
    {
      throw UsageErrorException("Not linked to native service");
    }

    const Rectangle srcRectangle(0, 0, NumericCast<int32_t>(m_windowMetrics.ExtentPx.Width.Value),
                                 NumericCast<int32_t>(m_windowMetrics.ExtentPx.Height.Value));
    Capture(rBitmap, desiredPixelFormat, srcRectangle);
  }


  void GraphicsService::Capture(Bitmap& rBitmap, const PixelFormat desiredPixelFormat, const Rectangle& srcRectangle)
  {
    if (!m_apiResources.NativeService)
    {
      throw UsageErrorException("Not linked to native service");
    }

    m_apiResources.NativeService->Capture(rBitmap, srcRectangle);
    if (!rBitmap.IsValid())
    {
      FSLLOG3_WARNING("Capture failed");
      return;
    }

    if (desiredPixelFormat == rBitmap.GetPixelFormat())
    {
      return;
    }

    // FIX: use a conversion service instead
    BitmapUtil::Convert(rBitmap, desiredPixelFormat);
  }


  std::shared_ptr<IBasic2D> GraphicsService::GetBasic2D()
  {
    // The basic2d interface is allowed
    if (!m_apiResources.NativeService)
    {
      throw UsageErrorException("Not linked to native service");
    }

    if (m_apiResources.NativeBasic2D)
    {
      return m_apiResources.NativeBasic2D;
    }

    m_apiResources.NativBasic2D = m_apiResources.NativeService->CreateBasic2D(m_windowMetrics.ExtentPx);
    m_apiResources.NativeBasic2D = std::make_shared<Basic2D>(m_apiResources.NativBasic2D);

    if (!m_apiResources.NativeBasic2D)
    {
      throw NotSupportedException("Native service returned a null pointer for CreateBasic2D");
    }
    return m_apiResources.NativeBasic2D;
  }


  std::shared_ptr<INativeBatch2D> GraphicsService::GetNativeBatch2D()
  {
    if (!m_apiResources.NativeService)
    {
      throw UsageErrorException("Not linked to native service");
    }

    if (m_resources.NativeBatch2D)
    {
      return m_resources.NativeBatch2D;
    }

    m_resources.NativeBatch2D = m_apiResources.NativeService->CreateNativeBatch2D(m_windowMetrics.ExtentPx);
    if (!m_resources.NativeBatch2D)
    {
      throw NotSupportedException("Native service returned a null pointer for CreateNativeBatch2D");
    }
    return m_resources.NativeBatch2D;
  }


  std::shared_ptr<INativeGraphics> GraphicsService::GetNativeGraphics()
  {
    if (!m_apiResources.NativeService)
    {
      throw UsageErrorException("Not linked to native service");
    }

    return m_apiResources.NativeService;
  }


  std::shared_ptr<IBasicRenderSystem> GraphicsService::GetBasicRenderSystem()
  {
    if (!m_apiResources.NativeService)
    {
      throw UsageErrorException("Not linked to native service");
    }
    return m_apiResources.NativeService->GetBasicRenderSystem();
  }


  void GraphicsService::SetWindowMetrics(const DemoWindowMetrics& windowMetrics)
  {
    if (windowMetrics == m_windowMetrics)
    {
      return;
    }

    m_windowMetrics = windowMetrics;
    if (m_apiResources.NativBasic2D)
    {
      m_apiResources.NativBasic2D->SetScreenExtent(windowMetrics.ExtentPx);
    }
    else if (m_preallocateBasic2D)
    {
      GetBasic2D();
    }

    if (m_resources.NativeBatch2D)
    {
      m_resources.NativeBatch2D->SetScreenExtent(windowMetrics.ExtentPx);
    }
  }


  void GraphicsService::ClearActiveApi()
  {
    if (m_dependentResources.IsValid)
    {
      FSLLOG3_ERROR("DependentResources are still allocated, call order error.. Trying to DestroyDependentResources");
      DestroyDependentResources();
    }
    if (m_resources.IsValid)
    {
      FSLLOG3_ERROR("Resources are still allocated, call order error.. Trying to DestroyDevice");
      DestroyDevice();
    }

    m_apiResources = {};
  }


  void GraphicsService::SetActiveApi(const DemoHostFeature& activeAPI)
  {
    FSLLOG3_VERBOSE("GraphicsService::CreateDevice");
    if (m_apiResources.IsValid)
    {
      FSLLOG3_ERROR("SetActiveApi called from unexpected state, clearing old api");
      ClearActiveApi();
    }
    m_apiResources.IsValid = true;

    auto itr = m_nativeGraphicsServices.begin();
    while (itr != m_nativeGraphicsServices.end() && !m_apiResources.NativeService)
    {
      if ((*itr)->IsSupported(activeAPI))
      {
        m_apiResources.NativeService = *itr;
        m_apiResources.NativeServiceControl = std::dynamic_pointer_cast<INativeGraphicsServiceControl>(m_apiResources.NativeService);
      }
      ++itr;
    }

    FSLLOG3_WARNING_IF(!m_apiResources.NativeService, "Unsupported API for GraphicsService");
  }


  void GraphicsService::CreateDevice(const GraphicsDeviceCreateInfo& createInfo)
  {
    FSLLOG3_VERBOSE("GraphicsService::CreateDevice");
    if (!m_apiResources.IsValid)
    {
      throw UsageErrorException("SetActiveApi must be called before CreateDevice");
    }
    if (m_resources.IsValid)
    {
      FSLLOG3_ERROR("Old device is still valid, trying to destroy it");
      DestroyDevice();
    }

    if (m_apiResources.NativeServiceControl)
    {
      NativeGraphicsDeviceCreateInfo nativeCreateInfo(createInfo.MaxFramesInFlight, createInfo.pCustomCreateInfo);
      m_apiResources.NativeServiceControl->CreateDevice(nativeCreateInfo);
    }

    m_preallocateBasic2D = createInfo.PreallocateBasic2D;

    m_resources = DeviceResources(true);
  }


  void GraphicsService::DestroyDevice()
  {
    if (!m_resources.IsValid)
    {
      return;
    }

    FSLLOG3_VERBOSE("GraphicsService::DestroyDevice");
    if (m_dependentResources.IsValid)
    {
      FSLLOG3_ERROR("Dependent resources still allocated, trying to destory them");
      DestroyDependentResources();
    }
    assert(m_apiResources.IsValid);

    if (m_apiResources.NativeServiceControl)
    {
      m_apiResources.NativeServiceControl->DestroyDevice();
    }

    m_resources = {};
  }


  void GraphicsService::CreateDependentResources(const GraphicsDependentCreateInfo& createInfo)
  {
    FSLLOG3_VERBOSE("GraphicsService::CreateDependentResources");
    if (!m_resources.IsValid)
    {
      throw UsageErrorException("CreateDevice must be called");
    }
    if (m_dependentResources.IsValid)
    {
      FSLLOG3_ERROR("Dependent resources are still valid, trying to destroy them");
      DestroyDependentResources();
    }

    if (m_apiResources.NativeServiceControl)
    {
      BasicNativeDependentCreateInfo nativeCreateInfo(createInfo.ExtentPx, createInfo.pCustomCreateInfo);
      m_apiResources.NativeServiceControl->CreateDependentResources(nativeCreateInfo);
    }
    m_dependentResources = DeviceDependentResources(true);
  }


  void GraphicsService::DestroyDependentResources()
  {
    if (!m_dependentResources.IsValid)
    {
      return;
    }
    FSLLOG3_VERBOSE("GraphicsService::DestroyDependentResources");
    if (m_apiResources.NativeServiceControl)
    {
      m_apiResources.NativeServiceControl->DestroyDependentResources();
    }
    m_dependentResources = {};
  }

  void GraphicsService::PreUpdate()
  {
    if (m_apiResources.NativeServiceControl)
    {
      m_apiResources.NativeServiceControl->PreUpdate();
    }
  }

  void GraphicsService::BeginFrame(const GraphicsBeginFrameInfo& frameInfo)
  {
    if (m_apiResources.NativeServiceControl)
    {
      BasicNativeBeginFrameInfo nativeFrameInfo(frameInfo.FrameIndex, frameInfo.pCustomInfo);
      m_apiResources.NativeServiceControl->BeginFrame(nativeFrameInfo);
    }
  }

  void GraphicsService::EndFrame()
  {
    if (m_apiResources.NativeServiceControl)
    {
      m_apiResources.NativeServiceControl->EndFrame();
    }
  }

  void GraphicsService::OnRenderSystemEvent(const BasicRenderSystemEvent theEvent)
  {
    if (m_apiResources.NativeServiceControl)
    {
      m_apiResources.NativeServiceControl->OnRenderSystemEvent(theEvent);
    }
  }


}
