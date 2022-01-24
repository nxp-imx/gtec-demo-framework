#if !defined(__ANDROID__) && defined(__linux__) && !defined(FSL_WINDOWSYSTEM_X11) && defined(FSL_WINDOWSYSTEM_FRAMEBUFFER) && \
  !defined(FSL_WINDOWSYSTEM_WAYLAND)
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

#include <FslNativeWindow/Platform/FB/PlatformNativeWindowSystemFB.hpp>
#include <FslNativeWindow/Platform/FB/PlatformNativeWindowFB.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/LogRectangle.hpp>
#include <FslNativeWindow/Base/INativeWindowEventQueue.hpp>
#include <FslNativeWindow/Base/NativeWindowEventHelper.hpp>
#include <FslNativeWindow/Base/NativeWindowSystemSetup.hpp>
#include <FslNativeWindow/Base/NativeWindowSetup.hpp>
#include <pthread.h>
#include <iostream>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <fstream>

namespace Fsl
{
  namespace
  {
    std::weak_ptr<INativeWindowEventQueue> g_eventQueue;
    volatile bool g_appFinished = false;
    volatile bool g_kbInitialized = false;
    int g_kbFd;

    bool InitKB()
    {
      int lSize;
      char baseBuffer[20] = "/dev/input/";

      // Grep the available input devices to look for a Keyboard
      int res = system("grep -E 'Handlers|EV=' /proc/bus/input/devices | grep -B1 'EV=120013' | grep -Eo 'event[0-9]+' > KBevent.txt");
      FSLLOG3_WARNING_IF(res != 0, "grep failed with {}", res);

      res = system("sync");
      FSLLOG3_WARNING_IF(res != 0, "sync failed with {}", res);

      std::ifstream fileStream("KBevent.txt", std::ios::in);
      if (!fileStream.good())
      {
        FSLLOG3_INFO("Keyboard Event File Could not be opened");
        return false;
      }
      fileStream.seekg(0, std::ifstream::end);
      const std::streamoff streamLength = fileStream.tellg();
      fileStream.seekg(0, std::ifstream::beg);
      lSize = static_cast<int>(streamLength);
      if (0 == lSize)
      {
        FSLLOG3_WARNING("No Keyboard Device could be found");
        return false;
      }


      // we only expect keyboard on event0 to event99
      if (lSize > 8)
      {
        FSLLOG3_WARNING("Not a valid Event File found");
        return false;
      }
      std::string kbString;
      getline(fileStream, kbString);
      strncat(baseBuffer, kbString.c_str(), lSize);


      g_kbFd = open(baseBuffer, O_RDONLY);
      int kbFlags = fcntl(g_kbFd, F_GETFL, 0);
      fcntl(g_kbFd, F_SETFL, kbFlags | O_NONBLOCK);

      if (g_kbFd == -1)
      {
        FSLLOG3_WARNING("Could Not Initialize Keyboard Device");
        return false;
      }
      return true;
    }

    bool CloseKB()
    {
      return -1 != close(g_kbFd);
    }

