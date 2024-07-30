#ifdef FSL_WINDOWSYSTEM_SDL2
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslNativeWindow/Base/INativeWindowEventQueue.hpp>
#include <FslNativeWindow/Base/NativeWindowEventHelper.hpp>
#include <FslNativeWindow/Base/NativeWindowSetup.hpp>
#include <FslNativeWindow/Base/NativeWindowSystemSetup.hpp>
#include <FslNativeWindow/Base/VirtualGamepadState.hpp>
#include <FslNativeWindow/Platform/Adapter/SDL/PlatformNativeWindowAdapterSDL.hpp>
#include <FslNativeWindow/Platform/Adapter/SDL/PlatformNativeWindowSystemAdapterSDL.hpp>
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_syswm.h>
#include <fmt/format.h>
#include <algorithm>
#include <optional>

#if 0
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define VERBOSE_LOG(X) FSLLOG3_VERBOSE2(X)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define VERBOSE_LOG_IF(cONDITION, X) FSLLOG3_VERBOSE2_IF(cONDITION, X)
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define VERBOSE_LOG(X) \
  {                    \
  }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define VERBOSE_LOG_IF(cONDITION, X) \
  {                                  \
  }
#endif

namespace Fsl
{
  namespace
  {
    struct PlatformNativeWindowRecord
    {
      PlatformNativeDisplayType DisplayType{};
      PlatformNativeWindowType WindowType{};

      PlatformNativeWindowRecord() = default;
      PlatformNativeWindowRecord(PlatformNativeDisplayType displayType, PlatformNativeWindowType windowType)
        : DisplayType(displayType)
        , WindowType(windowType)
      {
      }
    };

#ifndef SDL_VIDEO_DRIVER_EMSCRIPTEN
    PlatformNativeWindowRecord ExtractNativeWindowRecord(SDL_SysWMinfo systemInfo)
    {
#ifdef SDL_VIDEO_DRIVER_WINDOWS
      return {systemInfo.info.win.hinstance, systemInfo.info.win.window};
#elif defined(SDL_VIDEO_DRIVER_X11)
      return {systemInfo.info.x11.display, systemInfo.info.x11.window};
#elif defined(SDL_VIDEO_DRIVER_WAYLAND)
      return {systemInfo.info.wl.display, systemInfo.info.wl.surface};
#else
#error Unsupported SDL platform
#endif
    }
#endif


    std::optional<VirtualKey::Enum> TryConvert(SDL_Keysym keysym)
    {
      const SDL_Keycode key = keysym.sym;
      switch (key)
      {
      case SDLK_ESCAPE:
        return VirtualKey::Escape;
      case SDLK_TAB:
        return VirtualKey::Tab;
      case SDLK_RETURN:
        return VirtualKey::Return;
      case SDLK_SPACE:
        return VirtualKey::Space;
      case SDLK_BACKSPACE:
        return VirtualKey::Backspace;

      case SDLK_UP:
        return VirtualKey::UpArrow;
      case SDLK_DOWN:
        return VirtualKey::DownArrow;
      case SDLK_LEFT:
        return VirtualKey::LeftArrow;
      case SDLK_RIGHT:
        return VirtualKey::RightArrow;

      case SDLK_INSERT:
        return VirtualKey::Insert;
      case SDLK_DELETE:
        return VirtualKey::Delete;
      case SDLK_HOME:
        return VirtualKey::Home;
      case SDLK_END:
        return VirtualKey::End;
      case SDLK_PAGEUP:
        return VirtualKey::PageUp;
      case SDLK_PAGEDOWN:
        return VirtualKey::PageDown;
      case SDLK_SCROLLLOCK:
        return VirtualKey::ScrollLock;
      case SDLK_KP_MEMADD:
        return VirtualKey::Add;
      case SDLK_KP_MEMSUBTRACT:
        return VirtualKey::Subtract;
      default:
        break;
      }

      if (key >= SDLK_0 && key <= SDLK_9)
      {
        // Take advantage of the simple encoding
        return static_cast<VirtualKey::Enum>(VirtualKey::Code0 + (key - SDLK_0));
      }
      if (key >= SDLK_a && key <= SDLK_z)
      {
        // Take advantage of the simple encoding
        return static_cast<VirtualKey::Enum>(VirtualKey::A + (key - SDLK_a));
      }
      if (key >= SDLK_F1 && key <= SDLK_F12)
      {
        return static_cast<VirtualKey::Enum>(VirtualKey::F1 + (key - SDLK_F1));
      }
      return {};
    }

