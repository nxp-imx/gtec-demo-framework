/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <Shared/ObjectSelection/OptionParser.hpp>
#include <FslBase/BasicTypes.hpp>
#include <FslBase/Log/Log.hpp>
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
        NearPlaneMouse = DEMO_APP_OPTION_BASE,
        FarPlaneMouse,
        OrientedBoundingBox,
        AxisAlignedBoundingBox,
        RandomSeed,
        Objects,
      };
    };
  }

  OptionParser::OptionParser()
    : m_nearPlaneMouseEnabled(false)
    , m_farPlaneMouseEnabled(false)
    , m_orientedBoundingBoxEnabled(true)
    , m_axisAlignedBoundingBoxEnabled(true)
    , m_randomSeedEnabled(false)
    , m_objectCount(32)
  {
  }


  OptionParser::~OptionParser() = default;


  void OptionParser::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.emplace_back("NearPlaneMouse", OptionArgument::OptionRequired, CommandId::NearPlaneMouse,
                          "Draw the near plane mouse intersection on/off");
    rOptions.emplace_back("FarPlaneMouse", OptionArgument::OptionRequired, CommandId::FarPlaneMouse, "Draw the far plane mouse intersection on/off");
    rOptions.emplace_back("OBB", OptionArgument::OptionRequired, CommandId::OrientedBoundingBox, "Draw the Oriented Bounding Boxes on/off");
    rOptions.emplace_back("AABB", OptionArgument::OptionRequired, CommandId::AxisAlignedBoundingBox, "Draw the Axis Aligned Bounding Boxes on/off");
    rOptions.emplace_back("RandomSeed", OptionArgument::OptionNone, CommandId::RandomSeed,
                          "Use a random seed for the scene generation instead of a fixed one");
    rOptions.emplace_back("Objects", OptionArgument::OptionRequired, CommandId::Objects, "Specify the number of objects to generate");
  }


  OptionParseResult::Enum OptionParser::OnParse(const int32_t cmdId, const char* const pszOptArg)
  {
    uint32_t uintValue;
    bool boolValue;

    switch (cmdId)
    {
    case CommandId::NearPlaneMouse:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_nearPlaneMouseEnabled = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::FarPlaneMouse:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_farPlaneMouseEnabled = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::OrientedBoundingBox:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_orientedBoundingBoxEnabled = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::AxisAlignedBoundingBox:
      if (StringParseUtil::Parse(boolValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_axisAlignedBoundingBoxEnabled = boolValue;
      return OptionParseResult::Parsed;
    case CommandId::RandomSeed:
      m_randomSeedEnabled = true;
      return OptionParseResult::Parsed;
    case CommandId::Objects:
      if (StringParseUtil::Parse(uintValue, pszOptArg) <= 0)
      {
        return OptionParseResult::Failed;
      }
      m_objectCount = std::max(uintValue, 1u);
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
