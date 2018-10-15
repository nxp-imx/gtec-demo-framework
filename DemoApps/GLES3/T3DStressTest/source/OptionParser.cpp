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
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include <FslBase/Exceptions.hpp>
#include <algorithm>
#include <cmath>
#include "OptionParser.hpp"

namespace Fsl
{
  namespace
  {
    struct CommandId
    {
      enum Enum
      {
        RenderMode = DEMO_APP_OPTION_BASE,
        Vertices,
        TextureResolution,
        HighShaderPrecision,
        Lights,
        ToggleMinMax,
        Layers,
        ForceFinish,
        // Development options
        DevOverride,
        DLayerCount,
        DHairLength,
        DHairDensity,
        DFurTextureDimensions,
        DInstanceCount,
        DVertexCountX,
        DVertexCountY,
        DUseTriangleStrip,
        DCameraDistance,
        DShowNormals,
        DEnableDepthTest,
        DShareInstanceVertices,
        DTextureRepeatCount,
      };
    };
  }

  OptionParser::OptionParser()
    : m_enableDevOverride(false)
  {
  }


  OptionParser::~OptionParser() = default;


  void OptionParser::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.emplace_back("RenderMode", OptionArgument::OptionRequired, CommandId::RenderMode,
                          "The render mode: 0=Multi-pass, 1=ES3 instancing (default).");
    rOptions.emplace_back("Vertices", OptionArgument::OptionRequired, CommandId::Vertices, "Number of vertices in geometry used");
    rOptions.emplace_back("TextureResolution", OptionArgument::OptionRequired, CommandId::TextureResolution,
                          "This controls the resolution of both noise and color textures, applied on both axis");
    rOptions.emplace_back("HighShaderPrecision", OptionArgument::OptionRequired, CommandId::HighShaderPrecision,
                          "Shader arithmetic precision. \nfalse = low, \ntrue = high");
    rOptions.emplace_back("Lights", OptionArgument::OptionRequired, CommandId::Lights,
                          "number of light sources used in fragment shader calculations");
    // rOptions.push_back(Option("T3DS_dcic_roi", OptionArgument::OptionRequired, CommandId::T3DS_dcic_roi, "Region of interest for DCIC. When
    // periodic checksum test runs, the area outside the ROI will be rendered transparent\nIf the rect is set to [0,0,0,0], the DCIC check is
    // disabled."));  rOptions.push_back(Option("T3DS_dcic_period", OptionArgument::OptionRequired, CommandId::T3DS_dcic_period, "Number of seconds
    // between DCIC checks"));  rOptions.push_back(Option("T3DS_dcic_time_offset", OptionArgument::OptionRequired, CommandId::T3DS_dcic_time_offset,
    // "Time offset in seconds from beginning of test to first DCIC check"));  rOptions.push_back(Option("T3DS_target_rect",
    // OptionArgument::OptionRequired, CommandId::T3DS_target_rect, "Target rectangle on screen where the output of the 3D test will be placed. The 3D
    // test will always be placed in higher Z order than the VG and 2D Layers. If resolution is different that source size, scaling is used"));
    // rOptions.push_back(Option("T3DS_freeze_on_error", OptionArgument::OptionRequired, CommandId::T3DS_freeze_on_error, "Decides whether the test
    // should continue even though an error was found with the DCIC"));  rOptions.push_back(Option("T3DS_direct_render",
    // OptionArgument::OptionRequired, CommandId::T3DS_direct_render, "Decides whether test runs directly on framebuffer, bypassing composition"));
    // rOptions.push_back(Option("T3DS_gpio_group", OptionArgument::OptionRequired, CommandId::T3DS_gpio_group, "The GPIO group of the selected
    // pad"));  rOptions.push_back(Option("T3DS_gpio_bit", OptionArgument::OptionRequired, CommandId::T3DS_gpio_bit, "The bit index of the selected
    // pad in the GPIO group"));  rOptions.push_back(Option("T3DS_IOMUXC_SW_MUX_CTL_PAD", OptionArgument::OptionRequired,
    // CommandId::T3DS_IOMUXC_SW_MUX_CTL_PAD, "Address of the IOMUX register controlling the mode of the selected PAD. If 0, the GPIO is disabled."));
    // rOptions.push_back(Option("T3DS_IOMUXC_SW_MUX_CTL_PAD_value", OptionArgument::OptionRequired, CommandId::T3DS_IOMUXC_SW_MUX_CTL_PAD_value,
    // "Value (ALT mode) of the IOMUX control register"));
    rOptions.emplace_back("ToggleMinMax", OptionArgument::OptionRequired, CommandId::ToggleMinMax,
                          "Decides whether the test should switch between 0 and configured load every 5 seconds");
    rOptions.emplace_back("Layers", OptionArgument::OptionRequired, CommandId::Layers, "number of shell texture layers used for fur rendering");
    rOptions.emplace_back("ForceFinish", OptionArgument::OptionRequired, CommandId::ForceFinish, "If true each frame will execute a glFinish call");

