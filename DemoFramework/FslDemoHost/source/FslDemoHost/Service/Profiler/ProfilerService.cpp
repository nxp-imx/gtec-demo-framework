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

#include <FslDemoHost/Service/Profiler/ProfilerService.hpp>
#include <FslDemoHost/Service/Profiler/ProfilerServiceOptionParser.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Exceptions.hpp>
#include <algorithm>
#include <cassert>
#include <limits>
#include <sstream>

namespace Fsl
{
  namespace
  {
    const uint16_t MAX_CUSTOM_COUNTERS = 20;

    inline int32_t CapTime(const uint64_t value)
    {
      return static_cast<int32_t>(std::max(std::min(value, static_cast<uint64_t>(std::numeric_limits<int32_t>::max())), (uint64_t)0));
    }
  }

  ProfilerService::ProfilerService(const ServiceProvider& serviceProvider, const std::shared_ptr<ProfilerServiceOptionParser>& optionParser)
    : ThreadLocalService(serviceProvider)
    , m_maxCapacity(optionParser->GetAverageEntries())
    , m_combinedTime()
    , m_customCounters(MAX_CUSTOM_COUNTERS)
    , m_customCounterCount(0)
    , m_customConfigurationRevision(1)
  {
    for (uint16_t i = 0; i < static_cast<uint16_t>(m_customCounters.size()); ++i)
      m_customCounters[i].RealIndex = i;
  }


  ProfilerService::~ProfilerService()
  {
  }


  ProfilerFrameTime ProfilerService::GetLastFrameTime() const
  {
    if (m_entries.size() > 0)
    {
      const ProfilerRecord entry = m_entries.back();
      return ProfilerFrameTime(entry.UpdateTime, entry.DrawTime, entry.TotalTime);
    }
    else
      return ProfilerFrameTime();
  }


  ProfilerFrameTime ProfilerService::GetAverageFrameTime() const
  {
    const int32_t numFrames = static_cast<int32_t>(m_entries.size());
    if(numFrames > 0 )
      return ProfilerFrameTime(m_combinedTime.UpdateTime / numFrames, m_combinedTime.DrawTime / numFrames, m_combinedTime.TotalTime / numFrames);
    else
      return ProfilerFrameTime();
  }


  int32_t ProfilerService::GetCustomCounterCapacity() const
  {
    return static_cast<int32_t>(m_customCounters.size());
  }


  int32_t ProfilerService::GetCustomCounterCount() const
  {
    return m_customCounterCount;
  }


  ProfilerCustomCounterHandle ProfilerService::GetCustomCounterHandle(const int32_t index) const
  {
    if (index < 0 || index >= m_customCounterCount)
      throw IndexOutOfRangeException("index out of bounds");

    const int32_t handleIndex = m_customCounters[index].RealIndex;
    assert(handleIndex >= 0);
    return ProfilerCustomCounterHandle((static_cast<uint32_t>(handleIndex) << 16) | m_customCounters[index].Version);
  }


  ProfilerCustomCounterHandle ProfilerService::CreateCustomCounter(const std::string& name, const int32_t minValue, const int32_t maxValue, const Color& colorHint)
  {
    if (m_customCounterCount >= static_cast<int32_t>(m_customCounters.size()))
      throw NotSupportedException("Capacity reached");

    ++m_customConfigurationRevision;
    if (m_customConfigurationRevision == 0)
      ++m_customConfigurationRevision;

    const uint32_t freeIndex = m_customCounters[m_customCounterCount].RealIndex;
    assert(m_customCounters[freeIndex].IsInUse == false);

    m_customCounters[freeIndex].Desc = ProfilerCustomCounterDesc(name, minValue, maxValue, colorHint);
    m_customCounters[freeIndex].Value = minValue;
    m_customCounters[freeIndex].IsInUse = true;

    ++m_customCounterCount;
    return ProfilerCustomCounterHandle((freeIndex << 16) | m_customCounters[freeIndex].Version);
  }


