/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslBase/Log/Logger1.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestLog_BasicLogger = TestFixtureFslBase;

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


TEST(TestFixtureFslBase_TestLogBasicLog, SetLogLevel)
{
  ScopedLogLevel scope;

  Fsl::LogConfig::SetLogLevel(LogType::Verbose4);
  EXPECT_EQ(Fsl::LogConfig::GetLogLevel(), LogType::Verbose4);
}


TEST(TestFixtureFslBase_TestLogBasicLog, WriteLine_CString_nullptr_Info)
{
  ScopedLogLevel scope;

  Fsl::LogConfig::SetLogLevel(LogType::Verbose4);

  LogLocation location(__FILE__, __FUNCTION__, __LINE__);

  Fsl::Logger::WriteLine(location, LogType::Info, nullptr);
}


TEST(TestFixtureFslBase_TestLogBasicLog, WriteLine_CString_Info)
{
  ScopedLogLevel scope;

  Fsl::LogConfig::SetLogLevel(LogType::Verbose4);

  LogLocation location(__FILE__, __FUNCTION__, __LINE__);

  Fsl::Logger::WriteLine(location, LogType::Info, "hello world");
}


TEST(TestFixtureFslBase_TestLogBasicLog, WriteLine_String_Info)
{
  ScopedLogLevel scope;

  Fsl::LogConfig::SetLogLevel(LogType::Verbose4);

  LogLocation location(__FILE__, __FUNCTION__, __LINE__);

  Fsl::Logger::WriteLine(location, LogType::Info, std::string("hello world"));
}


TEST(TestFixtureFslBase_TestLogBasicLog, WriteLine_CString_nullptr_Warning)
{
  ScopedLogLevel scope;

  Fsl::LogConfig::SetLogLevel(LogType::Verbose4);

  LogLocation location(__FILE__, __FUNCTION__, __LINE__);

  Fsl::Logger::WriteLine(location, LogType::Warning, nullptr);
}


TEST(TestFixtureFslBase_TestLogBasicLog, WriteLine_CString_Warning)
{
  ScopedLogLevel scope;

  Fsl::LogConfig::SetLogLevel(LogType::Verbose4);

  LogLocation location(__FILE__, __FUNCTION__, __LINE__);

  Fsl::Logger::WriteLine(location, LogType::Warning, "hello world");
}


TEST(TestFixtureFslBase_TestLogBasicLog, WriteLine_String_Warning)
{
  ScopedLogLevel scope;

  Fsl::LogConfig::SetLogLevel(LogType::Verbose4);

  LogLocation location(__FILE__, __FUNCTION__, __LINE__);

  Fsl::Logger::WriteLine(location, LogType::Warning, std::string("hello world"));
}


TEST(TestFixtureFslBase_TestLogBasicLog, WriteLine_CString_nullptr_Error)
{
  ScopedLogLevel scope;

  Fsl::LogConfig::SetLogLevel(LogType::Verbose4);

  LogLocation location(__FILE__, __FUNCTION__, __LINE__);

  Fsl::Logger::WriteLine(location, LogType::Error, nullptr);
}


TEST(TestFixtureFslBase_TestLogBasicLog, WriteLine_CString_Error)
{
  ScopedLogLevel scope;

  Fsl::LogConfig::SetLogLevel(LogType::Verbose4);

  LogLocation location(__FILE__, __FUNCTION__, __LINE__);

  Fsl::Logger::WriteLine(location, LogType::Error, "hello world");
}


TEST(TestFixtureFslBase_TestLogBasicLog, WriteLine_String_Error)
{
  ScopedLogLevel scope;

  Fsl::LogConfig::SetLogLevel(LogType::Verbose4);

  LogLocation location(__FILE__, __FUNCTION__, __LINE__);

  Fsl::Logger::WriteLine(location, LogType::Error, std::string("hello world"));
}
