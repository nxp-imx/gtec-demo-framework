#ifndef FSLBASE_RTTI_TYPEINFO_HPP
#define FSLBASE_RTTI_TYPEINFO_HPP

#include <typeinfo>

namespace Fsl
{
  // A safe typeinfo class that can be stored and compared.
  // Based on Modern C++ Design: Generic Programming and Design Patterns Applied
  // In C++11 std::type_index can be used instead
  class TypeInfo
  {
  public:
    // Constructors
    TypeInfo(); // needed for containers
    TypeInfo(const std::type_info&); // non-explicit

    // Access for the wrapped std::type_info
    const std::type_info& Get() const;

    // Compatibility functions
    bool before(const TypeInfo& rhs) const;

    const char* name() const;

  private:
    const std::type_info* m_pInfo;
  };

  // Comparison operators
  inline bool operator==(const TypeInfo& lhs, const TypeInfo& rhs)
    // type_info::operator== return type is int in some VC libraries
  {
    return (lhs.Get() == rhs.Get()) != 0;
  }

  inline bool operator<(const TypeInfo& lhs, const TypeInfo& rhs)
  {
    return lhs.before(rhs);
  }

  inline bool operator!=(const TypeInfo& lhs, const TypeInfo& rhs)
  {
    return !(lhs == rhs);
  }

  inline bool operator>(const TypeInfo& lhs, const TypeInfo& rhs)
  {
    return rhs < lhs;
  }

  inline bool operator<=(const TypeInfo& lhs, const TypeInfo& rhs)
  {
    return !(lhs > rhs);
  }

  inline bool operator>=(const TypeInfo& lhs, const TypeInfo& rhs)
  {
    return !(lhs < rhs);
  }
}



#endif
