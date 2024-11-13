#ifndef FSLBASE_COLLECTIONS_HANDLEVECTOR_HPP
#define FSLBASE_COLLECTIONS_HANDLEVECTOR_HPP
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

#include <FslBase/Collections/HandleVectorConfig.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Span/TypedFlexSpan.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

namespace Fsl
{
  //! A vector that provides a unique handle for the added entry
  //! The lookup of the element via the handle is a o(1) operation. Its just direct lookup via the handle to get the actual index.
  //! Removing elements by handle is also a o(1) operation.
  template <typename T>
  class HandleVector
  {
  public:
    using value_type = T;
    using size_type = uint32_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using const_pointer = const T*;
    using pointer = T*;

    using index_type = size_type;
    using handle_type = int32_t;

  private:
    struct Record
    {
      //! Used to convert a handle to a actual index
      index_type HandleToIndex{};
      //! The handle associated with this element
      handle_type Handle{};
      //! The actual element
      T Element{};

      constexpr Record() = default;

      constexpr Record(const index_type arrayIndex, const handle_type handle)
        : HandleToIndex(arrayIndex)
        , Handle(handle)
      {
      }
    };

    std::vector<Record> m_data;
    size_type m_count{};

  public:
    static constexpr const auto ElementByteSize = sizeof(Record);

    HandleVector(const HandleVector&) = default;
    HandleVector& operator=(const HandleVector&) = default;
    HandleVector(HandleVector&&) noexcept = default;
    HandleVector& operator=(HandleVector&&) noexcept = default;

    HandleVector() = default;

    explicit HandleVector(const size_type capacity)
      : m_data(capacity)
    {
      if (capacity <= 0u)
      {
        throw std::invalid_argument("count must be greater than zero");
      }

      for (index_type i = 0; i < m_data.size(); ++i)
      {
        m_data[i] = Record(UncheckedNumericCast<index_type>(i), UncheckedNumericCast<handle_type>(i));
      }
      m_count = 0;
    }

    TypedFlexSpan<T> AsSpan()
    {
      return TypedFlexSpan<T>(&m_data[0].Element, m_count, sizeof(Record));
    }

    ReadOnlyTypedFlexSpan<T> AsSpan() const
    {
      return ReadOnlyTypedFlexSpan<T>(&m_data[0].Element, m_count, sizeof(Record));
    }

    constexpr bool Empty() const noexcept
    {
      return m_count <= 0;
    }

    //! @brief Free all entries
    //! @param zeroFreed if this is cleared vector elements are not cleared (only do this for simple types)
    //!                  only the size is set to zero.
    void Clear(const bool zeroFreed = true)
    {
      if (zeroFreed)
      {
        for (index_type i = 0; i < m_count; ++i)
        {
          m_data[i].Element = {};
        }
      }
      m_count = 0;
    }

    //! Reserve room for the given number of elements by growing the capacity to the requested number.
    //! If the current capacity exceeds the requested capacity this will do nothing.
    void Reserve(const size_type capacity)
    {
      if (capacity < m_data.size())
      {
        return;
      }
      constexpr size_type GrowBy = 200;
      const auto finalCapacity = ((capacity / GrowBy) + ((capacity % GrowBy) > 0 ? 1 : 0)) * GrowBy;

      // Resize the array
      auto oldCapacity = UncheckedNumericCast<size_type>(m_data.size());
      m_data.resize(finalCapacity);

      // Update the lookup values
      const auto dataSize = UncheckedNumericCast<size_type>(m_data.size());
      for (index_type i = oldCapacity; i < dataSize; ++i)
      {
        m_data[i] = Record(UncheckedNumericCast<index_type>(i), UncheckedNumericCast<handle_type>(i));
      }
    }


    //! Convert a handle to a index
    constexpr index_type FastHandleToIndex(const handle_type handle) const noexcept
    {
      assert(IsValidHandle(handle));
      return m_data[handle].HandleToIndex;
    }

    //! @brief Convert a index to the corresponding handle
    constexpr handle_type FastIndexToHandle(const index_type index) const noexcept
    {
      assert(IsValidIndex(index));
      return m_data[index].Handle;
    }


