#ifndef FSLBASE_COLLECTIONS_TIGHTHOSTEDVECTOR_HPP
#define FSLBASE_COLLECTIONS_TIGHTHOSTEDVECTOR_HPP
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

#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslBase/Span/SpanUtil.hpp>
#include <algorithm>
#include <array>
#include <cassert>
#include <vector>

namespace Fsl
{
  //! @brief Simple vector wrapper that allows us to host multiple 'vectors' inside one single std::vector.
  //! @note  This works well for use cases where the hosted vectors don't change sizes often.
  //!        'Tight' basically means that we don't reserve any extract capacity for the vectors at 0 to TVectorCount-1 so modifying them is
  //!        relative expensive.
  template <typename T, std::size_t TVectorCount = 0>
  class TightHostedVector
  {
  public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;

    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;
    using reverse_iterator = typename std::vector<T>::reverse_iterator;
    using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;

  private:
    struct VectorRecord
    {
      size_type EndIndex{};
    };

    std::array<VectorRecord, TVectorCount> m_hosted{};
    std::vector<T> m_content;

  public:
    TightHostedVector() = default;

    iterator Begin(const size_type hostedArrayIndex) noexcept
    {
      return m_content.begin() + StartIndex(hostedArrayIndex);
    }

    const_iterator CBegin(const size_type hostedArrayIndex) const noexcept
    {
      return m_content.cbegin() + StartIndex(hostedArrayIndex);
    }


    iterator End(const size_type hostedArrayIndex) noexcept
    {
      return m_content.begin() + EndIndex(hostedArrayIndex);
    }

    const_iterator CEnd(const size_type hostedArrayIndex) const noexcept
    {
      return m_content.cbegin() + EndIndex(hostedArrayIndex);
    }

    constexpr iterator Erase(const size_type hostedArrayIndex, const_iterator pos)
    {
      auto index = pos - Begin(hostedArrayIndex);
      UncheckedRemoveAt(hostedArrayIndex, index);
      return Begin(hostedArrayIndex) + index;
    }


    bool Empty(const size_type hostedArrayIndex) const noexcept
    {
      return Size(hostedArrayIndex) <= 0u;
    }

    size_type Size(const size_type hostedArrayIndex) const noexcept
    {
      return hostedArrayIndex > 0u ? m_hosted[hostedArrayIndex].EndIndex - m_hosted[hostedArrayIndex - 1].EndIndex
                                   : m_hosted[hostedArrayIndex].EndIndex;
    }

    void ClearAll() noexcept
    {
      m_content.clear();
      m_hosted = {};
    }

    void Clear(const size_type hostedArrayIndex) noexcept
    {
      assert(hostedArrayIndex < TVectorCount);
      auto count = Size(hostedArrayIndex);
      if (count > 0u)
      {
        // now move any our end indices of the rest of the 'hosted vectors'
        for (std::size_t i = hostedArrayIndex; i < m_hosted.size(); ++i)
        {
          assert(m_hosted[i].EndIndex >= count);
          m_hosted[i].EndIndex -= count;
        }
      }
    }


    void PushBack(const size_type hostedArrayIndex, const T& value)
    {
      assert(hostedArrayIndex < TVectorCount);
      m_content.insert(m_content.begin() + m_hosted[hostedArrayIndex].EndIndex, value);
      // now move any our end indices of the rest of the 'hosted vectors'
      for (std::size_t i = hostedArrayIndex; i < m_hosted.size(); ++i)
      {
        ++m_hosted[i].EndIndex;
      }
    }

    void PushBack(const size_type hostedArrayIndex, const ReadOnlySpan<T> values)
    {
      assert(hostedArrayIndex < TVectorCount);
      if (!values.empty())
      {
        {    // ensure we have enough capacity
          const std::size_t minCapacity = m_content.size() + values.size();
          if (minCapacity > m_content.capacity())
          {
            m_content.reserve(minCapacity);
          }
        }

        m_content.insert(m_content.begin() + m_hosted[hostedArrayIndex].EndIndex, values.begin(), values.end());
        // now move any our end indices of the rest of the 'hosted vectors'
        for (std::size_t i = hostedArrayIndex; i < m_hosted.size(); ++i)
        {
          m_hosted[i].EndIndex += values.size();
        }
      }
    }

