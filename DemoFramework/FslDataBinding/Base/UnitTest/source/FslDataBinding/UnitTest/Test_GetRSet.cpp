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
#include <FslDataBinding/Base/DataBindingService.hpp>
#include <FslDataBinding/Base/Exceptions.hpp>
#include <fmt/format.h>
#include <memory>
#include "UTDependencyObject.hpp"
#include "UTDependencyObject2.hpp"
#include "UTReadOnlyDependencyObject.hpp"

using namespace Fsl;

namespace
{
  using Test_GetRSet = TestFixtureFslBase;
}


TEST(Test_GetRSet, OnDemandInstancesConstruct)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  EXPECT_EQ(0u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTReadOnlyDependencyObject t1(dataBindingService);

  EXPECT_EQ(0u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());
}


TEST(Test_GetRSet, OnDemandInstancesBind)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  EXPECT_EQ(0u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));

  EXPECT_EQ(4u, dataBindingService->InstanceCount());
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
}


TEST(Test_GetRSet, SetBinding_OneWay_DepProperty_DepProperty)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  const uint32_t defaultValue = 0;
  const uint32_t newValue1 = 100;

  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  // Binding will change the target to match the source on the next execute
  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));

  // Binding by itself does not set the value right away
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
}

// FIX: look into this
TEST(Test_GetRSet, SetBinding_OneWay_DepProperty_ClearWithInvalidDepProperty_NoBinding)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);

  const uint32_t defaultValue = 0;

  EXPECT_EQ(defaultValue, t0.GetProperty0Value());

  // Binding will change the target to match the source on the next execute (this basically clears the binding)
  t0.SetBinding(UTReadOnlyDependencyObject::Property0, DataBinding::DataBindingInstanceHandle());

  EXPECT_EQ(defaultValue, t0.GetProperty0Value());

  const uint32_t newValue1 = 100;
  EXPECT_TRUE(t0.SetProperty0Value(newValue1));

  EXPECT_EQ(newValue1, t0.GetProperty0Value());
}


TEST(Test_GetRSet, SetBinding_OneWay_DepProperty_ClearWithInvalidDepProperty_ExistingBinding0)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTReadOnlyDependencyObject t1(dataBindingService);

  const uint32_t firstValue0 = 1;
  const uint32_t firstValue1 = 2;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));

  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  t0.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));
  // Binding will change the target to match the source on the next execute (this basically clears the binding)
  t0.SetBinding(UTDependencyObject::Property0, DataBinding::DataBindingInstanceHandle());

  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  const uint32_t newValue1 = 100;
  EXPECT_TRUE(t0.SetProperty0Value(newValue1));

  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
}

TEST(Test_GetRSet, SetBinding_OneWay_DepProperty_ClearWithInvalidDepProperty_ExistingBinding1)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTReadOnlyDependencyObject t1(dataBindingService);

  const uint32_t firstValue0 = 1;
  const uint32_t firstValue1 = 2;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));

  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  t0.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));
  // Binding will change the target to match the source on the next execute (this basically clears the binding)
  t0.SetBinding(UTDependencyObject::Property0, DataBinding::DataBindingInstanceHandle());

  dataBindingService->ExecuteChanges();

  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  const uint32_t newValue1 = 100;
  EXPECT_TRUE(t0.SetProperty0Value(newValue1));

  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
}


TEST(Test_GetRSet, SetBinding_OneWay_DepProperty_ClearWithInvalidDepProperty_ExistingBinding2)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTReadOnlyDependencyObject t1(dataBindingService);

  const uint32_t firstValue0 = 1;
  const uint32_t firstValue1 = 2;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));

  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  t0.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));
  // Binding will change the target to match the source on the next execute (this basically clears the binding)
  t0.SetBinding(UTDependencyObject::Property0, DataBinding::DataBindingInstanceHandle());

  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  dataBindingService->ExecuteChanges();

  const uint32_t newValue1 = 100;
  EXPECT_TRUE(t0.SetProperty0Value(newValue1));

  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
}