    handle_type Add(const_reference element)
    {
      assert(Count() >= 0 && Count() <= Capacity());

      if (m_count >= m_data.size())
      {
        // Ensure we have enough capacity
        Reserve(UncheckedNumericCast<size_type>(m_data.size() + HandleVectorConfig::GrowBy));
      }

      // get a free handle, then add the item to the array
      handle_type newHandle = m_data[m_count].Handle;
      assert(m_data[newHandle].HandleToIndex == m_count);

      m_data[m_count].Element = element;

      ++m_count;

      assert(IsValidHandle(newHandle));
      assert(newHandle != HandleVectorConfig::InvalidHandle);
      return newHandle;
    }

    handle_type Add(T&& element)
    {
      assert(Count() >= 0 && Count() <= Capacity());

      if (m_count >= m_data.size())
      {
        // Ensure we have enough capacity
        Reserve(UncheckedNumericCast<size_type>(m_data.size() + HandleVectorConfig::GrowBy));
      }

      // get a free handle, then add the item to the array
      handle_type newHandle = m_data[m_count].Handle;
      assert(m_data[newHandle].HandleToIndex == m_count);

      m_data[m_count].Element = std::move(element);

      ++m_count;

      assert(IsValidHandle(newHandle));
      assert(newHandle != HandleVectorConfig::InvalidHandle);
      return newHandle;
    }

    //! @brief Insert the item at the given index
    //! @param handle The item handle to insert after
    //! @param element The object to insert. The value can be null for reference types
    handle_type Insert(const handle_type handle, const_reference element)
    {
      if (!IsValidHandle(handle))
      {
        throw std::invalid_argument("Invalid handle");
      }

      const index_type index = m_data[handle].HandleToIndex;
      return DoInsertAt(index, element);
    }

    //! @brief Insert the item at the given index
    //! @param handle The item handle to insert after
    //! @param element The object to insert. The value can be null for reference types
    handle_type Insert(const handle_type handle, T&& element)
    {
      if (!IsValidHandle(handle))
      {
        throw std::invalid_argument("Invalid handle");
      }

      const index_type index = m_data[handle].HandleToIndex;
      return DoInsertAt(index, std::move(element));
    }

    //! @brief Insert the item at the given index
    //! @param index The zero-based index at which item should be inserted.
    //! @param element The object to insert. The value can be null for reference types
    handle_type InsertAt(const index_type index, const_reference element)
    {
      if (/*index < 0 ||*/ index > Count())
      {
        throw IndexOutOfRangeException();
      }

      return DoInsertAt(index, element);
    }

    //! @brief Insert the item at the given index
    //! @param index The zero-based index at which item should be inserted.
    //! @param element The object to insert. The value can be null for reference types
    handle_type InsertAt(const index_type index, T&& element)
    {
      if (index < 0 || index > Count())
      {
        throw IndexOutOfRangeException();
      }

      return DoInsertAt(index, std::move(element));
    }


    //! @brief Remove the item with the given handle from the list.
    bool Remove(const handle_type handle) noexcept
    {
      if (IsValidHandle(handle))
      {
        assert(Count() > 0);
        const index_type index = m_data[handle].HandleToIndex;
        FastRemoveAt(index);
        return true;
      }
      return false;
    }


    /// Remove the item at the given index
    void RemoveAt(const index_type index)
    {
      if (!IsValidIndex(index))
      {
        throw IndexOutOfRangeException();
      }
      FastRemoveAt(index);
    }

