/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <FslDataBinding/Base/Bind/ConverterBinding.hpp>
#include <FslDataBinding/Base/Bind/MultiConverterBinding.hpp>
#include <FslDataBinding/Base/Bind/TwoWayConverterBinding.hpp>
#include <FslDataBinding/Base/Bind/TwoWayMultiConverterBinding.hpp>
#include <FslDataBinding/Base/Binding.hpp>
#include <FslDataBinding/Base/DataBindingService.hpp>
#include <FslDataBinding/Base/Exceptions.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <memory>
#include "UTDependencyObject.hpp"
#include "UTDependencyObject2.hpp"
#include "UTReadOnlyDependencyObject.hpp"

using namespace Fsl;

namespace
{
  using Test_GetRSetConvert_TwoWay = TestFixtureFslBase;
}

TEST(Test_GetRSetConvert_TwoWay, ConverterBinding_OneWayConverter_BoundAsTwoWay)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();
  UTReadOnlyDependencyObject src(dataBindingService);

  auto convertingBinding =
    std::make_shared<Fsl::DataBinding::ConverterBinding<uint32_t, float>>([](const float value) { return static_cast<uint32_t>(std::round(value)); });

  // A one-way converter binding can not be set as a TwoWay binding
  EXPECT_THROW(
    DataBinding::Binding(convertingBinding, DataBinding::BindingMode::TwoWay, src.GetPropertyHandle(UTReadOnlyDependencyObject::Property1)),
    NotSupportedException);
}


TEST(Test_GetRSetConvert_TwoWay, MultiConverterBinding_OneWayConverter_BoundAsTwoWay)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();
  UTReadOnlyDependencyObject src0(dataBindingService);
  UTDependencyObject src1(dataBindingService);

  auto convertingBinding = std::make_shared<Fsl::DataBinding::MultiConverterBinding<uint32_t, float, float>>(
    [](const float value0, const float value1) { return static_cast<uint32_t>(std::round(value0 + value1)); });

  // A one-way multi-converter binding can not be set as a TwoWay binding
  EXPECT_THROW(DataBinding::Binding(convertingBinding, DataBinding::BindingMode::TwoWay,
                                    src0.GetPropertyHandle(UTReadOnlyDependencyObject::Property1),
                                    src1.GetPropertyHandle(UTDependencyObject::Property1)),
               NotSupportedException);
}


TEST(Test_GetRSetConvert_TwoWay, SetBinding_DepProperty_DepProperty_Init)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject src(dataBindingService);
  UTDependencyObject dst(dataBindingService);

  const uint32_t srcDefaultValue0 = 0;
  const float dstDefaultValue = 0;
  const float srcNewValue = 1.5f;
  // const uint32_t dstConvertedValue = 2;

  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcDefaultValue0, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, src.GetProperty1Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  EXPECT_TRUE(src.SetProperty1Value(srcNewValue));
  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcDefaultValue0, dst.GetProperty0Value());
  EXPECT_EQ(srcNewValue, src.GetProperty1Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Binding will change the target to match the source on the next execute

  auto convertingBinding = std::make_shared<Fsl::DataBinding::TwoWayConverterBinding<uint32_t, float>>(
    [](const float value) { return static_cast<uint32_t>(std::round(value)); }, [](const uint32_t value) { return static_cast<float>(value); });

  EXPECT_THROW(dst.SetBinding(UTDependencyObject::Property0, DataBinding::Binding(convertingBinding, DataBinding::BindingMode::TwoWay,
                                                                                  src.GetPropertyHandle(UTReadOnlyDependencyObject::Property1))),
               DataBinding::TwoWayBindingReadOnlySourceException);
}


TEST(Test_GetRSetConvert_TwoWay, SetBinding_DepProperty_DepProperty_SetA)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject src(dataBindingService);
  UTDependencyObject dst(dataBindingService);

  const uint32_t srcDefaultValue0 = 0;
  const float dstDefaultValue = 0;
  const float srcNewValue = 1.5f;
  // const uint32_t dstConvertedValue = 2;

  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcDefaultValue0, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, src.GetProperty1Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  EXPECT_TRUE(src.SetProperty1Value(srcNewValue));
  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcDefaultValue0, dst.GetProperty0Value());
  EXPECT_EQ(srcNewValue, src.GetProperty1Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Binding will change the target to match the source on the next execute

  auto convertingBinding = std::make_shared<Fsl::DataBinding::TwoWayConverterBinding<uint32_t, float>>(
    [](const float value) { return static_cast<uint32_t>(std::round(value)); }, [](const uint32_t value) { return static_cast<float>(value); });

  EXPECT_THROW(dst.SetBinding(UTDependencyObject::Property0, DataBinding::Binding(convertingBinding, DataBinding::BindingMode::TwoWay,
                                                                                  src.GetPropertyHandle(UTReadOnlyDependencyObject::Property1))),
               DataBinding::TwoWayBindingReadOnlySourceException);
}


TEST(Test_GetRSetConvert_TwoWay, SetBinding_DepProperty_Multi_DepProperty_DepProperty_Init)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject src0(dataBindingService);
  UTDependencyObject src1(dataBindingService);
  UTDependencyObject dst(dataBindingService);

  const uint32_t src0DefaultValue = 0;
  const uint32_t src1DefaultValue = 0;
  const float dstDefaultValue = 0;

  const float src0NewValue = static_cast<float>(0x80) / 255.0f;
  const float src1NewValue = static_cast<float>(0xBF) / 255.0f;
  // 0x80 | (0xBF << 8) = 0xBF80
  // const uint32_t dstConvertedValue = 0xBF80;

  EXPECT_EQ(src0DefaultValue, src0.GetProperty0Value());
  EXPECT_EQ(src0DefaultValue, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, src0.GetProperty1Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  EXPECT_TRUE(src0.SetProperty1Value(src0NewValue));
  EXPECT_TRUE(src1.SetProperty1Value(src1NewValue));

  EXPECT_EQ(src0DefaultValue, src0.GetProperty0Value());
  EXPECT_EQ(src1DefaultValue, src1.GetProperty0Value());
  EXPECT_EQ(src0DefaultValue, dst.GetProperty0Value());
  EXPECT_EQ(src0NewValue, src0.GetProperty1Value());
  EXPECT_EQ(src1NewValue, src1.GetProperty1Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Binding will change the target to match the source on the next execute
  auto convertingBinding = std::make_shared<Fsl::DataBinding::TwoWayMultiConverterBinding<uint32_t, float, float>>(
    [](const float src0, const float src1)
    {
      return std::clamp(static_cast<uint32_t>(std::round(src0 * 255.0f)), 0u, 255u) |
             (std::clamp(static_cast<uint32_t>(std::round(src1 * 255.0f)), 0u, 255u) << 8);
    },
    [](const uint32_t src) { return std::make_tuple(static_cast<float>(src & 0xFF) / 255.0f, static_cast<float>((src >> 8) & 0xFF) / 255.0f); });

  EXPECT_THROW(dst.SetBinding(UTDependencyObject::Property0, DataBinding::Binding(convertingBinding, DataBinding::BindingMode::TwoWay,
                                                                                  src0.GetPropertyHandle(UTReadOnlyDependencyObject::Property1),
                                                                                  src1.GetPropertyHandle(UTDependencyObject::Property1))),
               DataBinding::TwoWayBindingReadOnlySourceException);
}
