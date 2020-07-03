#ifndef FSLGRAPHICS_FONT_BASICFONTKERNING_HPP
#define FSLGRAPHICS_FONT_BASICFONTKERNING_HPP
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

#include <FslBase/IO/Path.hpp>
#include <FslBase/String/UTF8String.hpp>
#include <FslGraphics/Font/IFontBasicKerning.hpp>
#include <vector>

namespace Fsl
{
  class UTF8String;

  class BasicFontKerning final : public IFontBasicKerning
  {
    UTF8String m_name;
    IO::Path m_pathName;
    FontDesc m_fontDesc;
    FontGlyphRange m_range;
    std::vector<FontGlyphBasicKerning> m_entries;

  public:
    BasicFontKerning(const BasicFontKerning&) = delete;
    BasicFontKerning& operator=(const BasicFontKerning&) = delete;

    BasicFontKerning& operator=(BasicFontKerning&& other) noexcept
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        Clear();

        // Claim ownership here
        m_name = std::move(other.m_name);
        m_pathName = std::move(other.m_pathName);
        m_fontDesc = other.m_fontDesc;
        m_range = other.m_range;
        m_entries = std::move(other.m_entries);

        // Remove the data from other
        other.m_fontDesc = {};
        other.m_range = {};
      }
      return *this;
    }

    BasicFontKerning(BasicFontKerning&& other) noexcept
      : m_name(std::move(other.m_name))
      , m_pathName(std::move(other.m_pathName))
      , m_fontDesc(other.m_fontDesc)
      , m_range(other.m_range)
      , m_entries(std::move(other.m_entries))
    {
      other.m_fontDesc = {};
      other.m_range = {};
    }

    BasicFontKerning() = default;

    const UTF8String& GetName() const final;
    const IO::Path& GetPathName() const final;
    FontDesc GetDesc() const final;
    int32_t RangeCount() const final;
    FontGlyphRange GetRange(const int32_t index) const final;
    int32_t Count() const final;
    FontGlyphBasicKerning Get(const int32_t index) const final;

    void SetRangeCapacity(const int32_t capacity);
    void SetGlyphKerningCapacity(const int32_t capacity);

    void SetName(const UTF8String& name);
    void SetPathName(const IO::PathView& name);
    void SetPathName(IO::Path name);
    void SetDesc(const FontDesc& desc);
    void SetRange(const int32_t index, const FontGlyphRange& range);
    void SetGlyphKerning(const int32_t index, const FontGlyphBasicKerning& kerning);

    bool IsValid() const;

  private:
    void Clear() noexcept
    {
      m_name.Clear();
      m_pathName.Clear();
      m_fontDesc = {};
      m_range = {};
      m_entries.clear();
    }
  };
}

#endif
