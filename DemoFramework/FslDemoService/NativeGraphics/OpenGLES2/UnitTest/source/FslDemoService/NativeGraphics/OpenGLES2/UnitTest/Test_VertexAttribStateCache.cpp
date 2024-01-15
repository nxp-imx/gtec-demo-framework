/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslBase/UncheckedNumericCast.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/VertexAttribStateCache.hpp>
#include "UnitTestVertexAttribStateFunctor.hpp"

using namespace Fsl;

namespace
{
  using TestFunctor = GLES2::UnitTestVertexAttribStateFunctor;

  bool ContainsArrayIndex(const ReadOnlySpan<GLES2::GLVertexElementAttribConfig> span, const GLuint attribIndex)
  {
    for (std::size_t i = 0; i < span.size(); ++i)
    {
      if (span[i].AttribIndex == attribIndex)
      {
        return true;
      }
    }
    return false;
  }

  void ValidateState(const std::array<GLES2::VertexAttribState, 32>& initialState, const std::array<GLES2::VertexAttribState, 32>& currentState,
                     const GLES2::VertexElementAttribLinks& attribLinks)
  {
    auto vertexStride = NumericCast<GLint>(attribLinks.VertexStride());
    auto span = attribLinks.AsSpan();
    for (std::size_t i = 0; i < span.size(); ++i)
    {
      const auto& spanEntry = span[i];
      const auto& stateAttribEntry = currentState[spanEntry.AttribIndex];
      EXPECT_TRUE(stateAttribEntry.Enabled);
      EXPECT_EQ(stateAttribEntry.Basic.Size, spanEntry.Size);
      EXPECT_EQ(UncheckedNumericCast<GLenum>(stateAttribEntry.Basic.Type), spanEntry.Type);
      EXPECT_EQ(stateAttribEntry.Basic.Normalized, spanEntry.Normalized != GL_FALSE);
      EXPECT_EQ(stateAttribEntry.Basic.Stride, vertexStride);
      EXPECT_EQ(stateAttribEntry.Basic.Pointer, spanEntry.Pointer);
    }

    // Ensure that the rest of the initial state is left untouched
    for (std::size_t i = 0; i < initialState.size(); ++i)
    {
      if (!ContainsArrayIndex(span, NumericCast<GLuint>(i)))
      {
        const auto& initial = initialState[i];
        const auto& current = currentState[i];
        EXPECT_EQ(current.Enabled, initial.Enabled);
        EXPECT_EQ(current.Basic.Size, initial.Basic.Size);
        EXPECT_EQ(current.Basic.Type, initial.Basic.Type);
        EXPECT_EQ(current.Basic.Normalized, initial.Basic.Normalized);
        EXPECT_EQ(current.Basic.Stride, initial.Basic.Stride);
        EXPECT_EQ(current.Basic.Pointer, initial.Basic.Pointer);
      }
    }
  }

  void ValidateState(const std::array<GLES2::VertexAttribState, 32>& initialState, const std::array<GLES2::VertexAttribState, 32>& currentState)
  {
    // Ensure that the rest of the initial state is left untouched
    for (std::size_t i = 0; i < initialState.size(); ++i)
    {
      const auto& initial = initialState[i];
      const auto& current = currentState[i];
      EXPECT_EQ(current.Enabled, initial.Enabled);
      EXPECT_EQ(current.Basic.Size, initial.Basic.Size);
      EXPECT_EQ(current.Basic.Type, initial.Basic.Type);
      EXPECT_EQ(current.Basic.Normalized, initial.Basic.Normalized);
      EXPECT_EQ(current.Basic.Stride, initial.Basic.Stride);
      EXPECT_EQ(current.Basic.Pointer, initial.Basic.Pointer);
    }
  }

  class Test_VertexAttribStateCache : public TestFixtureFslBase
  {
  protected:
    std::array<GLES2::VertexAttribState, 32> m_initialState;