    VirtualKey::Enum KeyToVirtualKey(int key)
    {
      switch (key)
      {
      case KEY_ESC:
        return VirtualKey::Escape;
      case KEY_TAB:
        return VirtualKey::Tab;
      case KEY_ENTER:
        return VirtualKey::Return;
      case KEY_SPACE:
        return VirtualKey::Space;
      case KEY_BACKSPACE:
        return VirtualKey::Backspace;
      case KEY_A:
        return VirtualKey::A;
      case KEY_B:
        return VirtualKey::B;
      case KEY_C:
        return VirtualKey::C;
      case KEY_D:
        return VirtualKey::D;
      case KEY_E:
        return VirtualKey::E;
      case KEY_F:
        return VirtualKey::F;
      case KEY_G:
        return VirtualKey::G;
      case KEY_H:
        return VirtualKey::H;
      case KEY_I:
        return VirtualKey::I;
      case KEY_J:
        return VirtualKey::J;
      case KEY_K:
        return VirtualKey::K;
      case KEY_L:
        return VirtualKey::L;
      case KEY_M:
        return VirtualKey::M;
      case KEY_N:
        return VirtualKey::N;
      case KEY_O:
        return VirtualKey::O;
      case KEY_P:
        return VirtualKey::P;
      case KEY_Q:
        return VirtualKey::Q;
      case KEY_R:
        return VirtualKey::R;
      case KEY_S:
        return VirtualKey::S;
      case KEY_T:
        return VirtualKey::T;
      case KEY_U:
        return VirtualKey::U;
      case KEY_V:
        return VirtualKey::V;
      case KEY_W:
        return VirtualKey::W;
      case KEY_X:
        return VirtualKey::X;
      case KEY_Y:
        return VirtualKey::Y;
      case KEY_Z:
        return VirtualKey::Z;
      case KEY_0:
        return VirtualKey::Code0;
      case KEY_1:
        return VirtualKey::Code1;
      case KEY_2:
        return VirtualKey::Code2;
      case KEY_3:
        return VirtualKey::Code3;
      case KEY_4:
        return VirtualKey::Code4;
      case KEY_5:
        return VirtualKey::Code5;
      case KEY_6:
        return VirtualKey::Code6;
      case KEY_7:
        return VirtualKey::Code7;
      case KEY_8:
        return VirtualKey::Code8;
      case KEY_9:
        return VirtualKey::Code9;
      case KEY_F1:
        return VirtualKey::F1;
      case KEY_F2:
        return VirtualKey::F2;
      case KEY_F3:
        return VirtualKey::F3;
      case KEY_F4:
        return VirtualKey::F4;
      case KEY_F5:
        return VirtualKey::F5;
      case KEY_F6:
        return VirtualKey::F6;
      case KEY_F7:
        return VirtualKey::F7;
      case KEY_F8:
        return VirtualKey::F8;
      case KEY_F9:
        return VirtualKey::F9;
      case KEY_F10:
        return VirtualKey::F10;
      case KEY_F11:
        return VirtualKey::F11;
      case KEY_F12:
        return VirtualKey::F12;
      case KEY_UP:
        return VirtualKey::UpArrow;
      case KEY_DOWN:
        return VirtualKey::DownArrow;
      case KEY_LEFT:
        return VirtualKey::LeftArrow;
      case KEY_RIGHT:
        return VirtualKey::RightArrow;
      case KEY_INSERT:
        return VirtualKey::Insert;
      case KEY_DELETE:
        return VirtualKey::Delete;
      case KEY_HOME:
        return VirtualKey::Home;
      case KEY_END:
        return VirtualKey::End;
      case KEY_PAGEUP:
        return VirtualKey::PageUp;
      case KEY_PAGEDOWN:
        return VirtualKey::PageDown;
      case KEY_SCROLLLOCK:
        return VirtualKey::ScrollLock;
      case KEY_PAUSE:
        return VirtualKey::Pause;
      case KEY_BREAK:
        return VirtualKey::Break;
      default:
        /*Unhandled Key Pressed set a default option */
        return VirtualKey::Unknown;
      }
    }


    /* Run a thread get user input from Keyboard and send the message*/
    void* InputThread(void* param)
    {
      struct input_event ev;
      ssize_t n;

      while (!g_appFinished)
      {
        n = read(g_kbFd, &ev, sizeof ev);
        if (-1 != n)
        {
          // A Key has been pressed
          if (ev.type == EV_KEY && ev.value >= 0 && ev.value <= 1)
          {
            std::shared_ptr<INativeWindowEventQueue> eventQueue = g_eventQueue.lock();
            if (eventQueue)
            {
              // Decode The Key
              const VirtualKey::Enum keyCode = KeyToVirtualKey(static_cast<int>(ev.code));
              eventQueue->PostEvent(NativeWindowEventHelper::EncodeInputKeyEvent(keyCode, ev.value != 0));
            }
          }
        }
      }
      CloseKB();
      return nullptr;
    }

