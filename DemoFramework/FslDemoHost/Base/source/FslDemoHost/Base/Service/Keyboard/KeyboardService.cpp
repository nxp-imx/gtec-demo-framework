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
#include <FslDemoApp/Base/Service/Events/Basic/KeyEvent.hpp>
#include <FslDemoApp/Base/Service/NativeWindowEvents/INativeWindowEvents.hpp>
#include <FslDemoHost/Base/Service/Events/IEventPoster.hpp>
#include <FslDemoHost/Base/Service/Keyboard/KeyboardService.hpp>
#include <FslNativeWindow/Base/NativeWindowEventHelper.hpp>
#include <algorithm>
#include <array>

namespace Fsl
{
  // FIX: this service most likely has a issue with not getting up key events on windows
  //      if the active window is changed while the key is down
  KeyboardService::KeyboardService(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)
  {
  }


  void KeyboardService::Link(const ServiceProvider& serviceProvider)
  {
    ThreadLocalService::Link(serviceProvider);
    const std::shared_ptr<INativeWindowEvents> events = serviceProvider.Get<INativeWindowEvents>();
    events->Register(shared_from_this());

    m_eventPoster = serviceProvider.Get<IEventPoster>();
  }


  KeyboardService::~KeyboardService() = default;


  KeyboardState KeyboardService::GetState() const
  {
    std::array<VirtualKey::Enum, 256> keysScratchpad{};
    FSLLOG_WARNING_IF(m_keys.size() > keysScratchpad.size(), "There are currently more than 256 pressed keys.");
    const uint32_t keyCount = std::min(static_cast<uint32_t>(keysScratchpad.size()), static_cast<uint32_t>(m_keys.size()));

    // Copy the keys to the array
    uint32_t keyIndex = 0;
    auto itr = m_keys.begin();
    while (itr != m_keys.end() && keyIndex < keyCount)
    {
      keysScratchpad[keyIndex] = *itr;
      ++itr;
      ++keyIndex;
    }
    return {keysScratchpad.data(), keyIndex};
  }


  void KeyboardService::OnNativeWindowEvent(const NativeWindowEvent& event)
  {
    if (event.Type != NativeWindowEventType::InputKey)
    {
      return;
    }

    VirtualKey::Enum key;
    bool isPressed;
    uint32_t deviceId;
    NativeWindowEventHelper::DecodeInputKeyEvent(event, key, isPressed, deviceId);

    const KeySet::iterator itr = m_keys.find(key);
    if (itr != m_keys.end())
    {
      // We found the key but it's no longer pressed -> so lets erase it
      if (!isPressed)
      {
        m_keys.erase(itr);
        m_eventPoster->Post(KeyEvent(key, isPressed, deviceId));
      }
    }
    else
    {
      // We didn't find the key but its pressed -> so lets add it
      if (isPressed)
      {
        m_keys.insert(key);
        m_eventPoster->Post(KeyEvent(key, isPressed, deviceId));
      }
    }
  }
}
