/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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
#include <FslBase/IO/File.hpp>
#include <FslBase/Log/IO/FmtPath.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/String/FmtStringViewLite.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <Shared/ModelViewer/OptionParser.hpp>
#include <algorithm>
#include <array>
#include <cmath>

namespace Fsl
{
  namespace
  {
    struct CommandId
    {
      enum Enum
      {
        Scene = DEMO_APP_OPTION_BASE,
        Model,
        Texture,
        MeshMode,
        ShaderMode,
        Wireframe
      };
    };

    template <typename TArray>
    std::string GetHelpString(const TArray& entries)
    {
      fmt::memory_buffer buf;
      fmt::format_to(std::back_inserter(buf), "{} ({})", entries[0].first, static_cast<int32_t>(entries[0].second));

      for (std::size_t i = 1; i < entries.size(); ++i)
      {
        fmt::format_to(std::back_inserter(buf), ", {} ({})", entries[i].first, static_cast<int32_t>(entries[i].second));
      }
      return fmt::to_string(buf);
    }

    template <typename TEnum, typename TArray>
    bool TryParseAsString(TEnum& rEnum, const TArray& entries, const StringViewLite& strOptArg, const TEnum defaultValue)
    {
      // Try to see if we can find a string match
      for (std::size_t i = 0; i < entries.size(); ++i)
      {
        if (strOptArg == entries[i].first)
        {
          rEnum = entries[i].second;
          return true;
        }
      }
      rEnum = defaultValue;
      return false;
    }


    template <typename TEnum, typename TArray>
    bool TryParseAsValue(TEnum& rEnum, const TArray& entries, const StringViewLite& strOptArg, const TEnum defaultValue)
    {
      rEnum = defaultValue;
      // Try to see if we can parse it as a number
      int32_t value = 0;
      try
      {
        StringParseUtil::Parse(value, strOptArg);
      }
      catch (const std::exception&)
      {
        return false;
      }

      for (std::size_t i = 0; i < entries.size(); ++i)
      {
        if (value == static_cast<int>(entries[i].second))
        {
          rEnum = entries[i].second;
          return true;
        }
      }
      return false;
    }

    template <typename TEnum, typename TArray>
    bool TryParse(TEnum& rEnum, const TArray& entries, const StringViewLite& strOptArg, const TEnum defaultValue)
    {
      if (TryParseAsString(rEnum, entries, strOptArg, defaultValue))
      {
        return true;
      }
      return TryParseAsValue(rEnum, entries, strOptArg, defaultValue);
    }


    constexpr std::array<std::pair<const char*, MeshMode>, 3> g_meshModes = {std::make_pair("SingleMesh", MeshMode::SingleMesh),
                                                                             std::make_pair("MultipleMeshes", MeshMode::MultipleMeshes),
                                                                             std::make_pair("Wireframe", MeshMode::Wireframe)};

    constexpr std::array<std::pair<const char*, ShaderMode>, 3> g_shaderMode = {std::make_pair("PerPixelTextured", ShaderMode::PerPixelTextured),
                                                                                std::make_pair("PerPixelNoTextures", ShaderMode::PerPixelNoTextures),
                                                                                std::make_pair("NoLightVertexColor", ShaderMode::NoLightVertexColor)};
  }

  OptionParser::OptionParser()
    : m_scene(0)
  {
  }


  OptionParser::~OptionParser() = default;


  void OptionParser::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    auto meshModes = GetHelpString(g_meshModes);
    auto shaderModes = GetHelpString(g_shaderMode);

    rOptions.emplace_back("s", "Scene", OptionArgument::OptionRequired, CommandId::Scene, "Select the scene to run (0 to 9)");
    rOptions.emplace_back("m", "Model", OptionArgument::OptionRequired, CommandId::Model,
                          "The path to the model file to load (must be relative to the content directory) *Experimental*");
    // rOptions.emplace_back("t", "Texture", OptionArgument::OptionRequired, CommandId::Texture, "The path to the texture file to load");
    rOptions.emplace_back("MeshMode", OptionArgument::OptionRequired, CommandId::MeshMode, fmt::format("Select the mesh mode: {}", meshModes));
    rOptions.emplace_back("ShaderMode", OptionArgument::OptionRequired, CommandId::ShaderMode,
                          fmt::format("Select the shader mode: {}", shaderModes));
    rOptions.emplace_back("Wireframe", OptionArgument::OptionNone, CommandId::Wireframe, "Set the mesh mode to wireframe");
  }


  OptionParseResult OptionParser::OnParse(const int32_t cmdId, const StringViewLite& strOptArg)
  {
    int intValue = 0;

    switch (cmdId)
    {
    case CommandId::Scene:
      if (StringParseUtil::Parse(intValue, strOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_scene = intValue;
      return OptionParseResult::Parsed;
    case CommandId::Model:
      m_customModelPath = strOptArg;
      return OptionParseResult::Parsed;
    case CommandId::MeshMode:
      if (!TryParse(m_meshMode, g_meshModes, strOptArg, MeshMode::SingleMesh))
      {
        FSLLOG3_INFO("Unsupported MeshMode: {}", strOptArg);
        return OptionParseResult::Failed;
      }
      return OptionParseResult::Parsed;
    case CommandId::ShaderMode:
      if (!TryParse(m_shaderMode, g_shaderMode, strOptArg, ShaderMode::PerPixelTextured))
      {
        FSLLOG3_INFO("Unsupported ShaderMode: {}", strOptArg);
        return OptionParseResult::Failed;
      }
      return OptionParseResult::Parsed;
    case CommandId::Wireframe:
      m_meshMode = MeshMode::Wireframe;
      return OptionParseResult::Parsed;
    default:
      return OptionParseResult::NotHandled;
    }
  }

  bool OptionParser::OnParsingComplete()
  {
    return true;
  }
}
