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

#include <FslGraphics/Vertices/VertexElementFormatUtil.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Math/LogMatrix.hpp>
#include <FslBase/Log/Math/LogPoint2.hpp>
#include <FslBase/Log/Math/LogVector2.hpp>
#include <FslBase/Log/Math/LogVector3.hpp>
#include <FslBase/Log/Math/LogVector4.hpp>
#include <FslGraphics/Log/LogColor.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  using TestVertices_VertexElementFormatUtil = TestFixtureFslGraphics;
}


TEST(TestVertices_VertexElementFormatUtil, GetBytesPerElement)
{
  EXPECT_THROW(VertexElementFormatUtil::GetBytesPerElement(VertexElementFormat::Undefined), NotSupportedException);
  EXPECT_EQ(4u, VertexElementFormatUtil::GetBytesPerElement(VertexElementFormat::Single));
  EXPECT_EQ(8u, VertexElementFormatUtil::GetBytesPerElement(VertexElementFormat::Vector2));
  EXPECT_EQ(12u, VertexElementFormatUtil::GetBytesPerElement(VertexElementFormat::Vector3));
  EXPECT_EQ(16u, VertexElementFormatUtil::GetBytesPerElement(VertexElementFormat::Vector4));
  EXPECT_EQ(64u, VertexElementFormatUtil::GetBytesPerElement(VertexElementFormat::Matrix4x4));
  EXPECT_EQ(1u, VertexElementFormatUtil::GetBytesPerElement(VertexElementFormat::X8_UNORM));
  EXPECT_EQ(1u, VertexElementFormatUtil::GetBytesPerElement(VertexElementFormat::X8_UINT));
  EXPECT_EQ(2u, VertexElementFormatUtil::GetBytesPerElement(VertexElementFormat::X8Y8_UNORM));
  EXPECT_EQ(2u, VertexElementFormatUtil::GetBytesPerElement(VertexElementFormat::X8Y8_UINT));
  EXPECT_EQ(3u, VertexElementFormatUtil::GetBytesPerElement(VertexElementFormat::X8Y8Z8_UNORM));
  EXPECT_EQ(3u, VertexElementFormatUtil::GetBytesPerElement(VertexElementFormat::X8Y8Z8_UINT));
  EXPECT_EQ(4u, VertexElementFormatUtil::GetBytesPerElement(VertexElementFormat::X8Y8Z8W8_UNORM));
  EXPECT_EQ(4u, VertexElementFormatUtil::GetBytesPerElement(VertexElementFormat::X8Y8Z8W8_UINT));
}


TEST(TestVertices_VertexElementFormatUtil, TryGetBytesPerElement)
{
  EXPECT_LE(VertexElementFormatUtil::TryGetBytesPerElement(VertexElementFormat::Undefined), 0);
  EXPECT_EQ(4, VertexElementFormatUtil::TryGetBytesPerElement(VertexElementFormat::Single));
  EXPECT_EQ(8, VertexElementFormatUtil::TryGetBytesPerElement(VertexElementFormat::Vector2));
  EXPECT_EQ(12, VertexElementFormatUtil::TryGetBytesPerElement(VertexElementFormat::Vector3));
  EXPECT_EQ(16, VertexElementFormatUtil::TryGetBytesPerElement(VertexElementFormat::Vector4));
  EXPECT_EQ(64, VertexElementFormatUtil::TryGetBytesPerElement(VertexElementFormat::Matrix4x4));
  EXPECT_EQ(1, VertexElementFormatUtil::TryGetBytesPerElement(VertexElementFormat::X8_UNORM));
  EXPECT_EQ(1, VertexElementFormatUtil::TryGetBytesPerElement(VertexElementFormat::X8_UINT));
  EXPECT_EQ(2, VertexElementFormatUtil::TryGetBytesPerElement(VertexElementFormat::X8Y8_UNORM));
  EXPECT_EQ(2, VertexElementFormatUtil::TryGetBytesPerElement(VertexElementFormat::X8Y8_UINT));
  EXPECT_EQ(3, VertexElementFormatUtil::TryGetBytesPerElement(VertexElementFormat::X8Y8Z8_UNORM));
  EXPECT_EQ(3, VertexElementFormatUtil::TryGetBytesPerElement(VertexElementFormat::X8Y8Z8_UINT));
  EXPECT_EQ(4, VertexElementFormatUtil::TryGetBytesPerElement(VertexElementFormat::X8Y8Z8W8_UNORM));
  EXPECT_EQ(4, VertexElementFormatUtil::TryGetBytesPerElement(VertexElementFormat::X8Y8Z8W8_UINT));
}

