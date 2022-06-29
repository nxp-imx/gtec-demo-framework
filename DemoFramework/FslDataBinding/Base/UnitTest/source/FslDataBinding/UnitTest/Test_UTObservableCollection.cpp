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
#include <FslDataBinding/Base/DataBindingService.hpp>
#include <FslDataBinding/Base/Exceptions.hpp>
#include <fmt/format.h>
#include <memory>
#include "UTDependencyObject2.hpp"
#include "UTObservableCollection.hpp"

using namespace Fsl;

namespace
{
  using Test_UTObservableCollection = TestFixtureFslBase;
}


TEST(Test_UTObservableCollection, Construct)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  EXPECT_EQ(0u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());

  UTObservableCollection t0(dataBindingService);

  // The data binding is on demand so nothing is registered per default
  EXPECT_EQ(0u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());
}


TEST(Test_UTObservableCollection, GetInstanceHandle)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  EXPECT_EQ(0u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());

  UTObservableCollection t0(dataBindingService);

  // The data binding is on demand so nothing is registered per default
  EXPECT_EQ(0u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());

  // Getting the instance id will cause the data source to register itself (since its the first time its called)
  auto hSourceInstance = t0.GetSourceInstanceHandle();
  EXPECT_TRUE(hSourceInstance.IsValid());

  EXPECT_EQ(1u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());

  // Getting the instance id a second time will return the cached entry
  auto hSourceInstance2 = t0.GetSourceInstanceHandle();
  EXPECT_TRUE(hSourceInstance2.IsValid());
  EXPECT_EQ(hSourceInstance, hSourceInstance2);

  EXPECT_EQ(1u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());
}


TEST(Test_UTObservableCollection, MarkAsChanged_NoTarget)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  EXPECT_EQ(0u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());

  UTObservableCollection t0(dataBindingService);

  EXPECT_EQ(0u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());

  // As there is no bound target no one will be notified (so it returns false)
  EXPECT_FALSE(t0.MarkAsChanged());

  // Since there is no bound target there will be no pending changes and no instance will need to be registered
  EXPECT_EQ(0u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());
}

TEST(Test_UTObservableCollection, ChangedNotification)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  auto t0 = std::make_shared<UTObservableCollection>(dataBindingService);
  UTDependencyObject2 t1(dataBindingService);

  EXPECT_EQ(0u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());

  EXPECT_EQ(0u, t1.TestProperty5ChangedCallCount);

  t1.SetProperty5Value(t0);

  EXPECT_EQ(4u, dataBindingService->InstanceCount());
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(0u, t1.TestProperty5ChangedCallCount);

  t0->MarkAsChanged();

  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(0u, t1.TestProperty5ChangedCallCount);

  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(1u, t1.TestProperty5ChangedCallCount);
}

TEST(Test_UTObservableCollection, ChangedNotification_PropertiesModified)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  auto t0 = std::make_shared<UTObservableCollection>(dataBindingService);
  UTDependencyObject2 t1(dataBindingService);
  UTDependencyObject t2(dataBindingService);

  EXPECT_EQ(0u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(0u, t1.TestProperty6ChangedCallCount);

  // Bind t2.prop0 to t1.prop7
  t2.SetBinding(UTDependencyObject::Property0, t1.GetPropertyHandle(UTDependencyObject2::Property7));
  EXPECT_EQ(4u, dataBindingService->InstanceCount());
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(0u, t1.TestProperty6ChangedCallCount);
  dataBindingService->ExecuteChanges();
  EXPECT_EQ(4u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());

  t1.SetProperty6Value(t0);

  EXPECT_EQ(7u, dataBindingService->InstanceCount());
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(0u, t1.TestProperty6ChangedCallCount);

  t0->MarkAsChanged();

  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(0u, t1.TestProperty6ChangedCallCount);
  EXPECT_EQ(0u, t1.GetProperty7Value());
  EXPECT_EQ(0u, t2.GetProperty0Value());

  dataBindingService->ExecuteChanges();

  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(1u, t1.TestProperty6ChangedCallCount);
  EXPECT_EQ(t1.TestProperty6ChangedCallCount, t1.GetProperty7Value());
  // Since changes to property6 gets reflected in property7 we expect that change to have processed right away
  EXPECT_EQ(t1.TestProperty6ChangedCallCount, t2.GetProperty0Value());
}


