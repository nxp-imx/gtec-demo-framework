#ifndef GLES3_T3DSTRESSTEST_OPTIONPARSER_HPP
#define GLES3_T3DSTRESSTEST_OPTIONPARSER_HPP
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

#include <FslDemoApp/ADemoOptionParser.hpp>
#include "Config.hpp"

namespace Fsl
{
  class OptionParser : public ADemoOptionParser
  {
    struct T3DStressTestVariables
    {
      uint16_t Vertices;
      uint16_t TextureResolution;
      bool HighShaderPrecision;
      uint8_t Lights;
      //Rectangle T3DS_dcic_roi;
      //uint8_t T3DS_dcic_period;
      //uint8_t T3DS_dcic_time_offset;
      //Rectangle T3DS_target_rect;
      //bool T3DS_freeze_on_error;
      //bool T3DS_direct_render;
      //uint8_t T3DS_gpio_group;
      //uint8_t T3DS_gpio_bit;
      //uint32_t T3DS_IOMUXC_SW_MUX_CTL_PAD;
      //uint32_t T3DS_IOMUXC_SW_MUX_CTL_PAD_value;
      int ToggleMinMax;
      uint16_t Layers;

      T3DStressTestVariables()
        : Vertices(1024)
        , TextureResolution(32)
        , HighShaderPrecision(true)
        , Lights(2)
        //, T3DS_dcic_roi(0,0,0,0)
        //, T3DS_dcic_period(0)
        //, T3DS_dcic_time_offset(0)
        //, T3DS_target_rect(0,0,1920,1080)
        //, T3DS_freeze_on_error(true)
        //, T3DS_direct_render(false)
        //, T3DS_gpio_group(0)
        //, T3DS_gpio_bit(0)
        //, T3DS_IOMUXC_SW_MUX_CTL_PAD(0)
        //, T3DS_IOMUXC_SW_MUX_CTL_PAD_value(0)
        , ToggleMinMax(false)
        , Layers(60)
      {
      }
    };

    Config m_config;
    T3DStressTestVariables m_variables;
    bool m_enableDevOverride;
  public:
    OptionParser();
    ~OptionParser();

    Config GetConfig() const { return m_config; }
  protected:
    virtual void OnArgumentSetup(std::deque<Option>& rOptions) override;
    virtual OptionParseResult::Enum OnParse(const int32_t cmdId, const char*const pszOptArg) override;
    virtual bool OnParsingComplete() override;
  };
}

#endif
