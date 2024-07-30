/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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
 *    * Neither the name of the NXP. nor the names of
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
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslBase/String/StringParseUtil.hpp>
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
        BmpFile = DEMO_APP_OPTION_BASE,
        OutputFile,
        Type,
      };
    };
    std::map<std::string, ImageType> g_formatList = {{"Gray", ImageType::Gray}, {"Rgb", ImageType::Rgb}};
  }

  OptionParser::OptionParser()
    : m_bmpfilename("rgb.bmp")
    , m_outputfilename("gauss.bmp")
    , m_type(ImageType::Gray)
  {
  }

  OptionParser::~OptionParser() = default;

  void OptionParser::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.emplace_back("InputBmpFile", OptionArgument::OptionRequired, CommandId::BmpFile, "Input BMP file.");
    rOptions.emplace_back("OutputBmpFile", OptionArgument::OptionRequired, CommandId::OutputFile, "Output BMP file.");
    rOptions.emplace_back("type", OptionArgument::OptionRequired, CommandId::Type, "Select type: Gray, Rgb (Default to Gray).");
  }

  OptionParseResult OptionParser::OnParse(const int32_t cmdId, const StringViewLite& strOptArg)
  {
    switch (cmdId)
    {
    case CommandId::BmpFile:
      m_bmpfilename = strOptArg;
      return OptionParseResult::Parsed;
    case CommandId::OutputFile:
      m_outputfilename = strOptArg;
      return OptionParseResult::Parsed;
    case CommandId::Type:
      {
        auto entry = g_formatList.find(std::string(strOptArg));
        if (entry == g_formatList.end())
        {
          FSLLOG3_ERROR("Unsupported type option: '{}'", strOptArg);
          return OptionParseResult::Failed;
        }
        m_type = entry->second;
        return OptionParseResult::Parsed;
      }
    default:
      return OptionParseResult::NotHandled;
    }
  }

  bool OptionParser::OnParsingComplete()
  {
    // If you return false, the app exits.
    return true;
  }
}
