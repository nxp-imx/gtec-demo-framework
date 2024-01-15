#ifndef FSLBASE_PLACEMENT_CONTROLLED_HPP
#define FSLBASE_PLACEMENT_CONTROLLED_HPP
/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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

#include <cstdint>
#include <new>
#include <type_traits>
#include <utility>

namespace Fsl
{
  //! @brief Control the objects assignment and move operations so that
  //!        - a copy will 'Destruct' the existing content before 'copy Constructing' the assigned content.
  //!        - a move will 'Destruct' the existing content before 'move Constructing' the moved assigned content.
  //!        This will ensure that any existing content will be destroyed in 'destruction order'.
  //!        So any type 'T' which has members that rely on the construction and destruction order but 'incorrectly' still uses a default copy or
  //!        move constructor or assignment will still operate correctly when placed inside this object.
  //!        If the default move and copy assignment had been specced to execute its assignment in the destruction order by default then
  //!        complex RAII objects with multiple RAII members would still have worked. But unfortunately thats not the case, so this is a
  //!        fast experimental workaround.
  //! @warning Experimental
  template <typename T>
  class Controlled
  {
    typename std::aligned_storage<sizeof(T), alignof(T)>::type m_buffer;

  public:
    using value_type = T;

    Controlled(const Controlled<T>& other) = delete;
    Controlled& operator=(const Controlled<T>& other) = delete;
    Controlled(Controlled<T>&& other) = delete;
    Controlled& operator=(Controlled<T>&& other) = delete;

    template <typename... Args>
    explicit Controlled(Args&&... args)
    {
      // Use aligned storage to ensure proper alignment.
      new (&m_buffer) T(std::forward<Args>(args)...);
    }

    ~Controlled() noexcept
    {
      get().~T();
    }

    template <typename U = T, std::enable_if_t<std::is_copy_constructible_v<U>, int> = 0>
    // NOLINTNEXTLINE(google-explicit-constructor)
    Controlled(const T& other)
    {
      // Copy the object from 'other' to 'this'
      new (&m_buffer) T(other);
    }

    template <typename U = T, std::enable_if_t<std::is_copy_constructible_v<U>, int> = 0>
    Controlled& operator=(const T& other)
    {
      // Destroy  the old object to ensure members are destroyed in destructor order.
      get().~T();
      // Copy the object from 'other' to 'this'
      new (&m_buffer) T(other);
      return *this;
    }

    // Move constructor
    template <typename U = T, std::enable_if_t<std::is_move_constructible_v<U>, int> = 0>
    // NOLINTNEXTLINE(google-explicit-constructor)
    Controlled(T&& other) noexcept
    {
      // Move the object from 'other' to 'this'
      new (&m_buffer) T(std::move(other));
    }

    // Move assignment operator
    template <typename U = T, std::enable_if_t<std::is_move_constructible_v<U>, int> = 0>
    Controlled& operator=(T&& other) noexcept
    {
      // Destroy  the old object to ensure members are destroyed in destructor order.
      get().~T();
      // Move the object from 'other' to 'this'
      new (&m_buffer) T(std::move(other));
      return *this;
    }


    void reset() noexcept
    {
      // Destroy the old object then default-construct a new object in the m_buffer.
      get().~T();
      new (&m_buffer) T();
    }

    T& get() noexcept
    {
      return *reinterpret_cast<T*>(&m_buffer);
    }

    const T& get() const noexcept
    {
      return *reinterpret_cast<const T*>(&m_buffer);
    }

    // Access member of T
    T* operator->() noexcept
    {
      return &get();
    }
  };
}

#endif
