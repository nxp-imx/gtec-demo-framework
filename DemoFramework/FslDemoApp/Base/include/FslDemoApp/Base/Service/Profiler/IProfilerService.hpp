#ifndef FSLDEMOAPP_BASE_SERVICE_PROFILER_IPROFILERSERVICE_HPP
#define FSLDEMOAPP_BASE_SERVICE_PROFILER_IPROFILERSERVICE_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslDemoApp/Base/Service/Profiler/ProfilerFrameTime.hpp>
#include <FslDemoApp/Base/Service/Profiler/ProfilerCustomCounterDesc.hpp>
#include <FslDemoApp/Base/Service/Profiler/ProfilerCustomCounterHandle.hpp>
#include <string>

namespace Fsl
{

  class IProfilerService
  {
  public:
    virtual ~IProfilerService() {}
    virtual ProfilerFrameTime GetLastFrameTime() const = 0;
    virtual ProfilerFrameTime GetAverageFrameTime() const = 0;

    //! @brief Get the maximum number of custom counters that we support
    virtual int32_t GetCustomCounterCapacity() const = 0;

    //! @brief Get the number of custom counters
    virtual int32_t GetCustomCounterCount() const = 0;

    //! @brief Get a custom counter handle by its index (0 <= index < GetCustomCounterCount())
    virtual ProfilerCustomCounterHandle GetCustomCounterHandle(const int32_t index) const = 0;

    //! @brief Create a new custom counter for the profiler to track
    //! @param name the name of the counter we only allow ASCII characters.
    //! @param minValue the minimum value that will be registered for this counter (can be used by a graph to scale the registered values)
    //! @param maxValue the maximum value that will be registered for this counter (can be used by a graph to scale the registered values)
    //! @param colorHint the color this value ought to be displayed using (but its only a hint so it might be completely ignored).
    virtual ProfilerCustomCounterHandle CreateCustomCounter(const std::string& name, const int32_t minValue, const int32_t maxValue, const Color& colorHint) = 0;

    //! @brief Destroy a existing custom counter.
    virtual void DestroyCustomCounter(const ProfilerCustomCounterHandle& handle) = 0;

    //! @brief Get the current value of the custom counter (if not set it will be minValue)
    virtual int32_t Get(const ProfilerCustomCounterHandle& handle) const = 0;

    //! @brief Update the current value of the custom counter
    virtual void Set(const ProfilerCustomCounterHandle& handle, const int32_t value) = 0;

    //! @brief Get a description of the custom counter
    virtual ProfilerCustomCounterDesc GetDescription(const ProfilerCustomCounterHandle& handle) const = 0;

    //! @brief The custom configuration revision is modified each time a custom counter is created or removed.
    //! @return the current revision (this will never be zero)
    //! @note  This generally allows a user of the profile service to keep a cached version of the information
    //         around without querying this service all the time for configuration info.
    //         Beware that its technically possible to 'overflow' this counter and make it return the same value
    //         as for a different configuration but it would require 0x100000000 changes between the time you
    //         read this and the next time it was read, so its unlikely to happen.
    virtual uint32_t GetCustomConfigurationRevision() const = 0;

    //! @brief Check if the handle is still considered valid
    virtual bool IsValidHandle(const ProfilerCustomCounterHandle& handle) const = 0;
  };

}

#endif