TEST(Test_GetRSet, SetBinding_OneWay_DepProperty_ClearWithInvalidDepProperty_ExistingBinding3)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTDependencyObject t0(dataBindingService);
  UTReadOnlyDependencyObject t1(dataBindingService);

  const uint32_t firstValue0 = 1;
  const uint32_t firstValue1 = 2;

  EXPECT_TRUE(t0.SetProperty0Value(firstValue0));
  EXPECT_TRUE(t1.SetProperty0Value(firstValue1));

  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  t0.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));
  // Binding will change the target to match the source on the next execute (this basically clears the binding)
  t0.SetBinding(UTDependencyObject::Property0, DataBinding::DataBindingInstanceHandle());

  EXPECT_EQ(firstValue0, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  const uint32_t newValue1 = 100;
  EXPECT_TRUE(t0.SetProperty0Value(newValue1));

  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());

  dataBindingService->ExecuteChanges();

  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(firstValue1, t1.GetProperty0Value());
}


TEST(Test_GetRSet, SetBinding_OneWay_DepProperty_DepProperty_DifferentTypes)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject2 t1(dataBindingService);

  const uint32_t defaultValue = 0;
  const uint32_t newValue1 = 100;

  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  // Binding will change the target to match the source on the next execute
  t1.SetBinding(UTDependencyObject2::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));

  // Binding by itself does not set the value right away
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
}


TEST(Test_GetRSet, SetBinding_OneWay_DepProperty_DepProperty_DifferentTypes2)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject2 t1(dataBindingService);

  const uint32_t defaultValue = 0;
  const uint32_t newValue1 = 100;

  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  // Binding will change the target to match the source on the next execute
  t1.SetBinding(UTDependencyObject2::Property2, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));

  // Binding by itself does not set the value right away
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty2Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty2Value());
}


TEST(Test_GetRSet, SetBinding_OneWay_DepProperty_DepProperty_IncompatibleTypes)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  const uint32_t defaultValue = 0;
  const uint32_t newValue1 = 100;

  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  // Since the properties are of different types and we dont supply a conversion this will fail
  EXPECT_THROW(t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property1)),
               DataBinding::BindingIncompatibleTypesException);
}


// FIX: look into this
TEST(Test_GetRSet, SetBinding_OneWay_DepProperty_DepProperty_CircularSelf)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  const uint32_t defaultValue = 0;
  const uint32_t newValue1 = 100;

  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  dataBindingService->ExecuteChanges();

  // t0 use t0 as source -> there is a cyclic dependency: t0->t0 :)
  // EXPECT_THROW(t0.SetBinding(UTReadOnlyDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0)),
  //             DataBinding::CyclicBindingException);
  // Will just fail to set the binding as its read only
  EXPECT_FALSE(t0.SetBinding(UTReadOnlyDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0)));
}


TEST(Test_GetRSet, SetBinding_OneWay_DepProperty_DepProperty_Circular0)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  const uint32_t defaultValue = 0;
  const uint32_t newValue1 = 100;

  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  // t1 use t0 as source -> The first binding will work fine
  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));

  dataBindingService->ExecuteChanges();

  //        t0  t1
  // source --  t0
  // target t1  --
  // t0 use t1 as source -> there is a cyclic dependency: t0->t1->t0
  // EXPECT_THROW(t0.SetBinding(UTReadOnlyDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0)),
  //             DataBinding::CyclicBindingException);
  // Will just fail to set the binding as its read only
  EXPECT_FALSE(t0.SetBinding(UTReadOnlyDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0)));
}


TEST(Test_GetRSet, SetBinding_OneWay_DepProperty_DepProperty_Circular1)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  const uint32_t defaultValue = 0;
  const uint32_t newValue1 = 100;

  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  // t1 use t0 as source -> The first binding will work fine
  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));
  //        t0  t1
  // source --  t0
  // target t1  --
  // t0 use t1 as source -> there is a cyclic dependency: t0->t1->t0
  // EXPECT_THROW(t0.SetBinding(UTReadOnlyDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0)),
  //             DataBinding::CyclicBindingException);
  // Will just fail to set the binding as its read only
  EXPECT_FALSE(t0.SetBinding(UTReadOnlyDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0)));
}