TEST(TestVertices_VertexElementFormatUtil, GetElementCount)
{
  EXPECT_THROW(VertexElementFormatUtil::GetElementCount(VertexElementFormat::Undefined), NotSupportedException);
  EXPECT_EQ(1u, VertexElementFormatUtil::GetElementCount(VertexElementFormat::Single));
  EXPECT_EQ(2u, VertexElementFormatUtil::GetElementCount(VertexElementFormat::Vector2));
  EXPECT_EQ(3u, VertexElementFormatUtil::GetElementCount(VertexElementFormat::Vector3));
  EXPECT_EQ(4u, VertexElementFormatUtil::GetElementCount(VertexElementFormat::Vector4));
  EXPECT_EQ(16u, VertexElementFormatUtil::GetElementCount(VertexElementFormat::Matrix4x4));
  EXPECT_EQ(1u, VertexElementFormatUtil::GetElementCount(VertexElementFormat::X8_UNORM));
  EXPECT_EQ(1u, VertexElementFormatUtil::GetElementCount(VertexElementFormat::X8_UINT));
  EXPECT_EQ(2u, VertexElementFormatUtil::GetElementCount(VertexElementFormat::X8Y8_UNORM));
  EXPECT_EQ(2u, VertexElementFormatUtil::GetElementCount(VertexElementFormat::X8Y8_UINT));
  EXPECT_EQ(3u, VertexElementFormatUtil::GetElementCount(VertexElementFormat::X8Y8Z8_UNORM));
  EXPECT_EQ(3u, VertexElementFormatUtil::GetElementCount(VertexElementFormat::X8Y8Z8_UINT));
  EXPECT_EQ(4u, VertexElementFormatUtil::GetElementCount(VertexElementFormat::X8Y8Z8W8_UNORM));
  EXPECT_EQ(4u, VertexElementFormatUtil::GetElementCount(VertexElementFormat::X8Y8Z8W8_UINT));
}

TEST(TestVertices_VertexElementFormatUtil, TryGetElementCount)
{
  EXPECT_LE(VertexElementFormatUtil::TryGetElementCount(VertexElementFormat::Undefined), 0);
  EXPECT_EQ(1, VertexElementFormatUtil::TryGetElementCount(VertexElementFormat::Single));
  EXPECT_EQ(2, VertexElementFormatUtil::TryGetElementCount(VertexElementFormat::Vector2));
  EXPECT_EQ(3, VertexElementFormatUtil::TryGetElementCount(VertexElementFormat::Vector3));
  EXPECT_EQ(4, VertexElementFormatUtil::TryGetElementCount(VertexElementFormat::Vector4));
  EXPECT_EQ(16, VertexElementFormatUtil::TryGetElementCount(VertexElementFormat::Matrix4x4));
  EXPECT_EQ(1, VertexElementFormatUtil::TryGetElementCount(VertexElementFormat::X8_UNORM));
  EXPECT_EQ(1, VertexElementFormatUtil::TryGetElementCount(VertexElementFormat::X8_UINT));
  EXPECT_EQ(2, VertexElementFormatUtil::TryGetElementCount(VertexElementFormat::X8Y8_UNORM));
  EXPECT_EQ(2, VertexElementFormatUtil::TryGetElementCount(VertexElementFormat::X8Y8_UINT));
  EXPECT_EQ(3, VertexElementFormatUtil::TryGetElementCount(VertexElementFormat::X8Y8Z8_UNORM));
  EXPECT_EQ(3, VertexElementFormatUtil::TryGetElementCount(VertexElementFormat::X8Y8Z8_UINT));
  EXPECT_EQ(4, VertexElementFormatUtil::TryGetElementCount(VertexElementFormat::X8Y8Z8W8_UNORM));
  EXPECT_EQ(4, VertexElementFormatUtil::TryGetElementCount(VertexElementFormat::X8Y8Z8W8_UINT));
}
