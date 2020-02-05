#ifndef OPENVG_VGSTRESSTEST_VGSTRESSTESTOPTIONPARSER_HPP
#define OPENVG_VGSTRESSTEST_VGSTRESSTESTOPTIONPARSER_HPP
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
#include "ConfigSpiral.hpp"

namespace Fsl
{
  class VGStressTestOptionParser : public ADemoOptionParser
  {
    struct VGStressTestVariables
    {
      uint16_t SpiralTwists;
      bool UseTexture;
      // bool VGS_freeze_on_error;
      // int16_t VGS_target_rect[4];
      // uint8_t VGS_dcic_period;
      // uint8_t VGS_dcic_time_offset;
      // uint8_t VGS_gpio_group;
      // uint8_t VGS_gpio_bit;
      // uint32_t VGS_IOMUXC_SW_MUX_CTL_PAD;
      // uint32_t VGS_IOMUXC_SW_MUX_CTL_PAD_value;
      uint16_t Layers;
      bool AdaptToScreen;
      // bool VGS_direct_render;
      bool ToggleMinMax;
      // Rectangle VGS_dcic_roi;

      VGStressTestVariables()
        : SpiralTwists(10)
        , UseTexture(false)
        , Layers(50)
        , AdaptToScreen(false)
        , ToggleMinMax(false)
      {
      }

      // result.VGS_freeze_on_error = 1;
      // result.VGS_target_rect[0] = 0;
      // result.VGS_target_rect[1] = 0;
      // result.VGS_target_rect[2] = 1920;
      // result.VGS_target_rect[3] = 1080;
      // result.VGS_dcic_period = 0;
      // result.VGS_dcic_time_offset = 0;
      // result.VGS_gpio_group = 0;
      // result.VGS_gpio_bit = 0;
      // result.VGS_IOMUXC_SW_MUX_CTL_PAD = 0;
      // result.VGS_IOMUXC_SW_MUX_CTL_PAD_value = 0;
      // result.VGS_direct_render = 0;
      // result.VGS_dcic_roi[0] = 0;
      // result.VGS_dcic_roi[1] = 0;
      // result.VGS_dcic_roi[2] = 0;
      // result.VGS_dcic_roi[3] = 0;
    };

    VGStressTestVariables m_variables;
    ConfigSpiral m_config;
    bool m_enableDevOverride;

  public:
    VGStressTestOptionParser();
    ~VGStressTestOptionParser();

    const ConfigSpiral GetConfig() const;

  protected:
    virtual void OnArgumentSetup(std::deque<Option>& rOptions) override;
    virtual OptionParseResult OnParse(const int32_t cmdId, const char* const pszOptArg) override;
    virtual bool OnParsingComplete() override;
  };
}

#endif
