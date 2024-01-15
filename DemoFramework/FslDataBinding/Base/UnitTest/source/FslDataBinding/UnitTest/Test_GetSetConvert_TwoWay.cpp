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

#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <FslDataBinding/Base/Bind/ConverterBinding.hpp>
#include <FslDataBinding/Base/Bind/MultiConverterBinding.hpp>
#include <FslDataBinding/Base/Bind/TwoWayConverterBinding.hpp>
#include <FslDataBinding/Base/Bind/TwoWayMultiConverterBinding.hpp>
#include <FslDataBinding/Base/Binding.hpp>
#include <FslDataBinding/Base/DataBindingService.hpp>
#include <FslDataBinding/Base/Exceptions.hpp>
#include <fmt/format.h>
#include <memory>
#include "UTDependencyObject.hpp"
#include "UTDependencyObject2.hpp"

using namespace Fsl;

namespace
{
  using Test_GetSetConvert_TwoWay = TestFixtureFslBase;
}

TEST(Test_GetSetConvert_TwoWay, ConverterBinding_OneWayConverter_BoundAsTwoWay)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();
  UTDependencyObject src(dataBindingService);

  auto convertingBinding =
    std::make_shared<Fsl::DataBinding::ConverterBinding<uint32_t, float>>([](const float value) { return static_cast<uint32_t>(std::round(value)); });

  // A one-way converter binding can not be set as a TwoWay binding
  EXPECT_THROW(DataBinding::Binding(convertingBinding, DataBinding::BindingMode::TwoWay, src.GetPropertyHandle(UTDependencyObject::Property1)),
               NotSupportedException);
}


TEST(Test_GetSetConvert_TwoWay, MultiConverterBinding_OneWayConverter_BoundAsTwoWay)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();
  UTDependencyObject src0(dataBindingService);
  UTDependencyObject src1(dataBindingService);

  auto convertingBinding = std::make_shared<Fsl::DataBinding::MultiConverterBinding<uint32_t, float, float>>(
    [](const float value0, const float value1) { return static_cast<uint32_t>(std::round(value0 + value1)); });

  // A one-way multi-converter binding can not be set as a TwoWay binding
  EXPECT_THROW(DataBinding::Binding(convertingBinding, DataBinding::BindingMode::TwoWay, src0.GetPropertyHandle(UTDependencyObject::Property1),
                                    src1.GetPropertyHandle(UTDependencyObject::Property1)),
               NotSupportedException);
}


TEST(Test_GetSetConvert_TwoWay, SetBinding_DepProperty_DepProperty_Init)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject src(dataBindingService);
  UTDependencyObject dst(dataBindingService);

  const uint32_t srcDefaultValue0 = 0;
  const float dstDefaultValue = 0;
  const float srcNewValue = 1.5f;
  const uint32_t dstConvertedValue = 2;

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

  EXPECT_TRUE(dst.SetBinding(UTDependencyObject::Property0, DataBinding::Binding(convertingBinding, DataBinding::BindingMode::TwoWay,
                                                                                 src.GetPropertyHandle(UTDependencyObject::Property1))));

  // Binding by itself does not set the value right away
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcDefaultValue0, dst.GetProperty0Value());
  EXPECT_EQ(srcNewValue, src.GetProperty1Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcNewValue, src.GetProperty1Value());
  // Examine the target
  EXPECT_EQ(dstConvertedValue, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());
}


TEST(Test_GetSetConvert_TwoWay, SetBinding_DepProperty_DepProperty_SetA)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject src(dataBindingService);
  UTDependencyObject dst(dataBindingService);

  const uint32_t srcDefaultValue0 = 0;
  const float dstDefaultValue = 0;
  const float srcNewValue = 1.5f;
  const uint32_t dstConvertedValue = 2;

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

  EXPECT_TRUE(dst.SetBinding(UTDependencyObject::Property0, DataBinding::Binding(convertingBinding, DataBinding::BindingMode::TwoWay,
                                                                                 src.GetPropertyHandle(UTDependencyObject::Property1))));


  // Binding by itself does not set the value right away
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcDefaultValue0, dst.GetProperty0Value());
  EXPECT_EQ(srcNewValue, src.GetProperty1Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcNewValue, src.GetProperty1Value());
  // Examine the target
  EXPECT_EQ(dstConvertedValue, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  const float srcNewValue2 = 2.25f;
  const uint32_t dstConvertedValue2 = 2;

  EXPECT_TRUE(src.SetProperty1Value(srcNewValue2));
  // Only the source was changed
  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcNewValue2, src.GetProperty1Value());
  EXPECT_EQ(dstConvertedValue, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  // Bound value update
  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcNewValue2, src.GetProperty1Value());
  EXPECT_EQ(dstConvertedValue2, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());
}


