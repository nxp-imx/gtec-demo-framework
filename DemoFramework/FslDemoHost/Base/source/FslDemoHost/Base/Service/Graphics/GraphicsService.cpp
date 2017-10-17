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

#include <FslBase/Log/Log.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
#include <FslGraphics/Render/Adapter/INativeBatch2D.hpp>
#include <FslDemoApp/Base/Service/Graphics/Basic2D.hpp>
#include <FslDemoHost/Base/Service/Graphics/GraphicsService.hpp>
#include <FslDemoHost/Base/Service/NativeGraphics/INativeGraphicsService.hpp>
#include <FslDemoHost/Base/Service/NativeGraphics/INativeGraphicsBasic2D.hpp>
#include <FslGraphics/Render/Adapter/INativeGraphics.hpp>
#include <cassert>
#include <limits>
#include <sstream>

namespace Fsl
{
  GraphicsService::GraphicsService(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)
    , m_nativeGraphicsServices()
    , m_nativeService()
    , m_nativBasic2D()
    , m_basic2D()
    , m_nativeBatch2D()
    , m_resolution()
  {
    // Acquire all providers of the INativeGraphicsService interface
    serviceProvider.Get(m_nativeGraphicsServices);
  }


  GraphicsService::~GraphicsService()
  {
  }


  void GraphicsService::Capture(Bitmap& rBitmap, const PixelFormat desiredPixelFormat)
  {
    if (!m_nativeService)
      throw UsageErrorException("Not linked to native service");

    const Rectangle srcRectangle(0, 0, m_resolution.X, m_resolution.Y);
    Capture(rBitmap, desiredPixelFormat, srcRectangle);
  }


  void GraphicsService::Capture(Bitmap& rBitmap, const PixelFormat desiredPixelFormat, const Rectangle& srcRectangle)
  {
    if (!m_nativeService)
      throw UsageErrorException("Not linked to native service");

    m_nativeService->Capture(rBitmap, srcRectangle);

    if (desiredPixelFormat == rBitmap.GetPixelFormat())
      return;

    // FIX: use a conversion service instead
    BitmapUtil::Convert(rBitmap, desiredPixelFormat);
  }


  std::shared_ptr<Basic2D> GraphicsService::GetBasic2D()
  {
    if (!m_nativeService)
      throw UsageErrorException("Not linked to native service");

    if (m_basic2D)
      return m_basic2D;

    m_nativBasic2D = m_nativeService->CreateBasic2D(m_resolution);
    m_basic2D = std::make_shared<Basic2D>(m_nativBasic2D);
    if (!m_basic2D)
      throw NotSupportedException("Native service returned a null pointer for CreateBasic2D");
    return m_basic2D;
  }


  std::shared_ptr<INativeBatch2D> GraphicsService::GetNativeBatch2D()
  {
    if (!m_nativeService)
      throw UsageErrorException("Not linked to native service");

    if (m_nativeBatch2D)
      return m_nativeBatch2D;

    m_nativeBatch2D = m_nativeService->CreateNativeBatch2D(m_resolution);
    if (!m_nativeBatch2D)
      throw NotSupportedException("Native service returned a null pointer for CreateNativeBatch2D");
    return m_nativeBatch2D;
  }


  std::shared_ptr<INativeGraphics> GraphicsService::GetNativeGraphics()
  {
    if (!m_nativeService)
      throw UsageErrorException("Not linked to native service");

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
    while (itr != m_nativeGraphicsServices.end() && ! m_nativeService)
    {
      if ( (*itr)->IsSupported(activeAPI))
      {
        m_nativeService = *itr;
      }
      ++itr;
    }

    FSLLOG_WARNING_IF(!m_nativeService, "Unsupported API for GraphicsService");
  }


  void GraphicsService::SetScreenResolution(const Point2& resolution, const bool preallocateBasic2D)
  {
    m_resolution = resolution;
    if (m_nativBasic2D)
      m_nativBasic2D->SetScreenResolution(resolution);
    if (m_nativeBatch2D)
      m_nativeBatch2D->SetScreenResolution(resolution);
    if (preallocateBasic2D)
      GetBasic2D();
  }

}
