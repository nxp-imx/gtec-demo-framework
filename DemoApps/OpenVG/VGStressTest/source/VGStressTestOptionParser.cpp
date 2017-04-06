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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include "VGStressTestOptionParser.hpp"

namespace Fsl
{
  namespace
  {
    struct CommandId
    {
      enum Enum
      {
        SpiralTwists = DEMO_APP_OPTION_BASE,
        UseTexture,
        //VGS_freeze_on_error,
        //VGS_target_rect,
        //VGS_dcic_period,
        //VGS_dcic_time_offset,
        //VGS_gpio_group,
        //VGS_gpio_bit,
        //VGS_IOMUXC_SW_MUX_CTL_PAD,
        //VGS_IOMUXC_SW_MUX_CTL_PAD_value,
        Layers,
        AdaptToScreen,
        //VGS_direct_render,
        ToggleMinMax,
        //VGS_dcic_roi,

        DevOverride,
        DSetType,
        DQuadricSpiralLayers,
        DQuadricSpiralRevolutionCount,
        DQuadricSpiralRevolutionChange,
        DQuadricSpiralStrokeLineWidth,
        DSegmentedSpiralRevolutionCount,
        DSegmentedSpiralRevolutionChange,
        DSegmentedSpiralStrokeLineWidth,
        DSegmentedSpiralSegmentsPerRevolution,
        DAdaptToScreen
      };

    };
  }

  VGStressTestOptionParser::VGStressTestOptionParser()
    : m_config()
    , m_enableDevOverride(false)
  {
  }


  VGStressTestOptionParser::~VGStressTestOptionParser()
  {

  }


  const ConfigSpiral VGStressTestOptionParser::GetConfig() const
  {
    return m_config;
  }