    std::optional<VirtualMouseButton> TryConvertMouseButton(const Uint8 button)
    {
      switch (button)
      {
      case SDL_BUTTON_LEFT:
        return VirtualMouseButton::Left;
      case SDL_BUTTON_MIDDLE:
        return VirtualMouseButton::Middle;
      case SDL_BUTTON_RIGHT:
        return VirtualMouseButton::Right;
      case SDL_BUTTON_X1:
        return VirtualMouseButton::X1;
      case SDL_BUTTON_X2:
        return VirtualMouseButton::X2;
      default:
        return {};
      }
    }


    PxSize2D DoGetWindowSize(SDL_Window* pSdlWindow)
    {
      assert(pSdlWindow != nullptr);
      // Query the window size
      int width{0};
      int height{0};
      SDL_GetWindowSize(pSdlWindow, &width, &height);
      return PxSize2D::Create(width, height);
    }


    std::optional<Vector2> TryDoGetDpi(const int displayIndex)
    {
      if (displayIndex >= 0)
      {
        float diagonalDpi{0.0f};
        float horizontalDpi{0.0f};
        float verticalDpi{0.0f};
        if (SDL_GetDisplayDPI(displayIndex, &diagonalDpi, &horizontalDpi, &verticalDpi) == 0)
        {
          return Vector2(horizontalDpi, verticalDpi);
        }
        FSLLOG3_WARNING("SDL failed to get display dpi: {}", SDL_GetError());
      }
      else
      {
        FSLLOG3_WARNING("SDL failed to get display dpi due to invalid display index");
      }
      return {};
    }

    std::shared_ptr<IPlatformNativeWindowAdapter>
      AllocateWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
                     const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    {
      return std::make_shared<PlatformNativeWindowAdapterSDL>(nativeWindowSetup, windowParams, pPlatformCustomWindowAllocationParams);
    }
  }


  PlatformNativeWindowSystemAdapterSDL::WindowRecord::WindowRecord(std::weak_ptr<PlatformNativeWindowAdapterSDL> window, const Uint32 windowId)
    : Window(std::move(window))
    , WindowId(windowId)
  {
  }

  PlatformNativeWindowSystemAdapterSDL::WindowRecord::~WindowRecord() = default;


  PlatformNativeWindowSystemAdapterSDL::PlatformNativeWindowSystemAdapterSDL(const NativeWindowSystemSetup& setup,
                                                                             const PlatformNativeWindowAllocationFunction& allocateWindowFunction,
                                                                             const PlatformNativeWindowSystemParams& systemParams)
    : PlatformNativeWindowSystemAdapter(setup, {})
    , m_allocationFunction(allocateWindowFunction ? allocateWindowFunction : AllocateWindow)
    , m_eventQueue(setup.GetEventQueue())
  {
    try
    {
      FSL_PARAM_NOT_USED(systemParams);

      if (SDL_Init(SDL_INIT_VIDEO) < 0)
      {
        throw GraphicsException(fmt::format("SDL_Init failed with {}", SDL_GetError()));
      }

      if (systemParams.UseExternalContext)
      {
        SDL_SetHint(SDL_HINT_VIDEO_EXTERNAL_CONTEXT, "1");
      }

      auto eventQueue = m_eventQueue.lock();
      if (eventQueue)
      {
        const NativeWindowEvent event = NativeWindowEventHelper::EncodeGamepadConfiguration(static_cast<uint32_t>(0));
        eventQueue->PostEvent(event);
      }
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_WARNING("PlatformNativeWindowSystemAdapterSDL threw exception {}", ex.what());
      Dispose();
      throw;
    }
  }


  PlatformNativeWindowSystemAdapterSDL::~PlatformNativeWindowSystemAdapterSDL()
  {
    Dispose();
  }


  std::shared_ptr<IPlatformNativeWindowAdapter>
    PlatformNativeWindowSystemAdapterSDL::CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup,
                                                             const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
  {
    auto newBaseWindow =
      m_allocationFunction(nativeWindowSetup, PlatformNativeWindowParams(m_platformDisplay, {}), pPlatformCustomWindowAllocationParams);
    auto newWindow = std::dynamic_pointer_cast<PlatformNativeWindowAdapterSDL>(newBaseWindow);
    if (newWindow == nullptr)
    {
      throw NotSupportedException("The CreateNativeWindow method returned a unsupported window type");
    }
    const Uint32 windowId = SDL_GetWindowID(newWindow->GetSdlWindow());
    if (windowId == 0)
    {
      throw NotSupportedException(fmt::format("The CreateNativeWindow {}", SDL_GetError()));
    }

    m_windows.emplace_back(newWindow, windowId);
    return newWindow;
  }


