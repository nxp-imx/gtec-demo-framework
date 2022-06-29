#ifndef FSLBASE_SPAN_READONLYSPAN_HPP
#define FSLBASE_SPAN_READONLYSPAN_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022 NXP
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

#include <FslBase/Attributes.hpp>
#include <FslBase/Iterator/PointerConstIterator.hpp>
#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <type_traits>

namespace Fsl
{
  class ReadOnlySpanBase
  {
  public:
    constexpr ReadOnlySpanBase() noexcept = default;
    static constexpr std::size_t extent = static_cast<std::size_t>(-1);
  };

  // This is similar to C++20 Span
  template <typename T>
  class ReadOnlySpan : public ReadOnlySpanBase
  {
  public:
    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;

    using const_iterator = PointerConstIterator<T>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // Disabled and using a base class we can define (workaround until C++17)
    // static constexpr std::size_t extent = std::size_t(-1);

  protected:
    const_pointer m_pData{nullptr};
    size_type m_length{0u};

  public:
    constexpr ReadOnlySpan() noexcept = default;
    constexpr ReadOnlySpan(const ReadOnlySpan& other) noexcept = default;
    //! @brief overload that allows you to create a ReadOnlySpan from pointer and count that is noexcept.
    //!        only use this in cases where you are 100% sure that your input is valid
    explicit constexpr ReadOnlySpan(const_pointer pData, size_type count, const OptimizationCheckFlag /*unused*/) noexcept
      : m_pData(pData)
      , m_length(count)
    {
      assert(pData != nullptr || count == 0u);
    }

    explicit constexpr ReadOnlySpan(const_pointer pData, size_type count) noexcept
      : m_pData(pData)
      , m_length(pData != nullptr ? count : 0u)
    {
      FSLLOG3_DEBUG_INFO_IF(pData == nullptr && count != 0, "forcing count to zero");
    }


    constexpr const_pointer data() const noexcept
    {
      return m_pData;
    }

    constexpr size_type size() const noexcept
    {
      return m_length;
    }

    constexpr size_type byte_size() const noexcept
    {
      return m_length * sizeof(T);
    }

    constexpr bool empty() const noexcept
    {
      return m_length == 0;
    }

    constexpr const_reference operator[](size_type pos) const noexcept
    {
      assert(pos < size());
      assert(m_pData != nullptr);
      return m_pData[pos];
    }

    constexpr const_reference at(size_type pos) const
    {
      if (pos >= size())
      {
        throw std::out_of_range("pos out of range");
      }
      assert(m_pData != nullptr);
      return m_pData[pos];
    }

    constexpr size_type length() const noexcept
    {
      return m_length;
    }

    constexpr const_reference back() const noexcept
    {
      assert(!empty());
      return m_pData[m_length - 1];
    }

    constexpr const_reference front() const noexcept
    {
      assert(!empty());
      return m_pData[0];
    }

    //! @brief Returns a view of the subspan [pos, pos + rcount), where rcount is the smaller of count and size() - pos.
    constexpr ReadOnlySpan subspan(size_type pos = 0, size_type count = extent) const
    {
      if (pos > m_length)
      {
        throw std::out_of_range("pos out of range");
      }
      auto maxLength = (m_length - pos);
      return ReadOnlySpan(m_pData + pos, (count <= maxLength ? count : maxLength), OptimizationCheckFlag::NoCheck);
    }

    //! @brief Returns a view of the subspan [pos, pos + rcount), where rcount is the smaller of count and size() - pos.
    constexpr ReadOnlySpan unsafe_subspan(size_type pos = 0, size_type count = extent) const noexcept
    {
      assert(pos <= m_length);
      auto maxLength = (m_length - pos);
      return ReadOnlySpan(m_pData + pos, (count <= maxLength ? count : maxLength), OptimizationCheckFlag::NoCheck);
    }

    //! @note  This functionality is not present on C++20 span
    constexpr bool equals(ReadOnlySpan value) const
    {
      return m_length == value.m_length ? DoEquals(m_pData, value.m_pData, m_length) : false;
    }

    //! @brief compare the ReadOnlySpan to this ReadOnlySpan
    //! @note  This functionality is not present on C++20 span
    constexpr int compare(ReadOnlySpan value) const noexcept
    {
      const auto countCompare = m_length <= value.m_length ? m_length : value.m_length;
      const auto result = (countCompare == 0 ? 0 : DoCompare(m_pData, value.m_pData, countCompare));
      return (result == 0 ? (static_cast<int>(m_length > value.m_length) - static_cast<int>(m_length < value.m_length)) : (result < 0 ? -1 : 1));
    }

    //! @note  This functionality is not present on C++20 span
    constexpr int compare(size_type pos1, size_type count1, ReadOnlySpan v) const
    {
      return subspan(pos1, count1).compare(v);
    }

    //! @note  This functionality is not present on C++20 span
    constexpr int compare(size_type pos1, size_type count1, ReadOnlySpan v, size_type pos2, size_type count2) const
    {
      return subspan(pos1, count1).compare(v.subspan(pos2, count2));
    }

