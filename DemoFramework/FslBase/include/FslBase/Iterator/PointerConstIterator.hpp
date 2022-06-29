#ifndef FSLBASE_ITERATOR_POINTERCONSTITERATOR_HPP
#define FSLBASE_ITERATOR_POINTERCONSTITERATOR_HPP
/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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
#include <cstddef>
#include <iterator>
#include <type_traits>

#ifdef NDEBUG
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSL_CUSTOM_ITERATOR_ASSERT_CHECK(X) \
  {                                         \
  }
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSL_CUSTOM_ITERATOR_CHECK_ENABLED 1
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSL_CUSTOM_ITERATOR_ASSERT_CHECK(X) assert((X))
#endif

namespace Fsl
{
  template <typename T>
  struct PointerIterator;

  template <typename T>
  struct PointerConstIterator
  {
    friend struct PointerIterator<T>;

    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;

    using pointer = value_type const*;
    using reference = value_type const&;

  private:
    pointer m_pData{nullptr};

#ifdef FSL_CUSTOM_ITERATOR_CHECK_ENABLED
    pointer m_pDataStart{nullptr};
    pointer m_pDataEnd{nullptr};

    explicit constexpr PointerConstIterator(pointer pData, pointer pDataStart, pointer pDataEnd) noexcept
      : m_pData(pData)
      , m_pDataStart(pDataStart)
      , m_pDataEnd(pDataEnd)
    {
      assert(pDataStart <= pDataEnd);
    }
#endif

  public:
    constexpr PointerConstIterator() noexcept = default;

#ifndef FSL_CUSTOM_ITERATOR_CHECK_ENABLED
    explicit constexpr PointerConstIterator(pointer pData) noexcept
      : m_pData(pData)
    {
    }
#else
    constexpr PointerConstIterator(pointer pData, pointer pDataStart, const std::size_t size) noexcept
      : m_pData(pData)
      , m_pDataStart(pDataStart)
      , m_pDataEnd(pDataStart + size)
    {
      assert(m_pDataStart <= m_pDataEnd);
    }
#endif

    // Pointer like functionality

    constexpr reference operator*() const noexcept
    {
      FSL_CUSTOM_ITERATOR_ASSERT_CHECK(m_pData >= m_pDataStart && m_pData < m_pDataEnd);
      assert(m_pData != nullptr);
      return *m_pData;
    }

    constexpr pointer operator->() noexcept
    {
      FSL_CUSTOM_ITERATOR_ASSERT_CHECK(m_pData >= m_pDataStart && m_pData < m_pDataEnd);
      assert(m_pData != nullptr);
      return m_pData;
    }

    // Direct access

    reference operator[](difference_type index) const
    {
      FSL_CUSTOM_ITERATOR_ASSERT_CHECK(m_pData >= m_pDataStart && m_pData < m_pDataEnd);
      FSL_CUSTOM_ITERATOR_ASSERT_CHECK((m_pData + index) < m_pDataEnd);
      assert(m_pData != nullptr);
      return m_pData[index];
    }

    // Inc / dec

    constexpr PointerConstIterator& operator++() noexcept
    {
      FSL_CUSTOM_ITERATOR_ASSERT_CHECK(m_pData >= m_pDataStart && m_pData < m_pDataEnd);
      assert(m_pData != nullptr);
      ++m_pData;
      return *this;
    }


    // NOLINTNEXTLINE(readability-const-return-type)
    constexpr const PointerConstIterator operator++(int) noexcept
    {
      FSL_CUSTOM_ITERATOR_ASSERT_CHECK(m_pData >= m_pDataStart && m_pData < m_pDataEnd);
      PointerConstIterator tmp = *this;
      ++(*this);
      return tmp;
    }

    constexpr PointerConstIterator& operator--() noexcept
    {
      FSL_CUSTOM_ITERATOR_ASSERT_CHECK(m_pData >= m_pDataStart && m_pData <= m_pDataEnd);
      assert(m_pData != nullptr);
      --m_pData;
      return *this;
    }

