#include <FslBase/RTTI/TypeInfo.hpp>
#include <cassert>

namespace Fsl
{
  TypeInfo::TypeInfo()
  {
    class Nil
    {
    };
    m_pInfo = &typeid(Nil);
    assert(m_pInfo);
  }


  TypeInfo::TypeInfo(const std::type_info& ti)
    : m_pInfo(&ti)
  {
    assert(m_pInfo);
  }


  bool TypeInfo::before(const TypeInfo& rhs) const
  {
    assert(m_pInfo);
    // type_info::before return type is int in some VC libraries
    return static_cast<int>(m_pInfo->before(*rhs.m_pInfo)) != 0;
  }


  const std::type_info& TypeInfo::Get() const
  {
    assert(m_pInfo);
    return *m_pInfo;
  }


  const char* TypeInfo::name() const
  {
    assert(m_pInfo);
    return m_pInfo->name();
  }
}
