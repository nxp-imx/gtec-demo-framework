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
#include <FslBase/Exceptions.hpp>
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <Shared/FurShellRendering/OptionParser.hpp>
#include <algorithm>
#include <cmath>
#include <cstring>

namespace Fsl
{
  namespace
  {
    struct CommandId
    {
      enum Enum
      {
        RenderMode = DEMO_APP_OPTION_BASE,
        LayerCount,
        HairLength,
        HairDensity,
        FurTextureSize,
        TorusMajorSegments,
        TorusMinorSegments,
        UseTriangleStrips,
        ShowNormals,
        TextureRepeatCountX,
        TextureRepeatCountY,
        HighShaderPrecision,
        Lights,
        ForceFinish,
        Demo,
        Quality,
        BackgroundColor
      };
    };
  }

  OptionParser::OptionParser(const int32_t defaultRenderMode)
    : m_config(defaultRenderMode)
    , m_quality(Quality::Low)
  {
  }


  OptionParser::~OptionParser() = default;


  void OptionParser::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.emplace_back("RenderMode", OptionArgument::OptionRequired, CommandId::RenderMode,
                          "The render mode: 0=Multi-pass, 1=Multi-pass VB, 2=OldSchool instancing, 3=ES3 instancing (default).");
    rOptions.emplace_back("LayerCount", OptionArgument::OptionRequired, CommandId::LayerCount, "The number of layers to use for rendering the fur");
    rOptions.emplace_back("HairLength", OptionArgument::OptionRequired, CommandId::HairLength, "The length of the hairs");
    rOptions.emplace_back("HairDensity", OptionArgument::OptionRequired, CommandId::HairDensity, "The hair density");
    rOptions.emplace_back("FurTextureSize", OptionArgument::OptionRequired, CommandId::FurTextureSize,
                          "This controls the resolution of the fur density texture, applied on both axis");
    rOptions.emplace_back("TorusMajorSegments", OptionArgument::OptionRequired, CommandId::TorusMajorSegments, "...");
    rOptions.emplace_back("TorusMinorSegments", OptionArgument::OptionRequired, CommandId::TorusMinorSegments, "...");
    rOptions.emplace_back("UseTriangleStrip", OptionArgument::OptionRequired, CommandId::UseTriangleStrips,
                          "Use triangle strips if true, triangle lists if false");
    rOptions.emplace_back("ShowNormals", OptionArgument::OptionRequired, CommandId::ShowNormals, "Render the normals");
    rOptions.emplace_back("TextureRepeatCountX", OptionArgument::OptionRequired, CommandId::TextureRepeatCountX,
                          "Controls the amount of times we show the texture in the x direction (1=once, 2=twice)");
    rOptions.emplace_back("TextureRepeatCountY", OptionArgument::OptionRequired, CommandId::TextureRepeatCountY,
                          "Controls the amount of times we show the texture in the y direction (1=once, 2=twice)");
    rOptions.emplace_back("HighShaderPrecision", OptionArgument::OptionRequired, CommandId::HighShaderPrecision,
                          "Shader arithmetic precision. \nfalse = low, \ntrue = high");
    rOptions.emplace_back("Lights", OptionArgument::OptionRequired, CommandId::Lights,
                          "number of light sources used in fragment shader calculations");
    rOptions.emplace_back("ForceFinish", OptionArgument::OptionRequired, CommandId::ForceFinish, "If true each frame will execute a glFinish call");
    rOptions.emplace_back("Demo", OptionArgument::OptionRequired, CommandId::Demo, "Select the demo to run (0 to 3)");
    rOptions.emplace_back("Quality", OptionArgument::OptionRequired, CommandId::Quality, "Select the rendering quality (low,medium,high).");
    rOptions.emplace_back("BackgroundColor", OptionArgument::OptionRequired, CommandId::BackgroundColor,
                          "Set the background color 0xRRGGBB (use decimal values).");
  }


  OptionParseResult OptionParser::OnParse(const int32_t cmdId, const StringViewLite& strOptArg)
  {
    bool boolValue = false;
    int intValue = 0;
    uint32_t uint32Value = 0;
    float floatValue = 0.0f;

    switch (cmdId)
    {
    case CommandId::RenderMode:
      if (StringParseUtil::Parse(intValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetRenderMode(intValue);
      return OptionParseResult::Parsed;
    case CommandId::LayerCount:
      if (StringParseUtil::Parse(intValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetLayerCount(intValue);
      m_modified.Flag(ModifiedFlags::LayerCount);
      return OptionParseResult::Parsed;
    case CommandId::HairLength:
      if (StringParseUtil::Parse(floatValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetHairLength(floatValue);
      m_modified.Flag(ModifiedFlags::HairLength);
      return OptionParseResult::Parsed;
    case CommandId::HairDensity:
      if (StringParseUtil::Parse(floatValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetHairDensity(floatValue);
      return OptionParseResult::Parsed;
    case CommandId::FurTextureSize:
      if (StringParseUtil::Parse(intValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      if (intValue < 0 || intValue >= std::numeric_limits<uint16_t>::max())
      {
        throw std::invalid_argument("Texture resolution out of range");
      }
      m_config.SetFurTextureSize(intValue);
      m_modified.Flag(ModifiedFlags::FurTextureDimension);
      return OptionParseResult::Parsed;
    case CommandId::TorusMajorSegments:
      if (StringParseUtil::Parse(intValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetTorusMajorSegments(intValue);
      m_modified.Flag(ModifiedFlags::TorusMajor);
      return OptionParseResult::Parsed;
    case CommandId::TorusMinorSegments:
      if (StringParseUtil::Parse(intValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetTorusMinorSegments(intValue);
      m_modified.Flag(ModifiedFlags::TorusMinor);
      return OptionParseResult::Parsed;
    case CommandId::UseTriangleStrips:
      if (StringParseUtil::Parse(boolValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetUseTriangleStrip(boolValue);
      return OptionParseResult::Parsed;
    case CommandId::ShowNormals:
      if (StringParseUtil::Parse(boolValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetShowNormals(boolValue);
      return OptionParseResult::Parsed;
    case CommandId::TextureRepeatCountX:
      if (StringParseUtil::Parse(intValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetTextureRepeatCountX(intValue);
      m_modified.Flag(ModifiedFlags::TextureRepeatX);
      return OptionParseResult::Parsed;
    case CommandId::TextureRepeatCountY:
      if (StringParseUtil::Parse(intValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetTextureRepeatCountY(intValue);
      m_modified.Flag(ModifiedFlags::TextureRepeatY);
      return OptionParseResult::Parsed;
    case CommandId::HighShaderPrecision:
      if (StringParseUtil::Parse(boolValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetUseHighShaderPrecision(boolValue);
      return OptionParseResult::Parsed;
    case CommandId::Lights:
      if (StringParseUtil::Parse(intValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetLightCount(intValue);
      return OptionParseResult::Parsed;
    case CommandId::ForceFinish:
      if (StringParseUtil::Parse(boolValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetForceFinishEachFrame(boolValue);
      return OptionParseResult::Parsed;
    case CommandId::BackgroundColor:
      if (StringParseUtil::Parse(uint32Value, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetBackgroundColor(uint32Value);
      m_modified.Flag(ModifiedFlags::BackgroundColor);
      return OptionParseResult::Parsed;
    case CommandId::Demo:
      if (StringParseUtil::Parse(intValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetDemoId(intValue);
      return OptionParseResult::Parsed;
    case CommandId::Quality:
      if (strOptArg == "low")
      {
        m_quality = Quality::Low;
      }
      else if (strOptArg == "medium")
      {
        m_quality = Quality::Medium;
      }
      else if (strOptArg == "high")
      {
        m_quality = Quality::High;
      }
      else
      {
        return OptionParseResult::Failed;
      }
      return OptionParseResult::Parsed;
    default:
      return OptionParseResult::NotHandled;
    }
  }


  bool OptionParser::OnParsingComplete()
  {
    switch (m_quality)
    {
    case Quality::Low:
      if (!m_modified.IsFlagged(ModifiedFlags::LayerCount))
      {
        m_config.SetLayerCount(10);
      }
      if (!m_modified.IsFlagged(ModifiedFlags::FurTextureDimension))
      {
        if (m_config.GetDemoId() != 0 && m_config.GetDemoId() != 3)
        {
          m_config.SetFurTextureSize(PxSize2D::Create(512, 512));
        }
        else
        {
          m_config.SetFurTextureSize(PxSize2D::Create(1024, 512));
        }
      }
      if (!m_modified.IsFlagged(ModifiedFlags::TextureRepeatX))
      {
        m_config.SetTextureRepeatCountX(1);
      }
      if (!m_modified.IsFlagged(ModifiedFlags::TextureRepeatY))
      {
        m_config.SetTextureRepeatCountY(1);
      }
      if (!m_modified.IsFlagged(ModifiedFlags::HairLength))
      {
        m_config.SetHairLength(2.0f);
      }
      if (!m_modified.IsFlagged(ModifiedFlags::TorusMajor))
      {
        m_config.SetTorusMajorSegments(64);
      }
      if (!m_modified.IsFlagged(ModifiedFlags::TorusMinor))
      {
        m_config.SetTorusMinorSegments(64);
      }
      if (m_config.GetDemoId() == 0 && !m_modified.IsFlagged(ModifiedFlags::BackgroundColor))
      {
        m_config.SetBackgroundColor(0xFF000000);
      }
      break;
    case Quality::Medium:
      if (!m_modified.IsFlagged(ModifiedFlags::LayerCount))
      {
        m_config.SetLayerCount(20);
      }
      if (!m_modified.IsFlagged(ModifiedFlags::FurTextureDimension))
      {
        if (m_config.GetDemoId() != 0 && m_config.GetDemoId() != 3)
        {
          m_config.SetFurTextureSize(PxSize2D::Create(1024, 1024));
        }
        else
        {
          m_config.SetFurTextureSize(PxSize2D::Create(1024, 512));
        }
      }
      if (!m_modified.IsFlagged(ModifiedFlags::TextureRepeatX))
      {
        m_config.SetTextureRepeatCountX(1);
      }
      if (!m_modified.IsFlagged(ModifiedFlags::TextureRepeatY))
      {
        m_config.SetTextureRepeatCountY(1);
      }
      if (!m_modified.IsFlagged(ModifiedFlags::HairLength))
      {
        m_config.SetHairLength(4.0f);
      }
      if (!m_modified.IsFlagged(ModifiedFlags::TorusMajor))
      {
        m_config.SetTorusMajorSegments(64);
      }
      if (!m_modified.IsFlagged(ModifiedFlags::TorusMinor))
      {
        m_config.SetTorusMinorSegments(64);
      }
      if (m_config.GetDemoId() == 0 && !m_modified.IsFlagged(ModifiedFlags::BackgroundColor))
      {
        m_config.SetBackgroundColor(0xFF000000);
      }
      break;
    case Quality::High:
      if (!m_modified.IsFlagged(ModifiedFlags::LayerCount))
      {
        m_config.SetLayerCount(40);
      }
      if (!m_modified.IsFlagged(ModifiedFlags::FurTextureDimension))
      {
        if (m_config.GetDemoId() != 0 && m_config.GetDemoId() != 3)
        {
          m_config.SetFurTextureSize(PxSize2D::Create(1024, 1024));
        }
        else
        {
          m_config.SetFurTextureSize(PxSize2D::Create(1024, 512));
        }
      }
      if (!m_modified.IsFlagged(ModifiedFlags::TextureRepeatX))
      {
        m_config.SetTextureRepeatCountX(1);
      }
      if (!m_modified.IsFlagged(ModifiedFlags::TextureRepeatY))
      {
        m_config.SetTextureRepeatCountY(1);
      }
      if (!m_modified.IsFlagged(ModifiedFlags::HairLength))
      {
        m_config.SetHairLength(4.0f);
      }
      if (!m_modified.IsFlagged(ModifiedFlags::TorusMajor))
      {
        m_config.SetTorusMajorSegments(64);
      }
      if (!m_modified.IsFlagged(ModifiedFlags::TorusMinor))
      {
        m_config.SetTorusMinorSegments(64);
      }

      if (m_config.GetDemoId() == 0 && !m_modified.IsFlagged(ModifiedFlags::BackgroundColor))
      {
        m_config.SetBackgroundColor(0xFF000000);
      }
      break;
    default:
      break;
    }

    // If you return false, the app exits.
    return true;
  }
}
