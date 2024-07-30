/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <FslDataBinding/Base/Bind/MultiConverterBinding.hpp>
#include <array>
#include <cmath>
#include <utility>
#include "../UTTypedDependencyPropertyMethods.hpp"
#include "../UTTypedDependencyPropertyRefMethods.hpp"

using namespace Fsl;

namespace
{
  using TestBind_MultiConverterBinding = TestFixtureFslBase;


  int32_t ConvertFloatAndUInt32ToInt(const float value0, const uint32_t value1)
  {
    return static_cast<int32_t>(std::round(value0 + static_cast<float>(value1)));
  }
}

TEST(TestBind_MultiConverterBinding, NormalGet0Get1Set)
{
  // internal property link objects
  UTTypedDependencyPropertyMethods<float> valFloat(1.5f);
  UTTypedDependencyPropertyMethods<uint32_t> valUInt32(7);
  UTTypedDependencyPropertyMethods<int32_t> valInt32(1);

  // binding with user defined conversion operator
  DataBinding::MultiConverterBinding<int32_t, float, uint32_t> binding(ConvertFloatAndUInt32ToInt);
  DataBinding::AMultiConverterBinding& genericConvertingBinding = binding;

  std::array<DataBinding::Internal::PropertyGetInfo, 2> getters = {DataBinding::Internal::PropertyGetInfo(valFloat.GetImplType(), &valFloat),
                                                                   DataBinding::Internal::PropertyGetInfo(valUInt32.GetImplType(), &valUInt32)};

  auto res = genericConvertingBinding.Convert(valInt32.GetImplType(), &valInt32, SpanUtil::AsReadOnlySpan(getters));

  EXPECT_EQ(DataBinding::Internal::PropertySetResult::ValueChanged, res);
  EXPECT_EQ(int32_t(2 + 7), valInt32.Get());
}


TEST(TestBind_MultiConverterBinding, NormalGet0RefGet1Set)
{
  // internal property link objects
  UTTypedDependencyPropertyRefMethods<float> valFloat(1.5f);
  UTTypedDependencyPropertyMethods<uint32_t> valUInt32(7);
  UTTypedDependencyPropertyMethods<int32_t> valInt32(1);

  // binding with user defined conversion operator
  DataBinding::MultiConverterBinding<int32_t, float, uint32_t> binding(ConvertFloatAndUInt32ToInt);
  DataBinding::AMultiConverterBinding& genericConvertingBinding = binding;

  std::array<DataBinding::Internal::PropertyGetInfo, 2> getters = {DataBinding::Internal::PropertyGetInfo(valFloat.GetImplType(), &valFloat),
                                                                   DataBinding::Internal::PropertyGetInfo(valUInt32.GetImplType(), &valUInt32)};

  auto res = genericConvertingBinding.Convert(valInt32.GetImplType(), &valInt32, SpanUtil::AsReadOnlySpan(getters));

  EXPECT_EQ(DataBinding::Internal::PropertySetResult::ValueChanged, res);
  EXPECT_EQ(int32_t(2 + 7), valInt32.Get());
}

TEST(TestBind_MultiConverterBinding, NormalGet0Get1RefSet)
{
  // internal property link objects
  UTTypedDependencyPropertyMethods<float> valFloat(1.5f);
  UTTypedDependencyPropertyRefMethods<uint32_t> valUInt32(7);
  UTTypedDependencyPropertyMethods<int32_t> valInt32(1);

  // binding with user defined conversion operator
  DataBinding::MultiConverterBinding<int32_t, float, uint32_t> binding(ConvertFloatAndUInt32ToInt);
  DataBinding::AMultiConverterBinding& genericConvertingBinding = binding;

  std::array<DataBinding::Internal::PropertyGetInfo, 2> getters = {DataBinding::Internal::PropertyGetInfo(valFloat.GetImplType(), &valFloat),
                                                                   DataBinding::Internal::PropertyGetInfo(valUInt32.GetImplType(), &valUInt32)};

  auto res = genericConvertingBinding.Convert(valInt32.GetImplType(), &valInt32, SpanUtil::AsReadOnlySpan(getters));

  EXPECT_EQ(DataBinding::Internal::PropertySetResult::ValueChanged, res);
  EXPECT_EQ(int32_t(2 + 7), valInt32.Get());
}

TEST(TestBind_MultiConverterBinding, NormalGetRef0Get1RefSet)
{
  // internal property link objects
  UTTypedDependencyPropertyRefMethods<float> valFloat(1.5f);
  UTTypedDependencyPropertyRefMethods<uint32_t> valUInt32(7);
  UTTypedDependencyPropertyMethods<int32_t> valInt32(1);

  // binding with user defined conversion operator
  DataBinding::MultiConverterBinding<int32_t, float, uint32_t> binding(ConvertFloatAndUInt32ToInt);
  DataBinding::AMultiConverterBinding& genericConvertingBinding = binding;

  std::array<DataBinding::Internal::PropertyGetInfo, 2> getters = {DataBinding::Internal::PropertyGetInfo(valFloat.GetImplType(), &valFloat),
                                                                   DataBinding::Internal::PropertyGetInfo(valUInt32.GetImplType(), &valUInt32)};

  auto res = genericConvertingBinding.Convert(valInt32.GetImplType(), &valInt32, SpanUtil::AsReadOnlySpan(getters));

  EXPECT_EQ(DataBinding::Internal::PropertySetResult::ValueChanged, res);
  EXPECT_EQ(int32_t(2 + 7), valInt32.Get());
}


