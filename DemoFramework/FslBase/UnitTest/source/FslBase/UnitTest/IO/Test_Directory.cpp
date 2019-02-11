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

#include <FslBase/Exceptions.hpp>
#include <FslBase/IO/Directory.hpp>
#include <FslBase/IO/File.hpp>
#include <FslBase/IO/PathDeque.hpp>
#include <FslBase/Log/IO/LogPath.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBaseContent.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  class TestIO_Directory : public TestFixtureFslBaseContent
  {
  protected:
    IO::Path m_helloWorldFilename;
    IO::Path m_testSubDirectory;
    IO::Path m_testFileFilename;
    IO::Path m_notExistingDirectory;

  public:
    TestIO_Directory()
      : m_helloWorldFilename(IO::Path::Combine(GetContentPath(), "HelloWorld.txt"))
      , m_testSubDirectory(IO::Path::Combine(GetContentPath(), "Test"))
      , m_testFileFilename(IO::Path::Combine(m_testSubDirectory, "TestFile.txt"))
      , m_notExistingDirectory(IO::Path::Combine(GetContentPath(), "ThisIsNotADirectory"))
    {
    }
  };
}


TEST_F(TestIO_Directory, Exists_Content)
{
  const IO::Path contentPath = GetContentPath();
  EXPECT_TRUE(IO::Directory::Exists(contentPath));
  EXPECT_FALSE(IO::File::Exists(contentPath));
  // The test subdirectory should exist
  EXPECT_TRUE(IO::Directory::Exists(m_testSubDirectory));
}


TEST_F(TestIO_Directory, Exists_Content_File)
{
  // This fails as its not a directory
  EXPECT_FALSE(IO::Directory::Exists(m_helloWorldFilename));
  EXPECT_TRUE(IO::File::Exists(m_helloWorldFilename));
  EXPECT_TRUE(IO::File::Exists(m_testFileFilename));
}

TEST_F(TestIO_Directory, NotExistingDirectory)
{
  EXPECT_FALSE(IO::Directory::Exists(m_notExistingDirectory));
  EXPECT_FALSE(IO::File::Exists(m_notExistingDirectory));
}


TEST_F(TestIO_Directory, GetCurrentWorkingDirectory)
{
  EXPECT_NO_THROW(IO::Directory::GetCurrentWorkingDirectory());
}


TEST_F(TestIO_Directory, GetFiles_TopDirectory)
{
  const IO::Path contentPath = GetContentPath();

  ASSERT_FALSE(contentPath.EndsWith('/'));

  IO::PathDeque pathDeque;
  EXPECT_NO_THROW(IO::Directory::GetFiles(pathDeque, contentPath, IO::SearchOptions::TopDirectoryOnly));
  EXPECT_GE(pathDeque.size(), 1u);

  // Check that the entries start with the content path and that the Hello file is found
  IO::Path slashTerminatedContentPath(contentPath.ToUTF8String() + '/');
  uint32_t helloCount = 0;
  for (const auto& entry : pathDeque)
  {
    ASSERT_TRUE(entry->StartsWith(slashTerminatedContentPath));
    if (*entry == m_helloWorldFilename)
    {
      ++helloCount;
    }
    // We expect all entries to be files
    EXPECT_TRUE(IO::File::Exists(*entry));
    ASSERT_LE(slashTerminatedContentPath.GetByteSize(), entry->GetByteSize());
    // Since we search the top directory only there should be no files containing a slash
    EXPECT_LT(entry->IndexOf('/', slashTerminatedContentPath.GetByteSize() + 1), 0);
  }
  EXPECT_EQ(1u, helloCount);
}


TEST_F(TestIO_Directory, GetFiles_AllDirectories)
{
  const IO::Path contentPath = GetContentPath();

  ASSERT_FALSE(contentPath.EndsWith('/'));

  IO::PathDeque pathDeque;
  EXPECT_NO_THROW(IO::Directory::GetFiles(pathDeque, contentPath, IO::SearchOptions::AllDirectories));
  // We know there are at least two files
  EXPECT_GE(pathDeque.size(), 2u);

  // Check that the entries start with the content path and that the Hello file is found
  IO::Path slashTerminatedContentPath(contentPath.ToUTF8String() + '/');
  uint32_t helloCount = 0;
  uint32_t testFileCount = 0;
  for (const auto& entry : pathDeque)
  {
    ASSERT_TRUE(entry->StartsWith(slashTerminatedContentPath));
    if (*entry == m_helloWorldFilename)
    {
      ++helloCount;
    }
    else if (*entry == m_testFileFilename)
    {
      ++testFileCount;
    }
    // We expect all entries to be files
    EXPECT_TRUE(IO::File::Exists(*entry));
  }
  EXPECT_EQ(1u, helloCount);
  EXPECT_EQ(1u, testFileCount);
}


TEST_F(TestIO_Directory, TryGetFiles_TopDirectoryOnly)
{
  const IO::Path contentPath = GetContentPath();

  IO::PathDeque pathDeque;
  EXPECT_TRUE(IO::Directory::TryGetFiles(pathDeque, contentPath, IO::SearchOptions::TopDirectoryOnly));
  EXPECT_GE(pathDeque.size(), 1u);
}


TEST_F(TestIO_Directory, TryGetFiles_AllDirectories)
{
  const IO::Path contentPath = GetContentPath();

  IO::PathDeque pathDeque;
  EXPECT_TRUE(IO::Directory::TryGetFiles(pathDeque, contentPath, IO::SearchOptions::AllDirectories));
  EXPECT_GE(pathDeque.size(), 2u);
}
