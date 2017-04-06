#ifndef FSLUTILOPENVX1_1_OBJECTARRAY_HPP
#define FSLUTILOPENVX1_1_OBJECTARRAY_HPP
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
    class ObjectArray
    {
      vx_object_array m_objectArray;
    public:
      ObjectArray(const ObjectArray&) = delete;
      ObjectArray& operator=(const ObjectArray&) = delete;

      //! @brief Move assignment operator
      ObjectArray& operator=(ObjectArray&& other)
      {
        if (this != &other)
        {
          // Free existing resources then transfer the content of other to this one and fill other with default values
          if (IsValid())
            Reset();

          // Claim ownership here
          m_objectArray = other.m_objectArray;

          // Remove the data from other
          other.m_objectArray = nullptr;
        }
        return *this;
      }

      //! @brief Move constructor
      ObjectArray(ObjectArray&& other)
        : m_objectArray(other.m_objectArray)
      {
        // Remove the data from other
        other.m_objectArray = nullptr;
      }

      //! @brief Create a 'invalid' instance (use Reset to populate it)
      ObjectArray()
        : m_objectArray(nullptr)
      {
      }

      //! @brief Assume control of the ObjectArray (this object becomes responsible for releasing it)
      explicit ObjectArray(const vx_object_array objectArray)
        : ObjectArray()
      {
        Reset(objectArray);
      }

      //! @brief Create the requested resource
      //! @note  Function: vxCreateObjectArray
      ObjectArray(const vx_context context, const vx_reference exemplar, const vx_size count)
        : ObjectArray()
      {
        Reset(context, exemplar, count);
      }

      ~ObjectArray()
      {
        Reset();
      }

      //! @brief returns the managed handle and releases the ownership.
      vx_object_array Release() FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT
      {
        const auto resource = m_objectArray;
        m_objectArray = nullptr;
        return resource;
      }

      //! @brief Destroys any owned resources and resets the object to its default state.
      void Reset()
      {
        if (! IsValid())
          return;

        assert(m_objectArray != nullptr);

        vxReleaseObjectArray(&m_objectArray);
        m_objectArray = nullptr;
      }

      //! @brief Destroys any owned resources and assume control of the ObjectArray (this object becomes responsible for releasing it)
      void Reset(const vx_object_array objectArray)
      {
        if (IsValid())
          Reset();


        m_objectArray = objectArray;
      }

      //! @brief Destroys any owned resources and then creates the requested one
      //! @note  Function: vxCreateObjectArray
      void Reset(const vx_context context, const vx_reference exemplar, const vx_size count)
      {
        // We do the check here to be user friendly, if it becomes a performance issue switch it to a assert.

        // Free any currently allocated resource
        if (IsValid())
          Reset();

        // Since we want to ensure that the resource is left untouched on error we use a local variable as a intermediary
        const vx_object_array objectArray = vxCreateObjectArray(context, exemplar, count);
        Util::Check(objectArray, "vxCreateObjectArray", __FILE__, __LINE__);

        // Everything is ready, so assign the members
        m_objectArray = objectArray;
      }

      //! @brief Get the associated resource handle
      vx_object_array Get() const
      {
        return m_objectArray;
      }

      //! @brief Get a pointer to the associated resource handle
      const vx_object_array* GetPointer() const
      {
        return &m_objectArray;
      }

      //! @brief Check if this object contains a valid resource
      inline bool IsValid() const
      {
        return m_objectArray != nullptr;
      }
    };

  }
}

#endif