TEST(Test_GetRSet, SetBinding_OneWay_DepProperty_DepProperty_DepProperty_Circular0)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t defaultValue = 0;
  const uint32_t newValue1 = 100;

  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  // t1 use t0 as source -> The first binding will work fine
  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));
  // t2 use t1 as source -> The second binding will work fine
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0));

  dataBindingService->ExecuteChanges();

  //        t0  t1  t2
  // source --  t0  t1
  // target t1  t2  --
  // t0 use t2 as source -> there is a cyclic dependency: t0->t1->t2->t0
  // EXPECT_THROW(t0.SetBinding(UTReadOnlyDependencyObject::Property0, t2.GetPropertyHandle(UTDependencyObject::Property0)),
  //             DataBinding::CyclicBindingException);
  // Will just fail to set the binding as its read only
  EXPECT_FALSE(t0.SetBinding(UTReadOnlyDependencyObject::Property0, t2.GetPropertyHandle(UTDependencyObject::Property0)));
}

TEST(Test_GetRSet, SetBinding_OneWay_DepProperty_DepProperty_DepProperty_Circular1)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t defaultValue = 0;
  const uint32_t newValue1 = 100;

  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  // t1 use t0 as source -> The first binding will work fine
  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));
  // t2 use t1 as source -> The second binding will work fine
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0));

  //        t0  t1  t2
  // source --  t0  t1
  // target t1  t2  --
  // t0 use t2 as source -> there is a cyclic dependency: t0->t1->t2->t0
  // EXPECT_THROW(t0.SetBinding(UTReadOnlyDependencyObject::Property0, t2.GetPropertyHandle(UTDependencyObject::Property0)),
  //             DataBinding::CyclicBindingException);
  // Will just fail to set the binding as its read only
  EXPECT_FALSE(t0.SetBinding(UTReadOnlyDependencyObject::Property0, t2.GetPropertyHandle(UTDependencyObject::Property0)));
}


TEST(Test_GetRSet, SetBinding_OneWay_DepProperty_DepProperty_DepProperty_NoCircular0)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t3(dataBindingService);

  {
    UTDependencyObject t2(dataBindingService);
    // t1 use t0 as source -> The first binding will work fine
    t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));
    // t2 use t1 as source -> The second binding will work fine
    t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0));
    // t3 use t1 as source -> The third binding will work fine
    t3.SetBinding(UTDependencyObject::Property0, t2.GetPropertyHandle(UTDependencyObject::Property0));
  }

  dataBindingService->ExecuteChanges();

  //        t0  t1 t2(dead)  t3
  // source --  t0 t1        t2
  // target t1  t2 t3        --
  // t1 use t3 as source -> while it looks like there might be a cyclic dependency: t1->t2->t3->t1, t2 is actually dead, so the circular dependency
  // has been broken.
  EXPECT_NO_THROW(t1.SetBinding(UTDependencyObject::Property0, t3.GetPropertyHandle(UTDependencyObject::Property0)));
}

TEST(Test_GetRSet, SetBinding_OneWay_DepProperty_DepProperty_DepProperty_NoCircular1)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t3(dataBindingService);

  {
    UTDependencyObject t2(dataBindingService);
    // t1 use t0 as source -> The first binding will work fine
    t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));
    // t2 use t1 as source -> The second binding will work fine
    t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0));
    // t3 use t1 as source -> The third binding will work fine
    t3.SetBinding(UTDependencyObject::Property0, t2.GetPropertyHandle(UTDependencyObject::Property0));
  }

  //        t0  t1 t2(dead)  t3
  // source --  t0 t1        t2
  // target t1  t2 t3        --
  // t1 use t3 as source -> while it looks like there might be a cyclic dependency: t1->t2->t3->t1, t2 is actually dead, so the circular dependency
  // has been broken.
  EXPECT_NO_THROW(t1.SetBinding(UTDependencyObject::Property0, t3.GetPropertyHandle(UTDependencyObject::Property0)));
}


