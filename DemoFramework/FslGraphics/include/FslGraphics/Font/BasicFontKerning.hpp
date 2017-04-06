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

#include <FslBase/String/UTF8String.hpp>
#include <FslGraphics/Font/IFontBasicKerning.hpp>
#include <vector>

namespace Fsl
{
  class UTF8String;

  class BasicFontKerning : public IFontBasicKerning
  {
    UTF8String m_name;
    UTF8String m_pathName;
    FontDesc m_fontDesc;
    FontGlyphRange m_range;
    std::vector<FontGlyphBasicKerning> m_entries;
  public:
    virtual UTF8String GetName() const override;
    virtual UTF8String GetPathName() const override;
    virtual FontDesc GetDesc() const override;
    virtual int32_t RangeCount() const override;
    virtual FontGlyphRange GetRange(const int32_t index) const override;
    virtual int32_t Count() const override;
    virtual FontGlyphBasicKerning Get(const int32_t index) const override;

    void SetRangeCapacity(const int32_t capacity);
    void SetGlyphKerningCapacity(const int32_t capacity);

    void SetName(const UTF8String& name);
    void SetPathName(const UTF8String& name);
    void SetDesc(const FontDesc& desc);
    void SetRange(const int32_t index, const FontGlyphRange& range);
    void SetGlyphKerning(const int32_t index, const FontGlyphBasicKerning& kerning);

    bool IsValid() const;
  };
}

#endif
