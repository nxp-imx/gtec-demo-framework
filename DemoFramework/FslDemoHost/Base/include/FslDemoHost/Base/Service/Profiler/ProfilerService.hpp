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

#include <FslDemoApp/Base/Service/Profiler/IProfilerService.hpp>
#include <FslDemoHost/Base/Service/Profiler/IProfilerServiceControl.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalService.hpp>
#include <deque>
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
      int32_t Value;
      uint16_t RealIndex;
      uint16_t Version;
      bool IsInUse;

      CustomCounter()
        : Desc()
        , Value(0)
        , RealIndex(0)
        , Version(0)
        , IsInUse(false)
      {
      }
    };

    struct ProfilerRecord
    {
      int32_t UpdateTime;
      int32_t DrawTime;
      int32_t TotalTime;

      ProfilerRecord()
        : UpdateTime(0)
        , DrawTime(0)
        , TotalTime(0)
      {
      }

      ProfilerRecord(const int32_t updateTime, const int32_t drawTime, const int32_t totalTime)
        : UpdateTime(updateTime)
        , DrawTime(drawTime)
        , TotalTime(totalTime)
      {
      }
    };

    std::deque<ProfilerRecord> m_entries;
    std::size_t m_maxCapacity;
    ProfilerRecord m_combinedTime;
    std::vector<CustomCounter> m_customCounters;
    int32_t m_customCounterCount;
    uint32_t m_customConfigurationRevision;
  public:
    ProfilerService(const ServiceProvider& serviceProvider, const std::shared_ptr<ProfilerServiceOptionParser>& optionParser);
    ~ProfilerService();

    // From IProfilerService
    virtual ProfilerFrameTime GetLastFrameTime() const override;
    virtual ProfilerFrameTime GetAverageFrameTime() const override;
    virtual int32_t GetCustomCounterCapacity() const override;
    virtual int32_t GetCustomCounterCount() const override;
    virtual ProfilerCustomCounterHandle GetCustomCounterHandle(const int32_t index) const override;
    virtual ProfilerCustomCounterHandle CreateCustomCounter(const std::string& name, const int32_t minValue, const int32_t maxValue, const Color& colorHint) override;
    virtual void DestroyCustomCounter(const ProfilerCustomCounterHandle& handle) override;
    virtual int32_t Get(const ProfilerCustomCounterHandle& handle) const override;
    virtual void Set(const ProfilerCustomCounterHandle& handle, const int32_t value) override;
    virtual ProfilerCustomCounterDesc GetDescription(const ProfilerCustomCounterHandle& handle) const override;
    virtual uint32_t GetCustomConfigurationRevision() const override;
    virtual bool IsValidHandle(const ProfilerCustomCounterHandle& handle) const override;

    // From IProfilerServiceControl
    virtual void AddFrameTimes(const uint64_t updateTime, const uint64_t drawTime, const uint64_t totalTime) override;
  private:
    inline int32_t ConvertHandleToIndex(const ProfilerCustomCounterHandle& handle) const;
  };
}

#endif