    // Development options
    rOptions.emplace_back("DevOverride", OptionArgument::OptionNone, CommandId::DevOverride, "Allow the developer options to override other options",
                          OptionGroup::Hidden);
    rOptions.emplace_back("DLayerCount", OptionArgument::OptionRequired, CommandId::DLayerCount, "The number of layers to use for rendering the fur",
                          OptionGroup::Hidden);
    rOptions.emplace_back("DHairLength", OptionArgument::OptionRequired, CommandId::DHairLength, "The length of the hairs", OptionGroup::Hidden);
    rOptions.emplace_back("DHairDensity", OptionArgument::OptionRequired, CommandId::DHairDensity, "The hair density", OptionGroup::Hidden);
    rOptions.emplace_back("DFurTextureDimensions", OptionArgument::OptionRequired, CommandId::DFurTextureDimensions,
                          "The size of the fur texture (pow 2 value, if too large it will crash the app)", OptionGroup::Hidden);
    rOptions.emplace_back("DInstanceCount", OptionArgument::OptionRequired, CommandId::DInstanceCount,
                          "The number of instances that should be generated of the mesh (if too large it will crash the app)", OptionGroup::Hidden);
    rOptions.emplace_back("DVertexCountX", OptionArgument::OptionRequired, CommandId::DVertexCountX,
                          "The number of vertices that the quad consists of in X (>= 2, if too large it will crash the app)", OptionGroup::Hidden);
    rOptions.emplace_back("DVertexCountY", OptionArgument::OptionRequired, CommandId::DVertexCountY,
                          "The number of vertices that the quad consists of in y (>= 2, if too large it will crash the app)", OptionGroup::Hidden);
    rOptions.emplace_back("DUseTriangleStrip", OptionArgument::OptionRequired, CommandId::DUseTriangleStrip,
                          "Use triangle strips if true, triangle lists if false", OptionGroup::Hidden);
    rOptions.emplace_back("DCameraDistance", OptionArgument::OptionRequired, CommandId::DCameraDistance, "Set the distance of the camera",
                          OptionGroup::Hidden);
    rOptions.emplace_back("DShowNormals", OptionArgument::OptionRequired, CommandId::DShowNormals, "Render the normals", OptionGroup::Hidden);
    rOptions.emplace_back("DEnableDepthTest", OptionArgument::OptionRequired, CommandId::DEnableDepthTest, "Enable / disable z-buffer checks",
                          OptionGroup::Hidden);
    rOptions.emplace_back("DShareInstanceVertices", OptionArgument::OptionRequired, CommandId::DShareInstanceVertices,
                          "Controls if instances shares the same vertices ", OptionGroup::Hidden);
    rOptions.emplace_back("DTextureRepeatCount", OptionArgument::OptionRequired, CommandId::DTextureRepeatCount,
                          "Controls the amount of times we show the texture in both x+y (1=once, 2=twice)", OptionGroup::Hidden);
  }


  OptionParseResult::Enum OptionParser::OnParse(const int32_t cmdId, const char* const pszOptArg)
  {
    bool boolValue;
    int intValue;
    float floatValue;

    switch (cmdId)
    {
    case CommandId::RenderMode:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetRenderMode(static_cast<RenderMode>(intValue));
      return OptionParseResult::Parsed;
    case CommandId::Vertices:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_variables.Vertices = intValue;
      return OptionParseResult::Parsed;
    case CommandId::TextureResolution:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      if (intValue < 0 || intValue >= std::numeric_limits<uint16_t>::max())
      {
        throw std::invalid_argument("Texture resolution out of range");
      }
      m_variables.TextureResolution = static_cast<uint16_t>(intValue);
      return OptionParseResult::Parsed;
    case CommandId::HighShaderPrecision:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_variables.HighShaderPrecision = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::Lights:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_variables.Lights = intValue;
      return OptionParseResult::Parsed;
    case CommandId::ToggleMinMax:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_variables.ToggleMinMax = static_cast<int>(boolValue);
      return OptionParseResult::Parsed;
    case CommandId::Layers:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_variables.Layers = intValue;
      return OptionParseResult::Parsed;
    case CommandId::ForceFinish:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetForceFinishEachFrame(boolValue);
      return OptionParseResult::Parsed;
    case CommandId::DevOverride:
      m_enableDevOverride = true;
      return OptionParseResult::Parsed;
    case CommandId::DLayerCount:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetLayerCount(intValue);
      return OptionParseResult::Parsed;
    case CommandId::DHairLength:
      if (StringParseUtil::Parse(floatValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetHairLength(floatValue);
      return OptionParseResult::Parsed;
    case CommandId::DHairDensity:
      if (StringParseUtil::Parse(floatValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetHairDensity(floatValue);
      return OptionParseResult::Parsed;
    case CommandId::DFurTextureDimensions:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetFurTextureDimensions(intValue);
      return OptionParseResult::Parsed;
    case CommandId::DInstanceCount:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetInstanceCount(intValue);
      return OptionParseResult::Parsed;
    case CommandId::DVertexCountX:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetVertexCountX(intValue);
      return OptionParseResult::Parsed;
    case CommandId::DVertexCountY:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetVertexCountY(intValue);
      return OptionParseResult::Parsed;
    case CommandId::DUseTriangleStrip:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetUseTriangleStrip(boolValue);
      return OptionParseResult::Parsed;
    case CommandId::DCameraDistance:
      if (StringParseUtil::Parse(floatValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetCameraDistance(floatValue);
      return OptionParseResult::Parsed;
    case CommandId::DShowNormals:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetShowNormals(boolValue);
      return OptionParseResult::Parsed;
    case CommandId::DEnableDepthTest:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetEnableDepthTest(boolValue);
      return OptionParseResult::Parsed;
    case CommandId::DShareInstanceVertices:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetShareInstanceVertices(boolValue);
      return OptionParseResult::Parsed;
    case CommandId::DTextureRepeatCount:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetTextureRepeatCount(intValue);
      return OptionParseResult::Parsed;
    default:
      return OptionParseResult::NotHandled;
    }
  }


  bool OptionParser::OnParsingComplete()
  {
    if (!m_enableDevOverride)
    {
      if (m_variables.Vertices < 4)    //|| m_variables.Vertices > 0xFFFF)
      {
        FSLLOG_ERROR("Vertices should be in the range 4-65535");
        return false;
      }
      // m_variables.TextureResolution = 1024;

      // We use a simple setup for now
      const auto vertexCountXY = static_cast<int>(std::sqrt(m_variables.Vertices));
      m_config.SetVertexCountX(vertexCountXY);
      m_config.SetVertexCountY(vertexCountXY);
      m_config.SetInstanceCount(1);

      // m_config.SetVertexCountX(2);
      // m_config.SetVertexCountY(2);
      // m_config.SetInstanceCount(m_variables.Vertices / 4);

      m_config.SetFurTextureDimensions(m_variables.TextureResolution);
      m_config.SetLayerCount(m_variables.Layers);

      // Make sure we always have some nice fur to look at instead of block heaven :)
      const int magicRepeatNum = std::max(2048 / MathHelper::ToPowerOfTwo(m_config.GetFurTextureDimensions()), 1);
      m_config.SetTextureRepeatCount(magicRepeatNum);

      m_config.SetUseHighShaderPrecision(static_cast<int>(m_variables.HighShaderPrecision) != 0);
      m_config.SetLightCount(m_variables.Lights);

      m_config.SetToggleMinMax(m_variables.ToggleMinMax != 0);
    }

    // m_configDcic.SetFreezeOnError(m_variables.T3DS_freeze_on_error != 0);
    // m_configDcic.SetPeriod(m_variables.T3DS_dcic_period);
    // m_configDcic.SetTimeOffset(m_variables.T3DS_dcic_time_offset);
    // m_configDcic.SetROI(m_variables.T3DS_dcic_roi[0], m_variables.T3DS_dcic_roi[1], m_variables.T3DS_dcic_roi[2], m_variables.T3DS_dcic_roi[3]);

    // m_configGpio.SetIOMUX(m_variables.T3DS_IOMUXC_SW_MUX_CTL_PAD);
    // m_configGpio.SetIOMUXValue(m_variables.T3DS_IOMUXC_SW_MUX_CTL_PAD_value);
    // m_configGpio.SetGPIOGroup(m_variables.T3DS_gpio_group);
    // m_configGpio.SetGPIOBit(m_variables.T3DS_gpio_bit);

    // m_configRender.SetTargetRectangle(m_variables.T3DS_target_rect[0], m_variables.T3DS_target_rect[1], m_variables.T3DS_target_rect[2],
    // m_variables.T3DS_target_rect[3]);  m_configRender.SetWindowMode(m_variables.T3DS_direct_render != 0 ? RenderWindowMode::Fullscreen :
    // RenderWindowMode::Window);
    // If you return false, the app exits.
    return true;
  }
}
