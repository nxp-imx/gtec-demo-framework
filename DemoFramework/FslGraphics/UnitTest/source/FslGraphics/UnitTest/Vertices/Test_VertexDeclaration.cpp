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

#include <FslGraphics/Vertices/VertexDeclaration.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Math/LogMatrix.hpp>
#include <FslBase/Log/Math/LogPoint2.hpp>
#include <FslBase/Log/Math/LogVector2.hpp>
#include <FslBase/Log/Math/LogVector3.hpp>
#include <FslBase/Log/Math/LogVector4.hpp>
#include <FslGraphics/Log/LogColor.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <array>
#include <utility>

using namespace Fsl;

namespace
{
  using TestVertices_VertexDeclaration = TestFixtureFslGraphics;
}


TEST(TestVertices_VertexDeclaration, Construct_Default)
{
  VertexDeclaration decl;
  EXPECT_EQ(0u, decl.VertexStride());
  EXPECT_EQ(0u, decl.Count());
  EXPECT_EQ(nullptr, decl.DirectAccess());
  EXPECT_THROW(decl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u), NotFoundException);
  EXPECT_LE(decl.VertexElementIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_THROW(decl.VertexElementGet(VertexElementUsage::Position, 0u), NotFoundException);
  EXPECT_EQ(VertexDeclaration(), decl);
}


TEST(TestVertices_VertexDeclaration, Construct)
{
  const std::array<VertexElementEx, 1> elements = {VertexElementEx(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u)};

  VertexDeclaration decl(elements.data(), elements.size(), sizeof(Vector3));

  EXPECT_EQ(sizeof(Vector3), decl.VertexStride());
  EXPECT_EQ(elements.size(), decl.Count());
  EXPECT_NE(nullptr, decl.DirectAccess());

  EXPECT_EQ(0, decl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u));
  EXPECT_EQ(0, decl.VertexElementIndexOf(VertexElementUsage::Position, 0u));
  EXPECT_EQ(elements[0], decl.VertexElementGet(VertexElementUsage::Position, 0u));

  EXPECT_NE(VertexDeclaration(), decl);
}


TEST(TestVertices_VertexDeclaration, Construct_Duplicated_Exact)
{
  const std::array<VertexElementEx, 2> elements = {VertexElementEx(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u),
                                                   VertexElementEx(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u)};

  EXPECT_THROW(VertexDeclaration(elements.data(), elements.size(), sizeof(Vector3)), NotSupportedException);
}


TEST(TestVertices_VertexDeclaration, Construct_Duplicated_SmallDiff1)
{
  const std::array<VertexElementEx, 2> elements = {VertexElementEx(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u),
                                                   VertexElementEx(0, VertexElementFormat::Vector2, VertexElementUsage::Position, 0u)};

  EXPECT_THROW(VertexDeclaration(elements.data(), elements.size(), sizeof(Vector3) + sizeof(Vector2)), NotSupportedException);
}

TEST(TestVertices_VertexDeclaration, Construct_Duplicated_SmallDiff2)
{
  const std::array<VertexElementEx, 2> elements = {VertexElementEx(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u),
                                                   VertexElementEx(12, VertexElementFormat::Vector2, VertexElementUsage::Position, 0u)};

  EXPECT_THROW(VertexDeclaration(elements.data(), elements.size(), sizeof(Vector3) + sizeof(Vector2)), NotSupportedException);
}

TEST(TestVertices_VertexDeclaration, Construct_Duplicated_SmallDiff3)
{
  const std::array<VertexElementEx, 2> elements = {VertexElementEx(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u),
                                                   VertexElementEx(12, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u)};

  EXPECT_THROW(VertexDeclaration(elements.data(), elements.size(), sizeof(Vector3) * 2), NotSupportedException);
}


TEST(TestVertices_VertexDeclaration, Construct_OutsideStride)
{
  const std::array<VertexElementEx, 2> elements = {VertexElementEx(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u),
                                                   VertexElementEx(12, VertexElementFormat::Vector3, VertexElementUsage::Position, 1u)};

  EXPECT_THROW(VertexDeclaration(elements.data(), elements.size(), sizeof(Vector3)), NotSupportedException);
}


