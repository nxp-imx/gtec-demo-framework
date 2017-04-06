#ifndef FSLUTILOPENVX1_1_SCALAR_HPP
#define FSLUTILOPENVX1_1_SCALAR_HPP
/****************************************************************************************************************************************************
* Copyright (c) 2016 Freescale Semiconductor, Inc.
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
*    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

// Auto-generated OpenVX 1.1 C++11 RAII classes by RAIIGen (https://github.com/Unarmed1000)
// Auto generation template based on RapidOpenVX https://github.com/Unarmed1000/RapidOpenVX with permission.

#include <FslUtilOpenVX1_1/Common.hpp>
#include <FslUtilOpenVX1_1/Util.hpp>
#include <FslBase/Attributes.hpp>
#include <VX/vx.h>
#include <cassert>

namespace Fsl
{
  namespace OpenVX
  {
    // This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
    class Scalar
    {
      vx_scalar m_scalar;
    public:
      Scalar(const Scalar&) = delete;
      Scalar& operator=(const Scalar&) = delete;

      //! @brief Move assignment operator
      Scalar& operator=(Scalar&& other)
      {
        if (this != &other)
        {
          // Free existing resources then transfer the content of other to this one and fill other with default values
          if (IsValid())
            Reset();

          // Claim ownership here
          m_scalar = other.m_scalar;

          // Remove the data from other
          other.m_scalar = nullptr;
        }
        return *this;
      }

      //! @brief Move constructor
      Scalar(Scalar&& other)
        : m_scalar(other.m_scalar)
      {
        // Remove the data from other
        other.m_scalar = nullptr;
      }

      //! @brief Create a 'invalid' instance (use Reset to populate it)
      Scalar()
        : m_scalar(nullptr)
      {
      }

      //! @brief Assume control of the Scalar (this object becomes responsible for releasing it)
      explicit Scalar(const vx_scalar scalar)
        : Scalar()
      {
        Reset(scalar);
      }

      //! @brief Create the requested resource
      //! @note  Function: vxCreateScalar
      Scalar(const vx_context context, const vx_enum dataType, const void * ptr)
        : Scalar()
      {
        Reset(context, dataType, ptr);
      }

      ~Scalar()
      {
        Reset();
      }

      //! @brief returns the managed handle and releases the ownership.
      vx_scalar Release() FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT
      {
        const auto resource = m_scalar;
        m_scalar = nullptr;
        return resource;
      }

      //! @brief Destroys any owned resources and resets the object to its default state.
      void Reset()
      {
        if (! IsValid())
          return;

        assert(m_scalar != nullptr);

        vxReleaseScalar(&m_scalar);
        m_scalar = nullptr;
      }

      //! @brief Destroys any owned resources and assume control of the Scalar (this object becomes responsible for releasing it)
      void Reset(const vx_scalar scalar)
      {
        if (IsValid())
          Reset();


        m_scalar = scalar;
      }

      //! @brief Destroys any owned resources and then creates the requested one
      //! @note  Function: vxCreateScalar
      void Reset(const vx_context context, const vx_enum dataType, const void * ptr)
      {
        // We do the check here to be user friendly, if it becomes a performance issue switch it to a assert.

        // Free any currently allocated resource
        if (IsValid())
          Reset();

        // Since we want to ensure that the resource is left untouched on error we use a local variable as a intermediary
        const vx_scalar scalar = vxCreateScalar(context, dataType, ptr);
        Util::Check(scalar, "vxCreateScalar", __FILE__, __LINE__);

        // Everything is ready, so assign the members
        m_scalar = scalar;
      }

      //! @brief Get the associated resource handle
      vx_scalar Get() const
      {
        return m_scalar;
      }

      //! @brief Get a pointer to the associated resource handle
      const vx_scalar* GetPointer() const
      {
        return &m_scalar;
      }

      //! @brief Check if this object contains a valid resource
      inline bool IsValid() const
      {
        return m_scalar != nullptr;
      }
    };

  }
}

#endif