    std::shared_ptr<INativeWindow> AllocateWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
                                                  const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    {
      return std::make_shared<PlatformNativeWindowFB>(nativeWindowSetup, windowParams, pPlatformCustomWindowAllocationParams);
    }
  }    // namespace


  PlatformNativeWindowSystemFB::PlatformNativeWindowSystemFB(const NativeWindowSystemSetup& setup,
                                                             const PlatformNativeWindowAllocationFunction& allocateWindowFunction,
                                                             const PlatformNativeWindowSystemParams& systemParams)
    : PlatformNativeWindowSystem(setup, systemParams.Display)
    , m_allocationFunction(allocateWindowFunction ? allocateWindowFunction : AllocateWindow)
    , m_destroyDisplayFunc(systemParams.DestroyDisplayFunc)
  {
    FSLLOG3_DEBUG_WARNING_IF(!m_platformDisplay, "No display provided");

    auto eventQueue = setup.GetEventQueue().lock();
    if (eventQueue)
    {
      const NativeWindowEvent event = NativeWindowEventHelper::EncodeGamepadConfiguration(0);
      eventQueue->PostEvent(event);
    }
  }


  PlatformNativeWindowSystemFB::~PlatformNativeWindowSystemFB()
  {
    if ((m_platformDisplay != nullptr) && m_destroyDisplayFunc)
    {
      m_destroyDisplayFunc(m_platformDisplay);
      m_platformDisplay = nullptr;
    }

    g_appFinished = true;
    if (g_kbInitialized)
    {
      CloseKB();
    }
  }


  std::shared_ptr<INativeWindow>
    PlatformNativeWindowSystemFB::CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup,
                                                     const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
  {
    return m_allocationFunction(nativeWindowSetup, PlatformNativeWindowParams(m_platformDisplay), pPlatformCustomWindowAllocationParams);
  }


  bool PlatformNativeWindowSystemFB::ProcessMessages(const NativeWindowProcessMessagesArgs& args)
  {
    bool bContinue = true;
    return bContinue;
  }


  PlatformNativeWindowFB::PlatformNativeWindowFB(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& platformWindowParams,
                                                 const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    : PlatformNativeWindow(nativeWindowSetup, platformWindowParams, pPlatformCustomWindowAllocationParams, NativeWindowCapabilityFlags::NoFlags)
    , m_destroyWindowFunc(platformWindowParams.DestroyWindowFunc)
  {
    m_platformWindow = (platformWindowParams.PlatformWindow);
    FSLLOG3_DEBUG_WARNING_IF(!m_platformWindow, "No window available");

    g_eventQueue = nativeWindowSetup.GetEventQueue();

    if (!InitKB())
    {
      FSLLOG3_WARNING("Failed to initialize keyboard.");
    }
    else
    {
      g_kbInitialized = true;
      pthread_t pid;
      pthread_create(&pid, nullptr, &InputThread, nullptr);
    }

    {    // Post the activation message to let the framework know we are ready
      std::shared_ptr<INativeWindowEventQueue> eventQueue = g_eventQueue.lock();
      if (eventQueue)
      {
        eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowActivationEvent(true));
      }
    }
  }


  PlatformNativeWindowFB::~PlatformNativeWindowFB()
  {
    if (m_platformWindow != nullptr && m_destroyWindowFunc)
    {
      m_destroyWindowFunc(m_platformWindow);
      m_platformWindow = nullptr;
    }

    g_appFinished = true;
    if (g_kbInitialized)
    {
      CloseKB();
    }
  }


  bool PlatformNativeWindowFB::TryGetNativeSize(PxPoint2& rSize) const
  {
    {    // Remove this once its implemented
      static bool warnedNotImplementedOnce = false;
      if (!warnedNotImplementedOnce)
      {
        FSLLOG3_INFO("PlatformNativeWindowFB: TryGetNativeSize is not implemented on this backend.");
        warnedNotImplementedOnce = true;
      }
    }

    rSize = PxPoint2();
    return false;
  }
}    // namespace Fsl
#endif
