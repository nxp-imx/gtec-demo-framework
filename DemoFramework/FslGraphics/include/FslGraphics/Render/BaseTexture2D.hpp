#ifndef FSLGRAPHICS_RENDER_BASETEXTURE2D_HPP
#define FSLGRAPHICS_RENDER_BASETEXTURE2D_HPP
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

#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <memory>

namespace Fsl
{
  class INativeTexture2D;

  //! @brief A simple texture base class
  class BaseTexture2D
  {
    std::shared_ptr<INativeTexture2D> m_native;
    PxExtent2D m_extent;
    PixelFormat m_pixelFormat{PixelFormat::Undefined};

  public:
    // move assignment operator
    BaseTexture2D& operator=(BaseTexture2D&& other) noexcept;
    // move constructor
    BaseTexture2D(BaseTexture2D&& other) noexcept;

    // Request that the compiler generates a copy constructor and assignment operator
    BaseTexture2D(const BaseTexture2D&) = default;
    BaseTexture2D& operator=(const BaseTexture2D&) = default;


    //! @brief Create a uninitialized texture (use SetData to add texture data to it)
    BaseTexture2D() = default;

    ~BaseTexture2D();

    //! @brief Check if this contains a valid texture.
    bool IsValid() const
    {
      return m_native != nullptr;
    }

    //! @brief If a texture is allocated this will releases it.
    void Reset() noexcept;

    //! @brief Get the texture size.
    PxExtent2D GetExtent() const
    {
      return m_extent;
    }

    //! @brief Get the texture size.
    PxSize2D GetSize() const noexcept;

    PixelFormat GetPixelFormat() const noexcept
    {
      return m_pixelFormat;
    }


    //! @brief Acquire the native texture (returns null if none exist)
    std::shared_ptr<INativeTexture2D> TryGetNative() const
    {
      return m_native;
    }

    const INativeTexture2D* TryGetNativePointer() const
    {
      return m_native.get();
    }


    //! @brief Acquire the native texture (throws if none exist)
    std::shared_ptr<INativeTexture2D> GetNative() const;


    bool operator==(const BaseTexture2D& rhs) const
    {
      return m_native == rhs.m_native && m_extent == rhs.m_extent;
    }


    bool operator!=(const BaseTexture2D& rhs) const
    {
      return !(*this == rhs);
    }

  protected:
    BaseTexture2D(std::shared_ptr<INativeTexture2D> native, const PxExtent2D extent, const PixelFormat pixelFormat);
    void DoReset(std::shared_ptr<INativeTexture2D> native, const PxExtent2D extent, const PixelFormat pixelFormat);
  };
}

#endif
