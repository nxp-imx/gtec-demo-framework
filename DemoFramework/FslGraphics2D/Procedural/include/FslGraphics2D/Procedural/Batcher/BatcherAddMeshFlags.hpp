#ifndef FSLGRAPHICS2D_PROCEDURAL_BATCHER_BATCHERADDMESHFLAGS_HPP
#define FSLGRAPHICS2D_PROCEDURAL_BATCHER_BATCHERADDMESHFLAGS_HPP
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

#include <FslBase/BasicTypes.hpp>

namespace Fsl
{
  enum class BatcherAddMeshFlags : uint32_t
  {
    // Can not use 'None' because of the x11 header, so NoFlags it is
    NoFlags = 0,

    //! Set this flag to ensure that the zpos is not increased by the add (good for cases where you need to render both opaque and transparent parts)
    DontIncreaseZPos = 0x01,
  };

  constexpr inline BatcherAddMeshFlags operator|(const BatcherAddMeshFlags lhs, const BatcherAddMeshFlags rhs)
  {
    return static_cast<BatcherAddMeshFlags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
  }

  constexpr inline BatcherAddMeshFlags operator&(const BatcherAddMeshFlags lhs, const BatcherAddMeshFlags rhs)
  {
    return static_cast<BatcherAddMeshFlags>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
  }


  namespace BatcherAddMeshFlagsUtil
  {
    inline bool IsEnabled(const BatcherAddMeshFlags srcFlag, BatcherAddMeshFlags flag)
    {
      return (srcFlag & flag) == flag;
    }

    inline void Enable(BatcherAddMeshFlags& rDstFlag, BatcherAddMeshFlags flag)
    {
      rDstFlag = rDstFlag | flag;
    }


    inline void Disable(BatcherAddMeshFlags& rDstFlag, BatcherAddMeshFlags flag)
    {
      rDstFlag = rDstFlag & (BatcherAddMeshFlags(~static_cast<uint32_t>(flag)));
    }

    inline void Set(BatcherAddMeshFlags& rDstFlag, BatcherAddMeshFlags flag, const bool enabled)
    {
      rDstFlag = enabled ? (rDstFlag | flag) : (rDstFlag & (BatcherAddMeshFlags(~static_cast<uint32_t>(flag))));
    }
  };
}

#endif
