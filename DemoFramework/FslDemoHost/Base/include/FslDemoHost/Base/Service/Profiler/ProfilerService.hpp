#ifndef FSLDEMOHOST_BASE_SERVICE_PROFILER_PROFILERSERVICE_HPP
#define FSLDEMOHOST_BASE_SERVICE_PROFILER_PROFILERSERVICE_HPP
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

#include <FslBase/Collections/CircularFixedSizeBuffer.hpp>
#include <FslDemoApp/Base/Service/Profiler/IProfilerService.hpp>
#include <FslDemoHost/Base/Service/Profiler/IProfilerServiceControl.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalService.hpp>
#include <vector>

namespace Fsl
{
  class ProfilerServiceOptionParser;

  class ProfilerService
    : public ThreadLocalService
    , public IProfilerService
    , public IProfilerServiceControl
  {
    struct CustomCounter
    {
      ProfilerCustomCounterDesc Desc;
      int32_t Value{0};
      uint16_t RealIndex{0};
      uint16_t Version{0};
      bool IsInUse{false};

      CustomCounter() = default;
    };

    struct ProfilerRecord
    {
      int32_t UpdateTime{0};
      int32_t DrawTime{0};
      int32_t TotalTime{0};

      ProfilerRecord() = default;

      ProfilerRecord(const int32_t updateTime, const int32_t drawTime, const int32_t totalTime)
        : UpdateTime(updateTime)
        , DrawTime(drawTime)
        , TotalTime(totalTime)
      {
      }
    };

    std::size_t m_maxCapacity;
    CircularFixedSizeBuffer<ProfilerRecord> m_entries;
    ProfilerRecord m_combinedTime;
    std::vector<CustomCounter> m_customCounters;
    int32_t m_customCounterCount;
    uint32_t m_customConfigurationRevision;

  public:
    ProfilerService(const ServiceProvider& serviceProvider, const std::shared_ptr<ProfilerServiceOptionParser>& optionParser);
    ~ProfilerService() override;

    // From IProfilerService
    ProfilerFrameTime GetLastFrameTime() const override;
    ProfilerFrameTime GetAverageFrameTime() const override;
    int32_t GetCustomCounterCapacity() const override;
    int32_t GetCustomCounterCount() const override;
    ProfilerCustomCounterHandle GetCustomCounterHandle(const int32_t index) const override;
    ProfilerCustomCounterHandle CreateCustomCounter(const std::string& name, const int32_t minValue, const int32_t maxValue,
                                                    const Color& colorHint) override;
    void DestroyCustomCounter(const ProfilerCustomCounterHandle& handle) override;
    int32_t Get(const ProfilerCustomCounterHandle& handle) const override;
    void Set(const ProfilerCustomCounterHandle& handle, const int32_t value) override;
    ProfilerCustomCounterDesc GetDescription(const ProfilerCustomCounterHandle& handle) const override;
    uint32_t GetCustomConfigurationRevision() const override;
    bool IsValidHandle(const ProfilerCustomCounterHandle& handle) const override;

    // From IProfilerServiceControl
    void AddFrameTimes(const uint64_t updateTime, const uint64_t drawTime, const uint64_t totalTime) override;

  private:
    inline int32_t ConvertHandleToIndex(const ProfilerCustomCounterHandle& handle) const;
  };
}

#endif
