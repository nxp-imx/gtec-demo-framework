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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <Shared/AntiAliasing/OptionParser.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <optional>

namespace Fsl
{
  namespace
  {
    struct CommandId
    {
      enum Enum
      {
        Content = DEMO_APP_OPTION_BASE,
        AAMethod,
        EnableFastResolve,
        Paused,
      };
    };

    struct ContentRecord
    {
      ContentWindowId Content{ContentWindowId::Car};
      StringViewLite Name;

      constexpr ContentRecord() noexcept = default;
      constexpr ContentRecord(const ContentWindowId content, const StringViewLite name) noexcept
        : Content(content)
        , Name(name)
      {
      }
    };

    struct AARecord
    {
      AntiAliasingMethod Method{AntiAliasingMethod::Disabled};
      StringViewLite Name;

      constexpr AARecord() noexcept = default;
      constexpr AARecord(const AntiAliasingMethod method, const StringViewLite name) noexcept
        : Method(method)
        , Name(name)
      {
      }
    };

    constexpr std::array<ContentRecord, 3> Content = {ContentRecord(ContentWindowId::Car, "car"), ContentRecord(ContentWindowId::Knight, "knight"),
                                                      ContentRecord(ContentWindowId::WireframeCube, "cube")};

    constexpr std::array<AARecord, 5> AAMethods = {AARecord(AntiAliasingMethod::Disabled, "disabled"), AARecord(AntiAliasingMethod::MSAA2X, "msaa2x"),
                                                   AARecord(AntiAliasingMethod::MSAA4X, "msaa4x"), AARecord(AntiAliasingMethod::MSAA8X, "msaa8x"),
                                                   AARecord(AntiAliasingMethod::SSAA2X, "ssaa2x")};


    ContentWindowId ToContentWindowId(const int32_t contentId)
    {
      if (contentId >= 0 && contentId <= UncheckedNumericCast<int32_t>(Content.size()))
      {
        return Content[contentId].Content;
      }
      throw NotSupportedException(fmt::format("ContentId: {}", contentId));
    }

    std::optional<ContentWindowId> TryParseContent(const StringViewLite& strValue)
    {
      for (const auto& entry : Content)
      {
        if (entry.Name == strValue)
        {
          return entry.Content;
        }
      }
      return {};
    }

    std::string GetContentString()
    {
      std::string res;
      if (!Content.empty())
      {
        res = Content[0].Name;
        for (std::size_t i = 1; i < Content.size(); ++i)
        {
          res += ", ";
          res += Content[i].Name;
        }
      }
      return res;
    }

    std::string GetAAMethodsString()
    {
      std::string res;
      if (!AAMethods.empty())
      {
        res = AAMethods[0].Name;
        for (std::size_t i = 1; i < AAMethods.size(); ++i)
        {
          res += ", ";
          res += AAMethods[i].Name;
        }
      }
      return res;
    }

    AntiAliasingMethod ToAntiAliasingMethod(const StringViewLite& strValue)
    {
      for (const auto& entry : AAMethods)
      {
        if (entry.Name == strValue)
        {
          return entry.Method;
        }
      }
      throw NotSupportedException(fmt::format("Method: '{}'. valid methods: {}", strValue, GetAAMethodsString()));
    }
  }

  OptionParser::OptionParser()
    : m_contentId(ContentWindowId::Car)
  {
  }


  OptionParser::~OptionParser() = default;


  void OptionParser::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.emplace_back("Content", OptionArgument::OptionRequired, CommandId::Content,
                          fmt::format("Select the content to focus on: {}", GetContentString()));
    rOptions.emplace_back("Method", OptionArgument::OptionRequired, CommandId::AAMethod,
                          fmt::format("Select the anti-aliasing method: {}", GetAAMethodsString()));
    rOptions.emplace_back("FastResolve", OptionArgument::OptionRequired, CommandId::EnableFastResolve,
                          "Enable fast resolve: true, false (does nothing if fast resolve is unavailable)");
    rOptions.emplace_back("p", "Pause", OptionArgument::OptionRequired, CommandId::Paused, "Pause the app: true, false");
  }


  OptionParseResult OptionParser::OnParse(const int32_t cmdId, const StringViewLite& strOptArg)
  {
    int32_t intValue = 0;
    bool boolValue = false;

    switch (cmdId)
    {
    case CommandId::Content:
      {
        const auto result = TryParseContent(strOptArg);
        if (result.has_value())
        {
          m_contentId = result.value();
        }
        else
        {
          if (StringParseUtil::Parse(intValue, strOptArg) <= 0)
          {
            return OptionParseResult::Failed;
          }
          m_contentId = ToContentWindowId(intValue);
        }
        return OptionParseResult::Parsed;
      }
    case CommandId::AAMethod:
      m_aaMethod = ToAntiAliasingMethod(strOptArg);
      return OptionParseResult::Parsed;
    case CommandId::EnableFastResolve:
      if (StringParseUtil::Parse(boolValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_enableFastResolve = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::Paused:
      if (StringParseUtil::Parse(boolValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_paused = boolValue;
      return OptionParseResult::Parsed;
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
