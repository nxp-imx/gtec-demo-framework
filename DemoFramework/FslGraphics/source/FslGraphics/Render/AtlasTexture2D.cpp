/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <FslBase/Exceptions.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>

namespace Fsl
{
  AtlasTexture2D& AtlasTexture2D::operator=(AtlasTexture2D&& other) noexcept
  {
    if (this != &other)
    {
      // Free existing resources then transfer the content of other to this one and fill other with default values
      Reset();

      // Claim ownership here
      m_atlas = std::move(other.m_atlas);
      m_info = other.m_info;

      other.m_info = {};
    }
    return *this;
  }

  AtlasTexture2D::AtlasTexture2D(AtlasTexture2D&& other) noexcept
    : m_atlas(std::move(other.m_atlas))
    , m_info(other.m_info)
  {
    // Remove the data from other
    other.m_info = {};
  }


  AtlasTexture2D::AtlasTexture2D(const BaseTexture2D& texAtlas, const AtlasTextureInfo& info)
    : m_atlas(texAtlas)
    , m_info(texAtlas.IsValid() ? info : AtlasTextureInfo())
  {
  }


  AtlasTexture2D::~AtlasTexture2D() = default;


  bool AtlasTexture2D::IsValid() const
  {
    return m_atlas.IsValid();
  }


  std::shared_ptr<INativeTexture2D> AtlasTexture2D::TryGetNative() const
  {
    return m_atlas.TryGetNative();
  }


  void AtlasTexture2D::Reset() noexcept
  {
    m_atlas.Reset();
    m_info = {};
  }


  void AtlasTexture2D::Reset(const BaseTexture2D& texAtlas, const AtlasTextureInfo& info)
  {
    m_atlas = texAtlas;
    m_info = texAtlas.IsValid() ? info : AtlasTextureInfo();
  }
}
