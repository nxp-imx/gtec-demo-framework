#ifndef FSLBASE_SPAN_SPAN_HPP
#define FSLBASE_SPAN_SPAN_HPP
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

#include <FslBase/Iterator/PointerIterator.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>

namespace Fsl
{
  // This is similar to C++20 Span
  template <typename T>
  class Span : public ReadOnlySpanBase
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


    using iterator = PointerIterator<T>;
    using const_iterator = PointerConstIterator<T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  protected:
    pointer m_pData{nullptr};
    size_type m_length{0u};

  public:
    constexpr Span() noexcept = default;
    constexpr Span(const Span& other) noexcept = default;
    //! @brief overload that allows you to create a Span from pointer and count that is noexcept.
    //!        only use this in cases where you are 100% sure that your input is valid
    explicit constexpr Span(pointer pData, size_type count, const OptimizationCheckFlag /*unused*/) noexcept
      : m_pData(pData)
      , m_length(count)
    {
      assert(pData != nullptr || count == 0u);
    }

    explicit constexpr Span(pointer pData, size_type count) noexcept
      : m_pData(pData)
      , m_length(pData != nullptr ? count : 0u)
    {
      FSLLOG3_DEBUG_INFO_IF(pData == nullptr && count != 0, "forcing count to zero");
    }


    constexpr const_pointer data() const noexcept
    {
      return m_pData;
    }

    constexpr pointer data() noexcept
    {
      return m_pData;
    }

    constexpr size_type size() const noexcept
    {
      return m_length;
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

    constexpr reference operator[](size_type pos) noexcept
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

    constexpr reference at(size_type pos)
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

    constexpr reference back() noexcept
    {
      assert(!empty());
      return m_pData[m_length - 1];
    }

    constexpr const_reference front() const noexcept
    {
      assert(!empty());
      return m_pData[0];
    }

    constexpr reference front() noexcept
    {
      assert(!empty());
      return m_pData[0];
    }

    constexpr size_type write(const ReadOnlySpan<T>& srcSpan)
    {
      if (srcSpan.length() > m_length)
      {
        throw std::out_of_range("span can not contain the srcSpan");
      }
      for (size_type i = 0; i < srcSpan.length(); ++i)
      {
        m_pData[i] = srcSpan[i];
      }
      return srcSpan.length();
    }

    constexpr size_type write(const size_type dstIndex, const ReadOnlySpan<T>& srcSpan)
    {
      assert(dstIndex < m_length);
      return subspan(dstIndex).write(srcSpan);
    }

    //! @brief Returns a view of the subspan [pos, pos + rcount), where rcount is the smaller of count and size() - pos.
    constexpr Span subspan(size_type pos = 0, size_type count = extent) const
    {
      if (pos > m_length)
      {
        throw std::out_of_range("pos out of range");
      }
      auto maxLength = (m_length - pos);
      return Span(m_pData + pos, (count <= maxLength ? count : maxLength), OptimizationCheckFlag::NoCheck);
    }

    //! @brief compare the Span to this Span
    //! @note  This functionality is not present on C++20 span
    constexpr int compare(Span value) const noexcept
    {
      return ReadOnlySpan<T>(m_pData, m_length).compare(value);
    }

    //! @note  This functionality is not present on C++20 span
    int compare(size_type pos1, size_type count1, Span v) const
    {
      return subspan(pos1, count1).compare(v);
    }

    //! @note  This functionality is not present on C++20 span
    int compare(size_type pos1, size_type count1, Span v, size_type pos2, size_type count2) const
    {
      return subspan(pos1, count1).compare(v.subspan(pos2, count2));
    }

    //! implicit conversion to ReadOnlySpan
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr operator ReadOnlySpan<T>() const noexcept
    {
      return ReadOnlySpan<T>(m_pData, m_length, OptimizationCheckFlag::NoCheck);
    }

    [[deprecated("Use AsReadOnlySpan")]] constexpr ReadOnlySpan<T> ToReadOnlySpan() const
    {
      return ReadOnlySpan<T>(m_pData, m_length);
    }

    constexpr ReadOnlySpan<T> AsReadOnlySpan() const noexcept
    {
      return ReadOnlySpan<T>(m_pData, m_length, OptimizationCheckFlag::NoCheck);
    }

    //! @brief Returns a view of the subspan [pos, pos + rcount), where rcount is the smaller of count and size() - pos.
    constexpr ReadOnlySpan<T> AsReadOnlySpan(size_type pos, size_type count = extent) const
    {
      return ReadOnlySpan<T>(m_pData, m_length, OptimizationCheckFlag::NoCheck).subspan(pos, count);
    }

    constexpr iterator begin() noexcept
    {
#ifdef NDEBUG
      return iterator(m_pData);
#else
      return {m_pData, m_pData, m_length};
#endif
    }


    constexpr iterator end() noexcept
    {
#ifdef NDEBUG
      return iterator(m_pData + m_length);
#else
      return {m_pData + m_length, m_pData, m_length};
#endif
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

    constexpr reverse_iterator rbegin() noexcept
    {
      return reverse_iterator(end());
    }


    constexpr reverse_iterator rend() noexcept
    {
      return reverse_iterator(begin());
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
    constexpr int DoCompare(const_pointer pDataLhs, const_pointer pDataRhs, const size_type count) const noexcept
    {
      assert(count >= 0);
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
  inline bool operator==(const Span<T>& lhs, const Span<T>& rhs) noexcept
  {
    return lhs.compare(rhs) == 0;
  }

  // Operator !=
  //! @note  This functionality is not present on C++20 span
  template <typename T>
  inline bool operator!=(const Span<T>& lhs, const Span<T>& rhs) noexcept
  {
    return !(lhs == rhs);
  }

  // Operator <
  //! @note  This functionality is not present on C++20 span
  template <typename T>
  inline bool operator<(const Span<T>& lhs, const Span<T>& rhs) noexcept
  {
    return lhs.compare(rhs) < 0;
  }

  // Operator <=
  //! @note  This functionality is not present on C++20 span
  template <typename T>
  inline bool operator<=(const Span<T>& lhs, const Span<T>& rhs) noexcept
  {
    return lhs.compare(rhs) <= 0;
  }

  // Operator >
  template <typename T>
  //! @note  This functionality is not present on C++20 span
  inline bool operator>(const Span<T>& lhs, const Span<T>& rhs) noexcept
  {
    return rhs < lhs;
  }

  // Operator >=
  template <typename T>
  //! @note  This functionality is not present on C++20 span
  inline bool operator>=(const Span<T>& lhs, const Span<T>& rhs) noexcept
  {
    return rhs <= lhs;
  }
}

#endif