  public:
    Test_VertexAttribStateCache()
    {
      TestFunctor::Populate();
      m_initialState = TestFunctor::GlobalState;
    }

    ~Test_VertexAttribStateCache() override
    {
      ValidateState(m_initialState, TestFunctor::GlobalState);
    }
  };


  GLES2::VertexElementAttribLinks CreateAttribLinksWithThreeEntries()
  {
    std::array<GLES2::GLVertexElementAttribConfig, 3> vertexElementAttribConfigs = {
      GLES2::GLVertexElementAttribConfig(10, 1, GL_FLOAT, GL_TRUE, nullptr),
      GLES2::GLVertexElementAttribConfig(11, 2, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(110)),
      GLES2::GLVertexElementAttribConfig(12, 3, GL_FLOAT, GL_TRUE, reinterpret_cast<const GLvoid*>(120)),
    };
    const uint32_t vertexStride = (1 + 2 + 3) * 4;

    return {ReadOnlySpanUtil::AsSpan(vertexElementAttribConfigs), vertexStride};
  }


  // update first, greater, greater
  GLES2::VertexElementAttribLinks CreateAttribLinksWithThreeEntriesUpdateFirst_GG()
  {
    std::array<GLES2::GLVertexElementAttribConfig, 3> vertexElementAttribConfigs = {
      GLES2::GLVertexElementAttribConfig(10, 4, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(210)),
      GLES2::GLVertexElementAttribConfig(21, 5, GL_FLOAT, GL_FALSE, nullptr),
      GLES2::GLVertexElementAttribConfig(22, 6, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(220)),
    };
    const uint32_t vertexStride = (1 + 2 + 3) * 4;

    return {ReadOnlySpanUtil::AsSpan(vertexElementAttribConfigs), vertexStride};
  }

  // update second, greater, greater
  GLES2::VertexElementAttribLinks CreateAttribLinksWithThreeEntriesUpdateSecond_GG()
  {
    std::array<GLES2::GLVertexElementAttribConfig, 3> vertexElementAttribConfigs = {
      GLES2::GLVertexElementAttribConfig(11, 4, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(210)),
      GLES2::GLVertexElementAttribConfig(21, 5, GL_FLOAT, GL_FALSE, nullptr),
      GLES2::GLVertexElementAttribConfig(22, 6, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(220)),
    };
    const uint32_t vertexStride = (1 + 2 + 3) * 4;

    return {ReadOnlySpanUtil::AsSpan(vertexElementAttribConfigs), vertexStride};
  }

  // update third, greater, greater
  GLES2::VertexElementAttribLinks CreateAttribLinksWithThreeEntriesUpdateThird_GG()
  {
    std::array<GLES2::GLVertexElementAttribConfig, 3> vertexElementAttribConfigs = {
      GLES2::GLVertexElementAttribConfig(12, 4, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(210)),
      GLES2::GLVertexElementAttribConfig(21, 5, GL_FLOAT, GL_FALSE, nullptr),
      GLES2::GLVertexElementAttribConfig(22, 6, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(220)),
    };
    const uint32_t vertexStride = (1 + 2 + 3) * 4;

    return {ReadOnlySpanUtil::AsSpan(vertexElementAttribConfigs), vertexStride};
  }

  // update first, less-than, less-than
  GLES2::VertexElementAttribLinks CreateAttribLinksWithThreeEntriesUpdateFirst_LL()
  {
    std::array<GLES2::GLVertexElementAttribConfig, 3> vertexElementAttribConfigs = {
      GLES2::GLVertexElementAttribConfig(4, 4, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(210)),
      GLES2::GLVertexElementAttribConfig(5, 5, GL_FLOAT, GL_FALSE, nullptr),
      GLES2::GLVertexElementAttribConfig(10, 6, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(220)),
    };
    const uint32_t vertexStride = (1 + 2 + 3) * 4;

    return {ReadOnlySpanUtil::AsSpan(vertexElementAttribConfigs), vertexStride};
  }

