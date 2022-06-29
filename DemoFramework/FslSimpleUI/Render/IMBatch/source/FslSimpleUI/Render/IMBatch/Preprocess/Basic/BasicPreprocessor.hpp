#ifndef FSLSIMPLEUI_RENDER_IMBATCH_PREPROCESS_BASIC_BASICPREPROCESSOR_HPP
#define FSLSIMPLEUI_RENDER_IMBATCH_PREPROCESS_BASIC_BASICPREPROCESSOR_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/Span.hpp>
#include <FslBase/Span/SpanUtil.hpp>
#include <FslSimpleUI/Render/Base/Command/EncodedCommand.hpp>
#include <FslSimpleUI/Render/IMBatch/DrawReorderMethod.hpp>
#include <vector>
#include "../../MeshManager.hpp"
#include "../PreprocessResult.hpp"
#include "../ProcessedCommandRecord.hpp"
#include "PreprocessUtil.hpp"
#include "PreprocessUtil_ForceTransparent.hpp"

namespace Fsl::UI::RenderIMBatch
{
  class BasicPreprocessor
  {
    bool m_allowDepthBuffer;
    PreprocessResult m_result;

  public:
    explicit BasicPreprocessor(const bool allowDepthBuffer)
      : m_allowDepthBuffer(allowDepthBuffer)
    {
    }

    inline void Process(std::vector<ProcessedCommandRecord>& rProcessedCommandRecords, ReadOnlySpan<EncodedCommand> commandSpan,
                        const MeshManager& meshManager)
    {
      if (m_allowDepthBuffer)
      {
        // When we have a depth buffer we split the opaque and transparent commands
        m_result = PreprocessUtil::Preprocess(rProcessedCommandRecords, commandSpan, meshManager);
      }
      else
      {
        // When we have no depth buffer we force process all entries into the same render queue
        // to preserve the draw order
        m_result = PreprocessUtil::PreprocessForceTransparent(rProcessedCommandRecords, commandSpan, meshManager);
      }
      assert(m_allowDepthBuffer || GetOpaqueSpan(rProcessedCommandRecords).empty());
    }

    inline Span<ProcessedCommandRecord> GetOpaqueSpan(std::vector<ProcessedCommandRecord>& rProcessedCommandRecords) const
    {
      return SpanUtil::AsSubSpan(rProcessedCommandRecords, m_result.OpaqueStartIndex, m_result.OpaqueCount);
    }

    inline Span<ProcessedCommandRecord> GetTransparentSpan(std::vector<ProcessedCommandRecord>& rProcessedCommandRecords) const
    {
      return SpanUtil::AsSubSpan(rProcessedCommandRecords, m_result.TransparentStartIndex, m_result.TransparentCount);
    }
  };
}
#endif
