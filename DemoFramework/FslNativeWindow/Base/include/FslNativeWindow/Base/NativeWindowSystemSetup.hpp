#ifndef FSLNATIVEWINDOW_BASE_NATIVEWINDOWSYSTEMSETUP_HPP
#define FSLNATIVEWINDOW_BASE_NATIVEWINDOWSYSTEMSETUP_HPP
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
#include <FslBase/ITag.hpp>
#include <FslNativeWindow/Base/NativeWindowConfig.hpp>

namespace Fsl
{
  class INativeWindowEventQueue;

  class NativeWindowSystemSetup
  {
    std::weak_ptr<INativeWindowEventQueue> m_eventQueue;
    uint32_t m_verbosityLevel;
    NativeWindowConfig m_config;
    std::shared_ptr<ITag> m_tag;
  public:
    NativeWindowSystemSetup(const std::weak_ptr<INativeWindowEventQueue>& eventQueue, const uint32_t verbosityLevel,
                            const NativeWindowConfig& config, const std::shared_ptr<ITag>& tag);
    ~NativeWindowSystemSetup();

    //! @brief Get the event queue if present
    std::weak_ptr<INativeWindowEventQueue> GetEventQueue() const;

    uint32_t GetVerbosityLevel() const;

    //! @brief Get the associated tag if any
    std::shared_ptr<ITag> GetTag() const;

    //! @brief Set the associated tag
    void SetTag(const std::shared_ptr<ITag>& tag);

    //! @brief Get the associated tag
    template<typename T>
    std::shared_ptr<T> GetTag() const
    {
      const std::shared_ptr<T> ptr = std::dynamic_pointer_cast<T>(m_tag);
      if (!ptr)
        throw UnknownTypeException(typeid(T).name());
      return ptr;
    }

    //! @brief Get the window config
    NativeWindowConfig GetConfig() const;

  };

}

#endif
