#ifdef _WIN32
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

#include <FslNativeWindowPlatform/Win32/PlatformNativeWindowSystemWin32.hpp>
#include <FslNativeWindowPlatform/Win32/PlatformNativeWindowWin32.hpp>
#include <FslNativeWindow/INativeWindowEventQueue.hpp>
#include <FslNativeWindow/NativeWindowEventHelper.hpp>
#include <FslNativeWindow/NativeWindowSetup.hpp>
#include <FslNativeWindow/NativeWindowSystemSetup.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Log/Log.hpp>
#include <algorithm>
#include <deque>
#include <iostream>
#include <memory>
#include <windowsx.h>
#include <Winuser.h>
#include "DPIHelperWin32.hpp"

namespace Fsl
{
  namespace
  {
    const uint32_t MAGIC_DEFAULT_DPI = 96;

    bool TryConvert(WPARAM wParam, VirtualKey::Enum& rKey)
    {
      switch (wParam)
      {
      case VK_ESCAPE:
        rKey = VirtualKey::Escape;
        return true;
      case VK_TAB:
        rKey = VirtualKey::Tab;
        return true;
      case VK_RETURN:
        rKey = VirtualKey::Return;
        return true;
      case VK_SPACE:
        rKey = VirtualKey::Space;
        return true;
      case VK_BACK:
        rKey = VirtualKey::Backspace;
        return true;

      case VK_UP:
        rKey = VirtualKey::UpArrow;
        return true;
      case VK_DOWN:
        rKey = VirtualKey::DownArrow;
        return true;
      case VK_LEFT:
        rKey = VirtualKey::LeftArrow;
        return true;
      case VK_RIGHT:
        rKey = VirtualKey::RightArrow;
        return true;

      case VK_INSERT:
        rKey = VirtualKey::Insert;
        return true;
      case VK_DELETE:
        rKey = VirtualKey::Delete;
        return true;
      case VK_HOME:
        rKey = VirtualKey::Home;
        return true;
      case VK_END:
        rKey = VirtualKey::End;
        return true;
      case VK_PRIOR:
        rKey = VirtualKey::PageUp;
        return true;
      case VK_NEXT:
        rKey = VirtualKey::PageDown;
        return true;
        // Snapshot only sends a up event, so we can't really use it
        // There are workarounds, but its not worth using time on at the moment.
        //case VK_SNAPSHOT:
        //  rKey = VirtualKey::PrintScreen;
        //  return true;
      case VK_SCROLL:
        rKey = VirtualKey::ScrollLock;
        return true;
      case VK_ADD:
        rKey = VirtualKey::Add;
        return true;
      case VK_SUBTRACT:
        rKey = VirtualKey::Subtract;
        return true;
      default:
        break;
      }

      if (wParam >= '0' && wParam <= '9')
      {
        // Take advantage of the simple encoding
        rKey = static_cast<VirtualKey::Enum>(VirtualKey::Code0 + (wParam - '0'));
        return true;
      }
      if (wParam >= 'A' && wParam <= 'Z')
      {
        // Take advantage of the simple encoding
        rKey = static_cast<VirtualKey::Enum>(VirtualKey::A + (wParam - 'A'));
        return true;
      }
      if (wParam >= VK_F1 && wParam <= VK_F12)
      {
        rKey = static_cast<VirtualKey::Enum>(VirtualKey::F1 + (wParam - VK_F1));
        return true;
      }

      return false;
    }
  }

  class PlatformNativeWindowSystemWin32State
  {
    struct WindowRecord
    {
      std::weak_ptr<PlatformNativeWindowWin32> Window;

      WindowRecord()
        : Window()
      {
      }

      WindowRecord(const std::weak_ptr<PlatformNativeWindowWin32>& window)
        : Window(window)
      {
      }
    };

    std::weak_ptr<INativeWindowEventQueue> m_eventQueue;
    bool m_forceActivated;
    int m_activated = 0;
    bool m_isMouseCaptured;
    uint32_t m_mouseButtonState;
    std::deque<WindowRecord> m_activeWindows;
  public:

    PlatformNativeWindowSystemWin32State(const std::weak_ptr<INativeWindowEventQueue>& eventQueue)
      : m_eventQueue(eventQueue)
      , m_forceActivated(true)  // For now we always force activation (meaning we dont lose deactivate on windows focus loss)
      , m_activated(0)
      , m_isMouseCaptured(false)
      , m_mouseButtonState(0)
    {
    }


