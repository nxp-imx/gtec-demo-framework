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

#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/Font/BasicFontKerning.hpp>
#include <utility>

namespace Fsl
{
  const UTF8String& BasicFontKerning::GetName() const
  {
    return m_name;
  }


  const IO::Path& BasicFontKerning::GetPathName() const
  {
    return m_pathName;
  }


  FontDesc BasicFontKerning::GetDesc() const
  {
    return m_fontDesc;
  }


  int32_t BasicFontKerning::RangeCount() const
  {
    return 1;
  }


  FontGlyphRange BasicFontKerning::GetRange(const int32_t index) const
  {
    if (index < 0 || index >= RangeCount())
    {
      throw std::invalid_argument("range index out of bounds");
    }
    return m_range;
  }


  int32_t BasicFontKerning::Count() const
  {
    return static_cast<int32_t>(m_entries.size());
  }


  FontGlyphBasicKerning BasicFontKerning::Get(const int32_t index) const
  {
    if (index < 0 || static_cast<std::size_t>(index) >= m_entries.size())
    {
      throw std::invalid_argument("glyph index out of bounds");
    }

    return m_entries[index];
  }


  void BasicFontKerning::SetRangeCapacity(const int32_t capacity)    // NOLINT(readability-convert-member-functions-to-static)
  {
    if (capacity != 1)
    {
      throw NotSupportedException("We only support a range capacity of one");
    }
  }


  void BasicFontKerning::SetGlyphKerningCapacity(const int32_t capacity)
  {
    m_entries.resize(capacity);
  }


  void BasicFontKerning::SetName(const UTF8String& name)
  {
    m_name = name;
  }


  void BasicFontKerning::SetPathName(const IO::PathView& name)
  {
    m_pathName = name;
  }


  void BasicFontKerning::SetPathName(IO::Path name)
  {
    m_pathName = std::move(name);
  }


  void BasicFontKerning::SetDesc(const FontDesc& desc)
  {
    m_fontDesc = desc;
  }


  void BasicFontKerning::SetRange(const int32_t index, const FontGlyphRange& range)
  {
    if (index != 0)
    {
      throw std::invalid_argument("range index out of bounds");
    }
    m_range = range;
  }


  void BasicFontKerning::SetGlyphKerning(const int32_t index, const FontGlyphBasicKerning& kerning)
  {
    if (index < 0 || static_cast<std::size_t>(index) >= m_entries.size())
    {
      throw std::invalid_argument("kerning index out of bounds");
    }
    m_entries[index] = kerning;
  }


  bool BasicFontKerning::IsValid() const    // NOLINT(readability-convert-member-functions-to-static)
  {
    // FIX: implement a validation check
    return true;
  }

}
