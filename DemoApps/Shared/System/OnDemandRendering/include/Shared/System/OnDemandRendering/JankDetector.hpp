#ifndef SHARED_SYSTEM_ONDEMANDRENDERING_JANKDETECTOR_HPP
#define SHARED_SYSTEM_ONDEMANDRENDERING_JANKDETECTOR_HPP
/****************************************************************************************************************************************************
 * Copyright 2021, 2023 NXP
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

#include <FslBase/Math/Pixel/PxValue.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslDemoApp/Shared/Host/DemoWindowMetrics.hpp>
#include <cassert>
#include <vector>

namespace Fsl
{
  struct DemoTime;
  struct DemoWindowMetrics;

  class JankDetector
  {
  public:
    struct AnimationRecord0
    {
      float SpeedPerSecond{0.0f};
      float FixedSpeedPerFrame{0.0f};

      // Camera is moving at a fixed speed per update (frame rate dependent)
      PxValue CameraPosition{0};
      // Box0 is moving at the same speed as the camera
      PxValue Box0Position{0};
      // Box1 is moving using delta time in fixed update
      PxValue Box1Position{0};
      // Box2 is moving using delta time in update
      PxValue Box2Position{0};
      // Box3 is moving using delta time in draw
      PxValue Box3Position{0};

      double AnimCamera{0};
      double AnimBox1{0};
      double AnimBox2{0};
      double AnimBox3{0};

      void Clear()
      {
        AnimCamera = 0.0f;
        AnimBox1 = 0.0f;
        AnimBox2 = 0.0f;
        AnimBox3 = 0.0f;
      }
    };

    struct AnimationRecord1
    {
      float SpeedPerSecond{0.0f};
      float FixedSpeedPerFrame{0.0f};

      // Box0 is moving at the same speed as the camera
      PxValue Box0Position{0};
      // Box1 is moving using delta time in fixed update
      PxValue Box1Position{0};
      // Box2 is moving using delta time in update
      PxValue Box2Position{0};
      // Box3 is moving using delta time in draw
      PxValue Box3Position{0};

      double AnimBox0{0};
      double AnimBox1{0};
      double AnimBox2{0};
      double AnimBox3{0};

      void Clear()
      {
        AnimBox0 = 0.0f;
        AnimBox1 = 0.0f;
        AnimBox2 = 0.0f;
        AnimBox3 = 0.0f;
      }
    };

    struct TimingRecords
    {
      // Box0 is moving at the same speed as the camera
      PxValue Box0Position{0};
      // Box1 is moving using delta time in fixed update
      PxValue Box1Position{0};
      // Box2 is moving using delta time in update
      PxValue Box2Position{0};
      // Box3 is moving using delta time in draw
      PxValue Box3Position{0};

      TimingRecords() = default;
      TimingRecords(const PxValue box0Position, const PxValue box1Position, const PxValue box2Position, const PxValue box3Position)
        : Box0Position(box0Position)
        , Box1Position(box1Position)
        , Box2Position(box2Position)
        , Box3Position(box3Position)
      {
      }
    };

  private:
    struct AnimationRecords
    {
      bool IsEnabled{false};
      uint32_t FrameCount{0};
      AnimationRecord0 Anim0;
      AnimationRecord1 Anim1;
      AnimationRecord1 Anim2;
    };

    struct Timing
    {
      std::vector<TimingRecords> Entries;
      std::size_t Count{0};
      std::size_t ValidEntries{0};

      void Clear()
      {
        Count = 0;
        ValidEntries = 0;
        assert(Count <= ValidEntries);
        assert(ValidEntries <= Entries.size());
        assert(Count <= Entries.size());
      }

      void EnsureCapacity(const std::size_t desiredCapacity)
      {
        if (desiredCapacity > Entries.size())
        {
          Entries.resize(desiredCapacity);
          Clear();
        }
        assert(Count <= ValidEntries);
        assert(ValidEntries <= Entries.size());
        assert(Count <= Entries.size());
      }

      void ClearCount()
      {
        Count = 0;
        assert(Count <= ValidEntries);
        assert(ValidEntries <= Entries.size());
        assert(Count <= Entries.size());
      }

      void Add(const TimingRecords& entry)
      {
        assert(Count <= ValidEntries);
        if (Count < Entries.size())
        {
          Entries[Count] = entry;
        }
        else
        {
          Entries.push_back(entry);
        }
        ++Count;
        if (Count > ValidEntries)
        {
          ValidEntries = Count;
        }
        assert(Count <= ValidEntries);
        assert(ValidEntries <= Entries.size());
        assert(Count <= Entries.size());
      }
    };

    DemoWindowMetrics m_cachedWindowMetrics;
    AnimationRecords m_animationRecords;
    Timing m_timing;

  public:
    explicit JankDetector();
    ~JankDetector();

    void ResetAnim();
    void OnConfigurationChanged(const DemoWindowMetrics& windowMetrics);
    void FixedUpdate(const DemoTime& demoTime, const bool animationEnabled, const uint32_t refreshRate);
    void OnUpdate(const DemoTime& demoTime);
    void OnDraw(const DemoTime& demoTime);

    AnimationRecord0 GetRecord0() const
    {
      return m_animationRecords.Anim0;
    }

    AnimationRecord1 GetRecord1() const
    {
      return m_animationRecords.Anim1;
    }

    AnimationRecord1 GetRecord2() const
    {
      return m_animationRecords.Anim2;
    }

    ReadOnlySpan<JankDetector::TimingRecords> Record1AsTimingSpan() const;
  };
}

#endif