    //! @brief Remove a range of elements from the vector
    //! @param startIndex the startIndex
    //! @param length the number of elements to remove.
    /// @throws std::invalid_argument if startIndex and length doesnt specify a valid range
    void RemoveRange(const index_type startIndex, const size_type length)
    {
      if (length > m_count)
      {
        throw std::invalid_argument("length larger than current count");
      }
      if (startIndex > (m_count - length))
      {
        throw std::invalid_argument("startIndex and length do not denote a valid range");
      }
      if (length <= 1u)
      {
        if (length == 1u)
        {
          RemoveAt(startIndex);
        }
        return;
      }

      assert(m_count >= length);
      m_count -= length;

      // Move all the previous handles one step to the left
      for (index_type i = startIndex; i < m_count; ++i)
      {
        auto& rSrc = m_data[i + length];
        auto& rDst = m_data[i];
        const auto handleA = rDst.Handle;
        const auto handleB = rSrc.Handle;
        rDst.Handle = handleB;
        rDst.Element = std::move(rSrc.Element);

        // swap the handles
        rSrc.Handle = handleA;

        // Patch the handle to index lookup table so we know the new index of element with a given handle
        m_data[handleB].HandleToIndex = i;
      }

      // Update the element at the end
      const index_type oldCount = m_count + length;
      for (index_type i = m_count; i < oldCount; ++i)
      {
        auto& rRecord = m_data[i];
        rRecord.Element = {};
        m_data[rRecord.Handle].HandleToIndex = i;
      }
    }

    //! @brief Remove the item with the given handle from the list.
    //!        This remove will swap the removed element with the last element.
    void RemoveBySwap(const handle_type handle)
    {
      if (!IsValidHandle(handle))
      {
        throw std::invalid_argument("Invalid handle");
      }

      assert(Count() > 0);
      const index_type index = m_data[handle].HandleToIndex;
      RemoveBySwapAt(index);
    }

    //! Remove the item at the given index from the list.
    //! This  remove will swap the removed element with the last element.
    void RemoveBySwapAt(const index_type index)
    {
      if (!IsValidIndex(index))
      {
        throw IndexOutOfRangeException();
      }

      assert(m_count > 0u);
      --m_count;

      // Swap the removed element with the last element in the vector
      const auto removedHandle = m_data[index].Handle;
      const auto endHandle = m_data[m_count].Handle;
      m_data[index].Handle = endHandle;
      m_data[m_count].Handle = removedHandle;
      // Move the last element, and "zero" the removed element
      m_data[index].Element = std::move(m_data[m_count].Element);
      m_data[m_count].Element = {};
      // Patch the handle to index lookup table so we know the new index of element with a given handle
      m_data[endHandle].HandleToIndex = index;
      m_data[removedHandle].HandleToIndex = m_count;
    }

    //! Get the entry
    const_pointer TryGet(const handle_type handle) const noexcept
    {
      return IsValidHandle(handle) ? &m_data[m_data[handle].HandleToIndex].Element : nullptr;
    }

    //! Get the entry
    pointer TryGet(const handle_type handle) noexcept
    {
      return IsValidHandle(handle) ? &m_data[m_data[handle].HandleToIndex].Element : nullptr;
    }

    //! Get the entry
    const_reference Get(const handle_type handle) const
    {
      if (!IsValidHandle(handle))
      {
        throw std::invalid_argument("invalid handle");
      }

      const index_type index = m_data[handle].HandleToIndex;
      return m_data[index].Element;
    }

    //! Get the entry
    const_reference FastGet(const handle_type handle) const noexcept
    {
      assert(IsValidHandle(handle));
      const index_type index = m_data[handle].HandleToIndex;
      return m_data[index].Element;
    }

    //! Get the entry
    reference Get(const handle_type handle)
    {
      if (!IsValidHandle(handle))
      {
        throw std::invalid_argument("invalid handle");
      }

      const index_type index = m_data[handle].HandleToIndex;
      return m_data[index].Element;
    }

    //! Get the entry
    reference FastGet(const handle_type handle) noexcept
    {
      assert(IsValidHandle(handle));
      const index_type index = m_data[handle].HandleToIndex;
      return m_data[index].Element;
    }


    //! Get the entry and the index of the entry
    const_reference Get(const handle_type handle, index_type& rIndex) const
    {
      if (!IsValidHandle(handle))
      {
        throw std::invalid_argument("invalid handle");
      }

      const index_type index = m_data[handle].HandleToIndex;
      rIndex = index;
      return m_data[index].Element;
    }

    //! Get the entry and the index of the entry
    reference Get(const handle_type handle, index_type& rIndex)
    {
      if (!IsValidHandle(handle))
      {
        throw std::invalid_argument("invalid handle");
      }

      const index_type index = m_data[handle].HandleToIndex;
      rIndex = index;
      return m_data[index].Element;
    }

