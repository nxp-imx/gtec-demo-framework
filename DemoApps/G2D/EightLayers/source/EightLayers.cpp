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

// The interaction with FB was copied from the existing G2D overlay example

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <cstring>
#include "EightLayers.hpp"
#include "OptionParser.hpp"
#include "ScopedFBHandle.hpp"
#include "G2DSurface.hpp"

namespace Fsl
{
  struct ScopedG2DHandle
  {
    void* Handle;

    ScopedG2DHandle()
      : Handle(nullptr)
    {
      if (g2d_open(&Handle) == -1 || Handle == nullptr)
      {
        if (Handle != nullptr)
          g2d_close(Handle);
        return;
      }
    }

    ~ScopedG2DHandle()
    {
      FSLLOG3_INFO("Closing G2D");
      g2d_close(Handle);
      FSLLOG3_INFO("Closing G2D");
    }
  };


  namespace
  {
    g2d_surface PrepareDstSurface(const std::shared_ptr<ScopedFBHandle> fb)
    {
      g2d_surface surface;
      std::memset(&surface, 0, sizeof(g2d_surface));
      surface.planes[0] = fb->PhysicalMemStart;
      surface.left = 0;
      surface.top = 0;
      surface.right = fb->ScreenInfo.xres;
      surface.bottom = fb->ScreenInfo.yres;
      surface.stride = fb->ScreenInfo.xres;    // Is this really not in bytes?
      surface.width = fb->ScreenInfo.xres;
      surface.height = fb->ScreenInfo.yres;
      surface.rot = G2D_ROTATION_0;
      surface.format = fb->ScreenInfo.bits_per_pixel == 16 ? G2D_RGB565 : (fb->ScreenInfo.red.offset == 0 ? G2D_RGBA8888 : G2D_BGRA8888);
      surface.blendfunc = G2D_ONE_MINUS_SRC_ALPHA;
      surface.global_alpha = 0xff;
      return surface;
    }
  }


  EightLayers::EightLayers(const DemoAppConfig& config)
    : DemoAppG2D(config)
    , m_layerCount(8)
    , m_fb(new ScopedFBHandle(config.GetOptions<OptionParser>()->Use32BPP(), config.GetOptions<OptionParser>()->GetAlignFrameBuffer()))
    , m_g2d(new ScopedG2DHandle())
    , m_srcSurfaces(m_layerCount)
    , m_layers(m_layerCount)
    , m_layerPointers(m_layerCount)
    , m_useDummyMode(config.GetOptions<OptionParser>()->UseDummyMode())
    , m_forceVSync(config.GetOptions<OptionParser>()->IsForceVSyncEnabled())
    , m_cache(config.GetOptions<OptionParser>()->IsCacheEnabled())
    , m_animate(config.GetOptions<OptionParser>()->UseAnimation())
    , m_use8Blit(!config.GetOptions<OptionParser>()->UseManual())
    , m_test(config.GetOptions<OptionParser>()->GetTestValue())
    , m_test2(config.GetOptions<OptionParser>()->GetTest2Value())
    , m_angle(0)
  {
#ifdef OLD_BOARD
    m_use8Blit = false;
#endif
    const bool alignFB = config.GetOptions<OptionParser>()->GetAlignFrameBuffer();
    const bool useOffscreen = config.GetOptions<OptionParser>()->UseOffscreen();
    FSLLOG3_INFO("UseDummyMode: {}", m_useDummyMode);
    FSLLOG3_INFO("ForceVSync: {}", m_forceVSync);
    FSLLOG3_INFO("Cache: {}", m_cache);
    FSLLOG3_INFO("EightBlit: {}", m_use8Blit);
    FSLLOG3_INFO("Offscreen: {}", useOffscreen);
    FSLLOG3_INFO("AlignFB: {}", alignFB);
    FSLLOG3_INFO("Test: {}", m_test);
    FSLLOG3_INFO("Test2: {}", m_test2);

    if (m_useDummyMode)
    {
      GetDemoAppControl()->RequestExit();
      return;
    }


    const std::shared_ptr<IContentManager> contentManager = GetContentManager();

    if (useOffscreen)
    {
      g2d_format g2dFormat = (g2d_format)config.GetOptions<OptionParser>()->GetOffscreenFormat();
      if (int(g2dFormat) < 0)
      {
        g2d_surface dstSurface = PrepareDstSurface(m_fb);
        g2dFormat = dstSurface.format;
      }
      FSLLOG3_INFO("*** Using offscreen, format: {}", int(g2dFormat));
      m_tmp.reset(new G2DSurface(m_fb->ScreenInfo.width, m_fb->ScreenInfo.height, g2dFormat, "offscreen", m_cache));
    }
    else
    {
      FSLLOG3_INFO("*** Using FB");
    }

    {
      int dstIndex = 0;
      Bitmap bitmap;
      // jpg
      std::string firstLayer = m_use8Blit ? "Textures/EightLayers/1_new.jpg" : "Textures/EightLayers/1_old.jpg";

      contentManager->Read(bitmap, firstLayer, PixelFormat::R8G8B8A8_UNORM);
      m_srcSurfaces[dstIndex].reset(new G2DSurface(bitmap, false, "1", m_cache));
      ++dstIndex;
      // png
      FSLLOG3_INFO("Loading 2");
      contentManager->Read(bitmap, "Textures/EightLayers/2.png", PixelFormat::R8G8B8A8_UNORM);
      FSLLOG3_INFO("Prepping 2");
      m_srcSurfaces[dstIndex].reset(new G2DSurface(bitmap, false, "2", m_cache));
      ++dstIndex;
      FSLLOG3_INFO("Loading 3");
      contentManager->Read(bitmap, "Textures/EightLayers/3.png", PixelFormat::R8G8B8A8_UNORM);
      FSLLOG3_INFO("Prepping 3");
      m_srcSurfaces[dstIndex].reset(new G2DSurface(bitmap, false, "3", m_cache));
      ++dstIndex;
      FSLLOG3_INFO("Loading 4");
      contentManager->Read(bitmap, "Textures/EightLayers/4.png", PixelFormat::R8G8B8A8_UNORM);
      FSLLOG3_INFO("Prepping 4");
      m_srcSurfaces[dstIndex].reset(new G2DSurface(bitmap, false, "4", m_cache));
      ++dstIndex;
      FSLLOG3_INFO("Loading 5");
      contentManager->Read(bitmap, "Textures/EightLayers/5.png", PixelFormat::R8G8B8A8_UNORM);
      FSLLOG3_INFO("Prepping 5");
      m_srcSurfaces[dstIndex].reset(new G2DSurface(bitmap, false, "5", m_cache));
      ++dstIndex;
      FSLLOG3_INFO("Loading 6");
      contentManager->Read(bitmap, "Textures/EightLayers/6.png", PixelFormat::R8G8B8A8_UNORM);
      FSLLOG3_INFO("Prepping 6");
      m_srcSurfaces[dstIndex].reset(new G2DSurface(bitmap, false, "6", m_cache));
      ++dstIndex;
      FSLLOG3_INFO("Loading 7");
      contentManager->Read(bitmap, "Textures/EightLayers/7.png", PixelFormat::R8G8B8A8_UNORM);
      FSLLOG3_INFO("Prepping 7");
      m_srcSurfaces[dstIndex].reset(new G2DSurface(bitmap, false, "7", m_cache));
      ++dstIndex;
      FSLLOG3_INFO("Loading 8");
      contentManager->Read(bitmap, "Textures/EightLayers/8.png", PixelFormat::R8G8B8A8_UNORM);
      FSLLOG3_INFO("Prepping 8");
      m_srcSurfaces[dstIndex].reset(new G2DSurface(bitmap, false, "8", m_cache));
      ++dstIndex;
    }
    UpdateInfo();
  }


