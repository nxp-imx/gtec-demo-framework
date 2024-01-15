#if !defined(__ANDROID__) && defined(__linux__) && defined(FSL_WINDOWSYSTEM_X11)
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
#include <FslBase/Log/Math/FmtPoint2.hpp>
#include <FslBase/Log/Math/FmtRectangle.hpp>
#include <FslBase/Log/Math/Pixel/FmtPxPoint2.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslNativeWindow/Base/INativeWindowEventQueue.hpp>
#include <FslNativeWindow/Base/NativeWindowEventHelper.hpp>
#include <FslNativeWindow/Base/NativeWindowSetup.hpp>
#include <FslNativeWindow/Base/NativeWindowSystemSetup.hpp>
#include <FslNativeWindow/Platform/Adapter/X11/PlatformNativeWindowAdapterX11.hpp>
#include <FslNativeWindow/Platform/Adapter/X11/PlatformNativeWindowSystemAdapterX11.hpp>
// #include <X11/extensions/XInput2.h>
#include <X11/extensions/Xrandr.h>
#include <X11/keysym.h>
#include <cassert>
#include <cmath>
#include <iostream>


// This file is a good reference for how X11 works https://cgit.freedesktop.org/xorg/app/xev/tree/xev.c
// This code is not a reproduction of the code at the link, but only here to give others a
// good reference source.

namespace Fsl
{
  namespace
  {
    const int32_t MAGIC_DEFAULT_DPI = 96;

    std::weak_ptr<INativeWindowEventQueue> g_eventQueue;
    int WaitForMap(Display* /*display*/, XEvent* event, XPointer arg)    // NOLINT(readability-non-const-parameter)
    {
      return static_cast<int>(event->type == MapNotify && event->xmap.window == *reinterpret_cast<Window*>(arg));
    }

    VirtualMouseButton::Enum MouseToVirtualMouse(unsigned int button)
    {
      switch (button)
      {
      case Button1:
        return VirtualMouseButton::Left;
      case Button2:
        return VirtualMouseButton::Middle;
      case Button3:
        return VirtualMouseButton::Right;
      default:
        return VirtualMouseButton::Left;
      }
    }


    VirtualKey::Enum KeyToVirtualKey(XKeyEvent* kEvent)
    {
      KeySym keyId = XLookupKeysym(kEvent, 0);

      switch (keyId)
      {
      case XK_A:
      case XK_a:
        return VirtualKey::A;
      case XK_B:
      case XK_b:
        return VirtualKey::B;
      case XK_C:
      case XK_c:
        return VirtualKey::C;
      case XK_D:
      case XK_d:
        return VirtualKey::D;
      case XK_E:
      case XK_e:
        return VirtualKey::E;
      case XK_F:
      case XK_f:
        return VirtualKey::F;
      case XK_G:
      case XK_g:
        return VirtualKey::G;
      case XK_H:
      case XK_h:
        return VirtualKey::H;
      case XK_I:
      case XK_i:
        return VirtualKey::I;
      case XK_J:
      case XK_j:
        return VirtualKey::J;
      case XK_K:
      case XK_k:
        return VirtualKey::K;
      case XK_L:
      case XK_l:
        return VirtualKey::L;
      case XK_M:
      case XK_m:
        return VirtualKey::M;
      case XK_N:
      case XK_n:
        return VirtualKey::N;
      case XK_O:
      case XK_o:
        return VirtualKey::O;
      case XK_P:
      case XK_p:
        return VirtualKey::P;
      case XK_Q:
      case XK_q:
        return VirtualKey::Q;
      case XK_R:
      case XK_r:
        return VirtualKey::R;
      case XK_S:
      case XK_s:
        return VirtualKey::S;
      case XK_T:
      case XK_t:
        return VirtualKey::T;
      case XK_U:
      case XK_u:
        return VirtualKey::U;
      case XK_V:
      case XK_v:
        return VirtualKey::V;
      case XK_W:
      case XK_w:
        return VirtualKey::W;
      case XK_X:
      case XK_x:
        return VirtualKey::X;
      case XK_Y:
      case XK_y:
        return VirtualKey::Y;
      case XK_Z:
      case XK_z:
        return VirtualKey::Z;
      case XK_0:
        return VirtualKey::Code0;
      case XK_1:
        return VirtualKey::Code1;
      case XK_2:
        return VirtualKey::Code2;
      case XK_3:
        return VirtualKey::Code3;
      case XK_4:
        return VirtualKey::Code4;
      case XK_5:
        return VirtualKey::Code5;
      case XK_6:
        return VirtualKey::Code6;
      case XK_7:
        return VirtualKey::Code7;
      case XK_8:
        return VirtualKey::Code8;
      case XK_9:
        return VirtualKey::Code9;
      case XK_Escape:
        return VirtualKey::Escape;
      case XK_Tab:
        return VirtualKey::Tab;
      case XK_Return:
        return VirtualKey::Return;
      case XK_space:
        return VirtualKey::Space;
      case XK_BackSpace:
        return VirtualKey::Backspace;
      case XK_Up:
        return VirtualKey::UpArrow;
      case XK_Down:
        return VirtualKey::DownArrow;
      case XK_Left:
        return VirtualKey::LeftArrow;
      case XK_Right:
        return VirtualKey::RightArrow;
      case XK_F1:
        return VirtualKey::F1;
      case XK_F2:
        return VirtualKey::F2;
      case XK_F3:
        return VirtualKey::F3;
      case XK_F4:
        return VirtualKey::F4;
      case XK_F5:
        return VirtualKey::F5;
      case XK_F6:
        return VirtualKey::F6;
      case XK_F7:
        return VirtualKey::F7;
      case XK_F8:
        return VirtualKey::F8;
      case XK_F9:
        return VirtualKey::F9;
      case XK_F10:
        return VirtualKey::F10;
      case XK_F11:
        return VirtualKey::F11;
      case XK_F12:
        return VirtualKey::F12;
      case XK_Insert:
        return VirtualKey::Insert;
      case XK_Delete:
        return VirtualKey::Delete;
      case XK_Home:
        return VirtualKey::Home;
      case XK_End:
        return VirtualKey::End;
      case XK_Page_Up:
        return VirtualKey::PageUp;
      case XK_Page_Down:
        return VirtualKey::PageDown;
      case XK_Print:
        return VirtualKey::PrintScreen;
      case XK_Pause:
        return VirtualKey::ScrollLock;
      case XK_Scroll_Lock:
        return VirtualKey::ScrollLock;
      default:
        return VirtualKey::Tab;
      }
    }


