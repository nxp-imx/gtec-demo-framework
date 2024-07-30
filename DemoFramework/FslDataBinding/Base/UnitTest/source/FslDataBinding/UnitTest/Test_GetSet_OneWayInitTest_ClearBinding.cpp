/****************************************************************************************************************************************************
 * Copyright 2023-2024 NXP
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
#include <FslDataBinding/Base/DataBindingService.hpp>
#include <FslDataBinding/Base/Exceptions.hpp>
#include <fmt/format.h>
#include <memory>
#include "UTDependencyObject.hpp"
#include "UTDependencyObject2.hpp"

using namespace Fsl;

namespace
{
  using Test_GetSet_OneWayInitTest_ClearBinding = TestFixtureFslBase;
}


TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_ChangeFirstProperty1)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t1.ClearBinding(UTDependencyObject::Property0);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // Since the binding was cleared before the execute we don't expect anything to have changed
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
}


TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_ChangeFirstProperty2)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t1.ClearBinding(UTDependencyObject::Property0);

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // Since the binding was cleared before the execute we don't expect anything to have changed
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
}


TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_ChangeSecondProperty1)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  // const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  EXPECT_TRUE(t1.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t1.ClearBinding(UTDependencyObject::Property0);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // Since the binding was cleared before the execute we don't expect anything to have changed
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
}


TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_ChangeSecondProperty2)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t1.ClearBinding(UTDependencyObject::Property0);

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // Since the binding was cleared before the execute we don't expect anything to have changed
  const uint32_t expectedValue = newValue1;
  EXPECT_EQ(expectedValue, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
}


TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeFirstProperty1A)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t1.ClearBinding(UTDependencyObject::Property0);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect t0 and t1 are unchanged and t2 should be equal to t1 as its bound
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue1, t2.GetProperty0Value());
}


TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeFirstProperty1B)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.ClearBinding(UTDependencyObject::Property0);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // Since the t2 binding was cleared then "t0 and t2 should be unchanged and t1 should be equal to t0"
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());
}


TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeFirstProperty1C)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t1.ClearBinding(UTDependencyObject::Property0);
  t2.ClearBinding(UTDependencyObject::Property0);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // Since the t1 and t2 binding was cleared then "t0, t1 and t2 should be unchanged"
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());
}


TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeFirstProperty1D)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.ClearBinding(UTDependencyObject::Property0);
  t1.ClearBinding(UTDependencyObject::Property0);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // Since the t1 and t2 binding was cleared then "t0, t1 and t2 should be unchanged"
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());
}


TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeFirstProperty2A)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t1.ClearBinding(UTDependencyObject::Property0);

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());

  // We expect t0, t1 to be unchanged and t2 to be equal t1 as its bound
  const uint32_t expectedValue = newValue1;
  EXPECT_EQ(expectedValue, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue1, t2.GetProperty0Value());
}


TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeFirstProperty2B)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t2.ClearBinding(UTDependencyObject::Property0);

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect T0 to keep its value and t1 to be equal to t0, where as t2 should keep its value due to the binding being cleared before the execute
  const uint32_t expectedValue = newValue1;
  EXPECT_EQ(expectedValue, t0.GetProperty0Value());
  EXPECT_EQ(expectedValue, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());
}


TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeFirstProperty2C)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t1.ClearBinding(UTDependencyObject::Property0);
  t2.ClearBinding(UTDependencyObject::Property0);

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect everything to be unchanged since all bindings were cleared
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());
}


TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeFirstProperty2D)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t2.ClearBinding(UTDependencyObject::Property0);
  t1.ClearBinding(UTDependencyObject::Property0);

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect everything to be unchanged since all bindings were cleared
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());
}

TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeSecondProperty1A)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  EXPECT_TRUE(t1.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t1.ClearBinding(UTDependencyObject::Property0);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect t0 to keep its value and t2 will be equal to t1 as its still bound
  const uint32_t expectedValue = firstValue0;
  EXPECT_EQ(expectedValue, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
  EXPECT_EQ(newValue1, t2.GetProperty0Value());
}

TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeSecondProperty1B)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  EXPECT_TRUE(t1.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t2.ClearBinding(UTDependencyObject::Property0);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect t2 and t0 will keep their value, t1 will be equal to t0 as its still bound
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue0, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());
}

TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeSecondProperty1C)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  EXPECT_TRUE(t1.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t1.ClearBinding(UTDependencyObject::Property0);
  t2.ClearBinding(UTDependencyObject::Property0);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect everything to be unchanged as nothing is bound
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());
}

TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeSecondProperty1D)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  EXPECT_TRUE(t1.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t2.ClearBinding(UTDependencyObject::Property0);
  t1.ClearBinding(UTDependencyObject::Property0);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect the 't0 value' as the 'initial bind' is a 'directional set' in this case from t0->t1->t2
  // We expect everything to be unchanged as nothing is bound
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());
}

TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeSecondProperty2A)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t1.ClearBinding(UTDependencyObject::Property0);

  EXPECT_TRUE(t1.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect t0 and t1 to keep their value. T2 will be equal to T1 as its bound
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
  EXPECT_EQ(newValue1, t2.GetProperty0Value());
}

TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeSecondProperty2B)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t2.ClearBinding(UTDependencyObject::Property0);

  // Setting a value on a one way bound property is allowed, but the execute will overwrite it
  EXPECT_TRUE(t1.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect t0 and t2 to keep their value. T1 will be equal to T0 as its bound
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue0, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());
}

TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeSecondProperty2C)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t1.ClearBinding(UTDependencyObject::Property0);
  t2.ClearBinding(UTDependencyObject::Property0);

  EXPECT_TRUE(t1.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect nothing to have changed as there is no bindings
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());
}

TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeSecondProperty2D)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t2.ClearBinding(UTDependencyObject::Property0);
  t1.ClearBinding(UTDependencyObject::Property0);

  EXPECT_TRUE(t1.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect nothing to have changed as there is no bindings
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());
}

TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeThirdProperty1A)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  EXPECT_TRUE(t2.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(newValue1, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t1.ClearBinding(UTDependencyObject::Property0);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect T0 and T1 to be unchanged, t2 will be equal to t1 as its bound.
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue1, t2.GetProperty0Value());
}

TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeThirdProperty1B)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  EXPECT_TRUE(t2.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(newValue1, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t2.ClearBinding(UTDependencyObject::Property0);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect T0 and T2 to be unchanged, t1 will be equal to t0 as its bound.
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue0, t1.GetProperty0Value());
  EXPECT_EQ(newValue1, t2.GetProperty0Value());
}

TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeThirdProperty1C)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  EXPECT_TRUE(t2.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(newValue1, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t1.ClearBinding(UTDependencyObject::Property0);
  t2.ClearBinding(UTDependencyObject::Property0);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect nothing to have changed as there are no bindings
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(newValue1, t2.GetProperty0Value());
}

TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeThirdProperty1D)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  EXPECT_TRUE(t2.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(newValue1, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t2.ClearBinding(UTDependencyObject::Property0);
  t1.ClearBinding(UTDependencyObject::Property0);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect nothing to have changed as there are no bindings
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(newValue1, t2.GetProperty0Value());
}

TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeThirdProperty2A)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t1.ClearBinding(UTDependencyObject::Property0);

  // Setting a value on a one way bound property is allowed, but the execute will overwrite it
  EXPECT_TRUE(t2.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(newValue1, t2.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect t0 and t1 to be unchanged, t2 will be equal to t1 as its bound
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue1, t2.GetProperty0Value());
}

TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeThirdProperty2B)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t2.ClearBinding(UTDependencyObject::Property0);

  // Setting a value on a one way bound property fails
  EXPECT_TRUE(t2.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(newValue1, t2.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect t0 and t2 to be unchanged, t1 will be equal to t0 as its bound
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue0, t1.GetProperty0Value());
  EXPECT_EQ(newValue1, t2.GetProperty0Value());
}

TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeThirdProperty2C)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t1.ClearBinding(UTDependencyObject::Property0);
  t2.ClearBinding(UTDependencyObject::Property0);

  // Setting a value on a one way bound property fails
  EXPECT_TRUE(t2.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(newValue1, t2.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect nothing to have changed as nothing is bound
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(newValue1, t2.GetProperty0Value());
}

TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_DepProperty_DepProperty_ChangeThirdProperty2D)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 11;
  const uint32_t firstValue2 = 12;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_TRUE(t2.SetProperty0Value(firstValue2));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t2.ClearBinding(UTDependencyObject::Property0);
  t1.ClearBinding(UTDependencyObject::Property0);

  // Setting a value on a one way bound property fails
  EXPECT_TRUE(t2.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(newValue1, t2.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect nothing to have changed as nothing is bound
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(newValue1, t2.GetProperty0Value());
}

TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_ConstrainedDepProperty_DepProperty_ChangeFirstProperty1)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  constexpr ConstrainedValue<uint32_t> ConstraintsT0(40, 60);
  t0.SetProperty0ValueConstraints(ConstraintsT0);

  const uint32_t firstValue0 = 50;
  const uint32_t firstValue1 = 11;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(ConstraintsT0.Max(), t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t1.ClearBinding(UTDependencyObject::Property0);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect nothing to have changed as nothing is bound
  EXPECT_EQ(ConstraintsT0.Max(), t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
}


TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_ConstrainedDepProperty_DepProperty_ChangeFirstProperty2)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  constexpr ConstrainedValue<uint32_t> ConstraintsT0(40, 60);
  t0.SetProperty0ValueConstraints(ConstraintsT0);

  const uint32_t firstValue0 = 50;
  const uint32_t firstValue1 = 11;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t1.ClearBinding(UTDependencyObject::Property0);

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(ConstraintsT0.Max(), t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect nothing to have changed as nothing is bound
  EXPECT_EQ(ConstraintsT0.Max(), t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
}


TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_ConstrainedDepProperty_ChangeFirstProperty1)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  constexpr ConstrainedValue<uint32_t> ConstraintsT1(40, 60);
  t1.SetProperty0ValueConstraints(ConstraintsT1);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 51;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t1.ClearBinding(UTDependencyObject::Property0);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect nothing to have changed as nothing is bound
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
}


TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_ConstrainedDepProperty_ChangeFirstProperty2)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  constexpr ConstrainedValue<uint32_t> ConstraintsT1(40, 60);
  t1.SetProperty0ValueConstraints(ConstraintsT1);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 51;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t1.ClearBinding(UTDependencyObject::Property0);

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect nothing to have changed as nothing is bound
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
}


TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_ConstrainedDepProperty_DepProperty_ChangeSecondProperty1)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  constexpr ConstrainedValue<uint32_t> ConstraintsT0(40, 60);
  t0.SetProperty0ValueConstraints(ConstraintsT0);

  const uint32_t firstValue0 = 50;
  const uint32_t firstValue1 = 11;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  EXPECT_TRUE(t1.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t1.ClearBinding(UTDependencyObject::Property0);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect nothing to have changed as nothing is bound
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
}


TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_ConstrainedDepProperty_DepProperty_ChangeSecondProperty2)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  constexpr ConstrainedValue<uint32_t> ConstraintsT0(40, 60);
  t0.SetProperty0ValueConstraints(ConstraintsT0);

  const uint32_t firstValue0 = 50;
  const uint32_t firstValue1 = 11;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t1.ClearBinding(UTDependencyObject::Property0);

  EXPECT_TRUE(t1.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect nothing to have changed as nothing is bound
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
}


TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_ConstrainedDepProperty_ChangeSecondProperty1)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  constexpr ConstrainedValue<uint32_t> ConstraintsT1(40, 60);
  t1.SetProperty0ValueConstraints(ConstraintsT1);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 51;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  EXPECT_TRUE(t1.SetProperty0Value(newValue1));
  const uint32_t clampedNewValue1 = std::clamp(newValue1, ConstraintsT1.Min(), ConstraintsT1.Max());
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(clampedNewValue1, t1.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t1.ClearBinding(UTDependencyObject::Property0);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect nothing to have changed as nothing is bound
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(clampedNewValue1, t1.GetProperty0Value());
}


TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_DepProperty_ConstrainedDepProperty_ChangeSecondProperty2)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  constexpr ConstrainedValue<uint32_t> ConstraintsT1(40, 60);
  t1.SetProperty0ValueConstraints(ConstraintsT1);

  const uint32_t firstValue0 = 10;
  const uint32_t firstValue1 = 51;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t1.ClearBinding(UTDependencyObject::Property0);

  // Setting a value on a one way bound property fails
  EXPECT_TRUE(t1.SetProperty0Value(newValue1));
  const uint32_t clampedNewValue1 = std::clamp(newValue1, ConstraintsT1.Min(), ConstraintsT1.Max());
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(clampedNewValue1, t1.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect nothing to have changed as nothing is bound
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(clampedNewValue1, t1.GetProperty0Value());
}

TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_ConstrainedDepProperty_ConstrainedDepProperty_ChangeSecondProperty1)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  constexpr ConstrainedValue<uint32_t> ConstraintsT0(45, 60);
  constexpr ConstrainedValue<uint32_t> ConstraintsT1(40, 50);
  t0.SetProperty0ValueConstraints(ConstraintsT0);
  t1.SetProperty0ValueConstraints(ConstraintsT1);

  const uint32_t firstValue0 = 46;
  const uint32_t firstValue1 = 47;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  EXPECT_TRUE(t1.SetProperty0Value(newValue1));
  const uint32_t clampedNewValue1 = std::clamp(newValue1, ConstraintsT1.Min(), ConstraintsT1.Max());
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(clampedNewValue1, t1.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t1.ClearBinding(UTDependencyObject::Property0);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect nothing to have changed as nothing is bound
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(clampedNewValue1, t1.GetProperty0Value());
}


TEST(Test_GetSet_OneWayInitTest_ClearBinding, SetBinding_ConstrainedDepProperty_ConstrainedDepProperty_ChangeSecondProperty2)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  constexpr ConstrainedValue<uint32_t> ConstraintsT0(45, 60);
  constexpr ConstrainedValue<uint32_t> ConstraintsT1(40, 50);
  t0.SetProperty0ValueConstraints(ConstraintsT0);
  t1.SetProperty0ValueConstraints(ConstraintsT1);

  const uint32_t firstValue0 = 46;
  const uint32_t firstValue1 = 47;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::OneWay);

  t1.ClearBinding(UTDependencyObject::Property0);

  EXPECT_TRUE(t1.SetProperty0Value(newValue1));
  const uint32_t clampedNewValue1 = std::clamp(newValue1, ConstraintsT1.Min(), ConstraintsT1.Max());
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(clampedNewValue1, t1.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect nothing to have changed as nothing is bound
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(clampedNewValue1, t1.GetProperty0Value());
}
