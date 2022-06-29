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

#include <FslBase/Exceptions.hpp>
#include <FslGraphics/Render/Adapter/INativeGraphics.hpp>
#include <FslGraphics/Render/Adapter/INativeTexture2D.hpp>
#include <FslGraphics/Render/BaseTexture2D.hpp>
#include <FslGraphics/Texture/RawTextureHelper.hpp>
#include <utility>

namespace Fsl
{
  BaseTexture2D& BaseTexture2D::operator=(BaseTexture2D&& other) noexcept
  {
    if (this != &other)
    {
      // Free existing resources then transfer the content of other to this one and fill other with default values
      Reset();

      // Claim ownership here
      m_native = std::move(other.m_native);
      m_extent = other.m_extent;
      m_pixelFormat = other.m_pixelFormat;

      other.m_extent = {};
      other.m_pixelFormat = {};
    }
    return *this;
  }

  BaseTexture2D::BaseTexture2D(BaseTexture2D&& other) noexcept
    : m_native(std::move(other.m_native))
    , m_extent(other.m_extent)
    , m_pixelFormat(other.m_pixelFormat)
  {
    // Remove the data from other
    other.m_extent = {};
    other.m_pixelFormat = {};
  }


  BaseTexture2D::~BaseTexture2D() = default;


  void BaseTexture2D::Reset() noexcept
  {
    m_native.reset();
    m_extent = {};
    m_pixelFormat = PixelFormat::Undefined;
  }


  std::shared_ptr<INativeTexture2D> BaseTexture2D::GetNative() const
  {
    if (!m_native)
    {
      throw GraphicsException("Not connected to native texture");
    }
    return m_native;
  }

  BaseTexture2D::BaseTexture2D(std::shared_ptr<INativeTexture2D> native, const PxExtent2D extent, const PixelFormat pixelFormat)
    : BaseTexture2D()
  {
    DoReset(std::move(native), extent, pixelFormat);
  }


  void BaseTexture2D::DoReset(std::shared_ptr<INativeTexture2D> native, const PxExtent2D extent, const PixelFormat pixelFormat)
  {
    if (!native)
    {
      throw std::invalid_argument("native texture is invalid");
    }
    if (extent.Width < 1 || extent.Height < 1)
    {
      throw std::invalid_argument("extent is invalid");
    }
    if (pixelFormat == PixelFormat::Undefined)
    {
      throw std::invalid_argument("pixelFormat is invalid");
    }
    Reset();

    m_native = std::move(native);
    m_extent = extent;
    m_pixelFormat = pixelFormat;
  }
}
