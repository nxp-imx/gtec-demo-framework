#ifndef FSLGRAPHICS_FONT_BASICFLATHASHTABLE_HPP
#define FSLGRAPHICS_FONT_BASICFLATHASHTABLE_HPP
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

#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <cassert>
#include <limits>
#include <type_traits>
#include <vector>

namespace Fsl
{
  //! @brief This is a very basic hash table intended the lookups need for rendering of bitmap fonts.
  template <typename T, typename TKey, uint32_t TBinCount>
  class BasicFlatHashTable
  {
  public:
    using key_type = TKey;

    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

  private:
    static_assert(TBinCount > 0, "The bin count must be larger than zero");
    static_assert((TBinCount - 1) <= std::numeric_limits<TKey>::max(), "The bin count must fit inside the key type");
    static_assert(std::is_unsigned<TKey>::value, "The key type must be unsigned");

    struct Record
    {
      element_type Element;
      //! Beware this is used as a length in some cases fro the final lookup table!!
      //! See TryGet and Defrag
      uint32_t OffsetNext{};
    };

    std::vector<Record> m_records;
    bool m_isEmpty{true};

  public:
    BasicFlatHashTable(const BasicFlatHashTable&) = delete;
    BasicFlatHashTable& operator=(const BasicFlatHashTable&) = delete;

    BasicFlatHashTable& operator=(BasicFlatHashTable&& other) noexcept = default;
    BasicFlatHashTable(BasicFlatHashTable&& other) noexcept = default;

    BasicFlatHashTable()
      : BasicFlatHashTable(ReadOnlySpan<element_type>())
    {
    }

    //! The char span must be a sorted span low to high id with no duplicates
    explicit BasicFlatHashTable(const ReadOnlySpan<element_type> elementSpan)
      : m_records(Populate(elementSpan))
      , m_isEmpty(elementSpan.empty())
    {
    }

    bool Empty() const
    {
      return m_isEmpty;
    }

    inline const_pointer TryGet(const key_type id) const
    {
      // Do a quick lookup of the hash bin.
      const uint32_t hashKey = UncheckedNumericCast<uint32_t>(id % TBinCount);
      uint32_t srcIndex = hashKey;

      // we found the bin, but if the first entry doesn't match then we look at all the entries in the bin one by one
      if (m_records[srcIndex].Element.GetId() != id && m_records[srcIndex].OffsetNext != 0u)
      {
        srcIndex += m_records[srcIndex].OffsetNext;
        const auto endSrcIndex = srcIndex + m_records[srcIndex].OffsetNext;

        while (srcIndex < endSrcIndex && m_records[srcIndex].Element.GetId() != id)
        {
          assert((m_records[srcIndex].Element.GetId() % TBinCount) == hashKey);
          ++srcIndex;
        }
      }
      return (srcIndex < m_records.size() && m_records[srcIndex].Element.GetId() == id) ? &m_records[srcIndex].Element : nullptr;
    }

    const_reference Get(const key_type id, const_reference defaultValue) const
    {
      // Do a quick lookup of the hash bin.
      const uint32_t hashKey = UncheckedNumericCast<uint32_t>(id % TBinCount);
      uint32_t srcIndex = hashKey;

      // we found the bin, but if the first entry doesn't match then we look at all the entries in the bin one by one
      if (m_records[srcIndex].Element.GetId() != id && m_records[srcIndex].OffsetNext != 0u)
      {
        srcIndex += m_records[srcIndex].OffsetNext;
        const auto endSrcIndex = srcIndex + m_records[srcIndex].OffsetNext;

        while (srcIndex < endSrcIndex && m_records[srcIndex].Element.GetId() != id)
        {
          assert((m_records[srcIndex].Element.GetId() % TBinCount) == hashKey);
          ++srcIndex;
        }
      }
      return (srcIndex < m_records.size() && m_records[srcIndex].Element.GetId() == id) ? m_records[srcIndex].Element : defaultValue;
    }

  private:
    static std::vector<Record> Populate(const ReadOnlySpan<element_type> elementSpan)
    {
      std::vector<Record> records(CalcCapacity(elementSpan));
      assert(records.size() <= 0xFFFFFFFFu);

      // pre-populate the first BINS entries with dummy records
      for (key_type i = 0; i < TBinCount; ++i)
      {
        records[i].Element.SetId(0xFFFFFFFFu);
      }

      // Populate the lookup table
      uint32_t currentEntries = TBinCount;
      for (std::size_t i = 0; i < elementSpan.size(); ++i)
      {
        Add(records, currentEntries, key_type(elementSpan[i].GetId() % TBinCount), elementSpan[i]);
      }
      assert(currentEntries == records.size());

      return Defrag(records);
    }

    static std::vector<Record> Defrag(const std::vector<Record>& src)
    {
      std::vector<Record> result(src.size());
      uint32_t dstIndex = TBinCount;
      // Travel the first two records first and flatten it into the dst in sequential order
      for (uint32_t i = 0; i < TBinCount; ++i)
      {
        result[i] = src[i];
        if (src[i].OffsetNext != 0u)
        {
          uint32_t childCount = 0u;
          uint32_t previousDstIndex = i;
          uint32_t srcIndex = i;
          while (src[srcIndex].OffsetNext != 0u)
          {
            // Patch the record that points to the new one
            result[previousDstIndex].OffsetNext = dstIndex - previousDstIndex;
            // move to the next record
            srcIndex += src[srcIndex].OffsetNext;
            result[dstIndex] = src[srcIndex];
            assert((src[srcIndex].Element.GetId() % TBinCount) == i);
            previousDstIndex = dstIndex;
            ++dstIndex;
            ++childCount;
          }
          // Since we are ensuring that all 'hash-key' children are stored sequentially, we use the first child's offset as a count instead.
          auto childIndex = i + result[i].OffsetNext;
          result[childIndex].OffsetNext = childCount;
        }
      }
      assert(dstIndex == src.size());
      return result;
    }

    static void Add(std::vector<Record>& rDstRecords, uint32_t& rDstEntries, const key_type keyHash, const element_type& element)
    {
      assert(rDstEntries <= rDstRecords.size());

      if (element.GetId() < TBinCount)
      {
        // If this fires a char record was duplicated, this should not be possible
        assert(rDstRecords[element.GetId()].Element.GetId() == 0xFFFFFFFFu);
        rDstRecords[element.GetId()].Element = element;
      }
      else
      {
        uint32_t index = UncheckedNumericCast<uint32_t>(keyHash);
        while (rDstRecords[index].OffsetNext != 0u)
        {
          assert(rDstRecords[index].Element.GetId() != 0 || (index < TBinCount && rDstRecords[index].Element.GetId() == 0xFFFFFFFFu));
          index += rDstRecords[index].OffsetNext;
        }
        rDstRecords[index].OffsetNext = rDstEntries - index;
        rDstRecords[rDstEntries].Element = element;
        assert(rDstRecords[rDstEntries].OffsetNext == 0u);
        ++rDstEntries;
      }
    }

    static std::size_t CalcCapacity(const ReadOnlySpan<element_type> elements)
    {
      std::size_t additional = 0u;
      for (std::size_t i = 0; i < elements.size(); ++i)
      {
        additional += (elements[i].GetId() >= TBinCount ? 1 : 0);
      }
      return TBinCount + additional;
    }
  };
}

#endif
