#ifndef SHARED_UI_BENCHMARK_PERSISTENCE_JSON_TEMPJSONMACROS_HPP
#define SHARED_UI_BENCHMARK_PERSISTENCE_JSON_TEMPJSONMACROS_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <nlohmann/json.hpp>

// Workaround macros until we get the official NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT from https://github.com/nlohmann/json/pull/2819

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TEMP_NLOHMANN_JSON_FROM_WITH_DEFAULTS(v1) \
  {                                               \
    const auto it = nlohmann_json_j.find(#v1);    \
    if (it != nlohmann_json_j.end())              \
    {                                             \
      it->get_to(nlohmann_json_t.v1);             \
    }                                             \
  }

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TEMP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Type, ...)                           \
  inline void to_json(nlohmann::json& nlohmann_json_j, const Type& nlohmann_json_t)               \
  {                                                                                               \
    NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__))                      \
  }                                                                                               \
  inline void from_json(const nlohmann::json& nlohmann_json_j, Type& nlohmann_json_t)             \
  {                                                                                               \
    NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(TEMP_NLOHMANN_JSON_FROM_WITH_DEFAULTS, __VA_ARGS__)) \
  }

#endif