TEST(TestBind_MultiConverterBinding, NormalGet0Get1SetRef)
{
  // internal property link objects
  UTTypedDependencyPropertyMethods<float> valFloat(1.5f);
  UTTypedDependencyPropertyMethods<uint32_t> valUInt32(7);
  UTTypedDependencyPropertyRefMethods<int32_t> valInt32(1);

  // binding with user defined conversion operator
  DataBinding::MultiConverterBinding<int32_t, float, uint32_t> binding(ConvertFloatAndUInt32ToInt);
  DataBinding::AMultiConverterBinding& genericConvertingBinding = binding;

  std::array<DataBinding::Internal::PropertyGetInfo, 2> getters = {DataBinding::Internal::PropertyGetInfo(valFloat.GetImplType(), &valFloat),
                                                                   DataBinding::Internal::PropertyGetInfo(valUInt32.GetImplType(), &valUInt32)};

  auto res = genericConvertingBinding.Convert(valInt32.GetImplType(), &valInt32, SpanUtil::AsReadOnlySpan(getters));

  EXPECT_EQ(DataBinding::Internal::PropertySetResult::ValueChanged, res);
  EXPECT_EQ(int32_t(2 + 7), valInt32.Get());
}


TEST(TestBind_MultiConverterBinding, NormalGet0RefGet1SetRef)
{
  // internal property link objects
  UTTypedDependencyPropertyRefMethods<float> valFloat(1.5f);
  UTTypedDependencyPropertyMethods<uint32_t> valUInt32(7);
  UTTypedDependencyPropertyRefMethods<int32_t> valInt32(1);

  // binding with user defined conversion operator
  DataBinding::MultiConverterBinding<int32_t, float, uint32_t> binding(ConvertFloatAndUInt32ToInt);
  DataBinding::AMultiConverterBinding& genericConvertingBinding = binding;

  std::array<DataBinding::Internal::PropertyGetInfo, 2> getters = {DataBinding::Internal::PropertyGetInfo(valFloat.GetImplType(), &valFloat),
                                                                   DataBinding::Internal::PropertyGetInfo(valUInt32.GetImplType(), &valUInt32)};

  auto res = genericConvertingBinding.Convert(valInt32.GetImplType(), &valInt32, SpanUtil::AsReadOnlySpan(getters));

  EXPECT_EQ(DataBinding::Internal::PropertySetResult::ValueChanged, res);
  EXPECT_EQ(int32_t(2 + 7), valInt32.Get());
}

TEST(TestBind_MultiConverterBinding, NormalGet0Get1RefSetRef)
{
  // internal property link objects
  UTTypedDependencyPropertyMethods<float> valFloat(1.5f);
  UTTypedDependencyPropertyRefMethods<uint32_t> valUInt32(7);
  UTTypedDependencyPropertyRefMethods<int32_t> valInt32(1);

  // binding with user defined conversion operator
  DataBinding::MultiConverterBinding<int32_t, float, uint32_t> binding(ConvertFloatAndUInt32ToInt);
  DataBinding::AMultiConverterBinding& genericConvertingBinding = binding;

  std::array<DataBinding::Internal::PropertyGetInfo, 2> getters = {DataBinding::Internal::PropertyGetInfo(valFloat.GetImplType(), &valFloat),
                                                                   DataBinding::Internal::PropertyGetInfo(valUInt32.GetImplType(), &valUInt32)};

  auto res = genericConvertingBinding.Convert(valInt32.GetImplType(), &valInt32, SpanUtil::AsReadOnlySpan(getters));

  EXPECT_EQ(DataBinding::Internal::PropertySetResult::ValueChanged, res);
  EXPECT_EQ(int32_t(2 + 7), valInt32.Get());
}

TEST(TestBind_MultiConverterBinding, NormalGetRef0Get1RefSetRef)
{
  // internal property link objects
  UTTypedDependencyPropertyRefMethods<float> valFloat(1.5f);
  UTTypedDependencyPropertyRefMethods<uint32_t> valUInt32(7);
  UTTypedDependencyPropertyRefMethods<int32_t> valInt32(1);

  // binding with user defined conversion operator
  DataBinding::MultiConverterBinding<int32_t, float, uint32_t> binding(ConvertFloatAndUInt32ToInt);
  DataBinding::AMultiConverterBinding& genericConvertingBinding = binding;

  std::array<DataBinding::Internal::PropertyGetInfo, 2> getters = {DataBinding::Internal::PropertyGetInfo(valFloat.GetImplType(), &valFloat),
                                                                   DataBinding::Internal::PropertyGetInfo(valUInt32.GetImplType(), &valUInt32)};

  auto res = genericConvertingBinding.Convert(valInt32.GetImplType(), &valInt32, SpanUtil::AsReadOnlySpan(getters));

  EXPECT_EQ(DataBinding::Internal::PropertySetResult::ValueChanged, res);
  EXPECT_EQ(int32_t(2 + 7), valInt32.Get());
}