  void VGStressTestOptionParser::OnArgumentSetup(std::deque<Option>& rOptions)
  {



    rOptions.push_back(Option("SpiralTwists", OptionArgument::OptionRequired, CommandId::SpiralTwists, "Number of times the spiral twists, controls geometry complexity and overdraw"));
    rOptions.push_back(Option("UseTexture", OptionArgument::OptionRequired, CommandId::UseTexture, "Determines whether the spiral should be filled with a texture or just with semitransparent colors"));
    //rOptions.push_back(Option("VGS_freeze_on_error", OptionArgument::OptionRequired, CommandId::VGS_freeze_on_error, "Decides whether the test should continue even though an error was found with the DCIC"));
    //rOptions.push_back(Option("VGS_target_rect", OptionArgument::OptionRequired, CommandId::VGS_target_rect, "Target rectangle on screen where the output of the VG test will be placed. The VG test will always be placed in higher Z order than the Composer Layers. If resolution is different that source size, scaling is used"));
    //rOptions.push_back(Option("VGS_dcic_period", OptionArgument::OptionRequired, CommandId::VGS_dcic_period, "Number of seconds between DCIC checks"));
    //rOptions.push_back(Option("VGS_dcic_time_offset", OptionArgument::OptionRequired, CommandId::VGS_dcic_time_offset, "Time offset in seconds from beginning of test to first DCIC check"));
    //rOptions.push_back(Option("VGS_gpio_group", OptionArgument::OptionRequired, CommandId::VGS_gpio_group, "The GPIO group of the selected pad"));
    //rOptions.push_back(Option("VGS_gpio_bit", OptionArgument::OptionRequired, CommandId::VGS_gpio_bit, "The bit index of the selected pad in the GPIO group"));
    //rOptions.push_back(Option("VGS_IOMUXC_SW_MUX_CTL_PAD", OptionArgument::OptionRequired, CommandId::VGS_IOMUXC_SW_MUX_CTL_PAD, "Address of the IOMUX register controlling the mode of the selected PAD. If 0, the GPIO is disabled."));
    //rOptions.push_back(Option("VGS_IOMUXC_SW_MUX_CTL_PAD_value", OptionArgument::OptionRequired, CommandId::VGS_IOMUXC_SW_MUX_CTL_PAD_value, "Value (ALT mode) of the IOMUX control register"));
    rOptions.push_back(Option("Layers", OptionArgument::OptionRequired, CommandId::Layers, "Number of times the spiral is drawn,controls geometry complexity and overdraw"));
    rOptions.push_back(Option("AdaptToScreen", OptionArgument::OptionRequired, CommandId::AdaptToScreen, "Adapt the spiral to fit on screen"));
    //rOptions.push_back(Option("DirectRender", OptionArgument::OptionRequired, CommandId::DirectRender, "Decides whether test runs directly on framebuffer, bypassing composition"));
    rOptions.push_back(Option("ToggleMinMax", OptionArgument::OptionRequired, CommandId::ToggleMinMax, "Decides whether the test should switch between 0 and configured load every 5 seconds"));
    //rOptions.push_back(Option("VGS_dcic_roi", OptionArgument::OptionRequired, CommandId::VGS_dcic_roi, "Region of interest for DCIC. When periodic checksum test runs, the area outside the ROI will be rendered transparent\nIf the rect is set to [0,0,0,0], the DCIC check is disabled."));

    // Development options
    rOptions.push_back(Option("DevOverride", OptionArgument::OptionNone, CommandId::DevOverride, "Allow the developer options to override other options", OptionGroup::Hidden));
    rOptions.push_back(Option("DSetType", OptionArgument::OptionRequired, CommandId::DSetType, "The type of spiral to render (0=Quadric, 1=Quadric textured, 2=Cubic, 3=Segmented", OptionGroup::Hidden));
    rOptions.push_back(Option("DQuadricSpiralLayers", OptionArgument::OptionRequired, CommandId::DQuadricSpiralLayers, "The number of layers to render (only works with quadric spirals atm)", OptionGroup::Hidden));
    rOptions.push_back(Option("DQuadricSpiralRevolutionCount", OptionArgument::OptionRequired, CommandId::DQuadricSpiralRevolutionCount, "The number of spiral revolutions", OptionGroup::Hidden));
    rOptions.push_back(Option("DQuadricSpiralRevolutionChange", OptionArgument::OptionRequired, CommandId::DQuadricSpiralRevolutionChange, "The number of pixels to grow per revolution", OptionGroup::Hidden));
    rOptions.push_back(Option("DQuadricSpiralStrokeLineWidth", OptionArgument::OptionRequired, CommandId::DQuadricSpiralStrokeLineWidth, "The line width", OptionGroup::Hidden));
    rOptions.push_back(Option("DSegmentedSpiralRevolutionCount", OptionArgument::OptionRequired, CommandId::DSegmentedSpiralRevolutionCount, "The number of spiral revolutions (segmented & cubic)", OptionGroup::Hidden));
    rOptions.push_back(Option("DSegmentedSpiralRevolutionChange", OptionArgument::OptionRequired, CommandId::DSegmentedSpiralRevolutionChange, "The number of pixels to grow per revolution (segmented & cubic)", OptionGroup::Hidden));
    rOptions.push_back(Option("DSegmentedSpiralStrokeLineWidth", OptionArgument::OptionRequired, CommandId::DSegmentedSpiralStrokeLineWidth, "The line width (used for segmented and cubic)", OptionGroup::Hidden));
    rOptions.push_back(Option("DSegmentedSpiralSegmentsPerRevolution", OptionArgument::OptionRequired, CommandId::DSegmentedSpiralSegmentsPerRevolution, "The number of segments per revolution (segmented)", OptionGroup::Hidden));
    rOptions.push_back(Option("DAdaptToScreen", OptionArgument::OptionRequired, CommandId::DAdaptToScreen, "Adapt the spiral to fit on screen (If true the revolution change parameter is ignored)", OptionGroup::Hidden));
  }


