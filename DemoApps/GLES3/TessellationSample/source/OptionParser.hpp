#ifndef GLES3_TESSELLATIONSAMPLE_OPTIONPARSER_HPP
#define GLES3_TESSELLATIONSAMPLE_OPTIONPARSER_HPP
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

#include <FslBase/Math/Vector3.hpp>
#include <FslDemoApp/Base/ADemoOptionParser.hpp>

namespace Fsl
{
  class OptionParser : public ADemoOptionParser
  {
    int32_t m_scene;
    float m_displacementFactor;
    float m_displacementMod;
    float m_fieldOfView;
    Vector3 m_matSpecular;
    float m_matShininess;

  public:
    OptionParser();
    ~OptionParser() override;

    int32_t GetScene() const
    {
      return m_scene;
    }
    float GetDisplacementFactor() const
    {
      return m_displacementFactor;
    }
    float GetDisplacementMod() const
    {
      return m_displacementMod;
    }
    float GetFieldOfView() const
    {
      return m_fieldOfView;
    }
    Vector3 GetMatSpecular() const
    {
      return m_matSpecular;
    }
    float GetMatShininess() const
    {
      return m_matShininess;
    }

  protected:
    void OnArgumentSetup(std::deque<Option>& rOptions) override;
    OptionParseResult::Enum OnParse(const int32_t cmdId, const char* const pszOptArg) override;
    bool OnParsingComplete() override;
  };
}

#endif