  bool PlatformNativeWindowSystemAdapterSDL::ProcessMessages(const NativeWindowProcessMessagesArgs& args)
  {
    auto eventQueue = m_eventQueue.lock();
    INativeWindowEventQueue* pEventQueue = eventQueue.get();

    bool quitRequest = false;
    SDL_Event event{};
    while (SDL_PollEvent(&event) != 0)
    {
      if (pEventQueue != nullptr)
      {
        switch (event.type)
        {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
          OnKeyEvent(*pEventQueue, event.key, event.type == SDL_KEYDOWN);
          break;
        case SDL_MOUSEMOTION:
          OnMouseMotion(*pEventQueue, event.motion);
          break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
          OnMouseButton(*pEventQueue, event.button);
          break;
        case SDL_MOUSEWHEEL:
          OnMouseWheel(*pEventQueue, event.wheel);
          break;
        case SDL_WINDOWEVENT:
          OnWindowEvent(*pEventQueue, event.window);
          break;
        default:
          break;
        }
      }

      switch (event.type)
      {
      case SDL_QUIT:
        quitRequest = true;
        break;
      default:
        break;
      }
    }
    return !quitRequest;
  }


  void PlatformNativeWindowSystemAdapterSDL::Dispose() noexcept
  {
    for (auto& rEntry : m_windows)
    {
      auto window = rEntry.Window.lock();
      if (window)
      {
        window->Dispose();
      }
    }
    m_windows.clear();
    SDL_Quit();
  }


  void PlatformNativeWindowSystemAdapterSDL::OnKeyEvent(INativeWindowEventQueue& eventQueue, const SDL_KeyboardEvent& theEvent, const bool pressed)
  {
    std::optional<VirtualKey::Enum> keycode = TryConvert(theEvent.keysym);
    if (keycode.has_value())
    {
      const NativeWindowEvent event = NativeWindowEventHelper::EncodeInputKeyEvent(keycode.value(), pressed);
      eventQueue.PostEvent(event);
    }
  }


  void PlatformNativeWindowSystemAdapterSDL::OnMouseMotion(INativeWindowEventQueue& eventQueue, const SDL_MouseMotionEvent& theEvent)
  {
    auto window = TryFindWindow(theEvent.windowID);
    if (window.has_value())
    {
      window.value()->OnMouseMotion(eventQueue, theEvent);
    }
  }


  void PlatformNativeWindowSystemAdapterSDL::OnMouseButton(INativeWindowEventQueue& eventQueue, const SDL_MouseButtonEvent& theEvent)
  {
    auto window = TryFindWindow(theEvent.windowID);
    if (window.has_value())
    {
      window.value()->OnMouseButton(eventQueue, theEvent);
    }
  }


  void PlatformNativeWindowSystemAdapterSDL::OnMouseWheel(INativeWindowEventQueue& eventQueue, const SDL_MouseWheelEvent& theEvent)
  {
    auto window = TryFindWindow(theEvent.windowID);
    if (window.has_value())
    {
      window.value()->OnMouseWheel(eventQueue, theEvent);
    }
  }


  void PlatformNativeWindowSystemAdapterSDL::OnWindowEvent(INativeWindowEventQueue& eventQueue, const SDL_WindowEvent& theEvent)
  {
    auto window = TryFindWindow(theEvent.windowID);
    if (window.has_value())
    {
      window.value()->OnWindowEvent(eventQueue, theEvent);
    }
  }

  std::optional<std::shared_ptr<PlatformNativeWindowAdapterSDL>> PlatformNativeWindowSystemAdapterSDL::TryFindWindow(const Uint32 windowId)
  {
    auto itrFind = std::find_if(m_windows.begin(), m_windows.end(), [windowId](const WindowRecord& entry) { return entry.WindowId == windowId; });
    if (itrFind != m_windows.end())
    {
      auto window = itrFind->Window.lock();
      if (window)
      {
        return window;
      }


      m_windows.erase(itrFind);
    }
    return {};
  }


