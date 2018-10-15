/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslBase/Transition/TransitionTimeSpan.hpp>
#include <FslBase/Exceptions.hpp>
#include <cassert>
#include <limits>

namespace Fsl
{
  TransitionTimeSpan::TransitionTimeSpan(const int32_t time, TransitionTimeUnit unit)
  {
    switch (unit)
    {
    case TransitionTimeUnit::Milliseconds:
      Ticks = static_cast<int64_t>(time) * 10000;
      break;
    case TransitionTimeUnit::Microseconds:
      Ticks = static_cast<int64_t>(time) * 10;
      break;
    default:
      throw NotSupportedException("The given time unit has not been implemented");
    }
  }

  TransitionTimeSpan::TransitionTimeSpan(const uint32_t time, TransitionTimeUnit unit)
  {
    switch (unit)
    {
    case TransitionTimeUnit::Milliseconds:
      Ticks = static_cast<int64_t>(time) * 10000;
      break;
    case TransitionTimeUnit::Microseconds:
      Ticks = static_cast<int64_t>(time) * 10;
      break;
    default:
      throw NotSupportedException("The given time unit has not been implemented");
    }
  }

  TransitionTimeSpan::TransitionTimeSpan(const int64_t time, TransitionTimeUnit unit)
  {
    switch (unit)
    {
    case TransitionTimeUnit::Milliseconds:
      assert(time <= (std::numeric_limits<int64_t>::max() / 10000));
      Ticks = time * 10000;
      break;
    case TransitionTimeUnit::Microseconds:
      assert(time <= (std::numeric_limits<int64_t>::max() / 10));
      Ticks = time * 10;
      break;
    default:
      throw NotSupportedException("The given time unit has not been implemented");
    }
  }

  TransitionTimeSpan::TransitionTimeSpan(const uint64_t time, TransitionTimeUnit unit)
  {
    switch (unit)
    {
    case TransitionTimeUnit::Milliseconds:
      assert(time <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max() / 10000));
      Ticks = time * 10000;
      break;
    case TransitionTimeUnit::Microseconds:
      assert(time <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max() / 10));
      Ticks = time * 10;
      break;
    default:
      throw NotSupportedException("The given time unit has not been implemented");
    }
  }
}
