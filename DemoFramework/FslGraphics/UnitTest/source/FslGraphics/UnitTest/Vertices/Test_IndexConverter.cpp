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
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <FslGraphics/Vertices/IndexConverter.hpp>
#include <FslUnitTest/Test/TestArray.hpp>
#include <array>

using namespace Fsl;

namespace
{
  using TestVertices_IndexConverter = TestFixtureFslGraphics;

  template <typename TDst, typename TSrc, std::size_t TSize>
  void DoGenericConvert(std::array<TDst, TSize>& rDst, const std::array<TSrc, TSize>& src)
  {
    IndexConverter::GenericConvert(rDst.data(), rDst.size() * sizeof(TDst), sizeof(TDst), src.data(), src.size() * sizeof(TSrc), sizeof(TSrc),
                                   src.size());
  }

  template <typename TDst, typename TSrc, std::size_t TSize>
  void DoGenericConvertNullSrc(std::array<TDst, TSize>& rDst, const std::array<TSrc, TSize>& src)
  {
    const TSrc* pNull = nullptr;
    IndexConverter::GenericConvert(rDst.data(), rDst.size() * sizeof(TDst), sizeof(TDst), pNull, src.size() * sizeof(TSrc), sizeof(TSrc), src.size());
  }

  template <typename TDst, typename TSrc, std::size_t TSize>
  void DoGenericConvertNullDst(std::array<TDst, TSize>& rDst, const std::array<TSrc, TSize>& src)
  {
    TDst* pNull = nullptr;
    IndexConverter::GenericConvert(pNull, rDst.size() * sizeof(TDst), sizeof(TDst), src.data(), src.size() * sizeof(TSrc), sizeof(TSrc), src.size());
  }
}


TEST(TestVertices_IndexConverter, Convert_src_uint8_dst_uint8)
{
  const std::array<uint8_t, 3> src = {1, 2, 3};
  std::array<uint8_t, 3> dst{};

  IndexConverter::Convert(dst.data(), dst.size(), src.data(), src.size());

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(src, dst));
}


TEST(TestVertices_IndexConverter, Convert_src_uint8_dst_uint8_NullSrc)
{
  const std::array<uint8_t, 3> src = {1, 2, 3};
  std::array<uint8_t, 3> dst{};

  const uint8_t* pNull = nullptr;
  EXPECT_THROW(IndexConverter::Convert(dst.data(), dst.size(), pNull, src.size()), std::invalid_argument);
}


TEST(TestVertices_IndexConverter, Convert_src_uint8_dst_uint8_NullDst)
{
  const std::array<uint8_t, 3> src = {1, 2, 3};
  std::array<uint8_t, 3> dst{};

  uint8_t* pNull = nullptr;
  EXPECT_THROW(IndexConverter::Convert(pNull, dst.size(), src.data(), src.size()), std::invalid_argument);
}


TEST(TestVertices_IndexConverter, Convert_src_uint8_dst_uint16)
{
  const std::array<uint8_t, 3> src = {1, 2, 3};
  std::array<uint16_t, 3> dst{};

  IndexConverter::Convert(dst.data(), dst.size(), src.data(), src.size());

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(src, dst));
}


TEST(TestVertices_IndexConverter, Convert_src_uint8_dst_uint16_NullSrc)
{
  const std::array<uint8_t, 3> src = {1, 2, 3};
  std::array<uint16_t, 3> dst{};

  const uint8_t* pNull = nullptr;
  EXPECT_THROW(IndexConverter::Convert(dst.data(), dst.size(), pNull, src.size()), std::invalid_argument);
}


TEST(TestVertices_IndexConverter, Convert_src_uint8_dst_uint16_NullDst)
{
  const std::array<uint8_t, 3> src = {1, 2, 3};
  std::array<uint16_t, 3> dst{};

  uint16_t* pNull = nullptr;
  EXPECT_THROW(IndexConverter::Convert(pNull, dst.size(), src.data(), src.size()), std::invalid_argument);
}


TEST(TestVertices_IndexConverter, Convert_src_uint16_dst_uint8)
{
  const std::array<uint16_t, 3> src = {1, 2, 3};
  std::array<uint8_t, 3> dst{};

  IndexConverter::Convert(dst.data(), dst.size(), src.data(), src.size());

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(src, dst));
}

