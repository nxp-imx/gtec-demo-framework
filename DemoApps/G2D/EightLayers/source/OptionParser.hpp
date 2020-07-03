#ifndef G2D_EIGHTLAYERS_OPTIONPARSER_HPP
#define G2D_EIGHTLAYERS_OPTIONPARSER_HPP
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

#include <FslDemoApp/Base/ADemoOptionParser.hpp>

namespace Fsl
{
  class OptionParser : public ADemoOptionParser
  {
    bool m_useForceVSync;
    bool m_useCache;
    bool m_use32BPP;
    bool m_useAnimation;
    bool m_useManual;
    bool m_alignFB;
    bool m_useOffscreen;
    bool m_useDummyMode;
    int m_offscreenFormat;
    int m_test;
    int m_test2;

  public:
    OptionParser();
    ~OptionParser();

    const bool IsForceVSyncEnabled() const
    {
      return m_useForceVSync;
    }
    const bool IsCacheEnabled() const
    {
      return m_useCache;
    }
    const bool Use32BPP() const
    {
      return m_use32BPP;
    }
    const bool UseAnimation() const
    {
      return m_useAnimation;
    }
    const bool UseManual() const
    {
      return m_useManual;
    }
    const bool GetAlignFrameBuffer() const
    {
      return m_alignFB;
    }
    const bool UseDummyMode() const
    {
      return m_useDummyMode;
    }
    const bool UseOffscreen() const
    {
      return m_useOffscreen;
    }
    const int GetOffscreenFormat() const
    {
      return m_offscreenFormat;
    }
    const int GetTestValue() const
    {
      return m_test;
    }
    const int GetTest2Value() const
    {
      return m_test2;
    }

  protected:
    virtual void OnArgumentSetup(std::deque<Option>& rOptions) override;
    virtual OptionParseResult OnParse(const int32_t cmdId, const StringViewLite& strOptArg) override;
    virtual bool OnParsingComplete() override;
  };
}

#endif