    void UncheckedPopBack(const size_type hostedArrayIndex)
    {
      assert(hostedArrayIndex < TVectorCount);
      assert(Size(hostedArrayIndex) > 0u);
      assert(m_hosted[hostedArrayIndex].EndIndex > 0u);

      // erase the element
      m_content.erase(m_content.begin() + (m_hosted[hostedArrayIndex].EndIndex - 1u));
      // now move any our end indices of the rest of the 'hosted vectors'
      for (std::size_t i = hostedArrayIndex; i < m_hosted.size(); ++i)
      {
        assert(m_hosted[i].EndIndex > 0u);
        --m_hosted[i].EndIndex;
      }
    }

    void UncheckedRemoveAt(const size_type hostedArrayIndex, const size_type elementIndex)
    {
      assert(hostedArrayIndex < TVectorCount);
      assert(elementIndex < Size(hostedArrayIndex));

      // erase the element
      m_content.erase(m_content.begin() + StartIndex(hostedArrayIndex) + elementIndex);

      // now move any our end indices of the rest of the 'hosted vectors'
      for (std::size_t i = hostedArrayIndex; i < m_hosted.size(); ++i)
      {
        assert(m_hosted[i].EndIndex > 0u);
        --m_hosted[i].EndIndex;
      }
    }

    const_reference UncheckedAt(const size_type hostedArrayIndex, const size_type elementIndex) const noexcept
    {
      assert(hostedArrayIndex < TVectorCount);
      assert(Size(hostedArrayIndex) > 0u);
      return m_content[StartIndex(hostedArrayIndex) + elementIndex];
    }

    reference UncheckedAt(const size_type hostedArrayIndex, const size_type elementIndex) noexcept
    {
      assert(hostedArrayIndex < TVectorCount);
      assert(Size(hostedArrayIndex) > 0u);
      return m_content[StartIndex(hostedArrayIndex) + elementIndex];
    }


    ReadOnlySpan<T> AsReadOnlySpan(const size_type hostedArrayIndex) const noexcept
    {
      assert(hostedArrayIndex < TVectorCount);
      return hostedArrayIndex > 0u ? ReadOnlySpanUtil::AsSpan(m_content, m_hosted[hostedArrayIndex - 1].EndIndex,
                                                              m_hosted[hostedArrayIndex].EndIndex - m_hosted[hostedArrayIndex - 1].EndIndex,
                                                              OptimizationCheckFlag::NoCheck)
                                   : ReadOnlySpanUtil::AsSpan(m_content, 0u, m_hosted[hostedArrayIndex].EndIndex, OptimizationCheckFlag::NoCheck);
    }

    Span<T> AsSpan(const size_type hostedArrayIndex) noexcept
    {
      assert(hostedArrayIndex < TVectorCount);
      return hostedArrayIndex > 0u
               ? SpanUtil::AsSpan(m_content, m_hosted[hostedArrayIndex - 1].EndIndex,
                                  m_hosted[hostedArrayIndex].EndIndex - m_hosted[hostedArrayIndex - 1].EndIndex, OptimizationCheckFlag::NoCheck)
               : SpanUtil::AsSpan(m_content, 0u, m_hosted[hostedArrayIndex].EndIndex, OptimizationCheckFlag::NoCheck);
    }

  private:
    size_type StartIndex(const size_type hostedArrayIndex) const noexcept
    {
      assert(hostedArrayIndex < TVectorCount);
      return hostedArrayIndex > 0u ? m_hosted[hostedArrayIndex - 1].EndIndex : 0u;
    }

    size_type EndIndex(const size_type hostedArrayIndex) const noexcept
    {
      assert(hostedArrayIndex < TVectorCount);
      return m_hosted[hostedArrayIndex].EndIndex;
    }
  };
}

#endif