    void AddWindow(const std::weak_ptr<PlatformNativeWindowWin32>& window)
    {
      // Garbage collect all dead windows
      for (auto itr = m_activeWindows.begin(); itr != m_activeWindows.end(); ++itr)
      {
        if (itr->Window.expired())
          itr = m_activeWindows.erase(itr);
      }

      m_activeWindows.push_back(WindowRecord(window));
    }


    std::weak_ptr<INativeWindowEventQueue> GetEventQueue() const
    {
      return m_eventQueue;
    }

    std::shared_ptr<PlatformNativeWindowWin32> TryGetWindow(HWND hWnd) const
    {
      auto compareFunc = [hWnd](const WindowRecord& val)
      {
        auto window = val.Window.lock();
        return (window && window->GetPlatformWindow() == hWnd);
      };

      auto itr = std::find_if(m_activeWindows.begin(), m_activeWindows.end(), compareFunc);
      return (itr != m_activeWindows.end() ? itr->Window.lock() : std::shared_ptr<PlatformNativeWindowWin32>());
    }


    LRESULT OnKeyMessage(HWND hWnd, const std::shared_ptr<INativeWindowEventQueue>& eventQueue, WPARAM wParam, LPARAM lParam, const bool isPressed)
    {
      VirtualKey::Enum keycode;
      if (TryConvert(wParam, keycode))
      {
        const NativeWindowEvent event = NativeWindowEventHelper::EncodeInputKeyEvent(keycode, isPressed);
        eventQueue->PostEvent(event);
      }
      return 0;
    }


    LRESULT OnMouseButton(HWND hWnd, const std::shared_ptr<INativeWindowEventQueue>& eventQueue, WPARAM wParam, LPARAM lParam, const VirtualMouseButton::Enum button, const bool isPressed)
    {
      const Point2 position(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
      const NativeWindowEvent event = NativeWindowEventHelper::EncodeInputMouseButtonEvent(button, isPressed, position);
      eventQueue->PostEvent(event);

      if (isPressed)
        m_mouseButtonState |= button;
      else
        m_mouseButtonState &= ~button;

      if (m_mouseButtonState != 0)
        SetCapture(hWnd);
      else
        ReleaseCapture();
      return 0;
    }


    LRESULT OnMouseMove(HWND hWnd, const std::shared_ptr<INativeWindowEventQueue>& eventQueue, WPARAM wParam, LPARAM lParam)
    {
      const Point2 position(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
      const NativeWindowEvent event = NativeWindowEventHelper::EncodeInputMouseMoveEvent(position);
      eventQueue->PostEvent(event);
      return 0;
    }


    LRESULT OnMouseWheel(HWND hWnd, const std::shared_ptr<INativeWindowEventQueue>& eventQueue, WPARAM wParam, LPARAM lParam)
    {
      const int32_t zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
      const Point2 position(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
      const NativeWindowEvent event = NativeWindowEventHelper::EncodeInputMouseWheelEvent(zDelta, position);
      eventQueue->PostEvent(event);
      return 0;
    }

    LRESULT OnActivateMessage(HWND hWnd, const std::shared_ptr<INativeWindowEventQueue>& eventQueue, WPARAM wParam, LPARAM lParam)
    {
      const int hiWord = ((wParam >> 16) & 0xFFFF);
      const int lowWord = (wParam & 0xFFFF);

      // Order of commands on activate == false and minimize
      // - NativeWindowEventType::WindowActivation false
      // - NativeWindowEventType::WindowSuspend true
      // Order of commands on activate == true and restore
      // - NativeWindowEventType::WindowSuspend false
      // - NativeWindowEventType::WindowActivation true

      // When the window is minimized we send the suspend message
      //const bool isMinimize = hiWord != 0;
      //if (isMinimize && lowWord != 0)
      //  eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowSuspendEvent(false));

      // Ensure that we only send state changes
      const int activated = (lowWord != 0 || m_forceActivated ? 1 : 0);
      if (activated != m_activated)
      {
        m_activated = activated;
        eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowActivationEvent(activated != 0));
      }

      // When the window is minimized we send the suspend message
      //if (isMinimize && lowWord == 0)
      //  eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowSuspendEvent(true));
      return 0;
    }


    LRESULT OnSize(HWND hWnd, const std::shared_ptr<INativeWindowEventQueue>& eventQueue, WPARAM wParam, LPARAM lParam)
    {
      // The window has been resized, but neither the SIZE_MINIMIZED nor SIZE_MAXIMIZED value applies.
      if (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED)
        eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowResizedEvent());
      return 0;
    }


    LRESULT OnDPICHanged(HWND hWnd, WPARAM wParam, LPARAM lParam)
    {
      // wParam = new DPI
      const auto newDpiX = LOWORD(wParam);
      const auto newDpiY = HIWORD(wParam);
      // for now we just spy on this info
      auto window = TryGetWindow(hWnd);
      if (window)
        window->OnDPIChanged(Point2(newDpiX, newDpiY));

      // A pointer to a RECT structure that provides a suggested size and position of the current window scaled for the new DPI.
      // The expectation is that apps will reposition and resize windows based on the suggestions provided by lParam when handling this message.
      //LPRECT lprcNewScale;
      //lprcNewScale = (LPRECT)lParam;
      //const auto newWidth = lprcNewScale->right - lprcNewScale->left;
      //const auto newHeight = lprcNewScale->bottom - lprcNewScale->top;
      //// Resize the window for the new DPI setting
      //SetWindowPos(hWnd, HWND_TOP, lprcNewScale->left, lprcNewScale->top, newWidth, newHeight, SWP_NOZORDER | SWP_NOACTIVATE);
      // Render the window contents again
      //RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
      return 1;
    }
  };

