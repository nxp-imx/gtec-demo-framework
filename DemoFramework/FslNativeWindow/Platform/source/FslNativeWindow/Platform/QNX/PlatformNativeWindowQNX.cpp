#if defined(__QNXNTO__)
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

#include <FslNativeWindow/Platform/QNX/PlatformNativeWindowQNX.hpp>
#include <FslNativeWindow/Platform/QNX/PlatformNativeWindowSystemQNX.hpp>
#include <FslNativeWindow/Base/INativeWindowEventQueue.hpp>
#include <FslNativeWindow/Base/NativeWindowEventHelper.hpp>
#include <FslNativeWindow/Base/NativeWindowSetup.hpp>
#include <FslNativeWindow/Base/NativeWindowSystemSetup.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <iostream>

#if 0
#define LOCAL_LOG(X) FSLLOG3_INFO("PlatformNativeWindow QNX: {}", (X))
#else
#define LOCAL_LOG(X) \
  {                  \
  }
#endif


namespace Fsl
{
  namespace
  {
    std::weak_ptr<INativeWindowEventQueue> g_eventQueue;

    VirtualMouseButton::Enum MouseToVirtualMouse(unsigned int button)
    {
      return VirtualMouseButton::Left;
    }


    std::shared_ptr<INativeWindow> AllocateWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
                                                  const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    {
      return std::make_shared<PlatformNativeWindowQNX>(nativeWindowSetup, windowParams, pPlatformCustomWindowAllocationParams);
    }

  }    // namespace


  PlatformNativeWindowSystemQNX::PlatformNativeWindowSystemQNX(const NativeWindowSystemSetup& setup,
                                                               const PlatformNativeWindowAllocationFunction& allocateWindowFunction,
                                                               const PlatformNativeWindowSystemParams& systemParams)
    : PlatformNativeWindowSystem(setup, 0)
    , m_allocationFunction(allocateWindowFunction ? allocateWindowFunction : AllocateWindow)
  {
    LOCAL_LOG("QNX: PlatformNativeWindowSystemQNX start");

    auto eventQueue = setup.GetEventQueue().lock();
    if (eventQueue)
    {
      const NativeWindowEvent event = NativeWindowEventHelper::EncodeGamepadConfiguration(0);
      eventQueue->PostEvent(event);
    }

    LOCAL_LOG("QNX: PlatformNativeWindowSystemQNX end");
  }


  PlatformNativeWindowSystemQNX::~PlatformNativeWindowSystemQNX()
  {
    LOCAL_LOG("QNX: PlatformNativeWindowSystemQNX ");
  }


  std::shared_ptr<INativeWindow>
    PlatformNativeWindowSystemQNX::CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup,
                                                      const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
  {
    LOCAL_LOG("QNX: CreateNativeWindow start ");
    const auto window = m_allocationFunction(nativeWindowSetup, PlatformNativeWindowParams(m_platformDisplay), pPlatformCustomWindowAllocationParams);

    auto ptr = std::dynamic_pointer_cast<PlatformNativeWindowQNX>(window);
    if (!ptr)
      throw NotSupportedException("Allocation function did not allocate a PlatformNativeWindowQNX as required");


    LOCAL_LOG("QNX: CreateNativeWindow end");

    return window;
  }


  bool PlatformNativeWindowSystemQNX::ProcessMessages(const NativeWindowProcessMessagesArgs& args)
  {
    return true;
  }