TEST(Test_GetSetConvert_TwoWay, SetBinding_DepProperty_DepProperty_SetB)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject src(dataBindingService);
  UTDependencyObject dst(dataBindingService);

  const uint32_t srcDefaultValue0 = 0;
  const float dstDefaultValue = 0;
  const float srcNewValue = 1.5f;
  const uint32_t dstConvertedValue = 2;

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

  EXPECT_TRUE(dst.SetBinding(UTDependencyObject::Property0, DataBinding::Binding(convertingBinding, DataBinding::BindingMode::TwoWay,
                                                                                 src.GetPropertyHandle(UTDependencyObject::Property1))));


  // Binding by itself does not set the value right away
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcDefaultValue0, dst.GetProperty0Value());
  EXPECT_EQ(srcNewValue, src.GetProperty1Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcNewValue, src.GetProperty1Value());
  // Examine the target
  EXPECT_EQ(dstConvertedValue, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  const float srcNewValue2 = 3.0;
  const uint32_t dstConvertedValue2 = 3;

  // Since this is a two way converter binding, setting the same value should actually trigger the 'bindings'
  EXPECT_TRUE(dst.SetProperty0Value(dstConvertedValue2));
  // Only the dst was changed
  EXPECT_NE(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcNewValue, src.GetProperty1Value());
  EXPECT_EQ(dstConvertedValue2, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  // Bound value update
  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcNewValue2, src.GetProperty1Value());
  EXPECT_EQ(dstConvertedValue2, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());
}


TEST(Test_GetSetConvert_TwoWay, SetBinding_DepProperty_DepProperty_SetB_DefaultBindingModeOverload)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject src(dataBindingService);
  UTDependencyObject dst(dataBindingService);

  const uint32_t srcDefaultValue0 = 0;
  const float dstDefaultValue = 0;
  const float srcNewValue = 1.5f;
  const uint32_t dstConvertedValue = 2;

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

  EXPECT_TRUE(
    dst.SetBinding(UTDependencyObject::Property0, DataBinding::Binding(convertingBinding, src.GetPropertyHandle(UTDependencyObject::Property1))));

  // Binding by itself does not set the value right away
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcDefaultValue0, dst.GetProperty0Value());
  EXPECT_EQ(srcNewValue, src.GetProperty1Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcNewValue, src.GetProperty1Value());
  // Examine the target
  EXPECT_EQ(dstConvertedValue, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  const float srcNewValue2 = 3.0;
  const uint32_t dstConvertedValue2 = 3;

  // Since this is a two way converter binding, setting the same value should actually trigger the 'bindings'
  EXPECT_TRUE(dst.SetProperty0Value(dstConvertedValue2));
  // Only the dst was changed
  EXPECT_NE(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcNewValue, src.GetProperty1Value());
  EXPECT_EQ(dstConvertedValue2, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  // Bound value update
  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcNewValue2, src.GetProperty1Value());
  EXPECT_EQ(dstConvertedValue2, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());
}

// Another possible bug covered by the below case
// - Setting values in the two way 'tree'
// - Then setting a final value on a node to its 'existing' value
// This should actually force the tree to the last 'existing' value instead of the previous different value

#ifdef LOCAL_TWO_WAY_CONVERTER_SET_TO_SAME_VALUE_DOES_NOT_TRIGGER_NEW_SET

