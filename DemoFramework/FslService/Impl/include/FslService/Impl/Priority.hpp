#ifndef FSLSERVICE_IMPL_PRIORITY_HPP
#define FSLSERVICE_IMPL_PRIORITY_HPP
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

namespace Fsl
{
  class Priority
  {
    int32_t m_value{0};

  public:
    using value_type = int32_t;

    constexpr Priority() noexcept = default;

    // @brief Create a new priority (higher value equals higher priority)
    explicit constexpr Priority(const int32_t priority) noexcept
      : m_value(priority)
    {
    }

    //! @brief Get the minimum priority
    static Priority Min();

    //! @brief Get the maximum priority
    static Priority Max();

    //! @brief Get the priority value
    constexpr int32_t GetValue() const noexcept
    {
      return m_value;
    }

    constexpr bool operator==(const Priority& val) const noexcept
    {
      return m_value == val.m_value;
    }
    constexpr bool operator!=(const Priority& val) const noexcept
    {
      return m_value != val.m_value;
    }
    constexpr bool operator<(const Priority& val) const noexcept
    {
      return m_value < val.m_value;
    }
    constexpr bool operator>(const Priority& val) const noexcept
    {
      return m_value > val.m_value;
    }
  };

  constexpr inline Priority operator-(const Priority& lhs, const int32_t rhs) noexcept
  {
    return Priority(lhs.GetValue() - rhs);
  }

  constexpr inline Priority operator+(const Priority& lhs, const int32_t rhs) noexcept
  {
    return Priority(lhs.GetValue() + rhs);
  }
}


#endif
