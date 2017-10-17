#ifndef RAPIDOPENVX_CONVOLUTION_HPP
#define RAPIDOPENVX_CONVOLUTION_HPP
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
  class Convolution
  {
    vx_convolution m_convolution;
  public:
    Convolution(const Convolution&) = delete;
    Convolution& operator=(const Convolution&) = delete;

    //! @brief Move assignment operator
    Convolution& operator=(Convolution&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_convolution = other.m_convolution;

        // Remove the data from other
        other.m_convolution = nullptr;
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    Convolution(Convolution&& other)
      : m_convolution(other.m_convolution)
    {
      // Remove the data from other
      other.m_convolution = nullptr;
    }

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    Convolution()
      : m_convolution(nullptr)
    {
    }

    //! @brief Assume control of the Convolution (this object becomes responsible for releasing it)
    explicit Convolution(const vx_convolution convolution)
      : Convolution()
    {
      Reset(convolution);
    }

    //! @brief Create the requested resource
    //! @note  Function: vxCreateConvolution
    Convolution(const vx_context context, const vx_size columns, const vx_size rows)
      : Convolution()
    {
      Reset(context, columns, rows);
    }

    ~Convolution()
    {
      Reset();
    }

    //! @brief returns the managed handle and releases the ownership.
    vx_convolution Release() RAPIDOPENVX_FUNC_POSTFIX_WARN_UNUSED_RESULT
    {
      const auto resource = m_convolution;
      m_convolution = nullptr;
      return resource;
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset()
    {
      if (! IsValid())
        return;

      assert(m_convolution != nullptr);

      vxReleaseConvolution(&m_convolution);
      m_convolution = nullptr;
    }

    //! @brief Destroys any owned resources and assume control of the Convolution (this object becomes responsible for releasing it)
    void Reset(const vx_convolution convolution)
    {
      if (IsValid())
        Reset();


      m_convolution = convolution;
    }

    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vxCreateConvolution
    void Reset(const vx_context context, const vx_size columns, const vx_size rows)
    {
#ifndef RAPIDOPENVX_DISABLE_PARAM_VALIDATION
#else
#endif

      // Free any currently allocated resource
      if (IsValid())
        Reset();

      // Since we want to ensure that the resource is left untouched on error we use a local variable as a intermediary
      const vx_convolution convolution = vxCreateConvolution(context, columns, rows);
      CheckError(convolution, "vxCreateConvolution", __FILE__, __LINE__);

      // Everything is ready, so assign the members
      m_convolution = convolution;
    }

    //! @brief Get the associated resource handle
    vx_convolution Get() const
    {
      return m_convolution;
    }

    //! @brief Get a pointer to the associated resource handle
    const vx_convolution* GetPointer() const
    {
      return &m_convolution;
    }

    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_convolution != nullptr;
    }
  };
}

#endif