TEST(Test_GetSetConvert_TwoWay, SetBinding_DepProperty_DepProperty_SetB_SameValue)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject src(dataBindingService);
  UTDependencyObject dst(dataBindingService);

  const uint32_t srcDefaultValue0 = 0;
  const float dstDefaultValue = 0;
  const float srcNewValue = 1.5f;
  const uint32_t dstConvertedValue = 2;

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

  EXPECT_TRUE(dst.SetBinding(UTDependencyObject::Property0, DataBinding::Binding(convertingBinding, DataBinding::BindingMode::TwoWay,
                                                                                 src.GetPropertyHandle(UTDependencyObject::Property1))));


  // Binding by itself does not set the value right away
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcDefaultValue0, dst.GetProperty0Value());
  EXPECT_EQ(srcNewValue, src.GetProperty1Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcNewValue, src.GetProperty1Value());
  // Examine the target
  EXPECT_EQ(dstConvertedValue, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Since this is a two way converter binding, setting the same value should actually trigger the 'bindings'
  EXPECT_TRUE(dst.SetProperty0Value(dstConvertedValue));
  // while the dst was set to the same value, we expect a pending change to be scheduled due to this being a two-way converter binding!
  EXPECT_NE(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcNewValue, src.GetProperty1Value());
  EXPECT_EQ(dstConvertedValue, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  const auto twoWayNewValue = static_cast<float>(dstConvertedValue);
  // Bound value update
  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(twoWayNewValue, src.GetProperty1Value());
  EXPECT_EQ(dstConvertedValue, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());
}

#endif

TEST(Test_GetSetConvert_TwoWay, SetBinding_DepProperty_Multi_DepProperty_DepProperty_Init)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject src0(dataBindingService);
  UTDependencyObject src1(dataBindingService);
  UTDependencyObject dst(dataBindingService);

  const uint32_t src0DefaultValue = 0;
  const uint32_t src1DefaultValue = 0;
  const float dstDefaultValue = 0;

  const float src0NewValue = static_cast<float>(0x80) / 255.0f;
  const float src1NewValue = static_cast<float>(0xBF) / 255.0f;
  // 0x80 | (0xBF << 8) = 0xBF80
  const uint32_t dstConvertedValue = 0xBF80;

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

  EXPECT_TRUE(dst.SetBinding(UTDependencyObject::Property0, DataBinding::Binding(convertingBinding, DataBinding::BindingMode::TwoWay,
                                                                                 src0.GetPropertyHandle(UTDependencyObject::Property1),
                                                                                 src1.GetPropertyHandle(UTDependencyObject::Property1))));

  // Binding by itself does not set the value right away
  EXPECT_EQ(2u, dataBindingService->PendingChanges());
  EXPECT_EQ(src0DefaultValue, src0.GetProperty0Value());
  EXPECT_EQ(src1DefaultValue, src1.GetProperty0Value());
  EXPECT_EQ(src0DefaultValue, dst.GetProperty0Value());
  EXPECT_EQ(src0NewValue, src0.GetProperty1Value());
  EXPECT_EQ(src1NewValue, src1.GetProperty1Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(src0DefaultValue, src0.GetProperty0Value());
  EXPECT_FLOAT_EQ(src0NewValue, src0.GetProperty1Value());
  EXPECT_FLOAT_EQ(src1NewValue, src1.GetProperty1Value());
  // Examine the target
  EXPECT_EQ(dstConvertedValue, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());
}

TEST(Test_GetSetConvert_TwoWay, SetBinding_DepProperty_Multi_DepProperty_DepProperty_Set_Src0)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject src0(dataBindingService);
  UTDependencyObject src1(dataBindingService);
  UTDependencyObject dst(dataBindingService);

  const uint32_t src0DefaultValue = 0;
  const uint32_t src1DefaultValue = 0;
  const float dstDefaultValue = 0;

  const float src0NewValue = static_cast<float>(0x80) / 255.0f;
  const float src1NewValue = static_cast<float>(0xBF) / 255.0f;
  // 0x80 | (0xBF << 8) = 0xBF80
  const uint32_t dstConvertedValue = 0xBF80;

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

  EXPECT_TRUE(dst.SetBinding(UTDependencyObject::Property0, DataBinding::Binding(convertingBinding, DataBinding::BindingMode::TwoWay,
                                                                                 src0.GetPropertyHandle(UTDependencyObject::Property1),
                                                                                 src1.GetPropertyHandle(UTDependencyObject::Property1))));

  // Binding by itself does not set the value right away
  EXPECT_EQ(2u, dataBindingService->PendingChanges());
  EXPECT_EQ(src0DefaultValue, src0.GetProperty0Value());
  EXPECT_EQ(src1DefaultValue, src1.GetProperty0Value());
  EXPECT_EQ(src0DefaultValue, dst.GetProperty0Value());
  EXPECT_EQ(src0NewValue, src0.GetProperty1Value());
  EXPECT_EQ(src1NewValue, src1.GetProperty1Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(src0DefaultValue, src0.GetProperty0Value());
  EXPECT_FLOAT_EQ(src0NewValue, src0.GetProperty1Value());
  EXPECT_FLOAT_EQ(src1NewValue, src1.GetProperty1Value());
  // Examine the target
  EXPECT_EQ(dstConvertedValue, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  const float src0NewValue2 = static_cast<float>(0x22) / 255.0f;
  const uint32_t dstConvertedValue2 = (dstConvertedValue & 0xFF00) | std::clamp(static_cast<uint32_t>(std::round(src0NewValue2 * 255.0f)), 0u, 255u);

  EXPECT_TRUE(src0.SetProperty1Value(src0NewValue2));
  // Only the src0 has changed
  EXPECT_FLOAT_EQ(src0NewValue2, src0.GetProperty1Value());
  EXPECT_FLOAT_EQ(src1NewValue, src1.GetProperty1Value());
  EXPECT_EQ(dstConvertedValue, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(src0DefaultValue, src0.GetProperty0Value());
  EXPECT_FLOAT_EQ(src0NewValue2, src0.GetProperty1Value());
  EXPECT_FLOAT_EQ(src1NewValue, src1.GetProperty1Value());
  EXPECT_EQ(dstConvertedValue2, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());
}

TEST(Test_GetSetConvert_TwoWay, SetBinding_DepProperty_Multi_DepProperty_DepProperty_Set_Src1)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject src0(dataBindingService);
  UTDependencyObject src1(dataBindingService);
  UTDependencyObject dst(dataBindingService);

  const uint32_t src0DefaultValue = 0;
  const uint32_t src1DefaultValue = 0;
  const float dstDefaultValue = 0;

  const float src0NewValue = static_cast<float>(0x80) / 255.0f;
  const float src1NewValue = static_cast<float>(0xBF) / 255.0f;
  // 0x80 | (0xBF << 8) = 0xBF80
  const uint32_t dstConvertedValue = 0xBF80;

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

  EXPECT_TRUE(dst.SetBinding(UTDependencyObject::Property0, DataBinding::Binding(convertingBinding, DataBinding::BindingMode::TwoWay,
                                                                                 src0.GetPropertyHandle(UTDependencyObject::Property1),
                                                                                 src1.GetPropertyHandle(UTDependencyObject::Property1))));

  // Binding by itself does not set the value right away
  EXPECT_EQ(2u, dataBindingService->PendingChanges());
  EXPECT_EQ(src0DefaultValue, src0.GetProperty0Value());
  EXPECT_EQ(src1DefaultValue, src1.GetProperty0Value());
  EXPECT_EQ(src0DefaultValue, dst.GetProperty0Value());
  EXPECT_EQ(src0NewValue, src0.GetProperty1Value());
  EXPECT_EQ(src1NewValue, src1.GetProperty1Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(src0DefaultValue, src0.GetProperty0Value());
  EXPECT_FLOAT_EQ(src0NewValue, src0.GetProperty1Value());
  EXPECT_FLOAT_EQ(src1NewValue, src1.GetProperty1Value());
  // Examine the target
  EXPECT_EQ(dstConvertedValue, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  const float src1NewValue2 = static_cast<float>(0x22) / 255.0f;
  const uint32_t dstConvertedValue2 =
    (dstConvertedValue & 0xFF) | (std::clamp(static_cast<uint32_t>(std::round(src1NewValue2 * 255.0f)), 0u, 255u) << 8);

  EXPECT_TRUE(src1.SetProperty1Value(src1NewValue2));
  // Only the ssrc1 has changed
  EXPECT_FLOAT_EQ(src0NewValue, src0.GetProperty1Value());
  EXPECT_FLOAT_EQ(src1NewValue2, src1.GetProperty1Value());
  EXPECT_EQ(dstConvertedValue, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(src0DefaultValue, src0.GetProperty0Value());
  EXPECT_FLOAT_EQ(src0NewValue, src0.GetProperty1Value());
  EXPECT_FLOAT_EQ(src1NewValue2, src1.GetProperty1Value());
  EXPECT_EQ(dstConvertedValue2, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());
}

TEST(Test_GetSetConvert_TwoWay, SetBinding_DepProperty_Multi_DepProperty_DepProperty_Set_Src0AndSrc1)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject src0(dataBindingService);
  UTDependencyObject src1(dataBindingService);
  UTDependencyObject dst(dataBindingService);

  const uint32_t src0DefaultValue = 0;
  const uint32_t src1DefaultValue = 0;
  const float dstDefaultValue = 0;

  const float src0NewValue = static_cast<float>(0x80) / 255.0f;
  const float src1NewValue = static_cast<float>(0xBF) / 255.0f;
  // 0x80 | (0xBF << 8) = 0xBF80
  const uint32_t dstConvertedValue = 0xBF80;

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

  EXPECT_TRUE(dst.SetBinding(UTDependencyObject::Property0, DataBinding::Binding(convertingBinding, DataBinding::BindingMode::TwoWay,
                                                                                 src0.GetPropertyHandle(UTDependencyObject::Property1),
                                                                                 src1.GetPropertyHandle(UTDependencyObject::Property1))));

  // Binding by itself does not set the value right away
  EXPECT_EQ(2u, dataBindingService->PendingChanges());
  EXPECT_EQ(src0DefaultValue, src0.GetProperty0Value());
  EXPECT_EQ(src1DefaultValue, src1.GetProperty0Value());
  EXPECT_EQ(src0DefaultValue, dst.GetProperty0Value());
  EXPECT_EQ(src0NewValue, src0.GetProperty1Value());
  EXPECT_EQ(src1NewValue, src1.GetProperty1Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(src0DefaultValue, src0.GetProperty0Value());
  EXPECT_FLOAT_EQ(src0NewValue, src0.GetProperty1Value());
  EXPECT_FLOAT_EQ(src1NewValue, src1.GetProperty1Value());
  // Examine the target
  EXPECT_EQ(dstConvertedValue, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());


  const float src0NewValue2 = static_cast<float>(0x92) / 255.0f;
  const float src1NewValue2 = static_cast<float>(0x22) / 255.0f;
  const uint32_t dstConvertedValue2 = std::clamp(static_cast<uint32_t>(std::round(src0NewValue2 * 255.0f)), 0u, 255u) |
                                      (std::clamp(static_cast<uint32_t>(std::round(src1NewValue2 * 255.0f)), 0u, 255u) << 8);

  EXPECT_TRUE(src0.SetProperty1Value(src0NewValue2));
  EXPECT_TRUE(src1.SetProperty1Value(src1NewValue2));
  // Only the src0+src1 has changed
  EXPECT_FLOAT_EQ(src0NewValue2, src0.GetProperty1Value());
  EXPECT_FLOAT_EQ(src1NewValue2, src1.GetProperty1Value());
  EXPECT_EQ(dstConvertedValue, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(src0DefaultValue, src0.GetProperty0Value());
  EXPECT_FLOAT_EQ(src0NewValue2, src0.GetProperty1Value());
  EXPECT_FLOAT_EQ(src1NewValue2, src1.GetProperty1Value());
  EXPECT_EQ(dstConvertedValue2, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());
}


TEST(Test_GetSetConvert_TwoWay, SetBinding_DepProperty_Multi_DepProperty_DepProperty_Set_Dst)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject src0(dataBindingService);
  UTDependencyObject src1(dataBindingService);
  UTDependencyObject dst(dataBindingService);

  const uint32_t src0DefaultValue = 0;
  const uint32_t src1DefaultValue = 0;
  const float dstDefaultValue = 0;

  const float src0NewValue = static_cast<float>(0x80) / 255.0f;
  const float src1NewValue = static_cast<float>(0xBF) / 255.0f;
  // 0x80 | (0xBF << 8) = 0xBF80
  const uint32_t dstConvertedValue = 0xBF80;

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

  EXPECT_TRUE(dst.SetBinding(UTDependencyObject::Property0, DataBinding::Binding(convertingBinding, DataBinding::BindingMode::TwoWay,
                                                                                 src0.GetPropertyHandle(UTDependencyObject::Property1),
                                                                                 src1.GetPropertyHandle(UTDependencyObject::Property1))));

  // Binding by itself does not set the value right away
  EXPECT_EQ(2u, dataBindingService->PendingChanges());
  EXPECT_EQ(src0DefaultValue, src0.GetProperty0Value());
  EXPECT_EQ(src1DefaultValue, src1.GetProperty0Value());
  EXPECT_EQ(src0DefaultValue, dst.GetProperty0Value());
  EXPECT_EQ(src0NewValue, src0.GetProperty1Value());
  EXPECT_EQ(src1NewValue, src1.GetProperty1Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(src0DefaultValue, src0.GetProperty0Value());
  EXPECT_FLOAT_EQ(src0NewValue, src0.GetProperty1Value());
  EXPECT_FLOAT_EQ(src1NewValue, src1.GetProperty1Value());
  // Examine the target
  EXPECT_EQ(dstConvertedValue, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());


  const uint32_t dstConvertedValue2 = 0x1234;
  const float src0NewValue2 = static_cast<float>(dstConvertedValue2 & 0xFF) / 255.0f;
  const float src1NewValue2 = static_cast<float>((dstConvertedValue2 >> 8) & 0xFF) / 255.0f;

  EXPECT_TRUE(dst.SetProperty0Value(dstConvertedValue2));
  // Only the src0+src1 has changed
  EXPECT_FLOAT_EQ(src0NewValue, src0.GetProperty1Value());
  EXPECT_FLOAT_EQ(src1NewValue, src1.GetProperty1Value());
  EXPECT_EQ(dstConvertedValue2, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(src0DefaultValue, src0.GetProperty0Value());
  EXPECT_FLOAT_EQ(src0NewValue2, src0.GetProperty1Value());
  EXPECT_FLOAT_EQ(src1NewValue2, src1.GetProperty1Value());
  EXPECT_EQ(dstConvertedValue2, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());
}