TEST(Test_UTObservableCollection, SetBinding_Oneway_TypedObserverDependencyProperty_TypedObserverDependencyProperty)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  auto t0A = std::make_shared<UTObservableCollection>(dataBindingService);
  auto t0B = std::make_shared<UTObservableCollection>(dataBindingService);
  UTDependencyObject2 t1(dataBindingService);
  UTDependencyObject2 t2(dataBindingService);

  EXPECT_EQ(0u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(0u, t1.TestProperty6ChangedCallCount);

  // Bind t2.prop5 to t1.prop5 (link between two TypedObserverDependencyProperty)
  t2.SetBinding(UTDependencyObject2::Property5, t1.GetPropertyHandle(UTDependencyObject2::Property5));
  EXPECT_EQ(6u, dataBindingService->InstanceCount());
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(0u, t1.TestProperty6ChangedCallCount);
  dataBindingService->ExecuteChanges();
  EXPECT_EQ(6u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());
}

TEST(Test_UTObservableCollection, ChangeSource_Oneway_TypedObserverDependencyProperty_TypedObserverDependencyProperty_WithPrebindValue)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  auto srcCol0 = std::make_shared<UTObservableCollection>(dataBindingService);
  auto srcCol1 = std::make_shared<UTObservableCollection>(dataBindingService);
  UTDependencyObject2 t1(dataBindingService);
  UTDependencyObject2 t2(dataBindingService);
  t1.SetProperty5Value(srcCol0);

  EXPECT_EQ(4u, dataBindingService->InstanceCount());
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(0u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(0u, t2.TestProperty5ChangedCallCount);
  EXPECT_EQ(srcCol0, t1.GetProperty5Value());
  EXPECT_EQ(std::shared_ptr<DataBinding::IObservableObject>(), t2.GetProperty5Value());

  dataBindingService->ExecuteChanges();

  EXPECT_EQ(4u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(1u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(0u, t2.TestProperty5ChangedCallCount);
  EXPECT_EQ(srcCol0, t1.GetProperty5Value());
  EXPECT_EQ(std::shared_ptr<DataBinding::IObservableObject>(), t2.GetProperty5Value());


  // Bind t2.prop5 to t1.prop5 (link between two TypedObserverDependencyProperty)
  t2.SetBinding(UTDependencyObject2::Property5, t1.GetPropertyHandle(UTDependencyObject2::Property5));
  // +3 (dependency object, dependency property + observer property)
  EXPECT_EQ(7u, dataBindingService->InstanceCount());
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(1u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(0u, t2.TestProperty5ChangedCallCount);
  dataBindingService->ExecuteChanges();
  EXPECT_EQ(7u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(1u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(1u, t2.TestProperty5ChangedCallCount);

  t1.SetProperty5Value(srcCol1);

  EXPECT_EQ(8u, dataBindingService->InstanceCount());
  EXPECT_EQ(2u, dataBindingService->PendingChanges());
  EXPECT_EQ(1u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(1u, t2.TestProperty5ChangedCallCount);

  // The source value was changed right away but the linked one is still pending
  EXPECT_EQ(srcCol1, t1.GetProperty5Value());
  EXPECT_EQ(srcCol0, t2.GetProperty5Value());

  // So lets execute the changes to get it in-sync
  ASSERT_NO_THROW(dataBindingService->ExecuteChanges());

  EXPECT_EQ(8u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(2u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(2u, t2.TestProperty5ChangedCallCount);

  // The source value was changed right away but the linked one is still pending
  EXPECT_EQ(srcCol1, t1.GetProperty5Value());
  EXPECT_EQ(srcCol1, t2.GetProperty5Value());
}

TEST(Test_UTObservableCollection, ChangeSource_Oneway_TypedObserverDependencyProperty_TypedObserverDependencyProperty)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  auto srcCol0 = std::make_shared<UTObservableCollection>(dataBindingService);
  UTDependencyObject2 t1(dataBindingService);
  UTDependencyObject2 t2(dataBindingService);

  EXPECT_EQ(0u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(0u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(0u, t2.TestProperty5ChangedCallCount);
  EXPECT_EQ(std::shared_ptr<DataBinding::IObservableObject>(), t1.GetProperty5Value());
  EXPECT_EQ(std::shared_ptr<DataBinding::IObservableObject>(), t2.GetProperty5Value());

  // Bind t2.prop5 to t1.prop5 (link between two TypedObserverDependencyProperty)
  t2.SetBinding(UTDependencyObject2::Property5, t1.GetPropertyHandle(UTDependencyObject2::Property5));
  EXPECT_EQ(6u, dataBindingService->InstanceCount());
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(0u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(0u, t2.TestProperty5ChangedCallCount);
  EXPECT_EQ(std::shared_ptr<DataBinding::IObservableObject>(), t1.GetProperty5Value());
  EXPECT_EQ(std::shared_ptr<DataBinding::IObservableObject>(), t2.GetProperty5Value());

  dataBindingService->ExecuteChanges();
  EXPECT_EQ(6u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(0u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(0u, t2.TestProperty5ChangedCallCount);

  t1.SetProperty5Value(srcCol0);

  EXPECT_EQ(7u, dataBindingService->InstanceCount());
  // t1.property5 was changed and t1.property5 observer caused a pending change from the source to be registered
  EXPECT_EQ(2u, dataBindingService->PendingChanges());
  EXPECT_EQ(0u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(0u, t2.TestProperty5ChangedCallCount);
  // The source value was changed right away but the linked one is still pending
  EXPECT_EQ(srcCol0, t1.GetProperty5Value());
  EXPECT_EQ(std::shared_ptr<DataBinding::IObservableObject>(), t2.GetProperty5Value());

  // So lets execute the changes to get it in-sync
  ASSERT_NO_THROW(dataBindingService->ExecuteChanges());

  EXPECT_EQ(7u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(1u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(1u, t2.TestProperty5ChangedCallCount);
  // Both values are now the same
  EXPECT_EQ(srcCol0, t1.GetProperty5Value());
  EXPECT_EQ(srcCol0, t2.GetProperty5Value());
}


TEST(Test_UTObservableCollection, ChangeSource_Oneway_TypedObserverDependencyProperty_TypedObserverDependencyProperty_WithPrebindValue_Clear)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  auto srcCol0 = std::make_shared<UTObservableCollection>(dataBindingService);
  UTDependencyObject2 t1(dataBindingService);
  UTDependencyObject2 t2(dataBindingService);
  t1.SetProperty5Value(srcCol0);

  EXPECT_EQ(4u, dataBindingService->InstanceCount());
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(0u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(0u, t2.TestProperty5ChangedCallCount);
  EXPECT_EQ(srcCol0, t1.GetProperty5Value());
  EXPECT_EQ(std::shared_ptr<DataBinding::IObservableObject>(), t2.GetProperty5Value());

  dataBindingService->ExecuteChanges();

  EXPECT_EQ(4u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(1u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(0u, t2.TestProperty5ChangedCallCount);
  EXPECT_EQ(srcCol0, t1.GetProperty5Value());
  EXPECT_EQ(std::shared_ptr<DataBinding::IObservableObject>(), t2.GetProperty5Value());


  // Bind t2.prop5 to t1.prop5 (link between two TypedObserverDependencyProperty)
  t2.SetBinding(UTDependencyObject2::Property5, t1.GetPropertyHandle(UTDependencyObject2::Property5));
  // +3 (dependency object, dependency property + observer property)
  EXPECT_EQ(7u, dataBindingService->InstanceCount());
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(1u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(0u, t2.TestProperty5ChangedCallCount);
  dataBindingService->ExecuteChanges();
  EXPECT_EQ(7u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(1u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(1u, t2.TestProperty5ChangedCallCount);

  t1.SetProperty5Value(std::shared_ptr<DataBinding::IObservableObject>());

  EXPECT_EQ(7u, dataBindingService->InstanceCount());
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(1u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(1u, t2.TestProperty5ChangedCallCount);

  // The source value was changed right away but the linked one is still pending
  EXPECT_EQ(std::shared_ptr<DataBinding::IObservableObject>(), t1.GetProperty5Value());
  EXPECT_EQ(srcCol0, t2.GetProperty5Value());

  // So lets execute the changes to get it in-sync
  ASSERT_NO_THROW(dataBindingService->ExecuteChanges());

  EXPECT_EQ(7u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(1u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(1u, t2.TestProperty5ChangedCallCount);

  // The source value was changed right away but the linked one is still pending
  EXPECT_EQ(std::shared_ptr<DataBinding::IObservableObject>(), t1.GetProperty5Value());
  EXPECT_EQ(std::shared_ptr<DataBinding::IObservableObject>(), t2.GetProperty5Value());
}

TEST(Test_UTObservableCollection, ChangeSource_Oneway_TypedObserverDependencyProperty_TypedObserverDependencyProperty_Clear)
{
  auto dataBindingService = std::make_shared<DataBinding::DataBindingService>();

  auto srcCol0 = std::make_shared<UTObservableCollection>(dataBindingService);
  UTDependencyObject2 t1(dataBindingService);
  UTDependencyObject2 t2(dataBindingService);

  EXPECT_EQ(0u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(0u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(0u, t2.TestProperty5ChangedCallCount);
  EXPECT_EQ(std::shared_ptr<DataBinding::IObservableObject>(), t1.GetProperty5Value());
  EXPECT_EQ(std::shared_ptr<DataBinding::IObservableObject>(), t2.GetProperty5Value());

  // Bind t2.prop5 to t1.prop5 (link between two TypedObserverDependencyProperty)
  t2.SetBinding(UTDependencyObject2::Property5, t1.GetPropertyHandle(UTDependencyObject2::Property5));
  EXPECT_EQ(6u, dataBindingService->InstanceCount());
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(0u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(0u, t2.TestProperty5ChangedCallCount);
  EXPECT_EQ(std::shared_ptr<DataBinding::IObservableObject>(), t1.GetProperty5Value());
  EXPECT_EQ(std::shared_ptr<DataBinding::IObservableObject>(), t2.GetProperty5Value());

  dataBindingService->ExecuteChanges();
  EXPECT_EQ(6u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(0u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(0u, t2.TestProperty5ChangedCallCount);

  t1.SetProperty5Value(srcCol0);

  EXPECT_EQ(7u, dataBindingService->InstanceCount());
  // t1.property5 was changed and t1.property5 observer caused a pending change from the source to be registered
  EXPECT_EQ(2u, dataBindingService->PendingChanges());
  EXPECT_EQ(0u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(0u, t2.TestProperty5ChangedCallCount);
  // The source value was changed right away but the linked one is still pending
  EXPECT_EQ(srcCol0, t1.GetProperty5Value());
  EXPECT_EQ(std::shared_ptr<DataBinding::IObservableObject>(), t2.GetProperty5Value());

  // So lets execute the changes to get it in-sync
  ASSERT_NO_THROW(dataBindingService->ExecuteChanges());

  EXPECT_EQ(7u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(1u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(1u, t2.TestProperty5ChangedCallCount);
  // Both values are now the same
  EXPECT_EQ(srcCol0, t1.GetProperty5Value());
  EXPECT_EQ(srcCol0, t2.GetProperty5Value());

  t1.SetProperty5Value(std::shared_ptr<DataBinding::IObservableObject>());

  EXPECT_EQ(7u, dataBindingService->InstanceCount());
  EXPECT_EQ(1u, dataBindingService->PendingChanges());
  EXPECT_EQ(1u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(1u, t2.TestProperty5ChangedCallCount);
  // Both values are now the same
  EXPECT_EQ(std::shared_ptr<DataBinding::IObservableObject>(), t1.GetProperty5Value());
  EXPECT_EQ(srcCol0, t2.GetProperty5Value());

  // So lets execute the changes to get it in-sync
  ASSERT_NO_THROW(dataBindingService->ExecuteChanges());

  EXPECT_EQ(7u, dataBindingService->InstanceCount());
  EXPECT_EQ(0u, dataBindingService->PendingChanges());
  EXPECT_EQ(1u, t1.TestProperty5ChangedCallCount);
  EXPECT_EQ(1u, t2.TestProperty5ChangedCallCount);
  // Both values are now the same
  EXPECT_EQ(std::shared_ptr<DataBinding::IObservableObject>(), t1.GetProperty5Value());
  EXPECT_EQ(std::shared_ptr<DataBinding::IObservableObject>(), t2.GetProperty5Value());
}
