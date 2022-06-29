/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
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
#include <FslBase/Log/IO/LogPath.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBaseContent.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  constexpr uint32_t HelloFileLength = 11u;
  constexpr uint32_t TestFileLength = 8u;


  class TestIO_File : public TestFixtureFslBaseContent
  {
  protected:
    IO::Path m_helloWorldFilename;
    IO::Path m_testFileFilename;
    IO::Path m_notExistingFilename;

  public:
    TestIO_File()
      : m_helloWorldFilename(IO::Path::Combine(GetContentPath(), "HelloWorld.txt"))
      , m_testFileFilename(IO::Path::Combine(GetContentPath(), "Test/TestFile.txt"))
      , m_notExistingFilename(IO::Path::Combine(GetContentPath(), "ThisIsNotAFile.txt"))
    {
    }
  };


  template <std::size_t TSize>
  void Expect_EQ(const std::array<uint8_t, TSize> expectedContent, const std::vector<uint8_t>& content)
  {
    EXPECT_EQ(expectedContent.size(), content.size());
    for (std::size_t i = 0; i < expectedContent.size(); ++i)
    {
      EXPECT_EQ(expectedContent[i], content[i]);
    }
  }

  template <std::size_t TSize>
  void Expect_EQ(const std::array<uint8_t, TSize> expectedContent, const std::array<uint8_t, TSize>& content)
  {
    for (std::size_t i = 0; i < expectedContent.size(); ++i)
    {
      EXPECT_EQ(expectedContent[i], content[i]);
    }
  }
}


TEST_F(TestIO_File, Exists_Content)
{
  const IO::Path contentPath = GetContentPath();
  EXPECT_TRUE(IO::Directory::Exists(contentPath));
  EXPECT_FALSE(IO::File::Exists(contentPath));
}


TEST_F(TestIO_File, Exists_Content_File)
{
  // This fails as its not a directory
  EXPECT_FALSE(IO::Directory::Exists(m_helloWorldFilename));
  EXPECT_TRUE(IO::File::Exists(m_helloWorldFilename));
  EXPECT_TRUE(IO::File::Exists(m_testFileFilename));
}


TEST_F(TestIO_File, GetAttributes_File)
{
  auto fileAttributes = IO::File::GetAttributes(m_helloWorldFilename);
  EXPECT_TRUE(fileAttributes.HasFlag(IO::FileAttributes::File));
  EXPECT_FALSE(fileAttributes.HasFlag(IO::FileAttributes::Directory));
}


TEST_F(TestIO_File, GetAttributes_Directory)
{
  auto fileAttributes = IO::File::GetAttributes(GetContentPath());
  EXPECT_TRUE(fileAttributes.HasFlag(IO::FileAttributes::Directory));
  EXPECT_FALSE(fileAttributes.HasFlag(IO::FileAttributes::File));
}


TEST_F(TestIO_File, GetAttributes_FileDontExist)
{
  EXPECT_THROW(IO::File::GetAttributes(m_notExistingFilename), NotFoundException);
}


TEST_F(TestIO_File, TryGetAttributes_File)
{
  IO::FileAttributes fileAttributes;
  ASSERT_TRUE(IO::File::TryGetAttributes(m_helloWorldFilename, fileAttributes));
  EXPECT_TRUE(fileAttributes.HasFlag(IO::FileAttributes::File));
  EXPECT_FALSE(fileAttributes.HasFlag(IO::FileAttributes::Directory));
}


TEST_F(TestIO_File, TryGetAttributes_Directory)
{
  IO::FileAttributes fileAttributes;
  ASSERT_TRUE(IO::File::TryGetAttributes(GetContentPath(), fileAttributes));
  EXPECT_TRUE(fileAttributes.HasFlag(IO::FileAttributes::Directory));
  EXPECT_FALSE(fileAttributes.HasFlag(IO::FileAttributes::File));
}


TEST_F(TestIO_File, TryGetAttributes_FileDontExist)
{
  IO::FileAttributes fileAttributes;
  ASSERT_FALSE(IO::File::TryGetAttributes(m_notExistingFilename, fileAttributes));
  EXPECT_EQ(0u, fileAttributes.Flags);
}