  PlatformNativeWindowQNX::PlatformNativeWindowQNX(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& platformWindowParams,
                                                   const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    : PlatformNativeWindow(nativeWindowSetup, platformWindowParams, pPlatformCustomWindowAllocationParams)
  {
    LOCAL_LOG("QNX: PlatformNativeWindowSystemQNX start");

    // QNX Screen window initialization
    // int rc;
    int size[2];
    int position[2];
    const NativeWindowConfig nativeWindowConfig = nativeWindowSetup.GetConfig();

    g_eventQueue = nativeWindowSetup.GetEventQueue();

    int usage = SCREEN_USAGE_OPENGL_ES2;

    int format = SCREEN_FORMAT_RGBA8888;

    int x = screen_create_context(&screen_ctx, 0);
    if (x)
    {
      throw GraphicsException("screen_create_context failed");
    }

    if (screen_create_window(&m_platformWindow, screen_ctx))
    {
      throw GraphicsException("screen_create_window failed");
    }

    if (screen_set_window_property_iv(m_platformWindow, SCREEN_PROPERTY_USAGE, &usage))
    {
      throw GraphicsException("screen_set_window_property_iv SCREEN_PROPERTY_USAGE failed");
    }

    if (screen_set_window_property_iv(m_platformWindow, SCREEN_PROPERTY_FORMAT, &format))
    {
      throw GraphicsException("screen_set_window_property_iv SCREEN_PROPERTY_FORMAT failed");
    }

    if (SelectDisplay(screen_ctx, (screen_window_t)m_platformWindow, nativeWindowSetup.GetConfig().GetDisplayId()))
    {
      FSLLOG3_ERROR("SelectDisplay failed!");
    }

    if (screen_create_window_buffers(m_platformWindow, 2))
    {
      fprintf(stderr, "screen_create_window_buffers failed\n");
    }

    if (nativeWindowConfig.GetWindowMode() != WindowMode::Window)
    {
      FSLLOG3_INFO("Window Size/Position not defined, setting them to MAX Display Resolution");
      position[0] = 0;
      position[1] = 0;
      // default window size is fullscreen, so no screen_set_window_property_iv call is required
    }
    else
    {
      const Rectangle windowRectangle = nativeWindowConfig.GetWindowRectangle();
      size[0] = windowRectangle.Width();
      size[1] = windowRectangle.Height();
      position[0] = windowRectangle.X();
      position[1] = windowRectangle.Y();

      if (screen_set_window_property_iv(m_platformWindow, SCREEN_PROPERTY_SIZE, size))
      {
        fprintf(stderr, "SCREEN_PROPERTY_SIZE failed\n");
      }
    }


    if (screen_set_window_property_iv(m_platformWindow, SCREEN_PROPERTY_POSITION, position))
    {
      fprintf(stderr, "SCREEN_PROPERTY_POSITION failed\n");
    }


    /*
     // TODO: disable VSYNC for performance test
     int interval = 0;
     if (screen_set_window_property_iv(m_platformWindow, SCREEN_PROPERTY_SWAP_INTERVAL, &interval)) {
       fprintf(stderr, "SCREEN_PROPERTY_POSITION failed\n");
     }
     */

    // Post the activation message to let the framework know we are ready
    std::shared_ptr<INativeWindowEventQueue> eventQueue = g_eventQueue.lock();
    if (eventQueue)
      eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowActivationEvent(true));

    LOCAL_LOG("QNX: PlatformNativeWindowSystemQNX end");
  }


  PlatformNativeWindowQNX::~PlatformNativeWindowQNX()
  {
    LOCAL_LOG("QNX: ~PlatformNativeWindowQNX");
  }


  bool PlatformNativeWindowQNX::TryGetDPI(Vector2& rDPI) const
  {
    {    // Remove this once its implemented
      static bool warnedNotImplementedOnce = false;
      if (!warnedNotImplementedOnce)
      {
        FSLLOG3_INFO("PlatformNativeWindowQNX: TryGetDPI is not implemented on this backend.");
        warnedNotImplementedOnce = true;
      }
    }

    rDPI = Vector2();
    return false;
  }


  bool PlatformNativeWindowQNX::TryGetSize(Point2& rSize) const
  {
    {    // Remove this once its implemented
      static bool warnedNotImplementedOnce = false;
      if (!warnedNotImplementedOnce)
      {
        FSLLOG3_INFO("PlatformNativeWindowQNX: TryGetSize is not implemented on this backend.");
        warnedNotImplementedOnce = true;
      }
    }

    rSize = Point2();
    return false;
  }


  int PlatformNativeWindowQNX::SelectDisplay(screen_context_t ctx, screen_window_t win, int id)
  {
    screen_display_t* screen_disp;
    int screen_ndisplays;
    int i;

    // default display selected = don't search display ID
    if (id == 0x0)
    {
      return 0;
    }

    if (screen_get_context_property_iv(ctx, SCREEN_PROPERTY_DISPLAY_COUNT, &screen_ndisplays))
    {
      fprintf(stderr, "screen_get_context_property_iv(SCREEN_PROPERTY_DISPLAY_COUNT)");
      return -1;
    }


    screen_disp = (screen_display_t*)calloc(screen_ndisplays, sizeof(*screen_disp));
    if (screen_disp == NULL)
    {
      fprintf(stderr, "could not allocate memory for display list\n");
      return -1;
    }

    if (screen_get_context_property_pv(ctx, SCREEN_PROPERTY_DISPLAYS, (void**)screen_disp))
    {
      fprintf(stderr, "screen_get_context_property_ptr(SCREEN_PROPERTY_DISPLAYS)");
      free(screen_disp);
      return -1;
    }


    for (i = 0; i < screen_ndisplays; ++i)
    {
      int actual_id = 0;    // invalid
      (void)screen_get_display_property_iv(screen_disp[i], SCREEN_PROPERTY_ID, &actual_id);
      if (actual_id == id)
      {
        break;
      }
    }

    if (i >= screen_ndisplays)
    {
      fprintf(stderr, "couldn't find display %d\n", id);
      free(screen_disp);
      return -1;
    }

    if (screen_set_window_property_pv(win, SCREEN_PROPERTY_DISPLAY, (void**)&screen_disp[i]))
    {
      fprintf(stderr, "screen_set_window_property_ptr(SCREEN_PROPERTY_DISPLAY)");
      free(screen_disp);
      return -1;
    }

    free(screen_disp);

    return 0;
  }
}    // namespace Fsl
#endif