TEST(TestVertices_VertexDeclaration, Construct_OutOfOrder)
{
  const std::array<VertexElementEx, 2> elements = {VertexElementEx(12, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u),
                                                   VertexElementEx(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 1u)};

  VertexDeclaration decl(elements.data(), elements.size(), sizeof(Vector3) * 2);

  EXPECT_EQ(sizeof(Vector3) * 2, decl.VertexStride());
  EXPECT_EQ(elements.size(), decl.Count());
  EXPECT_NE(nullptr, decl.DirectAccess());

  // The vertex decl will reorder things to be in order
  EXPECT_EQ(0, decl.VertexElementGetIndexOf(VertexElementUsage::Position, 1u));
  EXPECT_EQ(0, decl.VertexElementIndexOf(VertexElementUsage::Position, 1u));
  EXPECT_EQ(elements[1], decl.VertexElementGet(VertexElementUsage::Position, 1u));

  EXPECT_EQ(1, decl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u));
  EXPECT_EQ(1, decl.VertexElementIndexOf(VertexElementUsage::Position, 0u));
  EXPECT_EQ(elements[0], decl.VertexElementGet(VertexElementUsage::Position, 0u));
}


TEST(TestVertices_VertexDeclaration, Construct_Null)
{
  const std::array<VertexElementEx, 1> elements = {VertexElementEx(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u)};

  EXPECT_THROW(VertexDeclaration(nullptr, elements.size(), sizeof(Vector3)), std::invalid_argument);
}


TEST(TestVertices_VertexDeclaration, Reset_Empty)
{
  VertexDeclaration decl;

  const std::array<VertexElementEx, 1> elements = {VertexElementEx(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u)};
  decl.Reset(elements.data(), elements.size(), sizeof(Vector3));

  EXPECT_EQ(sizeof(Vector3), decl.VertexStride());
  EXPECT_EQ(elements.size(), decl.Count());
  EXPECT_NE(nullptr, decl.DirectAccess());

  EXPECT_EQ(0, decl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u));
  EXPECT_EQ(0, decl.VertexElementIndexOf(VertexElementUsage::Position, 0u));
  EXPECT_EQ(elements[0], decl.VertexElementGet(VertexElementUsage::Position, 0u));

  EXPECT_NE(VertexDeclaration(), decl);
}


TEST(TestVertices_VertexDeclaration, Reset_Empty_Null)
{
  VertexDeclaration decl;

  const std::array<VertexElementEx, 1> elements = {VertexElementEx(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u)};
  EXPECT_THROW(decl.Reset(nullptr, elements.size(), sizeof(Vector3)), std::invalid_argument);
}


TEST(TestVertices_VertexDeclaration, MoveConstruct)
{
  std::array<VertexElementEx, 1> elements = {VertexElementEx(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u)};

  VertexDeclaration decl1(elements.data(), elements.size(), sizeof(Vector3));
  VertexDeclaration decl2(std::move(decl1));

  // FslGraphics types prefer to reset their content when moved
  EXPECT_EQ(VertexDeclaration(), decl1);    // NOLINT(bugprone-use-after-move)

  EXPECT_EQ(sizeof(Vector3), decl2.VertexStride());
  EXPECT_EQ(elements.size(), decl2.Count());
  EXPECT_NE(nullptr, decl2.DirectAccess());

  EXPECT_EQ(0, decl2.VertexElementGetIndexOf(VertexElementUsage::Position, 0u));
  EXPECT_EQ(0, decl2.VertexElementIndexOf(VertexElementUsage::Position, 0u));
  EXPECT_EQ(elements[0], decl2.VertexElementGet(VertexElementUsage::Position, 0u));
}


TEST(TestVertices_VertexDeclaration, MoveAssign)
{
  std::array<VertexElementEx, 1> elements = {VertexElementEx(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u)};

  VertexDeclaration decl1(elements.data(), elements.size(), sizeof(Vector3));
  VertexDeclaration decl2;

  decl2 = std::move(decl1);

  // FslGraphics types prefer to reset their content when moved
  EXPECT_EQ(VertexDeclaration(), decl1);    // NOLINT(bugprone-use-after-move)

  EXPECT_EQ(sizeof(Vector3), decl2.VertexStride());
  EXPECT_EQ(elements.size(), decl2.Count());
  EXPECT_NE(nullptr, decl2.DirectAccess());

  EXPECT_EQ(0, decl2.VertexElementGetIndexOf(VertexElementUsage::Position, 0u));
  EXPECT_EQ(0, decl2.VertexElementIndexOf(VertexElementUsage::Position, 0u));
  EXPECT_EQ(elements[0], decl2.VertexElementGet(VertexElementUsage::Position, 0u));
}