TEST_F(TestIO_File, GetLength)
{
  EXPECT_EQ(HelloFileLength, IO::File::GetLength(m_helloWorldFilename));
  EXPECT_EQ(TestFileLength, IO::File::GetLength(m_testFileFilename));
}


TEST_F(TestIO_File, ReadAllText)
{
  std::string content = IO::File::ReadAllText(m_helloWorldFilename);
  EXPECT_EQ(std::string("Hello world"), content);
}


TEST_F(TestIO_File, ReadAllText_FileDontExist)
{
  EXPECT_THROW(IO::File::ReadAllText(m_notExistingFilename), IOException);
}


TEST_F(TestIO_File, TryReadAllText)
{
  std::string content;
  ASSERT_TRUE(IO::File::TryReadAllText(content, m_helloWorldFilename));
  EXPECT_EQ(std::string("Hello world"), content);
}


TEST_F(TestIO_File, TryReadAllText_FileDontExist)
{
  std::string content("NotTouched");
  ASSERT_FALSE(IO::File::TryReadAllText(content, m_notExistingFilename));
  EXPECT_EQ(std::string("NotTouched"), content);
}


TEST_F(TestIO_File, ReadAllBytes)
{
  std::array<uint8_t, HelloFileLength> expectedContent = {static_cast<uint8_t>('H'), static_cast<uint8_t>('e'), static_cast<uint8_t>('l'),
                                                          static_cast<uint8_t>('l'), static_cast<uint8_t>('o'), static_cast<uint8_t>(' '),
                                                          static_cast<uint8_t>('w'), static_cast<uint8_t>('o'), static_cast<uint8_t>('r'),
                                                          static_cast<uint8_t>('l'), static_cast<uint8_t>('d')};

  std::vector<uint8_t> content;
  IO::File::ReadAllBytes(content, m_helloWorldFilename);

  EXPECT_EQ(expectedContent.size(), content.size());
  Expect_EQ(expectedContent, content);
}


TEST_F(TestIO_File, ReadAllBytes_FileDontExist)
{
  std::vector<uint8_t> content;
  EXPECT_THROW(IO::File::ReadAllBytes(content, m_notExistingFilename), IOException);
}


TEST_F(TestIO_File, ReadAllBytes_OldSchool)
{
  std::array<uint8_t, HelloFileLength> expectedContent = {static_cast<uint8_t>('H'), static_cast<uint8_t>('e'), static_cast<uint8_t>('l'),
                                                          static_cast<uint8_t>('l'), static_cast<uint8_t>('o'), static_cast<uint8_t>(' '),
                                                          static_cast<uint8_t>('w'), static_cast<uint8_t>('o'), static_cast<uint8_t>('r'),
                                                          static_cast<uint8_t>('l'), static_cast<uint8_t>('d')};

  std::array<uint8_t, HelloFileLength> content{};
  IO::File::ReadAllBytes(content.data(), content.size(), m_helloWorldFilename);

  EXPECT_EQ(expectedContent.size(), content.size());
  Expect_EQ(expectedContent, content);
}

TEST_F(TestIO_File, ReadAllBytes_OldSchool_NullPtr)
{
  std::array<uint8_t, HelloFileLength> content{};
  EXPECT_THROW(IO::File::ReadAllBytes(nullptr, content.size(), m_helloWorldFilename), std::invalid_argument);
}


TEST_F(TestIO_File, ReadAllBytes_OldSchool_TooSmallArray)
{
  std::array<uint8_t, HelloFileLength - 2> content{};
  EXPECT_THROW(IO::File::ReadAllBytes(content.data(), content.size(), m_helloWorldFilename), IOException);
}


TEST_F(TestIO_File, ReadAllBytes_OldSchool_FileDontExist)
{
  std::array<uint8_t, HelloFileLength> content{};
  EXPECT_THROW(IO::File::ReadAllBytes(content.data(), content.size(), m_notExistingFilename), IOException);
}


