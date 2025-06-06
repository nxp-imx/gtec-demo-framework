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
#include <algorithm>
#include <memory>
#include "UTDependencyObject.hpp"
#include "UTDependencyObject2.hpp"

using namespace Fsl;

namespace
{
  using Test_GetSet_TwoWayInitTest = TestFixtureFslBase;
}


TEST(Test_GetSet_TwoWayInitTest, SetBinding_DepProperty_DepProperty_ChangeFirstProperty1)
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

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect the 't0 value' as the 'initial bind' is a 'directional set' in this case from t0->t1
  const uint32_t expectedValue = newValue1;
  EXPECT_EQ(expectedValue, t0.GetProperty0Value());
  EXPECT_EQ(expectedValue, t1.GetProperty0Value());
}


TEST(Test_GetSet_TwoWayInitTest, SetBinding_DepProperty_DepProperty_ChangeFirstProperty2)
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

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect the 't0 value' as the set occurs after the bind
  const uint32_t expectedValue = newValue1;
  EXPECT_EQ(expectedValue, t0.GetProperty0Value());
  EXPECT_EQ(expectedValue, t1.GetProperty0Value());
}


TEST(Test_GetSet_TwoWayInitTest, SetBinding_DepProperty_DepProperty_ChangeSecondProperty1)
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

  EXPECT_TRUE(t1.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect the 't0 value' as the 'initial bind' is a 'directional set' in this case from t0->t1
  const uint32_t expectedValue = firstValue0;
  EXPECT_EQ(expectedValue, t0.GetProperty0Value());
  EXPECT_EQ(expectedValue, t1.GetProperty0Value());
}


TEST(Test_GetSet_TwoWayInitTest, SetBinding_DepProperty_DepProperty_ChangeSecondProperty2)
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

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  EXPECT_TRUE(t1.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect the 't1 value' as the set occurs after the bind
  const uint32_t expectedValue = newValue1;
  EXPECT_EQ(expectedValue, t0.GetProperty0Value());
  EXPECT_EQ(expectedValue, t1.GetProperty0Value());
}


TEST(Test_GetSet_TwoWayInitTest, SetBinding_DepProperty_DepProperty_DepProperty_ChangeFirstProperty1)
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

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect the 't0 value' as the 'initial bind' is a 'directional set' in this case from t0->t1->t2
  const uint32_t expectedValue = newValue1;
  EXPECT_EQ(expectedValue, t0.GetProperty0Value());
  EXPECT_EQ(expectedValue, t1.GetProperty0Value());
  EXPECT_EQ(expectedValue, t2.GetProperty0Value());
}


TEST(Test_GetSet_TwoWayInitTest, SetBinding_DepProperty_DepProperty_DepProperty_ChangeFirstProperty2)
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

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect the 't0 value' as the set occurs after the bind
  const uint32_t expectedValue = newValue1;
  EXPECT_EQ(expectedValue, t0.GetProperty0Value());
  EXPECT_EQ(expectedValue, t1.GetProperty0Value());
  EXPECT_EQ(expectedValue, t2.GetProperty0Value());
}


TEST(Test_GetSet_TwoWayInitTest, SetBinding_DepProperty_DepProperty_DepProperty_ChangeSecondProperty1)
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

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect the 't0 value' as the 'initial bind' is a 'directional set' in this case from t0->t1->t2
  const uint32_t expectedValue = firstValue0;
  EXPECT_EQ(expectedValue, t0.GetProperty0Value());
  EXPECT_EQ(expectedValue, t1.GetProperty0Value());
  EXPECT_EQ(expectedValue, t2.GetProperty0Value());
}