TEST(Test_GetRSet, ChangeSource_OneWay_DepProperty_DepProperty)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  const uint32_t initialValue0 = 100;
  const uint32_t initialValue1 = 1;
  EXPECT_TRUE(t0.SetProperty0Value(initialValue0));
  EXPECT_TRUE(t1.SetProperty0Value(initialValue1));

  EXPECT_EQ(initialValue0, t0.GetProperty0Value());
  EXPECT_EQ(initialValue1, t1.GetProperty0Value());

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(initialValue0, t0.GetProperty0Value());
  EXPECT_EQ(initialValue0, t1.GetProperty0Value());

  // Changing the source value will update the target on the next execute
  const uint32_t newValue = 200;
  EXPECT_TRUE(t0.SetProperty0Value(newValue));

  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue, t0.GetProperty0Value());
  EXPECT_EQ(initialValue0, t1.GetProperty0Value());

  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue, t0.GetProperty0Value());
  EXPECT_EQ(newValue, t1.GetProperty0Value());
}

TEST(Test_GetRSet, ChangeTarget_OneWay_DepProperty_DepProperty)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();
  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  const uint32_t initialValue = 100;
  EXPECT_TRUE(t0.SetProperty0Value(initialValue));
  EXPECT_TRUE(t1.SetProperty0Value(initialValue));

  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));
  dataBindingService->ExecuteChanges();

  // Changing the target value of a one-way binding will be allowed, but it will be changed upon the next execute
  const uint32_t newValue = 200;
  EXPECT_TRUE(t1.SetProperty0Value(newValue));

  EXPECT_EQ(initialValue, t0.GetProperty0Value());
  EXPECT_EQ(newValue, t1.GetProperty0Value());

  EXPECT_EQ(1u, dataBindingService->PendingChanges());

  dataBindingService->ExecuteChanges();

  EXPECT_EQ(initialValue, t0.GetProperty0Value());
  EXPECT_EQ(initialValue, t1.GetProperty0Value());
}

TEST(Test_GetRSet, SetBinding_OneWay_DepProperty_DepProperty_DoubleBind)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();
  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);

  const uint32_t initialValue = 100;
  EXPECT_TRUE(t0.SetProperty0Value(initialValue));
  EXPECT_TRUE(t1.SetProperty0Value(initialValue));

  EXPECT_TRUE(t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0)));
  EXPECT_FALSE(t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0)));
}

TEST(Test_GetRSet, SetBinding_OneWay_DepProperty_DepProperty_SwitchBind)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();
  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t initialValue = 100;
  const uint32_t t2Value = 200;
  EXPECT_TRUE(t0.SetProperty0Value(initialValue));
  EXPECT_TRUE(t1.SetProperty0Value(initialValue));
  EXPECT_TRUE(t2.SetProperty0Value(t2Value));

  EXPECT_TRUE(t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0)));
  EXPECT_TRUE(t1.SetBinding(UTDependencyObject::Property0, t2.GetPropertyHandle(UTDependencyObject::Property0)));

  dataBindingService->ExecuteChanges();

  EXPECT_EQ(initialValue, t0.GetProperty0Value());
  EXPECT_EQ(t2Value, t1.GetProperty0Value());
  EXPECT_EQ(t2Value, t2.GetProperty0Value());
}

TEST(Test_GetRSet, SetBinding_OneWay_DepProperties_Bind_T1T0_T2T1)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t defaultValue = 0;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());
  EXPECT_EQ(defaultValue, t2.GetProperty0Value());

  EXPECT_EQ(0u, dataBindingService->PendingChanges());

  // Binding will change the target to match the source on the next execute
  // t1 <- t0
  // t2 <- t1
  EXPECT_TRUE(t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0)));
  EXPECT_TRUE(t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0)));

  // Binding by itself does not set the value right away
  EXPECT_EQ(2u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());
  EXPECT_EQ(defaultValue, t2.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
  EXPECT_EQ(newValue1, t2.GetProperty0Value());
}