  namespace
  {
    std::weak_ptr<PlatformNativeWindowSystemWin32State> g_windowSystemState;

    LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
    {
      auto windowSystemState = g_windowSystemState.lock();


      if (uiMsg == WM_CLOSE)
      {
        PostQuitMessage(0);
        return 0;
      }
      else if (windowSystemState)
      {
        auto eventQueue = windowSystemState->GetEventQueue().lock();
        if (eventQueue)
        {
          switch (uiMsg)
          {
          case WM_ACTIVATE:
            return windowSystemState->OnActivateMessage(hWnd, eventQueue, wParam, lParam);
          case WM_KEYDOWN:
            return windowSystemState->OnKeyMessage(hWnd, eventQueue, wParam, lParam, true);
          case WM_KEYUP:
            return windowSystemState->OnKeyMessage(hWnd, eventQueue, wParam, lParam, false);
          case WM_LBUTTONDOWN:
            return windowSystemState->OnMouseButton(hWnd, eventQueue, wParam, lParam, VirtualMouseButton::Left, true);
          case WM_LBUTTONUP:
            return windowSystemState->OnMouseButton(hWnd, eventQueue, wParam, lParam, VirtualMouseButton::Left, false);
          case WM_MBUTTONDOWN:
            return windowSystemState->OnMouseButton(hWnd, eventQueue, wParam, lParam, VirtualMouseButton::Middle, true);
          case WM_MBUTTONUP:
            return windowSystemState->OnMouseButton(hWnd, eventQueue, wParam, lParam, VirtualMouseButton::Middle, false);
          case WM_RBUTTONDOWN:
            return windowSystemState->OnMouseButton(hWnd, eventQueue, wParam, lParam, VirtualMouseButton::Right, true);
          case WM_RBUTTONUP:
            return windowSystemState->OnMouseButton(hWnd, eventQueue, wParam, lParam, VirtualMouseButton::Right, false);
          case WM_MOUSEMOVE:
            return windowSystemState->OnMouseMove(hWnd, eventQueue, wParam, lParam);
          case WM_MOUSEWHEEL:
            return windowSystemState->OnMouseWheel(hWnd, eventQueue, wParam, lParam);
          case WM_SIZE:
            return windowSystemState->OnSize(hWnd, eventQueue, wParam, lParam);
          case WM_DPICHANGED:
            return windowSystemState->OnDPICHanged(hWnd, wParam, lParam);
          case WM_QUIT:
          case WM_QUERYENDSESSION:
          default:
            break;
          }
        }
      }
      return DefWindowProc(hWnd, uiMsg, wParam, lParam);
    }


    //! @brief Adjust the client size of the window to the expected dimensions
    void DoSetClientSize(HWND hwnd, int clientWidth, int clientHeight)
    {
      if (IsWindow(hwnd))
      {
        DWORD dwStyle = static_cast<DWORD>(GetWindowLongPtr(hwnd, GWL_STYLE));
        DWORD dwExStyle = static_cast<DWORD>(GetWindowLongPtr(hwnd, GWL_EXSTYLE));
        auto menu = GetMenu(hwnd);

        RECT rc = { 0, 0, clientWidth, clientHeight };

        if (!AdjustWindowRectEx(&rc, dwStyle, menu ? TRUE : FALSE, dwExStyle))
          MessageBox(nullptr, TEXT("AdjustWindowRectEx Failed!"), TEXT("Error"), MB_OK);
        SetWindowPos(hwnd, nullptr, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOMOVE);
      }
    }