TEST(TestVertices_IndexConverter, Convert_src_uint16_dst_uint8_value_overflow)
{
  const std::array<uint16_t, 3> src = {1, 2, 0x100};
  std::array<uint8_t, 3> dst{};

  EXPECT_THROW(IndexConverter::Convert(dst.data(), dst.size(), src.data(), src.size()), NotSupportedException);
}


TEST(TestVertices_IndexConverter, Convert_src_uint16_dst_uint8_NullSrc)
{
  const std::array<uint16_t, 3> src = {1, 2, 3};
  std::array<uint8_t, 3> dst{};

  const uint16_t* pNull = nullptr;
  EXPECT_THROW(IndexConverter::Convert(dst.data(), dst.size(), pNull, src.size()), std::invalid_argument);
}


TEST(TestVertices_IndexConverter, Convert_src_uint16_dst_uint8_NullDst)
{
  const std::array<uint16_t, 3> src = {1, 2, 3};
  std::array<uint8_t, 3> dst{};

  uint8_t* pNull = nullptr;
  EXPECT_THROW(IndexConverter::Convert(pNull, dst.size(), src.data(), src.size()), std::invalid_argument);
}


TEST(TestVertices_IndexConverter, Convert_src_uint16_dst_uint16)
{
  const std::array<uint16_t, 3> src = {1, 2, 3};
  std::array<uint16_t, 3> dst{};

  IndexConverter::Convert(dst.data(), dst.size(), src.data(), src.size());

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(src, dst));
}


TEST(TestVertices_IndexConverter, Convert_src_uint16_dst_uint16_NullSrc)
{
  const std::array<uint16_t, 3> src = {1, 2, 3};
  std::array<uint16_t, 3> dst{};

  const uint16_t* pNull = nullptr;
  EXPECT_THROW(IndexConverter::Convert(dst.data(), dst.size(), pNull, src.size()), std::invalid_argument);
}


TEST(TestVertices_IndexConverter, Convert_src_uint16_dst_uint16_NullDst)
{
  const std::array<uint16_t, 3> src = {1, 2, 3};
  std::array<uint16_t, 3> dst{};

  uint16_t* pNull = nullptr;
  EXPECT_THROW(IndexConverter::Convert(pNull, dst.size(), src.data(), src.size()), std::invalid_argument);
}

// GenericConvert


TEST(TestVertices_IndexConverter, GenericConvert_src_uint8_dst_uint8)
{
  const std::array<uint8_t, 3> src = {1, 2, 3};
  std::array<uint8_t, 3> dst{};

  DoGenericConvert(dst, src);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(src, dst));
}


TEST(TestVertices_IndexConverter, GenericConvert_src_uint8_dst_uint8_NullSrc)
{
  const std::array<uint8_t, 3> src = {1, 2, 3};
  std::array<uint8_t, 3> dst{};

  EXPECT_THROW(DoGenericConvertNullSrc(dst, src), std::invalid_argument);
}


TEST(TestVertices_IndexConverter, GenericConvert_src_uint8_dst_uint8_NullDst)
{
  const std::array<uint8_t, 3> src = {1, 2, 3};
  std::array<uint8_t, 3> dst{};

  EXPECT_THROW(DoGenericConvertNullDst(dst, src), std::invalid_argument);
}


TEST(TestVertices_IndexConverter, GenericConvert_src_uint8_dst_uint16)
{
  const std::array<uint8_t, 3> src = {1, 2, 3};
  std::array<uint16_t, 3> dst{};

  DoGenericConvert(dst, src);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(src, dst));
}


TEST(TestVertices_IndexConverter, GenericConvert_src_uint8_dst_uint16_NullSrc)
{
  const std::array<uint8_t, 3> src = {1, 2, 3};
  std::array<uint16_t, 3> dst{};

  EXPECT_THROW(DoGenericConvertNullSrc(dst, src), std::invalid_argument);
}