    // NOLINTNEXTLINE(readability-const-return-type)
    constexpr const PointerConstIterator operator--(int) noexcept
    {
      FSL_CUSTOM_ITERATOR_ASSERT_CHECK(m_pData >= m_pDataStart && m_pData <= m_pDataEnd);
      PointerConstIterator tmp = *this;
      --(*this);
      return tmp;
    }

    // Other math

    constexpr PointerConstIterator& operator+=(difference_type offset) noexcept
    {
      FSL_CUSTOM_ITERATOR_ASSERT_CHECK(m_pData >= m_pDataStart && m_pData < m_pDataEnd);
      FSL_CUSTOM_ITERATOR_ASSERT_CHECK((m_pData + offset) <= m_pDataEnd);
      assert(m_pData != nullptr);
      m_pData += offset;
      return *this;
    }

    constexpr PointerConstIterator operator+(difference_type offset) const noexcept
    {
#ifndef FSL_CUSTOM_ITERATOR_CHECK_ENABLED
      return PointerConstIterator(m_pData + offset);
#else
      assert(m_pDataStart <= m_pDataEnd);
      // Allow 'just' out of bounds at end
      assert(m_pData >= m_pDataStart && (m_pData + offset) <= m_pDataEnd);
      return PointerConstIterator(m_pData + offset, m_pDataStart, m_pDataEnd);
#endif
    }

    constexpr friend PointerConstIterator operator+(difference_type offset, const PointerConstIterator& rhs) noexcept
    {
#ifndef FSL_CUSTOM_ITERATOR_CHECK_ENABLED
      return PointerConstIterator(offset + rhs.m_pData);
#else
      assert(rhs.m_pDataStart <= rhs.m_pDataEnd);
      // Allow 'just' out of bounds at end
      assert(rhs.m_pData >= rhs.m_pDataStart && (rhs.m_pData + offset) <= rhs.m_pDataEnd);
      return PointerConstIterator(offset + rhs.m_pData, rhs.m_pDataStart, rhs.m_pDataEnd);
#endif
    }

    constexpr PointerConstIterator& operator-=(difference_type offset) noexcept
    {
      FSL_CUSTOM_ITERATOR_ASSERT_CHECK(m_pData >= m_pDataStart && m_pData <= m_pDataEnd);
      FSL_CUSTOM_ITERATOR_ASSERT_CHECK((m_pData + 1 - offset) >= m_pDataStart);
      m_pData -= offset;
      return *this;
    }

    constexpr PointerConstIterator operator-(difference_type offset) const noexcept
    {
#ifndef FSL_CUSTOM_ITERATOR_CHECK_ENABLED
      return PointerConstIterator(m_pData - offset);
#else
      assert(m_pDataStart <= m_pDataEnd);
      // Allow 'just' out of bounds at start
      assert((m_pData + 1 - offset) >= m_pDataStart && m_pData <= m_pDataEnd);
      return PointerConstIterator(m_pData - offset, m_pDataStart, m_pDataEnd);
#endif
    }

    constexpr difference_type operator-(const PointerConstIterator& rhs) const noexcept
    {
      FSL_CUSTOM_ITERATOR_ASSERT_CHECK(m_pData >= m_pDataStart && m_pData <= m_pDataEnd);
      assert(m_pData != nullptr);
      assert(rhs.m_pData != nullptr);
      return m_pData - rhs.m_pData;
    }


    // Comparison

    constexpr bool operator==(const PointerConstIterator& rhs) const noexcept
    {
      return m_pData == rhs.m_pData;
    }

    constexpr bool operator!=(const PointerConstIterator& rhs) const noexcept
    {
      return m_pData != rhs.m_pData;
    }

    constexpr bool operator<(const PointerConstIterator& rhs) const noexcept
    {
      return m_pData < rhs.m_pData;
    }

    constexpr bool operator<=(const PointerConstIterator& rhs) const noexcept
    {
      return m_pData <= rhs.m_pData;
    }

    constexpr bool operator>(const PointerConstIterator& rhs) const noexcept
    {
      return m_pData > rhs.m_pData;
    }

    constexpr bool operator>=(const PointerConstIterator& rhs) const noexcept
    {
      return m_pData >= rhs.m_pData;
    }
  };
}

#endif
