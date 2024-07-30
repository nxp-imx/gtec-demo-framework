/****************************************************************************************************************************************************
 * Copyright 2022, 2024 NXP
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

#include <FslBase/Collections/HandleVector.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <functional>
#include <memory>
#include <utility>

using namespace Fsl;

namespace
{
  using Test_GetSetExperiments = TestFixtureFslBase;

  struct Dummy
  {
    int32_t TestValue{0};

    int32_t GetValue() const noexcept
    {
      return TestValue;
    }

    void SetValue(const int32_t value)
    {
      TestValue = value;
    }

    void NoOp()
    {
    }
  };

  //! These binding objects rely heavily on the user managing the lifetime of the get and set target objects.
  namespace BakedPointer
  {
    struct IGet
    {
      virtual ~IGet() = default;
    };

    struct ISet
    {
      virtual ~ISet() = default;

      virtual void Set(const IGet* const pGet) = 0;
    };

    template <typename T>
    struct TypedGet final : IGet
    {
      // NOLINTNEXTLINE(readability-identifier-naming)
      std::function<T()> m_fnGet;

      explicit TypedGet(std::function<T()> fnGet)
        : m_fnGet(std::move(fnGet))
      {
      }

      virtual T Get() const noexcept
      {
        return m_fnGet();
      }
    };

    template <typename T>
    struct TypedSet final : ISet
    {
      // NOLINTNEXTLINE(readability-identifier-naming)
      std::function<void(const int32_t)> m_fnSet;

      explicit TypedSet(std::function<void(const int32_t)> fnSet)
        : m_fnSet(std::move(fnSet))
      {
      }

      void Set(const IGet* const pGet) final
      {
        const auto* pTypedGet = dynamic_cast<const TypedGet<T>*>(pGet);
        if (pTypedGet == nullptr)
        {
          throw std::exception();
        }
        m_fnSet(pTypedGet->Get());
      }
    };
  }

  //! These binding objects rely heavily on the user managing the lifetime of the get and set target objects.
  namespace BakedPointer2
  {
    struct IGet
    {
      virtual ~IGet() = default;
    };

    template <typename T>
    struct ITypedGet : IGet
    {
      virtual T Get() const noexcept = 0;
    };

    struct ISet
    {
      virtual ~ISet() = default;

      virtual void Set(const IGet* const pGet) = 0;
    };

    template <typename TTarget, typename T>
    struct TypedGet final : ITypedGet<T>
    {
      // NOLINTNEXTLINE(readability-identifier-naming)
      const TTarget* m_pBakedThis;
      // NOLINTNEXTLINE(readability-identifier-naming)
      std::function<T(const TTarget*)> m_fnGet;

      TypedGet(const TTarget* pBakedThis, std::function<T(const TTarget*)> fnGet)
        : m_pBakedThis(pBakedThis)
        , m_fnGet(std::move(fnGet))
      {
      }

      T Get() const noexcept final
      {
        return m_fnGet(m_pBakedThis);
      }
    };


    template <typename TTarget, typename T>
    struct TypedSet final : ISet
    {
      // NOLINTNEXTLINE(readability-identifier-naming)
      TTarget* m_pBakedThis;
      // NOLINTNEXTLINE(readability-identifier-naming)
      std::function<void(TTarget* pBakedThis, const int32_t)> m_fnSet;

      TypedSet(TTarget* pBakedThis, std::function<void(TTarget* pBakedThis, const int32_t)> fnSet)
        : m_pBakedThis(pBakedThis)
        , m_fnSet(std::move(fnSet))
      {
      }

      void Set(const IGet* const pGet) final
      {
        const auto* pTypedGet = dynamic_cast<const ITypedGet<T>*>(pGet);
        if (pTypedGet == nullptr)
        {
          throw std::exception();
        }

        m_fnSet(m_pBakedThis, pTypedGet->Get());
      }
    };
  }

  namespace External
  {
    struct PropertyManager
    {
      // NOLINTNEXTLINE(readability-identifier-naming)
      HandleVector<uint32_t> m_uint32;

      int32_t Create()
      {
        return m_uint32.Add(0);
      }

      void Destroy(int32_t handle)
      {
        m_uint32.Remove(handle);
      }

      uint32_t Get(const int32_t handle)
      {
        return m_uint32.Get(handle);
      }

      bool Set(const int32_t handle, const uint32_t value)
      {
        auto& rRecord = m_uint32.Get(handle);
        bool changed = value != rRecord;
        if (value != rRecord)
        {
          rRecord = value;
        }
        return changed;
      }
    };

    struct ExternalDummy
    {
      // NOLINTNEXTLINE(readability-identifier-naming)
      std::shared_ptr<PropertyManager> m_propertyManager;
      // NOLINTNEXTLINE(readability-identifier-naming)
      int32_t m_hTestValue{0};

      explicit ExternalDummy(std::shared_ptr<PropertyManager> propertyManager)
        : m_propertyManager(std::move(propertyManager))
        , m_hTestValue(m_propertyManager->Create())
      {
      }
      ~ExternalDummy()
      {
        m_propertyManager->Destroy(m_hTestValue);
      }

      uint32_t GetValue() const
      {
        return m_propertyManager->Get(m_hTestValue);
      }

      void SetValue(const uint32_t value)
      {
        m_propertyManager->Set(m_hTestValue, value);
      }
    };

    struct IBinding
    {
      virtual ~IBinding() = default;

      virtual void Set(PropertyManager& rPropertyManager, const int32_t hSetHandle, const int32_t hGetHandle) = 0;
    };


    template <typename T>
    struct TypedBinding final : IBinding
    {
      void Set(PropertyManager& rPropertyManager, const int32_t hSetHandle, const int32_t hGetHandle) final
      {
        rPropertyManager.Set(hSetHandle, rPropertyManager.Get(hGetHandle));
      }
    };
  }

  namespace Baked
  {
    struct IDependencyProperty
    {
      virtual ~IDependencyProperty() = default;
    };

    template <typename T>
    struct TypedDependencyProperty : IDependencyProperty
    {
      // NOLINTNEXTLINE(readability-identifier-naming)
      T m_value{};

      T Get() const noexcept
      {
        return m_value;
      }

      bool Set(const T value)
      {
        bool changed = value != m_value;
        if (changed)
        {
          m_value = value;
        }
        return changed;
      }
    };


    struct BakedDummy
    {
      // NOLINTNEXTLINE(readability-identifier-naming)
      TypedDependencyProperty<uint32_t> m_property0;

      uint32_t GetValue() const noexcept
      {
        return m_property0.Get();
      }

      void SetValue(const uint32_t value)
      {
        m_property0.Set(value);
      }
    };

    struct IDynamicBinding
    {
      virtual ~IDynamicBinding() = default;

      virtual void Set(IDependencyProperty* const pSetProp, const IDependencyProperty* const pGetProp) = 0;
    };


    template <typename T>
    struct DynamicTypedBinding final : IDynamicBinding
    {
      void Set(IDependencyProperty* const pSetProp, const IDependencyProperty* const pGetProp) final
      {
        const auto* const pTypedGet = dynamic_cast<const TypedDependencyProperty<T>*>(pGetProp);
        if (pTypedGet == nullptr)
        {
          throw std::exception();
        }
        auto* const pTypedSet = dynamic_cast<TypedDependencyProperty<T>*>(pSetProp);
        if (pTypedSet == nullptr)
        {
          throw std::exception();
        }

        pTypedSet->Set(pTypedGet->Get());
      }
    };

    struct IBoundDynamicBinding
    {
      virtual ~IBoundDynamicBinding() = default;

      virtual void Set(const IDependencyProperty* const pGetProp) = 0;
    };

    template <typename T>
    struct BoundDynamicTypedBinding final : IBoundDynamicBinding
    {
      // NOLINTNEXTLINE(readability-identifier-naming)
      TypedDependencyProperty<T>* m_pSet;

      explicit BoundDynamicTypedBinding(TypedDependencyProperty<T>* pSet)
        : m_pSet(pSet)
      {
      }

      void Set(const IDependencyProperty* const pGetProp) final
      {
        const auto* const pTypedGet = dynamic_cast<const TypedDependencyProperty<T>*>(pGetProp);
        if (pTypedGet == nullptr)
        {
          throw std::exception();
        }
        m_pSet->Set(pTypedGet->Get());
      }
    };
  }
}


TEST(Test_GetSetExperiments, MemFn)
{
  // While interesting, since the type is unspecified its not possible to store it
  auto func = std::mem_fn(&Dummy::SetValue);
  Dummy t;
  func(t, 42);
  EXPECT_EQ(42, t.TestValue);
}

TEST(Test_GetSetExperiments, Function)
{
  std::function<void(Dummy * dummy, const int32_t)> fnSet = &Dummy::SetValue;
  Dummy t;
  fnSet(&t, 42);
  EXPECT_EQ(42, t.TestValue);
}

TEST(Test_GetSetExperiments, Lambda)
{
  Dummy t;
  std::function<void(const int32_t)> fn = [&t](const int32_t value) { t.SetValue(value); };
  fn(42);
  EXPECT_EQ(42, t.TestValue);
}


TEST(Test_GetSetExperiments, MemberFunctionGetSetViaBakedPointers)
{
  using namespace BakedPointer;

  Dummy t0;
  Dummy t1;

  auto typedGet = std::make_unique<TypedGet<int32_t>>([&t0]() { return t0.GetValue(); });
  auto typedSet = std::make_unique<TypedSet<int32_t>>([&t1](const int32_t value) { t1.SetValue(value); });

  t0.SetValue(100);
  EXPECT_EQ(100, t0.TestValue);
  EXPECT_EQ(0, t1.TestValue);

  IGet* get = typedGet.get();
  ISet* set = typedSet.get();

  set->Set(get);

  EXPECT_EQ(100, t0.TestValue);
  EXPECT_EQ(100, t1.TestValue);
}


TEST(Test_GetSetExperiments, MemberFunctionGetSetViaBakedPointers2)
{
  using namespace BakedPointer2;

  Dummy t0;
  Dummy t1;

  auto typedGet = std::make_unique<TypedGet<Dummy, int32_t>>(&t0, &Dummy::GetValue);
  auto typedSet = std::make_unique<TypedSet<Dummy, int32_t>>(&t1, &Dummy::SetValue);

  t0.SetValue(100);
  EXPECT_EQ(100, t0.TestValue);
  EXPECT_EQ(0, t1.TestValue);

  IGet* get = typedGet.get();
  ISet* set = typedSet.get();

  set->Set(get);

  EXPECT_EQ(100, t0.TestValue);
  EXPECT_EQ(100, t1.TestValue);
}

TEST(Test_GetSetExperiments, MemberFunctionGetSetViaExternal)
{
  using namespace External;

  auto propertyManager = std::make_shared<PropertyManager>();
  ExternalDummy t0(propertyManager);
  ExternalDummy t1(propertyManager);

  t0.SetValue(100);
  EXPECT_EQ(100u, t0.GetValue());
  EXPECT_EQ(0u, t1.GetValue());

  auto typedBinding = std::make_unique<TypedBinding<int32_t>>();

  IBinding* pBinding = typedBinding.get();
  pBinding->Set(*propertyManager, t1.m_hTestValue, t0.m_hTestValue);

  EXPECT_EQ(100u, t0.GetValue());
  EXPECT_EQ(100u, t1.GetValue());
}

TEST(Test_GetSetExperiments, MemberFunctionGetSetViaBake)
{
  using namespace Baked;

  BakedDummy t0;
  BakedDummy t1;

  t0.SetValue(100);
  EXPECT_EQ(100u, t0.GetValue());
  EXPECT_EQ(0u, t1.GetValue());

  IDependencyProperty* pProp0 = &t0.m_property0;
  IDependencyProperty* pProp1 = &t1.m_property0;

  {
    DynamicTypedBinding<uint32_t> dynamicBinding;
    IDynamicBinding& binding = dynamicBinding;

    binding.Set(pProp1, pProp0);

    EXPECT_EQ(100u, t0.GetValue());
    EXPECT_EQ(100u, t1.GetValue());
  }

  t0.SetValue(200);
  EXPECT_EQ(200u, t0.GetValue());
  EXPECT_EQ(100u, t1.GetValue());

  {
    BoundDynamicTypedBinding<uint32_t> dynamicBinding(&t1.m_property0);
    IBoundDynamicBinding& binding = dynamicBinding;

    binding.Set(pProp0);

    EXPECT_EQ(200u, t0.GetValue());
    EXPECT_EQ(200u, t1.GetValue());
  }
}

TEST(Test_GetSetExperiments, TestMemberFunctionPointers)
{
  int32_t (Dummy::*pFnGetMethod)() const = &Dummy::GetValue;
  void (Dummy::*pFnSetMethod)(const int32_t) = &Dummy::SetValue;

  Dummy t0;

  const int32_t defaultValue = 0;
  const int32_t newValue = 100;
  EXPECT_EQ(defaultValue, t0.GetValue());
  EXPECT_EQ(defaultValue, (t0.*pFnGetMethod)());


  (t0.*pFnSetMethod)(newValue);
  EXPECT_EQ(newValue, t0.GetValue());
  EXPECT_EQ(newValue, (t0.*pFnGetMethod)());
}

namespace
{
  namespace MagicMemberPointer1
  {
    template <typename TValueType>
    class SetMemberPointer
    {
      using FnTypedCallProxy = void (*)(void*, const TValueType);
      void* m_pObject{};
      FnTypedCallProxy m_pStub{};

      constexpr SetMemberPointer(void* pObject, FnTypedCallProxy pStub) noexcept
        : m_pObject(pObject)
        , m_pStub(pStub)
      {
      }

    public:
      constexpr SetMemberPointer() noexcept = default;

      template <class T, void (T::*TSetMethod)(const TValueType)>
      static constexpr SetMemberPointer Create(T* pObject) noexcept
      {
        return {pObject, &TypedCallProxy<T, TSetMethod>};
      }


      void Call(const TValueType value) const
      {
        (*m_pStub)(m_pObject, value);
      }

    private:
      template <class T, void (T::*TSetMethod)(const TValueType)>
      static void TypedCallProxy(void* pObject, const TValueType value)
      {
        T* p = static_cast<T*>(pObject);
        (p->*TSetMethod)(value);
      }
    };
  }
}

TEST(Test_GetSetExperiments, TestMemberFunctionPointers_MagicPointer)
{
  using namespace MagicMemberPointer1;

  Dummy t0;

  SetMemberPointer<int32_t> set = SetMemberPointer<int32_t>::Create<Dummy, &Dummy::SetValue>(&t0);

  const int32_t defaultValue = 0;
  const int32_t newValue = 100;
  EXPECT_EQ(defaultValue, t0.GetValue());


  set.Call(newValue);
  EXPECT_EQ(newValue, t0.GetValue());
}

namespace
{
  namespace MagicMemberPointer2
  {
    template <typename TValueType>
    class SetMemberPointer
    {
      using FnTypedCallProxy = void (*)(void*, const TValueType);
      FnTypedCallProxy m_pStub{};

      constexpr explicit SetMemberPointer(FnTypedCallProxy pStub) noexcept
        : m_pStub(pStub)
      {
      }

    public:
      constexpr SetMemberPointer() noexcept = default;

      template <class T, void (T::*TSetMethod)(const TValueType)>
      static constexpr SetMemberPointer Create() noexcept
      {
        return SetMemberPointer(&TypedCallProxy<T, TSetMethod>);
      }


      void Call(void* pObjectThis, const TValueType value) const
      {
        (*m_pStub)(pObjectThis, value);
      }

    private:
      template <class T, void (T::*TSetMethod)(const TValueType)>
      static void TypedCallProxy(void* pObjectThis, const TValueType value)
      {
        T* p = static_cast<T*>(pObjectThis);
        (p->*TSetMethod)(value);
      }
    };
  }
}

TEST(Test_GetSetExperiments, TestMemberFunctionPointers_MagicPointer2)
{
  using namespace MagicMemberPointer2;

  Dummy t0;

  SetMemberPointer<int32_t> set = SetMemberPointer<int32_t>::Create<Dummy, &Dummy::SetValue>();

  const int32_t defaultValue = 0;
  const int32_t newValue = 100;
  EXPECT_EQ(defaultValue, t0.GetValue());


  set.Call(&t0, newValue);
  EXPECT_EQ(newValue, t0.GetValue());
}


namespace
{
  namespace MagicMemberPointer3
  {
    template <typename TValueType>
    class GetSetMemberPointer
    {
      using FnTypedGetMemberCallProxy = TValueType (*)(void* pObjectThis);
      using FnTypedSetMemberCallProxy = bool (*)(void* pObjectThis, const TValueType);

      FnTypedGetMemberCallProxy m_fnGetProxy{};
      FnTypedSetMemberCallProxy m_fnSetProxy{};

      constexpr GetSetMemberPointer(FnTypedGetMemberCallProxy fnGetProxy, FnTypedSetMemberCallProxy fnSetProxy) noexcept
        : m_fnGetProxy(fnGetProxy)
        , m_fnSetProxy(fnSetProxy)
      {
      }

    public:
      constexpr GetSetMemberPointer() noexcept = default;

      template <class T, TValueType (T::*TGetMethod)() const, bool (T::*TSetMethod)(const TValueType)>
      static constexpr GetSetMemberPointer Create() noexcept
      {
        return {&TypedGetMemberCallProxy<T, TGetMethod>, &TypedSetMemberCallProxy<T, TSetMethod>};
      }


      TValueType Get(void* pObjectThis) const
      {
        return (*m_fnGetProxy)(pObjectThis);
      }

      void Set(void* pObjectThis, const TValueType value)
      {
        (*m_fnSetProxy)(pObjectThis, value);
      }

    private:
      template <class T, TValueType (T::*TGetMethod)() const>
      static TValueType TypedGetMemberCallProxy(void* pObjectThis)
      {
        return (static_cast<T*>(pObjectThis)->*TGetMethod)();
      }

      template <class T, bool (T::*TSetMethod)(const TValueType)>
      static bool TypedSetMemberCallProxy(void* pObjectThis, const TValueType value)
      {
        return (static_cast<T*>(pObjectThis)->*TSetMethod)(value);
      }
    };

    struct TestDummy
    {
      int32_t TestValue{0};

      int32_t GetValue() const noexcept
      {
        return TestValue;
      }

      bool SetValue(const int32_t value)
      {
        TestValue = value;
        return true;
      }
    };
  }
}

TEST(Test_GetSetExperiments, TestMemberFunctionPointers_MagicPointer3)
{
  using namespace MagicMemberPointer3;

  TestDummy t0;

  auto propMagic = GetSetMemberPointer<int32_t>::Create<TestDummy, &TestDummy::GetValue, &TestDummy::SetValue>();

  const int32_t defaultValue = 0;
  const int32_t newValue = 100;
  EXPECT_EQ(defaultValue, t0.GetValue());
  EXPECT_EQ(defaultValue, propMagic.Get(&t0));


  propMagic.Set(&t0, newValue);
  EXPECT_EQ(newValue, t0.GetValue());
  EXPECT_EQ(newValue, propMagic.Get(&t0));
}


namespace
{
  namespace MagicMemberPointer4
  {
    class IGetSetMemberPointer
    {
    public:
      virtual ~IGetSetMemberPointer() = default;
    };

    template <typename T, typename TValueType>
    class TypedGetSetMemberPointer : public IGetSetMemberPointer
    {
      using FnGet = TValueType (T::*)() const;
      using FnSet = bool (T::*)(const TValueType);

      FnGet m_fnGet{};
      FnSet m_fnSet{};

      constexpr TypedGetSetMemberPointer(FnGet fnGetProxy, FnSet fnSetProxy) noexcept
        : m_fnGet(fnGetProxy)
        , m_fnSet(fnSetProxy)
      {
      }

    public:
      constexpr TypedGetSetMemberPointer() noexcept = default;

      template <TValueType (T::*TGetMethod)() const, bool (T::*TSetMethod)(const TValueType)>
      static constexpr TypedGetSetMemberPointer Create() noexcept
      {
        return {TGetMethod, TSetMethod};
      }

      TValueType Get(const T* pObjectThis) const
      {
        return (pObjectThis->*m_fnGet)();
      }

      void Set(T* pObjectThis, const TValueType value)
      {
        (pObjectThis->*m_fnSet)(value);
      }
    };

    struct TestDummy
    {
      int32_t TestValue{0};

      int32_t GetValue() const noexcept
      {
        return TestValue;
      }

      bool SetValue(const int32_t value)
      {
        TestValue = value;
        return true;
      }
    };
  }
}

TEST(Test_GetSetExperiments, TestMemberFunctionPointers_MagicPointer4)
{
  using namespace MagicMemberPointer4;

  TestDummy t0;

  auto propMagic = TypedGetSetMemberPointer<TestDummy, int32_t>::Create<&TestDummy::GetValue, &TestDummy::SetValue>();

  const int32_t defaultValue = 0;
  const int32_t newValue = 100;
  EXPECT_EQ(defaultValue, t0.GetValue());
  // EXPECT_EQ(defaultValue, propMagic.Get(&t0));


  propMagic.Set(&t0, newValue);
  EXPECT_EQ(newValue, t0.GetValue());
  // EXPECT_EQ(newValue, propMagic.Get(&t0));

  // FIX: implement support like this
  // DependencyPropertyDefinition Property0 = DependencyPropertyDefinition::Create<TestDummy, int32_t, TestDummy::GetValue,
  // &TestDummy::SetValue>(propertyName)
}
