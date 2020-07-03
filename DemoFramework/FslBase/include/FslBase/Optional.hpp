#ifndef FSLBASE_OPTIONAL_HPP
#define FSLBASE_OPTIONAL_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <cassert>
#include <exception>
#include <utility>

namespace Fsl
{
  class BadOptionalAccess : std::exception
  {
  public:
  };

  //! A temporary solution until C++17 std::optional
  template <typename T>
  class Optional
  {
    bool m_hasValue{false};
    T m_value{};

  public:
    using value_type = T;

    constexpr Optional() noexcept = default;

    Optional(const Optional& src) = default;
    Optional(Optional&& src) noexcept = default;

    explicit Optional(const T& value)
      : m_hasValue(true)
      , m_value(value)
    {
    }

    explicit Optional(T&& src) noexcept
      : m_hasValue(true)
      , m_value(std::move(src))
    {
    }

    constexpr Optional& operator=(const Optional& other) = default;
    constexpr Optional& operator=(Optional&& other) noexcept = default;

    Optional& operator=(const T& value)
    {
      m_hasValue = true;
      m_value = value;
      return *this;
    }

    Optional& operator=(T&& value) noexcept
    {
      m_hasValue = true;
      m_value = std::move(value);
      return *this;
    }

    constexpr T& Value() &
    {
      if (!m_hasValue)
      {
        throw BadOptionalAccess();
      }
      return m_value;
    }

    constexpr const T& Value() const&
    {
      if (!m_hasValue)
      {
        throw BadOptionalAccess();
      }
      return m_value;
    }

    constexpr T&& Value() &&
    {
      if (!m_hasValue)
      {
        throw BadOptionalAccess();
      }
      return m_value;
    }

    constexpr const T&& Value() const&&
    {
      if (!m_hasValue)
      {
        throw BadOptionalAccess();
      }
      return m_value;
    }


    template <class U>
    constexpr T ValueOr(U&& defaultValue) const&
    {
      return m_hasValue ? m_value : defaultValue;
    }

    template <class U>
    constexpr T ValueOr(U&& defaultValue) &&
    {
      if (m_hasValue)
      {
        m_hasValue = false;
        return std::move(m_value);
      }
      return std::forward<U>(defaultValue);
    }

    constexpr bool HasValue() const noexcept
    {
      return m_hasValue;
    }

    constexpr explicit operator bool() const noexcept
    {
      return m_hasValue;
    }

    constexpr const T* operator->() const
    {
      assert(m_hasValue);
      return &m_value;
    }

    constexpr T* operator->()
    {
      assert(m_hasValue);
      return &m_value;
    }

    constexpr const T& operator*() const&
    {
      assert(m_hasValue);
      return m_value;
    }

    constexpr T& operator*() &
    {
      assert(m_hasValue);
      return m_value;
    }

    constexpr const T&& operator*() const&&
    {
      assert(m_hasValue);
      return m_value;
    }

    constexpr T&& operator*() &&
    {
      assert(m_hasValue);
      return m_value;
    }
  };

  template <class T, class U>
  constexpr bool operator==(const Optional<T>& lhs, const Optional<U>& rhs)
  {
    if (bool(lhs) != bool(rhs))
    {
      return false;
    }
    if (!bool(lhs))
    {
      return true;
    }
    return *lhs == *rhs;
  }

  template <class T, class U>
  constexpr bool operator!=(const Optional<T>& lhs, const Optional<U>& rhs)
  {
    return !(lhs == rhs);
  }

  template <class T, class U>
  constexpr bool operator<(const Optional<T>& lhs, const Optional<U>& rhs)
  {
    if (!bool(rhs))
    {
      return false;
    }
    if (!bool(lhs))
    {
      return true;
    }
    return *lhs < *rhs;
  }

  template <class T, class U>
  constexpr bool operator<=(const Optional<T>& lhs, const Optional<U>& rhs)
  {
    if (!bool(lhs))
    {
      return true;
    }
    if (!bool(rhs))
    {
      return false;
    }
    return *lhs <= *rhs;
  }

  template <class T, class U>
  constexpr bool operator>(const Optional<T>& lhs, const Optional<U>& rhs)
  {
    if (!bool(lhs))
    {
      return false;
    }
    if (!bool(rhs))
    {
      return true;
    }
    return *lhs > *rhs;
  }

  template <class T, class U>
  constexpr bool operator>=(const Optional<T>& lhs, const Optional<U>& rhs)
  {
    if (!bool(rhs))
    {
      return true;
    }
    if (!bool(lhs))
    {
      return false;
    }
    return *lhs >= *rhs;
  }


  template <class T, class U>
  constexpr bool operator==(const Optional<T>& opt, const U& value)
  {
    return bool(opt) ? *opt == value : false;
  }

  template <class T, class U>
  constexpr bool operator==(const T& value, const Optional<U>& opt)
  {
    return bool(opt) ? value == *opt : false;
  }

  template <class T, class U>
  constexpr bool operator!=(const Optional<T>& opt, const U& value)
  {
    return bool(opt) ? *opt != value : true;
  }

  template <class T, class U>
  constexpr bool operator!=(const T& value, const Optional<U>& opt)
  {
    return bool(opt) ? value != *opt : true;
  }

  template <class T, class U>
  constexpr bool operator<(const Optional<T>& opt, const U& value)
  {
    return bool(opt) ? *opt < value : true;
  }

  template <class T, class U>
  constexpr bool operator<(const T& value, const Optional<U>& opt)
  {
    return bool(opt) ? value < *opt : false;
  }

  template <class T, class U>
  constexpr bool operator<=(const Optional<T>& opt, const U& value)
  {
    return bool(opt) ? *opt <= value : true;
  }

  template <class T, class U>
  constexpr bool operator<=(const T& value, const Optional<U>& opt)
  {
    return bool(opt) ? value <= *opt : false;
  }

  template <class T, class U>
  constexpr bool operator>(const Optional<T>& opt, const U& value)
  {
    return bool(opt) ? *opt > value : false;
  }

  template <class T, class U>
  constexpr bool operator>(const T& value, const Optional<U>& opt)
  {
    return bool(opt) ? value > *opt : true;
  }

  template <class T, class U>
  constexpr bool operator>=(const Optional<T>& opt, const U& value)
  {
    return bool(opt) ? *opt >= value : false;
  }

  template <class T, class U>
  constexpr bool operator>=(const T& value, const Optional<U>& opt)
  {
    return bool(opt) ? value >= *opt : true;
  }
}

#endif