    //! Set the entry
    void Set(const handle_type handle, const_reference element)
    {
      if (!IsValidHandle(handle))
      {
        throw std::invalid_argument("invalid handle");
      }

      index_type index = m_data[handle].HandleToIndex;
      m_data[index].Element = element;
    }

    //! Set the entry
    void Set(const handle_type handle, T&& element)
    {
      if (!IsValidHandle(handle))
      {
        throw std::invalid_argument("invalid handle");
      }

      index_type index = m_data[handle].HandleToIndex;
      m_data[index].Element = std::move(element);
    }

    const_reference At(const index_type index) const
    {
      if (!IsValidIndex(index))
      {
        throw IndexOutOfRangeException();
      }
      return m_data[index].Element;
    }

    reference At(const index_type index)
    {
      if (!IsValidIndex(index))
      {
        throw IndexOutOfRangeException();
      }
      return m_data[index].Element;
    }

    //! @brief Set the element at the given index
    void SetAt(const index_type index, const_reference value)
    {
      if (!IsValidIndex(index))
      {
        throw IndexOutOfRangeException();
      }

      m_data[index].Element = value;
    }

    //! @brief Set the element at the given index
    void SetAt(const index_type index, T&& value)
    {
      if (!IsValidIndex(index))
      {
        throw IndexOutOfRangeException();
      }

      m_data[index].Element = std::move(value);
    }

    void Swap(const handle_type handle0, const handle_type handle1)
    {
      if (!IsValidHandle(handle0) || !IsValidHandle(handle1))
      {
        throw std::invalid_argument("invalid handle");
      }

      SwapAt(FastHandleToIndex(handle0), FastHandleToIndex(handle1));
    }

    //! @brief Swap the records at the given indices
    void SwapAt(const index_type index0, const index_type index1)
    {
      if (!IsValidIndex(index0))
      {
        throw IndexOutOfRangeException("index0");
      }
      if (!IsValidIndex(index1))
      {
        throw IndexOutOfRangeException("index1");
      }

      if (index0 == index1)
      {
        return;
      }

      auto& rRec1 = m_data[index0];
      auto& rRec2 = m_data[index1];

      // Patch the handle lookup by setting the new location
      assert(m_data[rRec1.Handle].HandleToIndex == index0);
      assert(m_data[rRec2.Handle].HandleToIndex == index1);
      m_data[rRec1.Handle].HandleToIndex = index1;
      m_data[rRec2.Handle].HandleToIndex = index0;

      // Swap the stored element and the handle
      auto tmpElement = std::move(rRec1.Element);
      rRec1.Element = std::move(rRec2.Element);
      rRec2.Element = std::move(tmpElement);

      auto tmpHandle = rRec1.Handle;
      rRec1.Handle = rRec2.Handle;
      rRec2.Handle = tmpHandle;
    }


    /// @brief Move the element at fromIndex to toIndex.
    ///        This has the same effect as "Remove(fromIndex), Insert(ToIndex)" it will just be faster.
    void MoveFromTo(const handle_type hFrom, const handle_type hTo)
    {
      if (!IsValidHandle(hFrom))
      {
        throw std::invalid_argument("invalid handle");
      }
      if (!IsValidHandle(hTo))
      {
        throw std::invalid_argument("invalid handle");
      }

      MoveAtFromTo(FastHandleToIndex(hFrom), FastHandleToIndex(hTo));
    }