TEST(Test_GetSet_TwoWayInitTest, SetBinding_DepProperty_DepProperty_DepProperty_ChangeSecondProperty2)
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

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  EXPECT_TRUE(t1.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
  EXPECT_EQ(firstValue2, t2.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect the 't1 value' as the set occurs after the bind
  const uint32_t expectedValue = newValue1;
  EXPECT_EQ(expectedValue, t0.GetProperty0Value());
  EXPECT_EQ(expectedValue, t1.GetProperty0Value());
  EXPECT_EQ(expectedValue, t2.GetProperty0Value());
}

TEST(Test_GetSet_TwoWayInitTest, SetBinding_DepProperty_DepProperty_DepProperty_ChangeThirdProperty1)
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

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect the 't0 value' as the 'initial bind' is a 'directional set' in this case from t0->t1->t2
  const uint32_t expectedValue = firstValue0;
  EXPECT_EQ(expectedValue, t0.GetProperty0Value());
  EXPECT_EQ(expectedValue, t1.GetProperty0Value());
  EXPECT_EQ(expectedValue, t2.GetProperty0Value());
}

TEST(Test_GetSet_TwoWayInitTest, SetBinding_DepProperty_DepProperty_DepProperty_ChangeThirdProperty2)
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

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  EXPECT_TRUE(t2.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
  EXPECT_EQ(newValue1, t2.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect the 't2 value' as the set occurs after the bind
  const uint32_t expectedValue = newValue1;
  EXPECT_EQ(expectedValue, t0.GetProperty0Value());
  EXPECT_EQ(expectedValue, t1.GetProperty0Value());
  EXPECT_EQ(expectedValue, t2.GetProperty0Value());
}


TEST(Test_GetSet_TwoWayInitTest, SetBinding_ConstrainedDepProperty_DepProperty_ChangeFirstProperty1)
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

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect the 't0 value' as the 'initial bind' is a 'directional set' in this case from t0->t1
  const uint32_t expectedValue = ConstraintsT0.Max();
  EXPECT_EQ(expectedValue, t0.GetProperty0Value());
  EXPECT_EQ(expectedValue, t1.GetProperty0Value());
}


TEST(Test_GetSet_TwoWayInitTest, SetBinding_ConstrainedDepProperty_DepProperty_ChangeFirstProperty2)
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

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(ConstraintsT0.Max(), t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect the 't0 value' as the set occurs after the bind
  const uint32_t expectedValue = ConstraintsT0.Max();
  EXPECT_EQ(expectedValue, t0.GetProperty0Value());
  EXPECT_EQ(expectedValue, t1.GetProperty0Value());
}


TEST(Test_GetSet_TwoWayInitTest, SetBinding_DepProperty_ConstrainedDepProperty_ChangeFirstProperty1)
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

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect the 't0 value' as the 'initial bind' is a 'directional set' in this case from t0->t1 and
  // constraints are not back propagated. It is up to the user to ensure the constraints are correct on all two way bound properties
  const uint32_t expectedValue = newValue1;
  EXPECT_EQ(expectedValue, t0.GetProperty0Value());
  EXPECT_EQ(std::clamp(expectedValue, ConstraintsT1.Min(), ConstraintsT1.Max()), t1.GetProperty0Value());
}


TEST(Test_GetSet_TwoWayInitTest, SetBinding_DepProperty_ConstrainedDepProperty_ChangeFirstProperty2)
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

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect the 't0 value' as the set occurs after the bind
  const uint32_t expectedValue = newValue1;
  EXPECT_EQ(expectedValue, t0.GetProperty0Value());
  EXPECT_EQ(std::clamp(expectedValue, ConstraintsT1.Min(), ConstraintsT1.Max()), t1.GetProperty0Value());
}


TEST(Test_GetSet_TwoWayInitTest, SetBinding_ConstrainedDepProperty_DepProperty_ChangeSecondProperty1)
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

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect the 't0 value' as the 'initial bind' is a 'directional set' in this case from t0->t1 (which overrides the set of T1 before the bind)
  const uint32_t expectedValue = firstValue0;
  EXPECT_EQ(expectedValue, t0.GetProperty0Value());
  EXPECT_EQ(expectedValue, t1.GetProperty0Value());
}