TEST(Test_GetRSet, SetBinding_OneWay_DepProperties_Bind_T2T1_T1T0)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t defaultValue = 0;
  const uint32_t newValue1 = 100;

  EXPECT_TRUE(t0.SetProperty0Value(newValue1));
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());
  EXPECT_EQ(defaultValue, t2.GetProperty0Value());

  // Binding will change the target to match the source on the next execute
  // t2 <- t1
  // t1 <- t0
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0));
  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));

  // Binding by itself does not set the value right away
  EXPECT_EQ(2u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());
  EXPECT_EQ(defaultValue, t2.GetProperty0Value());

  // Execute all pending changes
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue1, t0.GetProperty0Value());
  EXPECT_EQ(newValue1, t1.GetProperty0Value());
  EXPECT_EQ(newValue1, t2.GetProperty0Value());
}


TEST(Test_GetRSet, ChangeSource_OneWay_DepProperty_DepProperty_T1T0_T2T1)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t defaultValue = 0;

  // t1 <- t0
  // t2 <- t1
  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0));
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  // Changing the source value will update the target on the next execute
  const uint32_t newValue = 200;
  EXPECT_TRUE(t0.SetProperty0Value(newValue));

  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue, t0.GetProperty0Value());
  EXPECT_EQ(newValue, t1.GetProperty0Value());
  EXPECT_EQ(newValue, t2.GetProperty0Value());
}


TEST(Test_GetRSet, ChangeSource_OneWay_DepProperty_DepProperty_T2T1_T1T0)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t defaultValue = 0;

  // t2 <- t1
  // t1 <- t0
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0));
  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));
  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  // Changing the source value will update the target on the next execute
  const uint32_t newValue = 200;
  EXPECT_TRUE(t0.SetProperty0Value(newValue));

  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue, t0.GetProperty0Value());
  EXPECT_EQ(defaultValue, t1.GetProperty0Value());

  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue, t0.GetProperty0Value());
  EXPECT_EQ(newValue, t1.GetProperty0Value());
  EXPECT_EQ(newValue, t2.GetProperty0Value());
}

TEST(Test_GetRSet, ChangeSource_OneWay_DepProperty_DepProperty_T1T0_T2T1_Constraints)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  constexpr ConstrainedValue<uint32_t> ConstraintsT1(32, 92);
  t1.SetProperty0ValueConstraints(ConstraintsT1);

  const uint32_t defaultValue = 0;

  // t1 <- t0
  // t2 <- t1
  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0));

  EXPECT_EQ(2u, dataBindingService->PendingChanges());
  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(ConstraintsT1.Min(), t1.GetProperty0Value());
  EXPECT_EQ(defaultValue, t2.GetProperty0Value());


  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(ConstraintsT1.Min(), t1.GetProperty0Value());
  EXPECT_EQ(ConstraintsT1.Min(), t2.GetProperty0Value());

  // Changing the source value will update the target on the next execute
  const uint32_t newValue = 200;
  EXPECT_TRUE(t0.SetProperty0Value(newValue));

  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue, t0.GetProperty0Value());
  EXPECT_EQ(ConstraintsT1.Min(), t1.GetProperty0Value());
  EXPECT_EQ(ConstraintsT1.Min(), t2.GetProperty0Value());

  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue, t0.GetProperty0Value());
  EXPECT_EQ(ConstraintsT1.Max(), t1.GetProperty0Value());
  EXPECT_EQ(ConstraintsT1.Max(), t2.GetProperty0Value());
}

TEST(Test_GetRSet, ChangeSource_OneWay_DepProperty_DepProperty_T1T0_T2T1_ConstraintsSetAfterBindButBeforeExecute)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t defaultValue = 0;

  // t1 <- t0
  // t2 <- t1
  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0));

  constexpr ConstrainedValue<uint32_t> ConstraintsT1(32, 92);
  t1.SetProperty0ValueConstraints(ConstraintsT1);

  EXPECT_EQ(2u, dataBindingService->PendingChanges());
  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(ConstraintsT1.Min(), t1.GetProperty0Value());
  EXPECT_EQ(defaultValue, t2.GetProperty0Value());

  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(ConstraintsT1.Min(), t1.GetProperty0Value());
  EXPECT_EQ(ConstraintsT1.Min(), t2.GetProperty0Value());

  // Changing the source value will update the target on the next execute
  const uint32_t newValue = 200;
  EXPECT_TRUE(t0.SetProperty0Value(newValue));

  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue, t0.GetProperty0Value());
  EXPECT_EQ(ConstraintsT1.Min(), t1.GetProperty0Value());
  EXPECT_EQ(ConstraintsT1.Min(), t2.GetProperty0Value());

  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue, t0.GetProperty0Value());
  EXPECT_EQ(ConstraintsT1.Max(), t1.GetProperty0Value());
  EXPECT_EQ(ConstraintsT1.Max(), t2.GetProperty0Value());
}