    struct MonitorRecord
    {
      HMONITOR Handle;
      Rectangle Rect;

      MonitorRecord()
      {
      }

      MonitorRecord(HMONITOR hMonitor, Rectangle rect)
      {
        Handle = hMonitor;
        Rect = rect;
      }
    };

    std::deque<MonitorRecord> g_monitorDeque;

    BOOL CALLBACK MonitorEnumProc(_In_  HMONITOR hMonitor, _In_  HDC      hdcMonitor, _In_  LPRECT   lprcMonitor, _In_  LPARAM   dwData)
    {
      if (hdcMonitor == nullptr &&  lprcMonitor != nullptr)
      {
        const int32_t left = lprcMonitor->left;
        const int32_t top = lprcMonitor->top;
        const int32_t dx = lprcMonitor->right - left;
        const int32_t dy = lprcMonitor->bottom - top;
        if ( dx > 0 && dy > 0 )
          g_monitorDeque.push_back(MonitorRecord(hMonitor, Rectangle(left, top, dx, dy)));
      }
      return true;
    }

    bool TryGetMonitor(int32_t displayId, MonitorRecord& rRecord, int32_t& rMaxDisplayId)
    {
      rMaxDisplayId = 0;
      g_monitorDeque.clear();
      EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, 0);
      rMaxDisplayId = static_cast<int32_t>(g_monitorDeque.size());
      if (displayId < 0 || displayId >= rMaxDisplayId)
      {
        g_monitorDeque.clear();
        rRecord = MonitorRecord();
        return false;
      }
      rRecord = g_monitorDeque[displayId];
      g_monitorDeque.clear();
      return true;
    }


    MonitorRecord GetMonitorRecord(const int32_t requestedDisplayId)
    {
      int32_t maxDisplayId;
      MonitorRecord record;
      if (!TryGetMonitor(requestedDisplayId, record, maxDisplayId))
      {
        FSLLOG_WARNING("Backend max display Id is: " << maxDisplayId << " so " << requestedDisplayId << " is unsupported, using displayid 0.");
        if (!TryGetMonitor(0, record, maxDisplayId))
          throw GraphicsException("Could not find a valid monitor");
      }
      return record;
    }





