/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslBase/Transition/TransitionCache.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/EqualHelper.hpp>
#include <cassert>

namespace Fsl
{
  TransitionCache::TransitionCache()
    : m_empty(std::make_shared<std::vector<float>>())
    , m_zeroTransition(std::make_shared<std::vector<float>>(1))

  {
    (*m_zeroTransition)[0] = 1.0f;

    // initialize the maps with base values
    m_lookupLinearBasedTables[0] = m_empty;
    m_lookupLinearBasedTables[1] = m_zeroTransition;
    m_lookupCosBasedTables[0] = m_empty;
    m_lookupCosBasedTables[1] = m_zeroTransition;
  }


  std::shared_ptr<std::vector<float>> TransitionCache::GetLookupTable(const int32_t length, const TransitionType type)
  {
    if (length > 1)
    {
      switch (type)
      {
      case TransitionType::Linear:
        return GetLinearLookupTable(length);
        break;
      case TransitionType::Smooth:
      default:
        return GetCosBasedLookupTable(length);
      }
    }
    return m_zeroTransition;
  }


  //------------------------------------------------------------------------------------------------------------------------------------------------

  /// <summary>
  /// Get a table that moves from 0 to 1 over the specified length linearly
  /// </summary>
  /// <param name="length"></param>
  /// <returns></returns>
  std::shared_ptr<std::vector<float>> TransitionCache::GetLinearLookupTable(const int32_t length)
  {
    assert(length >= 0);
    const auto itr = m_lookupLinearBasedTables.find(length);
    if (itr != m_lookupLinearBasedTables.end())
    {
      return itr->second;
    }

    auto table = std::make_shared<std::vector<float>>(length);

    // fill the table
    float res = 0;
    float add = 1.0f / (length - 1);
    auto& rTable = *table;
    for (int32_t i = 0; i < length; ++i)
    {
      rTable[i] = res;
      res += add;
    }
    rTable[length - 1] = 1.0f;
    m_lookupLinearBasedTables[length] = table;

    // Validate that the lookup table starts at zero and ends at one to ensure that
    // we get proper transitions that start at source position and ends at the dest position.
    assert(rTable.size() == 1 || EqualHelper::IsAlmostEqual(rTable.front(), 0));
    assert(EqualHelper::IsAlmostEqual(rTable.back(), 1));
    return table;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------------

  /// <summary>
  /// </summary>
  /// <param name="length"></param>
  /// <returns></returns>
  std::shared_ptr<std::vector<float>> TransitionCache::GetCosBasedLookupTable(const int32_t length)
  {
    assert(length >= 0);
    const auto itr = m_lookupCosBasedTables.find(length);
    if (itr != m_lookupCosBasedTables.end())
    {
      return itr->second;
    }

    auto table = std::make_shared<std::vector<float>>(length);

    // fill the table
    float rads = 0;
    float radAdd = (MathHelper::TO_RADS * 180) / (length - 1);
    auto& rTable = *table;
    for (int32_t i = 0; i < length; ++i)
    {
      rTable[i] = static_cast<float>(((-std::cos(rads)) + 1) / 2);
      rads += radAdd;
    }

    m_lookupCosBasedTables[length] = table;

    // Validate that the lookup table starts at zero and ends at one to ensure that
    // we get proper transitions that start at source position and ends at the dest position.
    assert(rTable.size() == 1 || EqualHelper::IsAlmostEqual(rTable.front(), 0));
    assert(EqualHelper::IsAlmostEqual(rTable.back(), 1));
    return table;
  }
}
