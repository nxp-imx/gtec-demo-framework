#ifdef __ANDROID__
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
#include <FslBase/Log/Math/LogRectangle.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslNativeWindow/Base/INativeWindowEventQueue.hpp>
#include <FslNativeWindow/Base/NativeWindowEventHelper.hpp>
#include <FslNativeWindow/Base/NativeWindowSetup.hpp>
#include <FslNativeWindow/Base/NativeWindowSystemSetup.hpp>
#include <FslNativeWindow/Platform/Adapter/Android/PlatformNativeWindowAdapterAndroid.hpp>
#include <FslNativeWindow/Platform/Adapter/Android/PlatformNativeWindowSystemAdapterAndroid.hpp>
#include <FslNativeWindow/Platform/Adapter/Android/PlatformNativeWindowSystemAndroidTag.hpp>
#include <Platform/Android/JNIUtil/JNIUtil.hpp>
#include <android/log.h>
#include <android/native_window_jni.h>
#include <android/sensor.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <algorithm>
#include <thread>

// #define LOCAL_DEBUG_THREADS 1
// #define LOCAL_DEBUG_INPUT 1

#ifdef LOCAL_DEBUG_THREADS
#include <fmt/ostream.h>
#define LOCAL_THREAD_PRINT(X) FSLLOG3_INFO("{} Current threadId: '{}'", (X), std::this_thread::get_id())
#else
#define LOCAL_THREAD_PRINT(X) \
  {                           \
  }
#endif

#ifdef LOCAL_DEBUG_INPUT
#include <fmt/ostream.h>
#define LOCAL_INPUT_PRINT(...) FSLLOG3_INFO(__VA_ARGS__)
#else
#define LOCAL_INPUT_PRINT(...) \
  {                            \
  }
#endif

namespace Fsl
{
  namespace
  {
    std::weak_ptr<INativeWindowEventQueue> g_eventQueue;

    ANativeWindow* g_hWindow = nullptr;
    bool g_isSuspended = false;
    bool g_isActivated = false;
    bool g_isTouchDown = false;

    void PostActivated(const std::shared_ptr<INativeWindowEventQueue>& eventQueue, bool activate)
    {
      if (activate != g_isActivated)
      {
        g_isActivated = activate;
        eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowActivationEvent(activate));
      }
    }