  void ProfilerService::DestroyCustomCounter(const ProfilerCustomCounterHandle& handle)
  {
    const int32_t handleIndex = ConvertHandleToIndex(handle);
    ++m_customConfigurationRevision;
    if (m_customConfigurationRevision == 0)
      ++m_customConfigurationRevision;

    // 0,1,2,3,4,5,6,7
    // A,B,C,-,-,-,-,-
    // Remove A
    // 2,1,0,3,4,5,6,7
    // -,B,C,-,-,-,-,-


    --m_customCounterCount;
    std::swap(m_customCounters[m_customCounterCount].RealIndex, m_customCounters[handleIndex].RealIndex);
    m_customCounters[handleIndex].Desc = ProfilerCustomCounterDesc();
    m_customCounters[handleIndex].Value = 0;
    m_customCounters[handleIndex].IsInUse = false;
    ++m_customCounters[handleIndex].Version;
  }


  int32_t ProfilerService::Get(const ProfilerCustomCounterHandle& handle) const
  {
    const int32_t handleIndex = ConvertHandleToIndex(handle);
    return m_customCounters[handleIndex].Value;
  }


  void ProfilerService::Set(const ProfilerCustomCounterHandle& handle, const int32_t value)
  {
    const int32_t handleIndex = ConvertHandleToIndex(handle);
    m_customCounters[handleIndex].Value = value;
  }


  ProfilerCustomCounterDesc ProfilerService::GetDescription(const ProfilerCustomCounterHandle& handle) const
  {
    const int32_t handleIndex = ConvertHandleToIndex(handle);
    return m_customCounters[handleIndex].Desc;
  }


  uint32_t ProfilerService::GetCustomConfigurationRevision() const
  {
    return m_customConfigurationRevision;
  }


  bool ProfilerService::IsValidHandle(const ProfilerCustomCounterHandle& handle) const
  {
    const uint16_t handleVersion = static_cast<uint16_t>(handle.Value & 0xFFFF);
    const uint32_t handleIndex = handle.Value >> 16;

    if (handleIndex >= static_cast<uint32_t>(m_customCounters.size()) || !m_customCounters[handleIndex].IsInUse || m_customCounters[handleIndex].Version != handleVersion)
      return false;
    return true;
  }


  void ProfilerService::AddFrameTimes(const uint64_t updateTime, const uint64_t drawTime, const uint64_t totalTime)
  {
    const int32_t cappedUpdateTime = CapTime(updateTime);
    const int32_t cappedDrawTime = CapTime(drawTime);
    const int32_t cappedTotalTime = CapTime(totalTime);

    m_entries.push_back(ProfilerRecord(cappedUpdateTime, cappedDrawTime, cappedTotalTime));

    m_combinedTime.UpdateTime += cappedUpdateTime;
    m_combinedTime.DrawTime += cappedDrawTime;
    m_combinedTime.TotalTime += cappedTotalTime;

    if (m_entries.size() > m_maxCapacity)
    {
      const ProfilerRecord frontEntry = m_entries.front();
      m_entries.pop_front();

      m_combinedTime.UpdateTime -= frontEntry.UpdateTime;
      m_combinedTime.DrawTime -= frontEntry.DrawTime;
      m_combinedTime.TotalTime -= frontEntry.TotalTime;

      assert(m_combinedTime.UpdateTime >= 0);
      assert(m_combinedTime.DrawTime >= 0);
      assert(m_combinedTime.TotalTime >= 0);
    }
  }


  int32_t ProfilerService::ConvertHandleToIndex(const ProfilerCustomCounterHandle& handle) const
  {
    const uint16_t handleVersion = static_cast<uint16_t>(handle.Value & 0xFFFF);
    const uint32_t handleIndex = handle.Value >> 16;

    if (handleIndex >= m_customCounters.size() || !m_customCounters[handleIndex].IsInUse || m_customCounters[handleIndex].Version != handleVersion)
      throw std::invalid_argument("Unknown handle supplied");

    return handleIndex;
  }

}