  // update second, less-than, less-than
  GLES2::VertexElementAttribLinks CreateAttribLinksWithThreeEntriesUpdateSecond_LL()
  {
    std::array<GLES2::GLVertexElementAttribConfig, 3> vertexElementAttribConfigs = {
      GLES2::GLVertexElementAttribConfig(4, 4, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(210)),
      GLES2::GLVertexElementAttribConfig(5, 5, GL_FLOAT, GL_FALSE, nullptr),
      GLES2::GLVertexElementAttribConfig(11, 6, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(220)),
    };
    const uint32_t vertexStride = (1 + 2 + 3) * 4;

    return {ReadOnlySpanUtil::AsSpan(vertexElementAttribConfigs), vertexStride};
  }

  // update third, less-than, less-than
  GLES2::VertexElementAttribLinks CreateAttribLinksWithThreeEntriesUpdateThird_LL()
  {
    std::array<GLES2::GLVertexElementAttribConfig, 3> vertexElementAttribConfigs = {
      GLES2::GLVertexElementAttribConfig(4, 4, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(210)),
      GLES2::GLVertexElementAttribConfig(5, 5, GL_FLOAT, GL_FALSE, nullptr),
      GLES2::GLVertexElementAttribConfig(12, 6, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(220)),
    };
    const uint32_t vertexStride = (1 + 2 + 3) * 4;

    return {ReadOnlySpanUtil::AsSpan(vertexElementAttribConfigs), vertexStride};
  }

  // update first, less-than, greater-than
  GLES2::VertexElementAttribLinks CreateAttribLinksWithThreeEntriesUpdateFirst_LG()
  {
    std::array<GLES2::GLVertexElementAttribConfig, 3> vertexElementAttribConfigs = {
      GLES2::GLVertexElementAttribConfig(4, 4, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(210)),
      GLES2::GLVertexElementAttribConfig(10, 5, GL_FLOAT, GL_FALSE, nullptr),
      GLES2::GLVertexElementAttribConfig(20, 6, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(220)),
    };
    const uint32_t vertexStride = (1 + 2 + 3) * 4;

    return {ReadOnlySpanUtil::AsSpan(vertexElementAttribConfigs), vertexStride};
  }

  // update second, less-than, greater-than
  GLES2::VertexElementAttribLinks CreateAttribLinksWithThreeEntriesUpdateSecond_LG()
  {
    std::array<GLES2::GLVertexElementAttribConfig, 3> vertexElementAttribConfigs = {
      GLES2::GLVertexElementAttribConfig(4, 4, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(210)),
      GLES2::GLVertexElementAttribConfig(11, 5, GL_FLOAT, GL_FALSE, nullptr),
      GLES2::GLVertexElementAttribConfig(20, 6, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(220)),
    };
    const uint32_t vertexStride = (1 + 2 + 3) * 4;

    return {ReadOnlySpanUtil::AsSpan(vertexElementAttribConfigs), vertexStride};
  }

  // update third, less-than, greater-than
  GLES2::VertexElementAttribLinks CreateAttribLinksWithThreeEntriesUpdateThird_LG()
  {
    std::array<GLES2::GLVertexElementAttribConfig, 3> vertexElementAttribConfigs = {
      GLES2::GLVertexElementAttribConfig(4, 4, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(210)),
      GLES2::GLVertexElementAttribConfig(12, 5, GL_FLOAT, GL_FALSE, nullptr),
      GLES2::GLVertexElementAttribConfig(20, 6, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(220)),
    };
    const uint32_t vertexStride = (1 + 2 + 3) * 4;

    return {ReadOnlySpanUtil::AsSpan(vertexElementAttribConfigs), vertexStride};
  }


