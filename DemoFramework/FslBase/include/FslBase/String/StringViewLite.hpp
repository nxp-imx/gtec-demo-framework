#ifndef FSLBASE_STRING_STRINGVIEWLITE_HPP
#define FSLBASE_STRING_STRINGVIEWLITE_HPP
/****************************************************************************************************************************************************
 * Copyright 2019, 2022-2024 NXP
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

#include <FslBase/Attributes.hpp>
#include <FslBase/String/CStringPointer.hpp>
#include <FslBase/String/StringViewUtil.hpp>
#include <cassert>
#include <compare>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <string_view>

namespace Fsl
{
  // This is basically a safer version of std::string_view intended for use as method parameters to ensure we dont trigger any of the very bad
  // constructors and methods that fall over if they get a c-string that is a nullptr. As it is having one method that takes a std::string_view is
  // dangerous as any "const char*const psz" that is null supplied to it will trigger undefined behavior and most likely crash.
  // So this basically maps the constructors and function of a std::string_view but with code safety in mind.
  class StringViewLite
  {
  public:
    using traits_type = std::string_view::traits_type;
    using value_type = std::string_view::value_type;
    using pointer = std::string_view::pointer;
    using const_pointer = std::string_view::const_pointer;
    using reference = std::string_view::reference;
    using const_reference = std::string_view::const_reference;
    using const_iterator = std::string_view::const_iterator;
    using iterator = std::string_view::iterator;
    using const_reverse_iterator = std::string_view::const_reverse_iterator;
    using reverse_iterator = std::string_view::reverse_iterator;
    using size_type = std::string_view::size_type;
    using difference_type = std::string_view::difference_type;

    // NOLINTNEXTLINE(readability-identifier-naming)
    static constexpr const size_type npos = std::string_view::npos;

  private:
    std::string_view m_view;

  public:
    constexpr StringViewLite() noexcept = default;
    constexpr StringViewLite(const StringViewLite& other) noexcept = default;


    explicit constexpr StringViewLite(const const_pointer pStr, size_type count)
      : m_view(StringViewUtil::Create(pStr, count))
    {
    }

    constexpr StringViewLite(const const_pointer psz) noexcept    // NOLINT(google-explicit-constructor)
      : m_view(StringViewUtil::Create(psz))
    {
    }

    constexpr StringViewLite(const std::string_view strView) noexcept    // NOLINT(google-explicit-constructor)
      : m_view(strView)
    {
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr const_pointer data() const noexcept
    {
      return m_view.data();
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type size() const noexcept
    {
      return m_view.size();
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type max_size() const noexcept
    {
      return m_view.max_size();
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    [[nodiscard]] constexpr bool empty() const noexcept
    {
      return m_view.empty();
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    constexpr const_reference operator[](size_type pos) const
    {
      assert(pos < size());
      return m_view[pos];
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr const_reference at(size_type pos) const
    {
      return m_view.at(pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type length() const noexcept
    {
      return m_view.length();
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr const_reference back() const
    {
      assert(!empty());
      return m_view.back();
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr const_reference front() const
    {
      assert(!empty());
      return m_view.front();
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr void remove_prefix(size_type n)
    {
      assert(n <= m_view.length());
      m_view.remove_prefix(n);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr void remove_suffix(size_type n)
    {
      assert(n <= m_view.length());
      m_view.remove_suffix(n);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr void swap(std::string_view& v) noexcept
    {
      m_view.swap(v);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr void swap(StringViewLite& v) noexcept
    {
      m_view.swap(v.m_view);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    //! @brief Returns a view of the substring [pos, pos + rcount), where rcount is the smaller of count and size() - pos.
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr StringViewLite substr(size_type pos = 0, size_type count = npos) const
    {
      return {m_view.substr(pos, count)};
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // constexpr size_type copy(CharT* dest, size_type count, size_type pos = 0) const
    //{
    // }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr int compare(std::string_view value) const noexcept
    {
      return m_view.compare(value);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr int compare(StringViewLite value) const noexcept
    {
      return m_view.compare(value.m_view);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    int compare(size_type pos, size_type count, StringViewLite v) const
    {
      return m_view.compare(pos, count, v.m_view);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr int compare(size_type pos1, size_type count1, std::string_view v) const
    {
      return m_view.compare(pos1, count1, v);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr int compare(size_type pos1, size_type count1, std::string_view v, size_type pos2, size_type count2) const
    {
      return m_view.compare(pos1, count1, v, pos2, count2);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr int compare(size_type pos1, size_type count1, StringViewLite v, size_type pos2, size_type count2) const
    {
      return m_view.compare(pos1, count1, v.m_view, pos2, count2);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr int compare(const value_type* const psz) const noexcept
    {
      return compare(CStringPointer(psz));
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr int compare(size_type pos1, size_type count1, const char* const psz) const
    {
      return compare(pos1, count1, CStringPointer(psz));
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr int compare(size_type pos1, size_type count1, const char* const psz, size_type count2) const
    {
      return compare(pos1, count1, CStringPointer(psz), 0, count2);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr bool starts_with(StringViewLite value) const noexcept
    {
      return m_view.starts_with(value.m_view);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    constexpr bool StartsWith(std::string_view value) const noexcept
    {
      return m_view.starts_with(value);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr bool starts_with(char ch) const noexcept
    {
      return m_view.starts_with(ch);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr bool starts_with(const char* const psz) const noexcept
    {
      return StartsWith(CStringPointer(psz));
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr bool ends_with(StringViewLite value) const noexcept
    {
      return m_view.ends_with(value);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr bool ends_with(std::string_view value) const noexcept
    {
      return m_view.ends_with(value);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr bool ends_with(char ch) const noexcept
    {
      return m_view.ends_with(ch);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr bool ends_with(const char* const psz) const noexcept
    {
      return ends_with(CStringPointer(psz));
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type find(StringViewLite v, size_type pos = 0) const noexcept
    {
      return m_view.find(v.m_view, pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type find(std::string_view v, size_type pos = 0) const noexcept
    {
      return m_view.find(v, pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type find(const value_type ch, size_type pos = 0) const noexcept
    {
      return m_view.find(ch, pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type find(const char* psz, size_type pos = 0) const noexcept
    {
      return find(CStringPointer(psz), pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type rfind(StringViewLite v, size_type pos = npos) const noexcept
    {
      return m_view.rfind(v.m_view, pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type rfind(std::string_view v, size_type pos = npos) const noexcept
    {
      return m_view.rfind(v, pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type rfind(value_type ch, size_type pos = npos) const noexcept
    {
      return m_view.rfind(ch, pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type rfind(const char* psz, size_type pos = npos) const noexcept
    {
      return rfind(CStringPointer(psz), pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type find_first_of(StringViewLite v, size_type pos = 0) const noexcept
    {
      return m_view.find_first_not_of(v.m_view, pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type find_first_of(std::string_view v, size_type pos = 0) const noexcept
    {
      return m_view.find_first_not_of(v, pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type find_first_of(value_type ch, size_type pos = 0) const noexcept
    {
      return m_view.find_first_not_of(ch, pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type find_first_of(const char* const psz, size_type pos = 0) const noexcept
    {
      return find_first_of(CStringPointer(psz), pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type find_last_of(StringViewLite v, size_type pos = npos) const noexcept
    {
      return m_view.find_last_of(v.m_view, pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type find_last_of(std::string_view v, size_type pos = npos) const noexcept
    {
      return m_view.find_last_of(v, pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type find_last_of(value_type ch, size_type pos = npos) const noexcept
    {
      return m_view.find_last_of(ch, pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type find_last_of(const char* const psz, size_type pos = npos) const noexcept
    {
      return find_last_of(CStringPointer(psz), pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type find_first_not_of(StringViewLite v, size_type pos = 0) const noexcept
    {
      return m_view.find_first_not_of(v.m_view, pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type find_first_not_of(std::string_view v, size_type pos = 0) const noexcept
    {
      return m_view.find_first_not_of(v, pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type find_first_not_of(value_type ch, size_type pos = 0) const noexcept
    {
      return m_view.find_first_not_of(ch, pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type find_first_not_of(const char* const psz, size_type pos = 0) const noexcept
    {
      return find_first_not_of(CStringPointer(psz), pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type find_last_not_of(StringViewLite v, size_type pos = npos) const noexcept
    {
      return m_view.find_last_not_of(v.m_view, pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type find_last_not_of(std::string_view v, size_type pos = npos) const noexcept
    {
      return m_view.find_last_not_of(v, pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type find_last_not_of(value_type ch, size_type pos = npos) const noexcept
    {
      return m_view.find_last_not_of(ch, pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr size_type find_last_not_of(const char* const psz, size_type pos = npos) const noexcept
    {
      return find_last_not_of(CStringPointer(psz), pos);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr const_iterator begin() const noexcept
    {
      return m_view.begin();
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr const_iterator end() const noexcept
    {
      return m_view.end();
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr const_iterator cbegin() const noexcept
    {
      return m_view.cbegin();
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr const_iterator cend() const noexcept
    {
      return m_view.cend();
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr const_reverse_iterator rbegin() const noexcept
    {
      return m_view.rbegin();
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr const_reverse_iterator rend() const noexcept
    {
      return m_view.rend();
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr const_reverse_iterator crbegin() const noexcept
    {
      return rbegin();
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr const_reverse_iterator crend() const noexcept
    {
      return rend();
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr std::string_view AsStringView() const noexcept
    {
      return m_view;
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr operator std::string_view() const noexcept
    {
      return m_view;
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    constexpr auto operator<=>(const StringViewLite& other) const noexcept = default;
    constexpr bool operator==(const StringViewLite& other) const noexcept = default;

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr StringViewLite UncheckedCreate(const char* const psz) noexcept
    {
      return {StringViewUtil::UncheckedCreate(psz)};
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr StringViewLite UncheckedCreate(const char* const pStr, const std::size_t count) noexcept
    {
      return {StringViewUtil::UncheckedCreate(pStr, count)};
    }
  };


  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // const char*const psz
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr auto operator<=>(const StringViewLite lhs, const char* const psz) noexcept
  {
    return lhs.AsStringView() <=> CStringPointer(psz);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr bool operator==(const StringViewLite lhs, const char* const psz) noexcept
  {
    return lhs.AsStringView() == CStringPointer(psz);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // std::string_view
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr auto operator<=>(const StringViewLite lhs, std::string_view rhs) noexcept
  {
    return lhs.AsStringView() <=> rhs;
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr bool operator==(const StringViewLite lhs, std::string_view rhs) noexcept
  {
    return lhs.AsStringView() == rhs;
  }
}

// namespace std
// {
//   template <>
//   struct hash<Fsl::StringViewLite>
//   {
//     inline std::size_t operator()(const Fsl::StringViewLite value) const
//     {
//       return hash<std::string_view>()(value.AsStringView());
//     }
//   };
// }

#endif