    constexpr bool starts_with(value_type value) const noexcept
    {
      return (m_length >= 1 ? m_pData[0] == value : false);
    }

    //! @note  This functionality is not present on C++20 span
    constexpr bool starts_with(ReadOnlySpan value) const noexcept
    {
      if (value.m_length <= m_length)
      {
        return unsafe_subspan(0, value.m_length).compare(value) == 0;
      }
      return false;
    }

    constexpr bool ends_with(value_type value) const noexcept
    {
      return (m_length >= 1 ? m_pData[m_length - 1] == value : false);
    }

    //! @note  This functionality is not present on C++20 span
    constexpr bool ends_with(ReadOnlySpan value) const noexcept
    {
      if (value.m_length <= m_length)
      {
        return unsafe_subspan(m_length - value.m_length, value.m_length).compare(value) == 0;
      }
      return false;
    }

    //! @note  This functionality is not present on C++20 span
    constexpr size_type find(const value_type ch, size_type pos = 0) const noexcept
    {
      while (pos < m_length && m_pData[pos] != ch)
      {
        ++pos;
      }
      return (pos < m_length ? pos : extent);
    }

    //! @note  This functionality is not present on C++20 span
    constexpr size_type rfind(value_type ch, size_type pos = extent) const noexcept
    {
      if (m_length > 0u)
      {
        const_pointer pCurrent = pos <= m_length ? (m_pData + pos) : ((m_pData + m_length) - 1u);
        while (pCurrent >= m_pData && *pCurrent != ch)
        {
          --pCurrent;
        }
        return (pCurrent >= m_pData ? static_cast<size_type>(pCurrent - m_pData) : extent);
      }
      return extent;
    }


    constexpr const_iterator begin() const noexcept
    {
#ifdef NDEBUG
      return const_iterator(m_pData);
#else
      return {m_pData, m_pData, m_length};
#endif
    }

    constexpr const_iterator end() const noexcept
    {
#ifdef NDEBUG
      return const_iterator(m_pData + m_length);
#else
      return {m_pData + m_length, m_pData, m_length};
#endif
    }

    constexpr const_iterator cbegin() const noexcept
    {
      return begin();
    }

    constexpr const_iterator cend() const noexcept
    {
      return end();
    }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
      return const_reverse_iterator(end());
    }


    constexpr const_reverse_iterator rend() const noexcept
    {
      return const_reverse_iterator(begin());
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
      return rbegin();
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
      return rend();
    }

  private:
    constexpr inline static bool DoEquals(const_pointer pDataLhs, const_pointer pDataRhs, const size_type count) noexcept
    {
      assert(pDataLhs != nullptr || count == 0);
      assert(pDataRhs != nullptr || count == 0);
      for (size_type i = 0; i < count; ++i)
      {
        if (pDataLhs[i] != pDataRhs[i])
        {
          return false;
        }
      }
      return true;
    }

    constexpr inline static int DoCompare(const_pointer pDataLhs, const_pointer pDataRhs, const size_type count) noexcept
    {
      assert(pDataLhs != nullptr || count == 0);
      assert(pDataRhs != nullptr || count == 0);
      for (size_type i = 0; i < count; ++i)
      {
        auto res = pDataLhs[i] - pDataRhs[i];
        if (res != 0)
        {
          return res;
        }
      }
      return 0;
    }
  };

  // Operator ==
  //! @note  This functionality is not present on C++20 span
  template <typename T>
  constexpr inline bool operator==(const ReadOnlySpan<T>& lhs, const ReadOnlySpan<T>& rhs) noexcept
  {
    return lhs.equals(rhs);
  }

  // Operator !=
  //! @note  This functionality is not present on C++20 span
  template <typename T>
  constexpr inline bool operator!=(const ReadOnlySpan<T>& lhs, const ReadOnlySpan<T>& rhs) noexcept
  {
    return !(lhs == rhs);
  }

  // Operator <
  //! @note  This functionality is not present on C++20 span
  template <typename T>
  constexpr inline bool operator<(const ReadOnlySpan<T>& lhs, const ReadOnlySpan<T>& rhs) noexcept
  {
    return lhs.compare(rhs) < 0;
  }

  // Operator <=
  //! @note  This functionality is not present on C++20 span
  template <typename T>
  constexpr inline bool operator<=(const ReadOnlySpan<T>& lhs, const ReadOnlySpan<T>& rhs) noexcept
  {
    return lhs.compare(rhs) <= 0;
  }

  // Operator >
  template <typename T>
  //! @note  This functionality is not present on C++20 span
  constexpr inline bool operator>(const ReadOnlySpan<T>& lhs, const ReadOnlySpan<T>& rhs) noexcept
  {
    return rhs < lhs;
  }

  // Operator >=
  template <typename T>
  //! @note  This functionality is not present on C++20 span
  constexpr inline bool operator>=(const ReadOnlySpan<T>& lhs, const ReadOnlySpan<T>& rhs) noexcept
  {
    return rhs <= lhs;
  }
}

#endif