TEST(Test_GetRSet, ChangeSource_OneWay_DepProperty_DepProperty_T1T0_T2T1_ConstraintsSetAfterBindAndExecute)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  UTReadOnlyDependencyObject t0(dataBindingService);
  UTDependencyObject t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  const uint32_t defaultValue = 0;

  // t1 <- t0
  // t2 <- t1
  t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject::Property0));

  EXPECT_EQ(2u, dataBindingService->PendingChanges());

  dataBindingService->ExecuteChanges();

  constexpr ConstrainedValue<uint32_t> ConstraintsT1(32, 92);
  t1.SetProperty0ValueConstraints(ConstraintsT1);

  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(ConstraintsT1.Min(), t1.GetProperty0Value());
  EXPECT_EQ(defaultValue, t2.GetProperty0Value());

  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(defaultValue, t0.GetProperty0Value());
  EXPECT_EQ(ConstraintsT1.Min(), t1.GetProperty0Value());
  EXPECT_EQ(ConstraintsT1.Min(), t2.GetProperty0Value());

  // Changing the source value will update the target on the next execute
  const uint32_t newValue = 200;
  EXPECT_TRUE(t0.SetProperty0Value(newValue));

  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue, t0.GetProperty0Value());
  EXPECT_EQ(ConstraintsT1.Min(), t1.GetProperty0Value());
  EXPECT_EQ(ConstraintsT1.Min(), t2.GetProperty0Value());

  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(newValue, t0.GetProperty0Value());
  EXPECT_EQ(ConstraintsT1.Max(), t1.GetProperty0Value());
  EXPECT_EQ(ConstraintsT1.Max(), t2.GetProperty0Value());
}

TEST(Test_GetRSet, DestroyBoundTarget_OneWay_DepProperty_DepProperty_PendingChanges)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();
  UTReadOnlyDependencyObject t0(dataBindingService);

  {
    UTDependencyObject t1(dataBindingService);

    const uint32_t initialValue = 100;
    EXPECT_TRUE(t0.SetProperty0Value(initialValue));
    EXPECT_TRUE(t1.SetProperty0Value(initialValue));

    t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));
  }
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(1u, dataBindingService->PendingDestroys());
  EXPECT_EQ(4u, dataBindingService->InstanceCount());

  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(0u, dataBindingService->PendingDestroys());
  EXPECT_EQ(2u, dataBindingService->InstanceCount());

  EXPECT_TRUE(dataBindingService->SanityCheck());
}


TEST(Test_GetRSet, DestroyBoundTarget_OneWay_DepProperty_DepProperty_NoPendingChanges)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();
  UTReadOnlyDependencyObject t0(dataBindingService);

  {
    UTDependencyObject t1(dataBindingService);

    const uint32_t initialValue = 100;
    EXPECT_TRUE(t0.SetProperty0Value(initialValue));
    EXPECT_TRUE(t1.SetProperty0Value(initialValue));

    t1.SetBinding(UTDependencyObject::Property0, t0.GetPropertyHandle(UTReadOnlyDependencyObject::Property0));
    dataBindingService->ExecuteChanges();
  }

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(1u, dataBindingService->PendingDestroys());
  EXPECT_EQ(4u, dataBindingService->InstanceCount());

  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(0u, dataBindingService->PendingDestroys());
  EXPECT_EQ(2u, dataBindingService->InstanceCount());

  EXPECT_TRUE(dataBindingService->SanityCheck());
}