TEST(TestVertices_IndexConverter, GenericConvert_src_uint8_dst_uint16_NullDst)
{
  const std::array<uint8_t, 3> src = {1, 2, 3};
  std::array<uint16_t, 3> dst{};

  EXPECT_THROW(DoGenericConvertNullDst(dst, src), std::invalid_argument);
}


TEST(TestVertices_IndexConverter, GenericConvert_src_uint16_dst_uint8)
{
  const std::array<uint16_t, 3> src = {1, 2, 3};
  std::array<uint8_t, 3> dst{};

  DoGenericConvert(dst, src);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(src, dst));
}

TEST(TestVertices_IndexConverter, GenericConvert_src_uint16_dst_uint8_value_overflow)
{
  const std::array<uint16_t, 3> src = {1, 2, 0x100};
  std::array<uint8_t, 3> dst{};

  EXPECT_THROW(DoGenericConvert(dst, src), NotSupportedException);
}


TEST(TestVertices_IndexConverter, GenericConvert_src_uint16_dst_uint8_NullSrc)
{
  const std::array<uint16_t, 3> src = {1, 2, 3};
  std::array<uint8_t, 3> dst{};

  EXPECT_THROW(DoGenericConvertNullSrc(dst, src), std::invalid_argument);
}


TEST(TestVertices_IndexConverter, GenericConvert_src_uint16_dst_uint8_NullDst)
{
  const std::array<uint16_t, 3> src = {1, 2, 3};
  std::array<uint8_t, 3> dst{};

  EXPECT_THROW(DoGenericConvertNullDst(dst, src), std::invalid_argument);
}


TEST(TestVertices_IndexConverter, GenericConvert_src_uint16_dst_uint16)
{
  const std::array<uint16_t, 3> src = {1, 2, 3};
  std::array<uint16_t, 3> dst{};

  DoGenericConvert(dst, src);

  EXPECT_TRUE(Fsl::Test::IsArrayContentEqual(src, dst));
}


TEST(TestVertices_IndexConverter, GenericConvert_src_uint16_dst_uint16_NullSrc)
{
  const std::array<uint16_t, 3> src = {1, 2, 3};
  std::array<uint16_t, 3> dst{};

  EXPECT_THROW(DoGenericConvertNullSrc(dst, src), std::invalid_argument);
}


TEST(TestVertices_IndexConverter, GenericConvert_src_uint16_dst_uint16_NullDst)
{
  const std::array<uint16_t, 3> src = {1, 2, 3};
  std::array<uint16_t, 3> dst{};

  EXPECT_THROW(DoGenericConvertNullDst(dst, src), std::invalid_argument);
}


TEST(TestVertices_IndexConverter, GenericConvert_src_uint8_dst_uint32_NotSupported)
{
  const std::array<uint8_t, 3> src = {1, 2, 3};
  std::array<uint32_t, 3> dst{};

  EXPECT_THROW(DoGenericConvert(dst, src), NotSupportedException);
}


TEST(TestVertices_IndexConverter, GenericConvert_src_uint16_dst_uint32_NotSupported)
{
  const std::array<uint16_t, 3> src = {1, 2, 3};
  std::array<uint32_t, 3> dst{};

  EXPECT_THROW(DoGenericConvert(dst, src), NotSupportedException);
}


TEST(TestVertices_IndexConverter, GenericConvert_src_uint32_dst_uint8_NotSupported)
{
  const std::array<uint32_t, 3> src = {1, 2, 3};
  std::array<uint8_t, 3> dst{};

  EXPECT_THROW(DoGenericConvert(dst, src), NotSupportedException);
}


TEST(TestVertices_IndexConverter, GenericConvert_src_uint32_dst_uint16_NotSupported)
{
  const std::array<uint32_t, 3> src = {1, 2, 3};
  std::array<uint16_t, 3> dst{};

  EXPECT_THROW(DoGenericConvert(dst, src), NotSupportedException);
}


TEST(TestVertices_IndexConverter, GenericConvert_src_uint32_dst_uint32_NotSupported)
{
  const std::array<uint32_t, 3> src = {1, 2, 3};
  std::array<uint32_t, 3> dst{};

  EXPECT_THROW(DoGenericConvert(dst, src), NotSupportedException);
}
