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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Placement/Controlled.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <cstring>
#include <utility>

using namespace Fsl;

namespace
{
  using TestControlled = TestFixtureFslBase;

  class A
  {
    int m_id{0};

  public:
    A()
    {
      FSLLOG3_INFO("A{} Construct", m_id);
    }

    explicit A(const int id)
      : m_id(id)
    {
      FSLLOG3_INFO("A{} Construct", m_id);
    }


    A(const A& other)
      : m_id(other.m_id)
    {
      FSLLOG3_INFO("A{} Copy construct", m_id);
    }

    A(A&& other) noexcept
      : m_id(other.m_id)
    {
      FSLLOG3_INFO("A{} Move construct", m_id);
      other.m_id = 0;
    }

    // A& operator=(A&& other) noexcept = default;
    A& operator=(A&& other) noexcept
    {
      if (this != &other)
      {
        // Implemented move in same order as a default implementation (does not work with RAII objects with multiple entries as it differ from
        // destruction order)
        m_id = other.m_id;
        other.m_id = 0;
      }
      return *this;
    }

    ~A()
    {
      FSLLOG3_INFO("A{} Destruct", m_id);
    }
  };

  class B
  {
    int m_id{0};


  public:
    B()
    {
      FSLLOG3_INFO("B{} Construct", m_id);
    }

    explicit B(const int id)
      : m_id(id)
    {
      FSLLOG3_INFO("B{} Construct", m_id);
    }

    B(const B& other)
      : m_id(other.m_id)
    {
      FSLLOG3_INFO("B{} Copy construct", m_id);
    }

    B(B&& other) noexcept
      : m_id(other.m_id)
    {
      other.m_id = 0;
      FSLLOG3_INFO("B{} Move construct", m_id);
    }

    // B& operator=(B&& other) noexcept = default;
    B& operator=(B&& other) noexcept
    {
      if (this != &other)
      {
        // Implemented move in same order as a default implementation (does not work with RAII objects with multiple entries as it differ from
        // destruction order)
        m_id = other.m_id;
        other.m_id = 0;
      }
      return *this;
    }

    ~B()
    {
      FSLLOG3_INFO("B{} Destruct", m_id);
    }
  };

  class C
  {
    int m_id{0};

    A m_a;
    B m_b;

  public:
    C()
    {
      FSLLOG3_INFO("C{} Construct", m_id);
    }

    explicit C(const int id)
      : m_id(id)
      , m_a(id)
      , m_b(id)
    {
      FSLLOG3_INFO("C{} Construct", m_id);
    }

    C(const C& other)
      : m_id(other.m_id)
      , m_a(other.m_a)
      , m_b(other.m_b)
    {
      FSLLOG3_INFO("C{} Copy construct done", m_id);
    }

    C(C&& other) noexcept
      : m_id(other.m_id)
      , m_a(std::move(other.m_a))
      , m_b(std::move(other.m_b))
    {
      other.m_id = 0;
      FSLLOG3_INFO("C{} Move construct done", m_id);
    }

    // C& operator=(C&& other) noexcept = default;
    C& operator=(C&& other) noexcept
    {
      if (this != &other)
      {
        // Implemented move in same order as a default implementation (does not work with RAII objects with multiple entries as it differ from
        // destruction order)
        m_id = other.m_id;
        m_a = std::move(other.m_a);
        m_b = std::move(other.m_b);
        other.m_id = 0;
      }
      return *this;
    }


    ~C()
    {
      FSLLOG3_INFO("C{} Destruct", m_id);
    }
  };

}


TEST(TestControlled, Construct)
{
  FSLLOG3_INFO("Normal construct/destruct");
  {
    C object(10);
  }
  FSLLOG3_INFO("Placement construct/destruct");
  {
    Controlled<C> object(10);
  }
}


TEST(TestControlled, MoveConstruct)
{
  FSLLOG3_INFO("Normal move operator");
  {
    C val(10);
    C object(std::move(val));
  }
  FSLLOG3_INFO("Placement move construct/destruct");
  {
    C val(10);
    Controlled<C> object(std::move(val));
  }
}

TEST(TestControlled, MoveAssign)
{
  FSLLOG3_INFO("Normal move assign operator");
  {
    C object(10);
    C val(20);
    object = std::move(val);
  }
  FSLLOG3_INFO("Placement move assign construct/destruct");
  {
    Controlled<C> object(10);
    C val(20);
    object = std::move(val);
  }
}
