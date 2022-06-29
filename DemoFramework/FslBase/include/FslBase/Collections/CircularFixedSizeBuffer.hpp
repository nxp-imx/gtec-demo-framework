#ifndef FSLBASE_COLLECTIONS_CIRCULARFIXEDSIZEBUFFER_HPP
#define FSLBASE_COLLECTIONS_CIRCULARFIXEDSIZEBUFFER_HPP
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslBase/Span/ReadOnlySpan.hpp>
#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

namespace Fsl
{
  //! @brief implements a circular fixed size buffer
  //         Once the capacity has been reached and a new element is inserted we then remove the element at the other end to make room.
  //         - So a push_back on a full buffer will do a pop_front before storing the new element at the end.
  //         - So a push_front on a full buffer will do a pop_back before storing the new element at the end.
  template <typename T>
  class CircularFixedSizeBuffer
  {
  public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;

  private:
    std::vector<T> m_data;
    size_type m_frontIndex{0u};
    size_type m_entries{0u};

  public:
    explicit CircularFixedSizeBuffer(size_type count)
      : m_data(count)
    {
      if (count <= 0u)
      {
        throw std::invalid_argument("count must be greater than zero");
      }
    }

    const_reference front() const
    {
      // There must be at least one entry for the front call to be valid
      assert(m_entries > 0u);
      // Check integrity of internal values
      assert(m_frontIndex < m_data.size());
      assert(m_entries <= m_data.size());
      return m_data[m_frontIndex];
    }

    reference front()
    {
      // There must be at least one entry for the front call to be valid
      assert(m_entries > 0u);
      // Check integrity of internal values
      assert(m_frontIndex < m_data.size());
      assert(m_entries <= m_data.size());
      return m_data[m_frontIndex];
    }

    const_reference back() const
    {
      // There must be at least one entry for the back call to be valid
      assert(m_entries > 0u);
      // Check integrity of internal values
      assert(m_frontIndex < m_data.size());
      assert(m_entries <= m_data.size());
      return m_data[(m_frontIndex + m_entries - 1u) % m_data.size()];
    }

    reference back()
    {
      // There must be at least one entry for the back call to be valid
      assert(m_entries > 0u);
      // Check integrity of internal values
      assert(m_frontIndex < m_data.size());
      assert(m_entries <= m_data.size());
      return m_data[(m_frontIndex + m_entries - 1u) % m_data.size()];
    }

    reference operator[](size_type pos)
    {
      // There must be at least one entry for the operator call to be valid
      assert(m_entries > 0u);
      // pos must be a valid index
      assert(pos < m_entries);
      // Check integrity of internal values
      assert(m_frontIndex < m_data.size());
      assert(m_entries <= m_data.size());
      return m_data[(m_frontIndex + pos) % m_data.size()];
    }

    const_reference operator[](size_type pos) const
    {
      // There must be at least one entry for the operator call to be valid
      assert(m_entries > 0u);
      // pos must be a valid index
      assert(pos < m_entries);
      // Check integrity of internal values
      assert(m_frontIndex < m_data.size());
      assert(m_entries <= m_data.size());
      return m_data[(m_frontIndex + pos) % m_data.size()];
    }

    reference at(size_type pos)
    {
      if (pos >= size())
      {
        throw std::out_of_range("index out of bounds");
      }
      // Check integrity of internal values
      assert(m_frontIndex < m_data.size());
      assert(m_entries <= m_data.size());
      return m_data.at((m_frontIndex + pos) % m_data.size());
    }

    const_reference at(size_type pos) const
    {
      if (pos >= size())
      {
        throw std::out_of_range("index out of bounds");
      }
      // Check integrity of internal values
      assert(m_frontIndex < m_data.size());
      assert(m_entries <= m_data.size());
      return m_data.at((m_frontIndex + pos) % m_data.size());
    }

    bool empty() const noexcept
    {
      return m_entries <= 0u;
    }

    size_type size() const noexcept
    {
      return m_entries;
    }

    size_type capacity() const noexcept
    {
      return m_data.size();
    }

    void clear() noexcept
    {
      while (m_entries > 0u)
      {
        pop_back();
      }
      assert(m_entries == 0u);
      m_frontIndex = 0u;
    }

    void push_back(const T& value)
    {
      if (m_entries >= m_data.size())
      {
        // The buffer has reached max capacity so we pop the front element to make room for a new one at the back
        pop_front();
      }
      assert(m_entries < m_data.size());
      m_data[(m_frontIndex + m_entries) % m_data.size()] = value;
      ++m_entries;
      assert(m_entries <= m_data.size());
    }

