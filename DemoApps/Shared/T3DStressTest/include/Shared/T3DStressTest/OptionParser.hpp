#ifndef SHARED_T3DSTRESSTEST_OPTIONPARSER_HPP
#define SHARED_T3DSTRESSTEST_OPTIONPARSER_HPP
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
#include "Config.hpp"

namespace Fsl
{
  class OptionParser : public ADemoOptionParser
  {
    struct T3DStressTestVariables
    {
      uint16_t Vertices{1024};
      uint16_t TextureResolution{32};
      bool HighShaderPrecision{true};
      uint8_t Lights{2};
      // Rectangle T3DS_dcic_roi;
      // uint8_t T3DS_dcic_period;
      // uint8_t T3DS_dcic_time_offset;
      // Rectangle T3DS_target_rect;
      // bool T3DS_freeze_on_error;
      // bool T3DS_direct_render;
      // uint8_t T3DS_gpio_group;
      // uint8_t T3DS_gpio_bit;
      // uint32_t T3DS_IOMUXC_SW_MUX_CTL_PAD;
      // uint32_t T3DS_IOMUXC_SW_MUX_CTL_PAD_value;
      int ToggleMinMax{0};
      uint16_t Layers{60};

      T3DStressTestVariables() = default;
    };

    Config m_config;
    T3DStressTestVariables m_variables;
    bool m_enableDevOverride;

  public:
    explicit OptionParser(const RenderMode defaultRenderMode = RenderMode::Instanced);
    ~OptionParser() override;

    Config GetConfig() const
    {
      return m_config;
    }

  protected:
    void OnArgumentSetup(std::deque<Option>& rOptions) override;
    OptionParseResult OnParse(const int32_t cmdId, const StringViewLite& strOptArg) override;
    bool OnParsingComplete() override;
  };
}

#endif