    //! @note dont call this very often, its probably slow as hell
    bool TryLookupDisplayScreenSize(PlatformNativeDisplayType platformDisplay, PlatformNativeWindowType platformWindow, XRRScreenSize& rScreenSize)
    {    // Cache some window info
      bool found = false;
      XRRScreenConfiguration* pScreenInfo = XRRGetScreenInfo(platformDisplay, platformWindow);
      {
        Rotation currentRotation{};
        const auto currentSize = XRRConfigCurrentConfiguration(pScreenInfo, &currentRotation);
        int sizeCount = 0;
        XRRScreenSize* pSizes = XRRConfigSizes(pScreenInfo, &sizeCount);
        if (currentSize <= sizeCount)
        {
          found = true;
          rScreenSize = pSizes[currentSize];
        }
        else
        {
          rScreenSize = XRRScreenSize{};
          found = false;
        }
      }
      XRRFreeScreenConfigInfo(pScreenInfo);
      return found;
    }


    int32_t CalcDPI(const int32_t width, const int32_t millimeterWidth)
    {
      assert(width > 0);
      assert(millimeterWidth > 0);
      // 1mm = 0.0393701f inches
      const auto w = static_cast<double>(width);
      const double inchesWidth = static_cast<double>(millimeterWidth) * 0.0393701;
      return static_cast<int32_t>(std::round(w / inchesWidth));
    }


    void TryUpdateDPI(PlatformNativeDisplayType platformDisplay, PlatformNativeWindowType platformWindow, Point2& rScreenDPI)
    {
      XRRScreenSize screenSize{};
      if (TryLookupDisplayScreenSize(platformDisplay, platformWindow, screenSize))
      {
        if (screenSize.width > 0 && screenSize.height > 0 && screenSize.mwidth > 0 && screenSize.mheight > 0)
        {
          FSLLOG3_VERBOSE4("PlatformNativeWindowAdapterX11| UpdateDPI: ");
          FSLLOG3_VERBOSE4("- screenSize.width: {}", screenSize.width);
          FSLLOG3_VERBOSE4("- screenSize.height: {}", screenSize.height);
          FSLLOG3_VERBOSE4("- screenSize.mwidth: {}", screenSize.mwidth);
          FSLLOG3_VERBOSE4("- screenSize.mheight: {}", screenSize.mheight);
          rScreenDPI = Point2(CalcDPI(screenSize.width, screenSize.mwidth), CalcDPI(screenSize.height, screenSize.mheight));
          FSLLOG3_VERBOSE4("- DPI: {}, {}", rScreenDPI.X, rScreenDPI.Y);
        }
        else
        {
          FSLLOG3_WARNING("Failed to acquire proper DPI information, using defaults");
        }
      }
      else
      {
        FSLLOG3_WARNING("Failed to acquire real screen DPI using defaults");
      }
    }