TEST(Test_GetSet_TwoWayInitTest, SetBinding_ConstrainedDepProperty_DepProperty_ChangeSecondProperty2)
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

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  EXPECT_TRUE(t1.SetProperty0Value(newValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());

  // We expect the 't1 value' as the set occurs after the bind and constraints are not back propagated (which is why t0 is smaller than t1)
  const uint32_t expectedValue = newValue1;
  EXPECT_EQ(std::clamp(expectedValue, ConstraintsT0.Min(), ConstraintsT0.Max()), t0.GetProperty0Value());
  EXPECT_EQ(expectedValue, t1.GetProperty0Value());
}


TEST(Test_GetSet_TwoWayInitTest, SetBinding_DepProperty_ConstrainedDepProperty_ChangeSecondProperty1)
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

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect the 't0 value' as the 'initial bind' is a 'directional set' in this case from t0->t1 (which overrides the set of T1 before the bind)
  const uint32_t expectedValue = firstValue0;
  EXPECT_EQ(expectedValue, t0.GetProperty0Value());
  EXPECT_EQ(std::clamp(expectedValue, ConstraintsT1.Min(), ConstraintsT1.Max()), t1.GetProperty0Value());
}


TEST(Test_GetSet_TwoWayInitTest, SetBinding_DepProperty_ConstrainedDepProperty_ChangeSecondProperty2)
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

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  EXPECT_TRUE(t1.SetProperty0Value(newValue1));
  const uint32_t clampedNewValue1 = std::clamp(newValue1, ConstraintsT1.Min(), ConstraintsT1.Max());
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(clampedNewValue1, t1.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect the 't1 value' as the set occurs after the bind
  const uint32_t expectedValue = clampedNewValue1;
  EXPECT_EQ(expectedValue, t0.GetProperty0Value());
  EXPECT_EQ(std::clamp(expectedValue, ConstraintsT1.Min(), ConstraintsT1.Max()), t1.GetProperty0Value());
}

TEST(Test_GetSet_TwoWayInitTest, SetBinding_ConstrainedDepProperty_ConstrainedDepProperty_ChangeSecondProperty1)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  constexpr ConstrainedValue<uint32_t> ConstraintsT0(45, 70);
  constexpr ConstrainedValue<uint32_t> ConstraintsT1(40, 60);
  t0.SetProperty0ValueConstraints(ConstraintsT0);
  t1.SetProperty0ValueConstraints(ConstraintsT1);

  const uint32_t firstValue0 = 50;
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

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect the 't0 value' as the 'initial bind' is a 'directional set' in this case from t0->t1 (which overrides the set of T1 before the bind)
  const uint32_t expectedValue = firstValue0;
  EXPECT_EQ(std::clamp(expectedValue, ConstraintsT0.Min(), ConstraintsT0.Max()), t0.GetProperty0Value());
  EXPECT_EQ(std::clamp(expectedValue, ConstraintsT1.Min(), ConstraintsT1.Max()), t1.GetProperty0Value());
}


TEST(Test_GetSet_TwoWayInitTest, SetBinding_ConstrainedDepProperty_ConstrainedDepProperty_ChangeSecondProperty2)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  constexpr ConstrainedValue<uint32_t> ConstraintsT0(45, 70);
  constexpr ConstrainedValue<uint32_t> ConstraintsT1(40, 60);
  t0.SetProperty0ValueConstraints(ConstraintsT0);
  t1.SetProperty0ValueConstraints(ConstraintsT1);

  const uint32_t firstValue0 = 50;
  const uint32_t firstValue1 = 51;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTDependencyObject::Property0), DataBinding::BindingMode::TwoWay);

  EXPECT_TRUE(t1.SetProperty0Value(newValue1));
  const uint32_t clampedNewValue1 = std::clamp(newValue1, ConstraintsT1.Min(), ConstraintsT1.Max());
  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(clampedNewValue1, t1.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  // We expect the 't1 value' as the set occurs after the bind
  const uint32_t expectedValue = clampedNewValue1;
  EXPECT_EQ(std::clamp(expectedValue, ConstraintsT0.Min(), ConstraintsT0.Max()), t0.GetProperty0Value());
  EXPECT_EQ(std::clamp(expectedValue, ConstraintsT1.Min(), ConstraintsT1.Max()), t1.GetProperty0Value());
}
