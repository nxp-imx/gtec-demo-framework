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

#include <FslNativeWindow/Platform/Win32/PlatformNativeWindowSystemWin32.hpp>
#include <FslNativeWindow/Platform/Win32/PlatformNativeWindowWin32.hpp>
#include <FslNativeWindow/Base/INativeWindowEventQueue.hpp>
#include <FslNativeWindow/Base/NativeWindowEventHelper.hpp>
#include <FslNativeWindow/Base/NativeWindowSetup.hpp>
#include <FslNativeWindow/Base/NativeWindowSystemSetup.hpp>
#include <FslNativeWindow/Base/VirtualGamepadState.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Log/Math/LogRectangle.hpp>
#include <FslBase/System/Platform/PlatformWin32.hpp>
#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <memory>
#include <Xinput.h>
#include <windowsx.h>
#include <Winuser.h>
#include "DPIHelperWin32.hpp"

#if 0
#define VERBOSE_LOG(X) FSLLOG(X)
#define VERBOSE_LOG_IF(cONDITION, X) FSLLOG_IF(cONDITION, X)
#else
#define VERBOSE_LOG(X) \
  {                    \
  }
#define VERBOSE_LOG_IF(cONDITION, X) \
  {                                  \
  }
#endif

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((USHORT)0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((USHORT)0x02)
#endif

namespace Fsl
{
  namespace
  {
    const bool USE_FORCE_ACTIVATED = true;    // For now we always force activation (meaning we dont lose deactivate on windows focus loss)

    inline void UpdateButton(uint32_t& rButtonFlags, const uint16_t wButtons, const uint16_t xinputFlag, const VirtualGamepadButton virtualFlag)
    {
      if ((wButtons & xinputFlag) == xinputFlag)
      {
        rButtonFlags |= static_cast<uint32_t>(virtualFlag);
      }
      else
      {
        rButtonFlags &= ~static_cast<uint32_t>(virtualFlag);
      }
    }

    inline int32_t CapValue(const long value, const char* const pszDebugHelp)
    {
      if (value > std::numeric_limits<int32_t>::max())
      {
        FSLLOG_DEBUG_WARNING("Raw input, capping " << pszDebugHelp << " value to fit in a int32_t (max)");
        return std::numeric_limits<int32_t>::max();
      }
      if (value < std::numeric_limits<int32_t>::min())
      {
        FSLLOG_DEBUG_WARNING("Raw input, capping " << pszDebugHelp << " value to fit in a int32_t (min)");
        return std::numeric_limits<int32_t>::min();
      }
      return static_cast<int32_t>(value);
    }

    inline bool TryToScreenSpace(HWND hwnd, RECT& rRect)
    {
      POINT point;
      point.x = rRect.left;
      point.y = rRect.top;
      if (ClientToScreen(hwnd, &point) == 0)
      {
        return false;
      }
      rRect.left = point.x;
      rRect.top = point.y;

      point.x = rRect.right;
      point.y = rRect.bottom;
      if (ClientToScreen(hwnd, &point) == 0)
      {
        return false;
      }

      rRect.right = point.x;
      rRect.bottom = point.y;
      return true;
    }
  }    // namespace

  struct PlatformGamepadStateWin32
  {
    VirtualGamepadState State;

    PlatformGamepadStateWin32() = default;

    PlatformGamepadStateWin32(const uint32_t deviceId, const bool isConnected, const XINPUT_STATE& state)

    {
      State.IsConnected = isConnected;
      assert(deviceId <= 255);
      State.DeviceId = static_cast<uint8_t>(deviceId);
      State.LeftTrigger = state.Gamepad.bLeftTrigger;
      State.RightTrigger = state.Gamepad.bRightTrigger;
      State.LeftThumbX = state.Gamepad.sThumbLX;
      State.LeftThumbY = state.Gamepad.sThumbLY;
      State.RightThumbX = state.Gamepad.sThumbRX;
      State.RightThumbY = state.Gamepad.sThumbRY;

      UpdateButton(State.Buttons, state.Gamepad.wButtons, XINPUT_GAMEPAD_A, VirtualGamepadButton::A);
      UpdateButton(State.Buttons, state.Gamepad.wButtons, XINPUT_GAMEPAD_B, VirtualGamepadButton::B);
      UpdateButton(State.Buttons, state.Gamepad.wButtons, XINPUT_GAMEPAD_X, VirtualGamepadButton::X);
      UpdateButton(State.Buttons, state.Gamepad.wButtons, XINPUT_GAMEPAD_Y, VirtualGamepadButton::Y);
      UpdateButton(State.Buttons, state.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER, VirtualGamepadButton::LeftShoulder);
      UpdateButton(State.Buttons, state.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER, VirtualGamepadButton::RightShoulder);
      UpdateButton(State.Buttons, state.Gamepad.wButtons, XINPUT_GAMEPAD_START, VirtualGamepadButton::Start);
      UpdateButton(State.Buttons, state.Gamepad.wButtons, XINPUT_GAMEPAD_BACK, VirtualGamepadButton::Back);
      UpdateButton(State.Buttons, state.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB, VirtualGamepadButton::LeftThumb);
      UpdateButton(State.Buttons, state.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB, VirtualGamepadButton::RightThumb);
      UpdateButton(State.Buttons, state.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP, VirtualGamepadButton::DpadUp);
      UpdateButton(State.Buttons, state.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN, VirtualGamepadButton::DpadDown);
      UpdateButton(State.Buttons, state.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT, VirtualGamepadButton::DpadLeft);
      UpdateButton(State.Buttons, state.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT, VirtualGamepadButton::DpadRight);
    }
  };

  namespace
  {
    const uint32_t MAGIC_DEFAULT_DPI = 96;

    inline void SendButtonEventIfChanged(const std::shared_ptr<INativeWindowEventQueue>& eventQueue, const uint32_t deviceId,
                                         const VirtualGamepadState& oldState, const VirtualGamepadState& newState, const VirtualGamepadButton button,
                                         const VirtualKey::Enum key)
    {
      const bool isPressed = newState.IsPressed(button);
      if (oldState.IsPressed(button) == isPressed)
      {
        return;
      }

      const NativeWindowEvent event = NativeWindowEventHelper::EncodeInputKeyEvent(key, isPressed);
      eventQueue->PostEvent(event);
    }


    void SendKeyboardGamepadEvents(const std::shared_ptr<INativeWindowEventQueue>& eventQueue, const uint32_t deviceId,
                                   const VirtualGamepadState& oldState, const VirtualGamepadState& newState)
    {
      SendButtonEventIfChanged(eventQueue, deviceId, oldState, newState, VirtualGamepadButton::A, VirtualKey::GamePadButtonA);
      SendButtonEventIfChanged(eventQueue, deviceId, oldState, newState, VirtualGamepadButton::B, VirtualKey::GamePadButtonB);
      SendButtonEventIfChanged(eventQueue, deviceId, oldState, newState, VirtualGamepadButton::X, VirtualKey::GamePadButtonX);
      SendButtonEventIfChanged(eventQueue, deviceId, oldState, newState, VirtualGamepadButton::Y, VirtualKey::GamePadButtonY);
      SendButtonEventIfChanged(eventQueue, deviceId, oldState, newState, VirtualGamepadButton::LeftShoulder, VirtualKey::GamePadButtonLeftShoulder);
      SendButtonEventIfChanged(eventQueue, deviceId, oldState, newState, VirtualGamepadButton::RightShoulder, VirtualKey::GamePadButtonRightShoulder);
      SendButtonEventIfChanged(eventQueue, deviceId, oldState, newState, VirtualGamepadButton::Start, VirtualKey::GamePadButtonStart);
      SendButtonEventIfChanged(eventQueue, deviceId, oldState, newState, VirtualGamepadButton::Back, VirtualKey::GamePadButtonBack);
      SendButtonEventIfChanged(eventQueue, deviceId, oldState, newState, VirtualGamepadButton::LeftThumb, VirtualKey::GamePadButtonLeftThumb);
      SendButtonEventIfChanged(eventQueue, deviceId, oldState, newState, VirtualGamepadButton::RightThumb, VirtualKey::GamePadButtonRightThumb);
      SendButtonEventIfChanged(eventQueue, deviceId, oldState, newState, VirtualGamepadButton::DpadUp, VirtualKey::GamePadDpadUp);
      SendButtonEventIfChanged(eventQueue, deviceId, oldState, newState, VirtualGamepadButton::DpadDown, VirtualKey::GamePadDpadDown);
      SendButtonEventIfChanged(eventQueue, deviceId, oldState, newState, VirtualGamepadButton::DpadLeft, VirtualKey::GamePadDpadLeft);
      SendButtonEventIfChanged(eventQueue, deviceId, oldState, newState, VirtualGamepadButton::DpadRight, VirtualKey::GamePadDpadRight);
    }


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
        // case VK_SNAPSHOT:
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
  }    // namespace

  class PlatformNativeWindowSystemWin32State
  {
    struct WindowRecord
    {
      std::weak_ptr<PlatformNativeWindowWin32> Window;

      WindowRecord() = default;

      WindowRecord(const std::weak_ptr<PlatformNativeWindowWin32>& window)
        : Window(window)
      {
      }
    };

    std::weak_ptr<INativeWindowEventQueue> m_eventQueue;
    bool m_forceActivated;
    bool m_activated;
    uint32_t m_mouseButtonState;
    std::deque<WindowRecord> m_activeWindows;

  public:
    PlatformNativeWindowSystemWin32State(const std::weak_ptr<INativeWindowEventQueue>& eventQueue)
      : m_eventQueue(eventQueue)
      , m_forceActivated(USE_FORCE_ACTIVATED)
      , m_activated(false)
      , m_mouseButtonState(0)
    {
    }

    void SetUseForceActivated(const bool useForceActivated)
    {
      m_forceActivated = useForceActivated;
    }

    void AddWindow(const std::weak_ptr<PlatformNativeWindowWin32>& window)
    {
      // Garbage collect all dead windows
      auto itr = m_activeWindows.begin();
      while (itr != m_activeWindows.end())
      {
        if (itr->Window.expired())
        {
          itr = m_activeWindows.erase(itr);
        }
        else
        {
          ++itr;
        }
      }

      m_activeWindows.emplace_back(window);
    }


    std::weak_ptr<INativeWindowEventQueue> GetEventQueue() const
    {
      return m_eventQueue;
    }

    std::shared_ptr<PlatformNativeWindowWin32> TryGetWindow(HWND hWnd) const
    {
      auto compareFunc = [hWnd](const WindowRecord& val) {
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


    LRESULT OnMouseButton(HWND hWnd, const std::shared_ptr<INativeWindowEventQueue>& eventQueue, WPARAM wParam, LPARAM lParam,
                          const VirtualMouseButton::Enum button, const bool isPressed)
    {
      const Point2 position(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
      const NativeWindowEvent event = NativeWindowEventHelper::EncodeInputMouseButtonEvent(button, isPressed, position);
      eventQueue->PostEvent(event);

      if (isPressed)
      {
        m_mouseButtonState |= button;
      }
      else
      {
        m_mouseButtonState &= ~button;
      }

      auto window = TryGetWindow(hWnd);
      if (window)
      {
        window->SYS_SetMouseCapture(m_mouseButtonState != 0);
      }
      return 0;
    }


    LRESULT OnMouseMove(HWND hWnd, const std::shared_ptr<INativeWindowEventQueue>& eventQueue, WPARAM wParam, LPARAM lParam)
    {
      const Point2 position(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

      auto window = TryGetWindow(hWnd);
      if (window)
      {
        window->OnMouseMove(eventQueue, position);
      }
      return 0;
    }


    LRESULT OnMouseWheel(HWND hWnd, const std::shared_ptr<INativeWindowEventQueue>& eventQueue, WPARAM wParam, LPARAM lParam)
    {
      const int32_t zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

      // Unlike the other coordinates, this is in screen space instead, so we need to transform
      POINT pt;
      pt.x = GET_X_LPARAM(lParam);
      pt.y = GET_Y_LPARAM(lParam);
      if (ScreenToClient(hWnd, &pt) != 0)
      {
        const Point2 position(pt.x, pt.y);

        const NativeWindowEvent event = NativeWindowEventHelper::EncodeInputMouseWheelEvent(zDelta, position);
        eventQueue->PostEvent(event);
        // FSLLOG("WHEEL: X: " << position.X << " Y: " << position.Y);
      }
      else
      {
        FSLLOG_ERROR("Failed to transform mouse wheel position to client space");
      }
      return 0;
    }


    void OnRawInput(HWND hWnd, const std::shared_ptr<INativeWindowEventQueue>& eventQueue, WPARAM wParam, LPARAM lParam)
    {
      if (!m_activated)
      {
        return;
      }

      auto window = TryGetWindow(hWnd);
      if (window)
      {
        window->OnRawInput(eventQueue, lParam);
      }
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
      // const bool isMinimize = hiWord != 0;
      // if (isMinimize && lowWord != 0)
      //  eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowSuspendEvent(false));

      // Ensure that we only send state changes
      const bool activated = (lowWord != 0 || m_forceActivated);
      if (activated != m_activated)
      {
        m_activated = activated;
        eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowActivationEvent(activated));
      }


      // When the window is minimized we send the suspend message
      // if (isMinimize && lowWord == 0)
      //  eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowSuspendEvent(true));
      return 0;
    }


    LRESULT OnSize(HWND hWnd, const std::shared_ptr<INativeWindowEventQueue>& eventQueue, WPARAM wParam, LPARAM lParam)
    {
      // The window has been resized, but neither the SIZE_MINIMIZED nor SIZE_MAXIMIZED value applies.
      if (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED)
      {
        eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowResizedEvent());
      }
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
      {
        window->OnDPIChanged(Point2(newDpiX, newDpiY));
      }

      // A pointer to a RECT structure that provides a suggested size and position of the current window scaled for the new DPI.
      // The expectation is that apps will reposition and resize windows based on the suggestions provided by lParam when handling this message.
      // LPRECT lprcNewScale;
      // lprcNewScale = (LPRECT)lParam;
      // const auto newWidth = lprcNewScale->right - lprcNewScale->left;
      // const auto newHeight = lprcNewScale->bottom - lprcNewScale->top;
      //// Resize the window for the new DPI setting
      // SetWindowPos(hWnd, HWND_TOP, lprcNewScale->left, lprcNewScale->top, newWidth, newHeight, SWP_NOZORDER | SWP_NOACTIVATE);
      // Render the window contents again
      // RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
      return 1;
    }

    void OnWindowCaptureChanged(HWND hWnd, WPARAM wParam, LPARAM lParam)
    {
      auto window = TryGetWindow(hWnd);
      if (window)
      {
        window->OnWindowCaptureChanged(reinterpret_cast<HWND>(lParam));
      }
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
      if (windowSystemState)
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
          case WM_INPUT:
            windowSystemState->OnRawInput(hWnd, eventQueue, wParam, lParam);
            break;
          case WM_SIZE:
            return windowSystemState->OnSize(hWnd, eventQueue, wParam, lParam);
          case WM_DPICHANGED:
            return windowSystemState->OnDPICHanged(hWnd, wParam, lParam);
          case WM_CAPTURECHANGED:
            windowSystemState->OnWindowCaptureChanged(hWnd, wParam, lParam);
            break;
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
      if (IsWindow(hwnd) != 0)
      {
        auto dwStyle = static_cast<DWORD>(GetWindowLongPtr(hwnd, GWL_STYLE));
        auto dwExStyle = static_cast<DWORD>(GetWindowLongPtr(hwnd, GWL_EXSTYLE));
        auto menu = GetMenu(hwnd);

        RECT rc = {0, 0, clientWidth, clientHeight};

        if (AdjustWindowRectEx(&rc, dwStyle, menu != nullptr ? TRUE : FALSE, dwExStyle) == 0)
        {
          MessageBox(nullptr, TEXT("AdjustWindowRectEx Failed!"), TEXT("Error"), MB_OK);
        }
        SetWindowPos(hwnd, nullptr, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOMOVE);
      }
    }

    struct MonitorRecord
    {
      HMONITOR Handle{nullptr};
      Rectangle Rect;

      MonitorRecord() = default;

      MonitorRecord(HMONITOR hMonitor, const Rectangle& rect)
        : Handle(hMonitor)
        , Rect(rect)
      {
      }
    };

    std::deque<MonitorRecord> g_monitorDeque;

    BOOL CALLBACK MonitorEnumProc(_In_ HMONITOR hMonitor, _In_ HDC hdcMonitor, _In_ LPRECT lprcMonitor, _In_ LPARAM dwData)
    {
      if (hdcMonitor == nullptr && lprcMonitor != nullptr)
      {
        const int32_t left = lprcMonitor->left;
        const int32_t top = lprcMonitor->top;
        const int32_t dx = lprcMonitor->right - left;
        const int32_t dy = lprcMonitor->bottom - top;
        if (dx > 0 && dy > 0)
        {
          g_monitorDeque.emplace_back(hMonitor, Rectangle(left, top, dx, dy));
        }
      }
      return 1;
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
      int32_t maxDisplayId = 0;
      MonitorRecord record;
      if (!TryGetMonitor(requestedDisplayId, record, maxDisplayId))
      {
        FSLLOG_WARNING("Backend max display Id is: " << maxDisplayId << " so " << requestedDisplayId << " is unsupported, using displayid 0.");
        if (!TryGetMonitor(0, record, maxDisplayId))
        {
          throw GraphicsException("Could not find a valid monitor");
        }
      }
      return record;
    }


    std::shared_ptr<INativeWindow> AllocateWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
                                                  const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    {
      return std::make_shared<PlatformNativeWindowWin32>(nativeWindowSetup, windowParams, pPlatformCustomWindowAllocationParams);
    }
  }    // namespace


  static_assert(XUSER_MAX_COUNT <= 255, "The virtual gamepad only supports up to 255 devices");


  PlatformNativeWindowSystemWin32::PlatformNativeWindowSystemWin32(const NativeWindowSystemSetup& setup,
                                                                   const PlatformNativeWindowAllocationFunction& allocateWindowFunction,
                                                                   const PlatformNativeWindowSystemParams& systemParams)
    : PlatformNativeWindowSystem(setup, GetModuleHandle(nullptr))
    , m_allocationFunction(allocateWindowFunction ? allocateWindowFunction : AllocateWindow)
    , m_dpiHelper(std::make_shared<DPIHelperWin32>())
    , m_eventQueue(setup.GetEventQueue())
    , m_gamepadState(new std::vector<PlatformGamepadStateWin32>(XUSER_MAX_COUNT))
  {
    if (!g_windowSystemState.expired())
    {
      throw NotSupportedException("PlatformNativeWindowSystemWin32 is a singleton, please destroy any previously allocated instance first");
    }

    // Create the singleton state object
    auto windowSystemState = std::make_shared<PlatformNativeWindowSystemWin32State>(setup.GetEventQueue());
    m_windowSystemState = windowSystemState;
    g_windowSystemState = windowSystemState;

    auto eventQueue = m_eventQueue.lock();
    if (eventQueue)
    {
      const NativeWindowEvent event = NativeWindowEventHelper::EncodeGamepadConfiguration(static_cast<uint32_t>(m_gamepadState->size()));
      eventQueue->PostEvent(event);
    }
  }


  PlatformNativeWindowSystemWin32::~PlatformNativeWindowSystemWin32()
  {
    g_windowSystemState.reset();
  }


  std::shared_ptr<INativeWindow>
    PlatformNativeWindowSystemWin32::CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup,
                                                        const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
  {
    auto newBaseWindow =
      m_allocationFunction(nativeWindowSetup, PlatformNativeWindowParams(m_platformDisplay, m_dpiHelper), pPlatformCustomWindowAllocationParams);
    auto newWindow = std::dynamic_pointer_cast<PlatformNativeWindowWin32>(newBaseWindow);
    if (newWindow == nullptr)
    {
      throw NotSupportedException("The CreateNativeWindow method returned a unsupported window type");
    }

    m_windowSystemState->AddWindow(newWindow);
    return newWindow;
  }


  bool PlatformNativeWindowSystemWin32::ProcessMessages(const NativeWindowProcessMessagesArgs& args)
  {
    MSG sMessage;
    bool bQuit = false;
    while (PeekMessage(&sMessage, nullptr, 0, 0, PM_REMOVE) != 0)
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

    ScanGamepads();
    return !bQuit;
  }


  void PlatformNativeWindowSystemWin32::ScanGamepads()
  {
    auto eventQueue = m_eventQueue.lock();
    if (!eventQueue)
    {
      return;
    }

    assert(m_gamepadState);
    assert(m_gamepadState->size() == XUSER_MAX_COUNT);

    auto& rGamepadState = *m_gamepadState;
    for (DWORD deviceIndex = 0; deviceIndex < XUSER_MAX_COUNT; ++deviceIndex)
    {
      bool isConnected;
      XINPUT_STATE state{};
      // Get the gamepad input state
      if (XInputGetState(deviceIndex, &state) == ERROR_SUCCESS)
      {
        // Controller is connected
        isConnected = true;
      }
      else
      {
        // Controller is not connected
        isConnected = false;
        state = XINPUT_STATE{};
      }

      auto oldState = rGamepadState[deviceIndex];
      const auto newState = PlatformGamepadStateWin32(deviceIndex, isConnected, state);
      rGamepadState[deviceIndex] = newState;

      if (oldState.State != newState.State)
      {
        // FSLLOG("S" << newState.State.IsConnected << ", " << (int)newState.State.Buttons << ", " << (int)newState.State.LeftTrigger << ", " <<
        // (int)newState.State.RightTrigger << ", " << newState.State.LeftThumbX << ", " << newState.State.LeftThumbY << ", " <<
        // newState.State.RightThumbX << ", " << newState.State.RightThumbY);

        const NativeWindowEvent event = NativeWindowEventHelper::EncodeVirtualGamepadStateEvent(newState.State);
        eventQueue->PostEvent(event);

        SendKeyboardGamepadEvents(eventQueue, deviceIndex, oldState.State, newState.State);
      }
    }
  }


  PlatformNativeWindowWin32::PlatformNativeWindowWin32(const NativeWindowSetup& nativeWindowSetup,
                                                       const PlatformNativeWindowParams& platformWindowParams,
                                                       const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    : PlatformNativeWindow(nativeWindowSetup, platformWindowParams, pPlatformCustomWindowAllocationParams)
    , m_dpiHelper(platformWindowParams.DpiHelper)
    , m_mouseCaptureEnabled(false)
    , m_mouseInternalCaptureEnabled(false)
    , m_mouseIsCaptured(false)
    , m_mouseCursorIsClipped(false)
    , m_mouseHideCursorEnabled(false)
    , m_mouseIsCursorHidden(false)
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

      FSLLOG_IF(nativeWindowSetup.GetVerbosityLevel() > 0, "PlatformNativeWindowWin32: Creating fullscreen window: " << targetRectangle);
    }
    break;
    case WindowMode::Window:
      FSLLOG_IF(nativeWindowSetup.GetVerbosityLevel() > 0, "PlatformNativeWindowWin32: Creating window: " << targetRectangle);
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

    const auto className = TEXT("PlatformNativeWindowWin32");

    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = static_cast<WNDPROC>(WndProc);
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = className;

    auto icon = static_cast<HICON>(LoadImage(nullptr, TEXT("FslSDKIcon.ico"), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE));
    if (icon == nullptr)
    {
      wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    }
    else
    {
      wc.hIcon = icon;
    }

    RegisterClass(&wc);

    const auto applicationName = PlatformWin32::Widen(nativeWindowSetup.GetApplicationName());
    m_platformWindow = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, className, applicationName.c_str(), dwStyle, targetRectangle.Left(),
                                      targetRectangle.Top(), targetRectangle.Width(), targetRectangle.Height(), nullptr, nullptr, hInstance, nullptr);
    if (m_platformWindow == nullptr)
    {
      throw GraphicsException("Failed to create native window");
    }

    // if (bFullscreen)
    //  SetWindowLong(m_platformWindow, GWL_STYLE, 0);

    DoSetClientSize(m_platformWindow, targetRectangle.Width(), targetRectangle.Height());

    ShowWindow(m_platformWindow, SW_SHOW);
    SetForegroundWindow(m_platformWindow);
    SetFocus(m_platformWindow);

    if (!m_dpiHelper->TryGetDPI(m_platformWindow, m_cachedDPIValue))
    {
      FSLLOG_DEBUG_WARNING("Failed to cache DPI value, using default");
      m_cachedDPIValue = Point2(MAGIC_DEFAULT_DPI, MAGIC_DEFAULT_DPI);
    }

    // Register for raw input
    {
      RAWINPUTDEVICE rid[1]{};
      rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
      rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
      rid[0].dwFlags = RIDEV_INPUTSINK;
      rid[0].hwndTarget = m_platformWindow;
      RegisterRawInputDevices(rid, 1, sizeof(RAWINPUTDEVICE));
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
    RECT rect{};
    if (GetClientRect(m_platformWindow, &rect) == 0)
    {
      rSize = Point2();
      return false;
    }
    rSize = Point2(rect.right - rect.left, rect.bottom - rect.top);
    return true;
  }


  bool PlatformNativeWindowWin32::TryCaptureMouse(const bool enableCapture)
  {
    if (m_mouseCaptureEnabled == enableCapture)
    {
      return true;
    }

    m_mouseCaptureEnabled = enableCapture;
    m_mouseHideCursorEnabled = enableCapture;
    VERBOSE_LOG("* CaptureMouse: " << enableCapture);

    auto windowSystemState = g_windowSystemState.lock();
    if (windowSystemState)
    {
      windowSystemState->SetUseForceActivated(enableCapture ? false : USE_FORCE_ACTIVATED);
    }


    ResolveMouseCapture();
    return true;
  }


  void PlatformNativeWindowWin32::OnDPIChanged(const Point2& value)
  {
    if (value == m_cachedDPIValue)
    {
      return;
    }

    m_cachedDPIValue = value;

    auto eventQueue = TryGetEventQueue();
    if (eventQueue)
    {
      eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowDPIChanged());
    }
  }


  void PlatformNativeWindowWin32::OnRawInput(const std::shared_ptr<INativeWindowEventQueue>& eventQueue, const LPARAM lParam)
  {
    const auto hRawInput = reinterpret_cast<HRAWINPUT>(lParam);

    UINT requiredSize;
    if (GetRawInputData(hRawInput, RID_INPUT, nullptr, &requiredSize, sizeof(RAWINPUTHEADER)) != 0)
    {
      return;
    }

    if (requiredSize > m_rawInputScratchpad.size())
    {
      m_rawInputScratchpad.resize(requiredSize);
    }

    const auto bytesWritten = GetRawInputData(hRawInput, RID_INPUT, m_rawInputScratchpad.data(), &requiredSize, sizeof(RAWINPUTHEADER));
    if (bytesWritten != sizeof(RAWINPUT))
    {
      return;
    }

    const auto pRawInput = reinterpret_cast<const RAWINPUT*>(m_rawInputScratchpad.data());
    if (pRawInput->header.dwType == RIM_TYPEMOUSE)
    {
      const int32_t deltaX = CapValue(pRawInput->data.mouse.lLastX, "x");
      const int32_t deltaY = CapValue(pRawInput->data.mouse.lLastY, "y");

      uint32_t buttonFlags = 0;
      if ((pRawInput->data.mouse.ulRawButtons & RI_MOUSE_LEFT_BUTTON_DOWN) != 0u)
      {
        m_rawMouseButtonFlags.SetFlag(VirtualMouseButton::Left, true);
      }
      else if ((pRawInput->data.mouse.ulRawButtons & RI_MOUSE_LEFT_BUTTON_UP) != 0u)
      {
        m_rawMouseButtonFlags.SetFlag(VirtualMouseButton::Left, false);
      }

      if ((pRawInput->data.mouse.ulRawButtons & RI_MOUSE_MIDDLE_BUTTON_DOWN) != 0u)
      {
        m_rawMouseButtonFlags.SetFlag(VirtualMouseButton::Middle, true);
      }
      else if ((pRawInput->data.mouse.ulRawButtons & RI_MOUSE_MIDDLE_BUTTON_UP) != 0u)
      {
        m_rawMouseButtonFlags.SetFlag(VirtualMouseButton::Middle, false);
      }

      if ((pRawInput->data.mouse.ulRawButtons & RI_MOUSE_RIGHT_BUTTON_DOWN) != 0u)
      {
        m_rawMouseButtonFlags.SetFlag(VirtualMouseButton::Right, true);
      }
      else if ((pRawInput->data.mouse.ulRawButtons & RI_MOUSE_RIGHT_BUTTON_UP) != 0u)
      {
        m_rawMouseButtonFlags.SetFlag(VirtualMouseButton::Right, false);
      }

      Point2 position(deltaX, deltaY);
      const NativeWindowEvent event = NativeWindowEventHelper::EncodeInputRawMouseMoveEvent(position, m_rawMouseButtonFlags);
      eventQueue->PostEvent(event);
    }
  }


  void PlatformNativeWindowWin32::OnMouseMove(const std::shared_ptr<INativeWindowEventQueue>& eventQueue, const Point2& position)
  {
    const NativeWindowEvent event = NativeWindowEventHelper::EncodeInputMouseMoveEvent(position);
    eventQueue->PostEvent(event);
    // FSLLOG("MOVE: X: " << position.X << " Y: " << position.Y);

    if (!m_mouseCaptureEnabled)
    {
      return;
    }

    ResolveMouseCapture();
  }


  void PlatformNativeWindowWin32::OnWindowCaptureChanged(const HWND newCaptureHwnd)
  {
    VERBOSE_LOG("- OnWindowCaptureChanged");
    VERBOSE_LOG_IF(m_mouseIsCaptured && newCaptureHwnd != m_platformWindow, "* Mouse captured force lost");


    m_mouseIsCaptured = (newCaptureHwnd == m_platformWindow);
    if (m_mouseIsCaptured)
    {
      return;
    }

    if (m_mouseCursorIsClipped)
    {
      m_mouseCursorIsClipped = false;
      VERBOSE_LOG("* Cursor not clipped to window");
    }

    if (m_mouseIsCursorHidden)
    {
      m_mouseIsCursorHidden = false;
      ShowCursor(1);
      VERBOSE_LOG("* Showing cursor");
    }
  }


  void PlatformNativeWindowWin32::SYS_SetMouseCapture(const bool enableCapture)
  {
    m_mouseInternalCaptureEnabled = enableCapture;
    ResolveMouseCapture();
  }


  void PlatformNativeWindowWin32::ResolveMouseCapture()
  {
    const bool enableCapture = m_mouseCaptureEnabled || m_mouseInternalCaptureEnabled;

    if (m_mouseIsCaptured == enableCapture && m_mouseCursorIsClipped == m_mouseCaptureEnabled && m_mouseHideCursorEnabled == m_mouseIsCursorHidden)
    {
      return;
    }

    if (GetForegroundWindow() != m_platformWindow)
    {
      return;
    }


    ResolveMouseCursorClip();
    ResolveMouseCursorVisible();

    if (m_mouseIsCaptured == enableCapture)
    {
      return;
    }
    VERBOSE_LOG("- ResolveMouseCapture");


    if (enableCapture)
    {
      SetCapture(m_platformWindow);
      VERBOSE_LOG("* Mouse captured");
      m_mouseIsCaptured = true;
    }
    else
    {
      ReleaseCapture();
      VERBOSE_LOG("* Mouse capture released");
      m_mouseIsCaptured = false;
    }
  }

  void PlatformNativeWindowWin32::ResolveMouseCursorClip()
  {
    if (m_mouseCursorIsClipped == m_mouseCaptureEnabled)
    {
      return;
    }


    VERBOSE_LOG("- ResolveMouseCursorClip");

    if (m_mouseCaptureEnabled)
    {
      RECT rect{};
      if (GetClientRect(m_platformWindow, &rect) == 0)
      {
        FSLLOG_DEBUG_WARNING("Failed to get client rect");
        return;
      }

      // clamp the mouse to the center pixel of the window
      rect.left = (rect.right - rect.left) / 2;
      rect.top = (rect.bottom - rect.top) / 2;
      rect.right = rect.left;
      rect.bottom = rect.top;
      // SetCursorPos(rect.left, rect.top);

      if (!TryToScreenSpace(m_platformWindow, rect) || (ClipCursor(&rect) == 0))
      {
        FSLLOG_DEBUG_WARNING("Failed to ClipCursor to window");
        return;
      }
      m_mouseCursorIsClipped = true;
      VERBOSE_LOG("* Cursor clipped to window");
    }
    else
    {
      ClipCursor(nullptr);
      m_mouseCursorIsClipped = false;
      VERBOSE_LOG("* Cursor not clipped to window");
    }
  }


  void PlatformNativeWindowWin32::ResolveMouseCursorVisible()
  {
    if (m_mouseHideCursorEnabled == m_mouseIsCursorHidden)
    {
      return;
    }

    VERBOSE_LOG("- ResolveMouseCursorVisible");

    VERBOSE_LOG_IF(m_mouseHideCursorEnabled, "* Hiding cursor");
    VERBOSE_LOG_IF(!m_mouseHideCursorEnabled, "* Showing cursor");

    ShowCursor(static_cast<BOOL>(!m_mouseHideCursorEnabled));
    m_mouseIsCursorHidden = m_mouseHideCursorEnabled;
  }
}    // namespace Fsl
#endif