    std::shared_ptr<IPlatformNativeWindowAdapter>
      AllocateWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
                     const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    {
      return std::make_shared<PlatformNativeWindowAdapterX11>(nativeWindowSetup, windowParams, pPlatformCustomWindowAllocationParams);
    }

    // struct InfoXInput2
    // {
    //   bool Available = false;
    //   int MajorOpCode = 0;
    //   int EventBase = 0;
    //   int ErrorBase = 0;
    //   int MajorVersion = 0;
    //   int MinorVersion = 0;
    // };
    // https://who-t.blogspot.com/2009/05/xi2-recipes-part-1.html
    // https://www.clearchain.com/blog/posts/xinput-1-xinput-2-conversion-guide
    // InfoXInput2 DetectXInput2(const PlatformNativeDisplayType platformDisplay)
    // {
    //   InfoXInput2 info;
    //   if (XQueryExtension(platformDisplay, "XInputExtension", &info.MajorOpCode, &info.EventBase, &info.ErrorBase) != 0)
    //   {
    //     int majorVersion = 2;
    //     int minorVersion = 0;
    //     if (XIQueryVersion(platformDisplay, &majorVersion, &minorVersion) == Success)
    //     {
    //       info.MajorVersion = majorVersion;
    //       info.MinorVersion = minorVersion;
    //       if (majorVersion >= 2)
    //       {
    //         info.Available = true;
    //       }
    //     }
    //   }
    //   return info;
    // }
  }    // namespace


  PlatformNativeWindowSystemAdapterX11::PlatformNativeWindowSystemAdapterX11(const NativeWindowSystemSetup& setup,
                                                                             const PlatformNativeWindowAllocationFunction& allocateWindowFunction,
                                                                             const PlatformNativeWindowSystemParams& /*systemParams*/)
    : PlatformNativeWindowSystemAdapter(setup, nullptr)
    , m_visualId(0)
    , m_allocationFunction(allocateWindowFunction ? allocateWindowFunction : AllocateWindow)
    , m_extensionRREnabled(false)
    , m_rrEventBase(0)
    , m_rrErrorBase(0)
  {
    FSLLOG3_WARNING_IF(setup.GetConfig().GetDisplayId() != 0, "X11 only supports the main display. Using DisplayId 0 instead of {}",
                       setup.GetConfig().GetDisplayId());
    FSLLOG3_VERBOSE3("PlatformNativeWindowSystemAdapterX11| XOpenDisplay");
    m_platformDisplay = XOpenDisplay(nullptr);
    if (m_platformDisplay == nullptr)
    {
      throw GraphicsException("Could not open display");
    }

    // Lookup the default visual
    auto* defaultVisual = DefaultVisual(m_platformDisplay, 0);
    m_visualId = XVisualIDFromVisual(defaultVisual);

    // Get some extension info
    m_extensionRREnabled = (XRRQueryExtension(m_platformDisplay, &m_rrEventBase, &m_rrErrorBase) != 0);
    FSLLOG3_VERBOSE3("PlatformNativeWindowSystemAdapterX11| Extension RR: {}", m_extensionRREnabled);
    if (m_extensionRREnabled)
    {
      int major = 0;
      int minor = 0;
      if (XRRQueryVersion(m_platformDisplay, &major, &minor) != 0)
      {
        FSLLOG3_VERBOSE3("PlatformNativeWindowSystemAdapterX11| RR version: {}.{}", major, minor);
        if (major == 1 && minor <= 1)
        {
          FSLLOG3_WARNING("PlatformNativeWindowSystemAdapterX11: Due to the XRR version some features have been disabled.");
          m_extensionRREnabled = false;
        }
      }
    }

    // auto xinput2Info = DetectXInput2(m_platformDisplay);
    // FSLLOG3_VERBOSE3(_IF(!xinput2Info.Available, "PlatformNativeWindowSystemAdapterX11| xinput2 unavailable");
    // FSLLOG3_VERBOSE3(_IF(xinput2Info.Available,
    //            "PlatformNativeWindowSystemAdapterX11| xinput2 version: " << xinput2Info.MajorVersion << "." << xinput2Info.MinorVersion);

    auto eventQueue = setup.GetEventQueue().lock();
    if (eventQueue)
    {
      const NativeWindowEvent event = NativeWindowEventHelper::EncodeGamepadConfiguration(0);
      eventQueue->PostEvent(event);
    }

    FSLLOG3_VERBOSE3("PlatformNativeWindowSystemAdapterX11| XOpenDisplay: Completed");
  }


