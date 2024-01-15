#ifndef FSLSERVICE_IMPL_SERVICETYPE_GLOBAL_THREADSAFESYNCHRONOUSSERVICETEMPLATE_HPP
#define FSLSERVICE_IMPL_SERVICETYPE_GLOBAL_THREADSAFESYNCHRONOUSSERVICETEMPLATE_HPP
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

#include <FslService/Impl/ServiceType/Global/ThreadSafeSynchronousServiceBase.hpp>

namespace Fsl
{
  template <typename T>
  class ThreadSafeSynchronousServiceTemplate0 : public ThreadSafeSynchronousServiceBase
  {
  public:
    ThreadSafeSynchronousServiceTemplate0()
      : ThreadSafeSynchronousServiceBase(typeid(T))
    {
    }

    explicit ThreadSafeSynchronousServiceTemplate0(const ServiceCaps::Flags flags)
      : ThreadSafeSynchronousServiceBase(typeid(T), flags)
    {
    }

    std::shared_ptr<IService> Allocate(ServiceProvider& provider) override
    {
      return std::shared_ptr<IService>(new T(provider));
    }
  };


  template <typename T, typename TItf>
  class ThreadSafeSynchronousServiceTemplate : public ThreadSafeSynchronousServiceBase
  {
  public:
    ThreadSafeSynchronousServiceTemplate()
      : ThreadSafeSynchronousServiceBase(typeid(TItf))
    {
    }

    explicit ThreadSafeSynchronousServiceTemplate(const ServiceCaps::Flags flags)
      : ThreadSafeSynchronousServiceBase(typeid(TItf), flags)
    {
    }

    std::shared_ptr<IService> Allocate(ServiceProvider& provider) override
    {
      return std::shared_ptr<IService>(new T(provider));
    }
  };


  template <typename T, typename TItf1, typename TItf2>
  class ThreadSafeSynchronousServiceTemplate2 : public IThreadSafeSynchronousServiceFactory
  {
    ServiceCaps::Flags m_flags{ServiceCaps::Default};

  public:
    ThreadSafeSynchronousServiceTemplate2() = default;

    explicit ThreadSafeSynchronousServiceTemplate2(const ServiceCaps::Flags flags)
      : m_flags(flags)
    {
    }


    std::shared_ptr<AServiceOptionParser> GetOptionParser() const override
    {
      return {};
    }


    ServiceCaps::Flags GetFlags() const override
    {
      return m_flags;
    }

    void FillInterfaceType(ServiceSupportedInterfaceDeque& rServiceInterfaceTypeDeque) const override
    {
      rServiceInterfaceTypeDeque.push_back(typeid(TItf1));
      rServiceInterfaceTypeDeque.push_back(typeid(TItf2));
    }


    std::shared_ptr<IService> Allocate(ServiceProvider& provider) override
    {
      return std::shared_ptr<IService>(new T(provider));
    }
  };


  template <typename T, typename TItf1, typename TItf2, typename TItf3>
  class ThreadSafeSynchronousServiceTemplate3 : public IThreadSafeSynchronousServiceFactory
  {
    ServiceCaps::Flags m_flags{ServiceCaps::Default};

  public:
    ThreadSafeSynchronousServiceTemplate3() = default;

    explicit ThreadSafeSynchronousServiceTemplate3(const ServiceCaps::Flags flags)
      : m_flags(flags)
    {
    }

    ServiceCaps::Flags GetFlags() const override
    {
      return m_flags;
    }

    void FillInterfaceType(ServiceSupportedInterfaceDeque& rServiceInterfaceTypeDeque) const override
    {
      rServiceInterfaceTypeDeque.push_back(typeid(TItf1));
      rServiceInterfaceTypeDeque.push_back(typeid(TItf2));
      rServiceInterfaceTypeDeque.push_back(typeid(TItf3));
    }


    std::shared_ptr<IService> Allocate(ServiceProvider& provider) override
    {
      return std::shared_ptr<IService>(new T(provider));
    }
  };
}

#endif
