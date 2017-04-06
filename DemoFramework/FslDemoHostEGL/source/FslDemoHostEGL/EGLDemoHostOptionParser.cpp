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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <FslDemoHostEGL/EGLDemoHostOptionParser.hpp>
#include <algorithm>

namespace Fsl
{
  namespace
  {
    struct CommandId
    {
      enum Enum
      {
        LogConfig = DEMO_HOST_OPTION_BASE,
        RedSize,
        GreenSize,
        BlueSize,
        AlphaSize,
        DepthSize
      };
    };


    OptionParseResult::Enum AddSize(std::deque<EGLint>& rConfigAttributes, EGLint attrib, const char*const pszOptArg, const char*const pszError)
    {
      int32_t intValue;
      StringParseUtil::Parse(intValue, pszOptArg);
      if (intValue < 0)
        throw std::invalid_argument(pszError);
      rConfigAttributes.push_back(attrib);
      rConfigAttributes.push_back(intValue);
      return OptionParseResult::Parsed;

    }
  }

  EGLDemoHostOptionParser::EGLDemoHostOptionParser()
    : m_logConfig(false)
    , m_configAttributes()
  {
  }


  void EGLDemoHostOptionParser::ArgumentSetup(std::deque<Option>& rOptions)
  {
    ADemoHostOptionParser::ArgumentSetup(rOptions);

    rOptions.push_back(Option("EGLLogConfig", OptionArgument::OptionNone, CommandId::LogConfig, "Output the EGL config to the log", OptionGroup::Host));
    rOptions.push_back(Option("EGLRedSize", OptionArgument::OptionRequired, CommandId::RedSize, "Force EGL_RED_SIZE to the given value", OptionGroup::Host));
    rOptions.push_back(Option("EGLGreenSize", OptionArgument::OptionRequired, CommandId::GreenSize, "Force EGL_GREEN_SIZE to the given value", OptionGroup::Host));
    rOptions.push_back(Option("EGLBlueSize", OptionArgument::OptionRequired, CommandId::BlueSize, "Force EGL_BLUE_SIZE to the given value", OptionGroup::Host));
    rOptions.push_back(Option("EGLAlphaSize", OptionArgument::OptionRequired, CommandId::AlphaSize, "Force EGL_ALPHA_SIZE to the given value", OptionGroup::Host));
    rOptions.push_back(Option("EGLDepthSize", OptionArgument::OptionRequired, CommandId::DepthSize, "Force EGL_DEPTH_SIZE to the given value", OptionGroup::Host));
  }


  OptionParseResult::Enum EGLDemoHostOptionParser::Parse(const int32_t cmdId, const char*const pszOptArg)
  {
    // We rely on the fact that a command line argument can be specified once (so no duplicated entries are added)
    switch (cmdId)
    {
    case CommandId::LogConfig:
      m_logConfig = true;
      return OptionParseResult::Parsed;
    case CommandId::RedSize:
      return AddSize(m_configAttributes, EGL_RED_SIZE, pszOptArg, "EGLRedSize can not be negative");
    case CommandId::GreenSize:
      return AddSize(m_configAttributes, EGL_GREEN_SIZE, pszOptArg, "EGLGreenSize can not be negative");
    case CommandId::BlueSize:
      return AddSize(m_configAttributes, EGL_BLUE_SIZE, pszOptArg, "EGLBlueSize can not be negative");
    case CommandId::AlphaSize:
      return AddSize(m_configAttributes, EGL_ALPHA_SIZE, pszOptArg, "EGLAlphaSize can not be negative");
    case CommandId::DepthSize:
      return AddSize(m_configAttributes, EGL_DEPTH_SIZE, pszOptArg, "EGLDepthSize can not be negative");
    default:
      return ADemoHostOptionParser::Parse(cmdId, pszOptArg);
    }
  }


  bool EGLDemoHostOptionParser::ParsingComplete()
  {
    return ADemoHostOptionParser::ParsingComplete();
  }


  bool EGLDemoHostOptionParser::IsLogConfigEnabled() const
  {
    return m_logConfig;
  }


  void EGLDemoHostOptionParser::ExtractConfigAttributes(std::deque<EGLint>& rConfigAttributes)
  {
    if (m_configAttributes.size() > 0)
    {
      rConfigAttributes.clear();
      rConfigAttributes.resize(m_configAttributes.size());
      std::copy(m_configAttributes.begin(), m_configAttributes.end(), rConfigAttributes.begin());
      rConfigAttributes.push_back(EGL_NONE);
    }
  }

}
