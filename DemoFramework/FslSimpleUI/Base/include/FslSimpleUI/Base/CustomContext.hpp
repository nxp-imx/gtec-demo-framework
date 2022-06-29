#ifndef FSLSIMPLEUI_BASE_CUSTOMCONTEXT_HPP
#define FSLSIMPLEUI_BASE_CUSTOMCONTEXT_HPP
/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
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

#include <memory>
#include <stdexcept>
#include <utility>

namespace Fsl::UI
{
  template <typename T>
  class CustomContext
  {
  public:
    using value_type = T;

  private:
    std::weak_ptr<value_type> m_context;

  public:
    //! @brief Move assignment operator
    CustomContext& operator=(CustomContext&& other) noexcept
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        Reset();

        // Claim ownership here
        m_context = std::move(other.m_context);
      }
      return *this;
    }

    //! @brief Move constructor
    //! Transfer ownership from other to this
    CustomContext(CustomContext&& other) noexcept
      : m_context(std::move(other.m_context))
    {
    }


    CustomContext() = default;


    explicit CustomContext(std::weak_ptr<value_type> context)
      : m_context(context)
    {
    }

    //! @brief check if its expired
    //! @warning Even if this returns true the next call to get might fail
    //!          So if you want to use the custom context and allow for a 'null' result use TryGet
    bool IsExpired() const
    {
      return m_context.expired();
    }

    //! @brief Get the context if its still valid
    std::shared_ptr<value_type> Get() const
    {
      auto context = m_context.lock();
      if (context)
      {
        return context;
      }
      throw std::runtime_error("context has expired");
    }

    //! @brief Get the context if its still valid
    std::shared_ptr<value_type> TryGet() const
    {
      return m_context.lock();
    }

    void Reset() noexcept
    {
      m_context.reset();
    }
  };
}

#endif
