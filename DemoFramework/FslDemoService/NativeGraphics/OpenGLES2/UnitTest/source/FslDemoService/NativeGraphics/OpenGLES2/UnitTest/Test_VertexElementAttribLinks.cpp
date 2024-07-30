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

#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/VertexElementAttribLinks.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>
#include <FslGraphics/Vertices/VertexElement.hpp>
#include <FslUtil/OpenGLES2/Log/LogGLVertexElementAttribConfig.hpp>
#include <array>

using namespace Fsl;

namespace
{
  using Test_VertexElementAttribLinks = TestFixtureFslBase;
}


TEST(Test_VertexElementAttribLinks, Construct_Empty1)
{
  GLES2::VertexElementAttribLinks empty;
  EXPECT_TRUE(empty.AsSpan().empty());
  EXPECT_EQ(empty.VertexStride(), 0u);
}


TEST(Test_VertexElementAttribLinks, Construct_FromEmptyVertexDeclaration)
{
  VertexDeclarationSpan vertexDeclaration;
  ReadOnlySpan<GLES2::GLVertexAttribLink> attribLinks;

  GLES2::VertexElementAttribLinks empty(vertexDeclaration, attribLinks);
  EXPECT_TRUE(empty.AsSpan().empty());
  EXPECT_EQ(empty.VertexStride(), vertexDeclaration.VertexStride());
}


TEST(Test_VertexElementAttribLinks, Construct_FromEmptySpan)
{
  const ReadOnlySpan<GLES2::GLVertexElementAttribConfig> vertexElementAttribConfigs;
  const uint32_t vertexStride = 0;

  GLES2::VertexElementAttribLinks empty(vertexElementAttribConfigs, vertexStride);
  EXPECT_TRUE(empty.AsSpan().empty());
  EXPECT_EQ(empty.VertexStride(), vertexStride);
}

TEST(Test_VertexElementAttribLinks, Construct_FromVertexDeclaration_InOrder)
{
  std::array<VertexElement, 3> vertexElements = {
    VertexElement(0 * 4, VertexElementFormat::Single, VertexElementUsage::PointSize, 1),
    VertexElement(1 * 4, VertexElementFormat::Single, VertexElementUsage::TextureCoordinate, 2),
    VertexElement((1 + 2) * 4, VertexElementFormat::Single, VertexElementUsage::Position, 3),
  };
  const uint32_t vertexStride = (1 + 2 + 3) * 4;

  VertexDeclarationSpan vertexDeclaration(SpanUtil::AsReadOnlySpan(vertexElements), vertexStride);

  std::array<GLES2::GLVertexAttribLink, 3> attribLinks = {
    GLES2::GLVertexAttribLink(31, 0),
    GLES2::GLVertexAttribLink(32, 1),
    GLES2::GLVertexAttribLink(33, 2),
  };

  GLES2::VertexElementAttribLinks links(vertexDeclaration, SpanUtil::AsReadOnlySpan(attribLinks));

  const auto span = links.AsSpan();
  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.size(), 3u);
  EXPECT_EQ(links.VertexStride(), vertexStride);
  EXPECT_EQ(span[0], GLES2::GLVertexElementAttribConfig(31, 1, GL_FLOAT, GL_FALSE,
                                                        reinterpret_cast<const GLvoid*>(NumericCast<uintptr_t>(vertexElements[0].Offset))));
  EXPECT_EQ(span[1], GLES2::GLVertexElementAttribConfig(32, 1, GL_FLOAT, GL_FALSE,
                                                        reinterpret_cast<const GLvoid*>(NumericCast<uintptr_t>(vertexElements[1].Offset))));
  EXPECT_EQ(span[2], GLES2::GLVertexElementAttribConfig(33, 1, GL_FLOAT, GL_FALSE,
                                                        reinterpret_cast<const GLvoid*>(NumericCast<uintptr_t>(vertexElements[2].Offset))));
}