    void PostSuspend(const std::shared_ptr<INativeWindowEventQueue>& eventQueue, bool suspend)
    {
      if (suspend != g_isSuspended)
      {
        g_isSuspended = suspend;
        if (suspend && g_isActivated)
          PostActivated(eventQueue, false);

        eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowSuspendEvent(suspend));
      }
    }


    void CmdInitWindow(android_app* pAppState, const std::shared_ptr<INativeWindowEventQueue>& eventQueue)
    {
      // The window is being shown, get it ready.
      if (pAppState->window != nullptr)
      {
        FSLLOG3_VERBOSE("NativeWindow init");
        PostSuspend(eventQueue, false);
        g_hWindow = pAppState->window;
      }
      else
      {
        g_hWindow = nullptr;
        FSLLOG3_VERBOSE("No window pointer");
      }
    }


    void CmdShutdownWindow(const std::shared_ptr<INativeWindowEventQueue>& eventQueue)
    {
      // The window is being hidden or closed, clean it up.
      PostSuspend(eventQueue, true);
      g_hWindow = nullptr;
    }

    void CmdResizedWindow(const std::shared_ptr<INativeWindowEventQueue>& eventQueue)
    {
      eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowResizedEvent());
    }


    void CmdSetFocus(const std::shared_ptr<INativeWindowEventQueue>& eventQueue, const bool bEnabled)
    {
      PostActivated(eventQueue, bEnabled);
    }


    void CmdOnConfigChanged(android_app* pAppState, const std::shared_ptr<INativeWindowEventQueue>& eventQueue)
    {
      // Configuration was changed
      eventQueue->PostEvent(NativeWindowEventHelper::EncodeWindowConfigChanged());

      if (pAppState != nullptr && pAppState->userData != nullptr)
      {
        auto pNativeWindow = static_cast<PlatformNativeWindowAdapterAndroid*>(pAppState->userData);
        pNativeWindow->SYS_OnConfigChanged();
      }
    }


    void CmdLowMemory(const std::shared_ptr<INativeWindowEventQueue>& eventQueue)
    {
      eventQueue->PostEvent(NativeWindowEventHelper::EncodeLowMemoryEvent());
    }


    void CmdCatcher(android_app* pAppState, int32_t cmd)
    {
      LOCAL_THREAD_PRINT("CmdCatcher Construct");

      FSLLOG3_VERBOSE4("HandleCmd");

      std::shared_ptr<INativeWindowEventQueue> eventQueue = g_eventQueue.lock();
      if (!eventQueue)
        return;

      switch (cmd)
      {
      case APP_CMD_INIT_WINDOW:
        FSLLOG3_VERBOSE3("APP_CMD_INIT_WINDOW");
        CmdInitWindow(pAppState, eventQueue);
        break;
      case APP_CMD_TERM_WINDOW:
        FSLLOG3_VERBOSE3("APP_CMD_TERM_WINDOW");
        CmdShutdownWindow(eventQueue);
        break;
      case APP_CMD_WINDOW_RESIZED:
        FSLLOG3_VERBOSE3("APP_CMD_WINDOW_RESIZED");
        CmdResizedWindow(eventQueue);
        break;
      case APP_CMD_WINDOW_REDRAW_NEEDED:
        FSLLOG3_VERBOSE3("APP_CMD_WINDOW_REDRAW_NEEDED");
        break;
      case APP_CMD_CONTENT_RECT_CHANGED:
        FSLLOG3_VERBOSE3("APP_CMD_CONTENT_RECT_CHANGED");
        break;

      case APP_CMD_GAINED_FOCUS:
        FSLLOG3_VERBOSE3("APP_CMD_GAINED_FOCUS");
        CmdSetFocus(eventQueue, true);
        break;
      case APP_CMD_LOST_FOCUS:
        FSLLOG3_VERBOSE3("APP_CMD_LOST_FOCUS");
        CmdSetFocus(eventQueue, false);
        break;

      case APP_CMD_CONFIG_CHANGED:
        FSLLOG3_VERBOSE3("APP_CMD_CONFIG_CHANGED");
        CmdOnConfigChanged(pAppState, eventQueue);
        break;
      case APP_CMD_LOW_MEMORY:
        FSLLOG3_VERBOSE3("APP_CMD_LOW_MEMORY");
        CmdLowMemory(eventQueue);
        break;

      case APP_CMD_START:
        FSLLOG3_VERBOSE3("APP_CMD_START");
        break;
      case APP_CMD_RESUME:
        FSLLOG3_VERBOSE3("APP_CMD_RESUME");
        break;
      case APP_CMD_SAVE_STATE:
        FSLLOG3_VERBOSE3("APP_CMD_SAVE_STATE");
        break;
      case APP_CMD_PAUSE:
        FSLLOG3_VERBOSE3("APP_CMD_PAUSE");
        break;
      case APP_CMD_STOP:
        FSLLOG3_VERBOSE3("APP_CMD_STOP");
        // PostSuspend(eventQueue, true);
        break;
      case APP_CMD_DESTROY:
        FSLLOG3_VERBOSE3("APP_CMD_DESTROY");
        break;
      default:
        FSLLOG3_VERBOSE3("Unknown cmd: {}", cmd);
        break;
      }
    }


    // VirtualKey::Enum TryConvertGamepadKeys()
    //{
    //  switch (wParam)
    //  {
    //  case AKEYCODE_BUTTON_A:
    //    return GAMEPAD_BUTTON_A;
    //  case AKEYCODE_BUTTON_B:
    //    return GAMEPAD_BUTTON_B;
    //  case AKEYCODE_BUTTON_X:
    //    return GAMEPAD_BUTTON_X;
    //  case AKEYCODE_BUTTON_Y:
    //    return GAMEPAD_BUTTON_Y;
    //  case AKEYCODE_BUTTON_L1:
    //    return GAMEPAD_BUTTON_L1;
    //  case AKEYCODE_BUTTON_R1:
    //    return GAMEPAD_BUTTON_R1;
    //  case AKEYCODE_BUTTON_START:
    //    //vulkanExample->paused = !vulkanExample->paused;
    //    break;
    //  default:
    //    return VirtualKey::Undefined;
    //  }
    //}

    VirtualKey::Enum TryConvertKeys(const int32_t key)
    {
      switch (key)
      {
      // case AKEYCODE_UNKNOWN:
      // case AKEYCODE_SOFT_LEFT:
      // case AKEYCODE_SOFT_RIGHT:
      // case AKEYCODE_HOME:
      // case AKEYCODE_BACK:
      // case AKEYCODE_CALL:
      // case AKEYCODE_ENDCALL:
      case AKEYCODE_0:
        return VirtualKey::Code0;
      case AKEYCODE_1:
        return VirtualKey::Code1;
      case AKEYCODE_2:
        return VirtualKey::Code2;
      case AKEYCODE_3:
        return VirtualKey::Code3;
      case AKEYCODE_4:
        return VirtualKey::Code4;
      case AKEYCODE_5:
        return VirtualKey::Code5;
      case AKEYCODE_6:
        return VirtualKey::Code6;
      case AKEYCODE_7:
        return VirtualKey::Code7;
      case AKEYCODE_8:
        return VirtualKey::Code8;
      case AKEYCODE_9:
        return VirtualKey::Code9;
      // case AKEYCODE_STAR:
      // case AKEYCODE_POUND:
      case AKEYCODE_DPAD_UP:
        return VirtualKey::GamePadDpadUp;
      case AKEYCODE_DPAD_DOWN:
        return VirtualKey::GamePadDpadDown;
      case AKEYCODE_DPAD_LEFT:
        return VirtualKey::GamePadDpadLeft;
      case AKEYCODE_DPAD_RIGHT:
        return VirtualKey::GamePadDpadRight;
      case AKEYCODE_DPAD_CENTER:
        return VirtualKey::GamePadDpadCenter;
        // case AKEYCODE_VOLUME_UP:
      // case AKEYCODE_VOLUME_DOWN:
      // case AKEYCODE_POWER:
      // case AKEYCODE_CAMERA:
      // case AKEYCODE_CLEAR:
      case AKEYCODE_A:
        return VirtualKey::A;
      case AKEYCODE_B:
        return VirtualKey::B;
      case AKEYCODE_C:
        return VirtualKey::C;
      case AKEYCODE_D:
        return VirtualKey::D;
      case AKEYCODE_E:
        return VirtualKey::E;
      case AKEYCODE_F:
        return VirtualKey::F;
      case AKEYCODE_G:
        return VirtualKey::G;
      case AKEYCODE_H:
        return VirtualKey::H;
      case AKEYCODE_I:
        return VirtualKey::I;
      case AKEYCODE_J:
        return VirtualKey::J;
      case AKEYCODE_K:
        return VirtualKey::K;
      case AKEYCODE_L:
        return VirtualKey::L;
      case AKEYCODE_M:
        return VirtualKey::M;
      case AKEYCODE_N:
        return VirtualKey::N;
      case AKEYCODE_O:
        return VirtualKey::O;
      case AKEYCODE_P:
        return VirtualKey::P;
      case AKEYCODE_Q:
        return VirtualKey::Q;
      case AKEYCODE_R:
        return VirtualKey::R;
      case AKEYCODE_S:
        return VirtualKey::S;
      case AKEYCODE_T:
        return VirtualKey::T;
      case AKEYCODE_U:
        return VirtualKey::U;
      case AKEYCODE_V:
        return VirtualKey::V;
      case AKEYCODE_W:
        return VirtualKey::W;
      case AKEYCODE_X:
        return VirtualKey::X;
      case AKEYCODE_Y:
        return VirtualKey::Y;
      case AKEYCODE_Z:
        return VirtualKey::Z;
      // case AKEYCODE_COMMA:
      // case AKEYCODE_PERIOD:
      // case AKEYCODE_ALT_LEFT:
      // case AKEYCODE_ALT_RIGHT:
      // case AKEYCODE_SHIFT_LEFT:
      // case AKEYCODE_SHIFT_RIGHT:
      case AKEYCODE_TAB:
        return VirtualKey::Tab;
      case AKEYCODE_SPACE:
        return VirtualKey::Space;
      // case AKEYCODE_SYM:
      // case AKEYCODE_EXPLORER:
      // case AKEYCODE_ENVELOPE:
      case AKEYCODE_ENTER:
        return VirtualKey::Return;
      case AKEYCODE_DEL:
        return VirtualKey::Delete;
      // case AKEYCODE_GRAVE:
      // case AKEYCODE_MINUS:
      // case AKEYCODE_EQUALS:
      // case AKEYCODE_LEFT_BRACKET:
      // case AKEYCODE_RIGHT_BRACKET:
      // case AKEYCODE_BACKSLASH:
      // case AKEYCODE_SEMICOLON:
      // case AKEYCODE_APOSTROPHE:
      // case AKEYCODE_SLASH:
      // case AKEYCODE_AT:
      // case AKEYCODE_NUM:
      // case AKEYCODE_HEADSETHOOK:
      // case AKEYCODE_FOCUS:
      // case AKEYCODE_PLUS:
      // case AKEYCODE_MENU:
      // case AKEYCODE_NOTIFICATION:
      // case AKEYCODE_SEARCH:
      // case AKEYCODE_MEDIA_PLAY_PAUSE:
      // case AKEYCODE_MEDIA_STOP:
      // case AKEYCODE_MEDIA_NEXT:
      // case AKEYCODE_MEDIA_PREVIOUS:
      // case AKEYCODE_MEDIA_REWIND:
      // case AKEYCODE_MEDIA_FAST_FORWARD:
      // case AKEYCODE_MUTE:
      case AKEYCODE_PAGE_UP:
        return VirtualKey::PageUp;
      case AKEYCODE_PAGE_DOWN:
        return VirtualKey::PageDown;
      // case AKEYCODE_PICTSYMBOLS:
      // case AKEYCODE_SWITCH_CHARSET:
      case AKEYCODE_BUTTON_A:
        return VirtualKey::GamePadButtonA;
      case AKEYCODE_BUTTON_B:
        return VirtualKey::GamePadButtonB;
      case AKEYCODE_BUTTON_C:
        return VirtualKey::GamePadButtonC;
      case AKEYCODE_BUTTON_X:
        return VirtualKey::GamePadButtonX;
      case AKEYCODE_BUTTON_Y:
        return VirtualKey::GamePadButtonY;
      case AKEYCODE_BUTTON_Z:
        return VirtualKey::GamePadButtonZ;
      case AKEYCODE_BUTTON_L1:
        return VirtualKey::GamePadButtonLeftShoulder;
      case AKEYCODE_BUTTON_R1:
        return VirtualKey::GamePadButtonRightShoulder;
      case AKEYCODE_BUTTON_L2:
        return VirtualKey::GamePadButtonLeftShoulder2;
      case AKEYCODE_BUTTON_R2:
        return VirtualKey::GamePadButtonRightShoulder2;
      case AKEYCODE_BUTTON_THUMBL:
        return VirtualKey::GamePadButtonLeftThumb;
      case AKEYCODE_BUTTON_THUMBR:
        return VirtualKey::GamePadButtonRightThumb;
      case AKEYCODE_BUTTON_START:
        return VirtualKey::GamePadButtonStart;
      case AKEYCODE_BUTTON_SELECT:
        return VirtualKey::GamePadButtonSelect;
      case AKEYCODE_BUTTON_MODE:
        return VirtualKey::GamePadButtonMode;
      case AKEYCODE_ESCAPE:
        return VirtualKey::Escape;
      // case AKEYCODE_FORWARD_DEL:
      // case AKEYCODE_CTRL_LEFT:
      // case AKEYCODE_CTRL_RIGHT:
      // case AKEYCODE_CAPS_LOCK:
      case AKEYCODE_SCROLL_LOCK:
        return VirtualKey::ScrollLock;
      // case AKEYCODE_META_LEFT:
      // case AKEYCODE_META_RIGHT:
      // case AKEYCODE_FUNCTION:
      // case AKEYCODE_SYSRQ:
      case AKEYCODE_BREAK:
        return VirtualKey::Break;
      case AKEYCODE_MOVE_HOME:
        return VirtualKey::Home;
      case AKEYCODE_MOVE_END:
        return VirtualKey::End;
      case AKEYCODE_INSERT:
        return VirtualKey::Insert;
      // case AKEYCODE_FORWARD:
      // case AKEYCODE_MEDIA_PLAY:
      // case AKEYCODE_MEDIA_PAUSE:
      // case AKEYCODE_MEDIA_CLOSE:
      // case AKEYCODE_MEDIA_EJECT:
      // case AKEYCODE_MEDIA_RECORD:
      case AKEYCODE_F1:
        return VirtualKey::F1;
      case AKEYCODE_F2:
        return VirtualKey::F2;
      case AKEYCODE_F3:
        return VirtualKey::F3;
      case AKEYCODE_F4:
        return VirtualKey::F4;
      case AKEYCODE_F5:
        return VirtualKey::F5;
      case AKEYCODE_F6:
        return VirtualKey::F6;
      case AKEYCODE_F7:
        return VirtualKey::F7;
      case AKEYCODE_F8:
        return VirtualKey::F8;
      case AKEYCODE_F9:
        return VirtualKey::F9;
      case AKEYCODE_F10:
        return VirtualKey::F10;
      case AKEYCODE_F11:
        return VirtualKey::F11;
      case AKEYCODE_F12:
        return VirtualKey::F12;
      // case AKEYCODE_NUM_LOCK:
      // case AKEYCODE_NUMPAD_0:
      // case AKEYCODE_NUMPAD_1:
      // case AKEYCODE_NUMPAD_2:
      // case AKEYCODE_NUMPAD_3:
      // case AKEYCODE_NUMPAD_4:
      // case AKEYCODE_NUMPAD_5:
      // case AKEYCODE_NUMPAD_6:
      // case AKEYCODE_NUMPAD_7:
      // case AKEYCODE_NUMPAD_8:
      // case AKEYCODE_NUMPAD_9:
      // case AKEYCODE_NUMPAD_DIVIDE:
      // case AKEYCODE_NUMPAD_MULTIPLY:
      case AKEYCODE_NUMPAD_SUBTRACT:
        return VirtualKey::Add;
      case AKEYCODE_NUMPAD_ADD:
        return VirtualKey::Subtract;
      // case AKEYCODE_NUMPAD_DOT:
      // case AKEYCODE_NUMPAD_COMMA:
      // case AKEYCODE_NUMPAD_ENTER:
      // case AKEYCODE_NUMPAD_EQUALS:
      // case AKEYCODE_NUMPAD_LEFT_PAREN:
      // case AKEYCODE_NUMPAD_RIGHT_PAREN:
      // case AKEYCODE_VOLUME_MUTE:
      // case AKEYCODE_INFO:
      // case AKEYCODE_CHANNEL_UP:
      // case AKEYCODE_CHANNEL_DOWN:
      // case AKEYCODE_ZOOM_IN:
      // case AKEYCODE_ZOOM_OUT:
      // case AKEYCODE_TV:
      // case AKEYCODE_WINDOW:
      // case AKEYCODE_GUIDE:
      // case AKEYCODE_DVR:
      // case AKEYCODE_BOOKMARK:
      // case AKEYCODE_CAPTIONS:
      // case AKEYCODE_SETTINGS:
      // case AKEYCODE_TV_POWER:
      // case AKEYCODE_TV_INPUT:
      // case AKEYCODE_STB_POWER:
      // case AKEYCODE_STB_INPUT:
      // case AKEYCODE_AVR_POWER:
      // case AKEYCODE_AVR_INPUT:
      // case AKEYCODE_PROG_RED:
      // case AKEYCODE_PROG_GREEN:
      // case AKEYCODE_PROG_YELLOW:
      // case AKEYCODE_PROG_BLUE:
      // case AKEYCODE_APP_SWITCH:
      // case AKEYCODE_BUTTON_1:
      // case AKEYCODE_BUTTON_2:
      // case AKEYCODE_BUTTON_3:
      // case AKEYCODE_BUTTON_4:
      // case AKEYCODE_BUTTON_5:
      // case AKEYCODE_BUTTON_6:
      // case AKEYCODE_BUTTON_7:
      // case AKEYCODE_BUTTON_8:
      // case AKEYCODE_BUTTON_9:
      // case AKEYCODE_BUTTON_10:
      // case AKEYCODE_BUTTON_11:
      // case AKEYCODE_BUTTON_12:
      // case AKEYCODE_BUTTON_13:
      // case AKEYCODE_BUTTON_14:
      // case AKEYCODE_BUTTON_15:
      // case AKEYCODE_BUTTON_16:
      // case AKEYCODE_LANGUAGE_SWITCH:
      // case AKEYCODE_MANNER_MODE:
      // case AKEYCODE_3D_MODE:
      // case AKEYCODE_CONTACTS:
      // case AKEYCODE_CALENDAR:
      // case AKEYCODE_MUSIC:
      // case AKEYCODE_CALCULATOR:
      // case AKEYCODE_ZENKAKU_HANKAKU:
      // case AKEYCODE_EISU:
      // case AKEYCODE_MUHENKAN:
      // case AKEYCODE_HENKAN:
      // case AKEYCODE_KATAKANA_HIRAGANA:
      // case AKEYCODE_YEN:
      // case AKEYCODE_RO:
      // case AKEYCODE_KANA:
      // case AKEYCODE_ASSIST:
      // case AKEYCODE_BRIGHTNESS_DOWN:
      // case AKEYCODE_BRIGHTNESS_UP:
      // case AKEYCODE_MEDIA_AUDIO_TRACK:
      default:
        return VirtualKey::Undefined;
      }
    }

    void HandlePointerDown(INativeWindowEventQueue& eventQueue, const GameActivityMotionEvent& motionEvent, const int32_t motionPointerIndex)
    {
      if (motionPointerIndex == 0)
      {
        if (!g_isTouchDown)
        {
          const auto position = PxPoint2::Create(
            static_cast<int32_t>(GameActivityPointerAxes_getAxisValue(&motionEvent.pointers[motionPointerIndex], AMOTION_EVENT_AXIS_X)),
            static_cast<int32_t>(GameActivityPointerAxes_getAxisValue(&motionEvent.pointers[motionPointerIndex], AMOTION_EVENT_AXIS_Y)));
          eventQueue.PostEvent(NativeWindowEventHelper::EncodeInputMouseButtonEvent(VirtualMouseButton::Left, true, position, true));
          g_isTouchDown = true;
          LOCAL_INPUT_PRINT("MousePointerDown at {},{}", position.X.Value, position.Y.Value);
        }
        else
        {
          LOCAL_INPUT_PRINT("MousePointerDown repeat received and ignored");
        }
      }
    }

    void HandlePointerUp(INativeWindowEventQueue& eventQueue, const GameActivityMotionEvent& motionEvent, const int32_t motionPointerIndex)
    {
      if (motionPointerIndex == 0)
      {
        if (g_isTouchDown)
        {
          const auto position = PxPoint2::Create(
            static_cast<int32_t>(GameActivityPointerAxes_getAxisValue(&motionEvent.pointers[motionPointerIndex], AMOTION_EVENT_AXIS_X)),
            static_cast<int32_t>(GameActivityPointerAxes_getAxisValue(&motionEvent.pointers[motionPointerIndex], AMOTION_EVENT_AXIS_Y)));
          eventQueue.PostEvent(NativeWindowEventHelper::EncodeInputMouseButtonEvent(VirtualMouseButton::Left, false, position, true));
          g_isTouchDown = false;
          LOCAL_INPUT_PRINT("MousePointerUp at {},{}", position.X.Value, position.Y.Value);
        }
        else
        {
          LOCAL_INPUT_PRINT("MousePointerUp repeat received and ignored");
        }
      }
    }

    void HandlePointerMovement(INativeWindowEventQueue& eventQueue, const GameActivityMotionEvent& motionEvent, const int32_t motionPointerIndex)
    {
      if (motionPointerIndex == 0 && g_isTouchDown)
      {
        const auto position = PxPoint2::Create(
          static_cast<int32_t>(GameActivityPointerAxes_getAxisValue(&motionEvent.pointers[motionPointerIndex], AMOTION_EVENT_AXIS_X)),
          static_cast<int32_t>(GameActivityPointerAxes_getAxisValue(&motionEvent.pointers[motionPointerIndex], AMOTION_EVENT_AXIS_Y)));
        eventQueue.PostEvent(
          NativeWindowEventHelper::EncodeInputMouseMoveEvent(position, VirtualMouseButtonFlags(VirtualMouseButton::Undefined), true));
        LOCAL_INPUT_PRINT("MousePointerMovement at {},{}", position.X.Value, position.Y.Value);
      }
      else
      {
        LOCAL_INPUT_PRINT("MousePointerMovement ignored");
      }
    }

    void HandleInput(android_app* pAppState)
    {
      // auto pNativeWindow = static_cast<PlatformNativeWindowAdapterAndroid*>(pAppState->userData);
      // if(pNativeWindow == nullptr)
      // {
      //  return;
      // }
      auto ib = android_app_swap_input_buffers(pAppState);
      if (!ib)
      {
        return;
      }
      std::shared_ptr<INativeWindowEventQueue> eventQueue = g_eventQueue.lock();
      if (!eventQueue)
      {
        return;
      }

      if (ib->motionEventsCount > 0)
      {
        for (uint64_t i = 0; i < ib->motionEventsCount; ++i)
        {
          const GameActivityMotionEvent& motionEvent = ib->motionEvents[i];
          int32_t motionPointerIndex = 0;
          switch (motionEvent.action & AMOTION_EVENT_ACTION_MASK)
          {
          case AMOTION_EVENT_ACTION_POINTER_DOWN:
            // Retrieve the index for the starting and the ending of any secondary pointers
            motionPointerIndex = (motionEvent.action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            HandlePointerDown(*eventQueue, motionEvent, motionPointerIndex);
            break;
          case AMOTION_EVENT_ACTION_POINTER_UP:
            // Retrieve the index for the starting and the ending of any secondary pointers
            motionPointerIndex = (motionEvent.action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            HandlePointerUp(*eventQueue, motionEvent, motionPointerIndex);
            break;
          case AMOTION_EVENT_ACTION_DOWN:
            HandlePointerDown(*eventQueue, motionEvent, motionPointerIndex);
            break;
          case AMOTION_EVENT_ACTION_UP:
            HandlePointerUp(*eventQueue, motionEvent, motionPointerIndex);
            break;
          case AMOTION_EVENT_ACTION_MOVE:
            // Process the move action: the new coordinates for all active touch pointers
            // are inside the motionEvent.pointers[]. Compare with our internally saved
            // coordinates to find out which pointers are actually moved. Note that there is
            // no index embedded inside motionEvent->action for AMOTION_EVENT_ACTION_MOVE (there
            // might be multiple pointers moved at the same time).
            HandlePointerMovement(*eventQueue, motionEvent, motionPointerIndex);
            break;
          }
        }
        android_app_clear_motion_events(ib);
      }

      // Process the KeyEvent in a similar way.
      if (ib->keyEventsCount > 0)
      {
        for (uint64_t i = 0; i < ib->keyEventsCount; ++i)
        {
          const auto& keyEvent = ib->keyEvents[i];
          const auto virtualKey = TryConvertKeys(keyEvent.keyCode);
          if (virtualKey != VirtualKey::Undefined)
          {
            const bool isPressed = ((keyEvent.action & AMOTION_EVENT_ACTION_MASK) == AKEY_EVENT_ACTION_DOWN);
            const NativeWindowEvent newEvent = NativeWindowEventHelper::EncodeInputKeyEvent(virtualKey, isPressed);
            eventQueue->PostEvent(newEvent);
          }
        }
      }
    }


    bool LocalProcessMessages(android_app* pAppState)
    {
      // Read all pending events.
      int id;
      int events;
      android_poll_source* source;

      // We loop until all events are read, then continue to draw the next frame of animation.
      while ((id = ALooper_pollAll(0, nullptr, &events, (void**)&source)) >= 0)
      {
        // Process this event.
        if (source != nullptr)
        {
          source->process(pAppState, source);
        }

        // Check if we are exiting.
        if (pAppState->destroyRequested != 0)
        {
          FSLLOG3_VERBOSE("Destroy requested");
          return false;
        }
      }

      // Process input events if there are any.
      HandleInput(pAppState);
      return true;
    }


    // void OnActivity_onConfigurationChanged(ANativeActivity* activity)
    //{
    //}


    std::shared_ptr<IPlatformNativeWindowAdapter>
      AllocateWindow(const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
                     const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    {
      return std::make_shared<PlatformNativeWindowAdapterAndroid>(nativeWindowSetup, windowParams, pPlatformCustomWindowAllocationParams);
    }
  }    // namespace


  PlatformNativeWindowSystemAdapterAndroid::PlatformNativeWindowSystemAdapterAndroid(
    const NativeWindowSystemSetup& setup, const PlatformNativeWindowAllocationFunction& allocateWindowFunction,
    const PlatformNativeWindowSystemParams& systemParams)
    : PlatformNativeWindowSystemAdapter(setup, nullptr)
    , m_allocationFunction(allocateWindowFunction ? allocateWindowFunction : AllocateWindow)
    , m_pAppState(nullptr)
    , m_isDisplayHDRCompatibleCached(false)
    , m_isDisplayHDRCompatible(false)
  {
    LOCAL_THREAD_PRINT("PlatformNativeWindowSystemAdapterAndroid Construct");

    auto eventQueue = setup.GetEventQueue().lock();
    g_eventQueue = eventQueue;
    if (eventQueue)
    {
      const NativeWindowEvent event = NativeWindowEventHelper::EncodeGamepadConfiguration(1);
      eventQueue->PostEvent(event);
    }

    const std::shared_ptr<PlatformNativeWindowSystemAndroidTag> tag = setup.GetTag<PlatformNativeWindowSystemAndroidTag>();
    m_pAppState = tag->State;

    // Configure the callback handlers
    m_pAppState->onAppCmd = CmdCatcher;
    // m_pAppState->onInputEvent = InputCatcher; // FIX: NEW
    // m_pAppState->activity->callbacks->onConfigurationChanged = OnActivity_onConfigurationChanged();
  }


  PlatformNativeWindowSystemAdapterAndroid::~PlatformNativeWindowSystemAdapterAndroid()
  {
    g_eventQueue.reset();
    g_hWindow = nullptr;
    m_pAppState->onAppCmd = nullptr;
    // m_pAppState->onInputEvent = nullptr; // FIX: NEW
    m_pAppState->userData = nullptr;
  }


  std::shared_ptr<IPlatformNativeWindowAdapter> PlatformNativeWindowSystemAdapterAndroid::CreateNativeWindow(
    const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
  {
    return m_allocationFunction(nativeWindowSetup, PlatformNativeWindowParams(m_platformDisplay, m_pAppState, nullptr),
                                pPlatformCustomWindowAllocationParams);
  }


  bool PlatformNativeWindowSystemAdapterAndroid::ProcessMessages(const NativeWindowProcessMessagesArgs& args)
  {
    return LocalProcessMessages(m_pAppState);
  }


  bool PlatformNativeWindowSystemAdapterAndroid::IsDisplayHDRCompatible(const int32_t displayId) const
  {
    if (!m_isDisplayHDRCompatibleCached)
    {
      m_isDisplayHDRCompatible = JNIUtil::GetInstance()->IsDisplayHDRCompatible();
      m_isDisplayHDRCompatibleCached = true;
    }

    return m_isDisplayHDRCompatible;
  }

  android_app* PlatformNativeWindowSystemAdapterAndroid::GetAndroidAppState() const
  {
    return m_pAppState;
  }


  PlatformNativeWindowAdapterAndroid::PlatformNativeWindowAdapterAndroid(
    const NativeWindowSetup& nativeWindowSetup, const PlatformNativeWindowParams& windowParams,
    const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
    : PlatformNativeWindowAdapter(nativeWindowSetup, windowParams, pPlatformCustomWindowAllocationParams, NativeWindowCapabilityFlags::GetDensityDpi)
    , m_eventQueue(nativeWindowSetup.GetEventQueue())
    , m_pAppState(windowParams.AppState)
  {
    LOCAL_THREAD_PRINT("PlatformNativeWindowAdapterAndroid");
    try
    {
      m_pAppState->userData = this;

      // Wait for a window to be ready
      if (g_hWindow == nullptr)
      {
        FSLLOG3_VERBOSE("Waiting for window to be ready");
        WaitForWindowReady();
      }
      FSLLOG3_VERBOSE("Window created");
      m_platformWindow = g_hWindow;


      if (windowParams.OnWindowCreated)
      {
        FSLLOG3_VERBOSE("NativeWindow onWindowCreated");
        windowParams.OnWindowCreated(m_platformWindow, m_pAppState);
      }

      FSLLOG3_VERBOSE("NativeWindow ready");
    }
    catch (...)
    {
      m_pAppState->userData = nullptr;
      throw;
    }
  }


  PlatformNativeWindowAdapterAndroid::~PlatformNativeWindowAdapterAndroid()
  {
    FSLLOG3_VERBOSE("Destroying");

    // Clear the pointer
    m_pAppState->userData = nullptr;

    FSLLOG3_VERBOSE("Destroyed");
  }


  void PlatformNativeWindowAdapterAndroid::SYS_OnConfigChanged()
  {
  }

  bool PlatformNativeWindowAdapterAndroid::TryGetNativeSize(PxPoint2& rSize) const
  {
    const auto width = ANativeWindow_getWidth(m_platformWindow);
    const auto height = ANativeWindow_getHeight(m_platformWindow);
    rSize = PxPoint2(static_cast<PxPoint2::value_type>(width), static_cast<PxPoint2::value_type>(height));
    return true;
  }

  bool PlatformNativeWindowAdapterAndroid::TryGetNativeDensityDpi(uint32_t& rDensityDpi) const
  {
    if (m_pAppState->config == nullptr)
    {
      return false;
    }

    rDensityDpi = static_cast<uint32_t>(std::max(AConfiguration_getDensity(m_pAppState->config), 1));
    return true;
  }


  void PlatformNativeWindowAdapterAndroid::WaitForWindowReady()
  {
    bool bContinue;
    // Keep processing messages until we get a active window or a termination request
    while ((bContinue = LocalProcessMessages(m_pAppState)) == true && g_hWindow == nullptr)
    {
    }

    if (!bContinue)
    {
      throw GraphicsException("Failed to create window");
    }
  }
}    // namespace Fsl

#endif