  OptionParseResult::Enum VGStressTestOptionParser::OnParse(const int32_t cmdId, const char*const pszOptArg)
  {
    bool boolValue;
    int intValue;
    float floatValue;

    switch (cmdId)
    {
    case CommandId::SpiralTwists:
      return (StringParseUtil::Parse(m_variables.SpiralTwists, pszOptArg) > 0 ? OptionParseResult::Parsed : OptionParseResult::Failed);
    case CommandId::UseTexture:
      return (StringParseUtil::Parse(m_variables.UseTexture, pszOptArg) > 0 ? OptionParseResult::Parsed : OptionParseResult::Failed);
    case CommandId::Layers:
      return (StringParseUtil::Parse(m_variables.Layers, pszOptArg) > 0 ? OptionParseResult::Parsed : OptionParseResult::Failed);
    case CommandId::AdaptToScreen:
      return (StringParseUtil::Parse(m_variables.AdaptToScreen, pszOptArg) > 0 ? OptionParseResult::Parsed : OptionParseResult::Failed);
    case CommandId::ToggleMinMax:
      return (StringParseUtil::Parse(m_variables.ToggleMinMax, pszOptArg) > 0 ? OptionParseResult::Parsed : OptionParseResult::Failed);
    case CommandId::DevOverride:
      m_enableDevOverride = true;
      return OptionParseResult::Parsed;
    case CommandId::DSetType:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_config.SetType(intValue);
      return OptionParseResult::Parsed;
    case CommandId::DQuadricSpiralLayers:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_config.SetQuadricSpiralLayers(intValue);
      return OptionParseResult::Parsed;
    case CommandId::DQuadricSpiralRevolutionCount:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_config.SetQuadricSpiralRevolutionCount(intValue);
      return OptionParseResult::Parsed;
    case CommandId::DQuadricSpiralRevolutionChange:
      if (StringParseUtil::Parse(floatValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_config.SetQuadricSpiralRevolutionChange(floatValue);
      return OptionParseResult::Parsed;
    case CommandId::DQuadricSpiralStrokeLineWidth:
      if (StringParseUtil::Parse(floatValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_config.SetQuadricSpiralStrokeLineWidth(floatValue);
      return OptionParseResult::Parsed;
    case CommandId::DSegmentedSpiralRevolutionCount:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_config.SetSegmentedSpiralRevolutionCount(intValue);
      return OptionParseResult::Parsed;
    case CommandId::DSegmentedSpiralRevolutionChange:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_config.SetSegmentedSpiralRevolutionChange(static_cast<float>(intValue));
      return OptionParseResult::Parsed;
    case CommandId::DSegmentedSpiralStrokeLineWidth:
      if (StringParseUtil::Parse(floatValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_config.SetSegmentedSpiralStrokeLineWidth(floatValue);
      return OptionParseResult::Parsed;
    case CommandId::DSegmentedSpiralSegmentsPerRevolution:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_config.SetSegmentedSpiralSegmentsPerRevolution(intValue);
      return OptionParseResult::Parsed;
    case CommandId::DAdaptToScreen:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_config.SetAdaptToScreen(boolValue);
      return OptionParseResult::Parsed;
    default:
      return OptionParseResult::NotHandled;
    }
  }


  bool VGStressTestOptionParser::OnParsingComplete()
  {
    if (!m_enableDevOverride)
    {
      m_config.SetQuadricSpiralRevolutionCount(m_variables.SpiralTwists);
      m_config.SetType(m_variables.UseTexture);
      m_config.SetToggleMinMax(m_variables.ToggleMinMax);

      m_config.SetQuadricSpiralLayers(m_variables.Layers);

      m_config.SetAdaptToScreen(m_variables.AdaptToScreen);
    }

    //m_configDcic.SetFreezeOnError(m_variables.VGS_freeze_on_error != 0);
    //m_configDcic.SetPeriod(m_variables.VGS_dcic_period);
    //m_configDcic.SetTimeOffset(m_variables.VGS_dcic_time_offset);
    //m_configDcic.SetROI(m_variables.VGS_dcic_roi[0], m_variables.VGS_dcic_roi[1], m_variables.VGS_dcic_roi[2], m_variables.VGS_dcic_roi[3]);

    //m_configGpio.SetIOMUX(m_variables.VGS_IOMUXC_SW_MUX_CTL_PAD);
    //m_configGpio.SetIOMUXValue(m_variables.VGS_IOMUXC_SW_MUX_CTL_PAD_value);
    //m_configGpio.SetGPIOGroup(m_variables.VGS_gpio_group);
    //m_configGpio.SetGPIOBit(m_variables.VGS_gpio_bit);
    return true;
  }

}
