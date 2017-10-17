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
        Scene = DEMO_APP_OPTION_BASE,
        DisplacementFactor,
        DisplacementMod,
        FieldOfView,
        MatSpecular,
        MatShininess,
        Model,
        Texture
      };

    };
  }

  OptionParser::OptionParser()
    : m_scene(0)
    , m_displacementFactor(10)
    , m_displacementMod(0)
    , m_fieldOfView(45)
    , m_matSpecular(0.5f, 0.5f, 0.5f)
    , m_matShininess(10.0f)
  {

  }


  OptionParser::~OptionParser()
  {

  }


  void OptionParser::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    rOptions.push_back(Option("s", "Scene", OptionArgument::OptionRequired, CommandId::Scene, "Select the scene to run (0 to 4)"));
    rOptions.push_back(Option("f", "DFactor", OptionArgument::OptionRequired, CommandId::DisplacementFactor, "Set the initial displacement factor"));
    rOptions.push_back(Option("m", "DMod", OptionArgument::OptionRequired, CommandId::DisplacementMod, "Set the initial displacement mod"));
    rOptions.push_back(Option("fov", OptionArgument::OptionRequired, CommandId::FieldOfView, "Set the field of view"));
    rOptions.push_back(Option("MatSpecular", OptionArgument::OptionRequired, CommandId::MatSpecular, "Set the specular color"));
    rOptions.push_back(Option("MatShininess", OptionArgument::OptionRequired, CommandId::MatShininess, "Set the shininess"));
    //rOptions.push_back(Option("m", "Model", OptionArgument::OptionRequired, CommandId::Model, "The path to the model file to load"));
    //rOptions.push_back(Option("t", "Texture", OptionArgument::OptionRequired, CommandId::Texture, "The path to the texture file to load"));
  }


  OptionParseResult::Enum OptionParser::OnParse(const int32_t cmdId, const char*const pszOptArg)
  {
    float floatArrayValuesMax4[4]{};
    float floatValue;
    int intValue;

    switch (cmdId)
    {
    case CommandId::Scene:
      if (StringParseUtil::Parse(intValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_scene = intValue;
      return OptionParseResult::Parsed;
    case CommandId::DisplacementFactor:
      if (StringParseUtil::Parse(floatValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_displacementFactor = floatValue;
      return OptionParseResult::Parsed;
    case CommandId::DisplacementMod:
      if (StringParseUtil::Parse(floatValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_displacementMod = floatValue;
      return OptionParseResult::Parsed;
    case CommandId::FieldOfView:
      if (StringParseUtil::Parse(floatValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_fieldOfView = floatValue;
      return OptionParseResult::Parsed;
    case CommandId::MatSpecular:
    {
      auto res = StringParseUtil::ParseArray(floatArrayValuesMax4, sizeof(floatArrayValuesMax4) / sizeof(float), pszOptArg);
      if ( res.ArrayEntries != 3 )
        return OptionParseResult::Failed;
      m_matSpecular = Vector3(floatArrayValuesMax4[0], floatArrayValuesMax4[1], floatArrayValuesMax4[2]);
      return OptionParseResult::Parsed;
    }
    case CommandId::MatShininess:
      if (StringParseUtil::Parse(floatValue, pszOptArg) <= 0)
        return OptionParseResult::Failed;
      m_matShininess = floatValue;
      return OptionParseResult::Parsed;
    default:
      return OptionParseResult::NotHandled;
    }
  }


  bool OptionParser::OnParsingComplete()
  {
    //if (m_scene < 1)
    //  m_scene = 1;
    return true;
  }

}