  GLES2::VertexElementAttribLinks CreateAttribLinksWithThreeEntriesNoMatch()
  {
    std::array<GLES2::GLVertexElementAttribConfig, 3> vertexElementAttribConfigs = {
      GLES2::GLVertexElementAttribConfig(20, 4, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(210)),
      GLES2::GLVertexElementAttribConfig(21, 5, GL_FLOAT, GL_FALSE, nullptr),
      GLES2::GLVertexElementAttribConfig(22, 6, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(220)),
    };
    const uint32_t vertexStride = (1 + 2 + 3) * 4;

    return {ReadOnlySpanUtil::AsSpan(vertexElementAttribConfigs), vertexStride};
  }
}


TEST_F(Test_VertexAttribStateCache, Construct)
{
  GLES2::VertexAttribStateCache<TestFunctor> cache;
}


TEST_F(Test_VertexAttribStateCache, ConstructThenMove)
{
  GLES2::VertexAttribStateCache<TestFunctor> cache;
  GLES2::VertexAttribStateCache<TestFunctor> cache2 = std::move(cache);
}


TEST_F(Test_VertexAttribStateCache, ConstructThenMove2)
{
  GLES2::VertexAttribStateCache<TestFunctor> cache;
  GLES2::VertexAttribStateCache<TestFunctor> cache2;
  cache2 = std::move(cache);
}


TEST_F(Test_VertexAttribStateCache, ConstructFillWithOne)
{
  GLES2::VertexAttribStateCache<TestFunctor> cache;
  const auto links0 = CreateAttribLinksWithThreeEntries();
  cache.ChangeAttribs(links0);

  ValidateState(m_initialState, TestFunctor::GlobalState, links0);
}

TEST_F(Test_VertexAttribStateCache, ConstructFillWithSame)
{
  GLES2::VertexAttribStateCache<TestFunctor> cache;
  const auto links0 = CreateAttribLinksWithThreeEntries();

  cache.ChangeAttribs(links0);
  ValidateState(m_initialState, TestFunctor::GlobalState, links0);
  // Set same links again
  cache.ChangeAttribs(links0);
  ValidateState(m_initialState, TestFunctor::GlobalState, links0);
}

TEST_F(Test_VertexAttribStateCache, SwitchBetweenTwo_NoMatch)
{
  GLES2::VertexAttribStateCache<TestFunctor> cache;
  const auto links0 = CreateAttribLinksWithThreeEntries();
  const auto links1 = CreateAttribLinksWithThreeEntriesNoMatch();
  cache.ChangeAttribs(links0);
  ValidateState(m_initialState, TestFunctor::GlobalState, links0);
  cache.ChangeAttribs(links1);
  ValidateState(m_initialState, TestFunctor::GlobalState, links1);
}

TEST_F(Test_VertexAttribStateCache, SwitchBetweenTwo_UpdateFirstGG)
{
  GLES2::VertexAttribStateCache<TestFunctor> cache;
  const auto links0 = CreateAttribLinksWithThreeEntries();
  const auto links1 = CreateAttribLinksWithThreeEntriesUpdateFirst_GG();
  cache.ChangeAttribs(links0);
  ValidateState(m_initialState, TestFunctor::GlobalState, links0);
  cache.ChangeAttribs(links1);
  ValidateState(m_initialState, TestFunctor::GlobalState, links1);
}

TEST_F(Test_VertexAttribStateCache, SwitchBetweenTwo_UpdateSecondGG)
{
  GLES2::VertexAttribStateCache<TestFunctor> cache;
  const auto links0 = CreateAttribLinksWithThreeEntries();
  const auto links1 = CreateAttribLinksWithThreeEntriesUpdateSecond_GG();
  cache.ChangeAttribs(links0);
  ValidateState(m_initialState, TestFunctor::GlobalState, links0);
  cache.ChangeAttribs(links1);
  ValidateState(m_initialState, TestFunctor::GlobalState, links1);
}