    std::shared_ptr<INativeWindow> AllocateWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams, const PlatformNativeWindowAllocationParams*const pPlatformCustomWindowAllocationParams)
    {
      return std::make_shared<PlatformNativeWindowWin32>(nativeWindowSetup, windowParams, pPlatformCustomWindowAllocationParams);
    }
  }


  PlatformNativeWindowSystemWin32::PlatformNativeWindowSystemWin32(const NativeWindowSystemSetup& setup, const PlatformNativeWindowAllocationFunction& allocateWindowFunction, const PlatformNativeWindowSystemParams& systemParams)
    : PlatformNativeWindowSystem(setup, GetModuleHandle(nullptr))
    , m_allocationFunction(allocateWindowFunction ? allocateWindowFunction : AllocateWindow)
    , m_dpiHelper(std::make_shared<DPIHelperWin32>())
  {
    if (!g_windowSystemState.expired())
      throw NotSupportedException("PlatformNativeWindowSystemWin32 is a singleton, please destroy any previously allocated instance first");

    // Create the singleton state object
    auto windowSystemState = std::make_shared<PlatformNativeWindowSystemWin32State>(setup.GetEventQueue());
    m_windowSystemState = windowSystemState;
    g_windowSystemState = windowSystemState;
  }


  PlatformNativeWindowSystemWin32::~PlatformNativeWindowSystemWin32()
  {
    g_windowSystemState.reset();
  }


  std::shared_ptr<INativeWindow> PlatformNativeWindowSystemWin32::CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowAllocationParams*const pPlatformCustomWindowAllocationParams)
  {
    auto newBaseWindow = m_allocationFunction(nativeWindowSetup, PlatformNativeWindowParams(m_platformDisplay, m_dpiHelper), pPlatformCustomWindowAllocationParams);
    auto newWindow = std::dynamic_pointer_cast<PlatformNativeWindowWin32>(newBaseWindow);
    if (newWindow == nullptr)
      throw NotSupportedException("The CreateNativeWindow method returned a unsupported window type");

    m_windowSystemState->AddWindow(newWindow);
    return newWindow;
  }


  bool PlatformNativeWindowSystemWin32::ProcessMessages(const NativeWindowProcessMessagesArgs& args)
  {
    MSG sMessage;
    bool bQuit = false;
    if (PeekMessage(&sMessage, nullptr, 0, 0, PM_REMOVE))
    {
      switch (sMessage.message)
      {
      case WM_QUIT:
        bQuit = true;
        break;
      default:
        break;
      }

      TranslateMessage(&sMessage);
      DispatchMessage(&sMessage);
    }
    return !bQuit;
  }


  PlatformNativeWindowWin32::PlatformNativeWindowWin32(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& platformWindowParams, const PlatformNativeWindowAllocationParams*const pPlatformCustomWindowAllocationParams)
    : PlatformNativeWindow(nativeWindowSetup, platformWindowParams, pPlatformCustomWindowAllocationParams)
    , m_dpiHelper(platformWindowParams.DpiHelper)
  {
    const NativeWindowConfig nativeWindowConfig = nativeWindowSetup.GetConfig();
    WNDCLASS wc;
    RECT rect;
    HINSTANCE hInstance;

    Rectangle targetRectangle = nativeWindowConfig.GetWindowRectangle();

    bool bFullscreen = false;
    DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    switch (nativeWindowConfig.GetWindowMode())
    {
    case WindowMode::Fullscreen:
      {
        const int32_t displayIdRequest = nativeWindowSetup.GetConfig().GetDisplayId();
        if (displayIdRequest == 0)
        {
          const int width = GetSystemMetrics(SM_CXSCREEN);
          const int height = GetSystemMetrics(SM_CYSCREEN);
          targetRectangle = Rectangle(0, 0, width, height);
        }
        else
        {
          const MonitorRecord record = GetMonitorRecord(displayIdRequest);
          targetRectangle = record.Rect;
        }

        dwStyle = WS_POPUP | WS_VISIBLE | WS_SYSMENU;
        bFullscreen = true;
      }
      break;
    case WindowMode::Window:
      break;
    default:
      FSLLOG("WARNING: Unknown window mode");
      break;
    }


    rect.left = targetRectangle.Left();
    rect.right = targetRectangle.Right();
    rect.top = targetRectangle.Top();
    rect.bottom = targetRectangle.Bottom();

    hInstance = GetPlatformDisplay();


    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = (WNDPROC)WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = TEXT("OpenGLES");

    auto icon = (HICON)LoadImage(nullptr, TEXT("FslSDKIcon.ico"), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
    if (icon == nullptr)
      wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    else
      wc.hIcon = icon;

    RegisterClass(&wc);

    m_platformWindow = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, TEXT("OpenGLES"), TEXT("Main"), dwStyle, targetRectangle.Left(), targetRectangle.Top(), targetRectangle.Width(), targetRectangle.Height(), nullptr, nullptr, hInstance, nullptr);
    if (m_platformWindow == nullptr)
      throw GraphicsException("Failed to create native window");

    //if (bFullscreen)
    //  SetWindowLong(m_platformWindow, GWL_STYLE, 0);

    DoSetClientSize(m_platformWindow, targetRectangle.Width(), targetRectangle.Height());

    ShowWindow(m_platformWindow, SW_SHOW);
    SetForegroundWindow(m_platformWindow);
    SetFocus(m_platformWindow);

    if (! m_dpiHelper->TryGetDPI(m_platformWindow, m_cachedDPIValue))
    {
      FSLLOG_DEBUG_WARNING("Failed to cache DPI value, using default");
      m_cachedDPIValue = Point2(MAGIC_DEFAULT_DPI, MAGIC_DEFAULT_DPI);
    }
  }


  PlatformNativeWindowWin32::~PlatformNativeWindowWin32()
  {
    if (m_platformWindow != nullptr)
    {
      DestroyWindow(m_platformWindow);
      m_platformWindow = nullptr;
    }
  }


  bool PlatformNativeWindowWin32::TryGetDPI(Vector2& rDPI) const
  {
    rDPI = Vector2(m_cachedDPIValue.X, m_cachedDPIValue.Y);
    return true;
  }


  bool PlatformNativeWindowWin32::TryGetSize(Point2& rSize) const
  {
    RECT rect;
    if (!GetClientRect(m_platformWindow, &rect))
    {
      rSize = Point2();
      return false;
    }
    rSize = Point2(rect.right - rect.left, rect.bottom - rect.top);
    return true;
  }


  void PlatformNativeWindowWin32::OnDPIChanged(const Point2& value)
  {
    if (value == m_cachedDPIValue)
      return;

    m_cachedDPIValue = value;

    auto eventQueue = TryGetEventQueue();
    if ( eventQueue )
      eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowDPIChanged());
  }


}
#endif