TEST(Test_VertexElementAttribLinks, Construct_FromVertexDeclaration_OutOfOrder)
{
  std::array<VertexElement, 3> vertexElements = {
    VertexElement(0 * 4, VertexElementFormat::Single, VertexElementUsage::PointSize, 1),
    VertexElement(1 * 4, VertexElementFormat::Single, VertexElementUsage::TextureCoordinate, 2),
    VertexElement((1 + 2) * 4, VertexElementFormat::Single, VertexElementUsage::Position, 3),
  };
  const uint32_t vertexStride = (1 + 2 + 3) * 4;

  VertexDeclarationSpan vertexDeclaration(SpanUtil::AsReadOnlySpan(vertexElements), vertexStride);

  std::array<GLES2::GLVertexAttribLink, 3> attribLinks = {
    GLES2::GLVertexAttribLink(33, 2),
    GLES2::GLVertexAttribLink(32, 1),
    GLES2::GLVertexAttribLink(31, 0),
  };

  GLES2::VertexElementAttribLinks links(vertexDeclaration, SpanUtil::AsReadOnlySpan(attribLinks));

  const auto span = links.AsSpan();
  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.size(), 3u);
  EXPECT_EQ(links.VertexStride(), vertexStride);
  EXPECT_EQ(span[0], GLES2::GLVertexElementAttribConfig(31, 1, GL_FLOAT, GL_FALSE,
                                                        reinterpret_cast<const GLvoid*>(NumericCast<uintptr_t>(vertexElements[0].Offset))));
  EXPECT_EQ(span[1], GLES2::GLVertexElementAttribConfig(32, 1, GL_FLOAT, GL_FALSE,
                                                        reinterpret_cast<const GLvoid*>(NumericCast<uintptr_t>(vertexElements[1].Offset))));
  EXPECT_EQ(span[2], GLES2::GLVertexElementAttribConfig(33, 1, GL_FLOAT, GL_FALSE,
                                                        reinterpret_cast<const GLvoid*>(NumericCast<uintptr_t>(vertexElements[2].Offset))));
}

TEST(Test_VertexElementAttribLinks, Construct_FromSpan_InOrder)
{
  std::array<GLES2::GLVertexElementAttribConfig, 3> vertexElementAttribConfigs = {
    GLES2::GLVertexElementAttribConfig(31, 1, GL_FLOAT, GL_TRUE, nullptr),
    GLES2::GLVertexElementAttribConfig(32, 2, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(110)),
    GLES2::GLVertexElementAttribConfig(33, 3, GL_FLOAT, GL_TRUE, reinterpret_cast<const GLvoid*>(120)),
  };
  const uint32_t vertexStride = (1 + 2 + 3) * 4;

  GLES2::VertexElementAttribLinks links(SpanUtil::AsReadOnlySpan(vertexElementAttribConfigs), vertexStride);
  const auto span = links.AsSpan();
  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.size(), vertexElementAttribConfigs.size());
  EXPECT_EQ(links.VertexStride(), vertexStride);
  EXPECT_EQ(span[0], vertexElementAttribConfigs[0]);
  EXPECT_EQ(span[1], vertexElementAttribConfigs[1]);
  EXPECT_EQ(span[2], vertexElementAttribConfigs[2]);
}


TEST(Test_VertexElementAttribLinks, Construct_FromSpan_OutOfOrder)
{
  std::array<GLES2::GLVertexElementAttribConfig, 3> vertexElementAttribConfigs = {
    GLES2::GLVertexElementAttribConfig(33, 1, GL_FLOAT, GL_TRUE, nullptr),
    GLES2::GLVertexElementAttribConfig(32, 2, GL_FLOAT, GL_FALSE, reinterpret_cast<const GLvoid*>(110)),
    GLES2::GLVertexElementAttribConfig(31, 3, GL_FLOAT, GL_TRUE, reinterpret_cast<const GLvoid*>(120)),
  };
  const uint32_t vertexStride = (1 + 2 + 3) * 4;

  GLES2::VertexElementAttribLinks links(SpanUtil::AsReadOnlySpan(vertexElementAttribConfigs), vertexStride);
  const auto span = links.AsSpan();
  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.size(), vertexElementAttribConfigs.size());
  EXPECT_EQ(links.VertexStride(), vertexStride);
  // We expect that entries in VertexElementAttribLinks are sorted by AttribIndex (low to high)
  EXPECT_EQ(span[0], vertexElementAttribConfigs[2]);
  EXPECT_EQ(span[1], vertexElementAttribConfigs[1]);
  EXPECT_EQ(span[2], vertexElementAttribConfigs[0]);
}
