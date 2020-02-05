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

#include "OptionParser.hpp"
#include <FslBase/BasicTypes.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include <FslBase/Exceptions.hpp>
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
        Scene = DEMO_APP_OPTION_BASE,
        RenderMode,
        KernelLength,
        Sigma,
        ShaderType,
        Compare,
        CRMFlags,
        Caption,
        AutoSigma,
        DemoMode
      };
    };
  }

  OptionParser::OptionParser()
    : m_demoMode(false)
  {
  }


  OptionParser::~OptionParser() = default;


  void OptionParser::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.emplace_back("Scene", OptionArgument::OptionRequired, CommandId::Scene, "Select the scene to run (0 to 1)");
    rOptions.emplace_back("r", "RenderMode", OptionArgument::OptionRequired, CommandId::RenderMode,
                          "Select the render mode (0=ReferenceTwoPass, 1=ReferenceTwoPassLinear, 2=TwoPassLinarScaled, 3=ReferenceOnePass).");
    rOptions.emplace_back("l", "KernelLength", OptionArgument::OptionRequired, CommandId::KernelLength, "Set the gaussian blur kernel length");
    rOptions.emplace_back("s", "Sigma", OptionArgument::OptionRequired, CommandId::Sigma, "Set the gaussian blur sigma.");
    rOptions.emplace_back("t", "ShaderType", OptionArgument::OptionRequired, CommandId::ShaderType,
                          "Set the shader type (0=HardCoded, 1=NonDependent).");
    rOptions.emplace_back("c", "Compare", OptionArgument::OptionNone, CommandId::Compare, "Enable RenderMode compare.");
    rOptions.emplace_back(
      "f", "CRMFlags", OptionArgument::OptionNone, CommandId::CRMFlags,
      "Compare render mode flags. Render mode 0 is bit 0, render mode1 is bit 1. Defaults to all bits on except for the one pass reference");
    rOptions.emplace_back("AutoSigma", OptionArgument::OptionNone, CommandId::AutoSigma, "Try to use a optimal sigma for the kernel size");
    rOptions.emplace_back("Caption", OptionArgument::OptionRequired, CommandId::Caption, "Set the caption type (0=off,1=Algorithm,2=Description)");
    rOptions.emplace_back("d", "Demo", OptionArgument::OptionNone, CommandId::DemoMode, "Lock into demo mode (nothing can be overridden).");
  }


  OptionParseResult OptionParser::OnParse(const int32_t cmdId, const char* const pszOptArg)
  {
    //    bool boolValue;
    int intValue;
    uint32_t uintValue;
    float floatValue;

    switch (cmdId)
    {
    case CommandId::RenderMode:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_modified.Flag(ModifiedFlags::RenderMode);
      m_config.SetRenderMode(static_cast<RenderMode::Enum>(intValue));
      return OptionParseResult::Parsed;
    case CommandId::KernelLength:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_modified.Flag(ModifiedFlags::KernelLength);
      m_config.SetKernelLength(intValue);
      return OptionParseResult::Parsed;
    case CommandId::Sigma:
      if (StringParseUtil::Parse(floatValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_modified.Flag(ModifiedFlags::Sigma);
      m_config.SetSigma(floatValue);
      return OptionParseResult::Parsed;
    case CommandId::ShaderType:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_modified.Flag(ModifiedFlags::ShaderType);
      m_config.SetShaderType(intValue);
      return OptionParseResult::Parsed;
    case CommandId::Compare:
      m_config.SetCompareEnabled(true);
      return OptionParseResult::Parsed;
    case CommandId::CRMFlags:
      if (StringParseUtil::Parse(uintValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetCompareEnabled(true);
      m_config.SetCRMFlags(uintValue);
      return OptionParseResult::Parsed;
    case CommandId::DemoMode:
      m_demoMode = true;
      return OptionParseResult::Parsed;
    case CommandId::Caption:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_config.SetCaptionType(intValue);
      return OptionParseResult::Parsed;
    case CommandId::AutoSigma:
      m_config.SetUseOptimalSigma(true);
      return OptionParseResult::Parsed;
    default:
      return OptionParseResult::NotHandled;
    }
  }


  bool OptionParser::OnParsingComplete()
  {
    if (m_demoMode)
    {
      FSLLOG3_INFO("DemoMode enabled, all other arguments ignored!");
      m_config = Config();
      m_config.SetCompareEnabled(false);
      m_config.SetUseOptimalSigma(false);
      m_config.SetSigma(17);
      m_config.SetKernelLength(75);
      m_config.SetRenderMode(RenderMode::TwoPassLinarScaled);
      m_config.SetShaderType(ShaderType::HardCoded);
      m_config.SetCaptionType(CaptionType::Description);
    }

    // If you return false, the app exits.
    return true;
  }
}
