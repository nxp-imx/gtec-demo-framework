/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestLog_Log3Fmt = TestFixtureFslBase;

  // This class ensures that each test resets the global log level
  class ScopedLogLevel
  {
    LogType m_logLevel;

  public:
    ScopedLogLevel()
      : m_logLevel(Fsl::LogConfig::GetLogLevel())
    {
    }

    ~ScopedLogLevel()
    {
      Fsl::LogConfig::SetLogLevel(m_logLevel);
    }
  };
}


TEST(TestLog_Log3Fmt, Verbose5_ArgCString)
{
  ScopedLogLevel scope;

  Fsl::LogConfig::SetLogLevel(LogType::Verbose5);

  FSLLOG3_VERBOSE5("Hey {}", "hey");
}

TEST(TestLog_Log3Fmt, Verbose4_ArgCString)
{
  ScopedLogLevel scope;
  Fsl::LogConfig::SetLogLevel(LogType::Verbose5);

  FSLLOG3_VERBOSE4("Hey {}", "hey");
}

TEST(TestLog_Log3Fmt, Verbose3_ArgCString)
{
  ScopedLogLevel scope;
  Fsl::LogConfig::SetLogLevel(LogType::Verbose5);

  FSLLOG3_VERBOSE3("Hey {}", "hey");
}

TEST(TestLog_Log3Fmt, Verbose2_ArgCString)
{
  ScopedLogLevel scope;
  Fsl::LogConfig::SetLogLevel(LogType::Verbose5);

  FSLLOG3_VERBOSE2("Hey {}", "hey");
}

TEST(TestLog_Log3Fmt, Verbose1_ArgCString)
{
  ScopedLogLevel scope;
  Fsl::LogConfig::SetLogLevel(LogType::Verbose5);

  FSLLOG3_VERBOSE("Hey {}", "hey");
}

TEST(TestLog_Log3Fmt, Info_ArgCString)
{
  ScopedLogLevel scope;
  Fsl::LogConfig::SetLogLevel(LogType::Verbose5);

  FSLLOG3_INFO("Hey {}", "hey");
}

TEST(TestLog_Log3Fmt, Warning_ArgCString)
{
  ScopedLogLevel scope;
  Fsl::LogConfig::SetLogLevel(LogType::Verbose5);

  FSLLOG3_WARNING("Hey {}", "hey");
}

TEST(TestLog_Log3Fmt, Error_ArgCString)
{
  ScopedLogLevel scope;
  Fsl::LogConfig::SetLogLevel(LogType::Verbose5);

  FSLLOG3_ERROR("Hey {}", "hey");
}


TEST(TestLog_Log3Fmt, Verbose5_ArgString)
{
  ScopedLogLevel scope;

  Fsl::LogConfig::SetLogLevel(LogType::Verbose5);

  FSLLOG3_VERBOSE5("Hey {}", std::string("hey"));
}

TEST(TestLog_Log3Fmt, Verbose4_ArgString)
{
  ScopedLogLevel scope;
  Fsl::LogConfig::SetLogLevel(LogType::Verbose5);

  FSLLOG3_VERBOSE4("Hey {}", std::string("hey"));
}

TEST(TestLog_Log3Fmt, Verbose3_ArgString)
{
  ScopedLogLevel scope;
  Fsl::LogConfig::SetLogLevel(LogType::Verbose5);

  FSLLOG3_VERBOSE3("Hey {}", std::string("hey"));
}

TEST(TestLog_Log3Fmt, Verbose2_ArgString)
{
  ScopedLogLevel scope;
  Fsl::LogConfig::SetLogLevel(LogType::Verbose5);

  FSLLOG3_VERBOSE2("Hey {}", std::string("hey"));
}

TEST(TestLog_Log3Fmt, Verbose1_ArgString)
{
  ScopedLogLevel scope;
  Fsl::LogConfig::SetLogLevel(LogType::Verbose5);

  FSLLOG3_VERBOSE("Hey {}", std::string("hey"));
}

TEST(TestLog_Log3Fmt, Info_ArgString)
{
  ScopedLogLevel scope;
  Fsl::LogConfig::SetLogLevel(LogType::Verbose5);

  FSLLOG3_INFO("Hey {}", std::string("hey"));
}

TEST(TestLog_Log3Fmt, Warning_ArgString)
{
  ScopedLogLevel scope;
  Fsl::LogConfig::SetLogLevel(LogType::Verbose5);

  FSLLOG3_WARNING("Hey {}", std::string("hey"));
}

TEST(TestLog_Log3Fmt, Error_ArgString)
{
  ScopedLogLevel scope;
  Fsl::LogConfig::SetLogLevel(LogType::Verbose5);

  FSLLOG3_ERROR("Hey {}", std::string("hey"));
}