  PlatformNativeWindowAdapterSDL::PlatformNativeWindowAdapterSDL(
    const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& platformWindowParams,
    const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    : PlatformNativeWindowAdapter(nativeWindowSetup, platformWindowParams, pPlatformCustomWindowAllocationParams,
                                  NativeWindowCapabilityFlags::CaptureMouse | NativeWindowCapabilityFlags::GetDpi)
  {
    try
    {    // Create the native window
      const NativeWindowConfig nativeWindowConfig = nativeWindowSetup.GetConfig();
      const Rectangle screenRect = nativeWindowConfig.GetWindowRectangle();

      int winRectX = SDL_WINDOWPOS_UNDEFINED;
      int winRectY = SDL_WINDOWPOS_UNDEFINED;
      int winRectWidth = screenRect.Width();
      int winRectHeight = screenRect.Height();

      int windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;
      switch (nativeWindowConfig.GetWindowMode())
      {
      case WindowMode::Window:
        windowFlags |= SDL_WINDOW_RESIZABLE;
        winRectX = screenRect.Left();
        winRectY = screenRect.Top() + 50;
        break;
      case WindowMode::Fullscreen:
        windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        break;
      default:
        throw NotSupportedException("Unsupported WindowMode");
      }

      windowFlags |= platformWindowParams.WindowFlags;

      m_pSdlWindow = SDL_CreateWindow(nativeWindowSetup.GetApplicationName().c_str(), winRectX, winRectY, winRectWidth, winRectHeight, windowFlags);
      if (m_pSdlWindow == nullptr)
      {
        throw GraphicsException(fmt::format("Failed to create SDL window: {}", SDL_GetError()));
      }

      {
#ifndef FSL_PLATFORM_EMSCRIPTEN
        SDL_SysWMinfo systemInfo{};
        SDL_VERSION(&systemInfo.version);
        if (SDL_GetWindowWMInfo(m_pSdlWindow, &systemInfo) != SDL_TRUE)
        {
          throw GraphicsException(fmt::format("Failed to get native SDL window: {}", SDL_GetError()));
        }
        auto nativeWindowRecord = ExtractNativeWindowRecord(systemInfo);
#else
        PlatformNativeWindowRecord nativeWindowRecord{};
#endif
        m_platformWindow = nativeWindowRecord.WindowType;
        m_platformDisplay = nativeWindowRecord.DisplayType;
      }


      m_cachedSize = DoGetWindowSize(m_pSdlWindow);
      {    // Query the display dpi
        int displayIndex = SDL_GetWindowDisplayIndex(m_pSdlWindow);
        m_cachedDpi = TryDoGetDpi(displayIndex);
      }
    }
    catch (const std::exception& ex)
    {
      FSLLOG3_WARNING("PlatformNativeWindowAdapterSDL threw exception {}", ex.what());
      Dispose();
      throw;
    }
  }


  PlatformNativeWindowAdapterSDL::~PlatformNativeWindowAdapterSDL()
  {
    Dispose();
  }


  bool PlatformNativeWindowAdapterSDL::TryCaptureMouse(const bool enableCapture)
  {
    FSLLOG3_WARNING("FIX: not implemented, TryCaptureMouse");
    return false;
  }


  void PlatformNativeWindowAdapterSDL::Dispose() noexcept
  {
    if (m_pSdlWindow != nullptr)
    {
      SDL_DestroyWindow(m_pSdlWindow);
      m_pSdlWindow = nullptr;
    }
  }

  void PlatformNativeWindowAdapterSDL::OnMouseMotion(INativeWindowEventQueue& eventQueue, const SDL_MouseMotionEvent& theEvent)
  {
    const PxPoint2 position(PxValue(theEvent.x), PxValue(theEvent.y));
    const NativeWindowEvent event = NativeWindowEventHelper::EncodeInputMouseMoveEvent(position);
    eventQueue.PostEvent(event);

    m_cachedMouse.LastPositionPx = position;
  }


  void PlatformNativeWindowAdapterSDL::OnMouseButton(INativeWindowEventQueue& eventQueue, const SDL_MouseButtonEvent& theEvent)
  {
    auto mouseButton = TryConvertMouseButton(theEvent.button);
    if (!mouseButton.has_value())
    {
      return;
    }

    const PxPoint2 position(PxValue(theEvent.x), PxValue(theEvent.y));
    const NativeWindowEvent event =
      NativeWindowEventHelper::EncodeInputMouseButtonEvent(mouseButton.value(), theEvent.type == SDL_MOUSEBUTTONDOWN, position);
    eventQueue.PostEvent(event);

    m_cachedMouse.LastPositionPx = position;
  }


  void PlatformNativeWindowAdapterSDL::OnMouseWheel(INativeWindowEventQueue& eventQueue, const SDL_MouseWheelEvent& theEvent)
  {
    const NativeWindowEvent event = NativeWindowEventHelper::EncodeInputMouseWheelEvent(theEvent.y, m_cachedMouse.LastPositionPx);
    eventQueue.PostEvent(event);
  }


  void PlatformNativeWindowAdapterSDL::OnWindowEvent(INativeWindowEventQueue& eventQueue, const SDL_WindowEvent& theEvent)
  {
    bool checkDpi = false;
    bool checkSize = false;
    bool newActivatedState = m_isActivated;

    // theEvent.windowID
    switch (theEvent.event)
    {
    case SDL_WINDOWEVENT_SHOWN:
      VERBOSE_LOG("SDL_WINDOWEVENT_SHOWN");
      newActivatedState = true;
      break;
    case SDL_WINDOWEVENT_HIDDEN:
      VERBOSE_LOG("SDL_WINDOWEVENT_HIDDEN");
      newActivatedState = false;
      break;
    case SDL_WINDOWEVENT_EXPOSED:
      VERBOSE_LOG("SDL_WINDOWEVENT_EXPOSED");
      break;
    case SDL_WINDOWEVENT_MOVED:
      checkDpi = true;
      break;
    case SDL_WINDOWEVENT_RESIZED:
      checkDpi = true;
      checkSize = true;
      break;
    case SDL_WINDOWEVENT_SIZE_CHANGED:
      checkDpi = true;
      checkSize = true;
      break;
    case SDL_WINDOWEVENT_MINIMIZED:
      VERBOSE_LOG("SDL_WINDOWEVENT_MINIMIZED");
      newActivatedState = false;
      break;
    case SDL_WINDOWEVENT_MAXIMIZED:
      VERBOSE_LOG("SDL_WINDOWEVENT_MAXIMIZED");
      break;
    case SDL_WINDOWEVENT_RESTORED:
      VERBOSE_LOG("SDL_WINDOWEVENT_RESTORED");
      newActivatedState = true;
      break;
    case SDL_WINDOWEVENT_ENTER:
      VERBOSE_LOG("SDL_WINDOWEVENT_ENTER");
      break;
    case SDL_WINDOWEVENT_LEAVE:
      VERBOSE_LOG("SDL_WINDOWEVENT_LEAVE");
      break;
    case SDL_WINDOWEVENT_FOCUS_GAINED:
      VERBOSE_LOG("SDL_WINDOWEVENT_FOCUS_GAINED");
      break;
    case SDL_WINDOWEVENT_FOCUS_LOST:
      VERBOSE_LOG("SDL_WINDOWEVENT_FOCUS_LOST");
      break;
    case SDL_WINDOWEVENT_CLOSE:
      VERBOSE_LOG("SDL_WINDOWEVENT_CLOSE");
      break;
    case SDL_WINDOWEVENT_TAKE_FOCUS:
      VERBOSE_LOG("SDL_WINDOWEVENT_TAKE_FOCUS");
      break;
    default:
      break;
    }

    if (m_isActivated != newActivatedState)
    {
      m_isActivated = newActivatedState;
      eventQueue.PostEvent(NativeWindowEventHelper::EncodeWindowActivationEvent(newActivatedState));
    }

    if (checkSize || checkDpi)
    {
      if (checkSize)
      {
        auto currentSize = DoGetWindowSize(m_pSdlWindow);
        if (currentSize != m_cachedSize)
        {
          m_cachedSize = currentSize;
          eventQueue.PostEvent(NativeWindowEventHelper::EncodeWindowResizedEvent());
        }
      }
      if (checkDpi)
      {
        auto currentDpi = TryDoGetDpi(SDL_GetWindowDisplayIndex(m_pSdlWindow));
        if (currentDpi != m_cachedDpi)
        {
          m_cachedDpi = currentDpi;
          eventQueue.PostEvent(NativeWindowEventHelper::EncodeWindowConfigChanged());
        }
      }
    }
  }


  bool PlatformNativeWindowAdapterSDL::TryGetNativeSize(PxPoint2& rSize) const
  {
    rSize = TypeConverter::To<PxPoint2>(m_cachedSize);
    return true;
  }


  bool PlatformNativeWindowAdapterSDL::TryGetNativeDpi(Vector2& rDPI) const
  {
    if (m_cachedDpi.has_value())
    {
      rDPI = m_cachedDpi.value();
      return true;
    }
    return false;
  }
}
#endif
