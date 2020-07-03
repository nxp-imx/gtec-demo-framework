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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslDemoService/Graphics/Impl/Basic2D.hpp>
#include <FslDemoService/Graphics/Impl/GraphicsService.hpp>
#include <FslDemoService/Graphics/Impl/GraphicsServiceOptionParser.hpp>
#include <FslDemoService/NativeGraphics/Base/INativeGraphicsService.hpp>
#include <FslDemoService/NativeGraphics/Base/INativeGraphicsBasic2D.hpp>
#include <FslDemoService/Profiler/DefaultProfilerColors.hpp>
#include <FslGraphics/Render/Adapter/INativeGraphics.hpp>
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


  GraphicsService::~GraphicsService() = default;


  void GraphicsService::Update()
  {
    ThreadLocalService::Update();
    if (m_profilerService)
    {
      uint32_t batchDrawCount = 0;
      uint32_t batchVertices = 0;
      if (m_nativeBatch2D)
      {
        Batch2DStats stats = m_nativeBatch2D->GetStats();
        batchDrawCount = stats.Native.DrawCalls;
        batchVertices = stats.Native.Vertices;
      }

      m_profilerService->Set(m_hProfilerBatchDrawCalls, batchDrawCount);
      m_profilerService->Set(m_hProfilerBatchVertices, batchVertices);
    }
  }


  void GraphicsService::Capture(Bitmap& rBitmap, const PixelFormat desiredPixelFormat)
  {
    if (!m_nativeService)
    {
      throw UsageErrorException("Not linked to native service");
    }

    const Rectangle srcRectangle(0, 0, m_windowMetrics.ExtentPx.Width, m_windowMetrics.ExtentPx.Height);
    Capture(rBitmap, desiredPixelFormat, srcRectangle);
  }


  void GraphicsService::Capture(Bitmap& rBitmap, const PixelFormat desiredPixelFormat, const Rectangle& srcRectangle)
  {
    if (!m_nativeService)
    {
      throw UsageErrorException("Not linked to native service");
    }

    m_nativeService->Capture(rBitmap, srcRectangle);
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
    if (!m_nativeService)
    {
      throw UsageErrorException("Not linked to native service");
    }

    if (m_basic2D)
    {
      return m_basic2D;
    }

    m_nativBasic2D = m_nativeService->CreateBasic2D(m_windowMetrics.ExtentPx);
    m_basic2D = std::make_shared<Basic2D>(m_nativBasic2D);
    if (!m_basic2D)
    {
      throw NotSupportedException("Native service returned a null pointer for CreateBasic2D");
    }
    return m_basic2D;
  }


  std::shared_ptr<INativeBatch2D> GraphicsService::GetNativeBatch2D()
  {
    if (!m_nativeService)
    {
      throw UsageErrorException("Not linked to native service");
    }

    if (m_nativeBatch2D)
    {
      return m_nativeBatch2D;
    }

    m_nativeBatch2D = m_nativeService->CreateNativeBatch2D(m_windowMetrics.ExtentPx);
    if (!m_nativeBatch2D)
    {
      throw NotSupportedException("Native service returned a null pointer for CreateNativeBatch2D");
    }
    return m_nativeBatch2D;
  }


  std::shared_ptr<INativeGraphics> GraphicsService::GetNativeGraphics()
  {
    if (!m_nativeService)
    {
      throw UsageErrorException("Not linked to native service");
    }

    return m_nativeService;
  }


  void GraphicsService::Reset()
  {
    m_nativeBatch2D.reset();
    m_basic2D.reset();
    m_nativBasic2D.reset();
    m_nativeService.reset();
  }


  void GraphicsService::Configure(const DemoHostFeature& activeAPI)
  {
    Reset();
    NativeGraphicsServiceDeque::const_iterator itr = m_nativeGraphicsServices.begin();
    while (itr != m_nativeGraphicsServices.end() && !m_nativeService)
    {
      if ((*itr)->IsSupported(activeAPI))
      {
        m_nativeService = *itr;
      }
      ++itr;
    }

    FSLLOG3_WARNING_IF(!m_nativeService, "Unsupported API for GraphicsService");
  }


  void GraphicsService::SetWindowMetrics(const DemoWindowMetrics& windowMetrics, const bool preallocateBasic2D)
  {
    m_windowMetrics = windowMetrics;
    if (m_nativBasic2D)
    {
      m_nativBasic2D->SetScreenExtent(windowMetrics.ExtentPx);
    }
    if (m_nativeBatch2D)
    {
      m_nativeBatch2D->SetScreenExtent(windowMetrics.ExtentPx);
    }
    if (preallocateBasic2D)
    {
      GetBasic2D();
    }
  }
}
