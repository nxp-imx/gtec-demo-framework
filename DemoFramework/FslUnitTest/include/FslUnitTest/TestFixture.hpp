#ifndef FSLUNITTEST_TESTFIXTURE_HPP
#define FSLUNITTEST_TESTFIXTURE_HPP
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

#include <fmt/format.h>
#include <gtest/gtest.h>
#include <cstdlib>


#ifdef _WIN32
#pragma warning(push)
// Disable the warning about unsecure
#pragma warning(disable : 4996)
#endif

class TestFixture : public ::testing::Test
{
  // "$(FSL_GRAPHICS_INTERNAL)/UnitTests/Data"

  std::string m_contentPathBase;
  std::string m_tempPathBase;

public:
  TestFixture()
  {
    const auto rootPath = GetEnvVariable("FSL_GRAPHICS_SDK");
    m_contentPathBase = CombinePath(rootPath, "UnitTests/Data");
    m_tempPathBase = CombinePath(rootPath, "UnitTests/Data/Temp");
  }
  ~TestFixture() override = default;

  std::string GetContentPathBase() const
  {
    return m_contentPathBase;
  }

  std::string GetTempPathBase() const
  {
    return m_tempPathBase;
  }

  std::string GetContentPathBase(const std::string& relativePath) const
  {
    return CombinePath(m_contentPathBase, relativePath);
  }

  std::string GetContentTempPathBase(const std::string& relativePath) const
  {
    return CombinePath(m_contentPathBase, relativePath);
  }

protected:
  static std::string GetEnvVariable(const std::string& envName)
  {
    const auto rootPath = std::getenv(envName.c_str());
    if (rootPath == nullptr)
    {
      throw std::runtime_error(fmt::format("Environment variable '{0}' not found", envName));
    }
    return rootPath;
  }

  std::string CombinePath(const std::string& path1, const std::string& path2) const
  {
    if (IsPathRooted(path2))
    {
      return path2;
    }

    if (path1.empty())
    {
      return path2;
    }
    if (path2.empty())
    {
      return path1;
    }
    if (path1.back() != '/')
    {
      return path1 + '/' + path2;
    }
    return path1 + path2;
  }


  bool IsPathRooted(const std::string& path) const
  {
    // A fairly simple check for rooted paths
    return !path.empty() && (path.front() == '/' || Contains(path, ':'));
  }


  bool Contains(const std::string& path, const char ch) const
  {
    return std::find(path.begin(), path.end(), ch) != path.end();
  }
};

#ifdef _WIN32
#pragma warning(pop)
#endif

#endif
