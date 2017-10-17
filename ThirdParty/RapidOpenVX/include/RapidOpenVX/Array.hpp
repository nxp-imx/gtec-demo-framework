#ifndef RAPIDOPENVX_ARRAY_HPP
#define RAPIDOPENVX_ARRAY_HPP
//***************************************************************************************************************************************************
//* BSD 3-Clause License
//*
//* Copyright (c) 2016, Rene Thrane
//* All rights reserved.
//*
//* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//*
//* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
//*    documentation and/or other materials provided with the distribution.
//* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this
//*    software without specific prior written permission.
//*
//* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//***************************************************************************************************************************************************

// Auto-generated OpenVX 1.1 C++11 RAII classes by RAIIGen (https://github.com/Unarmed1000/RAIIGen)

#include <RapidOpenVX/ClaimMode.hpp>
#include <RapidOpenVX/CheckError.hpp>
#include <RapidOpenVX/System/Macro.hpp>
#include <VX/vx.h>
#include <cassert>

namespace RapidOpenVX
{
  //! This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
  class Array
  {
    vx_array m_array;
  public:
    Array(const Array&) = delete;
    Array& operator=(const Array&) = delete;

    //! @brief Move assignment operator
    Array& operator=(Array&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_array = other.m_array;

        // Remove the data from other
        other.m_array = nullptr;
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    Array(Array&& other)
      : m_array(other.m_array)
    {
      // Remove the data from other
      other.m_array = nullptr;
    }

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    Array()
      : m_array(nullptr)
    {
    }

    //! @brief Assume control of the Array (this object becomes responsible for releasing it)
    explicit Array(const vx_array array)
      : Array()
    {
      Reset(array);
    }

    //! @brief Create the requested resource
    //! @note  Function: vxCreateArray
    Array(const vx_context context, const vx_enum itemType, const vx_size capacity)
      : Array()
    {
      Reset(context, itemType, capacity);
    }

    ~Array()
    {
      Reset();
    }

    //! @brief returns the managed handle and releases the ownership.
    vx_array Release() RAPIDOPENVX_FUNC_POSTFIX_WARN_UNUSED_RESULT
    {
      const auto resource = m_array;
      m_array = nullptr;
      return resource;
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset()
    {
      if (! IsValid())
        return;

      assert(m_array != nullptr);

      vxReleaseArray(&m_array);
      m_array = nullptr;
    }

    //! @brief Destroys any owned resources and assume control of the Array (this object becomes responsible for releasing it)
    void Reset(const vx_array array)
    {
      if (IsValid())
        Reset();


      m_array = array;
    }

    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vxCreateArray
    void Reset(const vx_context context, const vx_enum itemType, const vx_size capacity)
    {
#ifndef RAPIDOPENVX_DISABLE_PARAM_VALIDATION
#else
#endif

      // Free any currently allocated resource
      if (IsValid())
        Reset();

      // Since we want to ensure that the resource is left untouched on error we use a local variable as a intermediary
      const vx_array array = vxCreateArray(context, itemType, capacity);
      CheckError(array, "vxCreateArray", __FILE__, __LINE__);

      // Everything is ready, so assign the members
      m_array = array;
    }

    //! @brief Get the associated resource handle
    vx_array Get() const
    {
      return m_array;
    }

    //! @brief Get a pointer to the associated resource handle
    const vx_array* GetPointer() const
    {
      return &m_array;
    }

    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_array != nullptr;
    }
  };
}

#endif