  PlatformNativeWindowSystemAdapterX11::~PlatformNativeWindowSystemAdapterX11()
  {
    FSLLOG3_VERBOSE3("PlatformNativeWindowSystemAdapterX11| XCloseDisplay begin");
    XCloseDisplay(m_platformDisplay);
    FSLLOG3_VERBOSE3("PlatformNativeWindowSystemAdapterX11| XCloseDisplay done");
  }


  std::shared_ptr<IPlatformNativeWindowAdapter>
    PlatformNativeWindowSystemAdapterX11::CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup,
                                                             const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
  {
    FSLLOG3_VERBOSE3("PlatformNativeWindowSystemAdapterX11| CreateNativeWindow")
    auto window = m_allocationFunction(nativeWindowSetup, PlatformNativeWindowParams(m_platformDisplay, m_visualId, m_extensionRREnabled),
                                       pPlatformCustomWindowAllocationParams);

    auto ptr = std::dynamic_pointer_cast<PlatformNativeWindowAdapterX11>(window);
    if (!ptr)
    {
      throw NotSupportedException("Allocation function did not allocate a PlatformNativeWindowAdapterX11 as required");
    }

    SetCreatedWindow(ptr);
    return window;
  }


  bool PlatformNativeWindowSystemAdapterX11::ProcessMessages(const NativeWindowProcessMessagesArgs& /*args*/)
  {
    const std::shared_ptr<PlatformNativeWindowAdapterX11> window = m_window.lock();
    VirtualKey::Enum keyCode = VirtualKey::Undefined;
    std::shared_ptr<INativeWindowEventQueue> eventQueue = g_eventQueue.lock();
    PxPoint2 mousePosition;
    VirtualMouseButton::Enum mouseButton = VirtualMouseButton::Undefined;
    bool bQuit = false;
    XEvent event;
    static int wheelEvents = 0;

    while (XPending(m_platformDisplay) > 0)
    {
      XNextEvent(m_platformDisplay, &event);

      switch (event.type)
      {
      case ConfigureNotify:
        if (window && event.xconfigure.window == window->GetPlatformWindow())
        {
          // FSLLOG3_INFO("Window moved or resized");
          window->OnConfigureNotify(event.xconfigure, eventQueue);
        }
        break;
      case DestroyNotify:
        bQuit = true;
        break;
      case ClientMessage:
        if (window)
        {
          if (static_cast<Atom>(event.xclient.data.l[0]) == window->m_wm_delete_window)
          {
            bQuit = true;
          }
          break;
        }
      case ButtonPress:
        {
          mousePosition = PxPoint2::Create(event.xbutton.x, event.xbutton.y);
          if (event.xbutton.button < Button4)
          {
            // Regular Mouse Buttons
            mouseButton = MouseToVirtualMouse(event.xbutton.button);
            if (eventQueue)
            {
              eventQueue->PostEvent(NativeWindowEventHelper::EncodeInputMouseButtonEvent(mouseButton, true, mousePosition));
            }
          }
          else
          {
            // Wheel Event
            if (Button4 == event.xbutton.button)
            {
              wheelEvents++;
            }
            else if (Button5 == event.xbutton.button)
            {
              wheelEvents--;
            }
            if (eventQueue)
            {
              eventQueue->PostEvent(NativeWindowEventHelper::EncodeInputMouseWheelEvent(wheelEvents, mousePosition));
            }
          }
          break;
        }
      case ButtonRelease:
        {
          mousePosition = PxPoint2::Create(event.xbutton.x, event.xbutton.y);
          mouseButton = MouseToVirtualMouse(event.xbutton.button);
          if (eventQueue)
          {
            eventQueue->PostEvent(NativeWindowEventHelper::EncodeInputMouseButtonEvent(mouseButton, false, mousePosition));
          }
          break;
        }
      case KeyPress:
        {
          keyCode = KeyToVirtualKey(&event.xkey);
          if (eventQueue)
          {
            eventQueue->PostEvent(NativeWindowEventHelper::EncodeInputKeyEvent(keyCode, true));
          }
          break;
        }
      case KeyRelease:
        {
          keyCode = KeyToVirtualKey(&event.xkey);
          if (eventQueue)
          {
            eventQueue->PostEvent(NativeWindowEventHelper::EncodeInputKeyEvent(keyCode, false));
          }
          break;
        }
      case MotionNotify:
        {
          mousePosition = PxPoint2::Create(event.xmotion.x, event.xmotion.y);
          if (eventQueue)
          {
            eventQueue->PostEvent(NativeWindowEventHelper::EncodeInputMouseMoveEvent(mousePosition));
          }
          break;
        }
      default:
        if (window && m_extensionRREnabled)
        {
          if (event.type == m_rrEventBase + RRScreenChangeNotify)
          {
            window->OnRRScreenChangeNotify(&event, eventQueue);
          }
        }
        break;
      }
    }
    return !bQuit;
  }