TEST_F(TestIO_File, ReadBytes_Partial_OldSchool)
{
  constexpr uint32_t SkipBegin = 2;
  constexpr uint32_t SkipEnd = 1;
  constexpr uint32_t SkipAll = SkipBegin + SkipEnd;
  std::array<uint8_t, HelloFileLength - SkipAll> expectedContent = {static_cast<uint8_t>('l'), static_cast<uint8_t>('l'), static_cast<uint8_t>('o'),
                                                                    static_cast<uint8_t>(' '), static_cast<uint8_t>('w'), static_cast<uint8_t>('o'),
                                                                    static_cast<uint8_t>('r'), static_cast<uint8_t>('l')};

  std::array<uint8_t, HelloFileLength - SkipAll> content{};
  IO::File::ReadBytes(content.data(), content.size(), 0, m_helloWorldFilename, SkipBegin, content.size());

  EXPECT_EQ(expectedContent.size(), content.size());
  Expect_EQ(expectedContent, content);
}

TEST_F(TestIO_File, ReadBytes_Partial_OldSchool_NullPtr)
{
  constexpr uint32_t SkipBegin = 2;
  constexpr uint32_t SkipEnd = 1;
  constexpr uint32_t SkipAll = SkipBegin + SkipEnd;
  std::array<uint8_t, HelloFileLength - SkipAll> content{};
  EXPECT_THROW(IO::File::ReadBytes(nullptr, content.size(), 0, m_helloWorldFilename, SkipBegin, content.size()), std::invalid_argument);
}


TEST_F(TestIO_File, ReadBytes_Partial_OldSchool_TooSmallArray)
{
  constexpr uint32_t SkipBegin = 2;
  constexpr uint32_t SkipEnd = 1;
  constexpr uint32_t SkipAll = SkipBegin + SkipEnd;
  std::array<uint8_t, HelloFileLength - SkipAll> content{};

  EXPECT_THROW(IO::File::ReadBytes(content.data(), content.size() - 1, 0, m_helloWorldFilename, SkipBegin, content.size()), std::invalid_argument);
}


TEST_F(TestIO_File, ReadBytes_Partial_OldSchool_FileDontExist)
{
  constexpr uint32_t SkipBegin = 2;
  constexpr uint32_t SkipEnd = 1;
  constexpr uint32_t SkipAll = SkipBegin + SkipEnd;
  std::array<uint8_t, HelloFileLength - SkipAll> content{};

  EXPECT_THROW(IO::File::ReadBytes(content.data(), content.size(), 0, m_notExistingFilename, SkipBegin, content.size()), IOException);
}


TEST_F(TestIO_File, ReadBytes)
{
  std::array<uint8_t, HelloFileLength> expectedContent = {static_cast<uint8_t>('H'), static_cast<uint8_t>('e'), static_cast<uint8_t>('l'),
                                                          static_cast<uint8_t>('l'), static_cast<uint8_t>('o'), static_cast<uint8_t>(' '),
                                                          static_cast<uint8_t>('w'), static_cast<uint8_t>('o'), static_cast<uint8_t>('r'),
                                                          static_cast<uint8_t>('l'), static_cast<uint8_t>('d')};

  std::vector<uint8_t> content = IO::File::ReadBytes(m_helloWorldFilename);

  EXPECT_EQ(expectedContent.size(), content.size());
  Expect_EQ(expectedContent, content);
}


TEST_F(TestIO_File, ReadBytes_FileDontExist)
{
  EXPECT_THROW(IO::File::ReadBytes(m_notExistingFilename), IOException);
}


TEST_F(TestIO_File, ReadBytes_Offset)
{
  std::array<uint8_t, 3> expectedContent = {static_cast<uint8_t>('w'), static_cast<uint8_t>('o'), static_cast<uint8_t>('r')};

  std::vector<uint8_t> content;
  IO::File::ReadBytes(content, m_helloWorldFilename, 6u, 3u);

  EXPECT_EQ(expectedContent.size(), content.size());
  Expect_EQ(expectedContent, content);
}


TEST_F(TestIO_File, ReadBytes_Offset_FileDontExist)
{
  std::vector<uint8_t> content;
  EXPECT_THROW(IO::File::ReadBytes(content, m_notExistingFilename, 6u, 3u), IOException);
}
