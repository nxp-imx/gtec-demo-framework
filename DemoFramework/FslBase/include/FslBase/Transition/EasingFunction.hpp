#ifndef FSLBASE_TRANSITION_EASINGFUNCTION_HPP
#define FSLBASE_TRANSITION_EASINGFUNCTION_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

// Easing functions inspired by https://easings.net/

#include <FslBase/Math/MathHelper.hpp>
#include <cmath>

//! @note This should be true for all easing functions
//!       - a progress value of 0 will return 0
//!       - a progress value of 1 will return 1
namespace Fsl::EasingFunction
{
  //! @param progress A value in the range of 0 to 1
  inline float Linear(const float progress)
  {
    return progress;
  }

  //! @param progress A value in the range of 0 to 1
  inline float EaseInSine(const float progress)
  {
    return 1.0f - std::cos((progress * MathHelper::PI) / 2.0f);
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseOutSine(const float progress)
  {
    return std::sin((progress * MathHelper::PI) / 2.0f);
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseInOutSine(const float progress)
  {
    return -(std::cos(MathHelper::PI * progress) - 1.0f) / 2.0f;
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseInQuad(const float progress)
  {
    return progress * progress;
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseOutQuad(const float progress)
  {
    return 1.0f - (1.0f - progress) * (1.0f - progress);
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseInOutQuad(const float progress)
  {
    // progress < 0.5f ? (2.0f * progress * progress) : (1.0f - (std::pow((-2.0f * progress) + 2.0f, 2.0f) / 2.0f));
    if (progress < 0.5f)
    {
      return (2.0f * progress * progress);
    }
    const float tmp = (-2.0f * progress) + 2.0f;
    return (1.0f - ((tmp * tmp) / 2.0f));
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseInCubic(const float progress)
  {
    return progress * progress * progress;
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseOutCubic(const float progress)
  {
    // 1.0f - std::pow(1.0f - progress, 3.0f);
    const float tmp = 1.0f - progress;
    return 1.0f - (tmp * tmp * tmp);
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseInOutCubic(const float progress)
  {
    // progress < 0.5f ? (4.0f * progress * progress * progress) : 1.0f - (std::pow((-2.0f * progress) + 2.0f, 3.0f) / 2.0f);
    if (progress < 0.5f)
    {
      return (4.0f * progress * progress * progress);
    }
    const float tmp = (-2.0f * progress) + 2.0f;
    return 1.0f - ((tmp * tmp * tmp) / 2.0f);
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseInQuart(const float progress)
  {
    return progress * progress * progress * progress;
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseOutQuart(const float progress)
  {
    // 1.0f - std::pow(1.0f - progress, 4.0f);
    const float tmp = 1.0f - progress;
    return 1.0f - (tmp * tmp * tmp * tmp);
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseInOutQuart(const float progress)
  {
    // progress < 0.5f ? (8.0f * progress * progress * progress * progress) : (1.0f - (std::pow((-2.0f * progress) + 2.0f, 4.0f) / 2.0f));
    if (progress < 0.5f)
    {
      return 8.0f * progress * progress * progress * progress;
    }
    const float tmp = (-2.0f * progress) + 2.0f;
    return 1.0f - ((tmp * tmp * tmp * tmp) / 2.0f);
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseInQuint(const float progress)
  {
    return progress * progress * progress * progress * progress;
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseOutQuint(const float progress)
  {
    // 1.0f - std::pow(1.0f - progress, 5.0f);
    const float tmp = 1.0f - progress;
    return 1.0f - (tmp * tmp * tmp * tmp * tmp);
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseInOutQuint(const float progress)
  {
    // progress < 0.5f ? (16.0f * progress * progress * progress * progress * progress) : 1.0f - (std::pow(-2.0f * progress + 2.0f, 5.0f) / 2.0f);
    if (progress < 0.5f)
    {
      return 16.0f * progress * progress * progress * progress * progress;
    }
    const float tmp = -2.0f * progress + 2.0f;
    return 1.0f - ((tmp * tmp * tmp * tmp * tmp) / 2.0f);
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseInExpo(const float progress)
  {
    return progress == 0.0f ? 0.0f : std::pow(2.0f, (10.0f * progress) - 10.0f);
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseOutExpo(const float progress)
  {
    return progress == 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * progress);
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseInOutExpo(const float progress)
  {
    if (progress <= 0.0f)
    {
      return 0.0f;
    }
    if (progress >= 1.0f)
    {
      return 1.0f;
    }
    const float progress20 = 20.0f * progress;
    return progress < 0.5f ? std::pow(2.0f, progress20 - 10.0f) / 2.0f : (2.0f - std::pow(2.0f, -progress20 + 10.0f)) / 2.0f;
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseInCirc(const float progress)
  {
    // 1.0f - std::sqrt(1.0f - std::pow(progress, 2.0f));
    return 1.0f - std::sqrt(1.0f - (progress * progress));
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseOutCirc(const float progress)
  {
    // std::sqrt(1.0f - std::pow(progress - 1.0f, 2.0f));
    const float tmp = progress - 1.0f;
    return std::sqrt(1.0f - (tmp * tmp));
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseInOutCirc(const float progress)
  {
    // progress < 0.5f ? (1.0f - std::sqrt(1.0f - std::pow(2.0f * progress, 2.0f))) / 2.0f
    //                 : (std::sqrt(1.0f - std::pow((-2.0f * progress) + 2.0f, 2.0f)) + 1.0f) / 2.0f;

    if (progress < 0.5f)
    {
      const float progress2 = 2.0f * progress;
      return (1.0f - std::sqrt(1.0f - (progress2 * progress2))) / 2.0f;
    }
    const float tmp = (-2.0f * progress) + 2.0f;
    return (std::sqrt(1.0f - (tmp * tmp)) + 1.0f) / 2.0f;
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseInBack(const float progress)
  {
    constexpr float C1 = 1.70158f;
    constexpr float C3 = C1 + 1.0f;
    return (C3 * progress * progress * progress) - (C1 * progress * progress);
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseOutBack(const float progress)
  {
    constexpr float C1 = 1.70158f;
    constexpr float C3 = C1 + 1.0f;
    // 1.0f + (c3 * std::pow(progress - 1.0f, 3)) + (c1 * std::pow(progress - 1.0f, 2));
    const float tmp = progress - 1.0f;
    return 1.0f + (C3 * (tmp * tmp * tmp)) + (C1 * (tmp * tmp));
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseInOutBack(const float progress)
  {
    constexpr float C1 = 1.70158f;
    constexpr float C2 = C1 * 1.525f;
    // progress < 0.5f ? (std::pow(2.0f * progress, 2.0f) * ((c2 + 1.0f) * 2.0f * progress - c2)) / 2.0f
    //                 : (std::pow(2.0f * progress - 2.0f, 2.0f) * ((c2 + 1.0f) * (progress * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
    if (progress < 0.5f)
    {
      const float progress2 = 2.0f * progress;
      return (progress2 * progress2 * (((C2 + 1.0f) * 2.0f * progress) - C2)) / 2.0f;
    }
    const float tmp = (2.0f * progress) - 2.0f;
    return ((tmp * tmp * (((C2 + 1.0f) * tmp) + C2)) + 2.0f) / 2.0f;
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseInElastic(const float progress)
  {
    if (progress <= 0.0f)
    {
      return 0.0f;
    }
    if (progress >= 1.0f)
    {
      return 1.0f;
    }
    constexpr float C4 = (2.0f * MathHelper::PI) / 3.0f;
    const float progress10 = progress * 10.0f;
    return -std::pow(2.0f, progress10 - 10.0f) * std::sin((progress10 - 10.75f) * C4);
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseOutElastic(const float progress)
  {
    if (progress <= 0.0f)
    {
      return 0.0f;
    }
    if (progress >= 1.0f)
    {
      return 1.0f;
    }
    constexpr float C4 = (2.0f * MathHelper::PI) / 3.0f;
    const float progress10 = progress * 10.0f;
    return std::pow(2.0f, -progress10) * std::sin((progress10 - 0.75f) * C4) + 1.0f;
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseInOutElastic(const float progress)
  {
    if (progress <= 0.0f)
    {
      return 0.0f;
    }
    if (progress >= 1.0f)
    {
      return 1.0f;
    }
    constexpr float C5 = (2.0f * MathHelper::PI) / 4.5f;
    const float progress20 = 20.f * progress;
    return progress < 0.5f ? -(std::pow(2.0f, progress20 - 10.0f) * std::sin((progress20 - 11.125f) * C5)) / 2.0f
                           : ((std::pow(2.0f, -progress20 + 10.0f) * std::sin((progress20 - 11.125f) * C5)) / 2.0f) + 1.0f;
  }


  // forward declare
  float EaseOutBounce(const float progress);


  //! @param progress A value in the range of 0 to 1
  inline float EaseInBounce(const float progress)
  {
    return 1.0f - EaseOutBounce(1.0f - progress);
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseOutBounce(const float progress)
  {
    constexpr float N1 = 7.5625f;
    constexpr float D1 = 2.75f;

    if (progress < (1.0f / D1))
    {
      return N1 * progress * progress;
    }
    if (progress < (2.0f / D1))
    {
      const float tmp = progress - (1.5f / D1);
      return (N1 * tmp * tmp) + 0.75f;
    }
    if (progress < 2.5 / D1)
    {
      const float tmp = progress - (2.25f / D1);
      return (N1 * tmp * tmp) + 0.9375f;
    }
    const float tmp = progress - (2.625f / D1);
    return (N1 * tmp * tmp) + 0.984375f;
  }


  //! @param progress A value in the range of 0 to 1
  inline float EaseInOutBounce(const float progress)
  {
    return progress < 0.5f ? (1.0f - EaseOutBounce(1.0f - (2 * progress))) / 2.0f : (1.0f + EaseOutBounce((2.0f * progress) - 1.0f)) / 2.0f;
  }

}

#endif
