#ifndef RAPIDOPENVX_DISTRIBUTION_HPP
#define RAPIDOPENVX_DISTRIBUTION_HPP
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
  class Distribution
  {
    vx_distribution m_distribution;
  public:
    Distribution(const Distribution&) = delete;
    Distribution& operator=(const Distribution&) = delete;

    //! @brief Move assignment operator
    Distribution& operator=(Distribution&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_distribution = other.m_distribution;

        // Remove the data from other
        other.m_distribution = nullptr;
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    Distribution(Distribution&& other)
      : m_distribution(other.m_distribution)
    {
      // Remove the data from other
      other.m_distribution = nullptr;
    }

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    Distribution()
      : m_distribution(nullptr)
    {
    }

    //! @brief Assume control of the Distribution (this object becomes responsible for releasing it)
    explicit Distribution(const vx_distribution distribution)
      : Distribution()
    {
      Reset(distribution);
    }

    //! @brief Create the requested resource
    //! @note  Function: vxCreateDistribution
    Distribution(const vx_context context, const vx_size numBins, const vx_int32 offset, const vx_uint32 range)
      : Distribution()
    {
      Reset(context, numBins, offset, range);
    }

    ~Distribution()
    {
      Reset();
    }

    //! @brief returns the managed handle and releases the ownership.
    vx_distribution Release() RAPIDOPENVX_FUNC_POSTFIX_WARN_UNUSED_RESULT
    {
      const auto resource = m_distribution;
      m_distribution = nullptr;
      return resource;
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset()
    {
      if (! IsValid())
        return;

      assert(m_distribution != nullptr);

      vxReleaseDistribution(&m_distribution);
      m_distribution = nullptr;
    }

    //! @brief Destroys any owned resources and assume control of the Distribution (this object becomes responsible for releasing it)
    void Reset(const vx_distribution distribution)
    {
      if (IsValid())
        Reset();


      m_distribution = distribution;
    }

    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vxCreateDistribution
    void Reset(const vx_context context, const vx_size numBins, const vx_int32 offset, const vx_uint32 range)
    {
#ifndef RAPIDOPENVX_DISABLE_PARAM_VALIDATION
#else
#endif

      // Free any currently allocated resource
      if (IsValid())
        Reset();

      // Since we want to ensure that the resource is left untouched on error we use a local variable as a intermediary
      const vx_distribution distribution = vxCreateDistribution(context, numBins, offset, range);
      CheckError(distribution, "vxCreateDistribution", __FILE__, __LINE__);

      // Everything is ready, so assign the members
      m_distribution = distribution;
    }

    //! @brief Get the associated resource handle
    vx_distribution Get() const
    {
      return m_distribution;
    }

    //! @brief Get a pointer to the associated resource handle
    const vx_distribution* GetPointer() const
    {
      return &m_distribution;
    }

    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_distribution != nullptr;
    }
  };
}

#endif