  void PlatformNativeWindowSystemAdapterX11::SetCreatedWindow(const std::weak_ptr<PlatformNativeWindowAdapterX11>& window)
  {
    FSLLOG3_VERBOSE3("PlatformNativeWindowSystemAdapterX11| SetCreatedWindow");
    auto currentWindow = m_window.lock();
    if (currentWindow)
    {
      throw NotSupportedException("We only support one active window");
    }

    m_window = window;
  }


  PlatformNativeWindowAdapterX11::PlatformNativeWindowAdapterX11(
    const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& platformWindowParams,
    const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    : PlatformNativeWindowAdapter(nativeWindowSetup, platformWindowParams, pPlatformCustomWindowAllocationParams, NativeWindowCapabilityFlags::GetDpi)
    , m_pVisual(nullptr)
    , m_cachedScreenDPI(MAGIC_DEFAULT_DPI, MAGIC_DEFAULT_DPI)
  {
    FSLLOG3_VERBOSE3("PlatformNativeWindowAdapterX11| Constructing");

    const NativeWindowConfig nativeWindowConfig = nativeWindowSetup.GetConfig();
    int windowWidth = 0;
    int windowHeight = 0;
    int windowX = 0;
    int windowY = 0;
    g_eventQueue = nativeWindowSetup.GetEventQueue();

    XVisualInfo visualInfo{};
    visualInfo.visualid = platformWindowParams.VisualId;

    int visualItemCount = 0;
    m_pVisual = XGetVisualInfo(m_platformDisplay, VisualIDMask, &visualInfo, &visualItemCount);
    if (m_pVisual == nullptr)
    {
      throw GraphicsException("Could not get native visual");
    }

    const int screen = DefaultScreen(m_platformDisplay);
    const Window rootwindow = RootWindow(m_platformDisplay, screen);
    XWindowAttributes windowATTR;
    XGetWindowAttributes(m_platformDisplay, rootwindow, &windowATTR);
    // GET SCREEN RESOURCES FOR THE DEVICE
    // XRRScreenResources * screenResources = XRRGetScreenResources(m_platformDisplay, rootwindow);

    // We now have info on the Screen Resources, get information about the available CRTs
    // XRRCrtcInfo * crtDeviceInformation = XRRGetCrtcInfo(m_platformDisplay, screenResources, screenResources->crtcs[0]);

    // printf("Specific CRT information %dx%d\n",crtDeviceInformation->width, crtDeviceInformation->height);

    if (nativeWindowConfig.GetWindowMode() != WindowMode::Window)
    {
      windowWidth = windowATTR.width;
      windowHeight = windowATTR.height;
      windowX = 0;
      windowY = 0;

      FSLLOG3_VERBOSE2("PlatformNativeWindowAdapterX11| Creating fullscreen window: (X={} Y={} Width={} Height={})", windowX, windowY, windowWidth,
                       windowHeight);
    }
    else
    {
      const Rectangle windowRectangle = nativeWindowConfig.GetWindowRectangle();
      windowWidth = windowRectangle.Width();
      windowHeight = windowRectangle.Height();
      windowX = windowRectangle.X();
      windowY = windowRectangle.Y();

      FSLLOG3_VERBOSE2("PlatformNativeWindowAdapterX11| Creating window: {}", windowRectangle);
    }

    m_colormap = XCreateColormap(m_platformDisplay, rootwindow, m_pVisual->visual, AllocNone);

    const unsigned long mask = CWBackPixel | CWBorderPixel | CWEventMask | CWColormap;

    XSetWindowAttributes windowAttributes;
    windowAttributes.colormap = m_colormap;
    windowAttributes.background_pixel = 0xFFFFFFFF;
    windowAttributes.border_pixel = 0;
    windowAttributes.event_mask = StructureNotifyMask | ExposureMask;


    m_platformWindow = XCreateWindow(m_platformDisplay, rootwindow, windowX, windowY, windowWidth, windowHeight, 0, m_pVisual->depth, InputOutput,
                                     m_pVisual->visual, mask, &windowAttributes);

    XMapWindow(m_platformDisplay, m_platformWindow);

    XEvent event;
    XIfEvent(m_platformDisplay, &event, WaitForMap, reinterpret_cast<XPointer>(&m_platformWindow));

    XSetWMColormapWindows(m_platformDisplay, m_platformWindow, &m_platformWindow, 1);

    XFlush(m_platformDisplay);

    // Hook up and listen for window close
    m_wm_delete_window = XInternAtom(m_platformDisplay, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(m_platformDisplay, m_platformWindow, &m_wm_delete_window, 1);

    // Listen for various events
    const long evMask = ExposureMask | StructureNotifyMask | VisibilityChangeMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask |
                        KeyReleaseMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask;
    XSelectInput(m_platformDisplay, m_platformWindow, evMask);

    // Enable some XRR events.
    if (platformWindowParams.ExtensionRREnabled)
    {
      XRRSelectInput(m_platformDisplay, m_platformWindow, RRScreenChangeNotifyMask);
    }

    m_cachedWindowSize = PxPoint2::Create(windowWidth, windowHeight);
    TryUpdateDPI(m_platformDisplay, m_platformWindow, m_cachedScreenDPI);

    {    // Post the activation message to let the framework know we are ready
      std::shared_ptr<INativeWindowEventQueue> eventQueue = g_eventQueue.lock();
      if (eventQueue)
      {
        eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowActivationEvent(true));
      }
    }
    FSLLOG3_VERBOSE3("PlatformNativeWindowAdapterX11| Constructed");
  }


  PlatformNativeWindowAdapterX11::~PlatformNativeWindowAdapterX11()
  {
    // X windows clean up
    XDestroyWindow(m_platformDisplay, m_platformWindow);
    XFreeColormap(m_platformDisplay, m_colormap);

    if (m_pVisual != nullptr)
    {
      XFree(m_pVisual);
    }
  }

  void PlatformNativeWindowAdapterX11::OnConfigureNotify(const XConfigureEvent& event, const std::shared_ptr<INativeWindowEventQueue>& eventQueue)
  {
    PxPoint2 newSize(PxPoint2::Create(event.width, event.height));
    if (newSize == m_cachedWindowSize)
    {
      return;
    }

    m_cachedWindowSize = newSize;
    FSLLOG3_VERBOSE2("PlatformNativeWindowAdapterX11| Updating cached size to {}", m_cachedWindowSize);

    if (eventQueue)
    {
      eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowResizedEvent());
    }
  }


