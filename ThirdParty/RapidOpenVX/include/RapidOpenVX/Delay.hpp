#ifndef RAPIDOPENVX_DELAY_HPP
#define RAPIDOPENVX_DELAY_HPP
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
  class Delay
  {
    vx_delay m_delay;
  public:
    Delay(const Delay&) = delete;
    Delay& operator=(const Delay&) = delete;

    //! @brief Move assignment operator
    Delay& operator=(Delay&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_delay = other.m_delay;

        // Remove the data from other
        other.m_delay = nullptr;
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    Delay(Delay&& other)
      : m_delay(other.m_delay)
    {
      // Remove the data from other
      other.m_delay = nullptr;
    }

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    Delay()
      : m_delay(nullptr)
    {
    }

    //! @brief Assume control of the Delay (this object becomes responsible for releasing it)
    explicit Delay(const vx_delay delay)
      : Delay()
    {
      Reset(delay);
    }

    //! @brief Create the requested resource
    //! @note  Function: vxCreateDelay
    Delay(const vx_context context, const vx_reference exemplar, const vx_size slots)
      : Delay()
    {
      Reset(context, exemplar, slots);
    }

    ~Delay()
    {
      Reset();
    }

    //! @brief returns the managed handle and releases the ownership.
    vx_delay Release() RAPIDOPENVX_FUNC_POSTFIX_WARN_UNUSED_RESULT
    {
      const auto resource = m_delay;
      m_delay = nullptr;
      return resource;
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset()
    {
      if (! IsValid())
        return;

      assert(m_delay != nullptr);

      vxReleaseDelay(&m_delay);
      m_delay = nullptr;
    }

    //! @brief Destroys any owned resources and assume control of the Delay (this object becomes responsible for releasing it)
    void Reset(const vx_delay delay)
    {
      if (IsValid())
        Reset();


      m_delay = delay;
    }

    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vxCreateDelay
    void Reset(const vx_context context, const vx_reference exemplar, const vx_size slots)
    {
#ifndef RAPIDOPENVX_DISABLE_PARAM_VALIDATION
#else
#endif

      // Free any currently allocated resource
      if (IsValid())
        Reset();

      // Since we want to ensure that the resource is left untouched on error we use a local variable as a intermediary
      const vx_delay delay = vxCreateDelay(context, exemplar, slots);
      CheckError(delay, "vxCreateDelay", __FILE__, __LINE__);

      // Everything is ready, so assign the members
      m_delay = delay;
    }

    //! @brief Get the associated resource handle
    vx_delay Get() const
    {
      return m_delay;
    }

    //! @brief Get a pointer to the associated resource handle
    const vx_delay* GetPointer() const
    {
      return &m_delay;
    }

    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_delay != nullptr;
    }
  };
}

#endif