    //! @brief Move the element at fromIndex to toIndex.
    //!        This has the same effect as "RemoveAt(fromIndex), InsertAt(ToIndex)" it will just be faster.
    void MoveAtFromTo(const index_type fromIndex, const index_type toIndex)
    {
      if (!IsValidIndex(fromIndex))
      {
        throw IndexOutOfRangeException("fromIndex");
      }
      if (!IsValidIndex(toIndex))
      {
        throw IndexOutOfRangeException("toIndex");
      }

      if (fromIndex == toIndex)
      {
        return;
      }

      if (fromIndex < toIndex)
      {
        auto entryIndex0 = std::move(m_data[fromIndex]);
        assert(m_data[entryIndex0.Handle].HandleToIndex == fromIndex);

        // Move all the previous handles one step to the left
        for (index_type i = fromIndex; i < toIndex; ++i)
        {
          auto& rEntry0 = m_data[i];
          auto& rEntry1 = m_data[i + 1];
          assert((i + 1) == m_data[rEntry1.Handle].HandleToIndex);
          rEntry0.Handle = rEntry1.Handle;
          rEntry0.Element = std::move(rEntry1.Element);
          // Patch the handle to index lookup table so we know the new index of element with a given handle
          m_data[rEntry1.Handle].HandleToIndex = i;
        }

        // Update the element at the end
        m_data[toIndex].Handle = entryIndex0.Handle;
        m_data[toIndex].Element = std::move(entryIndex0.Element);
        m_data[entryIndex0.Handle].HandleToIndex = toIndex;
      }
      else
      {
        auto entryIndex0 = std::move(m_data[fromIndex]);
        assert(m_data[entryIndex0.Handle].HandleToIndex == fromIndex);

        // Move all the handles one step to the right
        for (index_type i = fromIndex; i > toIndex; --i)
        {
          assert(i > 0u);
          auto& rEntry0 = m_data[i - 1];
          auto& rEntry1 = m_data[i];
          assert((i - 1) == m_data[rEntry0.Handle].HandleToIndex);
          rEntry1.Handle = rEntry0.Handle;
          rEntry1.Element = std::move(rEntry0.Element);
          // Patch the handle to index lookup table so we know the new index of element with a given handle
          m_data[rEntry0.Handle].HandleToIndex = i;
        }

        // Update the element at the end
        m_data[toIndex].Handle = entryIndex0.Handle;
        m_data[toIndex].Element = std::move(entryIndex0.Element);
        m_data[entryIndex0.Handle].HandleToIndex = toIndex;
      }
    }

    //! @brief Direct access to the elements
    const_reference operator[](const index_type index) const noexcept
    {
      assert(IsValidIndex(index));
      return m_data[index].Element;
    }

    /// Direct access to the elements
    reference operator[](const index_type index) noexcept
    {
      assert(IsValidIndex(index));
      return m_data[index].Element;
    }

    //! @brief the current element count
    size_type Count() const noexcept
    {
      return m_count;
    }

    //! @brief The current capacity of the vector
    size_type Capacity() const noexcept
    {
      return UncheckedNumericCast<size_type>(m_data.size());
    }

    //! @brief Check if the given handle is valid.
    //! @return True if the supplied handle is valid
    constexpr bool IsValidHandle(const handle_type handle) const noexcept
    {
      return (handle >= 0 && static_cast<std::size_t>(handle) < m_data.size() && m_data[handle].HandleToIndex < m_count);
    }