  void PlatformNativeWindowAdapterX11::OnRRScreenChangeNotify(XEvent* pEvent, const std::shared_ptr<INativeWindowEventQueue>& eventQueue)
  {
    XRRUpdateConfiguration(pEvent);
    auto* pSpecificEvent = reinterpret_cast<XRRScreenChangeNotifyEvent*>(pEvent);

    Point2 newDPI(CalcDPI(pSpecificEvent->width, pSpecificEvent->mwidth), CalcDPI(pSpecificEvent->height, pSpecificEvent->mheight));
    if (newDPI == m_cachedScreenDPI)
    {
      return;
    }

    m_cachedScreenDPI = newDPI;
    FSLLOG3_VERBOSE2("PlatformNativeWindowAdapterX11| Cached DPI updated: {}", m_cachedScreenDPI);
    if (eventQueue)
    {
      eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowConfigChanged());
    }
  }

  bool PlatformNativeWindowAdapterX11::TryGetNativeSize(PxPoint2& rSize) const
  {
    rSize = m_cachedWindowSize;
    return true;
  }

  bool PlatformNativeWindowAdapterX11::TryGetNativeDpi(Vector2& rDPI) const
  {
    rDPI = Vector2(m_cachedScreenDPI.X, m_cachedScreenDPI.Y);
    return true;
  }

}    // namespace Fsl
#endif