TEST_F(Test_VertexAttribStateCache, SwitchBetweenTwo_UpdateThirdGG)
{
  GLES2::VertexAttribStateCache<TestFunctor> cache;
  const auto links0 = CreateAttribLinksWithThreeEntries();
  const auto links1 = CreateAttribLinksWithThreeEntriesUpdateThird_GG();
  cache.ChangeAttribs(links0);
  ValidateState(m_initialState, TestFunctor::GlobalState, links0);
  cache.ChangeAttribs(links1);
  ValidateState(m_initialState, TestFunctor::GlobalState, links1);
}

TEST_F(Test_VertexAttribStateCache, SwitchBetweenTwo_UpdateFirstLL)
{
  GLES2::VertexAttribStateCache<TestFunctor> cache;
  const auto links0 = CreateAttribLinksWithThreeEntries();
  const auto links1 = CreateAttribLinksWithThreeEntriesUpdateFirst_LL();
  cache.ChangeAttribs(links0);
  ValidateState(m_initialState, TestFunctor::GlobalState, links0);
  cache.ChangeAttribs(links1);
  ValidateState(m_initialState, TestFunctor::GlobalState, links1);
}

TEST_F(Test_VertexAttribStateCache, SwitchBetweenTwo_UpdateSecondLL)
{
  GLES2::VertexAttribStateCache<TestFunctor> cache;
  const auto links0 = CreateAttribLinksWithThreeEntries();
  const auto links1 = CreateAttribLinksWithThreeEntriesUpdateSecond_LL();
  cache.ChangeAttribs(links0);
  ValidateState(m_initialState, TestFunctor::GlobalState, links0);
  cache.ChangeAttribs(links1);
  ValidateState(m_initialState, TestFunctor::GlobalState, links1);
}

TEST_F(Test_VertexAttribStateCache, SwitchBetweenTwo_UpdateThirdLL)
{
  GLES2::VertexAttribStateCache<TestFunctor> cache;
  const auto links0 = CreateAttribLinksWithThreeEntries();
  const auto links1 = CreateAttribLinksWithThreeEntriesUpdateThird_LL();
  cache.ChangeAttribs(links0);
  ValidateState(m_initialState, TestFunctor::GlobalState, links0);
  cache.ChangeAttribs(links1);
  ValidateState(m_initialState, TestFunctor::GlobalState, links1);
}

TEST_F(Test_VertexAttribStateCache, SwitchBetweenTwo_UpdateFirstLG)
{
  GLES2::VertexAttribStateCache<TestFunctor> cache;
  const auto links0 = CreateAttribLinksWithThreeEntries();
  const auto links1 = CreateAttribLinksWithThreeEntriesUpdateFirst_LG();
  cache.ChangeAttribs(links0);
  ValidateState(m_initialState, TestFunctor::GlobalState, links0);
  cache.ChangeAttribs(links1);
  ValidateState(m_initialState, TestFunctor::GlobalState, links1);
}

TEST_F(Test_VertexAttribStateCache, SwitchBetweenTwo_UpdateSecondLG)
{
  GLES2::VertexAttribStateCache<TestFunctor> cache;
  const auto links0 = CreateAttribLinksWithThreeEntries();
  const auto links1 = CreateAttribLinksWithThreeEntriesUpdateSecond_LG();
  cache.ChangeAttribs(links0);
  ValidateState(m_initialState, TestFunctor::GlobalState, links0);
  cache.ChangeAttribs(links1);
  ValidateState(m_initialState, TestFunctor::GlobalState, links1);
}

TEST_F(Test_VertexAttribStateCache, SwitchBetweenTwo_UpdateThirdLG)
{
  GLES2::VertexAttribStateCache<TestFunctor> cache;
  const auto links0 = CreateAttribLinksWithThreeEntries();
  const auto links1 = CreateAttribLinksWithThreeEntriesUpdateThird_LG();
  cache.ChangeAttribs(links0);
  ValidateState(m_initialState, TestFunctor::GlobalState, links0);
  cache.ChangeAttribs(links1);
  ValidateState(m_initialState, TestFunctor::GlobalState, links1);
}
