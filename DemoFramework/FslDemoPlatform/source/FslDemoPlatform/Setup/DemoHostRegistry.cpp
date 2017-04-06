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
#include <FslDemoPlatform/Setup/DemoHostRegistry.hpp>
#include <FslDemoApp/Host/DemoHostFeature.hpp>
#include <algorithm>
#include <sstream>

namespace Fsl
{
  namespace
  {
    bool CanSupport(const std::deque<DemoHostFeatureName::Enum>& availableFeatures, const std::deque<DemoHostFeature>& requiredFeatures)
    {
      std::deque<DemoHostFeature>::const_iterator itr = requiredFeatures.begin();
      while (itr != requiredFeatures.end())
      {
        if (std::find(availableFeatures.begin(), availableFeatures.end(), itr->Name) == availableFeatures.end())
          return false;
        ++itr;
      }
      return true;
    }
  }


  DemoHostRegistry::DemoHostRegistry()
    : m_records()
  {
  }


  DemoHostSetup DemoHostRegistry::GetSetup(const std::deque<DemoHostFeature>& features) const
  {
    // Try to locate a host that supports all the required features
    {
      std::deque<Record>::const_iterator itr = m_records.begin();
      while (itr != m_records.end())
      {
        if (CanSupport(itr->Features, features))
          return itr->HostSetup;
        ++itr;
      }
    }

    // None exists, so lets output a nice useful error message.
    {
      std::stringstream errorMessage;
      errorMessage << "There is no host available that supports: ";
      std::deque<DemoHostFeature>::const_iterator itr = features.begin();
      while (itr != features.end())
      {
        errorMessage << DemoHostFeatureName::ToString(itr->Name);
        ++itr;
        if (itr != features.end())
          errorMessage << ", ";
      }
      throw NotSupportedException(errorMessage.str());
    }
  }


  void DemoHostRegistry::Register(const DemoHostFeatureName::Enum feature, const DemoHostSetup& hostSetup)
  {
    m_records.push_back(Record(feature, hostSetup));
  }


  void DemoHostRegistry::Register(const std::deque<DemoHostFeatureName::Enum>& features, const DemoHostSetup& hostSetup)
  {
    m_records.push_back(Record(features, hostSetup));
  }


  DemoHostRegistry::Record::Record(const DemoHostFeatureName::Enum feature, const DemoHostSetup& hostSetup)
    : HostSetup(hostSetup)
    , Features()
  {
    Features.push_back(feature);
  }


  DemoHostRegistry::Record::Record(const std::deque<DemoHostFeatureName::Enum>& features, const DemoHostSetup& hostSetup)
    : HostSetup(hostSetup)
    , Features(features)
  {
  }


}