    void push_back(T&& value)
    {
      if (m_entries >= m_data.size())
      {
        // The buffer has reached max capacity so we pop the front element to make room for a new one at the back
        pop_front();
      }
      assert(m_entries < m_data.size());
      m_data[(m_frontIndex + m_entries) % m_data.size()] = std::move(value);
      ++m_entries;
      assert(m_entries <= m_data.size());
    }

    void push_front(const T& value)
    {
      if (m_entries >= m_data.size())
      {
        // The buffer has reached max capacity so we pop the front element to make room for a new one at the back
        pop_back();
      }
      assert(m_entries < m_data.size());
      m_frontIndex = (m_frontIndex - 1u) % m_data.size();
      m_data[m_frontIndex % m_data.size()] = value;
      ++m_entries;
      assert(m_entries <= m_data.size());
    }

    void push_front(T&& value)
    {
      if (m_entries >= m_data.size())
      {
        // The buffer has reached max capacity so we pop the front element to make room for a new one at the back
        pop_back();
      }
      assert(m_entries < m_data.size());
      m_frontIndex = (m_frontIndex - 1u) % m_data.size();
      m_data[m_frontIndex % m_data.size()] = std::move(value);
      ++m_entries;
      assert(m_entries <= m_data.size());
    }

    void pop_back()
    {
      assert(!empty());
      --m_entries;
      m_data[(m_frontIndex + m_entries) % m_data.size()] = {};
      assert(m_entries <= m_data.size());
    }

    void pop_front()
    {
      assert(!empty());
      m_data[m_frontIndex] = {};
      m_frontIndex = (m_frontIndex + 1u) % m_data.size();
      --m_entries;
      assert(m_frontIndex < m_data.size());
      assert(m_entries <= m_data.size());
    }

    void grow(const size_type growCapacityBy)
    {
      if (growCapacityBy <= 0)
      {
        return;
      }

      const size_type oldCapacity = m_data.size();
      const size_type newCapacity = oldCapacity + growCapacityBy;

      const size_type entriesFromFrontToEnd = (oldCapacity - m_frontIndex);
      m_data.resize(newCapacity);
      if (m_entries > entriesFromFrontToEnd)
      {
        // The current entries crosses the old buffer end point so we will need to move it
        const size_type entriesToMove = m_entries - entriesFromFrontToEnd;
        const size_type moveFromFrontToEndCount = std::min(growCapacityBy, entriesToMove);
        {    // move from front of the buffer to the newly allocated area
          for (size_type i = 0; i < moveFromFrontToEndCount; ++i)
          {
            std::swap(m_data[i], m_data[oldCapacity + i]);
          }
        }
        if (entriesToMove > moveFromFrontToEndCount)
        {
          // There are still entries left that need to be moved
          const size_type entriesLeft = entriesToMove - moveFromFrontToEndCount;
          for (size_type i = 0; i < entriesLeft; ++i)
          {
            std::swap(m_data[i], m_data[moveFromFrontToEndCount + i]);
          }
        }
      }
    }

    void resize_pop_front(const size_type newSize)
    {
      if (newSize < m_data.size())
      {
        // pop the front entry until we reached the new max size
        while (size() > newSize)
        {
          pop_front();
        }

        if (m_frontIndex <= newSize && m_entries <= (newSize - m_frontIndex))
        {
          // We can get away with resizing the current buffer as there is no overlap on the new end boundary
          m_data.resize(newSize);
        }
        else
        {
          // copy to a new buffer, then release the old one
          std::vector<T> tmp(newSize);
          for (std::size_t i = 0; i < size(); ++i)
          {
            tmp[i] = (*this)[i];
          }
          m_data = std::move(tmp);
          m_frontIndex = 0;
        }
      }
      else
      {
        grow(newSize - m_data.size());
      }
    }

    constexpr uint32_t segment_count() const noexcept
    {
      return m_entries <= (m_data.size() - m_frontIndex) ? (m_entries > 0 ? 1u : 0u) : 2u;
    }

    ReadOnlySpan<T> AsReadOnlySpan(const uint32_t segmentIndex) const
    {
      assert(segmentIndex < segment_count());
      const auto maxEntriesFirstSegment = (m_data.size() - m_frontIndex);
      return segmentIndex == 0
               ? ReadOnlySpan<T>(m_data.data() + m_frontIndex, (m_entries >= maxEntriesFirstSegment ? maxEntriesFirstSegment : m_entries),
                                 OptimizationCheckFlag::NoCheck)
               : ReadOnlySpan<T>(m_data.data(), m_entries - maxEntriesFirstSegment, OptimizationCheckFlag::NoCheck);
    }
  };
}

#endif