TEST(TestVertices_VertexDeclaration, Equal)
{
  const std::array<VertexElementEx, 1> elements1 = {VertexElementEx(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u)};
  const std::array<VertexElementEx, 2> elements2 = {VertexElementEx(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u),
                                                    VertexElementEx(12, VertexElementFormat::Vector3, VertexElementUsage::Position, 1u)};
  const std::array<VertexElementEx, 2> elements3 = {VertexElementEx(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u),
                                                    VertexElementEx(12, VertexElementFormat::Vector2, VertexElementUsage::Position, 1u)};
  const std::array<VertexElementEx, 2> elements4 = {VertexElementEx(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u),
                                                    VertexElementEx(12, VertexElementFormat::Vector3, VertexElementUsage::Normal, 1u)};

  const VertexDeclaration decl0;
  const VertexDeclaration decl1(elements1.data(), elements1.size(), sizeof(Vector3));
  const VertexDeclaration decl2(elements2.data(), elements2.size(), sizeof(Vector3) * 2);
  const VertexDeclaration decl3(elements3.data(), elements3.size(), sizeof(Vector3) + sizeof(Vector2));
  const VertexDeclaration decl4(elements4.data(), elements4.size(), sizeof(Vector3) * 2);
  EXPECT_EQ(decl0, decl0);
  EXPECT_EQ(decl1, decl1);
  EXPECT_EQ(decl2, decl2);
  EXPECT_EQ(decl3, decl3);
  EXPECT_EQ(decl4, decl4);
}


TEST(TestVertices_VertexDeclaration, NotEqual)
{
  const std::array<VertexElementEx, 1> elements1 = {VertexElementEx(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u)};
  const std::array<VertexElementEx, 2> elements2 = {VertexElementEx(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u),
                                                    VertexElementEx(12, VertexElementFormat::Vector3, VertexElementUsage::Position, 1u)};
  const std::array<VertexElementEx, 2> elements3 = {VertexElementEx(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u),
                                                    VertexElementEx(12, VertexElementFormat::Vector2, VertexElementUsage::Position, 1u)};
  const std::array<VertexElementEx, 2> elements4 = {VertexElementEx(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0u),
                                                    VertexElementEx(12, VertexElementFormat::Vector3, VertexElementUsage::Normal, 1u)};

  const VertexDeclaration decl0;
  const VertexDeclaration decl1(elements1.data(), elements1.size(), sizeof(Vector3));
  const VertexDeclaration decl2(elements2.data(), elements2.size(), sizeof(Vector3) * 2);
  const VertexDeclaration decl3(elements3.data(), elements3.size(), sizeof(Vector3) + sizeof(Vector2));
  const VertexDeclaration decl4(elements4.data(), elements4.size(), sizeof(Vector3) * 2);
  EXPECT_NE(decl0, decl1);
  EXPECT_NE(decl0, decl2);
  EXPECT_NE(decl0, decl3);
  EXPECT_NE(decl0, decl4);

  EXPECT_NE(decl1, decl0);
  EXPECT_NE(decl1, decl2);
  EXPECT_NE(decl1, decl3);
  EXPECT_NE(decl1, decl4);

  EXPECT_NE(decl2, decl0);
  EXPECT_NE(decl2, decl1);
  EXPECT_NE(decl2, decl3);
  EXPECT_NE(decl2, decl4);

  EXPECT_NE(decl3, decl0);
  EXPECT_NE(decl3, decl1);
  EXPECT_NE(decl3, decl2);
  EXPECT_NE(decl3, decl4);

  EXPECT_NE(decl4, decl0);
  EXPECT_NE(decl4, decl1);
  EXPECT_NE(decl4, decl2);
  EXPECT_NE(decl4, decl3);
}
