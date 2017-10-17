#ifndef RAPIDOPENVX_IMAGE_HPP
#define RAPIDOPENVX_IMAGE_HPP
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
  class Image
  {
    vx_image m_image;
  public:
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

    //! @brief Move assignment operator
    Image& operator=(Image&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_image = other.m_image;

        // Remove the data from other
        other.m_image = nullptr;
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    Image(Image&& other)
      : m_image(other.m_image)
    {
      // Remove the data from other
      other.m_image = nullptr;
    }

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    Image()
      : m_image(nullptr)
    {
    }

    //! @brief Assume control of the Image (this object becomes responsible for releasing it)
    explicit Image(const vx_image image)
      : Image()
    {
      Reset(image);
    }

    //! @brief Create the requested resource
    //! @note  Function: vxCreateImage
    Image(const vx_context context, const vx_uint32 width, const vx_uint32 height, const vx_df_image color)
      : Image()
    {
      Reset(context, width, height, color);
    }

    ~Image()
    {
      Reset();
    }

    //! @brief returns the managed handle and releases the ownership.
    vx_image Release() RAPIDOPENVX_FUNC_POSTFIX_WARN_UNUSED_RESULT
    {
      const auto resource = m_image;
      m_image = nullptr;
      return resource;
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset()
    {
      if (! IsValid())
        return;

      assert(m_image != nullptr);

      vxReleaseImage(&m_image);
      m_image = nullptr;
    }

    //! @brief Destroys any owned resources and assume control of the Image (this object becomes responsible for releasing it)
    void Reset(const vx_image image)
    {
      if (IsValid())
        Reset();


      m_image = image;
    }

    //! @brief Destroys any owned resources and then creates the requested one
    //! @note  Function: vxCreateImage
    void Reset(const vx_context context, const vx_uint32 width, const vx_uint32 height, const vx_df_image color)
    {
#ifndef RAPIDOPENVX_DISABLE_PARAM_VALIDATION
#else
#endif

      // Free any currently allocated resource
      if (IsValid())
        Reset();

      // Since we want to ensure that the resource is left untouched on error we use a local variable as a intermediary
      const vx_image image = vxCreateImage(context, width, height, color);
      CheckError(image, "vxCreateImage", __FILE__, __LINE__);

      // Everything is ready, so assign the members
      m_image = image;
    }

    //! @brief Get the associated resource handle
    vx_image Get() const
    {
      return m_image;
    }

    //! @brief Get a pointer to the associated resource handle
    const vx_image* GetPointer() const
    {
      return &m_image;
    }

    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_image != nullptr;
    }
  };
}

#endif
