/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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

#include <FslBase/IO/Path.hpp>
#include <FslBase/IO/PathView.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBaseContent.hpp>
#include <FslDataBinding/Base/Bind/ConverterBinding.hpp>
#include <FslDataBinding/Base/Bind/MultiConverterBinding.hpp>
#include <FslDataBinding/Base/DataBindingService.hpp>
#include <FslDataBinding/Base/DataBindingServiceDebug.hpp>
#include <FslDataBinding/Base/Exceptions.hpp>
#include <fmt/format.h>
#include <memory>
#include "UTDependencyObject.hpp"
#include "UTDependencyObject2.hpp"

using namespace Fsl;

namespace
{
  class Test_DataBindingServiceDebug : public TestFixtureFslBaseContent
  {
  };
}

TEST_F(Test_DataBindingServiceDebug, SetBinding_OneWay_DepProperty_DepProperty)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  const uint32_t defaultValue = 0;
  const uint32_t newValue1 = 100;

  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  t0.SetProperty0Value(newValue1);
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  // Binding will change the target to match the source on the next execute
  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0));

  // Binding by itself does not set the value right away
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());

  auto res = DataBinding::DataBindingServiceDebug::TryToDotFormatString(*dataBindingService);
  EXPECT_TRUE(res.has_value());

  EXPECT_TRUE(DataBinding::DataBindingServiceDebug::TryToDotFormatFile(GetTestPath(IO::PathView("DataBinding_OneWay_DepProperty_DepProperty.dot")),
                                                                       *dataBindingService));
}


TEST_F(Test_DataBindingServiceDebug, SetBinding_TwoWay_DepProperty_DepProperty)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  const uint32_t defaultValue = 0;
  const uint32_t newValue1 = 100;

  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  t0.SetProperty0Value(newValue1);
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  // Binding will change the target to match the source on the next execute
  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  // Binding by itself does not set the value right away
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());

  auto res = DataBinding::DataBindingServiceDebug::TryToDotFormatString(*dataBindingService);
  EXPECT_TRUE(res.has_value());

  EXPECT_TRUE(DataBinding::DataBindingServiceDebug::TryToDotFormatFile(GetTestPath(IO::PathView("DataBinding_TwoWay_DepProperty_DepProperty.dot")),
                                                                       *dataBindingService));
}


TEST_F(Test_DataBindingServiceDebug, SetBinding_OneWay_Convert_DepProperty_DepProperty)
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

  src.SetProperty1Value(srcNewValue);
  EXPECT_EQ(srcDefaultValue0, src.GetProperty0Value());
  EXPECT_EQ(srcDefaultValue0, dst.GetProperty0Value());
  EXPECT_EQ(srcNewValue, src.GetProperty1Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Binding will change the target to match the source on the next execute

  auto convertingBinding =
    std::make_shared<Fsl::DataBinding::ConverterBinding<uint32_t, float>>([](const float value) { return static_cast<uint32_t>(std::round(value)); });

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

  auto res = DataBinding::DataBindingServiceDebug::TryToDotFormatString(*dataBindingService);
  EXPECT_TRUE(res.has_value());

  EXPECT_TRUE(DataBinding::DataBindingServiceDebug::TryToDotFormatFile(
    GetTestPath(IO::PathView("DataBinding_OneWay_Convert_DepProperty_DepProperty.dot")), *dataBindingService));
}


TEST_F(Test_DataBindingServiceDebug, SetBinding_OneWay_Convert_DepProperty_Multi_DepProperty_DepProperty)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject src0(dataBindingService);
  UTDependencyObject src1(dataBindingService);
  UTDependencyObject dst(dataBindingService);

  const uint32_t src0DefaultValue = 0;
  const uint32_t src1DefaultValue = 0;
  const float dstDefaultValue = 0;

  const float src0NewValue = 1.7f;
  const float src1NewValue = 1.9f;
  const uint32_t dstConvertedValue = 4;    // 1.7f + 1.9f = 3.6 rounded -> 4

  EXPECT_EQ(src0DefaultValue, src0.GetProperty0Value());
  EXPECT_EQ(src0DefaultValue, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, src0.GetProperty1Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  src0.SetProperty1Value(src0NewValue);
  src1.SetProperty1Value(src1NewValue);

  EXPECT_EQ(src0DefaultValue, src0.GetProperty0Value());
  EXPECT_EQ(src1DefaultValue, src1.GetProperty0Value());
  EXPECT_EQ(src0DefaultValue, dst.GetProperty0Value());
  EXPECT_EQ(src0NewValue, src0.GetProperty1Value());
  EXPECT_EQ(src1NewValue, src1.GetProperty1Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  // Binding will change the target to match the source on the next execute

  auto convertingBinding = std::make_shared<Fsl::DataBinding::MultiConverterBinding<uint32_t, float, float>>(
    [](const float value0, const float value1) { return static_cast<uint32_t>(std::round(value0 + value1)); });

  EXPECT_TRUE(
    dst.SetBinding(UTDependencyObject::Property0, DataBinding::Binding(convertingBinding, src0.GetPropertyHandle(UTDependencyObject::Property1),
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
  EXPECT_EQ(src0NewValue, src0.GetProperty1Value());
  EXPECT_EQ(src1NewValue, src1.GetProperty1Value());
  // Examine the target
  EXPECT_EQ(dstConvertedValue, dst.GetProperty0Value());
  EXPECT_EQ(dstDefaultValue, dst.GetProperty1Value());

  auto res = DataBinding::DataBindingServiceDebug::TryToDotFormatString(*dataBindingService);
  EXPECT_TRUE(res.has_value());

  EXPECT_TRUE(DataBinding::DataBindingServiceDebug::TryToDotFormatFile(
    GetTestPath(IO::PathView("DataBinding_OneWay_Convert_DepProperty_Multi_DepProperty_DepProperty.dot")), *dataBindingService));
}