  EightLayers::~EightLayers()
  {
  }


  void EightLayers::Update(const DemoTime& demoTime)
  {
    if (m_useDummyMode)
      return;

    m_srcRect = Rectangle(0, 0, m_fb->ScreenInfo.xres, m_fb->ScreenInfo.yres);

    if (m_animate)
    {
      float pos = (std::sin(m_angle) + 1.0f);
      float xRangeFirst = (1200.0f / 2.0f);
      float xRangeLast = (1920.0f / 2.0f);

      float xRangeAdd = (xRangeLast - xRangeFirst) / (m_layerCount - 1);
      float xRange = xRangeFirst;
      for (std::size_t i = 0; i < m_layers.size(); ++i)
      {
        int offset = int(pos * xRange);
#ifndef OLD_BOARD
        // Modify the source address as a workaround for the srcRect being buggy.
        m_layers[i].s.planes[0] = m_srcSurfaces[i]->m_surface.planes[0] + (offset * 4);
        m_layers[i].s.left = m_srcRect.Left();
        m_layers[i].s.right = m_srcRect.Right();
#else
        m_layers[i].s.left = offset + m_srcRect.Left();
        m_layers[i].s.right = offset + m_srcRect.Right();
#endif
        m_layers[i].s.top = m_srcRect.Top();
        m_layers[i].s.bottom = m_srcRect.Bottom();
        xRange += xRangeAdd;
      }

      m_angle += 0.4f * demoTime.DeltaTime;
      if (m_angle > (MathHelper::TO_RADS * 360.0f))
        m_angle -= (MathHelper::TO_RADS * 360.0f);
    }
    else
    {
      for (std::size_t i = 0; i < m_layers.size(); ++i)
      {
        m_layers[i].s.left = m_srcRect.Left();
        m_layers[i].s.right = m_srcRect.Right();
        m_layers[i].s.top = m_srcRect.Top();
        m_layers[i].s.bottom = m_srcRect.Bottom();
      }
    }
  }


  void EightLayers::Draw(const FrameInfo& frameInfo)
  {
    if (m_useDummyMode)
      return;

    void* hG2D = m_g2d->Handle;

    g2d_disable(hG2D, G2D_GLOBAL_ALPHA);
    g2d_disable(hG2D, G2D_BLEND);

#ifndef OLD_BOARD
    if (m_use8Blit)
    {
      g2d_enable(hG2D, G2D_BLEND);
      g2d_multi_blit(hG2D, m_layerPointers.data(), static_cast<int>(m_layers.size()));
    }
    else
#endif
    {
      g2d_blit(hG2D, &m_layers[0].s, &m_layers[0].d);

      g2d_enable(hG2D, G2D_BLEND);
      for (std::size_t i = 1; i < m_layers.size(); ++i)
        g2d_blit(hG2D, &m_layers[i].s, &m_layers[0].d);
    }

    g2d_disable(hG2D, G2D_BLEND);
    g2d_finish(hG2D);

    m_fb->SwapBuffers(m_forceVSync, m_test);
    UpdateInfo();
  }


  void EightLayers::UpdateInfo()
  {
    const g2d_surface dstSurface = (!m_tmp ? PrepareDstSurface(m_fb) : m_tmp->m_surface);
    for (std::size_t i = 0; i < m_layers.size(); ++i)
    {
      m_layers[i].d = dstSurface;
      m_layers[i].s = m_srcSurfaces[i]->m_surface;
      m_layerPointers[i] = &m_layers[i];
    }
  }
}