    //! @brief Check if the given index is valid.
    //! @return True if the supplied handle is valid
    constexpr bool IsValidIndex(const index_type index) const noexcept
    {
      // Index is unsigned so we can simplify this
      // return (index >= 0u && index < m_count);
      return index < m_count;
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    bool DEBUG_IsValid() const
    {
      for (index_type i = 0; i < m_data.size(); ++i)
      {
        const handle_type handle = m_data[i].Handle;
        if (m_data[handle].HandleToIndex != i)
        {
          return false;
        }
        const index_type handleIndex = m_data[i].HandleToIndex;
        if (m_data[handleIndex].Handle != static_cast<handle_type>(i))
        {
          return false;
        }
      }
      return true;
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    void DEBUG_SanityCheck()
    {
      std::vector<int32_t> taken(m_data.size());
      {    // Validate indices to handle conversions
        for (std::size_t i = 0; i < taken.size(); ++i)
        {
          taken[i] = -1;
        }
        for (handle_type i = 0; i < UncheckedNumericCast<handle_type>(m_data.size()); ++i)
        {
          const auto handle = m_data[i].Handle;
          if (taken[handle] >= 0)
          {
            throw std::runtime_error("Index and handle was already claimed by another index");
          }
          taken[handle] = i;
        }
      }

      {    // Validate handle to index conversions
        for (std::size_t i = 0; i < taken.size(); ++i)
        {
          taken[i] = -1;
        }
        for (handle_type handle = 0; handle < UncheckedNumericCast<handle_type>(m_data.size()); ++handle)
        {
          const auto index = m_data[handle].HandleToIndex;
          if (taken[index] >= 0)
          {
            throw std::runtime_error("Handle and index was already claimed by another index");
          }
          taken[index] = static_cast<int32_t>(handle);
        }
      }
    }

    //! @brief  Onlycall this if you are 100% sure the index is valid
    void FastRemoveAt(const index_type index) noexcept
    {
      assert(IsValidIndex(index));
      assert(m_count > 0u);
      --m_count;

      const handle_type removedHandle = m_data[index].Handle;

      // Move all the previous handles one step to the left
      for (size_type i = index; i < m_count; ++i)
      {
        auto& rEntry0 = m_data[i];
        auto& rEntry1 = m_data[i + 1];
        assert((i + 1) == m_data[rEntry1.Handle].HandleToIndex);
        rEntry0.Handle = rEntry1.Handle;
        rEntry0.Element = std::move(rEntry1.Element);
        // Patch the handle to index lookup table so we know the new index of element with a given handle
        m_data[rEntry1.Handle].HandleToIndex = i;
      }

      // Update the element at the end
      m_data[m_count].Handle = removedHandle;
      m_data[m_count].Element = {};
      m_data[removedHandle].HandleToIndex = m_count;
    }


  private:
    handle_type DoInsertAt(const index_type index, const_reference element)
    {
      assert(Count() >= 0 && Count() <= Capacity());
      assert(/*index >= 0 &&*/ index <= Count());

      if (m_count >= m_data.size())
      {
        // Ensure we have enough capacity
        Reserve(UncheckedNumericCast<uint32_t>(m_data.size() + HandleVectorConfig::GrowBy));
      }

      // get a free handle, then move all elements to make room
      const handle_type newHandle = m_data[m_count].Handle;

      // Move all the handles one step to the right
      for (index_type i = m_count; i > index; --i)
      {
        assert(i > 0u);
        auto& rEntry0 = m_data[i - 1];
        auto& rEntry1 = m_data[i];
        assert((i - 1) == m_data[rEntry0.Handle].HandleToIndex);
        rEntry1.Handle = rEntry0.Handle;
        rEntry1.Element = std::move(rEntry0.Element);
        // Patch the handle to index lookup table so we know the new index of element with a given handle
        m_data[rEntry0.Handle].HandleToIndex = i;
      }

      // Insert the new handle at the location we just made room at
      m_data[index].Handle = newHandle;
      m_data[index].Element = element;
      m_data[newHandle].HandleToIndex = index;

      ++m_count;
      assert(newHandle != HandleVectorConfig::InvalidHandle);
      return newHandle;
    }

    handle_type DoInsertAt(const index_type index, T&& element)
    {
      assert(Count() >= 0 && Count() <= Capacity());
      assert(index >= 0 && index <= Count());

      if (m_count >= m_data.size())
      {
        // Ensure we have enough capacity
        Reserve(m_data.size() + HandleVectorConfig::GrowBy);
      }

      // get a free handle, then move all elements to make room
      const handle_type newHandle = m_data[m_count].Handle;

      // Move all the handles one step to the right
      for (index_type i = m_count; i > index; --i)
      {
        assert(i > 0u);
        auto& rEntry0 = m_data[i - 1];
        auto& rEntry1 = m_data[i];
        assert((i - 1) == m_data[rEntry0.Handle].HandleToIndex);
        rEntry1.Handle = rEntry0.Handle;
        rEntry1.Element = std::move(rEntry0.Element);
        // Patch the handle to index lookup table so we know the new index of element with a given handle
        m_data[rEntry0.Handle].HandleToIndex = i;
      }

      // Insert the new handle at the location we just made room at
      m_data[index].Handle = newHandle;
      m_data[index].Element = std::move(element);
      m_data[newHandle].HandleToIndex = index;

      ++m_count;
      assert(newHandle != HandleVectorConfig::InvalidHandle);
      return newHandle;
    }
  };
}

#endif
