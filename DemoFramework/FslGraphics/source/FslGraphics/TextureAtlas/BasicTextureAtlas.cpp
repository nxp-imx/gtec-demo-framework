/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/Exceptions.hpp>

namespace Fsl
{
  int32_t BasicTextureAtlas::Count() const
  {
    return static_cast<int32_t>(m_entries.size());
  }


  NamedAtlasTexture BasicTextureAtlas::GetEntry(const int32_t index) const
  {
    if (index < 0 || static_cast<std::size_t>(index) > m_entries.size())
    {
      throw std::invalid_argument("Out of bounds");
    }
    return NamedAtlasTexture(m_entries[index].Name, AtlasTextureInfo(m_entries[index].SrcRect, m_entries[index].TrimmedRect));
  }


  void BasicTextureAtlas::Reset(const int32_t capacity)
  {
    m_entries.resize(capacity);
  }


  void BasicTextureAtlas::SetEntry(const int32_t index, const Rectangle& srcRect, const Rectangle& trimmedRect, const UTF8String& name)
  {
    if (index < 0 || static_cast<std::size_t>(index) > m_entries.size())
    {
      throw std::invalid_argument("out of bounds");
    }

    m_entries[index] = Record(srcRect, trimmedRect, name);
  }
}
